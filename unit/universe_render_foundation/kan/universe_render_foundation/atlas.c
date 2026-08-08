#define _CRT_SECURE_NO_WARNINGS __CUSHION_PRESERVE__

#include <string.h>

#include <qsort.h>

#include <kan/cpu_profiler/markup.h>
#include <kan/log/logging.h>
#include <kan/resource_render_foundation/atlas.h>
#include <kan/universe/macro.h>
#include <kan/universe_render_foundation/atlas.h>
#include <kan/universe_render_foundation/render_graph.h>
#include <kan/universe_resource_provider/provider.h>

KAN_LOG_DEFINE_CATEGORY (render_foundation_atlas);
KAN_USE_STATIC_INTERNED_IDS
KAN_USE_STATIC_CPU_SECTIONS

KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (render_foundation_atlas_management)
UNIVERSE_RENDER_FOUNDATION_API KAN_UM_MUTATOR_GROUP_META (render_foundation_atlas_management,
                                                          KAN_RENDER_FOUNDATION_ATLAS_MANAGEMENT_MUTATOR_GROUP);

struct render_foundation_atlas_management_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (render_foundation_atlas_management)
    KAN_UM_BIND_STATE (render_foundation_atlas_management, state)
};

UNIVERSE_RENDER_FOUNDATION_API KAN_UM_MUTATOR_DEPLOY (render_foundation_atlas_management)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_cpu_static_sections_ensure_initialized ();

    kan_workflow_graph_node_depend_on (workflow_node, KAN_RESOURCE_PROVIDER_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_RENDER_FOUNDATION_FRAME_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_RENDER_FOUNDATION_ATLAS_MANAGEMENT_BEGIN_CHECKPOINT);
    kan_workflow_graph_node_make_dependency_of (workflow_node, KAN_RENDER_FOUNDATION_ATLAS_MANAGEMENT_END_CHECKPOINT);
}

static inline bool is_image_compatible_with_atlas (kan_render_image_t image,
                                                   const struct kan_resource_atlas_t *resource)
{
    kan_instance_size_t width;
    kan_instance_size_t height;
    kan_instance_size_t depth;
    kan_instance_size_t layers;
    kan_render_image_get_sizes (image, &width, &height, &depth, &layers);

    return width == resource->page_width && height == resource->page_height && depth == 1u &&
           layers == resource->page_count;
}

KAN_REFLECTION_IGNORE
enum atlas_entry_gpu_flags_t
{
    ATLAS_ENTRY_GPU_FLAG_NINE_SLICE = 1u << 0u,
    ATLAS_ENTRY_GPU_FLAG_COLOR_MULTIPLIER = 1u << 1u,
    ATLAS_ENTRY_GPU_FLAG_AUTOCROP = 1u << 2u,
};

KAN_REFLECTION_IGNORE
enum atlas_entry_nine_slice_gpu_flags_t
{
    ATLAS_ENTRY_NINE_SLICE_GPU_FLAG_TILED_X = 1u << 0u,
    ATLAS_ENTRY_NINE_SLICE_GPU_FLAG_TILED_Y = 1u << 1u,
};

/// \details Must be in sync with `atlas_entry` in `atlas_entry.rpl`.
KAN_REFLECTION_IGNORE
struct atlas_entry_gpu_data_t
{
    uint32_t flags;
    uint32_t page;

    struct kan_float_vector_2_t content_offset;
    struct kan_float_vector_2_t content_size;

    struct kan_float_vector_2_t uv_min;
    struct kan_float_vector_2_t uv_max;

    /// \details Needed for nine slice.
    struct kan_float_vector_2_t pixel_size;

    uint32_t nine_slice_flags;
    float nine_slice_left;
    float nine_slice_right;
    float nine_slice_top;
    float nine_slice_bottom;

    uint32_t color_table_multiplier_index;
};

static void load_atlas (struct render_foundation_atlas_management_state_t *state,
                        const struct kan_resource_atlas_t *resource,
                        struct kan_render_atlas_loaded_t *loaded)
{
    KAN_CPU_SCOPED_STATIC_SECTION (load_atlas)
    KAN_UMI_SINGLETON_READ (render_context, kan_render_context_singleton_t)

