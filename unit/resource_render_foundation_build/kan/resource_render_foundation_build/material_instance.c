#define _CRT_SECURE_NO_WARNINGS __CUSHION_PRESERVE__

#include <string.h>

#include <kan/api_common/min_max.h>
#include <kan/log/logging.h>
#include <kan/resource_pipeline/meta.h>
#include <kan/resource_render_foundation/material.h>
#include <kan/resource_render_foundation_build/material_instance.h>

KAN_LOG_DEFINE_CATEGORY (resource_render_foundation_material_instance);
KAN_USE_STATIC_INTERNED_IDS

void kan_resource_material_parameter_container_init (struct kan_resource_material_parameter_container_t *instance)
{
#define INIT_CONTAINER(NAME)                                                                                           \
    kan_dynamic_array_init (&instance->NAME, 0u, sizeof (struct kan_resource_material_parameter_##NAME##_t),           \
                            alignof (struct kan_resource_material_parameter_##NAME##_t),                               \
                            kan_allocation_group_stack_get ())

    INIT_CONTAINER (f1);
    INIT_CONTAINER (f2);
    INIT_CONTAINER (f3);
    INIT_CONTAINER (f4);

    INIT_CONTAINER (u1);
    INIT_CONTAINER (u2);
    INIT_CONTAINER (u3);
    INIT_CONTAINER (u4);

    INIT_CONTAINER (s1);
    INIT_CONTAINER (s2);
    INIT_CONTAINER (s3);
    INIT_CONTAINER (s4);

    INIT_CONTAINER (f3x3);
    INIT_CONTAINER (f4x4);

    INIT_CONTAINER (color_linear);
    INIT_CONTAINER (color_srgb);

#undef INIT_CONTAINER
}

void kan_resource_material_parameter_container_shutdown (struct kan_resource_material_parameter_container_t *instance)
{
    kan_dynamic_array_shutdown (&instance->f1);
    kan_dynamic_array_shutdown (&instance->f2);
    kan_dynamic_array_shutdown (&instance->f3);
    kan_dynamic_array_shutdown (&instance->f4);

    kan_dynamic_array_shutdown (&instance->u1);
    kan_dynamic_array_shutdown (&instance->u2);
    kan_dynamic_array_shutdown (&instance->u3);
    kan_dynamic_array_shutdown (&instance->u4);

    kan_dynamic_array_shutdown (&instance->s1);
    kan_dynamic_array_shutdown (&instance->s2);
    kan_dynamic_array_shutdown (&instance->s3);
    kan_dynamic_array_shutdown (&instance->s4);

    kan_dynamic_array_shutdown (&instance->f3x3);
    kan_dynamic_array_shutdown (&instance->f4x4);

    kan_dynamic_array_shutdown (&instance->color_linear);
    kan_dynamic_array_shutdown (&instance->color_srgb);
}

void kan_resource_material_tail_set_init (struct kan_resource_material_tail_set_t *instance)
{
    instance->tail_name = NULL;
    instance->index = 0u;
    kan_resource_material_parameter_container_init (&instance->parameters);
}

void kan_resource_material_tail_set_shutdown (struct kan_resource_material_tail_set_t *instance)
{
    kan_resource_material_parameter_container_shutdown (&instance->parameters);
}

void kan_resource_material_tail_append_init (struct kan_resource_material_tail_append_t *instance)
{
    instance->tail_name = NULL;
    kan_resource_material_parameter_container_init (&instance->parameters);
}

void kan_resource_material_tail_append_shutdown (struct kan_resource_material_tail_append_t *instance)
{
    kan_resource_material_parameter_container_shutdown (&instance->parameters);
}

void kan_resource_material_sampler_init (struct kan_resource_material_sampler_t *instance)
{
    instance->name = NULL;
    instance->sampler.min_filter = KAN_RENDER_FILTER_MODE_NEAREST;
    instance->sampler.mag_filter = KAN_RENDER_FILTER_MODE_NEAREST;
    instance->sampler.mip_map_mode = KAN_RENDER_MIP_MAP_MODE_NEAREST;
    instance->sampler.address_mode_u = KAN_RENDER_ADDRESS_MODE_REPEAT;
    instance->sampler.address_mode_v = KAN_RENDER_ADDRESS_MODE_REPEAT;
    instance->sampler.address_mode_w = KAN_RENDER_ADDRESS_MODE_REPEAT;
    instance->sampler.depth_compare_enabled = false;
    instance->sampler.depth_compare = KAN_RENDER_COMPARE_OPERATION_NEVER;
    instance->sampler.anisotropy_enabled = false;
    instance->sampler.anisotropy_max = 1.0f;
}

KAN_REFLECTION_STRUCT_FIELD_META (kan_resource_material_image_t, texture)
RESOURCE_RENDER_FOUNDATION_BUILD_API struct kan_resource_reference_meta_t
    kan_resource_material_image_reference_texture = {
        .type_name = "kan_resource_texture_t",
        .flags = 0u,
};

void kan_resource_material_image_init (struct kan_resource_material_image_t *instance)
{
    instance->name = NULL;
    instance->texture = NULL;
}

void kan_resource_material_variant_raw_init (struct kan_resource_material_variant_raw_t *instance)
{
    instance->name = NULL;
    kan_resource_material_parameter_container_init (&instance->parameters);
}

void kan_resource_material_variant_raw_shutdown (struct kan_resource_material_variant_raw_t *instance)
{
    kan_resource_material_parameter_container_shutdown (&instance->parameters);
}

KAN_REFLECTION_STRUCT_META (kan_resource_material_instance_raw_t)
RESOURCE_RENDER_FOUNDATION_BUILD_API struct kan_resource_type_meta_t kan_resource_material_instance_raw_resource_type =
    {
        .flags = 0u,
        .version = CUSHION_START_NS_X64,
        .move = NULL,
        .reset = NULL,
};

KAN_REFLECTION_STRUCT_FIELD_META (kan_resource_material_instance_raw_t, material)
RESOURCE_RENDER_FOUNDATION_BUILD_API struct kan_resource_reference_meta_t
    kan_resource_material_instance_raw_reference_material = {
        .type_name = "kan_resource_material_t",
        .flags = 0u,
};

KAN_REFLECTION_STRUCT_FIELD_META (kan_resource_material_instance_raw_t, parent)
RESOURCE_RENDER_FOUNDATION_BUILD_API struct kan_resource_reference_meta_t
    kan_resource_material_instance_raw_reference_parent = {
        .type_name = "kan_resource_material_instance_t",
        .flags = KAN_RESOURCE_REFERENCE_META_NULLABLE,
};

KAN_REFLECTION_STRUCT_FIELD_META (kan_resource_material_instance_raw_t, use_mixins)
RESOURCE_RENDER_FOUNDATION_BUILD_API struct kan_resource_reference_meta_t
    kan_resource_material_instance_raw_reference_use_mixins = {
        .type_name = "kan_resource_material_instance_mixin_t",
        .flags = KAN_RESOURCE_REFERENCE_META_NULLABLE,
};

void kan_resource_material_instance_raw_init (struct kan_resource_material_instance_raw_t *instance)
{
    instance->material = NULL;
    instance->parent = NULL;
    kan_dynamic_array_init (&instance->use_mixins, 0u, sizeof (kan_interned_string_t), alignof (kan_interned_string_t),
                            kan_allocation_group_stack_get ());
    kan_resource_material_parameter_container_init (&instance->parameters);
    kan_dynamic_array_init (&instance->tail_set, 0u, sizeof (struct kan_resource_material_tail_set_t),
                            alignof (struct kan_resource_material_tail_set_t), kan_allocation_group_stack_get ());
    kan_dynamic_array_init (&instance->tail_append, 0u, sizeof (struct kan_resource_material_tail_append_t),
                            alignof (struct kan_resource_material_tail_append_t), kan_allocation_group_stack_get ());
    kan_dynamic_array_init (&instance->samplers, 0u, sizeof (struct kan_resource_material_sampler_t),
                            alignof (struct kan_resource_material_sampler_t), kan_allocation_group_stack_get ());
    kan_dynamic_array_init (&instance->images, 0u, sizeof (struct kan_resource_material_image_t),
                            alignof (struct kan_resource_material_image_t), kan_allocation_group_stack_get ());
    kan_dynamic_array_init (&instance->variants, 0u, sizeof (struct kan_resource_material_variant_raw_t),
                            alignof (struct kan_resource_material_variant_raw_t), kan_allocation_group_stack_get ());
}

void kan_resource_material_instance_raw_shutdown (struct kan_resource_material_instance_raw_t *instance)
{
    kan_dynamic_array_shutdown (&instance->use_mixins);
    kan_resource_material_parameter_container_shutdown (&instance->parameters);
    KAN_DYNAMIC_ARRAY_SHUTDOWN_WITH_ITEMS_AUTO (instance->tail_set, kan_resource_material_tail_set)
    KAN_DYNAMIC_ARRAY_SHUTDOWN_WITH_ITEMS_AUTO (instance->tail_append, kan_resource_material_tail_append)
    kan_dynamic_array_shutdown (&instance->samplers);
    kan_dynamic_array_shutdown (&instance->images);
    KAN_DYNAMIC_ARRAY_SHUTDOWN_WITH_ITEMS_AUTO (instance->variants, kan_resource_material_variant_raw)
}

KAN_REFLECTION_STRUCT_META (kan_resource_material_instance_mixin_t)
RESOURCE_RENDER_FOUNDATION_BUILD_API struct kan_resource_type_meta_t
    kan_resource_material_instance_mixin_resource_type = {
        .flags = 0u,
        .version = CUSHION_START_NS_X64,
        .move = NULL,
        .reset = NULL,
};

void kan_resource_material_instance_mixin_init (struct kan_resource_material_instance_mixin_t *instance)
{
    kan_resource_material_parameter_container_init (&instance->parameters);
    kan_dynamic_array_init (&instance->tail_set, 0u, sizeof (struct kan_resource_material_tail_set_t),
                            alignof (struct kan_resource_material_tail_set_t), kan_allocation_group_stack_get ());
    kan_dynamic_array_init (&instance->tail_append, 0u, sizeof (struct kan_resource_material_tail_append_t),
                            alignof (struct kan_resource_material_tail_append_t), kan_allocation_group_stack_get ());
    kan_dynamic_array_init (&instance->samplers, 0u, sizeof (struct kan_resource_material_sampler_t),
                            alignof (struct kan_resource_material_sampler_t), kan_allocation_group_stack_get ());
    kan_dynamic_array_init (&instance->images, 0u, sizeof (struct kan_resource_material_image_t),
                            alignof (struct kan_resource_material_image_t), kan_allocation_group_stack_get ());
    kan_dynamic_array_init (&instance->variants, 0u, sizeof (struct kan_resource_material_variant_raw_t),
                            alignof (struct kan_resource_material_variant_raw_t), kan_allocation_group_stack_get ());
}

void kan_resource_material_instance_mixin_shutdown (struct kan_resource_material_instance_mixin_t *instance)
{
    kan_resource_material_parameter_container_shutdown (&instance->parameters);
    KAN_DYNAMIC_ARRAY_SHUTDOWN_WITH_ITEMS_AUTO (instance->tail_set, kan_resource_material_tail_set)
    KAN_DYNAMIC_ARRAY_SHUTDOWN_WITH_ITEMS_AUTO (instance->tail_append, kan_resource_material_tail_append)
    kan_dynamic_array_shutdown (&instance->samplers);
    kan_dynamic_array_shutdown (&instance->images);
    KAN_DYNAMIC_ARRAY_SHUTDOWN_WITH_ITEMS_AUTO (instance->variants, kan_resource_material_variant_raw)
}

static enum kan_resource_build_rule_result_t material_instance_build (
    struct kan_resource_build_rule_context_t *context);

KAN_REFLECTION_STRUCT_META (kan_resource_material_instance_t)
RESOURCE_RENDER_FOUNDATION_BUILD_API struct kan_resource_build_rule_t kan_resource_material_instance_build_rule = {
    .primary_input_type = "kan_resource_material_instance_raw_t",
    .platform_configuration_type = NULL,
    .secondary_types_count = 3u,
    .secondary_types =
        (const char *[]) {
            "kan_resource_material_t",
            "kan_resource_material_instance_t",
            "kan_resource_material_instance_mixin_t",
        },
    .functor = material_instance_build,
    .version = CUSHION_START_NS_X64,
};

static inline struct kan_float_vector_4_t color_linear_converter (uint32_t hex)
{
    return kan_color_linear_as_vector (kan_make_color_linear_from_web (hex));
}

static inline struct kan_float_vector_4_t color_srgb_converter (uint32_t hex)
{
    return kan_color_srgb_as_vector (kan_make_color_srgb_from_web (hex));
}

/// \brief Helper for parameter apply functions below that pipes all types to expected local PROCESS_TYPE macro.
#define PROCESS_PARAMETER_TYPES                                                                                        \
    PROCESS_TYPE (f1, KAN_RPL_META_VARIABLE_TYPE_F1, float, )                                                          \
    PROCESS_TYPE (f2, KAN_RPL_META_VARIABLE_TYPE_F2, struct kan_float_vector_2_t, )                                    \
    PROCESS_TYPE (f3, KAN_RPL_META_VARIABLE_TYPE_F3, struct kan_float_vector_3_t, )                                    \
    PROCESS_TYPE (f4, KAN_RPL_META_VARIABLE_TYPE_F4, struct kan_float_vector_4_t, )                                    \
                                                                                                                       \
    PROCESS_TYPE (u1, KAN_RPL_META_VARIABLE_TYPE_U1, uint32_t, )                                                       \
    PROCESS_TYPE (u2, KAN_RPL_META_VARIABLE_TYPE_U2, struct kan_uint32_vector_2_t, )                                   \
    PROCESS_TYPE (u3, KAN_RPL_META_VARIABLE_TYPE_U3, struct kan_uint32_vector_3_t, )                                   \
    PROCESS_TYPE (u4, KAN_RPL_META_VARIABLE_TYPE_U4, struct kan_uint32_vector_4_t, )                                   \
                                                                                                                       \
    PROCESS_TYPE (s1, KAN_RPL_META_VARIABLE_TYPE_S1, int32_t, )                                                        \
    PROCESS_TYPE (s2, KAN_RPL_META_VARIABLE_TYPE_S2, struct kan_int32_vector_2_t, )                                    \
    PROCESS_TYPE (s3, KAN_RPL_META_VARIABLE_TYPE_S3, struct kan_int32_vector_3_t, )                                    \
    PROCESS_TYPE (s4, KAN_RPL_META_VARIABLE_TYPE_S4, struct kan_int32_vector_4_t, )                                    \
                                                                                                                       \
    PROCESS_TYPE (f3x3, KAN_RPL_META_VARIABLE_TYPE_F3X3, struct kan_float_matrix_3x3_t, )                              \
    PROCESS_TYPE (f4x4, KAN_RPL_META_VARIABLE_TYPE_F4X4, struct kan_float_matrix_4x4_t, )                              \
                                                                                                                       \
    PROCESS_TYPE (color_linear, KAN_RPL_META_VARIABLE_TYPE_F4, struct kan_float_vector_4_t, color_linear_converter)    \
    PROCESS_TYPE (color_srgb, KAN_RPL_META_VARIABLE_TYPE_F4, struct kan_float_vector_4_t, color_srgb_converter)

static bool apply_tail_parameters (struct kan_resource_build_rule_context_t *context,
                                   const struct kan_rpl_meta_buffer_t *buffer_meta,
                                   const struct kan_resource_material_parameter_container_t *parameters,
                                   uint8_t *tail_item,
                                   kan_interned_string_t tail_name)
{
    bool successful = true;
#define PROCESS_TYPE(NAME, RPL_TYPE, OUTPUT_TYPE, CONVERTER)                                                           \
    for (kan_loop_size_t parameter_index = 0u; parameter_index < parameters->NAME.size; ++parameter_index)             \
    {                                                                                                                  \
        const struct kan_resource_material_parameter_##NAME##_t *parameter =                                           \
            &((struct kan_resource_material_parameter_##NAME##_t *) parameters->NAME.data)[parameter_index];           \
        bool parameter_found = false;                                                                                  \
                                                                                                                       \
        for (kan_loop_size_t meta_parameter_index = 0u; meta_parameter_index < buffer_meta->tail_item_parameters.size; \
             ++meta_parameter_index)                                                                                   \
        {                                                                                                              \
            const struct kan_rpl_meta_parameter_t *parameter_meta =                                                    \
                &((struct kan_rpl_meta_parameter_t *) buffer_meta->tail_item_parameters.data)[meta_parameter_index];   \
                                                                                                                       \
            if (parameter_meta->name != parameter->name)                                                               \
            {                                                                                                          \
                continue;                                                                                              \
            }                                                                                                          \
                                                                                                                       \
            parameter_found = true;                                                                                    \
            if (RPL_TYPE != parameter_meta->type)                                                                      \
            {                                                                                                          \
                KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,                                  \
                         "Material instance \"%s\" failed to apply parameter \"%s\" in tail \"%s\" as given "          \
                         "parameter has type \"%s\" while type \"%s\" is expected by material code.",                  \
                         context->primary_name, parameter->name, tail_name,                                            \
                         kan_rpl_meta_variable_type_to_string (RPL_TYPE),                                              \
                         kan_rpl_meta_variable_type_to_string (parameter_meta->type))                                  \
                successful = false;                                                                                    \
                break;                                                                                                 \
            }                                                                                                          \
                                                                                                                       \
            if (parameter_meta->total_item_count != 1u)                                                                \
            {                                                                                                          \
                KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,                                  \
                         "Material instance \"%s\" failed to apply parameter \"%s\" in tail \"%s\" as "                \
                         "parameter meta expects several values and it is not yet supported.",                         \
                         context->primary_name, parameter->name, tail_name)                                            \
                successful = false;                                                                                    \
                break;                                                                                                 \
            }                                                                                                          \
                                                                                                                       \
            *(OUTPUT_TYPE *) (tail_item + parameter_meta->offset) = CONVERTER (parameter->value);                      \
            break;                                                                                                     \
        }                                                                                                              \
                                                                                                                       \
        if (!parameter_found)                                                                                          \
        {                                                                                                              \
            KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,                                      \
                     "Material instance \"%s\" cannot apply parameter \"%s\" for tail \"%s\" as it is not "            \
                     "found across tail item parameters.",                                                             \
                     context->primary_name, parameter->name, tail_name)                                                \
            successful = false;                                                                                        \
        }                                                                                                              \
    }

    PROCESS_PARAMETER_TYPES
#undef PROCESS_TYPE
    return successful;
}

static bool apply_parameters_container (struct kan_resource_build_rule_context_t *context,
                                        const struct kan_resource_material_t *material,
                                        const struct kan_resource_material_parameter_container_t *array)
{
    struct kan_resource_material_instance_t *output = context->primary_output;
    bool successful = true;

#define PROCESS_TYPE(NAME, RPL_TYPE, OUTPUT_TYPE, CONVERTER)                                                           \
    for (kan_loop_size_t parameter_index = 0u; parameter_index < array->NAME.size; ++parameter_index)                  \
    {                                                                                                                  \
        const struct kan_resource_material_parameter_##NAME##_t *parameter =                                           \
            &((struct kan_resource_material_parameter_##NAME##_t *) array->NAME.data)[parameter_index];                \
        bool found = false;                                                                                            \
                                                                                                                       \
        for (kan_loop_size_t buffer_index = 0u; buffer_index < material->set_material.buffers.size && !found;          \
             ++buffer_index)                                                                                           \
        {                                                                                                              \
            const struct kan_rpl_meta_buffer_t *buffer_meta =                                                          \
                &((struct kan_rpl_meta_buffer_t *) material->set_material.buffers.data)[buffer_index];                 \
                                                                                                                       \
            for (kan_loop_size_t meta_parameter_index = 0u; meta_parameter_index < buffer_meta->main_parameters.size;  \
                 ++meta_parameter_index)                                                                               \
            {                                                                                                          \
                const struct kan_rpl_meta_parameter_t *parameter_meta =                                                \
                    &((struct kan_rpl_meta_parameter_t *) buffer_meta->main_parameters.data)[meta_parameter_index];    \
                                                                                                                       \
                if (parameter_meta->name != parameter->name)                                                           \
                {                                                                                                      \
                    continue;                                                                                          \
                }                                                                                                      \
                                                                                                                       \
                found = true;                                                                                          \
                if (RPL_TYPE != parameter_meta->type)                                                                  \
                {                                                                                                      \
                    KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,                              \
                             "Material instance \"%s\" failed to apply parameter \"%s\" as given parameter has type "  \
                             "\"%s\" while type \"%s\" is expected by material code.",                                 \
                             context->primary_name, parameter->name, kan_rpl_meta_variable_type_to_string (RPL_TYPE),  \
                             kan_rpl_meta_variable_type_to_string (parameter_meta->type))                              \
                    successful = false;                                                                                \
                    break;                                                                                             \
                }                                                                                                      \
                                                                                                                       \
                if (parameter_meta->total_item_count != 1u)                                                            \
                {                                                                                                      \
                    KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,                              \
                             "Material instance \"%s\" failed to apply parameter \"%s\" as parameter meta expects "    \
                             "several values and it is not yet supported.",                                            \
                             context->primary_name, parameter->name)                                                   \
                    successful = false;                                                                                \
                    break;                                                                                             \
                }                                                                                                      \
                                                                                                                       \
                struct kan_resource_buffer_binding_t *buffer =                                                         \
                    &((struct kan_resource_buffer_binding_t *) output->buffers.data)[buffer_index];                    \
                *(OUTPUT_TYPE *) (buffer->data.data + parameter_meta->offset) = CONVERTER (parameter->value);          \
                break;                                                                                                 \
            }                                                                                                          \
        }                                                                                                              \
                                                                                                                       \
        if (!found)                                                                                                    \
        {                                                                                                              \
            KAN_LOG (                                                                                                  \
                resource_render_foundation_material_instance, KAN_LOG_ERROR,                                           \
                "Material instance \"%s\" cannot apply parameter \"%s\" as it is not found across main parameters.",   \
                context->primary_name, parameter->name)                                                                \
            successful = false;                                                                                        \
        }                                                                                                              \
    }

    PROCESS_PARAMETER_TYPES
#undef PROCESS_TYPE
    return successful;
}

static bool apply_tail_set_array (struct kan_resource_build_rule_context_t *context,
                                  const struct kan_resource_material_t *material,
                                  const struct kan_dynamic_array_t *array)
{
    struct kan_resource_material_instance_t *output = context->primary_output;
    bool successful = true;

    for (kan_loop_size_t set_index = 0u; set_index < array->size; ++set_index)
    {
        const struct kan_resource_material_tail_set_t *tail_set =
            &((struct kan_resource_material_tail_set_t *) array->data)[set_index];
        bool tail_found = false;

        for (kan_loop_size_t buffer_index = 0u; buffer_index < material->set_material.buffers.size; ++buffer_index)
        {
            const struct kan_rpl_meta_buffer_t *buffer_meta =
                &((struct kan_rpl_meta_buffer_t *) material->set_material.buffers.data)[buffer_index];

            if (buffer_meta->tail_name != tail_set->tail_name)
            {
                continue;
            }

            tail_found = true;
            const kan_instance_size_t tail_offset =
                buffer_meta->main_size + buffer_meta->tail_item_size * tail_set->index;
            const kan_instance_size_t tail_end = tail_offset + buffer_meta->tail_item_size;

            struct kan_resource_buffer_binding_t *buffer =
                &((struct kan_resource_buffer_binding_t *) output->buffers.data)[buffer_index];

            if (tail_end > buffer->data.size)
            {
                if (tail_end > buffer->data.capacity)
                {
                    kan_dynamic_array_set_capacity (&buffer->data, tail_end + KAN_RESOURCE_RF_MI_TAIL_APPEND_CAPACITY *
                                                                                  buffer_meta->tail_item_size);
                }

                const kan_instance_size_t size_before = buffer->data.size;
                buffer->data.size = tail_end;
                memset (buffer->data.data + size_before, 0, tail_end - size_before);
            }

            successful &= apply_tail_parameters (context, buffer_meta, &tail_set->parameters,
                                                 buffer->data.data + tail_offset, tail_set->tail_name);
            break;
        }

        if (!tail_found)
        {
            KAN_LOG (
                resource_render_foundation_material_instance, KAN_LOG_ERROR,
                "Material instance \"%s\" cannot apply tail set \"%s\" as that tail name is not found across buffers.",
                context->primary_name, tail_set->tail_name)
            successful = false;
        }
    }

    return successful;
}

static bool apply_tail_append_array (struct kan_resource_build_rule_context_t *context,
                                     const struct kan_resource_material_t *material,
                                     const struct kan_dynamic_array_t *array)
{
    struct kan_resource_material_instance_t *output = context->primary_output;
    bool successful = true;

    for (kan_loop_size_t append_index = 0u; append_index < array->size; ++append_index)
    {
        const struct kan_resource_material_tail_append_t *tail_append =
            &((struct kan_resource_material_tail_append_t *) array->data)[append_index];
        bool tail_found = false;

        for (kan_loop_size_t buffer_index = 0u; buffer_index < material->set_material.buffers.size; ++buffer_index)
        {
            const struct kan_rpl_meta_buffer_t *buffer_meta =
                &((struct kan_rpl_meta_buffer_t *) material->set_material.buffers.data)[buffer_index];

            if (buffer_meta->tail_name != tail_append->tail_name)
            {
                continue;
            }

            tail_found = true;
            struct kan_resource_buffer_binding_t *buffer =
                &((struct kan_resource_buffer_binding_t *) output->buffers.data)[buffer_index];
            KAN_ASSERT ((buffer->data.size - buffer_meta->main_size) % buffer_meta->tail_item_size == 0u)

            const kan_instance_size_t tail_offset = buffer->data.size;
            const kan_instance_size_t tail_end = tail_offset + buffer_meta->tail_item_size;

            if (tail_end > buffer->data.capacity)
            {
                kan_dynamic_array_set_capacity (
                    &buffer->data, tail_end + KAN_RESOURCE_RF_MI_TAIL_APPEND_CAPACITY * buffer_meta->tail_item_size);
            }

            const kan_instance_size_t size_before = buffer->data.size;
            buffer->data.size = tail_end;
            memset (buffer->data.data + size_before, 0, tail_end - size_before);

            successful &= apply_tail_parameters (context, buffer_meta, &tail_append->parameters,
                                                 buffer->data.data + tail_offset, tail_append->tail_name);
            break;
        }

        if (!tail_found)
        {
            KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                     "Material instance \"%s\" cannot apply tail append \"%s\" as that tail name is not found across "
                     "buffers.",
                     context->primary_name, tail_append->tail_name)
            successful = false;
        }
    }

    return successful;
}

