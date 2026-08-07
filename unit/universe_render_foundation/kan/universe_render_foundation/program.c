#define _CRT_SECURE_NO_WARNINGS __CUSHION_PRESERVE__

#include <string.h>

#include <kan/cpu_profiler/markup.h>
#include <kan/log/logging.h>
#include <kan/resource_render_foundation/material.h>
#include <kan/resource_render_foundation/material_instance.h>
#include <kan/resource_render_foundation/render_pass.h>
#include <kan/universe/macro.h>
#include <kan/universe_render_foundation/program.h>
#include <kan/universe_render_foundation/render_graph.h>
#include <kan/universe_render_foundation/texture.h>
#include <kan/universe_resource_provider/provider.h>

KAN_LOG_DEFINE_CATEGORY (render_foundation_program);
KAN_USE_STATIC_INTERNED_IDS
KAN_USE_STATIC_CPU_SECTIONS

KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (render_foundation_program_core_management)
KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (render_foundation_material_instance_management)
UNIVERSE_RENDER_FOUNDATION_API KAN_UM_MUTATOR_GROUP_META (render_foundation_program_management,
                                                          KAN_RENDER_FOUNDATION_PROGRAM_MANAGEMENT_MUTATOR_GROUP);

static kan_render_pipeline_parameter_set_layout_t construct_parameter_set_layout_from_meta (
    kan_render_context_t render_context,
    const struct kan_rpl_meta_set_bindings_t *meta,
    kan_interned_string_t tracking_name,
    kan_allocation_group_t temporary_allocation_group)
{
    struct kan_render_parameter_binding_description_t
        bindings_static[KAN_UNIVERSE_RENDER_FOUNDATION_BINDINGS_MAX_STATIC];
    struct kan_render_parameter_binding_description_t *bindings = bindings_static;
    const kan_instance_size_t bindings_count = meta->buffers.size + meta->samplers.size + meta->images.size;

    if (bindings_count > KAN_UNIVERSE_RENDER_FOUNDATION_BINDINGS_MAX_STATIC)
    {
        bindings = kan_allocate_general (temporary_allocation_group,
                                         sizeof (struct kan_render_parameter_binding_description_t) * bindings_count,
                                         alignof (struct kan_render_parameter_binding_description_t));
    }

    CUSHION_DEFER
    {
        if (bindings != bindings_static)
        {
            kan_free_general (temporary_allocation_group, bindings,
                              sizeof (struct kan_render_parameter_binding_description_t) * bindings_count);
        }
    }

    kan_instance_size_t binding_output_index = 0u;
    for (kan_memory_size_t index = 0u; index < meta->buffers.size; ++index, ++binding_output_index)
    {
        struct kan_rpl_meta_buffer_t *buffer = &((struct kan_rpl_meta_buffer_t *) meta->buffers.data)[index];
        enum kan_render_parameter_binding_type_t binding_type = KAN_RENDER_PARAMETER_BINDING_TYPE_UNIFORM_BUFFER;

        switch (buffer->type)
        {
        case KAN_RPL_BUFFER_TYPE_UNIFORM:
            binding_type = KAN_RENDER_PARAMETER_BINDING_TYPE_UNIFORM_BUFFER;
            break;

        case KAN_RPL_BUFFER_TYPE_READ_ONLY_STORAGE:
            binding_type = KAN_RENDER_PARAMETER_BINDING_TYPE_STORAGE_BUFFER;
            break;

        case KAN_RPL_BUFFER_TYPE_PUSH_CONSTANT:
            // Should not be here.
            KAN_ASSERT (false)
            break;
        }

        bindings[binding_output_index] = (struct kan_render_parameter_binding_description_t) {
            .binding = buffer->binding,
            .type = binding_type,
            .descriptor_count = 1u,
            .used_stage_mask = (1u << KAN_RENDER_STAGE_GRAPHICS_VERTEX) | (1u << KAN_RENDER_STAGE_GRAPHICS_FRAGMENT),
        };
    }

    for (kan_memory_size_t index = 0u; index < meta->samplers.size; ++index, ++binding_output_index)
    {
        struct kan_rpl_meta_sampler_t *sampler = &((struct kan_rpl_meta_sampler_t *) meta->samplers.data)[index];
        bindings[binding_output_index] = (struct kan_render_parameter_binding_description_t) {
            .binding = sampler->binding,
            .type = KAN_RENDER_PARAMETER_BINDING_TYPE_SAMPLER,
            .descriptor_count = 1u,
            .used_stage_mask = (1u << KAN_RENDER_STAGE_GRAPHICS_VERTEX) | (1u << KAN_RENDER_STAGE_GRAPHICS_FRAGMENT),
        };
    }

    for (kan_memory_size_t index = 0u; index < meta->images.size; ++index, ++binding_output_index)
    {
        struct kan_rpl_meta_image_t *image = &((struct kan_rpl_meta_image_t *) meta->images.data)[index];
        bindings[binding_output_index] = (struct kan_render_parameter_binding_description_t) {
            .binding = image->binding,
            .type = KAN_RENDER_PARAMETER_BINDING_TYPE_IMAGE,
            .descriptor_count = image->image_array_size,
            .used_stage_mask = (1u << KAN_RENDER_STAGE_GRAPHICS_VERTEX) | (1u << KAN_RENDER_STAGE_GRAPHICS_FRAGMENT),
        };
    }

    struct kan_render_pipeline_parameter_set_layout_description_t description = {
        .bindings_count = binding_output_index,
        .bindings = bindings,
        .tracking_name = tracking_name,
    };

    return kan_render_pipeline_parameter_set_layout_create (render_context, &description);
}

struct render_foundation_material_instance_quality_on_insert_event_t
{
    kan_interned_string_t material_instance_name;
};

KAN_REFLECTION_STRUCT_META (kan_render_material_instance_quality_t)
UNIVERSE_RENDER_FOUNDATION_API struct kan_repository_meta_automatic_on_insert_event_t
    render_foundation_material_instance_quality_on_insert_event = {
        .event_type = "render_foundation_material_instance_quality_on_insert_event_t",
        .copy_outs_count = 1u,
        .copy_outs =
            (struct kan_repository_copy_out_t[]) {
                {
                    .source_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"name"}},
                    .target_path = {.reflection_path_length = 1u,
                                    .reflection_path = (const char *[]) {"material_instance_name"}},
                },
            },
};

struct render_foundation_material_instance_quality_on_delete_event_t
{
    kan_interned_string_t material_instance_name;
};

KAN_REFLECTION_STRUCT_META (kan_render_material_instance_quality_t)
UNIVERSE_RENDER_FOUNDATION_API struct kan_repository_meta_automatic_on_delete_event_t
    render_foundation_material_instance_quality_on_delete_event = {
        .event_type = "render_foundation_material_instance_quality_on_delete_event_t",
        .copy_outs_count = 1u,
        .copy_outs =
            (struct kan_repository_copy_out_t[]) {
                {
                    .source_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"name"}},
                    .target_path = {.reflection_path_length = 1u,
                                    .reflection_path = (const char *[]) {"material_instance_name"}},
                },
            },
};

struct render_foundation_material_instance_texture_usage_t
{
    kan_immutable kan_interned_string_t material_instance_name;
    kan_immutable kan_interned_string_t texture_name;
    kan_immutable kan_instance_size_t binding;
    kan_render_texture_quality_id_t quality_id;
    kan_render_image_t bound_image;
};

KAN_REFLECTION_STRUCT_META (render_foundation_material_instance_texture_usage_t)
UNIVERSE_RENDER_FOUNDATION_API struct kan_repository_meta_automatic_cascade_deletion_t
    render_foundation_material_instance_texture_usage_cascade_deletion = {
        .parent_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"quality_id"}},
        .child_type_name = "kan_render_texture_quality_t",
        .child_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"quality_id"}},
};

KAN_REFLECTION_STRUCT_META (kan_render_material_instance_loaded_t)
UNIVERSE_RENDER_FOUNDATION_API struct kan_repository_meta_automatic_cascade_deletion_t
    render_foundation_material_instance_texture_cascade_deletion = {
        .parent_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"name"}},
        .child_type_name = "render_foundation_material_instance_texture_usage_t",
        .child_key_path = {.reflection_path_length = 1u,
                           .reflection_path = (const char *[]) {"material_instance_name"}},
};

struct render_foundation_program_core_management_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (render_foundation_program_core_management)
    KAN_UM_BIND_STATE (render_foundation_program_core_management, state)
    kan_allocation_group_t temporary_allocation_group;
};

UNIVERSE_RENDER_FOUNDATION_API KAN_UM_MUTATOR_DEPLOY (render_foundation_program_core_management)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_cpu_static_sections_ensure_initialized ();
    state->temporary_allocation_group = kan_allocation_group_get_child (kan_allocation_group_stack_get (), "temporary");

    kan_workflow_graph_node_depend_on (workflow_node, KAN_RESOURCE_PROVIDER_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_RENDER_FOUNDATION_FRAME_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_RENDER_FOUNDATION_PROGRAM_MANAGEMENT_BEGIN_CHECKPOINT);
}

