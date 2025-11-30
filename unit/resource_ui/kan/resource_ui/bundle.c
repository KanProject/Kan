#include <kan/resource_pipeline/meta.h>
#include <kan/resource_ui/bundle.h>

KAN_REFLECTION_STRUCT_META (kan_resource_ui_bundle_t)
RESOURCE_UI_API struct kan_resource_type_meta_t kan_resource_ui_bundle_resource_type = {
    .flags = KAN_RESOURCE_TYPE_ROOT,
    .version = CUSHION_START_NS_X64,
    .move = NULL,
    .reset = NULL,
};

KAN_REFLECTION_STRUCT_FIELD_META (kan_resource_ui_bundle_t, image_material_instance)
RESOURCE_UI_API struct kan_resource_reference_meta_t kan_resource_ui_bundle_reference_image_material_instance = {
    .type_name = "kan_resource_material_instance_t",
    .flags = 0u,
};

KAN_REFLECTION_STRUCT_FIELD_META (kan_resource_ui_bundle_t, image_atlas)
RESOURCE_UI_API struct kan_resource_reference_meta_t kan_resource_ui_bundle_reference_image_atlas = {
    .type_name = "kan_resource_atlas_t",
    .flags = 0u,
};

KAN_REFLECTION_STRUCT_FIELD_META (kan_resource_ui_bundle_t, text_sdf_material_instance)
RESOURCE_UI_API struct kan_resource_reference_meta_t kan_resource_ui_bundle_reference_text_sdf_material_instance = {
    .type_name = "kan_resource_material_instance_t",
    .flags = 0u,
};

KAN_REFLECTION_STRUCT_FIELD_META (kan_resource_ui_bundle_t, text_icon_material_instance)
RESOURCE_UI_API struct kan_resource_reference_meta_t kan_resource_ui_bundle_reference_text_icon_material_instance = {
    .type_name = "kan_resource_material_instance_t",
    .flags = 0u,
};

void kan_resource_ui_bundle_init (struct kan_resource_ui_bundle_t *instance)
{
    instance->image_material_instance = NULL;
    instance->image_atlas = NULL;
    instance->text_sdf_material_instance = NULL;
    instance->text_icon_material_instance = NULL;

    kan_dynamic_array_init (&instance->button_styles, 0u, sizeof (struct kan_resource_ui_button_style_t),
                            alignof (struct kan_resource_ui_button_style_t), kan_allocation_group_stack_get ());
}

void kan_resource_ui_bundle_shutdown (struct kan_resource_ui_bundle_t *instance)
{
    kan_dynamic_array_shutdown (&instance->button_styles);
}