#define F1_TO_UNORM(TYPE, VALUE) (TYPE) roundf (KAN_CLAMP (VALUE, 0.0f, 1.0f) * (float) KAN_INT_MAX (TYPE))
#define F1_TO_SNORM(TYPE, VALUE) (TYPE) roundf (KAN_CLAMP (VALUE, -1.0f, 1.0f) * (float) KAN_INT_MAX (TYPE))
#define UINT_FIT(TYPE, VALUE) (TYPE) KAN_MIN (VALUE, KAN_INT_MAX (TYPE))
#define SINT_FIT(TYPE, VALUE) (TYPE) KAN_CLAMP (VALUE, KAN_INT_MIN (TYPE), KAN_INT_MAX (TYPE))

static inline bool apply_attribute_value_f1 (const struct kan_rpl_meta_attribute_t *attribute,
                                             const struct kan_resource_material_parameter_f1_t *parameter,
                                             void *address,
                                             const char *instance_name,
                                             const char *variant_name)
{
    if (attribute->class != KAN_RPL_META_ATTRIBUTE_CLASS_VECTOR_1)
    {
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "of class \"%s\" as 1-item class was expected.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_F1),
                 kan_rpl_meta_attribute_class_to_string (attribute->class))
        return false;
    }

    switch (attribute->item_format)
    {
    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_FLOAT_32:
        *(float *) address = parameter->value;
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_UNORM_8:
        *(uint8_t *) address = F1_TO_UNORM (uint8_t, parameter->value);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_UNORM_16:
        *(uint16_t *) address = F1_TO_UNORM (uint16_t, parameter->value);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_SNORM_8:
        *(uint8_t *) address = F1_TO_SNORM (uint8_t, parameter->value);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_SNORM_16:
        *(uint16_t *) address = F1_TO_SNORM (uint16_t, parameter->value);
        return true;

    default:
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "with format \"%s\" as this format is not supported.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_F1),
                 kan_rpl_meta_attribute_item_format_to_string (attribute->item_format))
        return false;
    }
}