static void load_pass (struct render_foundation_program_core_management_state_t *state,
                       const struct kan_render_context_singleton_t *render_context,
                       const struct kan_resource_render_pass_t *resource,
                       struct kan_render_foundation_pass_loaded_t *pass)
{
    KAN_CPU_SCOPED_STATIC_SECTION (load_pass)
    if (KAN_HANDLE_IS_VALID (pass->pass))
    {
        kan_render_pass_destroy (pass->pass);
        pass->pass = KAN_HANDLE_SET_INVALID (kan_render_pass_t);
    }

    for (kan_memory_size_t variant_index = 0u; variant_index < pass->variants.size; ++variant_index)
    {
        kan_render_foundation_pass_variant_shutdown (
            &((struct kan_render_foundation_pass_variant_t *) pass->variants.data)[variant_index]);
    }

    pass->attachments.size = 0u;
    pass->variants.size = 0u;

    pass->type = resource->type;
    struct kan_render_pass_description_t description = {
        .type = resource->type,
        .attachments_count = resource->attachments.size,
        .attachments = (struct kan_render_pass_attachment_t *) resource->attachments.data,
        .tracking_name = pass->name,
    };

    pass->pass = kan_render_pass_create (render_context->render_context, &description);
    if (!KAN_HANDLE_IS_VALID (pass->pass))
    {
        KAN_LOG (render_foundation_program, KAN_LOG_ERROR, "Failed to create render pass from resources \"%s\".",
                 pass->name)

        // Event needs to be sent even on failure as initial pass was destroyed.
        KAN_UMO_EVENT_INSERT_INIT (kan_render_foundation_pass_updated_event_t) {.name = pass->name};
        return;
    }

    kan_dynamic_array_set_capacity (&pass->attachments, resource->attachments.size);
    for (kan_memory_size_t index = 0u; index < (kan_memory_size_t) resource->attachments.size; ++index)
    {
        struct kan_render_foundation_pass_attachment_t *output = kan_dynamic_array_add_last (&pass->attachments);
        KAN_ASSERT (output)

        const struct kan_render_pass_attachment_t *input =
            &((struct kan_render_pass_attachment_t *) resource->attachments.data)[index];

        output->type = input->type;
        output->format = input->format;
    }

    kan_dynamic_array_set_capacity (&pass->variants, resource->variants.size);
    for (kan_memory_size_t index = 0u; index < (kan_memory_size_t) resource->variants.size; ++index)
    {
        struct kan_render_foundation_pass_variant_t *output = kan_dynamic_array_add_last (&pass->variants);
        KAN_ASSERT (output)

        kan_allocation_group_stack_push (pass->variants.allocation_group);
        kan_render_foundation_pass_variant_init (output);
        kan_allocation_group_stack_pop ();

        const struct kan_resource_render_pass_variant_t *input =
            &((struct kan_resource_render_pass_variant_t *) resource->variants.data)[index];

        output->name = input->name;
        kan_rpl_meta_set_bindings_shutdown (&output->pass_parameter_set_bindings);
        kan_rpl_meta_set_bindings_init_copy (&output->pass_parameter_set_bindings, &input->pass_set_bindings);

        char tracking_name_buffer[KAN_UNIVERSE_RENDER_FOUNDATION_NAME_BUFFER_LENGTH];
        snprintf (tracking_name_buffer, sizeof (tracking_name_buffer), "%s::pass_set::%s", pass->name, output->name);

        output->pass_parameter_set_layout = construct_parameter_set_layout_from_meta (
            render_context->render_context, &output->pass_parameter_set_bindings,
            kan_string_intern (tracking_name_buffer), state->temporary_allocation_group);

        if (!KAN_HANDLE_IS_VALID (output->pass_parameter_set_layout))
        {
            KAN_LOG (render_foundation_program, KAN_LOG_ERROR,
                     "Failed to create render pass \"%s\" parameter set layout for variant \"%s\".", pass->name,
                     output->name)
        }
    }

    KAN_UMO_EVENT_INSERT_INIT (kan_render_foundation_pass_updated_event_t) {.name = pass->name};
}

static void add_attributes_from_source (const struct kan_rpl_meta_attribute_source_t *source,
                                        struct kan_render_attribute_description_t *attributes,
                                        kan_instance_size_t *attribute_output_index_pointer)
{
    for (kan_memory_size_t attribute_index = 0u; attribute_index < source->attributes.size;
         ++attribute_index, ++*attribute_output_index_pointer)
    {
        struct kan_rpl_meta_attribute_t *attribute =
            &((struct kan_rpl_meta_attribute_t *) source->attributes.data)[attribute_index];

        enum kan_render_attribute_class_t class = KAN_RENDER_ATTRIBUTE_CLASS_VECTOR_1;
        enum kan_render_attribute_item_format_t format = KAN_RENDER_ATTRIBUTE_ITEM_FORMAT_FLOAT_32;

        switch (attribute->class)
        {
        case KAN_RPL_META_ATTRIBUTE_CLASS_VECTOR_1:
            class = KAN_RENDER_ATTRIBUTE_CLASS_VECTOR_1;
            break;

        case KAN_RPL_META_ATTRIBUTE_CLASS_VECTOR_2:
            class = KAN_RENDER_ATTRIBUTE_CLASS_VECTOR_2;
            break;

        case KAN_RPL_META_ATTRIBUTE_CLASS_VECTOR_3:
            class = KAN_RENDER_ATTRIBUTE_CLASS_VECTOR_3;
            break;

        case KAN_RPL_META_ATTRIBUTE_CLASS_VECTOR_4:
            class = KAN_RENDER_ATTRIBUTE_CLASS_VECTOR_4;
            break;

        case KAN_RPL_META_ATTRIBUTE_CLASS_MATRIX_3X3:
            class = KAN_RENDER_ATTRIBUTE_CLASS_MATRIX_3_3;
            break;

        case KAN_RPL_META_ATTRIBUTE_CLASS_MATRIX_4X4:
            class = KAN_RENDER_ATTRIBUTE_CLASS_MATRIX_4_4;
            break;
        }

        switch (attribute->item_format)
        {
        case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_FLOAT_16:
            format = KAN_RENDER_ATTRIBUTE_ITEM_FORMAT_FLOAT_16;
            break;

        case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_FLOAT_32:
            format = KAN_RENDER_ATTRIBUTE_ITEM_FORMAT_FLOAT_32;
            break;

        case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_UNORM_8:
            format = KAN_RENDER_ATTRIBUTE_ITEM_FORMAT_FLOAT_UNORM_8;
            break;

        case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_UNORM_16:
            format = KAN_RENDER_ATTRIBUTE_ITEM_FORMAT_FLOAT_UNORM_16;
            break;

        case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_SNORM_8:
            format = KAN_RENDER_ATTRIBUTE_ITEM_FORMAT_FLOAT_SNORM_8;
            break;

        case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_SNORM_16:
            format = KAN_RENDER_ATTRIBUTE_ITEM_FORMAT_FLOAT_SNORM_16;
            break;

        case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_UINT_8:
            format = KAN_RENDER_ATTRIBUTE_ITEM_FORMAT_FLOAT_UINT_8;
            break;

        case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_UINT_16:
            format = KAN_RENDER_ATTRIBUTE_ITEM_FORMAT_FLOAT_UINT_16;
            break;

        case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_UINT_32:
            format = KAN_RENDER_ATTRIBUTE_ITEM_FORMAT_FLOAT_UINT_32;
            break;

        case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_SINT_8:
            format = KAN_RENDER_ATTRIBUTE_ITEM_FORMAT_FLOAT_SINT_8;
            break;

        case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_SINT_16:
            format = KAN_RENDER_ATTRIBUTE_ITEM_FORMAT_FLOAT_SINT_16;
            break;

        case KAN_RPL_META_ATTRIBUTE_ITEM_FORMAT_SINT_32:
            format = KAN_RENDER_ATTRIBUTE_ITEM_FORMAT_FLOAT_SINT_32;
            break;
        }

        attributes[*attribute_output_index_pointer] = (struct kan_render_attribute_description_t) {
            .binding = source->binding,
            .location = attribute->location,
            .offset = attribute->offset,
            .class = class,
            .item_format = format,
        };
    }
}

