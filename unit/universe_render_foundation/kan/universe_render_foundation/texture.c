#include <string.h>

#include <kan/cpu_profiler/markup.h>
#include <kan/log/logging.h>
#include <kan/universe/macro.h>
#include <kan/universe_render_foundation/render_graph.h>
#include <kan/universe_render_foundation/texture.h>
#include <kan/universe_resource_provider/provider.h>

KAN_LOG_DEFINE_CATEGORY (render_foundation_texture);
KAN_USE_STATIC_INTERNED_IDS
KAN_USE_STATIC_CPU_SECTIONS

KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (render_foundation_texture_management)
UNIVERSE_RENDER_FOUNDATION_API KAN_UM_MUTATOR_GROUP_META (render_foundation_texture_management,
                                                          KAN_RENDER_FOUNDATION_TEXTURE_MANAGEMENT_MUTATOR_GROUP);

struct render_foundation_texture_quality_on_insert_event_t
{
    kan_interned_string_t texture_name;
};

KAN_REFLECTION_STRUCT_META (kan_render_texture_quality_t)
UNIVERSE_RENDER_FOUNDATION_API struct kan_repository_meta_automatic_on_insert_event_t
    render_foundation_texture_quality_on_insert_event = {
        .event_type = "render_foundation_texture_quality_on_insert_event_t",
        .copy_outs_count = 1u,
        .copy_outs =
            (struct kan_repository_copy_out_t[]) {
                {
                    .source_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"name"}},
                    .target_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"texture_name"}},
                },
            },
};

struct render_foundation_texture_quality_on_delete_event_t
{
    kan_interned_string_t texture_name;
};

KAN_REFLECTION_STRUCT_META (kan_render_texture_quality_t)
UNIVERSE_RENDER_FOUNDATION_API struct kan_repository_meta_automatic_on_delete_event_t
    render_foundation_texture_quality_on_delete_event = {
        .event_type = "render_foundation_texture_quality_on_delete_event_t",
        .copy_outs_count = 1u,
        .copy_outs =
            (struct kan_repository_copy_out_t[]) {
                {
                    .source_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"name"}},
                    .target_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"texture_name"}},
                },
            },
};

struct render_foundation_texture_streaming_t
{
    kan_immutable kan_interned_string_t texture_name;
    kan_immutable kan_interned_string_t data_name;
    kan_resource_streaming_id_t request_id;
};

KAN_REFLECTION_STRUCT_META (render_foundation_texture_streaming_t)
UNIVERSE_RENDER_FOUNDATION_API struct kan_repository_meta_automatic_cascade_deletion_t
    render_foundation_texture_streaming_request_cascade_deletion = {
        .parent_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"request_id"}},
        .child_type_name = "kan_resource_streaming_request_t",
        .child_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"request_id"}},
};

KAN_REFLECTION_STRUCT_META (kan_render_texture_loaded_t)
UNIVERSE_RENDER_FOUNDATION_API struct kan_repository_meta_automatic_cascade_deletion_t
    render_foundation_texture_usage_id_cascade_deletion = {
        .parent_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"name"}},
        .child_type_name = "render_foundation_texture_streaming_t",
        .child_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"texture_name"}},
};

struct render_foundation_texture_management_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (render_foundation_texture_management)
    KAN_UM_BIND_STATE (render_foundation_texture_management, state)
};

UNIVERSE_RENDER_FOUNDATION_API KAN_UM_MUTATOR_DEPLOY (render_foundation_texture_management)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_cpu_static_sections_ensure_initialized ();

    kan_workflow_graph_node_depend_on (workflow_node, KAN_RESOURCE_PROVIDER_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_RENDER_FOUNDATION_FRAME_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_RENDER_FOUNDATION_TEXTURE_MANAGEMENT_BEGIN_CHECKPOINT);
    kan_workflow_graph_node_make_dependency_of (workflow_node, KAN_RENDER_FOUNDATION_TEXTURE_MANAGEMENT_END_CHECKPOINT);
}