static inline bool apply_attribute_value_f2 (const struct kan_rpl_meta_attribute_t *attribute,
                                             const struct kan_resource_material_parameter_f2_t *parameter,
                                             void *address,
                                             const char *instance_name,
                                             const char *variant_name)
{
    if (attribute->class != KAN_RPL_META_ATTRIBUTE_CLASS_VECTOR_2)
    {
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "of class \"%s\" as 2-item class was expected.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_F2),
                 kan_rpl_meta_attribute_class_to_string (attribute->class))
        return false;
    }

    switch (attribute->item_format)
    {
    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_FLOAT_32:
        *(struct kan_float_vector_2_t *) address = parameter->value;
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_UNORM_8:
        ((uint8_t *) address)[0u] = F1_TO_UNORM (uint8_t, parameter->value.x);
        ((uint8_t *) address)[1u] = F1_TO_UNORM (uint8_t, parameter->value.y);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_UNORM_16:
        ((uint16_t *) address)[0u] = F1_TO_UNORM (uint16_t, parameter->value.x);
        ((uint16_t *) address)[1u] = F1_TO_UNORM (uint16_t, parameter->value.y);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_SNORM_8:
        ((uint8_t *) address)[0u] = F1_TO_SNORM (uint8_t, parameter->value.x);
        ((uint8_t *) address)[1u] = F1_TO_SNORM (uint8_t, parameter->value.y);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_SNORM_16:
        ((uint16_t *) address)[0u] = F1_TO_SNORM (uint16_t, parameter->value.x);
        ((uint16_t *) address)[1u] = F1_TO_SNORM (uint16_t, parameter->value.y);
        return true;

    default:
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "with format \"%s\" as this format is not supported.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_F2),
                 kan_rpl_meta_attribute_item_format_to_string (attribute->item_format))
        return false;
    }
}