static inline enum kan_render_compare_operation_t convert_compare_operation (enum kan_rpl_compare_operation_t operation)
{
    switch (operation)
    {
    case KAN_RPL_COMPARE_OPERATION_NEVER:
        return KAN_RENDER_COMPARE_OPERATION_NEVER;

    case KAN_RPL_COMPARE_OPERATION_ALWAYS:
        return KAN_RENDER_COMPARE_OPERATION_ALWAYS;

    case KAN_RPL_COMPARE_OPERATION_EQUAL:
        return KAN_RENDER_COMPARE_OPERATION_EQUAL;

    case KAN_RPL_COMPARE_OPERATION_NOT_EQUAL:
        return KAN_RENDER_COMPARE_OPERATION_NOT_EQUAL;

    case KAN_RPL_COMPARE_OPERATION_LESS:
        return KAN_RENDER_COMPARE_OPERATION_LESS;

    case KAN_RPL_COMPARE_OPERATION_LESS_OR_EQUAL:
        return KAN_RENDER_COMPARE_OPERATION_LESS_OR_EQUAL;

    case KAN_RPL_COMPARE_OPERATION_GREATER:
        return KAN_RENDER_COMPARE_OPERATION_GREATER;

    case KAN_RPL_COMPARE_OPERATION_GREATER_OR_EQUAL:
        return KAN_RENDER_COMPARE_OPERATION_GREATER_OR_EQUAL;
    }

    KAN_ASSERT (false)
    return KAN_RENDER_COMPARE_OPERATION_NEVER;
}

static inline enum kan_render_stencil_operation_t convert_stencil_operation (enum kan_rpl_stencil_operation_t operation)
{
    switch (operation)
    {
    case KAN_RPL_STENCIL_OPERATION_KEEP:
        return KAN_RENDER_STENCIL_OPERATION_KEEP;

    case KAN_RPL_STENCIL_OPERATION_ZERO:
        return KAN_RENDER_STENCIL_OPERATION_ZERO;

    case KAN_RPL_STENCIL_OPERATION_REPLACE:
        return KAN_RENDER_STENCIL_OPERATION_REPLACE;

    case KAN_RPL_STENCIL_OPERATION_INCREMENT_AND_CLAMP:
        return KAN_RENDER_STENCIL_OPERATION_INCREMENT_AND_CLAMP;

    case KAN_RPL_STENCIL_OPERATION_DECREMENT_AND_CLAMP:
        return KAN_RENDER_STENCIL_OPERATION_DECREMENT_AND_CLAMP;

    case KAN_RPL_STENCIL_OPERATION_INVERT:
        return KAN_RENDER_STENCIL_OPERATION_INVERT;

    case KAN_RPL_STENCIL_OPERATION_INCREMENT_AND_WRAP:
        return KAN_RENDER_STENCIL_OPERATION_INCREMENT_AND_WRAP;

    case KAN_RPL_STENCIL_OPERATION_DECREMENT_AND_WRAP:
        return KAN_RENDER_STENCIL_OPERATION_DECREMENT_AND_WRAP;
    }

    KAN_ASSERT (false)
    return KAN_RENDER_STENCIL_OPERATION_KEEP;
}

static inline enum kan_render_blend_factor_t convert_blend_factor (enum kan_rpl_blend_factor_t blend_factor)
{
    switch (blend_factor)
    {
    case KAN_RPL_BLEND_FACTOR_ZERO:
        return KAN_RENDER_BLEND_FACTOR_ZERO;

    case KAN_RPL_BLEND_FACTOR_ONE:
        return KAN_RENDER_BLEND_FACTOR_ONE;

    case KAN_RPL_BLEND_FACTOR_SOURCE_COLOR:
        return KAN_RENDER_BLEND_FACTOR_SOURCE_COLOR;

    case KAN_RPL_BLEND_FACTOR_ONE_MINUS_SOURCE_COLOR:
        return KAN_RENDER_BLEND_FACTOR_ONE_MINUS_SOURCE_COLOR;

    case KAN_RPL_BLEND_FACTOR_DESTINATION_COLOR:
        return KAN_RENDER_BLEND_FACTOR_DESTINATION_COLOR;

    case KAN_RPL_BLEND_FACTOR_ONE_MINUS_DESTINATION_COLOR:
        return KAN_RENDER_BLEND_FACTOR_ONE_MINUS_DESTINATION_COLOR;

    case KAN_RPL_BLEND_FACTOR_SOURCE_ALPHA:
        return KAN_RENDER_BLEND_FACTOR_SOURCE_ALPHA;

    case KAN_RPL_BLEND_FACTOR_ONE_MINUS_SOURCE_ALPHA:
        return KAN_RENDER_BLEND_FACTOR_ONE_MINUS_SOURCE_ALPHA;

    case KAN_RPL_BLEND_FACTOR_DESTINATION_ALPHA:
        return KAN_RENDER_BLEND_FACTOR_DESTINATION_ALPHA;

    case KAN_RPL_BLEND_FACTOR_ONE_MINUS_DESTINATION_ALPHA:
        return KAN_RENDER_BLEND_FACTOR_ONE_MINUS_DESTINATION_ALPHA;

    case KAN_RPL_BLEND_FACTOR_CONSTANT_COLOR:
        return KAN_RENDER_BLEND_FACTOR_CONSTANT_COLOR;

    case KAN_RPL_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR:
        return KAN_RENDER_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;

    case KAN_RPL_BLEND_FACTOR_CONSTANT_ALPHA:
        return KAN_RENDER_BLEND_FACTOR_CONSTANT_ALPHA;

    case KAN_RPL_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA:
        return KAN_RENDER_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;

    case KAN_RPL_BLEND_FACTOR_SOURCE_ALPHA_SATURATE:
        return KAN_RENDER_BLEND_FACTOR_SOURCE_ALPHA_SATURATE;
    }

    KAN_ASSERT (false)
    return KAN_RENDER_BLEND_FACTOR_ZERO;
}

static inline enum kan_render_blend_operation_t convert_blend_operation (enum kan_rpl_blend_operation_t blend_operation)
{
    switch (blend_operation)
    {
    case KAN_RPL_BLEND_OPERATION_ADD:
        return KAN_RENDER_BLEND_OPERATION_ADD;

    case KAN_RPL_BLEND_OPERATION_SUBTRACT:
        return KAN_RENDER_BLEND_OPERATION_SUBTRACT;

    case KAN_RPL_BLEND_OPERATION_REVERSE_SUBTRACT:
        return KAN_RENDER_BLEND_OPERATION_REVERSE_SUBTRACT;

    case KAN_RPL_BLEND_OPERATION_MIN:
        return KAN_RENDER_BLEND_OPERATION_MIN;

    case KAN_RPL_BLEND_OPERATION_MAX:
        return KAN_RENDER_BLEND_OPERATION_MAX;
    }

    KAN_ASSERT (false)
    return KAN_RENDER_BLEND_OPERATION_ADD;
}

static void load_material (struct render_foundation_program_core_management_state_t *state,
                           const struct kan_render_context_singleton_t *render_context,
                           const struct kan_resource_material_t *resource,
                           struct kan_render_material_loaded_t *loaded)
{
    KAN_CPU_SCOPED_STATIC_SECTION (load_material)
    const kan_interned_string_t name = loaded->name;
    // Not the most effective way to reset material content technically,
    // but should be rare enough for us to not care about it at all.
    kan_render_material_loaded_shutdown (loaded);
    kan_render_material_loaded_init (loaded);
    loaded->name = name;

    char tracking_name_buffer[KAN_UNIVERSE_RENDER_FOUNDATION_NAME_BUFFER_LENGTH];
    if (resource->set_material.buffers.size > 0u || resource->set_material.samplers.size > 0u ||
        resource->set_material.images.size > 0u)
    {
        snprintf (tracking_name_buffer, sizeof (tracking_name_buffer), "%s::material_set", loaded->name);
        loaded->set_material = construct_parameter_set_layout_from_meta (
            render_context->render_context, &resource->set_material, kan_string_intern (tracking_name_buffer),
            state->temporary_allocation_group);

        if (!KAN_HANDLE_IS_VALID (loaded->set_material))
        {
            KAN_LOG (render_foundation_program, KAN_LOG_ERROR,
                     "Failed to create material parameter set layout for material \"%s\".", loaded->name)
        }
    }

    if (resource->set_object.buffers.size > 0u || resource->set_object.samplers.size > 0u ||
        resource->set_object.images.size > 0u)
    {
        snprintf (tracking_name_buffer, sizeof (tracking_name_buffer), "%s::object_set", loaded->name);
        loaded->set_object = construct_parameter_set_layout_from_meta (
            render_context->render_context, &resource->set_object, kan_string_intern (tracking_name_buffer),
            state->temporary_allocation_group);

        if (!KAN_HANDLE_IS_VALID (loaded->set_object))
        {
            KAN_LOG (render_foundation_program, KAN_LOG_ERROR,
                     "Failed to create object parameter set layout for material \"%s\".", loaded->name)
        }
    }

    if (resource->set_shared.buffers.size > 0u || resource->set_shared.samplers.size > 0u ||
        resource->set_shared.images.size > 0u)
    {
        snprintf (tracking_name_buffer, sizeof (tracking_name_buffer), "%s::shared_set", loaded->name);
        loaded->set_shared = construct_parameter_set_layout_from_meta (
            render_context->render_context, &resource->set_shared, kan_string_intern (tracking_name_buffer),
            state->temporary_allocation_group);

        if (!KAN_HANDLE_IS_VALID (loaded->set_shared))
        {
            KAN_LOG (render_foundation_program, KAN_LOG_ERROR,
                     "Failed to create shared parameter set layout for material \"%s\".", loaded->name)
        }
    }