static inline enum kan_render_image_format_t texture_format_to_render_format (enum kan_resource_texture_format_t format)
{
    switch (format)
    {
    case KAN_RESOURCE_TEXTURE_FORMAT_UNCOMPRESSED_R8_SRGB:
        return KAN_RENDER_IMAGE_FORMAT_R8_SRGB;

    case KAN_RESOURCE_TEXTURE_FORMAT_UNCOMPRESSED_RG16_SRGB:
        return KAN_RENDER_IMAGE_FORMAT_RG16_SRGB;

    case KAN_RESOURCE_TEXTURE_FORMAT_UNCOMPRESSED_RGBA32_SRGB:
        return KAN_RENDER_IMAGE_FORMAT_RGBA32_SRGB;

    case KAN_RESOURCE_TEXTURE_FORMAT_UNCOMPRESSED_R8_UNORM:
        return KAN_RENDER_IMAGE_FORMAT_R8_UNORM;

    case KAN_RESOURCE_TEXTURE_FORMAT_UNCOMPRESSED_RG16_UNORM:
        return KAN_RENDER_IMAGE_FORMAT_RG16_UNORM;

    case KAN_RESOURCE_TEXTURE_FORMAT_UNCOMPRESSED_RGBA32_UNORM:
        return KAN_RENDER_IMAGE_FORMAT_RGBA32_UNORM;

    case KAN_RESOURCE_TEXTURE_FORMAT_UNCOMPRESSED_D16:
        return KAN_RENDER_IMAGE_FORMAT_D16_UNORM;

    case KAN_RESOURCE_TEXTURE_FORMAT_UNCOMPRESSED_D32:
        return KAN_RENDER_IMAGE_FORMAT_D32_SFLOAT;
    }

    KAN_ASSERT (false)
    return KAN_RENDER_IMAGE_FORMAT_RGBA32_SRGB;
}

static void recalculate_requested_mip (struct render_foundation_texture_management_state_t *state,
                                       const struct kan_resource_provider_singleton_t *provider,
                                       struct kan_render_texture_loaded_t *texture)
{
    texture->requested_best_mip = texture->streamed_mips.size;
    texture->requested_best_mip_frame_id = provider->logic_deduplication_frame_id;

    KAN_UML_VALUE_READ (quality, kan_render_texture_quality_t, name, &texture->name)
    {
        texture->requested_best_mip = KAN_MIN (texture->requested_best_mip, quality->best_advised_mip);
    }
}

static void remove_streaming_requests (struct render_foundation_texture_management_state_t *state,
                                       struct kan_render_texture_loaded_t *texture)
{
    KAN_UML_VALUE_DETACH (streaming, render_foundation_texture_streaming_t, texture_name, &texture->name)
    {
        KAN_UM_ACCESS_DELETE (streaming);
    }
}

static void create_streaming_requests (struct render_foundation_texture_management_state_t *state,
                                       const struct kan_resource_provider_singleton_t *provider,
                                       struct kan_render_texture_loaded_t *texture)
{
    KAN_ASSERT (texture->current_best_mip <= texture->streamed_mips.size)
    for (kan_instance_size_t index = texture->requested_best_mip; index < texture->current_best_mip; ++index)
    {
        kan_interned_string_t data_name = ((kan_interned_string_t *) texture->streamed_mips.data)[index];
        KAN_UMI_INDEXED_INSERT (binding, render_foundation_texture_streaming_t)
        binding->texture_name = texture->name;
        binding->data_name = data_name;
        binding->request_id = kan_next_resource_streaming_id (provider);

        KAN_UMI_INDEXED_INSERT (request, kan_resource_streaming_request_t)
        request->request_id = binding->request_id;
        request->type = KAN_STATIC_INTERNED_ID_GET (kan_resource_streamed_texture_data_t);
        request->name = data_name;
        request->priority = KAN_UNIVERSE_RENDER_FOUNDATION_TEXTURE_PRIORITY;
    }
}

static void load_texture (struct render_foundation_texture_management_state_t *state,
                          const struct kan_resource_provider_singleton_t *provider,
                          const struct kan_render_context_singleton_t *render_context,
                          const struct kan_resource_texture_t *resource,
                          struct kan_render_texture_loaded_t *loaded)
{
    KAN_CPU_SCOPED_STATIC_SECTION (load_texture)
    remove_streaming_requests (state, loaded);