static inline bool apply_attribute_value_f3 (const struct kan_rpl_meta_attribute_t *attribute,
                                             const struct kan_resource_material_parameter_f3_t *parameter,
                                             void *address,
                                             const char *instance_name,
                                             const char *variant_name)
{
    if (attribute->class != KAN_RPL_META_ATTRIBUTE_CLASS_VECTOR_3)
    {
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "of class \"%s\" as 3-item class was expected.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_F3),
                 kan_rpl_meta_attribute_class_to_string (attribute->class))
        return false;
    }

    switch (attribute->item_format)
    {
    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_FLOAT_32:
        *(struct kan_float_vector_3_t *) address = parameter->value;
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_UNORM_8:
        ((uint8_t *) address)[0u] = F1_TO_UNORM (uint8_t, parameter->value.x);
        ((uint8_t *) address)[1u] = F1_TO_UNORM (uint8_t, parameter->value.y);
        ((uint8_t *) address)[2u] = F1_TO_UNORM (uint8_t, parameter->value.z);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_UNORM_16:
        ((uint16_t *) address)[0u] = F1_TO_UNORM (uint16_t, parameter->value.x);
        ((uint16_t *) address)[1u] = F1_TO_UNORM (uint16_t, parameter->value.y);
        ((uint16_t *) address)[2u] = F1_TO_UNORM (uint16_t, parameter->value.z);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_SNORM_8:
        ((uint8_t *) address)[0u] = F1_TO_SNORM (uint8_t, parameter->value.x);
        ((uint8_t *) address)[1u] = F1_TO_SNORM (uint8_t, parameter->value.y);
        ((uint8_t *) address)[2u] = F1_TO_SNORM (uint8_t, parameter->value.z);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_SNORM_16:
        ((uint16_t *) address)[0u] = F1_TO_SNORM (uint16_t, parameter->value.x);
        ((uint16_t *) address)[1u] = F1_TO_SNORM (uint16_t, parameter->value.y);
        ((uint16_t *) address)[2u] = F1_TO_SNORM (uint16_t, parameter->value.z);
        return true;

    default:
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "with format \"%s\" as this format is not supported.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_F3),
                 kan_rpl_meta_attribute_item_format_to_string (attribute->item_format))
        return false;
    }
}

