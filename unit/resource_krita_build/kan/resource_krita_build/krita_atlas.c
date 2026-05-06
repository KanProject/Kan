#define _CRT_SECURE_NO_WARNINGS __CUSHION_PRESERVE__

#include <string.h>

#include <kan/api_common/min_max.h>
#include <kan/log/logging.h>
#include <kan/resource_krita_build/krita_atlas.h>
#include <kan/resource_krita_build/krita_import.h>
#include <kan/resource_pipeline/meta.h>

KAN_LOG_DEFINE_CATEGORY (resource_krita_atlas);

KAN_REFLECTION_STRUCT_FIELD_META (kan_resource_krita_atlas_source_t, krita_files)
RESOURCE_KRITA_BUILD_API struct kan_resource_reference_meta_t kan_resource_krita_atlas_source_reference_krita_files = {
    .type_name = "kan_resource_krita_header_t",
    .flags = 0u,
};

KAN_REFLECTION_STRUCT_META (kan_resource_krita_atlas_t)
RESOURCE_KRITA_BUILD_API struct kan_resource_type_meta_t kan_resource_krita_atlas_resource_type = {
    .flags = 0u,
    .version = CUSHION_START_NS_X64,
    .move = NULL,
    .reset = NULL,
};

void kan_resource_krita_atlas_source_init (struct kan_resource_krita_atlas_source_t *instance)
{
    instance->type = KAN_RESOURCE_ATLAS_IMAGE_TYPE_REGULAR;
    instance->color_table_multiplier_index = KAN_INT_MAX (kan_instance_size_t);
    kan_dynamic_array_init (&instance->krita_files, 0u, sizeof (kan_interned_string_t), alignof (kan_interned_string_t),
                            kan_allocation_group_stack_get ());
    instance->filter = NULL;
}

void kan_resource_krita_atlas_source_shutdown (struct kan_resource_krita_atlas_source_t *instance)
{
    kan_dynamic_array_shutdown (&instance->krita_files);
    instance->filter = NULL;
}

void kan_resource_krita_atlas_init (struct kan_resource_krita_atlas_t *instance)
{
    instance->page_width = 2048u;
    instance->page_height = 2048u;

    instance->border_size = 1u;
    instance->border = kan_make_color_linear (0.0f, 0.0f, 0.0f, 0.0f);

    kan_dynamic_array_init (&instance->sources, 0u, sizeof (struct kan_resource_krita_atlas_source_t),
                            alignof (struct kan_resource_krita_atlas_source_t), kan_allocation_group_stack_get ());
}

void kan_resource_krita_atlas_shutdown (struct kan_resource_krita_atlas_t *instance)
{
    KAN_DYNAMIC_ARRAY_SHUTDOWN_WITH_ITEMS_AUTO (instance->sources, kan_resource_krita_atlas_source)
}

static enum kan_resource_build_rule_result_t krita_atlas_build (struct kan_resource_build_rule_context_t *context);

KAN_REFLECTION_STRUCT_META (kan_resource_atlas_header_t)
RESOURCE_KRITA_BUILD_API struct kan_resource_build_rule_t kan_resource_krita_atlas_build_rule = {
    .primary_input_type = "kan_resource_krita_atlas_t",
    .platform_configuration_type = NULL,
    .secondary_types_count = 1u,
    .secondary_types = (const char *[]) {"kan_resource_krita_header_t"},
    .functor = krita_atlas_build,
    .version = CUSHION_START_NS_X64,
};

KAN_REFLECTION_IGNORE
enum krita_atlas_build_pass_t
{
    KRITA_ATLAS_BUILD_PASS_BASE = 0u,
    KRITA_ATLAS_BUILD_PASS_REPLACEMENTS,
};

static struct kan_resource_atlas_image_header_t krita_entry_to_atlas_image (
    const struct kan_resource_krita_atlas_source_t *source,
    const struct kan_resource_krita_header_entry_t *entry,
    float scale_factor)
{
    struct kan_resource_atlas_image_header_t result;
    result.source = entry->file;
    result.type = source->type;
    result.color_table_multiplier_index = source->color_table_multiplier_index;

    switch (result.type)
    {
    case KAN_RESOURCE_ATLAS_IMAGE_TYPE_REGULAR:
        break;

    case KAN_RESOURCE_ATLAS_IMAGE_TYPE_NINE_SLICE:
        result.nine_slice.tiled_x = source->nine_slice.tiled_x;
        result.nine_slice.tiled_y = source->nine_slice.tiled_y;
        result.nine_slice.left = (kan_instance_size_t) roundf (scale_factor * (float) source->nine_slice.left);
        result.nine_slice.right = (kan_instance_size_t) roundf (scale_factor * (float) source->nine_slice.right);
        result.nine_slice.top = (kan_instance_size_t) roundf (scale_factor * (float) source->nine_slice.top);
        result.nine_slice.bottom = (kan_instance_size_t) roundf (scale_factor * (float) source->nine_slice.bottom);
        break;
    }

    return result;
}