    if (KAN_HANDLE_IS_VALID (loaded->image))
    {
        kan_render_image_destroy (loaded->image);
        loaded->image = KAN_HANDLE_SET_INVALID (kan_render_image_t);
    }

    loaded->streamed_mips.size = 0u;
    kan_dynamic_array_set_capacity (&loaded->streamed_mips, resource->streamed_mips);
    loaded->width = resource->width;
    loaded->height = resource->height;
    loaded->depth = resource->depth;
    loaded->inlined_mips_count = resource->inlined_mips;

    loaded->current_best_mip = resource->streamed_mips;
    loaded->requested_best_mip = resource->streamed_mips;
    loaded->requested_best_mip_frame_id = 0u;
    loaded->streaming_frame_id = 0u;

    bool format_selected = false;
    kan_instance_size_t selected_format_item_index = 0u;

    for (kan_memory_size_t index = 0u; index < resource->formats.size; ++index)
    {
        const struct kan_resource_texture_format_item_t *format_item =
            &((struct kan_resource_texture_format_item_t *) resource->formats.data)[index];

        const enum kan_render_image_format_t render_format = texture_format_to_render_format (format_item->format);
        const uint8_t required_flags =
            KAN_RENDER_IMAGE_FORMAT_SUPPORT_FLAG_TRANSFER | KAN_RENDER_IMAGE_FORMAT_SUPPORT_FLAG_SAMPLED;

        if ((render_context->selected_device_info->image_format_support[render_format] & required_flags) ==
            required_flags)
        {
            selected_format_item_index = (kan_instance_size_t) index;
            format_selected = true;
            break;
        }
    }

    if (!format_selected)
    {
        KAN_LOG (render_foundation_texture, KAN_LOG_ERROR,
                 "Failed to load texture \"%s\" as there is no format with proper support flags.", loaded->name)

        // Send updated event anyway so users can unbind the broken texture.
        KAN_UMO_EVENT_INSERT_INIT (kan_render_texture_updated_event_t) {.name = loaded->name};
        return;
    }

    const struct kan_resource_texture_format_item_t *format_item =
        &((struct kan_resource_texture_format_item_t *) resource->formats.data)[selected_format_item_index];

    if (format_item->streamed_mips.size > 0u)
    {
        loaded->streamed_mips.size = format_item->streamed_mips.size;
        memcpy (loaded->streamed_mips.data, format_item->streamed_mips.data,
                sizeof (kan_interned_string_t) * format_item->streamed_mips.size);
    }

    KAN_ASSERT (format_item->inlined_mips.size == loaded->inlined_mips_count)
    KAN_ASSERT (loaded->inlined_mips_count > 0u)
    const kan_instance_size_t inlined_first_mip = resource->streamed_mips;
    loaded->format = format_item->format;

    struct kan_render_image_description_t description = {
        .format = texture_format_to_render_format (format_item->format),
        .width = KAN_MAX (1u, resource->width >> inlined_first_mip),
        .height = KAN_MAX (1u, resource->height >> inlined_first_mip),
        .depth = KAN_MAX (1u, resource->depth >> inlined_first_mip),
        .layers = 1u,
        .mips = (uint8_t) loaded->inlined_mips_count,

        .render_target = false,
        .supports_sampling = true,
        .always_treat_as_layered = false,
        .tracking_name = loaded->name,
    };

    kan_render_image_t new_image = kan_render_image_create (render_context->render_context, &description);
    if (!KAN_HANDLE_IS_VALID (new_image))
    {
        KAN_LOG (render_foundation_texture, KAN_LOG_ERROR,
                 "Failed to finish loading of texture \"%s\" as GPU image creation has failed.", loaded->name)

        // Send updated event anyway so users can unbind the broken texture.
        KAN_UMO_EVENT_INSERT_INIT (kan_render_texture_updated_event_t) {.name = loaded->name};
        return;
    }

    loaded->image = new_image;
    for (kan_instance_size_t index = 0u; index < format_item->inlined_mips.size; ++index)
    {
        struct kan_resource_inlined_texture_data_t *data =
            &((struct kan_resource_inlined_texture_data_t *) format_item->inlined_mips.data)[index];
        kan_render_image_upload_data (new_image, 0u, (uint8_t) index, data->data.size, data->data.data);
    }