static inline bool apply_attribute_value_f4 (const struct kan_rpl_meta_attribute_t *attribute,
                                             const struct kan_resource_material_parameter_f4_t *parameter,
                                             void *address,
                                             const char *instance_name,
                                             const char *variant_name)
{
    if (attribute->class != KAN_RPL_META_ATTRIBUTE_CLASS_VECTOR_4)
    {
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "of class \"%s\" as 4-item class was expected.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_F4),
                 kan_rpl_meta_attribute_class_to_string (attribute->class))
        return false;
    }

    switch (attribute->item_format)
    {
    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_FLOAT_32:
        *(struct kan_float_vector_4_t *) address = parameter->value;
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_UNORM_8:
        ((uint8_t *) address)[0u] = F1_TO_UNORM (uint8_t, parameter->value.x);
        ((uint8_t *) address)[1u] = F1_TO_UNORM (uint8_t, parameter->value.y);
        ((uint8_t *) address)[2u] = F1_TO_UNORM (uint8_t, parameter->value.z);
        ((uint8_t *) address)[3u] = F1_TO_UNORM (uint8_t, parameter->value.w);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_UNORM_16:
        ((uint16_t *) address)[0u] = F1_TO_UNORM (uint16_t, parameter->value.x);
        ((uint16_t *) address)[1u] = F1_TO_UNORM (uint16_t, parameter->value.y);
        ((uint16_t *) address)[2u] = F1_TO_UNORM (uint16_t, parameter->value.z);
        ((uint16_t *) address)[3u] = F1_TO_UNORM (uint16_t, parameter->value.w);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_SNORM_8:
        ((uint8_t *) address)[0u] = F1_TO_SNORM (uint8_t, parameter->value.x);
        ((uint8_t *) address)[1u] = F1_TO_SNORM (uint8_t, parameter->value.y);
        ((uint8_t *) address)[2u] = F1_TO_SNORM (uint8_t, parameter->value.z);
        ((uint8_t *) address)[3u] = F1_TO_SNORM (uint8_t, parameter->value.w);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_SNORM_16:
        ((uint16_t *) address)[0u] = F1_TO_SNORM (uint16_t, parameter->value.x);
        ((uint16_t *) address)[1u] = F1_TO_SNORM (uint16_t, parameter->value.y);
        ((uint16_t *) address)[2u] = F1_TO_SNORM (uint16_t, parameter->value.z);
        ((uint16_t *) address)[3u] = F1_TO_SNORM (uint16_t, parameter->value.w);
        return true;

    default:
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "with format \"%s\" as this format is not supported.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_F4),
                 kan_rpl_meta_attribute_item_format_to_string (attribute->item_format))
        return false;
    }
}

static inline bool apply_attribute_value_u1 (const struct kan_rpl_meta_attribute_t *attribute,
                                             const struct kan_resource_material_parameter_u1_t *parameter,
                                             void *address,
                                             const char *instance_name,
                                             const char *variant_name)
{
    if (attribute->class != KAN_RPL_META_ATTRIBUTE_CLASS_VECTOR_1)
    {
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "of class \"%s\" as 1-item class was expected.",
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_U1), instance_name, variant_name,
                 parameter->name, kan_rpl_meta_attribute_class_to_string (attribute->class))
        return false;
    }

    switch (attribute->item_format)
    {
    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_UINT_8:
        *(uint8_t *) address = UINT_FIT (uint8_t, parameter->value);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_UINT_16:
        *(uint16_t *) address = UINT_FIT (uint16_t, parameter->value);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_UINT_32:
        *(uint32_t *) address = UINT_FIT (uint32_t, parameter->value);
        return true;

    default:
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "with format \"%s\" as this format is not supported.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_U1),
                 kan_rpl_meta_attribute_item_format_to_string (attribute->item_format))
        return false;
    }
}

static inline bool apply_attribute_value_u2 (const struct kan_rpl_meta_attribute_t *attribute,
                                             const struct kan_resource_material_parameter_u2_t *parameter,
                                             void *address,
                                             const char *instance_name,
                                             const char *variant_name)
{
    if (attribute->class != KAN_RPL_META_ATTRIBUTE_CLASS_VECTOR_2)
    {
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "of class \"%s\" as 2-item class was expected.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_U2),
                 kan_rpl_meta_attribute_class_to_string (attribute->class))
        return false;
    }

    switch (attribute->item_format)
    {
    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_UINT_8:
        ((uint8_t *) address)[0u] = UINT_FIT (uint8_t, parameter->value.x);
        ((uint8_t *) address)[1u] = UINT_FIT (uint8_t, parameter->value.y);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_UINT_16:
        ((uint16_t *) address)[0u] = UINT_FIT (uint16_t, parameter->value.x);
        ((uint16_t *) address)[1u] = UINT_FIT (uint16_t, parameter->value.y);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_UINT_32:
        ((uint32_t *) address)[0u] = UINT_FIT (uint32_t, parameter->value.x);
        ((uint32_t *) address)[1u] = UINT_FIT (uint32_t, parameter->value.y);
        return true;

    default:
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "with format \"%s\" as this format is not supported.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_U2),
                 kan_rpl_meta_attribute_item_format_to_string (attribute->item_format))
        return false;
    }
}

static inline bool apply_attribute_value_u3 (const struct kan_rpl_meta_attribute_t *attribute,
                                             const struct kan_resource_material_parameter_u3_t *parameter,
                                             void *address,
                                             const char *instance_name,
                                             const char *variant_name)
{
    if (attribute->class != KAN_RPL_META_ATTRIBUTE_CLASS_VECTOR_3)
    {
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "of class \"%s\" as 3-item class was expected.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_U3),
                 kan_rpl_meta_attribute_class_to_string (attribute->class))
        return false;
    }

    switch (attribute->item_format)
    {
    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_UINT_8:
        ((uint8_t *) address)[0u] = UINT_FIT (uint8_t, parameter->value.x);
        ((uint8_t *) address)[1u] = UINT_FIT (uint8_t, parameter->value.y);
        ((uint8_t *) address)[2u] = UINT_FIT (uint8_t, parameter->value.z);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_UINT_16:
        ((uint16_t *) address)[0u] = UINT_FIT (uint16_t, parameter->value.x);
        ((uint16_t *) address)[1u] = UINT_FIT (uint16_t, parameter->value.y);
        ((uint16_t *) address)[2u] = UINT_FIT (uint16_t, parameter->value.z);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_UINT_32:
        ((uint32_t *) address)[0u] = UINT_FIT (uint32_t, parameter->value.x);
        ((uint32_t *) address)[1u] = UINT_FIT (uint32_t, parameter->value.y);
        ((uint32_t *) address)[2u] = UINT_FIT (uint32_t, parameter->value.z);
        return true;

    default:
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "with format \"%s\" as this format is not supported.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_U3),
                 kan_rpl_meta_attribute_item_format_to_string (attribute->item_format))
        return false;
    }
}

static inline bool apply_attribute_value_u4 (const struct kan_rpl_meta_attribute_t *attribute,
                                             const struct kan_resource_material_parameter_u4_t *parameter,
                                             void *address,
                                             const char *instance_name,
                                             const char *variant_name)
{
    if (attribute->class != KAN_RPL_META_ATTRIBUTE_CLASS_VECTOR_4)
    {
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "of class \"%s\" as 4-item class was expected.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_U4),
                 kan_rpl_meta_attribute_class_to_string (attribute->class))
        return false;
    }

    switch (attribute->item_format)
    {
    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_UINT_8:
        ((uint8_t *) address)[0u] = UINT_FIT (uint8_t, parameter->value.x);
        ((uint8_t *) address)[1u] = UINT_FIT (uint8_t, parameter->value.y);
        ((uint8_t *) address)[2u] = UINT_FIT (uint8_t, parameter->value.z);
        ((uint8_t *) address)[3u] = UINT_FIT (uint8_t, parameter->value.w);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_UINT_16:
        ((uint16_t *) address)[0u] = UINT_FIT (uint16_t, parameter->value.x);
        ((uint16_t *) address)[1u] = UINT_FIT (uint16_t, parameter->value.y);
        ((uint16_t *) address)[2u] = UINT_FIT (uint16_t, parameter->value.z);
        ((uint16_t *) address)[3u] = UINT_FIT (uint16_t, parameter->value.w);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_UINT_32:
        ((uint32_t *) address)[0u] = UINT_FIT (uint32_t, parameter->value.x);
        ((uint32_t *) address)[1u] = UINT_FIT (uint32_t, parameter->value.y);
        ((uint32_t *) address)[2u] = UINT_FIT (uint32_t, parameter->value.z);
        ((uint32_t *) address)[3u] = UINT_FIT (uint32_t, parameter->value.w);
        return true;

    default:
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "with format \"%s\" as this format is not supported.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_U4),
                 kan_rpl_meta_attribute_item_format_to_string (attribute->item_format))
        return false;
    }
}