static bool krita_atlas_build_pass (struct kan_resource_build_rule_context_t *context,
                                    enum krita_atlas_build_pass_t pass)
{
    const struct kan_resource_krita_atlas_t *input = context->primary_input;
    struct kan_resource_atlas_header_t *output = context->primary_output;
    bool successful = true;

    for (kan_instance_size_t source_index = 0u; source_index < input->sources.size; ++source_index)
    {
        const struct kan_resource_krita_atlas_source_t *source =
            &((struct kan_resource_krita_atlas_source_t *) input->sources.data)[source_index];

        for (kan_instance_size_t file_index = 0u; file_index < source->krita_files.size; ++file_index)
        {
            const kan_interned_string_t header_name = ((kan_interned_string_t *) source->krita_files.data)[file_index];
            const struct kan_resource_krita_header_t *header = NULL;
            const struct kan_resource_build_rule_secondary_node_t *secondary = context->secondary_input_first;

            while (secondary)
            {
                if (secondary->name == header_name)
                {
                    // We only check the name as we do not expect other secondary types.
                    header = secondary->data;
                    break;
                }

                secondary = secondary->next;
            }

            KAN_ASSERT (header)
            for (kan_instance_size_t image_index = 0u; image_index < header->entries.size; ++image_index)
            {
                const struct kan_resource_krita_header_entry_t *image =
                    &((struct kan_resource_krita_header_entry_t *) header->entries.data)[image_index];

                if (image->filter != source->filter)
                {
                    continue;
                }

                switch (pass)
                {
                case KRITA_ATLAS_BUILD_PASS_BASE:
                {
                    if (image->locale)
                    {
                        break;
                    }

                    struct kan_resource_atlas_entry_header_t *entry = kan_dynamic_array_add_last (&output->entries);
                    if (!entry)
                    {
                        kan_dynamic_array_set_capacity (&output->entries, output->entries.size * 2u);
                        entry = kan_dynamic_array_add_last (&output->entries);
                    }

                    kan_allocation_group_stack_push (output->entries.allocation_group);
                    kan_resource_atlas_entry_header_init (entry);
                    kan_allocation_group_stack_pop ();

                    entry->name = image->name;
                    entry->image = krita_entry_to_atlas_image (source, image, header->scale_factor);
                    break;
                }

                case KRITA_ATLAS_BUILD_PASS_REPLACEMENTS:
                {
                    if (!image->locale)
                    {
                        break;
                    }

                    bool found = false;
                    for (kan_instance_size_t entry_index = 0u; entry_index < output->entries.size; ++entry_index)
                    {
                        struct kan_resource_atlas_entry_header_t *entry =
                            &((struct kan_resource_atlas_entry_header_t *) output->entries.data)[entry_index];

                        if (entry->name != image->name)
                        {
                            continue;
                        }

                        found = true;
                        struct kan_resource_atlas_entry_replacement_header_t *replacement =
                            kan_dynamic_array_add_last (&entry->replacements);

                        if (!replacement)
                        {
                            kan_dynamic_array_set_capacity (&entry->replacements,
                                                            KAN_MAX (1u, entry->replacements.size * 2u));
                            replacement = kan_dynamic_array_add_last (&entry->replacements);
                        }

                        replacement->for_locale = image->locale;
                        replacement->image = krita_entry_to_atlas_image (source, image, header->scale_factor);
                    }

                    if (!found)
                    {
                        successful = false;
                        KAN_LOG (resource_krita_atlas, KAN_LOG_ERROR,
                                 "While building atlas \"%s\", unable to find entry \"%s\", but replacement for it "
                                 "with locale \"%s\" exists.",
                                 context->primary_name, image->name, image->locale)
                    }

                    break;
                }
                }
            }
        }
    }

    return successful;
}

static enum kan_resource_build_rule_result_t krita_atlas_build (struct kan_resource_build_rule_context_t *context)
{
    const struct kan_resource_krita_atlas_t *input = context->primary_input;
    struct kan_resource_atlas_header_t *output = context->primary_output;

    output->page_width = input->page_width;
    output->page_height = input->page_height;
    output->border_size = input->border_size;
    output->border = input->border;
    kan_dynamic_array_set_capacity (&output->entries, KAN_RESOURCE_KRITA_ATLAS_CAPACITY);

    if (!krita_atlas_build_pass (context, KRITA_ATLAS_BUILD_PASS_BASE) ||
        !krita_atlas_build_pass (context, KRITA_ATLAS_BUILD_PASS_REPLACEMENTS))
    {
        return KAN_RESOURCE_BUILD_RULE_FAILURE;
    }

    kan_dynamic_array_set_capacity (&output->entries, output->entries.size);
    return KAN_RESOURCE_BUILD_RULE_SUCCESS;
}
