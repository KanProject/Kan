#define _CRT_SECURE_NO_WARNINGS __CUSHION_PRESERVE__

#include <string.h>

#include <qsort.h>

#include <kan/container/hash_storage.h>
#include <kan/file_system/stream.h>
#include <kan/image/image.h>
#include <kan/inline_math/inline_math.h>
#include <kan/log/logging.h>
#include <kan/memory/allocation.h>
#include <kan/resource_pipeline/meta.h>
#include <kan/resource_render_foundation_build/atlas.h>
#include <kan/stream/random_access_stream_buffer.h>

KAN_LOG_DEFINE_CATEGORY (resource_render_foundation_atlas);

KAN_REFLECTION_STRUCT_FIELD_META (kan_resource_atlas_image_header_t, source)
RESOURCE_RENDER_FOUNDATION_BUILD_API struct kan_resource_reference_meta_t
    kan_resource_atlas_image_header_reference_source = {
        .type_name = NULL,
        .flags = 0u,
};

KAN_REFLECTION_STRUCT_FIELD_META (kan_resource_atlas_entry_replacement_header_t, for_locale)
RESOURCE_RENDER_FOUNDATION_BUILD_API struct kan_resource_reference_meta_t
    kan_resource_atlas_entry_replacement_header_reference_for_locale = {
        .type_name = "kan_resource_locale_t",
        .flags = 0u,
};

KAN_REFLECTION_STRUCT_META (kan_resource_atlas_header_t)
RESOURCE_RENDER_FOUNDATION_BUILD_API struct kan_resource_type_meta_t kan_resource_atlas_header_resource_type = {
    .flags = 0u,
    .version = CUSHION_START_NS_X64,
    .move = NULL,
    .reset = NULL,
};

void kan_resource_atlas_entry_header_init (struct kan_resource_atlas_entry_header_t *instance)
{
    instance->name = NULL;
    instance->image.source = NULL;
    instance->image.type = KAN_RESOURCE_ATLAS_IMAGE_TYPE_REGULAR;
    instance->image.color_table_multiplicator_index = -1;

    kan_dynamic_array_init (&instance->replacements, 0u, sizeof (struct kan_resource_atlas_entry_replacement_header_t),
                            alignof (struct kan_resource_atlas_entry_replacement_header_t),
                            kan_allocation_group_stack_get ());
}

void kan_resource_atlas_entry_header_shutdown (struct kan_resource_atlas_entry_header_t *instance)
{
    kan_dynamic_array_shutdown (&instance->replacements);
}

void kan_resource_atlas_header_init (struct kan_resource_atlas_header_t *instance)
{
    instance->page_width = 2048u;
    instance->page_height = 2048u;

    instance->border_size = 1u;
    instance->border_linear_r = 0.0f;
    instance->border_linear_g = 0.0f;
    instance->border_linear_b = 0.0f;
    instance->border_linear_a = 0.0f;

    kan_dynamic_array_init (&instance->entries, 0u, sizeof (struct kan_resource_atlas_entry_header_t),
                            alignof (struct kan_resource_atlas_entry_header_t), kan_allocation_group_stack_get ());
}

void kan_resource_atlas_header_shutdown (struct kan_resource_atlas_header_t *instance)
{
    KAN_DYNAMIC_ARRAY_SHUTDOWN_WITH_ITEMS_AUTO (instance->entries, kan_resource_atlas_entry_header)
}

static enum kan_resource_build_rule_result_t atlas_build (struct kan_resource_build_rule_context_t *context);

KAN_REFLECTION_STRUCT_META (kan_resource_atlas_t)
RESOURCE_RENDER_FOUNDATION_BUILD_API struct kan_resource_build_rule_t kan_resource_atlas_build_rule = {
    .primary_input_type = "kan_resource_atlas_header_t",
    .platform_configuration_type = NULL,
    .secondary_types_count = 0u,
    .secondary_types = NULL,
    .functor = atlas_build,
    .version = CUSHION_START_NS_X64,
};

KAN_REFLECTION_IGNORE
struct atlas_build_context_t
{
    kan_allocation_group_t main_allocation_group;
    struct kan_hash_storage_t images;
    struct kan_dynamic_array_t sorted_images;
    struct kan_bd_list_t allocation_pins;
    kan_instance_size_t pages_needed;
};

KAN_REFLECTION_IGNORE
struct atlas_image_node_t
{
    struct kan_hash_storage_node_t node;
    kan_interned_string_t name;