static inline bool apply_attribute_value_s1 (const struct kan_rpl_meta_attribute_t *attribute,
                                             const struct kan_resource_material_parameter_s1_t *parameter,
                                             void *address,
                                             const char *instance_name,
                                             const char *variant_name)
{
    if (attribute->class != KAN_RPL_META_ATTRIBUTE_CLASS_VECTOR_1)
    {
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "of class \"%s\" as 1-item class was expected.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_S1),
                 kan_rpl_meta_attribute_class_to_string (attribute->class))
        return false;
    }

    switch (attribute->item_format)
    {
    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_SINT_8:
        *(int8_t *) address = SINT_FIT (int8_t, parameter->value);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_SINT_16:
        *(int16_t *) address = SINT_FIT (int16_t, parameter->value);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_SINT_32:
        *(int32_t *) address = SINT_FIT (int32_t, parameter->value);
        return true;

    default:
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "with format \"%s\" as this format is not supported.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_S1),
                 kan_rpl_meta_attribute_item_format_to_string (attribute->item_format))
        return false;
    }
}

static inline bool apply_attribute_value_s2 (const struct kan_rpl_meta_attribute_t *attribute,
                                             const struct kan_resource_material_parameter_s2_t *parameter,
                                             void *address,
                                             const char *instance_name,
                                             const char *variant_name)
{
    if (attribute->class != KAN_RPL_META_ATTRIBUTE_CLASS_VECTOR_2)
    {
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "of class \"%s\" as 2-item class was expected.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_S2),
                 kan_rpl_meta_attribute_class_to_string (attribute->class))
        return false;
    }

    switch (attribute->item_format)
    {
    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_SINT_8:
        ((int8_t *) address)[0u] = SINT_FIT (int8_t, parameter->value.x);
        ((int8_t *) address)[1u] = SINT_FIT (int8_t, parameter->value.y);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_SINT_16:
        ((int16_t *) address)[0u] = SINT_FIT (int16_t, parameter->value.x);
        ((int16_t *) address)[1u] = SINT_FIT (int16_t, parameter->value.y);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_SINT_32:
        ((int32_t *) address)[0u] = SINT_FIT (int32_t, parameter->value.x);
        ((int32_t *) address)[1u] = SINT_FIT (int32_t, parameter->value.y);
        return true;

    default:
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "with format \"%s\" as this format is not supported.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_S2),
                 kan_rpl_meta_attribute_item_format_to_string (attribute->item_format))
        return false;
    }
}

static inline bool apply_attribute_value_s3 (const struct kan_rpl_meta_attribute_t *attribute,
                                             const struct kan_resource_material_parameter_s3_t *parameter,
                                             void *address,
                                             const char *instance_name,
                                             const char *variant_name)
{
    if (attribute->class != KAN_RPL_META_ATTRIBUTE_CLASS_VECTOR_3)
    {
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "of class \"%s\" as 3-item class was expected.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_S3),
                 kan_rpl_meta_attribute_class_to_string (attribute->class))
        return false;
    }

    switch (attribute->item_format)
    {
    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_SINT_8:
        ((int8_t *) address)[0u] = SINT_FIT (int8_t, parameter->value.x);
        ((int8_t *) address)[1u] = SINT_FIT (int8_t, parameter->value.y);
        ((int8_t *) address)[2u] = SINT_FIT (int8_t, parameter->value.z);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_SINT_16:
        ((int16_t *) address)[0u] = SINT_FIT (int16_t, parameter->value.x);
        ((int16_t *) address)[1u] = SINT_FIT (int16_t, parameter->value.y);
        ((int16_t *) address)[2u] = SINT_FIT (int16_t, parameter->value.z);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_SINT_32:
        ((int32_t *) address)[0u] = SINT_FIT (int32_t, parameter->value.x);
        ((int32_t *) address)[1u] = SINT_FIT (int32_t, parameter->value.y);
        ((int32_t *) address)[2u] = SINT_FIT (int32_t, parameter->value.z);
        return true;

    default:
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "with format \"%s\" as this format is not supported.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_S3),
                 kan_rpl_meta_attribute_item_format_to_string (attribute->item_format))
        return false;
    }
}

static inline bool apply_attribute_value_s4 (const struct kan_rpl_meta_attribute_t *attribute,
                                             const struct kan_resource_material_parameter_s4_t *parameter,
                                             void *address,
                                             const char *instance_name,
                                             const char *variant_name)
{
    if (attribute->class != KAN_RPL_META_ATTRIBUTE_CLASS_VECTOR_4)
    {
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "of class \"%s\" as 4-item class was expected.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_S4),
                 kan_rpl_meta_attribute_class_to_string (attribute->class))
        return false;
    }

    switch (attribute->item_format)
    {
    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_SINT_8:
        ((int8_t *) address)[0u] = SINT_FIT (int8_t, parameter->value.x);
        ((int8_t *) address)[1u] = SINT_FIT (int8_t, parameter->value.y);
        ((int8_t *) address)[2u] = SINT_FIT (int8_t, parameter->value.z);
        ((int8_t *) address)[3u] = SINT_FIT (int8_t, parameter->value.w);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_SINT_16:
        ((int16_t *) address)[0u] = SINT_FIT (int16_t, parameter->value.x);
        ((int16_t *) address)[1u] = SINT_FIT (int16_t, parameter->value.y);
        ((int16_t *) address)[2u] = SINT_FIT (int16_t, parameter->value.z);
        ((int16_t *) address)[3u] = SINT_FIT (int16_t, parameter->value.w);
        return true;

    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_SINT_32:
        ((int32_t *) address)[0u] = SINT_FIT (int32_t, parameter->value.x);
        ((int32_t *) address)[1u] = SINT_FIT (int32_t, parameter->value.y);
        ((int32_t *) address)[2u] = SINT_FIT (int32_t, parameter->value.z);
        ((int32_t *) address)[3u] = SINT_FIT (int32_t, parameter->value.w);
        return true;

    default:
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "with format \"%s\" as this format is not supported.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_S4),
                 kan_rpl_meta_attribute_item_format_to_string (attribute->item_format))
        return false;
    }
}

static inline bool apply_attribute_value_f3x3 (const struct kan_rpl_meta_attribute_t *attribute,
                                               const struct kan_resource_material_parameter_f3x3_t *parameter,
                                               void *address,
                                               const char *instance_name,
                                               const char *variant_name)
{
    if (attribute->class != KAN_RPL_META_ATTRIBUTE_CLASS_MATRIX_3X3)
    {
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "of class \"%s\" as 3x3 matrix class was expected.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_F3X3),
                 kan_rpl_meta_attribute_class_to_string (attribute->class))
        return false;
    }

    switch (attribute->item_format)
    {
    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_FLOAT_32:
        *(struct kan_float_matrix_3x3_t *) address = parameter->value;
        return true;

    default:
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "with format \"%s\" as this format is not supported.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_F3X3),
                 kan_rpl_meta_attribute_item_format_to_string (attribute->item_format))
        return false;
    }
}

static inline bool apply_attribute_value_f4x4 (const struct kan_rpl_meta_attribute_t *attribute,
                                               const struct kan_resource_material_parameter_f4x4_t *parameter,
                                               void *address,
                                               const char *instance_name,
                                               const char *variant_name)
{
    if (attribute->class != KAN_RPL_META_ATTRIBUTE_CLASS_MATRIX_4X4)
    {
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "of class \"%s\" as 4x4 class was expected.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_F4X4),
                 kan_rpl_meta_attribute_class_to_string (attribute->class))
        return false;
    }

    switch (attribute->item_format)
    {
    case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_FLOAT_32:
        *(struct kan_float_matrix_4x4_t *) address = parameter->value;
        return true;

    default:
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" of type \"%s\" to attribute "
                 "with format \"%s\" as this format is not supported.",
                 instance_name, variant_name, parameter->name,
                 kan_rpl_meta_variable_type_to_string (KAN_RPL_META_VARIABLE_TYPE_F4X4),
                 kan_rpl_meta_attribute_item_format_to_string (attribute->item_format))
        return false;
    }
}