    KAN_UMO_EVENT_INSERT_INIT (kan_render_texture_updated_event_t) {.name = loaded->name};
    recalculate_requested_mip (state, provider, loaded);
    create_streaming_requests (state, provider, loaded);
}

static void unload_excessive_mips (struct render_foundation_texture_management_state_t *state,
                                   const struct kan_render_context_singleton_t *render_context,
                                   struct kan_render_texture_loaded_t *loaded)
{
    if (!KAN_HANDLE_IS_VALID (loaded->image) || loaded->requested_best_mip <= loaded->current_best_mip)
    {
        return;
    }

    KAN_CPU_SCOPED_STATIC_SECTION (unload_excessive_mips)
    const kan_instance_size_t total_mips = loaded->streamed_mips.size + loaded->inlined_mips_count;
    KAN_ASSERT (loaded->requested_best_mip < total_mips)

    struct kan_render_image_description_t description = {
        .format = texture_format_to_render_format (loaded->format),
        .width = KAN_MAX (1u, loaded->width >> loaded->requested_best_mip),
        .height = KAN_MAX (1u, loaded->height >> loaded->requested_best_mip),
        .depth = KAN_MAX (1u, loaded->depth >> loaded->requested_best_mip),
        .layers = 1u,
        .mips = (uint8_t) (total_mips - loaded->requested_best_mip),

        .render_target = false,
        .supports_sampling = true,
        .always_treat_as_layered = false,
        .tracking_name = loaded->name,
    };

    kan_render_image_t new_image = kan_render_image_create (render_context->render_context, &description);
    if (!KAN_HANDLE_IS_VALID (new_image))
    {
        KAN_LOG (render_foundation_texture, KAN_LOG_ERROR,
                 "Failed to unload excessive mips of texture \"%s\" as GPU image creation has failed.", loaded->name)
        return;
    }

    kan_render_image_t old_image = loaded->image;
    CUSHION_DEFER { kan_render_image_destroy (old_image); }
    loaded->image = new_image;

    for (kan_instance_size_t absolute_mip = loaded->requested_best_mip; absolute_mip < total_mips; ++absolute_mip)
    {
        const kan_instance_size_t new_mip = absolute_mip - loaded->requested_best_mip;
        const kan_instance_size_t old_mip = absolute_mip - loaded->current_best_mip;
        kan_render_image_copy_data (old_image, 0u, (uint8_t) old_mip, new_image, 0u, (uint8_t) new_mip);
    }

    loaded->current_best_mip = loaded->requested_best_mip;
    KAN_UMO_EVENT_INSERT_INIT (kan_render_texture_updated_event_t) {.name = loaded->name};
}

static void on_quality_insert_or_delete (struct render_foundation_texture_management_state_t *state,
                                         const struct kan_resource_provider_singleton_t *provider,
                                         const struct kan_render_context_singleton_t *render_context,
                                         kan_interned_string_t texture_name)
{
    KAN_UMI_VALUE_UPDATE_OPTIONAL (loaded, kan_render_texture_loaded_t, name, &texture_name)
    if (!loaded || loaded->requested_best_mip_frame_id == provider->logic_deduplication_frame_id)
    {
        return;
    }

    const kan_instance_size_t old_required_mip = loaded->requested_best_mip;
    recalculate_requested_mip (state, provider, loaded);

    if (loaded->requested_best_mip != old_required_mip)
    {
        loaded->streaming_frame_id = 0u;
        // We just call all the functions for simplicity: create and unload will do checks anyway.
        remove_streaming_requests (state, loaded);
        create_streaming_requests (state, provider, loaded);
        unload_excessive_mips (state, render_context, loaded);
    }
}