    /// \brief We load all raw images at once. If they need to fit inside atlas, it should be okay to load them all.
    struct kan_image_raw_data_t raw_data;

    kan_instance_size_t allocated_page;
    kan_instance_size_t allocated_x;
    kan_instance_size_t allocated_y;
};

KAN_REFLECTION_IGNORE
struct atlas_allocation_pin_t
{
    struct kan_bd_list_node_t node;
    kan_instance_size_t page;
    kan_instance_size_t x;
    kan_instance_size_t y;
    kan_instance_size_t width;
    kan_instance_size_t height;
};

static void atlas_build_context_init (struct atlas_build_context_t *instance)
{
    instance->main_allocation_group =
        kan_allocation_group_get_child (kan_allocation_group_root (), "resource_render_foundation_atlas_build");
    kan_hash_storage_init (&instance->images, instance->main_allocation_group, KAN_RESOURCE_RF_ATLAS_IMAGE_BUCKETS);

    kan_dynamic_array_init (&instance->sorted_images, 0u, sizeof (struct atlas_image_node_t *),
                            alignof (struct atlas_image_node_t *), instance->main_allocation_group);

    kan_bd_list_init (&instance->allocation_pins);
    instance->pages_needed = 1u;
}

static void atlas_build_context_shutdown (struct atlas_build_context_t *instance)
{
    struct atlas_image_node_t *image_node = (struct atlas_image_node_t *) instance->images.items.first;
    while (image_node)
    {
        struct atlas_image_node_t *next = (struct atlas_image_node_t *) image_node->node.list_node.next;
        kan_image_raw_data_shutdown (&image_node->raw_data);
        kan_free_batched (instance->main_allocation_group, image_node);
        image_node = next;
    }

    kan_hash_storage_shutdown (&instance->images);
    kan_dynamic_array_shutdown (&instance->sorted_images);
    struct atlas_allocation_pin_t *pin = (struct atlas_allocation_pin_t *) instance->allocation_pins.first;

    while (pin)
    {
        struct atlas_allocation_pin_t *next = (struct atlas_allocation_pin_t *) pin->node.next;
        kan_free_batched (instance->main_allocation_group, pin);
        pin = next;
    }
}

static bool atlas_image_sort_comparator (struct atlas_image_node_t *left, struct atlas_image_node_t *right)
{
    // Use > as we need bigger -> smaller order.
    if (left->raw_data.height != right->raw_data.height)
    {
        return left->raw_data.height > right->raw_data.height;
    }

    return left->raw_data.width > right->raw_data.width;
}

static inline struct atlas_image_node_t *atlas_build_context_find_image (struct atlas_build_context_t *context,
                                                                         kan_interned_string_t image_name)
{
    const struct kan_hash_storage_bucket_t *bucket = kan_hash_storage_query (&context->images, (kan_hash_t) image_name);
    struct atlas_image_node_t *node = (struct atlas_image_node_t *) bucket->first;
    const struct atlas_image_node_t *node_end =
        (struct atlas_image_node_t *) (bucket->last ? bucket->last->next : NULL);

    while (node != node_end)
    {
        if (node->name == image_name)
        {
            return node;
        }

        node = (struct atlas_image_node_t *) node->node.list_node.next;
    }

    return NULL;
}

static enum kan_resource_build_rule_result_t atlas_build (struct kan_resource_build_rule_context_t *context)
{
    const struct kan_resource_atlas_header_t *input = context->primary_input;
    struct kan_resource_atlas_t *output = context->primary_output;

    output->page_width = input->page_width;
    output->page_height = input->page_height;

    struct atlas_build_context_t build_context;
    atlas_build_context_init (&build_context);
    CUSHION_DEFER { atlas_build_context_shutdown (&build_context); }

    // We can rely on the fact that resource pipeline has already collected all unique referenced images.
    struct kan_resource_build_rule_secondary_node_t *secondary = context->secondary_input_first;