    if (!KAN_HANDLE_IS_VALID (loaded->image) || !is_image_compatible_with_atlas (loaded->image, resource))
    {
        if (KAN_HANDLE_IS_VALID (loaded->image))
        {
            kan_render_image_destroy (loaded->image);
        }

        struct kan_render_image_description_t description = {
            .format = KAN_RENDER_IMAGE_FORMAT_RGBA32_SRGB,
            .width = resource->page_width,
            .height = resource->page_height,
            .depth = 1u,
            .layers = resource->page_count,
            .mips = 1u,

            .render_target = false,
            .supports_sampling = true,
            .always_treat_as_layered = true,
            .tracking_name = loaded->name,
        };

        loaded->image = kan_render_image_create (render_context->render_context, &description);
    }

    if (!KAN_HANDLE_IS_VALID (loaded->entry_buffer) ||
        kan_render_buffer_get_full_size (loaded->entry_buffer) !=
            sizeof (struct atlas_entry_gpu_data_t) * resource->total_entries)
    {
        if (KAN_HANDLE_IS_VALID (loaded->entry_buffer))
        {
            kan_render_buffer_destroy (loaded->entry_buffer);
        }

        // Currently, we do not fill initial data as we'd like to fill all entry gpu data into the patch address instead
        // of just using memcpy.
        loaded->entry_buffer = kan_render_buffer_create (
            render_context->render_context, KAN_RENDER_BUFFER_TYPE_STORAGE,
            sizeof (struct atlas_entry_gpu_data_t) * resource->total_entries, NULL, loaded->name);
    }

    if (!KAN_HANDLE_IS_VALID (loaded->image))
    {
        KAN_LOG (render_foundation_atlas, KAN_LOG_ERROR,
                 "Failed to finish loading of atlas \"%s\" as GPU image creation has failed.", loaded->name)
        return;
    }

    if (!KAN_HANDLE_IS_VALID (loaded->entry_buffer))
    {
        KAN_LOG (render_foundation_atlas, KAN_LOG_ERROR,
                 "Failed to finish loading of atlas \"%s\" as GPU entry buffer creation has failed.", loaded->name)
        return;
    }

    const kan_instance_size_t page_size = resource->page_width * resource->page_height * 4u;
    for (kan_instance_size_t page_index = 0u; page_index < resource->page_count; ++page_index)
    {
        kan_render_image_upload_data (loaded->image, page_index, 0u, page_size,
                                      resource->data.data + page_size * page_index);
    }

    loaded->mapping.size = 0u;
    kan_dynamic_array_set_capacity (&loaded->mapping, resource->total_entries);

    loaded->locale_requirements.size = 0u;
    kan_dynamic_array_set_capacity (&loaded->locale_requirements, resource->total_entries);

    kan_instance_size_t data_index = 0u;
    struct atlas_entry_gpu_data_t *entry_data = kan_render_buffer_patch (
        loaded->entry_buffer, 0u, sizeof (struct atlas_entry_gpu_data_t) * resource->total_entries);