    struct kan_render_attribute_source_description_t
        attribute_sources_static[KAN_UNIVERSE_RENDER_FOUNDATION_BINDINGS_MAX_STATIC];
    struct kan_render_attribute_source_description_t *attribute_sources = NULL;
    kan_instance_size_t attributes_sources_count = 0u;

    CUSHION_DEFER
    {
        if (attribute_sources && attribute_sources != attribute_sources_static)
        {
            kan_free_general (state->temporary_allocation_group, attribute_sources,
                              sizeof (struct kan_render_attribute_source_description_t) * attributes_sources_count);
        }
    }

    struct kan_render_attribute_description_t attributes_static[KAN_UNIVERSE_RENDER_FOUNDATION_BINDINGS_MAX_STATIC];
    struct kan_render_attribute_description_t *attributes = NULL;
    kan_instance_size_t attributes_count = 0u;

    CUSHION_DEFER
    {
        if (attributes && attributes != attributes_static)
        {
            kan_free_general (state->temporary_allocation_group, attributes,
                              sizeof (struct kan_render_attribute_description_t) * attributes_count);
        }
    }

    if (resource->vertex_attribute_sources.size > 0u || resource->has_instanced_attribute_source)
    {
        attribute_sources = attribute_sources_static;
        attributes_sources_count = resource->vertex_attribute_sources.size;

        if (resource->has_instanced_attribute_source)
        {
            ++attributes_sources_count;
        }

        if (attributes_sources_count > KAN_UNIVERSE_RENDER_FOUNDATION_BINDINGS_MAX_STATIC)
        {
            attribute_sources = kan_allocate_general (
                state->temporary_allocation_group,
                sizeof (struct kan_render_attribute_source_description_t) * attributes_sources_count,
                alignof (struct kan_render_attribute_source_description_t));
        }

        for (kan_memory_size_t index = 0u; index < resource->vertex_attribute_sources.size; ++index)
        {
            struct kan_rpl_meta_attribute_source_t *source =
                &((struct kan_rpl_meta_attribute_source_t *) resource->vertex_attribute_sources.data)[index];
            attributes_count += source->attributes.size;

            attribute_sources[index] = (struct kan_render_attribute_source_description_t) {
                .binding = source->binding,
                .stride = source->block_size,
                .rate = KAN_RENDER_ATTRIBUTE_RATE_PER_VERTEX,
            };
        }

        if (resource->has_instanced_attribute_source)
        {
            attributes_count += resource->instanced_attribute_source.attributes.size;
            attribute_sources[resource->vertex_attribute_sources.size] =
                (struct kan_render_attribute_source_description_t) {
                    .binding = resource->instanced_attribute_source.binding,
                    .stride = resource->instanced_attribute_source.block_size,
                    .rate = KAN_RENDER_ATTRIBUTE_RATE_PER_INSTANCE,
                };
        }
    }

    if (attributes_count > 0u)
    {
        attributes = attributes_static;
        if (attributes_count > KAN_UNIVERSE_RENDER_FOUNDATION_BINDINGS_MAX_STATIC)
        {
            attributes = kan_allocate_general (state->temporary_allocation_group,
                                               sizeof (struct kan_render_attribute_description_t) * attributes_count,
                                               alignof (struct kan_render_attribute_description_t));
        }

        kan_instance_size_t attribute_output_index = 0u;
        for (kan_memory_size_t source_index = 0u; source_index < resource->vertex_attribute_sources.size;
             ++source_index)
        {
            struct kan_rpl_meta_attribute_source_t *source =
                &((struct kan_rpl_meta_attribute_source_t *) resource->vertex_attribute_sources.data)[source_index];
            add_attributes_from_source (source, attributes, &attribute_output_index);
        }

        if (resource->has_instanced_attribute_source)
        {
            add_attributes_from_source (&resource->instanced_attribute_source, attributes, &attribute_output_index);
        }
    }

    // We always just use active priority as we'll lock the commit until everything is compiled.
    const enum kan_render_pipeline_compilation_priority_t pipeline_priority =
        KAN_RENDER_PIPELINE_COMPILATION_PRIORITY_ACTIVE;
    kan_dynamic_array_set_capacity (&loaded->pipelines, resource->pipelines.size);