    while (secondary)
    {
        if (!secondary->type)
        {
            struct kan_stream_t *image_load_stream =
                kan_direct_file_stream_open_for_read (secondary->third_party_path, true);

            if (!image_load_stream)
            {
                KAN_LOG (resource_render_foundation_atlas, KAN_LOG_ERROR,
                         "Failed to open image at \"%s\" for atlas header \"%s\".", context->primary_third_party_path,
                         context->primary_name)
                return KAN_RESOURCE_BUILD_RULE_FAILURE;
            }

            // Should be fine to use the same buffer size as for textures.
            image_load_stream =
                kan_random_access_stream_buffer_open_for_read (image_load_stream, KAN_RESOURCE_RF_TEXTURE_LOAD_BUFFER);
            CUSHION_DEFER { image_load_stream->operations->close (image_load_stream); }

            struct kan_image_raw_data_t image_data;
            kan_image_raw_data_init (&image_data);

            if (!kan_image_load (image_load_stream, &image_data))
            {
                KAN_LOG (resource_render_foundation_atlas, KAN_LOG_ERROR,
                         "Failed to load image at \"%s\" for atlas header \"%s\".", context->primary_third_party_path,
                         context->primary_name)

                kan_image_raw_data_shutdown (&image_data);
                return KAN_RESOURCE_BUILD_RULE_FAILURE;
            }

            if (image_data.width > input->page_width - input->border_size * 2u)
            {
                KAN_LOG (resource_render_foundation_atlas, KAN_LOG_ERROR,
                         "Failed to add image at \"%s\" to atlas \"%s\": its width %u is too big for atlas page width "
                         "%u (border %u).",
                         context->primary_third_party_path, context->primary_name, (unsigned int) image_data.width,
                         (unsigned int) image_data.height, (unsigned int) input->border_size)

                kan_image_raw_data_shutdown (&image_data);
                return KAN_RESOURCE_BUILD_RULE_FAILURE;
            }

            if (image_data.height > input->page_height - input->border_size * 2u)
            {
                KAN_LOG (resource_render_foundation_atlas, KAN_LOG_ERROR,
                         "Failed to add image at \"%s\" to atlas \"%s\": its height %u is too big for atlas page "
                         "height %u (border %u).",
                         context->primary_third_party_path, context->primary_name, (unsigned int) image_data.width,
                         (unsigned int) image_data.height, (unsigned int) input->border_size)

                kan_image_raw_data_shutdown (&image_data);
                return KAN_RESOURCE_BUILD_RULE_FAILURE;
            }

            struct atlas_image_node_t *new_node =
                kan_allocate_batched (build_context.main_allocation_group, sizeof (struct atlas_image_node_t));

            new_node->node.hash = (kan_hash_t) secondary->name;
            new_node->name = secondary->name;
            new_node->raw_data = image_data;

            new_node->allocated_page = 0u;
            new_node->allocated_x = 0u;
            new_node->allocated_y = 0u;

            kan_hash_storage_update_bucket_count_default (&build_context.images, KAN_RESOURCE_RF_ATLAS_IMAGE_BUCKETS);
            kan_hash_storage_add (&build_context.images, &new_node->node);
        }

        secondary = secondary->next;
    }

    kan_dynamic_array_set_capacity (&build_context.sorted_images, build_context.images.items.size);
    {
        struct atlas_image_node_t *image_node = (struct atlas_image_node_t *) build_context.images.items.first;
        while (image_node)
        {
            void *spot = kan_dynamic_array_add_last (&build_context.sorted_images);
            *(struct atlas_image_node_t **) spot = image_node;
            image_node = (struct atlas_image_node_t *) image_node->node.list_node.next;
        }

        struct atlas_image_node_t *temporary;
#define AT_INDEX(INDEX) (((struct atlas_image_node_t **) build_context.sorted_images.data)[INDEX])
#define LESS(first_index, second_index)                                                                                \
    __CUSHION_PRESERVE__ atlas_image_sort_comparator (AT_INDEX (first_index), AT_INDEX (second_index))
#define SWAP(first_index, second_index)                                                                                \
    __CUSHION_PRESERVE__                                                                                               \
    temporary = AT_INDEX (first_index), AT_INDEX (first_index) = AT_INDEX (second_index),                              \
    AT_INDEX (second_index) = temporary

        QSORT (build_context.sorted_images.size, LESS, SWAP);
#undef LESS
#undef SWAP
#undef AT_INDEX
    }