    for (kan_instance_size_t entry_index = 0u; entry_index < resource->entries.size; ++entry_index, ++data_index)
    {
        const struct kan_resource_atlas_entry_t *entry =
            &((struct kan_resource_atlas_entry_t *) resource->entries.data)[entry_index];
        const kan_instance_size_t data_index_start = data_index;

#define FILL_ENTRY_GPU_DATA                                                                                            \
    {                                                                                                                  \
        uint32_t entry_flags = 0u;                                                                                     \
        switch (input->type)                                                                                           \
        {                                                                                                              \
        case KAN_RESOURCE_ATLAS_IMAGE_TYPE_REGULAR:                                                                    \
            break;                                                                                                     \
                                                                                                                       \
        case KAN_RESOURCE_ATLAS_IMAGE_TYPE_NINE_SLICE:                                                                 \
            entry_flags |= ATLAS_ENTRY_GPU_FLAG_NINE_SLICE;                                                            \
            break;                                                                                                     \
        }                                                                                                              \
                                                                                                                       \
        if (input->color_table_multiplier_index != KAN_INT_MAX (kan_instance_size_t))                                  \
        {                                                                                                              \
            entry_flags |= ATLAS_ENTRY_GPU_FLAG_COLOR_MULTIPLIER;                                                      \
        }                                                                                                              \
                                                                                                                       \
        if (input->source_width != input->width || input->source_height != input->height)                              \
        {                                                                                                              \
            entry_flags |= ATLAS_ENTRY_GPU_FLAG_AUTOCROP;                                                              \
        }                                                                                                              \
                                                                                                                       \
        output->flags = entry_flags;                                                                                   \
        output->page = (uint32_t) input->page;                                                                         \
                                                                                                                       \
        output->content_offset.x = (float) input->source_offset_x / (float) input->source_width;                       \
        output->content_offset.y = (float) input->source_offset_y / (float) input->source_height;                      \
        output->content_size.x = (float) input->width / (float) input->source_width;                                   \
        output->content_size.y = (float) input->height / (float) input->source_height;                                 \
                                                                                                                       \
        output->uv_min.x = (float) input->x / (float) resource->page_width;                                            \
        output->uv_min.y = (float) input->y / (float) resource->page_height;                                           \
        output->uv_max.x = (float) (input->x + input->width) / (float) resource->page_width;                           \
        output->uv_max.y = (float) (input->y + input->height) / (float) resource->page_height;                         \
                                                                                                                       \
        output->pixel_size.x = (float) input->width;                                                                   \
        output->pixel_size.y = (float) input->height;                                                                  \
                                                                                                                       \
        if (input->type == KAN_RESOURCE_ATLAS_IMAGE_TYPE_NINE_SLICE)                                                   \
        {                                                                                                              \
            uint32_t nine_slice_flags = 0u;                                                                            \
            if (input->nine_slice.tiled_x)                                                                             \
            {                                                                                                          \
                nine_slice_flags |= ATLAS_ENTRY_NINE_SLICE_GPU_FLAG_TILED_X;                                           \
            }                                                                                                          \
                                                                                                                       \
            if (input->nine_slice.tiled_y)                                                                             \
            {                                                                                                          \
                nine_slice_flags |= ATLAS_ENTRY_NINE_SLICE_GPU_FLAG_TILED_Y;                                           \
            }                                                                                                          \
                                                                                                                       \
            output->nine_slice_flags = nine_slice_flags;                                                               \
            output->nine_slice_left = (float) input->nine_slice.left;                                                  \
            output->nine_slice_right = (float) input->nine_slice.right;                                                \
            output->nine_slice_top = (float) input->nine_slice.top;                                                    \
            output->nine_slice_bottom = (float) input->nine_slice.bottom;                                              \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            output->nine_slice_left = 0.0f;                                                                            \
            output->nine_slice_right = 0.0f;                                                                           \
            output->nine_slice_top = 0.0f;                                                                             \
            output->nine_slice_bottom = 0.0f;                                                                          \
        }                                                                                                              \
                                                                                                                       \
        output->color_table_multiplier_index = (uint32_t) input->color_table_multiplier_index;                         \
    }

        for (kan_memory_size_t replacement_index = 0u; replacement_index < entry->replacements.size;
             ++replacement_index, ++data_index)
        {
            const struct kan_resource_atlas_entry_replacement_t *replacement =
                &((struct kan_resource_atlas_entry_replacement_t *) entry->replacements.data)[replacement_index];

            *(kan_interned_string_t *) kan_dynamic_array_add_last (&loaded->locale_requirements) =
                replacement->for_locale;

            const struct kan_resource_atlas_image_t *input = &replacement->image;
            struct atlas_entry_gpu_data_t *output = &entry_data[data_index];
            FILL_ENTRY_GPU_DATA
        }

        *(kan_interned_string_t *) kan_dynamic_array_add_last (&loaded->locale_requirements) = NULL;
        const struct kan_resource_atlas_image_t *input = &entry->image;
        struct atlas_entry_gpu_data_t *output = &entry_data[data_index];
        FILL_ENTRY_GPU_DATA

        struct kan_render_atlas_loaded_entry_mapping_t *mapping = kan_dynamic_array_add_last (&loaded->mapping);
        mapping->entry_name = entry->name;
        mapping->match_start = data_index_start;
#undef FILL_ENTRY_GPU_DATA
    }

    {
        struct kan_render_atlas_loaded_entry_mapping_t temporary;
#define AT_INDEX(INDEX) (((struct kan_render_atlas_loaded_entry_mapping_t *) loaded->mapping.data)[INDEX])
#define LESS(first_index, second_index)                                                                                \
    __CUSHION_PRESERVE__ AT_INDEX (first_index).entry_name < AT_INDEX (second_index).entry_name
#define SWAP(first_index, second_index)                                                                                \
    __CUSHION_PRESERVE__                                                                                               \
    temporary = AT_INDEX (first_index), AT_INDEX (first_index) = AT_INDEX (second_index),                              \
    AT_INDEX (second_index) = temporary

        QSORT (loaded->mapping.size, LESS, SWAP);
#undef LESS
#undef SWAP
#undef AT_INDEX
    }

    KAN_UMO_EVENT_INSERT_INIT (kan_render_atlas_updated_event_t) {.name = loaded->name};
}