static inline bool apply_attribute_value_color_linear (
    const struct kan_rpl_meta_attribute_t *attribute,
    const struct kan_resource_material_parameter_color_linear_t *parameter,
    void *address,
    const char *instance_name,
    const char *variant_name)
{
    struct kan_resource_material_parameter_f4_t transitive_parameter = {
        .name = parameter->name,
        .value = color_linear_converter (parameter->value),
    };

    return apply_attribute_value_f4 (attribute, &transitive_parameter, address, instance_name, variant_name);
}

static inline bool apply_attribute_value_color_srgb (
    const struct kan_rpl_meta_attribute_t *attribute,
    const struct kan_resource_material_parameter_color_srgb_t *parameter,
    void *address,
    const char *instance_name,
    const char *variant_name)
{
    struct kan_resource_material_parameter_f4_t transitive_parameter = {
        .name = parameter->name,
        .value = color_srgb_converter (parameter->value),
    };

    return apply_attribute_value_f4 (attribute, &transitive_parameter, address, instance_name, variant_name);
}

#undef F1_TO_UNORM
#undef F1_TO_SNORM
#undef UINT_FIT
#undef SINT_FIT

static bool apply_samplers_array (struct kan_resource_build_rule_context_t *context,
                                  const struct kan_resource_material_t *material,
                                  const struct kan_dynamic_array_t *array)
{
    struct kan_resource_material_instance_t *output = context->primary_output;
    bool successful = true;

    for (kan_loop_size_t input_index = 0u; input_index < array->size; ++input_index)
    {
        const struct kan_resource_material_sampler_t *input_sampler =
            &((struct kan_resource_material_sampler_t *) array->data)[input_index];
        bool found = false;

        for (kan_loop_size_t meta_index = 0u; meta_index < material->set_material.samplers.size; ++meta_index)
        {
            const struct kan_rpl_meta_sampler_t *meta_sampler =
                &((struct kan_rpl_meta_sampler_t *) material->set_material.samplers.data)[meta_index];

            if (meta_sampler->name != input_sampler->name)
            {
                continue;
            }

            found = true;
            struct kan_resource_sampler_binding_t *output_sampler =
                &((struct kan_resource_sampler_binding_t *) output->samplers.data)[meta_index];
            output_sampler->sampler = input_sampler->sampler;
            break;
        }

        if (!found)
        {
            KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                     "Material instance \"%s\" cannot set sampler \"%s\" as it is not found across samplers.",
                     context->primary_name, input_sampler->name)
            successful = false;
        }
    }

    return successful;
}

static bool apply_images_array (struct kan_resource_build_rule_context_t *context,
                                const struct kan_resource_material_t *material,
                                const struct kan_dynamic_array_t *array)
{
    struct kan_resource_material_instance_t *output = context->primary_output;
    bool successful = true;

    for (kan_loop_size_t input_index = 0u; input_index < array->size; ++input_index)
    {
        const struct kan_resource_material_image_t *input_image =
            &((struct kan_resource_material_image_t *) array->data)[input_index];
        bool found = false;

        for (kan_loop_size_t meta_index = 0u; meta_index < material->set_material.images.size; ++meta_index)
        {
            const struct kan_rpl_meta_image_t *meta_image =
                &((struct kan_rpl_meta_image_t *) material->set_material.images.data)[meta_index];

            if (meta_image->name != input_image->name)
            {
                continue;
            }

            found = true;
            struct kan_resource_image_binding_t *output_image =
                &((struct kan_resource_image_binding_t *) output->images.data)[meta_index];
            output_image->texture = input_image->texture;
            break;
        }

        if (!found)
        {
            KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                     "Material instance \"%s\" cannot set image \"%s\" as it is not found across images.",
                     context->primary_name, input_image->name)
            successful = false;
        }
    }

    return successful;
}

static bool apply_variants_array (struct kan_resource_build_rule_context_t *context,
                                  const struct kan_resource_material_t *material,
                                  const struct kan_dynamic_array_t *array)
{
    const struct kan_resource_material_instance_raw_t *input = context->primary_input;
    if (array->size > 0u && material->has_instanced_attribute_source)
    {
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" has variants, but material \"%s\" has no instanced parameters, therefore "
                 "variants are not possible here.",
                 context->primary_name, input->material)
        return false;
    }

    struct kan_resource_material_instance_t *output = context->primary_output;
    bool successful = true;

    for (kan_loop_size_t input_index = 0u; input_index < array->size; ++input_index)
    {
        const struct kan_resource_material_variant_raw_t *input_variant =
            &((struct kan_resource_material_variant_raw_t *) array->data)[input_index];
        struct kan_resource_material_variant_t *output_variant = NULL;

        for (kan_loop_size_t existent_index = 0u; existent_index < output->variants.size; ++existent_index)
        {
            struct kan_resource_material_variant_t *existent =
                &((struct kan_resource_material_variant_t *) output->variants.data)[existent_index];

            if (existent->name == input_variant->name)
            {
                output_variant = existent;
                break;
            }
        }

        if (!output_variant)
        {
            output_variant = kan_dynamic_array_add_last (&output->variants);
            if (!output_variant)
            {
                kan_dynamic_array_set_capacity (&output->variants, KAN_MAX (1u, output->variants.capacity * 2u));
                output_variant = kan_dynamic_array_add_last (&output->variants);
            }

            kan_allocation_group_stack_push (output->variants.allocation_group);
            kan_resource_material_variant_init (output_variant);
            kan_allocation_group_stack_pop ();

            output_variant->name = input_variant->name;
            kan_dynamic_array_set_capacity (&output_variant->instanced_data,
                                            material->instanced_attribute_source.block_size);
            output_variant->instanced_data.size = output_variant->instanced_data.capacity;
        }

#define PROCESS_TYPE(NAME)                                                                                             \
    for (kan_loop_size_t parameter_index = 0u; parameter_index < input_variant->parameters.NAME.size;                  \
         ++parameter_index)                                                                                            \
    {                                                                                                                  \
        const struct kan_resource_material_parameter_##NAME##_t *parameter =                                           \
            &((struct kan_resource_material_parameter_##NAME##_t *)                                                    \
                  input_variant->parameters.NAME.data)[parameter_index];                                               \
        bool found = false;                                                                                            \
                                                                                                                       \
        for (kan_loop_size_t attribute_index = 0u;                                                                     \
             attribute_index < material->instanced_attribute_source.attributes.size; ++attribute_index)                \
        {                                                                                                              \
            const struct kan_rpl_meta_attribute_t *attribute =                                                         \
                &((struct kan_rpl_meta_attribute_t *)                                                                  \
                      material->instanced_attribute_source.attributes.data)[attribute_index];                          \
                                                                                                                       \
            if (attribute->name != parameter->name)                                                                    \
            {                                                                                                          \
                continue;                                                                                              \
            }                                                                                                          \
                                                                                                                       \
            found = true;                                                                                              \
            uint8_t *address = output_variant->instanced_data.data + attribute->offset;                                \
            successful &= apply_attribute_value_##NAME (attribute, parameter, address, context->primary_name,          \
                                                        input_variant->name);                                          \
            break;                                                                                                     \
        }                                                                                                              \
                                                                                                                       \
        if (!found)                                                                                                    \
        {                                                                                                              \
            KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,                                      \
                     "Material instance \"%s\" variant \"%s\" cannot set parameter \"%s\" as it is not found "         \
                     "across instanced attributes.",                                                                   \
                     context->primary_name, input_variant->name, parameter->name)                                      \
            successful = false;                                                                                        \
        }                                                                                                              \
    }

        PROCESS_TYPE (f1)
        PROCESS_TYPE (f2)
        PROCESS_TYPE (f3)
        PROCESS_TYPE (f4)

        PROCESS_TYPE (u1)
        PROCESS_TYPE (u2)
        PROCESS_TYPE (u3)
        PROCESS_TYPE (u4)

        PROCESS_TYPE (s1)
        PROCESS_TYPE (s2)
        PROCESS_TYPE (s3)
        PROCESS_TYPE (s4)

        PROCESS_TYPE (f3x3)
        PROCESS_TYPE (f4x4)

        PROCESS_TYPE (color_linear)
        PROCESS_TYPE (color_srgb)
#undef PROCESS_TYPE
    }

    return successful;
}