    for (kan_memory_size_t index = 0u; index < resource->pipelines.size; ++index)
    {
        const struct kan_resource_material_pipeline_t *input =
            &((struct kan_resource_material_pipeline_t *) resource->pipelines.data)[index];

        snprintf (tracking_name_buffer, sizeof (tracking_name_buffer), "%s::%s::%s", loaded->name, input->pass_name,
                  input->variant_name ? input->variant_name : "<base>");
        const kan_interned_string_t tracking_name = kan_string_intern (tracking_name_buffer);

        KAN_UMI_VALUE_READ_OPTIONAL (pass, kan_render_foundation_pass_loaded_t, name, &input->pass_name)
        if (!pass || !KAN_HANDLE_IS_VALID (pass->pass))
        {
            KAN_LOG (render_foundation_program, KAN_LOG_ERROR,
                     "Failed to create pipeline for material \"%s\" for pass \"%s\" for variant \"%s\" as pass is not "
                     "available in runtime for some reason.",
                     loaded->name, input->pass_name, input->variant_name ? input->variant_name : "<base>")
            continue;
        }

        if ((kan_render_get_supported_code_format_flags () & (kan_memory_size_t) (1u << input->code_format)) == 0u)
        {
            KAN_LOG (render_foundation_program, KAN_LOG_ERROR,
                     "Failed to create pipeline for material \"%s\" for pass \"%s\" for variant \"%s\" as its code "
                     "format is not supported.",
                     loaded->name, input->pass_name, input->variant_name ? input->variant_name : "<base>")
            continue;
        }

        kan_render_code_module_t code_module = kan_render_code_module_create (
            render_context->render_context, input->code.size, input->code.data, tracking_name);

        if (!KAN_HANDLE_IS_VALID (code_module))
        {
            KAN_LOG (render_foundation_program, KAN_LOG_ERROR,
                     "Failed to create pipeline for material \"%s\" for pass \"%s\" for variant \"%s\" as code module "
                     "creation has failed.",
                     loaded->name, input->pass_name, input->variant_name ? input->variant_name : "<base>")
            continue;
        }

        CUSHION_DEFER { kan_render_code_module_destroy (code_module); }
        kan_render_pipeline_parameter_set_layout_t pass_layout = KAN_HANDLE_INITIALIZE_INVALID;

        for (kan_memory_size_t variant_index = 0u; variant_index < pass->variants.size; ++variant_index)
        {
            const struct kan_render_foundation_pass_variant_t *variant =
                &((struct kan_render_foundation_pass_variant_t *) pass->variants.data)[variant_index];

            if (variant->name == input->variant_name)
            {
                pass_layout = variant->pass_parameter_set_layout;
                break;
            }
        }

        enum kan_render_polygon_mode_t polygon_mode = KAN_RENDER_POLYGON_MODE_FILL;
        switch (input->pipeline_settings.polygon_mode)
        {
        case KAN_RPL_POLYGON_MODE_FILL:
            polygon_mode = KAN_RENDER_POLYGON_MODE_FILL;
            break;

        case KAN_RPL_POLYGON_MODE_WIREFRAME:
            polygon_mode = KAN_RENDER_POLYGON_MODE_WIREFRAME;
            break;
        }

        enum kan_render_cull_mode_t cull_mode = KAN_RENDER_CULL_MODE_BACK;
        switch (input->pipeline_settings.cull_mode)
        {
        case KAN_RPL_CULL_MODE_NONE:
            cull_mode = KAN_RENDER_CULL_MODE_NONE;
            break;

        case KAN_RPL_CULL_MODE_BACK:
            cull_mode = KAN_RENDER_CULL_MODE_BACK;
            break;

        case KAN_RPL_CULL_MODE_FRONT:
            cull_mode = KAN_RENDER_CULL_MODE_FRONT;
            break;
        }

#define COLOR_OUTPUTS_STATIC_COUNT 4u
#define ENTRY_POINTS_STATIC_COUNT 4u

        struct kan_render_color_output_setup_description_t color_outputs_static[COLOR_OUTPUTS_STATIC_COUNT];
        struct kan_render_color_output_setup_description_t *color_outputs = NULL;

        if (input->color_outputs.size > 0u)
        {
            color_outputs = color_outputs_static;
            if (input->color_outputs.size > COLOR_OUTPUTS_STATIC_COUNT)
            {
                color_outputs = kan_allocate_general (
                    state->temporary_allocation_group,
                    sizeof (struct kan_render_color_output_setup_description_t) * input->color_outputs.size,
                    alignof (struct kan_render_color_output_setup_description_t));
            }

            for (kan_memory_size_t color_index = 0u; color_index < input->color_outputs.size; ++color_index)
            {
                struct kan_rpl_meta_color_output_t *color_output =
                    &((struct kan_rpl_meta_color_output_t *) input->color_outputs.data)[color_index];

                color_outputs[color_index] = (struct kan_render_color_output_setup_description_t) {
                    .use_blend = color_output->use_blend,
                    .write_r = color_output->write_r,
                    .write_g = color_output->write_g,
                    .write_b = color_output->write_b,
                    .write_a = color_output->write_a,
                    .source_color_blend_factor = convert_blend_factor (color_output->source_color_blend_factor),
                    .destination_color_blend_factor =
                        convert_blend_factor (color_output->destination_color_blend_factor),
                    .color_blend_operation = convert_blend_operation (color_output->color_blend_operation),
                    .source_alpha_blend_factor = convert_blend_factor (color_output->source_alpha_blend_factor),
                    .destination_alpha_blend_factor =
                        convert_blend_factor (color_output->destination_alpha_blend_factor),
                    .alpha_blend_operation = convert_blend_operation (color_output->alpha_blend_operation),
                };
            }
        }

        CUSHION_DEFER
        {
            if (color_outputs && color_outputs != color_outputs_static)
            {
                kan_free_general (
                    state->temporary_allocation_group, color_outputs,
                    sizeof (struct kan_render_color_output_setup_description_t) * input->color_outputs.size);
            }
        }

        struct kan_render_pipeline_code_entry_point_t entry_points_static[ENTRY_POINTS_STATIC_COUNT];
        struct kan_render_pipeline_code_module_usage_t code_module_usage = {
            .code_module = code_module,
            .entry_points_count = input->entry_points.size,
            .entry_points = NULL,
        };

        if (code_module_usage.entry_points_count > 0u)
        {
            code_module_usage.entry_points = entry_points_static;
            if (code_module_usage.entry_points_count > ENTRY_POINTS_STATIC_COUNT)
            {
                code_module_usage.entry_points = kan_allocate_general (
                    state->temporary_allocation_group,
                    sizeof (struct kan_render_pipeline_code_entry_point_t) * code_module_usage.entry_points_count,
                    alignof (struct kan_render_pipeline_code_entry_point_t));
            }

            for (kan_memory_size_t point_index = 0u; point_index < code_module_usage.entry_points_count; ++point_index)
            {
                struct kan_rpl_entry_point_t *entry_point =
                    &((struct kan_rpl_entry_point_t *) input->entry_points.data)[point_index];
                enum kan_render_stage_t stage = KAN_RENDER_STAGE_GRAPHICS_FRAGMENT;

                switch (entry_point->stage)
                {
                case KAN_RPL_PIPELINE_STAGE_GRAPHICS_CLASSIC_VERTEX:
                    stage = KAN_RENDER_STAGE_GRAPHICS_VERTEX;
                    break;

                case KAN_RPL_PIPELINE_STAGE_GRAPHICS_CLASSIC_FRAGMENT:
                    stage = KAN_RENDER_STAGE_GRAPHICS_FRAGMENT;
                    break;
                }

                code_module_usage.entry_points[point_index] = (struct kan_render_pipeline_code_entry_point_t) {
                    .stage = stage,
                    .function_name = entry_point->function_name,
                };
            }
        }

        CUSHION_DEFER
        {
            if (code_module_usage.entry_points && code_module_usage.entry_points != entry_points_static)
            {
                kan_free_general (
                    state->temporary_allocation_group, code_module_usage.entry_points,
                    sizeof (struct kan_render_pipeline_code_entry_point_t) * code_module_usage.entry_points_count);
            }
        }

        kan_render_pipeline_parameter_set_layout_t parameter_sets[4u] = {
            pass_layout,
            loaded->set_material,
            loaded->set_object,
            loaded->set_shared,
        };

        struct kan_render_graphics_pipeline_description_t description = {
            .pass = pass->pass,
            .topology = KAN_RENDER_GRAPHICS_TOPOLOGY_TRIANGLE_LIST,
            .attribute_sources_count = attributes_sources_count,
            .attribute_sources = attribute_sources,
            .attributes_count = attributes_count,
            .attributes = attributes,
            .push_constant_size = resource->push_constant_size,
            .parameter_set_layouts_count = 4u,
            .parameter_set_layouts = parameter_sets,

            .polygon_mode = polygon_mode,
            .cull_mode = cull_mode,
            .use_depth_clamp = false,

            .output_setups_count = input->color_outputs.size,
            .output_setups = color_outputs,

            .blend_constant_r = input->color_blend_constants.r,
            .blend_constant_g = input->color_blend_constants.g,
            .blend_constant_b = input->color_blend_constants.b,
            .blend_constant_a = input->color_blend_constants.a,

            .depth_test_enabled = input->pipeline_settings.depth_test,
            .depth_write_enabled = input->pipeline_settings.depth_write,
            .depth_bounds_test_enabled = input->pipeline_settings.depth_bounds_test,
            .depth_compare_operation = convert_compare_operation (input->pipeline_settings.depth_compare_operation),
            .min_depth = input->pipeline_settings.depth_min,
            .max_depth = input->pipeline_settings.depth_max,

            .stencil_test_enabled = input->pipeline_settings.stencil_test,
            .stencil_front =
                {
                    .on_fail = convert_stencil_operation (input->pipeline_settings.stencil_front_on_fail),
                    .on_depth_fail = convert_stencil_operation (input->pipeline_settings.stencil_front_on_depth_fail),
                    .on_pass = convert_stencil_operation (input->pipeline_settings.stencil_front_on_pass),
                    .compare = convert_compare_operation (input->pipeline_settings.stencil_front_compare),
                    .compare_mask = input->pipeline_settings.stencil_front_compare_mask,
                    .write_mask = input->pipeline_settings.stencil_front_write_mask,
                    .reference = input->pipeline_settings.stencil_front_reference,
                },
            .stencil_back =
                {
                    .on_fail = convert_stencil_operation (input->pipeline_settings.stencil_back_on_fail),
                    .on_depth_fail = convert_stencil_operation (input->pipeline_settings.stencil_back_on_depth_fail),
                    .on_pass = convert_stencil_operation (input->pipeline_settings.stencil_back_on_pass),
                    .compare = convert_compare_operation (input->pipeline_settings.stencil_back_compare),
                    .compare_mask = input->pipeline_settings.stencil_back_compare_mask,
                    .write_mask = input->pipeline_settings.stencil_back_write_mask,
                    .reference = input->pipeline_settings.stencil_back_reference,
                },

            .code_modules_count = 1u,
            .code_modules = &code_module_usage,
            .tracking_name = tracking_name,
        };

        kan_render_graphics_pipeline_t pipeline =
            kan_render_graphics_pipeline_create (render_context->render_context, &description, pipeline_priority);

        if (!KAN_HANDLE_IS_VALID (pipeline))
        {
            KAN_LOG (render_foundation_program, KAN_LOG_ERROR,
                     "Failed to create pipeline for material \"%s\" for pass \"%s\" for variant \"%s\" as pipeline "
                     "creation function has failed.",
                     loaded->name, input->pass_name, input->variant_name ? input->variant_name : "<base>")
            continue;
        }

#undef COLOR_OUTPUTS_STATIC_COUNT
#undef ENTRY_POINTS_STATIC_COUNT

        struct kan_render_material_pipeline_t *output = kan_dynamic_array_add_last (&loaded->pipelines);
        output->pass_name = input->pass_name;
        output->variant_name = input->variant_name;
        output->pipeline = pipeline;
    }

    kan_dynamic_array_set_capacity (&loaded->vertex_attribute_sources, resource->vertex_attribute_sources.size);
    for (kan_memory_size_t index = 0u; index < resource->vertex_attribute_sources.size; ++index)
    {
        const struct kan_rpl_meta_attribute_source_t *input =
            &((struct kan_rpl_meta_attribute_source_t *) resource->vertex_attribute_sources.data)[index];

        struct kan_rpl_meta_attribute_source_t *output = kan_dynamic_array_add_last (&loaded->vertex_attribute_sources);
        kan_rpl_meta_attribute_source_init_copy (output, input);
    }

    loaded->push_constant_size = resource->push_constant_size;
    if ((loaded->has_instanced_attribute_source = resource->has_instanced_attribute_source))
    {
        kan_rpl_meta_attribute_source_shutdown (&loaded->instanced_attribute_source);
        kan_rpl_meta_attribute_source_init_copy (&loaded->instanced_attribute_source,
                                                 &resource->instanced_attribute_source);
    }

    kan_rpl_meta_set_bindings_shutdown (&loaded->set_material_bindings);
    kan_rpl_meta_set_bindings_init_copy (&loaded->set_material_bindings, &resource->set_material);

    kan_rpl_meta_set_bindings_shutdown (&loaded->set_object_bindings);
    kan_rpl_meta_set_bindings_init_copy (&loaded->set_object_bindings, &resource->set_object);

    kan_rpl_meta_set_bindings_shutdown (&loaded->set_shared_bindings);
    kan_rpl_meta_set_bindings_init_copy (&loaded->set_shared_bindings, &resource->set_shared);
    KAN_UMO_EVENT_INSERT_INIT (kan_render_material_updated_event_t) {.name = loaded->name};
}