UNIVERSE_RENDER_FOUNDATION_API KAN_UM_MUTATOR_EXECUTE (render_foundation_atlas_management)
{
    KAN_UMI_SINGLETON_READ (render_context, kan_render_context_singleton_t)
    if (!KAN_HANDLE_IS_VALID (render_context->render_context))
    {
        return;
    }

    KAN_UMI_SINGLETON_READ (resource_provider, kan_resource_provider_singleton_t)
    if (resource_provider->transaction_state == KAN_RESOURCE_TRANSACTION_STATE_COMMIT)
    {
        KAN_UML_RESOURCE_LOADED_EVENT_FETCH (loaded_event, kan_resource_atlas_t)
        {
            KAN_UMI_RESOURCE_RETRIEVE_FRESH_LOADED (resource, kan_resource_atlas_t, &loaded_event->name)
            KAN_UMI_VALUE_UPDATE_OPTIONAL (existing_loaded, kan_render_atlas_loaded_t, name, &loaded_event->name)

            if (existing_loaded)
            {
                load_atlas (state, resource, existing_loaded);
            }
            else
            {
                KAN_UMO_INDEXED_INSERT (new_loaded, kan_render_atlas_loaded_t)
                {
                    new_loaded->name = loaded_event->name;
                    load_atlas (state, resource, new_loaded);
                }
            }
        }
    }

    if (resource_provider->transaction_state == KAN_RESOURCE_TRANSACTION_STATE_NONE)
    {
        KAN_UML_RESOURCE_UNLOAD_PLANNED_EVENT_FETCH (unload_event, kan_resource_atlas_t)
        {
            KAN_UMI_VALUE_DELETE_OPTIONAL (existing_loaded, kan_render_atlas_loaded_t, name, &unload_event->name)
            if (existing_loaded)
            {
                KAN_UM_ACCESS_DELETE (existing_loaded);
            }
        }
    }
}

void kan_render_atlas_loaded_init (struct kan_render_atlas_loaded_t *instance)
{
    instance->name = NULL;
    instance->image = KAN_HANDLE_SET_INVALID (kan_render_image_t);
    instance->entry_buffer = KAN_HANDLE_SET_INVALID (kan_render_buffer_t);

    kan_dynamic_array_init (&instance->mapping, 0u, sizeof (struct kan_render_atlas_loaded_entry_mapping_t),
                            alignof (struct kan_render_atlas_loaded_entry_mapping_t),
                            kan_allocation_group_stack_get ());

    kan_dynamic_array_init (&instance->locale_requirements, 0u, sizeof (kan_interned_string_t),
                            alignof (kan_interned_string_t), kan_allocation_group_stack_get ());
}

kan_instance_size_t kan_render_atlas_loaded_query (const struct kan_render_atlas_loaded_t *instance,
                                                   kan_interned_string_t entry_name,
                                                   kan_interned_string_t locale_name)
{
    kan_memory_size_t left = 0u;
    kan_memory_size_t right = instance->mapping.size;
    const struct kan_render_atlas_loaded_entry_mapping_t *mappings =
        (struct kan_render_atlas_loaded_entry_mapping_t *) instance->mapping.data;
    const kan_interned_string_t *locale_requirements = (kan_interned_string_t *) instance->locale_requirements.data;

    while (left < right)
    {
        kan_memory_size_t middle = (left + right) / 2u;
        if (entry_name < mappings[middle].entry_name)
        {
            right = middle;
        }
        else if (entry_name > mappings[middle].entry_name)
        {
            left = middle + 1u;
        }
        else
        {
            // Found the proper mapping. Now need to match locales.
            kan_instance_size_t match = mappings[middle].match_start;

            while (locale_requirements[match])
            {
                if (locale_requirements[match] == locale_name)
                {
                    // Locale-specific replacement discovered.
                    return match;
                }

                ++match;
                KAN_ASSERT (match < instance->locale_requirements.size)
            }

            // Encountered null-locale entry, match has ended with no replacements.
            return match;
        }
    }

    return KAN_INT_MAX (kan_instance_size_t);
}

void kan_render_atlas_loaded_shutdown (struct kan_render_atlas_loaded_t *instance)
{
    if (KAN_HANDLE_IS_VALID (instance->image))
    {
        kan_render_image_destroy (instance->image);
    }

    if (KAN_HANDLE_IS_VALID (instance->entry_buffer))
    {
        kan_render_buffer_destroy (instance->entry_buffer);
    }

    kan_dynamic_array_shutdown (&instance->mapping);
    kan_dynamic_array_shutdown (&instance->locale_requirements);
}