static void update_texture_streaming (struct render_foundation_texture_management_state_t *state,
                                      const struct kan_resource_provider_singleton_t *provider,
                                      const struct kan_render_context_singleton_t *render_context,
                                      kan_interned_string_t texture_name)
{
    KAN_UMI_VALUE_UPDATE_OPTIONAL (loaded, kan_render_texture_loaded_t, name, &texture_name)
    if (!loaded || loaded->streaming_frame_id == provider->logic_deduplication_frame_id)
    {
        return;
    }

    KAN_CPU_SCOPED_STATIC_SECTION (update_texture_streaming)
    loaded->streaming_frame_id = provider->logic_deduplication_frame_id;

    if (loaded->current_best_mip <= loaded->requested_best_mip)
    {
        // Quality is already updated and no stream in is needed. Just remove the request.
        remove_streaming_requests (state, loaded);
        return;
    }

    bool all_data_loaded = true;
    KAN_ASSERT (loaded->current_best_mip <= loaded->streamed_mips.size)

    for (kan_instance_size_t streamed_mip = loaded->requested_best_mip; streamed_mip < loaded->current_best_mip;
         ++streamed_mip)
    {
        kan_interned_string_t data_name = ((kan_interned_string_t *) loaded->streamed_mips.data)[streamed_mip];
        KAN_UMI_RESOURCE_RETRIEVE_FRESH_LOADED (mip_data, kan_resource_streamed_texture_data_t, &data_name)

        if (!mip_data)
        {
            all_data_loaded = false;
            break;
        }
    }

    if (!all_data_loaded)
    {
        return;
    }

    const kan_instance_size_t total_mips = loaded->streamed_mips.size + loaded->inlined_mips_count;
    KAN_ASSERT (loaded->requested_best_mip < total_mips)

    struct kan_render_image_description_t description = {
        .format = texture_format_to_render_format (loaded->format),
        .width = KAN_MAX (1u, loaded->width >> loaded->requested_best_mip),
        .height = KAN_MAX (1u, loaded->height >> loaded->requested_best_mip),
        .depth = KAN_MAX (1u, loaded->depth >> loaded->requested_best_mip),
        .layers = 1u,
        .mips = (uint8_t) (total_mips - loaded->requested_best_mip),

        .render_target = false,
        .supports_sampling = true,
        .always_treat_as_layered = false,
        .tracking_name = loaded->name,
    };

    kan_render_image_t new_image = kan_render_image_create (render_context->render_context, &description);
    if (!KAN_HANDLE_IS_VALID (new_image))
    {
        KAN_LOG (render_foundation_texture, KAN_LOG_ERROR,
                 "Failed to stream in new mips of texture \"%s\" as GPU image creation has failed.", loaded->name)
        return;
    }

    kan_render_image_t old_image = loaded->image;
    CUSHION_DEFER { kan_render_image_destroy (old_image); }
    loaded->image = new_image;

    for (kan_instance_size_t absolute_mip = loaded->requested_best_mip; absolute_mip < total_mips; ++absolute_mip)
    {
        const kan_instance_size_t new_mip = absolute_mip - loaded->requested_best_mip;
        if (absolute_mip >= loaded->current_best_mip)
        {
            // Copy already existent mip.
            const kan_instance_size_t old_mip = absolute_mip - loaded->current_best_mip;
            kan_render_image_copy_data (old_image, 0u, (uint8_t) old_mip, new_image, 0u, (uint8_t) new_mip);
        }
        else
        {
            // Upload newly streamed mip.
            KAN_ASSERT (absolute_mip < loaded->streamed_mips.size)
            kan_interned_string_t data_name = ((kan_interned_string_t *) loaded->streamed_mips.data)[absolute_mip];
            KAN_UMI_RESOURCE_RETRIEVE_FRESH_LOADED (mip_data, kan_resource_streamed_texture_data_t, &data_name)
            KAN_ASSERT (mip_data)
            kan_render_image_upload_data (new_image, 0u, (uint8_t) new_mip, mip_data->data.size, mip_data->data.data);
        }
    }

    loaded->current_best_mip = loaded->requested_best_mip;
    KAN_UMO_EVENT_INSERT_INIT (kan_render_texture_updated_event_t) {.name = loaded->name};
    // Requests are removed as they're no longer needed.
    remove_streaming_requests (state, loaded);
}