UNIVERSE_RENDER_FOUNDATION_API KAN_UM_MUTATOR_EXECUTE (render_foundation_program_core_management)
{
    KAN_UMI_SINGLETON_READ (render_context, kan_render_context_singleton_t)
    if (!KAN_HANDLE_IS_VALID (render_context->render_context))
    {
        return;
    }

    KAN_UMI_SINGLETON_READ (provider, kan_resource_provider_singleton_t)
    if (provider->transaction_state == KAN_RESOURCE_TRANSACTION_STATE_COMMIT)
    {
        KAN_UML_RESOURCE_LOADED_EVENT_FETCH (pass_loaded_event, kan_resource_render_pass_t)
        {
            KAN_UMI_RESOURCE_RETRIEVE_FRESH_LOADED (resource, kan_resource_render_pass_t, &pass_loaded_event->name)
            KAN_UMI_VALUE_UPDATE_OPTIONAL (existing_loaded, kan_render_foundation_pass_loaded_t, name,
                                           &pass_loaded_event->name)

            if (existing_loaded)
            {
                load_pass (state, render_context, resource, existing_loaded);
            }
            else
            {
                KAN_UMO_INDEXED_INSERT (new_loaded, kan_render_foundation_pass_loaded_t)
                {
                    new_loaded->name = pass_loaded_event->name;
                    load_pass (state, render_context, resource, new_loaded);
                }
            }
        }

        // We do not need to manually update materials after pass updates as if pass was updated
        // then material would always be updated by build system as well.

        KAN_UML_RESOURCE_LOADED_EVENT_FETCH (material_loaded_event, kan_resource_material_t)
        {
            KAN_UMI_RESOURCE_RETRIEVE_FRESH_LOADED (resource, kan_resource_material_t, &material_loaded_event->name)
            KAN_UMI_VALUE_UPDATE_OPTIONAL (existing_loaded, kan_render_material_loaded_t, name,
                                           &material_loaded_event->name)

            if (existing_loaded)
            {
                load_material (state, render_context, resource, existing_loaded);
            }
            else
            {
                KAN_UMO_INDEXED_INSERT (new_loaded, kan_render_material_loaded_t)
                {
                    new_loaded->name = material_loaded_event->name;
                    load_material (state, render_context, resource, new_loaded);
                }
            }
        }

        if (!kan_render_context_are_pipelines_compiled (render_context->render_context))
        {
            kan_resource_provider_singleton_extend_commit (provider);
        }
    }

    if (provider->transaction_state == KAN_RESOURCE_TRANSACTION_STATE_NONE)
    {
        KAN_UML_RESOURCE_UNLOAD_PLANNED_EVENT_FETCH (pass_unload_event, kan_resource_render_pass_t)
        {
            KAN_UMI_VALUE_DELETE_OPTIONAL (existing_loaded, kan_render_foundation_pass_loaded_t, name,
                                           &pass_unload_event->name)

            if (existing_loaded)
            {
                KAN_UM_ACCESS_DELETE (existing_loaded);
            }
        }

        KAN_UML_RESOURCE_UNLOAD_PLANNED_EVENT_FETCH (material_unload_event, kan_resource_material_t)
        {
            KAN_UMI_VALUE_DELETE_OPTIONAL (existing_loaded, kan_render_material_loaded_t, name,
                                           &material_unload_event->name)

            if (existing_loaded)
            {
                KAN_UM_ACCESS_DELETE (existing_loaded);
            }
        }
    }
}

struct render_foundation_material_instance_management_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (render_foundation_material_instance_management)
    KAN_UM_BIND_STATE (render_foundation_material_instance_management, state)
    kan_allocation_group_t temporary_allocation_group;
};

UNIVERSE_RENDER_FOUNDATION_API KAN_UM_MUTATOR_DEPLOY (render_foundation_material_instance_management)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_cpu_static_sections_ensure_initialized ();
    state->temporary_allocation_group = kan_allocation_group_get_child (kan_allocation_group_stack_get (), "temporary");

    kan_workflow_graph_node_depend_on (workflow_node, KAN_RESOURCE_PROVIDER_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_RENDER_FOUNDATION_TEXTURE_MANAGEMENT_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, "render_foundation_program_core_management");
    kan_workflow_graph_node_make_dependency_of (workflow_node, KAN_RENDER_FOUNDATION_PROGRAM_MANAGEMENT_END_CHECKPOINT);
}

static void update_material_instance_quality (struct render_foundation_material_instance_management_state_t *state,
                                              const struct kan_resource_provider_singleton_t *provider,
                                              struct kan_render_material_instance_loaded_t *loaded)
{
    const kan_instance_size_t old_value = loaded->requested_best_mip;
    loaded->requested_best_mip = KAN_INT_MAX (kan_instance_size_t);
    loaded->requested_best_mip_frame_id = provider->logic_deduplication_frame_id;

    KAN_UML_VALUE_READ (quality, kan_render_material_instance_quality_t, name, &loaded->name)
    {
        loaded->requested_best_mip = KAN_MIN (loaded->requested_best_mip, quality->best_advised_mip);
    }

    if (old_value == loaded->requested_best_mip)
    {
        return;
    }

    KAN_UMI_SINGLETON_READ (texture_singleton, kan_render_texture_singleton_t)
    KAN_UML_VALUE_UPDATE (usage, render_foundation_material_instance_texture_usage_t, material_instance_name,
                          &loaded->name)
    {
        if (KAN_TYPED_ID_32_IS_VALID (usage->quality_id))
        {
            KAN_UMI_VALUE_DETACH_REQUIRED (texture_quality, kan_render_texture_quality_t, quality_id,
                                           &usage->quality_id)
            KAN_UM_ACCESS_DELETE (texture_quality);
            usage->quality_id = KAN_TYPED_ID_32_SET_INVALID (kan_render_texture_quality_id_t);
        }

        if (loaded->requested_best_mip != KAN_INT_MAX (kan_instance_size_t))
        {
            usage->quality_id = kan_next_texture_quality_id (texture_singleton);
            KAN_UMI_INDEXED_INSERT (texture_quality, kan_render_texture_quality_t)
            texture_quality->quality_id = usage->quality_id;
            texture_quality->name = usage->texture_name;
            texture_quality->best_advised_mip = loaded->requested_best_mip;
        }
    }
}

static void load_material_instance (struct render_foundation_material_instance_management_state_t *state,
                                    const struct kan_resource_provider_singleton_t *provider,
                                    const struct kan_render_context_singleton_t *render_context,
                                    const struct kan_resource_material_instance_t *resource,
                                    struct kan_render_material_instance_loaded_t *loaded)
{
    KAN_CPU_SCOPED_STATIC_SECTION (load_material)
    const kan_interned_string_t name = loaded->name;
    // Not the most effective way to reset material instance content technically,
    // but should be rare enough for us to not care about it at all.
    kan_render_material_instance_loaded_shutdown (loaded);
    kan_render_material_instance_loaded_init (loaded);

    KAN_UML_VALUE_DETACH (usage_to_delete, render_foundation_material_instance_texture_usage_t, material_instance_name,
                          &name)
    {
        KAN_UM_ACCESS_DELETE (usage_to_delete);
    }

    loaded->name = name;
    loaded->material_name = resource->material;

    // Using required here is fine as if resources were built without errors and loading order is right,
    // then material should always exist and be loaded at that point.
    KAN_UMI_VALUE_READ_REQUIRED (material_loaded, kan_render_material_loaded_t, name, &resource->material)

    struct kan_render_parameter_update_description_t bindings_static[KAN_UNIVERSE_RENDER_FOUNDATION_MI_UPDATES_COUNT];
    struct kan_render_parameter_update_description_t *bindings = bindings_static;
    const kan_instance_size_t bindings_count = resource->buffers.size + resource->samplers.size + resource->images.size;

    if (bindings_count > KAN_UNIVERSE_RENDER_FOUNDATION_MI_UPDATES_COUNT)
    {
        bindings = kan_allocate_general (state->temporary_allocation_group,
                                         sizeof (struct kan_render_parameter_update_description_t) * bindings_count,
                                         alignof (struct kan_render_parameter_update_description_t));
    }

    CUSHION_DEFER
    {
        if (bindings != bindings_static)
        {
            kan_free_general (state->temporary_allocation_group, bindings,
                              sizeof (struct kan_render_parameter_update_description_t) * bindings_count);
        }
    }

    char tracking_name_buffer[KAN_UNIVERSE_RENDER_FOUNDATION_NAME_BUFFER_LENGTH];
    struct kan_render_parameter_update_description_t *bindings_output = bindings;
    kan_dynamic_array_set_capacity (&loaded->bound_buffers, resource->buffers.size);