    // Use ladder-like packing algorithm to allocate coordinates for images.
    {
        build_context.pages_needed = 1u;
        struct atlas_allocation_pin_t *first_pin =
            kan_allocate_batched (build_context.main_allocation_group, sizeof (struct atlas_allocation_pin_t));

        first_pin->page = 0u;
        first_pin->x = input->border_size;
        first_pin->y = input->border_size;
        first_pin->width = input->page_width - input->border_size * 2u;
        first_pin->height = input->page_height - input->border_size * 2u;
        kan_bd_list_add (&build_context.allocation_pins, NULL, &first_pin->node);

        struct atlas_image_node_t *image_node = (struct atlas_image_node_t *) build_context.images.items.first;
        while (image_node)
        {
            struct atlas_allocation_pin_t *suitable_pin =
                (struct atlas_allocation_pin_t *) build_context.allocation_pins.first;

            while (suitable_pin)
            {
                if (suitable_pin->width <= image_node->raw_data.width &&
                    suitable_pin->height <= image_node->raw_data.height)
                {
                    // Can allocate from here.
                    break;
                }

                suitable_pin = (struct atlas_allocation_pin_t *) suitable_pin->node.next;
            }

            if (!suitable_pin)
            {
                // No space on existing pages, allocate new page.
                suitable_pin =
                    kan_allocate_batched (build_context.main_allocation_group, sizeof (struct atlas_allocation_pin_t));

                suitable_pin->page = build_context.pages_needed;
                ++build_context.pages_needed;

                suitable_pin->x = input->border_size;
                suitable_pin->y = input->border_size;
                suitable_pin->width = input->page_width - input->border_size * 2u;
                suitable_pin->height = input->page_height - input->border_size * 2u;
                kan_bd_list_add (&build_context.allocation_pins, NULL, &suitable_pin->node);
            }

            image_node->allocated_page = suitable_pin->page;
            image_node->allocated_x = suitable_pin->x;
            image_node->allocated_y = suitable_pin->y;

            if (suitable_pin->height - image_node->raw_data.height > input->border_size)
            {
                // Can allocate under this allocation in the future. Create new pin for that.
                struct atlas_allocation_pin_t *bottom_pin =
                    kan_allocate_batched (build_context.main_allocation_group, sizeof (struct atlas_allocation_pin_t));

                bottom_pin->page = suitable_pin->page;
                bottom_pin->x = suitable_pin->x;
                bottom_pin->y = suitable_pin->y + image_node->raw_data.height + input->border_size;
                bottom_pin->width = suitable_pin->width;
                bottom_pin->height = suitable_pin->height - image_node->raw_data.height - input->border_size;
                kan_bd_list_add (&build_context.allocation_pins, suitable_pin->node.next, &bottom_pin->node);
            }

            suitable_pin->x += image_node->raw_data.width;
            suitable_pin->width -= image_node->raw_data.width;

            // Further allocations along this pin cannot be bigger that this allocation.
            // It should never happen due to sorting, but we still set height to keep proper values in data.
            suitable_pin->height = image_node->raw_data.height;

            if (suitable_pin->width > input->border_size)
            {
                // Add border between allocations.
                suitable_pin->x += input->border_size;
                suitable_pin->width -= input->border_size;
            }
            else
            {
                // Too small to do anything, just delete then.
                kan_bd_list_remove (&build_context.allocation_pins, &suitable_pin->node);
                kan_free_batched (build_context.main_allocation_group, suitable_pin);
            }

            image_node = (struct atlas_image_node_t *) image_node->node.list_node.next;
        }
    }

    // Now allocate and fill atlas data.
    {
        output->page_count = build_context.pages_needed;
        const kan_instance_size_t page_size = input->page_width * input->page_height;
        const kan_instance_size_t page_offset = page_size * 4u;
        kan_dynamic_array_set_capacity (&output->data, output->page_count * page_offset);
        output->data.size = output->data.capacity;

        const uint8_t border_r = (uint8_t) roundf (255.0f * kan_color_transfer_rgb_to_srgb (input->border_linear_r));
        const uint8_t border_g = (uint8_t) roundf (255.0f * kan_color_transfer_rgb_to_srgb (input->border_linear_g));
        const uint8_t border_b = (uint8_t) roundf (255.0f * kan_color_transfer_rgb_to_srgb (input->border_linear_b));
        const uint8_t border_a = (uint8_t) roundf (255.0f * input->border_linear_a);

        // Start by filling everything in the atlas with border color.
        for (kan_loop_size_t pixel_index = 0u; pixel_index < build_context.pages_needed * page_size; ++pixel_index)
        {
            output->data.data[pixel_index * 4u] = border_r;
            output->data.data[pixel_index * 4u + 1u] = border_g;
            output->data.data[pixel_index * 4u + 2u] = border_b;
            output->data.data[pixel_index * 4u + 3u] = border_a;
        }

        // Now write images into their allocated space.
        struct atlas_image_node_t *image_node = (struct atlas_image_node_t *) build_context.images.items.first;
        while (image_node)
        {
            const kan_instance_size_t page_base_pixel = image_node->allocated_page * page_size;
            for (kan_instance_size_t image_y = 0u; image_y < image_node->raw_data.height; ++image_y)
            {
                const kan_instance_size_t image_row_pixel = image_y * image_node->raw_data.width;
                const kan_instance_size_t atlas_row_pixel =
                    page_base_pixel + (image_node->allocated_y + image_y) * input->page_width;

                memcpy (&output->data.data[atlas_row_pixel * 4u], &image_node->raw_data.data[image_row_pixel * 4u],
                        image_node->raw_data.width * 4u);
            }

            image_node = (struct atlas_image_node_t *) image_node->node.list_node.next;
        }
    }

