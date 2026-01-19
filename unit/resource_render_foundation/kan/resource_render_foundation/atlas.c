#include <kan/resource_pipeline/meta.h>
#include <kan/resource_render_foundation/atlas.h>

KAN_REFLECTION_STRUCT_FIELD_META (kan_resource_atlas_entry_replacement_t, for_locale)
RESOURCE_RENDER_FOUNDATION_API struct kan_resource_reference_meta_t
    kan_resource_atlas_entry_replacement_reference_for_locale = {
        .type_name = "kan_resource_locale_t",
        .flags = 0u,
};

KAN_REFLECTION_STRUCT_META (kan_resource_atlas_t)
RESOURCE_RENDER_FOUNDATION_API struct kan_resource_type_meta_t kan_resource_atlas_resource_type = {
    .flags = 0u,
    .version = CUSHION_START_NS_X64,
    .move = NULL,
    .reset = NULL,
};

void kan_resource_atlas_entry_init (struct kan_resource_atlas_entry_t *instance)
{
    instance->name = NULL;
    instance->image.page = 0u;
    instance->image.x = 0u;
    instance->image.y = 0u;
    instance->image.width = 0u;
    instance->image.height = 0u;
    instance->image.type = KAN_RESOURCE_ATLAS_IMAGE_TYPE_REGULAR;
    instance->image.color_table_multiplier_index = KAN_INT_MAX (kan_instance_size_t);

    kan_dynamic_array_init (&instance->replacements, 0u, sizeof (struct kan_resource_atlas_entry_replacement_t),
                            alignof (struct kan_resource_atlas_entry_replacement_t), kan_allocation_group_stack_get ());
}

void kan_resource_atlas_entry_shutdown (struct kan_resource_atlas_entry_t *instance)
{
    kan_dynamic_array_shutdown (&instance->replacements);
}

void kan_resource_atlas_init (struct kan_resource_atlas_t *instance)
{
    kan_dynamic_array_init (&instance->entries, 0u, sizeof (struct kan_resource_atlas_entry_t),
                            alignof (struct kan_resource_atlas_entry_t), kan_allocation_group_stack_get ());

    instance->total_entries = 0u;
    instance->page_count = 0u;
    instance->page_width = 0u;
    instance->page_height = 0u;

    kan_dynamic_array_init (&instance->data, 0u, sizeof (uint8_t), alignof (uint32_t),
                            kan_allocation_group_stack_get ());
}

void kan_resource_atlas_shutdown (struct kan_resource_atlas_t *instance)
{
    KAN_DYNAMIC_ARRAY_SHUTDOWN_WITH_ITEMS_AUTO (instance->entries, kan_resource_atlas_entry)
    kan_dynamic_array_shutdown (&instance->data);
}