    for (kan_memory_size_t index = 0u; index < resource->buffers.size; ++index)
    {
        const struct kan_resource_buffer_binding_t *buffer_binding =
            &((struct kan_resource_buffer_binding_t *) resource->buffers.data)[index];

        if (buffer_binding->data.size == 0u)
        {
            // Zero size buffers are possible for buffers with zero count of tails and empty main part.
            continue;
        }

        enum kan_render_buffer_type_t buffer_type = KAN_RENDER_BUFFER_TYPE_UNIFORM;
        switch (buffer_binding->type)
        {
        case KAN_RPL_BUFFER_TYPE_UNIFORM:
            buffer_type = KAN_RENDER_BUFFER_TYPE_UNIFORM;
            break;

        case KAN_RPL_BUFFER_TYPE_READ_ONLY_STORAGE:
            buffer_type = KAN_RENDER_BUFFER_TYPE_STORAGE;
            break;

        case KAN_RPL_BUFFER_TYPE_PUSH_CONSTANT:
            // Should not be here in the meta.
            KAN_ASSERT (false)
            break;
        }

        snprintf (tracking_name_buffer, sizeof (tracking_name_buffer), "%s::buffer%u", loaded->name,
                  (unsigned int) buffer_binding->binding);

        kan_render_buffer_t new_buffer =
            kan_render_buffer_create (render_context->render_context, buffer_type, buffer_binding->data.size,
                                      buffer_binding->data.data, kan_string_intern (tracking_name_buffer));

        if (!KAN_HANDLE_IS_VALID (new_buffer))
        {
            KAN_LOG (render_foundation_program, KAN_LOG_ERROR,
                     "Failed to create buffer for material instance \"%s\" at binding %u.", loaded->name,
                     (unsigned int) buffer_binding->binding)
            continue;
        }

        struct kan_render_material_instance_bound_buffer_t *bound_buffer =
            kan_dynamic_array_add_last (&loaded->bound_buffers);
        bound_buffer->binding = buffer_binding->binding;
        bound_buffer->buffer = new_buffer;

        bindings_output->binding = buffer_binding->binding;
        bindings_output->buffer_binding.buffer = new_buffer;
        bindings_output->buffer_binding.offset = 0u;
        bindings_output->buffer_binding.range = buffer_binding->data.size;
        ++bindings_output;
    }

    for (kan_memory_size_t index = 0u; index < resource->samplers.size; ++index)
    {
        const struct kan_resource_sampler_binding_t *sampler_binding =
            &((struct kan_resource_sampler_binding_t *) resource->samplers.data)[index];

        bindings_output->binding = sampler_binding->binding;
        bindings_output->sampler_binding.sampler = sampler_binding->sampler;
        ++bindings_output;
    }

    for (kan_memory_size_t index = 0u; index < resource->images.size; ++index)
    {
        const struct kan_resource_image_binding_t *binding =
            &((struct kan_resource_image_binding_t *) resource->images.data)[index];

        KAN_UMO_INDEXED_INSERT (usage, render_foundation_material_instance_texture_usage_t)
        {
            usage->material_instance_name = loaded->name;
            usage->texture_name = binding->texture;
            usage->binding = binding->binding;
            usage->quality_id = KAN_TYPED_ID_32_SET_INVALID (kan_render_texture_quality_id_t);
            usage->bound_image = KAN_HANDLE_SET_INVALID (kan_render_image_t);

            KAN_UMI_VALUE_READ_OPTIONAL (texture_loaded, kan_render_texture_loaded_t, name, &binding->texture)
            if (texture_loaded)
            {
                usage->bound_image = texture_loaded->image;
            }

            if (KAN_HANDLE_IS_VALID (usage->bound_image))
            {
                bindings_output->binding = usage->binding;
                bindings_output->image_binding.image = texture_loaded->image;
                bindings_output->image_binding.array_index = 0u;
                bindings_output->image_binding.layer_offset = 0u;
                bindings_output->image_binding.layer_count = 1u;
                ++bindings_output;
            }
        }
    }

    // Technically, having no material set is not an error as it just means that there would be no bindings, and it is
    // technically fine, as if it would not fine, then it wouldn't be built as a resource.
    if (KAN_HANDLE_IS_VALID (material_loaded->set_material))
    {
        struct kan_render_pipeline_parameter_set_description_t description = {
            .layout = material_loaded->set_material,
            .stable_binding = true,
            .tracking_name = loaded->name,
            .initial_bindings_count = (kan_instance_size_t) (bindings_output - bindings),
            .initial_bindings = bindings,
        };

        loaded->parameter_set = kan_render_pipeline_parameter_set_create (render_context->render_context, &description);
        if (!KAN_HANDLE_IS_VALID (loaded->parameter_set))
        {
            KAN_LOG (render_foundation_program, KAN_LOG_ERROR,
                     "Failed to create parameter set for material instance \"%s\".", loaded->name)
        }

        kan_dynamic_array_set_capacity (&loaded->variants, resource->variants.size);
        for (kan_memory_size_t index = 0u; index < resource->variants.size; ++index)
        {
            const struct kan_resource_material_variant_t *source =
                &((struct kan_resource_material_variant_t *) resource->variants.data)[index];
            struct kan_resource_material_variant_t *target = kan_dynamic_array_add_last (&loaded->variants);

            kan_allocation_group_stack_push (loaded->variants.allocation_group);
            kan_resource_material_variant_init (target);
            kan_allocation_group_stack_pop ();

            target->name = source->name;
            kan_dynamic_array_set_capacity (&target->instanced_data, source->instanced_data.size);
            target->instanced_data.size = source->instanced_data.size;
            memcpy (target->instanced_data.data, source->instanced_data.data, source->instanced_data.size);
        }
    }

    update_material_instance_quality (state, provider, loaded);
    KAN_UMO_EVENT_INSERT_INIT (kan_render_material_instance_updated_event_t) {.name = name};
}

static void on_material_instance_quality_insert_or_delete (
    struct render_foundation_material_instance_management_state_t *state,
    const struct kan_resource_provider_singleton_t *provider,
    kan_interned_string_t name)
{
    KAN_UMI_VALUE_UPDATE_OPTIONAL (loaded, kan_render_material_instance_loaded_t, name, &name)
    if (!loaded || loaded->requested_best_mip_frame_id == provider->logic_deduplication_frame_id)
    {
        return;
    }

    update_material_instance_quality (state, provider, loaded);
}

UNIVERSE_RENDER_FOUNDATION_API KAN_UM_MUTATOR_EXECUTE (render_foundation_material_instance_management)
{
    KAN_UMI_SINGLETON_READ (render_context, kan_render_context_singleton_t)
    if (!KAN_HANDLE_IS_VALID (render_context->render_context))
    {
        return;
    }

    KAN_UMI_SINGLETON_READ (provider, kan_resource_provider_singleton_t)
    if (provider->transaction_state == KAN_RESOURCE_TRANSACTION_STATE_COMMIT)
    {
        // We do not need to manually update material instances after material updates as if material was updated
        // then material instance would always be updated by build system as well.

        KAN_UML_RESOURCE_LOADED_EVENT_FETCH (loaded_event, kan_resource_material_instance_t)
        {
            KAN_UMI_RESOURCE_RETRIEVE_FRESH_LOADED (resource, kan_resource_material_instance_t, &loaded_event->name)
            KAN_UMI_VALUE_UPDATE_OPTIONAL (existing_loaded, kan_render_material_instance_loaded_t, name,
                                           &loaded_event->name)

            if (existing_loaded)
            {
                load_material_instance (state, provider, render_context, resource, existing_loaded);
            }
            else
            {
                KAN_UMO_INDEXED_INSERT (new_loaded, kan_render_material_instance_loaded_t)
                {
                    new_loaded->name = loaded_event->name;
                    load_material_instance (state, provider, render_context, resource, new_loaded);
                }
            }
        }

        KAN_UML_EVENT_FETCH (texture_updated_event, kan_render_texture_updated_event_t)
        {
            KAN_CPU_SCOPED_STATIC_SECTION (texture_updated)
            KAN_UMI_VALUE_READ_OPTIONAL (texture, kan_render_texture_loaded_t, name, &texture_updated_event->name)
            if (!texture)
            {
                continue;
            }

            KAN_UML_VALUE_UPDATE (usage, render_foundation_material_instance_texture_usage_t, texture_name,
                                  &texture_updated_event->name)
            {
                if (KAN_HANDLE_IS_EQUAL (texture->image, usage->bound_image))
                {
                    continue;
                }

                usage->bound_image = texture->image;
                KAN_UMI_VALUE_READ_REQUIRED (material_instance, kan_render_material_instance_loaded_t, name,
                                             &usage->material_instance_name)

                if (KAN_HANDLE_IS_VALID (material_instance->parameter_set))
                {
                    struct kan_render_parameter_update_description_t binding;
                    binding.binding = usage->binding;
                    binding.image_binding.image = usage->bound_image;
                    binding.image_binding.array_index = 0u;
                    binding.image_binding.layer_offset = 0u;
                    binding.image_binding.layer_count = 1u;
                    kan_render_pipeline_parameter_set_update (material_instance->parameter_set, 1u, &binding);
                }
            }
        }
    }

    if (provider->transaction_state == KAN_RESOURCE_TRANSACTION_STATE_NONE)
    {
        KAN_UML_RESOURCE_UNLOAD_PLANNED_EVENT_FETCH (unload_event, kan_resource_material_instance_t)
        {
            KAN_UMI_VALUE_DELETE_OPTIONAL (existing_loaded, kan_render_material_instance_loaded_t, name,
                                           &unload_event->name)

            if (existing_loaded)
            {
                KAN_UM_ACCESS_DELETE (existing_loaded);
            }
        }

        KAN_UML_EVENT_FETCH (on_insert_event, render_foundation_material_instance_quality_on_insert_event_t)
        {
            on_material_instance_quality_insert_or_delete (state, provider, on_insert_event->material_instance_name);
        }

        KAN_UML_EVENT_FETCH (on_delete_event, render_foundation_material_instance_quality_on_delete_event_t)
        {
            on_material_instance_quality_insert_or_delete (state, provider, on_delete_event->material_instance_name);
        }
    }
}