    kan_dynamic_array_set_capacity (&output->entries, input->entries.size);
    kan_dynamic_array_set_capacity (&output->replacements, input->entries.size);

    for (kan_loop_size_t entry_index = 0u; entry_index < input->entries.size; ++entry_index)
    {
        const struct kan_resource_atlas_entry_header_t *entry_source =
            &((struct kan_resource_atlas_entry_header_t *) input->entries.data)[entry_index];

        const struct atlas_image_node_t *image_node =
            atlas_build_context_find_image (&build_context, entry_source->image.source);
        // We must've processed all images.
        KAN_ASSERT (image_node)

        struct kan_resource_atlas_primary_entry_t *entry_target = kan_dynamic_array_add_last (&output->entries);
        KAN_ASSERT (entry_target)
        entry_target->name = entry_source->name;

#define COPY_IMAGE(TARGET, SOURCE, NODE)                                                                               \
    (TARGET).page = (NODE)->allocated_page;                                                                            \
    (TARGET).x = (NODE)->allocated_x;                                                                                  \
    (TARGET).y = (NODE)->allocated_y;                                                                                  \
    (TARGET).width = (NODE)->raw_data.width;                                                                           \
    (TARGET).height = (NODE)->raw_data.height;                                                                         \
    (TARGET).type = (SOURCE).type;                                                                                     \
                                                                                                                       \
    switch ((TARGET).type)                                                                                             \
    {                                                                                                                  \
    case KAN_RESOURCE_ATLAS_IMAGE_TYPE_REGULAR:                                                                        \
        break;                                                                                                         \
                                                                                                                       \
    case KAN_RESOURCE_ATLAS_IMAGE_TYPE_NINE_PATCH:                                                                     \
        (TARGET).nine_patch = (SOURCE).nine_patch;                                                                     \
        break;                                                                                                         \
    }                                                                                                                  \
                                                                                                                       \
    (TARGET).color_table_multiplicator_index = (SOURCE).color_table_multiplicator_index

        COPY_IMAGE (entry_target->image, entry_source->image, image_node);
        for (kan_loop_size_t replacement_index = 0u; replacement_index < entry_source->replacements.size;
             ++replacement_index)
        {
            const struct kan_resource_atlas_entry_replacement_header_t *replacement_source =
                &((struct kan_resource_atlas_entry_replacement_header_t *)
                      entry_source->replacements.data)[replacement_index];

            image_node = atlas_build_context_find_image (&build_context, replacement_source->image.source);
            // We must've processed all images.
            KAN_ASSERT (image_node)

            struct kan_resource_atlas_replacement_entry_t *replacement_target =
                kan_dynamic_array_add_last (&output->replacements);

            if (!replacement_target)
            {
                kan_dynamic_array_set_capacity (&output->replacements, output->replacements.size * 2u);
                replacement_target = kan_dynamic_array_add_last (&output->replacements);
                KAN_ASSERT (replacement_target)
            }

            replacement_target->name = entry_source->name;
            replacement_target->for_locale = replacement_source->for_locale;
            COPY_IMAGE (replacement_target->image, replacement_source->image, image_node);
        }

#undef COPY_IMAGE
    }

    kan_dynamic_array_set_capacity (&output->replacements, output->replacements.size);
    return KAN_RESOURCE_BUILD_RULE_SUCCESS;
}