UNIVERSE_RENDER_FOUNDATION_API KAN_UM_MUTATOR_EXECUTE (render_foundation_texture_management)
{
    KAN_UMI_SINGLETON_READ (render_context, kan_render_context_singleton_t)
    if (!KAN_HANDLE_IS_VALID (render_context->render_context))
    {
        return;
    }

    KAN_UMI_SINGLETON_READ (provider, kan_resource_provider_singleton_t)
    if (provider->transaction_state == KAN_RESOURCE_TRANSACTION_STATE_COMMIT)
    {
        KAN_UML_RESOURCE_LOADED_EVENT_FETCH (loaded_event, kan_resource_texture_t)
        {
            KAN_UMI_RESOURCE_RETRIEVE_FRESH_LOADED (resource, kan_resource_texture_t, &loaded_event->name)
            KAN_UMI_VALUE_UPDATE_OPTIONAL (existing_loaded, kan_render_texture_loaded_t, name, &loaded_event->name)

            if (existing_loaded)
            {
                load_texture (state, provider, render_context, resource, existing_loaded);
            }
            else
            {
                KAN_UMO_INDEXED_INSERT (new_loaded, kan_render_texture_loaded_t)
                {
                    new_loaded->name = loaded_event->name;
                    load_texture (state, provider, render_context, resource, new_loaded);
                }
            }
        }
    }

    if (provider->transaction_state == KAN_RESOURCE_TRANSACTION_STATE_NONE)
    {
        KAN_UML_RESOURCE_UNLOAD_PLANNED_EVENT_FETCH (unload_event, kan_resource_texture_t)
        {
            KAN_UMI_VALUE_DELETE_OPTIONAL (existing_loaded, kan_render_texture_loaded_t, name, &unload_event->name)
            if (existing_loaded)
            {
                KAN_UM_ACCESS_DELETE (existing_loaded);
            }
        }

        KAN_UML_EVENT_FETCH (on_insert_event, render_foundation_texture_quality_on_insert_event_t)
        {
            on_quality_insert_or_delete (state, provider, render_context, on_insert_event->texture_name);
        }

        KAN_UML_EVENT_FETCH (on_delete_event, render_foundation_texture_quality_on_delete_event_t)
        {
            on_quality_insert_or_delete (state, provider, render_context, on_delete_event->texture_name);
        }

        KAN_UML_RESOURCE_LOADED_EVENT_FETCH (loaded_event, kan_resource_streamed_texture_data_t)
        {
            KAN_UML_VALUE_READ (streaming, render_foundation_texture_streaming_t, data_name, &loaded_event->name)
            {
                const kan_interned_string_t texture_name = streaming->texture_name;
                KAN_UM_ACCESS_CLOSE_IMMEDIATELY (streaming);
                update_texture_streaming (state, provider, render_context, texture_name);
            }
        }
    }
}

void kan_render_texture_quality_init (struct kan_render_texture_quality_t *instance)
{
    instance->quality_id = KAN_TYPED_ID_32_SET_INVALID (kan_render_texture_quality_id_t);
    instance->name = NULL;
    instance->best_advised_mip = 0u;
}

void kan_render_texture_singleton_init (struct kan_render_texture_singleton_t *instance)
{
    instance->quality_id_counter = kan_atomic_int_init (1);
}

void kan_render_texture_loaded_init (struct kan_render_texture_loaded_t *instance)
{
    instance->name = NULL;
    instance->image = KAN_HANDLE_SET_INVALID (kan_render_image_t);
    kan_dynamic_array_init (&instance->streamed_mips, 0u, sizeof (kan_interned_string_t),
                            alignof (kan_interned_string_t), kan_allocation_group_stack_get ());

    instance->width = 0u;
    instance->height = 0u;
    instance->depth = 0u;
    instance->format = KAN_RESOURCE_TEXTURE_FORMAT_UNCOMPRESSED_R8_SRGB;
    instance->inlined_mips_count = 0u;

    instance->current_best_mip = 0u;
    instance->requested_best_mip = 0u;
    instance->requested_best_mip_frame_id = 0u;
    instance->streaming_frame_id = 0u;
}

void kan_render_texture_loaded_shutdown (struct kan_render_texture_loaded_t *instance)
{
    if (KAN_HANDLE_IS_VALID (instance->image))
    {
        kan_render_image_destroy (instance->image);
    }

    kan_dynamic_array_shutdown (&instance->streamed_mips);
}