void kan_render_program_singleton_init (struct kan_render_program_singleton_t *instance)
{
    instance->material_instance_quality_id_counter = kan_atomic_int_init (1);
}

void kan_render_foundation_pass_variant_init (struct kan_render_foundation_pass_variant_t *instance)
{
    instance->name = NULL;
    instance->pass_parameter_set_layout = KAN_HANDLE_SET_INVALID (kan_render_pipeline_parameter_set_layout_t);
    kan_rpl_meta_set_bindings_init (&instance->pass_parameter_set_bindings);
}

void kan_render_foundation_pass_variant_shutdown (struct kan_render_foundation_pass_variant_t *instance)
{
    if (KAN_HANDLE_IS_VALID (instance->pass_parameter_set_layout))
    {
        kan_render_pipeline_parameter_set_layout_destroy (instance->pass_parameter_set_layout);
    }

    kan_rpl_meta_set_bindings_shutdown (&instance->pass_parameter_set_bindings);
}

void kan_render_foundation_pass_loaded_init (struct kan_render_foundation_pass_loaded_t *instance)
{
    instance->name = NULL;
    instance->type = KAN_RENDER_PASS_GRAPHICS;
    instance->pass = KAN_HANDLE_SET_INVALID (kan_render_pass_t);

    kan_dynamic_array_init (&instance->attachments, 0u, sizeof (struct kan_render_foundation_pass_attachment_t),
                            alignof (struct kan_render_foundation_pass_attachment_t),
                            kan_allocation_group_stack_get ());

    kan_dynamic_array_init (&instance->variants, 0u, sizeof (struct kan_render_foundation_pass_variant_t),
                            alignof (struct kan_render_foundation_pass_variant_t), kan_allocation_group_stack_get ());
}

void kan_render_foundation_pass_loaded_shutdown (struct kan_render_foundation_pass_loaded_t *instance)
{
    if (KAN_HANDLE_IS_VALID (instance->pass))
    {
        kan_render_pass_destroy (instance->pass);
    }

    kan_dynamic_array_shutdown (&instance->attachments);
    KAN_DYNAMIC_ARRAY_SHUTDOWN_WITH_ITEMS_AUTO (instance->variants, kan_render_foundation_pass_variant)
}

void kan_render_material_pipeline_init (struct kan_render_material_pipeline_t *instance)
{
    instance->pass_name = NULL;
    instance->variant_name = NULL;
    instance->pipeline = KAN_HANDLE_SET_INVALID (kan_render_graphics_pipeline_t);
}

void kan_render_material_pipeline_shutdown (struct kan_render_material_pipeline_t *instance)
{
    if (KAN_HANDLE_IS_VALID (instance->pipeline))
    {
        kan_render_graphics_pipeline_destroy (instance->pipeline);
    }
}

void kan_render_material_loaded_init (struct kan_render_material_loaded_t *instance)
{
    instance->name = NULL;
    kan_dynamic_array_init (&instance->pipelines, 0u, sizeof (struct kan_render_material_pipeline_t),
                            alignof (struct kan_render_material_pipeline_t), kan_allocation_group_stack_get ());

    instance->set_material = KAN_HANDLE_SET_INVALID (kan_render_pipeline_parameter_set_layout_t);
    instance->set_object = KAN_HANDLE_SET_INVALID (kan_render_pipeline_parameter_set_layout_t);
    instance->set_shared = KAN_HANDLE_SET_INVALID (kan_render_pipeline_parameter_set_layout_t);

    kan_dynamic_array_init (&instance->vertex_attribute_sources, 0u, sizeof (struct kan_rpl_meta_attribute_source_t),
                            alignof (struct kan_rpl_meta_attribute_source_t), kan_allocation_group_stack_get ());
    instance->push_constant_size = 0u;

    instance->has_instanced_attribute_source = false;
    kan_rpl_meta_attribute_source_init (&instance->instanced_attribute_source);

    kan_rpl_meta_set_bindings_init (&instance->set_material_bindings);
    kan_rpl_meta_set_bindings_init (&instance->set_object_bindings);
    kan_rpl_meta_set_bindings_init (&instance->set_shared_bindings);
}

void kan_render_material_loaded_shutdown (struct kan_render_material_loaded_t *instance)
{
    KAN_DYNAMIC_ARRAY_SHUTDOWN_WITH_ITEMS_AUTO (instance->pipelines, kan_render_material_pipeline)
    if (KAN_HANDLE_IS_VALID (instance->set_material))
    {
        kan_render_pipeline_parameter_set_layout_destroy (instance->set_material);
    }

    if (KAN_HANDLE_IS_VALID (instance->set_object))
    {
        kan_render_pipeline_parameter_set_layout_destroy (instance->set_object);
    }

    if (KAN_HANDLE_IS_VALID (instance->set_shared))
    {
        kan_render_pipeline_parameter_set_layout_destroy (instance->set_shared);
    }

    KAN_DYNAMIC_ARRAY_SHUTDOWN_WITH_ITEMS_AUTO (instance->vertex_attribute_sources, kan_rpl_meta_attribute_source)
    kan_rpl_meta_attribute_source_shutdown (&instance->instanced_attribute_source);

    kan_rpl_meta_set_bindings_shutdown (&instance->set_material_bindings);
    kan_rpl_meta_set_bindings_shutdown (&instance->set_object_bindings);
    kan_rpl_meta_set_bindings_shutdown (&instance->set_shared_bindings);
}

void kan_render_material_instance_quality_init (struct kan_render_material_instance_quality_t *instance)
{
    instance->quality_id = KAN_TYPED_ID_32_SET_INVALID (kan_render_material_instance_quality_id_t);
    instance->name = NULL;
    instance->best_advised_mip = 0u;
}

void kan_render_material_instance_variant_init (struct kan_render_material_instance_variant_t *instance)
{
    instance->name = NULL;
    kan_dynamic_array_init (&instance->instanced_data, 0u, sizeof (uint8_t),
                            KAN_RESOURCE_RENDER_FOUNDATION_BUFFER_ALIGNMENT, kan_allocation_group_stack_get ());
}

void kan_render_material_instance_variant_shutdown (struct kan_render_material_instance_variant_t *instance)
{
    kan_dynamic_array_shutdown (&instance->instanced_data);
}

void kan_render_material_instance_bound_buffer_init (struct kan_render_material_instance_bound_buffer_t *instance)
{
    instance->binding = 0u;
    instance->buffer = KAN_HANDLE_SET_INVALID (kan_render_buffer_t);
}

void kan_render_material_instance_bound_buffer_shutdown (struct kan_render_material_instance_bound_buffer_t *instance)
{
    if (KAN_HANDLE_IS_VALID (instance->buffer))
    {
        kan_render_buffer_destroy (instance->buffer);
    }
}

void kan_render_material_instance_loaded_init (struct kan_render_material_instance_loaded_t *instance)
{
    instance->name = NULL;
    instance->material_name = NULL;
    instance->parameter_set = KAN_HANDLE_SET_INVALID (kan_render_pipeline_parameter_set_t);
    instance->requested_best_mip = KAN_INT_MAX (kan_instance_size_t);
    instance->requested_best_mip_frame_id = 0u;

    kan_dynamic_array_init (&instance->variants, 0u, sizeof (struct kan_render_material_instance_variant_t),
                            alignof (struct kan_render_material_instance_variant_t), kan_allocation_group_stack_get ());
    kan_dynamic_array_init (&instance->bound_buffers, 0u, sizeof (struct kan_render_material_instance_bound_buffer_t),
                            alignof (struct kan_render_material_instance_bound_buffer_t),
                            kan_allocation_group_stack_get ());
}

void kan_render_material_instance_loaded_shutdown (struct kan_render_material_instance_loaded_t *instance)
{
    if (KAN_HANDLE_IS_VALID (instance->parameter_set))
    {
        kan_render_pipeline_parameter_set_destroy (instance->parameter_set);
    }

    KAN_DYNAMIC_ARRAY_SHUTDOWN_WITH_ITEMS_AUTO (instance->variants, kan_render_material_instance_variant)
    KAN_DYNAMIC_ARRAY_SHUTDOWN_WITH_ITEMS_AUTO (instance->bound_buffers, kan_render_material_instance_bound_buffer)
}