static enum kan_resource_build_rule_result_t material_instance_build (struct kan_resource_build_rule_context_t *context)
{
    kan_static_interned_ids_ensure_initialized ();
    const struct kan_resource_material_instance_raw_t *input = context->primary_input;
    struct kan_resource_material_instance_t *output = context->primary_output;
    output->material = input->material;

    const struct kan_resource_material_t *material = NULL;
    const struct kan_resource_material_instance_t *parent = NULL;

    struct kan_resource_build_rule_secondary_node_t *secondary_node = context->secondary_input_first;
    while (secondary_node)
    {
        if (secondary_node->type == KAN_STATIC_INTERNED_ID_GET (kan_resource_material_t) &&
            secondary_node->name == input->material)
        {
            material = secondary_node->data;
        }
        else if (secondary_node->type == KAN_STATIC_INTERNED_ID_GET (kan_resource_material_instance_t) &&
                 secondary_node->name == input->parent)
        {
            parent = secondary_node->data;
        }

        secondary_node = secondary_node->next;
    }

    // Should never happen.
    KAN_ASSERT (material)
    KAN_ASSERT (parent || !input->parent)

    if (parent && input->material != parent->material)
    {
        KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                 "Material instance \"%s\" uses material \"%s\" while its parent \"%s\" uses material \"%s\". Material "
                 "instance must use the same material as parent.",
                 context->primary_name, input->material, input->parent, parent->material)
        return KAN_RESOURCE_BUILD_RULE_FAILURE;
    }

    if (parent)
    {
        kan_dynamic_array_set_capacity (&output->buffers, parent->buffers.size);
        for (kan_loop_size_t index = 0u; index < parent->buffers.size; ++index)
        {
            const struct kan_resource_buffer_binding_t *source =
                &((struct kan_resource_buffer_binding_t *) parent->buffers.data)[index];

            struct kan_resource_buffer_binding_t *target = kan_dynamic_array_add_last (&output->buffers);
            kan_allocation_group_stack_push (output->buffers.allocation_group);
            kan_resource_buffer_binding_init (target);
            kan_allocation_group_stack_pop ();

            target->binding = source->binding;
            target->type = source->type;
            kan_dynamic_array_set_capacity (&target->data, source->data.size);
            target->data.size = source->data.size;
            memcpy (target->data.data, source->data.data, source->data.size);
        }

        kan_dynamic_array_set_capacity (&output->samplers, parent->samplers.size);
        output->samplers.size = parent->samplers.size;
        memcpy (output->samplers.data, parent->samplers.data,
                sizeof (struct kan_resource_sampler_binding_t) * parent->samplers.size);

        kan_dynamic_array_set_capacity (&output->images, parent->images.size);
        output->images.size = parent->images.size;
        memcpy (output->images.data, parent->images.data,
                sizeof (struct kan_resource_image_binding_t) * parent->images.size);

        kan_dynamic_array_set_capacity (&output->variants, parent->variants.size);
        for (kan_loop_size_t index = 0u; index < parent->variants.size; ++index)
        {
            const struct kan_resource_material_variant_t *source =
                &((struct kan_resource_material_variant_t *) parent->variants.data)[index];

            struct kan_resource_material_variant_t *target = kan_dynamic_array_add_last (&output->variants);
            kan_allocation_group_stack_push (output->variants.allocation_group);
            kan_resource_material_variant_init (target);
            kan_allocation_group_stack_pop ();

            target->name = source->name;
            kan_dynamic_array_set_capacity (&target->instanced_data, source->instanced_data.size);
            target->instanced_data.size = source->instanced_data.size;
            memcpy (target->instanced_data.data, source->instanced_data.data, source->instanced_data.size);
        }
    }
    else
    {
        kan_dynamic_array_set_capacity (&output->buffers, material->set_material.buffers.size);
        for (kan_loop_size_t index = 0u; index < material->set_material.buffers.size; ++index)
        {
            const struct kan_rpl_meta_buffer_t *source =
                &((struct kan_rpl_meta_buffer_t *) material->set_material.buffers.data)[index];
            struct kan_resource_buffer_binding_t *target = kan_dynamic_array_add_last (&output->buffers);

            kan_allocation_group_stack_push (output->buffers.allocation_group);
            kan_resource_buffer_binding_init (target);
            kan_allocation_group_stack_pop ();

            target->binding = source->binding;
            target->type = source->type;
            kan_dynamic_array_set_capacity (&target->data, source->main_size);
            target->data.size = source->main_size;
            memset (target->data.data, 0, source->main_size);
        }

        kan_dynamic_array_set_capacity (&output->samplers, material->set_material.samplers.size);
        for (kan_loop_size_t index = 0u; index < material->set_material.samplers.size; ++index)
        {
            const struct kan_rpl_meta_sampler_t *source =
                &((struct kan_rpl_meta_sampler_t *) material->set_material.samplers.data)[index];
            struct kan_resource_sampler_binding_t *target = kan_dynamic_array_add_last (&output->samplers);

            target->binding = source->binding;
            target->sampler.min_filter = KAN_RENDER_FILTER_MODE_NEAREST;
            target->sampler.mag_filter = KAN_RENDER_FILTER_MODE_NEAREST;
            target->sampler.mip_map_mode = KAN_RENDER_MIP_MAP_MODE_NEAREST;
            target->sampler.address_mode_u = KAN_RENDER_ADDRESS_MODE_REPEAT;
            target->sampler.address_mode_v = KAN_RENDER_ADDRESS_MODE_REPEAT;
            target->sampler.address_mode_w = KAN_RENDER_ADDRESS_MODE_REPEAT;
            target->sampler.depth_compare_enabled = false;
            target->sampler.depth_compare = KAN_RENDER_COMPARE_OPERATION_NEVER;
            target->sampler.anisotropy_enabled = false;
            target->sampler.anisotropy_max = 1.0f;
        }

        kan_dynamic_array_set_capacity (&output->images, material->set_material.images.size);
        for (kan_loop_size_t index = 0u; index < material->set_material.images.size; ++index)
        {
            const struct kan_rpl_meta_image_t *source =
                &((struct kan_rpl_meta_image_t *) material->set_material.images.data)[index];
            struct kan_resource_image_binding_t *target = kan_dynamic_array_add_last (&output->images);

            target->binding = source->binding;
            target->texture = NULL;
        }
    }

    bool successful = true;
#define APPLY_STAGE(FUNCTION, FIELD)                                                                                   \
    secondary_node = context->secondary_input_first;                                                                   \
                                                                                                                       \
    while (secondary_node)                                                                                             \
    {                                                                                                                  \
        if (secondary_node->type == KAN_STATIC_INTERNED_ID_GET (kan_resource_material_instance_mixin_t))               \
        {                                                                                                              \
            const struct kan_resource_material_instance_mixin_t *mixin = secondary_node->data;                         \
            successful &= FUNCTION (context, material, &mixin->FIELD);                                                 \
        }                                                                                                              \
                                                                                                                       \
        secondary_node = secondary_node->next;                                                                         \
    }                                                                                                                  \
                                                                                                                       \
    successful &= FUNCTION (context, material, &input->FIELD)

    APPLY_STAGE (apply_parameters_container, parameters);
    APPLY_STAGE (apply_tail_set_array, tail_set);
    APPLY_STAGE (apply_tail_append_array, tail_append);
    APPLY_STAGE (apply_samplers_array, samplers);
    APPLY_STAGE (apply_images_array, images);
    APPLY_STAGE (apply_variants_array, variants);
#undef APPLY_STAGE

    // Check if texture slots are filled for better error reporting.
    // We should not have NULL texture references and they will be reported anyway,
    // but reporting them here makes errors more readable and fixable.

    for (kan_loop_size_t index = 0u; index < output->images.size; ++index)
    {
        const struct kan_resource_image_binding_t *binding =
            &((struct kan_resource_image_binding_t *) output->images.data)[index];

        if (!binding->texture)
        {
            const struct kan_rpl_meta_image_t *meta =
                &((struct kan_rpl_meta_image_t *) material->set_material.images.data)[index];

            KAN_LOG (resource_render_foundation_material_instance, KAN_LOG_ERROR,
                     "Material instance \"%s\" did not set any texture to slot \"%s\" which is considered an error.",
                     context->primary_name, meta->name)
            successful = false;
        }
    }

    return successful ? KAN_RESOURCE_BUILD_RULE_SUCCESS : KAN_RESOURCE_BUILD_RULE_FAILURE;
}
