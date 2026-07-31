#include <qsort.h>
#include <stddef.h>

#include <kan/api_common/alignment.h>
#include <kan/api_common/min_max.h>
#include <kan/context/all_system_names.h>
#include <kan/context/hot_reload_coordination_system.h>
#include <kan/context/reflection_system.h>
#include <kan/context/virtual_file_system.h>
#include <kan/log/logging.h>
#include <kan/platform/hardware.h>
#include <kan/precise_time/precise_time.h>
#include <kan/resource_pipeline/index.h>
#include <kan/resource_pipeline/meta.h>
#include <kan/serialization/binary.h>
#include <kan/stream/random_access_stream_buffer.h>
#include <kan/universe/macro.h>
#include <kan/universe/reflection_system_generator_helpers.h>
#include <kan/universe/universe.h>
#include <kan/virtual_file_system/virtual_file_system.h>
#include "provider.h"

KAN_LOG_DEFINE_CATEGORY (universe_resource_provider);
KAN_USE_STATIC_INTERNED_IDS
KAN_USE_STATIC_CPU_SECTIONS

UNIVERSE_RESOURCE_PROVIDER_API KAN_UM_MUTATOR_GROUP_META (resource_provider, KAN_RESOURCE_PROVIDER_MUTATOR_GROUP);

struct resource_provider_private_singleton_t
{
    kan_instance_size_t entry_id_counter;
    kan_hot_reload_virtual_file_event_provider_t file_event_provider;
};

UNIVERSE_RESOURCE_PROVIDER_API void resource_provider_private_singleton_init (
    struct resource_provider_private_singleton_t *instance)
{
    instance->entry_id_counter = KAN_TYPED_ID_32_INVALID_LITERAL;
    instance->file_event_provider = KAN_HANDLE_SET_INVALID (kan_hot_reload_virtual_file_event_provider_t);
}

UNIVERSE_RESOURCE_PROVIDER_API void resource_provider_private_singleton_shutdown (
    struct resource_provider_private_singleton_t *instance)
{
    if (KAN_HANDLE_IS_VALID (instance->file_event_provider))
    {
        kan_hot_reload_virtual_file_event_provider_destroy (instance->file_event_provider);
    }
}

struct resource_streaming_request_on_insert_event_t
{
    kan_interned_string_t type;
    kan_interned_string_t name;
};

KAN_REFLECTION_STRUCT_META (kan_resource_streaming_request_t)
UNIVERSE_RESOURCE_PROVIDER_API struct kan_repository_meta_automatic_on_insert_event_t
    resource_streaming_request_on_insert = {
        .event_type = "resource_streaming_request_on_insert_event_t",
        .copy_outs_count = 2u,
        .copy_outs =
            (struct kan_repository_copy_out_t[]) {
                {
                    .source_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"type"}},
                    .target_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"type"}},
                },
                {
                    .source_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"name"}},
                    .target_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"name"}},
                },
            },
};

struct resource_streaming_request_on_delete_event_t
{
    kan_interned_string_t type;
    kan_interned_string_t name;
};

KAN_REFLECTION_STRUCT_META (kan_resource_streaming_request_t)
UNIVERSE_RESOURCE_PROVIDER_API struct kan_repository_meta_automatic_on_delete_event_t
    resource_streaming_request_on_delete = {
        .event_type = "resource_streaming_request_on_delete_event_t",
        .copy_outs_count = 2u,
        .copy_outs =
            (struct kan_repository_copy_out_t[]) {
                {
                    .source_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"type"}},
                    .target_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"type"}},
                },
                {
                    .source_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"name"}},
                    .target_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"name"}},
                },
            },
};

struct resource_loading_native_t
{
    /// \details We don't need generic entry access and we could go to typed entry from loading function right away.
    ///          Therefore, we need to cache type here to avoid getting type from generic entry.
    kan_interned_string_t type;

    kan_serialization_binary_reader_t binary_reader;
};

struct resource_loading_third_party_t
{
    kan_stable_size_t read;
    kan_stable_size_t size;
};

struct resource_provider_operation_state_t
{
    bool is_native;
    struct kan_stream_t *stream;

    KAN_REFLECTION_VISIBILITY_CONDITION_FIELD (is_native)
    KAN_REFLECTION_VISIBILITY_CONDITION_VALUE (true)
    struct resource_loading_native_t native;

    KAN_REFLECTION_VISIBILITY_CONDITION_FIELD (is_native)
    KAN_REFLECTION_VISIBILITY_CONDITION_VALUE (true)
    struct resource_loading_third_party_t third_party;
};

UNIVERSE_RESOURCE_PROVIDER_API void resource_provider_operation_state_init (
    struct resource_provider_operation_state_t *instance)
{
    instance->is_native = true;
    instance->stream = NULL;
    instance->native.type = NULL;
    instance->native.binary_reader = KAN_HANDLE_SET_INVALID (kan_serialization_binary_reader_t);
}

UNIVERSE_RESOURCE_PROVIDER_API void resource_provider_operation_state_shutdown (
    struct resource_provider_operation_state_t *instance)
{
    if (instance->is_native)
    {
        if (KAN_HANDLE_IS_VALID (instance->native.binary_reader))
        {
            kan_serialization_binary_reader_destroy (instance->native.binary_reader);
        }
    }

    if (instance->stream)
    {
        instance->stream->operations->close (instance->stream);
    }
}

struct resource_provider_loading_operation_t
{
    kan_resource_entry_id_t entry_id;
    struct resource_provider_operation_state_t state;
};

UNIVERSE_RESOURCE_PROVIDER_API void resource_provider_loading_operation_init (
    struct resource_provider_loading_operation_t *instance)
{
    instance->entry_id = KAN_TYPED_ID_32_SET_INVALID (kan_resource_entry_id_t);
    resource_provider_operation_state_init (&instance->state);
}

UNIVERSE_RESOURCE_PROVIDER_API void resource_provider_loading_operation_shutdown (
    struct resource_provider_loading_operation_t *instance)
{
    resource_provider_operation_state_shutdown (&instance->state);
}

struct resource_provider_streaming_operation_t
{
    kan_resource_entry_id_t entry_id;
    struct resource_provider_operation_state_t state;
    kan_instance_size_t priority;
    kan_instance_size_t priority_frame_id;
};

UNIVERSE_RESOURCE_PROVIDER_API void resource_provider_streaming_operation_init (
    struct resource_provider_streaming_operation_t *instance)
{
    instance->entry_id = KAN_TYPED_ID_32_SET_INVALID (kan_resource_entry_id_t);
    instance->priority = 0u;
    instance->priority_frame_id = 0u;
    resource_provider_operation_state_init (&instance->state);
}

UNIVERSE_RESOURCE_PROVIDER_API void resource_provider_streaming_operation_shutdown (
    struct resource_provider_streaming_operation_t *instance)
{
    resource_provider_operation_state_shutdown (&instance->state);
}

struct resource_provider_transactional_flip_event_t
{
    kan_resource_entry_id_t entry_id;
    kan_interned_string_t type;
};

struct resource_provider_transactional_unload_event_t
{
    kan_resource_entry_id_t entry_id;
    kan_interned_string_t type;
};

KAN_REFLECTION_IGNORE
struct universe_resource_provider_generated_node_t
{
    struct universe_resource_provider_generated_node_t *next;
    const struct kan_reflection_struct_t *source_resource_type;
    bool streamed;
    bool transitively_loaded;

    struct kan_reflection_struct_t loaded_entry_type;
    struct kan_reflection_struct_t registered_event_type;
    struct kan_reflection_struct_t updated_event_type;
    struct kan_reflection_struct_t loaded_event_type;
    struct kan_reflection_struct_t unload_planned_event_type;
    struct kan_reflection_struct_t unregistered_event_type;
};

struct kan_reflection_generator_universe_resource_provider_t
{
    kan_memory_size_t boostrap_iteration;
    kan_allocation_group_t generated_reflection_group;
    struct universe_resource_provider_generated_node_t *first_node;
    kan_instance_size_t nodes_count;

    KAN_REFLECTION_IGNORE
    struct kan_reflection_struct_t mutator_type;

    KAN_REFLECTION_IGNORE
    struct kan_reflection_function_t mutator_deploy_function;

    KAN_REFLECTION_IGNORE
    struct kan_reflection_function_t mutator_execute_function;

    KAN_REFLECTION_IGNORE
    struct kan_reflection_function_t mutator_undeploy_function;
};

struct resource_provider_resource_type_interface_t
{
    kan_interned_string_t resource_type_name;

    struct kan_repository_indexed_insert_query_t insert_loaded_entry;
    struct kan_repository_indexed_value_read_query_t read_loaded_entry_by_id;
    struct kan_repository_indexed_value_update_query_t update_loaded_entry_by_id;
    struct kan_repository_indexed_value_delete_query_t delete_loaded_entry_by_id;
    struct kan_repository_indexed_value_write_query_t write_loaded_entry_by_id;

    struct kan_repository_event_insert_query_t insert_registered_event;
    struct kan_repository_event_insert_query_t insert_updated_event;
    struct kan_repository_event_insert_query_t insert_loaded_event;
    struct kan_repository_event_insert_query_t insert_unload_planned_event;
    struct kan_repository_event_insert_query_t insert_unregistered_event;

    struct universe_resource_provider_generated_node_t *source_node;
};

KAN_REFLECTION_IGNORE
struct resource_provider_execution_shared_state_t
{
    struct kan_atomic_int_t workers_left;
    struct kan_atomic_int_t concurrency_lock;
    struct kan_repository_indexed_sequence_write_cursor_t loading_cursor;
    struct kan_repository_indexed_interval_descending_write_cursor_t streaming_cursor;
    kan_stable_size_t end_time_ns;

    /// \brief Private and private write access are shared between everyone exclusively for id counter usage.
    struct kan_repository_singleton_write_access_t private_access;

    struct resource_provider_private_singleton_t *private;
    kan_cpu_job_t job;
};

struct resource_provider_state_t
{
    kan_allocation_group_t my_allocation_group;
    struct kan_resource_provider_configuration_t configuration;

    kan_reflection_registry_t reflection_registry;
    kan_serialization_binary_script_storage_t shared_script_storage;
    kan_context_system_t hot_reload_system;
    kan_context_system_t virtual_file_system;

    KAN_UM_GENERATE_STATE_QUERIES (resource_provider)
    KAN_UM_BIND_STATE (resource_provider, state)

    struct kan_repository_indexed_insert_query_t insert__kan_resource_loaded_third_party_entry;
    struct kan_repository_indexed_sequence_write_query_t write_sequence__resource_provider_loading_operation;
    struct kan_repository_indexed_interval_write_query_t
        write_interval__resource_provider_streaming_operation__priority;

    KAN_REFLECTION_IGNORE
    struct resource_provider_execution_shared_state_t execution_shared_state;

    KAN_REFLECTION_IGNORE
    struct kan_stack_group_allocator_t temporary_allocator;

    kan_instance_size_t trailing_data_count;

    KAN_REFLECTION_IGNORE
    struct resource_provider_resource_type_interface_t trailing_data[];
};

UNIVERSE_RESOURCE_PROVIDER_API void resource_provider_state_init (struct resource_provider_state_t *instance)
{
    instance->my_allocation_group = kan_allocation_group_stack_get ();
    kan_resource_provider_configuration_init (&instance->configuration);
    kan_stack_group_allocator_init (&instance->temporary_allocator,
                                    kan_allocation_group_get_child (instance->my_allocation_group, "temporary"),
                                    KAN_UNIVERSE_RESOURCE_PROVIDER_TEMPORARY_CHUNK_SIZE);
}

UNIVERSE_RESOURCE_PROVIDER_API void resource_provider_state_shutdown (struct resource_provider_state_t *instance)
{
    kan_stack_group_allocator_shutdown (&instance->temporary_allocator);
}

static inline struct resource_provider_resource_type_interface_t *query_resource_type_interface (
    struct resource_provider_state_t *state, kan_interned_string_t type)
{
    KAN_UNIVERSE_REFLECTION_GENERATOR_FIND_GENERATED_STATE (struct resource_provider_resource_type_interface_t,
                                                            resource_type_name, type);
}

static inline struct kan_repository_indexed_value_read_access_t read_loaded_entry (
    struct resource_provider_resource_type_interface_t *interface, kan_resource_entry_id_t entry_id)
{
    struct kan_repository_indexed_value_read_cursor_t cursor =
        kan_repository_indexed_value_read_query_execute (&interface->read_loaded_entry_by_id, &entry_id);
    CUSHION_DEFER { kan_repository_indexed_value_read_cursor_close (&cursor); }
    return kan_repository_indexed_value_read_cursor_next (&cursor);
}

static inline struct kan_repository_indexed_value_update_access_t update_loaded_entry (
    struct resource_provider_resource_type_interface_t *interface, kan_resource_entry_id_t entry_id)
{
    struct kan_repository_indexed_value_update_cursor_t cursor =
        kan_repository_indexed_value_update_query_execute (&interface->update_loaded_entry_by_id, &entry_id);
    CUSHION_DEFER { kan_repository_indexed_value_update_cursor_close (&cursor); }
    return kan_repository_indexed_value_update_cursor_next (&cursor);
}

static bool delete_loaded_entry_by_id (struct resource_provider_resource_type_interface_t *interface,
                                       kan_resource_entry_id_t entry_id)
{
    struct kan_repository_indexed_value_delete_cursor_t cursor =
        kan_repository_indexed_value_delete_query_execute (&interface->delete_loaded_entry_by_id, &entry_id);
    CUSHION_DEFER { kan_repository_indexed_value_delete_cursor_close (&cursor); }

    struct kan_repository_indexed_value_delete_access_t access =
        kan_repository_indexed_value_delete_cursor_next (&cursor);

    if (kan_repository_indexed_value_delete_access_resolve (&access))
    {
        kan_repository_indexed_value_delete_access_delete (&access);
        return true;
    }

    return false;
}

static inline struct kan_repository_indexed_value_write_access_t write_loaded_entry (
    struct resource_provider_resource_type_interface_t *interface, kan_resource_entry_id_t entry_id)
{
    struct kan_repository_indexed_value_write_cursor_t cursor =
        kan_repository_indexed_value_write_query_execute (&interface->write_loaded_entry_by_id, &entry_id);
    CUSHION_DEFER { kan_repository_indexed_value_write_cursor_close (&cursor); }
    return kan_repository_indexed_value_write_cursor_next (&cursor);
}

UNIVERSE_RESOURCE_PROVIDER_API KAN_UM_MUTATOR_DEPLOY_SIGNATURE (mutator_template_deploy_resource_provider,
                                                                resource_provider_state_t)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_cpu_static_sections_ensure_initialized ();

    const struct kan_resource_provider_configuration_t *configuration =
        kan_universe_world_query_configuration (world, kan_string_intern (KAN_RESOURCE_PROVIDER_CONFIGURATION));

    KAN_ASSERT (configuration)
    state->configuration = *configuration;

    state->reflection_registry = kan_universe_get_reflection_registry (universe);
    state->shared_script_storage = kan_serialization_binary_script_storage_create (state->reflection_registry);

    state->hot_reload_system =
        kan_context_query (kan_universe_get_context (universe), KAN_CONTEXT_HOT_RELOAD_COORDINATION_SYSTEM_NAME);

    state->virtual_file_system =
        kan_context_query (kan_universe_get_context (universe), KAN_CONTEXT_VIRTUAL_FILE_SYSTEM_NAME);
    KAN_ASSERT (KAN_HANDLE_IS_VALID (state->virtual_file_system))

    kan_workflow_graph_node_depend_on (workflow_node, KAN_RESOURCE_PROVIDER_BEGIN_CHECKPOINT);
    kan_workflow_graph_node_make_dependency_of (workflow_node, KAN_RESOURCE_PROVIDER_END_CHECKPOINT);
}

static kan_resource_entry_id_t register_new_entry (struct resource_provider_state_t *state,
                                                   struct resource_provider_private_singleton_t *private,
                                                   kan_interned_string_t package,
                                                   kan_interned_string_t type,
                                                   kan_interned_string_t name,
                                                   const char *path)
{
    kan_resource_entry_id_t entry_id = KAN_TYPED_ID_32_SET (kan_resource_entry_id_t, ++private->entry_id_counter);
    struct resource_provider_resource_type_interface_t *interface = NULL;

    if (type)
    {
        interface = query_resource_type_interface (state, type);
        if (!interface)
        {
            KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                     "Failed to insert entry \"%s\" of type \"%s\" from path \"%s\": given type is not a known "
                     "resource type, check meta.",
                     name, type, path)
            return KAN_TYPED_ID_32_SET_INVALID (kan_resource_entry_id_t);
        }
    }

    KAN_UMO_INDEXED_INSERT (registered, kan_resource_registered_entry_t)
    {
        registered->entry_id = entry_id;
        registered->package = package;
        registered->type = type;
        registered->name = name;

        const kan_instance_size_t path_length = (kan_instance_size_t) strlen (path);
        registered->path = kan_allocate_general (registered->my_allocation_group, path_length + 1u, alignof (char));
        memcpy (registered->path, path, path_length + 1u);
        registered->path_hash = kan_string_hash (registered->path);
    }

    if (type)
    {
        struct kan_repository_event_insertion_package_t insert_event =
            kan_repository_event_insert_query_execute (&interface->insert_registered_event);
        struct kan_resource_registered_event_view_t *event = kan_repository_event_insertion_package_get (&insert_event);

        if (event)
        {
            event->entry_id = entry_id;
            event->name = name;
            kan_repository_event_insertion_package_submit (&insert_event);
        }
    }
    else
    {
        KAN_UMO_EVENT_INSERT_INIT (kan_resource_third_party_registered_event_t) {.name = name};
    }

    return entry_id;
}

static void register_new_entry_with_duplication_check (struct resource_provider_state_t *state,
                                                       struct resource_provider_private_singleton_t *private,
                                                       kan_interned_string_t package,
                                                       kan_interned_string_t type,
                                                       kan_interned_string_t name,
                                                       const char *path)
{
    KAN_UML_VALUE_READ (potential_duplicate, kan_resource_registered_entry_t, name, &name)
    {
        if (potential_duplicate->type == type)
        {
            KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                     "Failed to insert entry \"%s\" of type \"%s\" from path \"%s\" due to name collision.", name, type,
                     path)
            return;
        }
    }

    register_new_entry (state, private, package, type, name, path);
}

static bool load_package_resource_index_if_any (struct resource_provider_state_t *state,
                                                struct resource_provider_private_singleton_t *private,
                                                kan_virtual_file_system_volume_t volume,
                                                struct kan_file_system_path_container_t *path_container,
                                                struct kan_resource_package_state_t *package)
{
    const kan_instance_size_t base_length = path_container->length;
    CUSHION_DEFER { kan_file_system_path_container_reset_length (path_container, base_length); }
    kan_file_system_path_container_append (path_container, KAN_RESOURCE_INDEX_DEFAULT_NAME);

    if (!kan_virtual_file_system_check_existence (volume, path_container->path))
    {
        return false;
    }

    kan_file_system_path_container_reset_length (path_container, base_length);
    kan_file_system_path_container_append (path_container,
                                           KAN_RESOURCE_INDEX_ACCOMPANYING_STRING_REGISTRY_DEFAULT_NAME);

    if (kan_virtual_file_system_check_existence (volume, path_container->path))
    {
        struct kan_stream_t *stream = kan_virtual_file_stream_open_for_read (volume, path_container->path);
        if (!stream)
        {
            KAN_LOG (
                universe_resource_provider, KAN_LOG_ERROR,
                "Failed to read index accompanying string registry at virtual path \"%s\": unable to open read stream.",
                path_container->path)

            // We still have index, but failed to decode it.
            return true;
        }

        stream = kan_random_access_stream_buffer_open_for_read (stream, KAN_UNIVERSE_RESOURCE_PROVIDER_IO_BUFFER);
        CUSHION_DEFER { stream->operations->close (stream); }

        kan_serialization_interned_string_registry_reader_t reader =
            kan_serialization_interned_string_registry_reader_create (stream, true);
        CUSHION_DEFER { kan_serialization_interned_string_registry_reader_destroy (reader); }
        enum kan_serialization_state_t serialization_state;

        while ((serialization_state = kan_serialization_interned_string_registry_reader_step (reader)) ==
               KAN_SERIALIZATION_IN_PROGRESS)
        {
        }

        if (serialization_state == KAN_SERIALIZATION_FAILED)
        {
            KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                     "Failed to read index accompanying string registry at virtual path \"%s\": serialization error.",
                     path_container->path)

            // We still have index, but failed to decode it.
            return true;
        }

        package->string_registry = kan_serialization_interned_string_registry_reader_get (reader);
    }

    kan_file_system_path_container_reset_length (path_container, base_length);
    kan_file_system_path_container_append (path_container, KAN_RESOURCE_INDEX_DEFAULT_NAME);

    struct kan_stream_t *stream = kan_virtual_file_stream_open_for_read (volume, path_container->path);
    if (!stream)
    {
        KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                 "Failed to read resource index at virtual path \"%s\": unable to open read stream.",
                 path_container->path)

        // We still have index, but failed to decode it.
        return true;
    }

    stream = kan_random_access_stream_buffer_open_for_read (stream, KAN_UNIVERSE_RESOURCE_PROVIDER_IO_BUFFER);
    CUSHION_DEFER { stream->operations->close (stream); }

    struct kan_resource_index_t resource_index;
    kan_resource_index_init (&resource_index);
    CUSHION_DEFER { kan_resource_index_shutdown (&resource_index); }

    kan_serialization_binary_reader_t reader = kan_serialization_binary_reader_create (
        stream, &resource_index, KAN_STATIC_INTERNED_ID_GET (kan_resource_index_t), state->shared_script_storage,
        package->string_registry, kan_resource_index_get_allocation_group ());
    enum kan_serialization_state_t serialization_state;

    while ((serialization_state = kan_serialization_binary_reader_step (reader)) == KAN_SERIALIZATION_IN_PROGRESS)
    {
    }

    kan_serialization_binary_reader_destroy (reader);
    if (serialization_state == KAN_SERIALIZATION_FAILED)
    {
        KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                 "Failed to read resource index at virtual path \"%s\": serialization error.", path_container->path)

        // We still have index, but failed to decode it.
        return true;
    }

    for (kan_memory_size_t container_index = 0u; container_index < resource_index.containers.size; ++container_index)
    {
        const struct kan_resource_index_container_t *container =
            &((struct kan_resource_index_container_t *) resource_index.containers.data)[container_index];

        for (kan_memory_size_t item_index = 0u; item_index < container->items.size; ++item_index)
        {
            const struct kan_resource_index_item_t *item =
                &((struct kan_resource_index_item_t *) container->items.data)[item_index];

            kan_file_system_path_container_reset_length (path_container, base_length);
            kan_file_system_path_container_append (path_container, item->path);
            register_new_entry_with_duplication_check (state, private, package->name, container->type, item->name,
                                                       path_container->path);
        }
    }

    for (kan_memory_size_t item_index = 0u; item_index < resource_index.third_party_items.size; ++item_index)
    {
        const struct kan_resource_index_item_t *item =
            &((struct kan_resource_index_item_t *) resource_index.third_party_items.data)[item_index];

        kan_file_system_path_container_reset_length (path_container, base_length);
        kan_file_system_path_container_append (path_container, item->path);
        register_new_entry_with_duplication_check (state, private, package->name, NULL, item->name,
                                                   path_container->path);
    }

    return true;
}

struct scan_file_internal_result_t
{
    bool successful;
    kan_interned_string_t type;
    kan_interned_string_t name;
};

static struct scan_file_internal_result_t scan_file_internal (struct resource_provider_state_t *state,
                                                              struct resource_provider_private_singleton_t *private,
                                                              kan_virtual_file_system_volume_t volume,
                                                              kan_instance_size_t path_length,
                                                              const char *path)
{
    struct scan_file_internal_result_t result = {
        .successful = false,
        .type = NULL,
        .name = NULL,
    };

    const char *path_end = path + path_length;
    const char *name_begin = kan_file_system_path_walk_to_name_begin (path, path_length);

    if (path_length > 4u && *(path_end - 4u) == '.' && *(path_end - 3u) == 'b' && *(path_end - 2u) == 'i' &&
        *(path_end - 1u) == 'n')
    {
        // Known binary format, okay to move forward with header scan.
    }
    else if (path_length > 3u && *(path_end - 3u) == '.' && *(path_end - 2u) == 'r' && *(path_end - 1u) == 'd')
    {
        KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                 "Failed to scan entry at virtual path \"%s\": it looks like readable data format, but only binaries "
                 "and third party are supported as of now.",
                 path)
        return result;
    }
    else
    {
        // Treat this file as deployed resource in third party format.
        result.successful = true;
        result.name = kan_char_sequence_intern (name_begin, path_end);
        return result;
    }

    // Logic for binary format processing below.
    result.name = kan_char_sequence_intern (name_begin, path_end - 4u);
    struct kan_stream_t *stream = kan_virtual_file_stream_open_for_read (volume, path);

    if (!stream)
    {
        KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                 "Failed to scan entry at virtual path \"%s\": unable to open read stream to read type header.", path)
        return result;
    }

    stream = kan_random_access_stream_buffer_open_for_read (stream, KAN_UNIVERSE_RESOURCE_PROVIDER_TYPE_HEADER_BUFFER);
    CUSHION_DEFER { stream->operations->close (stream); }

    if (!kan_serialization_binary_read_type_header (
            stream, &result.type,
            // We expect non-indexed files to be encoded without string registries.
            KAN_HANDLE_SET_INVALID (kan_serialization_interned_string_registry_t)))
    {
        KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                 "Failed to scan entry at virtual path \"%s\": unable to read type header.", path)
        return result;
    }

    result.successful = true;
    return result;
}

static void scan_file (struct resource_provider_state_t *state,
                       struct resource_provider_private_singleton_t *private,
                       kan_virtual_file_system_volume_t volume,
                       struct kan_file_system_path_container_t *container,
                       kan_interned_string_t package)
{
    struct scan_file_internal_result_t scan_result =
        scan_file_internal (state, private, volume, container->length, container->path);

    if (scan_result.successful)
    {
        register_new_entry_with_duplication_check (state, private, package, scan_result.type, scan_result.name,
                                                   container->path);
    }
}

static void scan_directory (struct resource_provider_state_t *state,
                            struct resource_provider_private_singleton_t *private,
                            kan_virtual_file_system_volume_t volume,
                            struct kan_file_system_path_container_t *container,
                            kan_interned_string_t assigned_package_name,
                            const char *this_directory_name)
{
    const kan_instance_size_t base_length = container->length;

#if KAN_WITH_ASSERT
    // Assert that directory structure is linearized and we don't have any overlapping packages.
    if (assigned_package_name)
    {
        CUSHION_DEFER { kan_file_system_path_container_reset_length (container, base_length); }
        kan_file_system_path_container_append (container, KAN_RESOURCE_PACKAGE_FILE_NAME);

        KAN_ASSERT_FORMATTED (
            !kan_virtual_file_system_check_existence (volume, container->path),
            "Detected non-linear package structure: new package file at \"%s\" while already inside package \"%s\"!",
            container->path, assigned_package_name)
    }
#endif

    bool package_root = false;
    if (!assigned_package_name)
    {
        CUSHION_DEFER { kan_file_system_path_container_reset_length (container, base_length); }
        kan_file_system_path_container_append (container, KAN_RESOURCE_PACKAGE_FILE_NAME);

        if (kan_virtual_file_system_check_existence (volume, container->path))
        {
            KAN_UMI_INDEXED_INSERT (package, kan_resource_package_state_t)
            package_root = true;
            package->name = kan_string_intern (this_directory_name);
            assigned_package_name = package->name;

            struct kan_stream_t *stream = kan_virtual_file_stream_open_for_read (volume, container->path);
            if (!stream)
            {
                KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                         "Failed to read resource package manifest at virtual path \"%s\": unable to open read stream.",
                         container->path)
                return;
            }

            stream = kan_random_access_stream_buffer_open_for_read (stream, KAN_UNIVERSE_RESOURCE_PROVIDER_IO_BUFFER);
            CUSHION_DEFER { stream->operations->close (stream); }

            kan_serialization_binary_reader_t reader = kan_serialization_binary_reader_create (
                stream, &package->manifest, KAN_STATIC_INTERNED_ID_GET (kan_resource_package_t),
                state->shared_script_storage, KAN_HANDLE_SET_INVALID (kan_serialization_interned_string_registry_t),
                kan_resource_index_get_allocation_group ());
            enum kan_serialization_state_t serialization_state;

            while ((serialization_state = kan_serialization_binary_reader_step (reader)) ==
                   KAN_SERIALIZATION_IN_PROGRESS)
            {
            }

            kan_serialization_binary_reader_destroy (reader);
            if (serialization_state == KAN_SERIALIZATION_FAILED)
            {
                KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                         "Failed to read resource package manifest at virtual path \"%s\": serialization error",
                         container->path)
                return;
            }

            kan_file_system_path_container_reset_length (container, base_length);
            if (load_package_resource_index_if_any (state, private, volume, container, package))
            {
                // No need for further scan if resource index was read.
                return;
            }
        }
    }

    struct kan_virtual_file_system_directory_iterator_t iterator =
        kan_virtual_file_system_directory_iterator_create (volume, container->path);
    CUSHION_DEFER { kan_virtual_file_system_directory_iterator_destroy (&iterator); }
    const char *entry_name;

    while ((entry_name = kan_virtual_file_system_directory_iterator_advance (&iterator)))
    {
        if ((entry_name[0u] == '.' && entry_name[1u] == '\0') ||
            (entry_name[0u] == '.' && entry_name[1u] == '.' && entry_name[2u] == '\0'))
        {
            continue;
        }

        if (package_root && strcmp (entry_name, KAN_RESOURCE_PACKAGE_FILE_NAME) == 0)
        {
            continue;
        }

        CUSHION_DEFER { kan_file_system_path_container_reset_length (container, base_length); }
        kan_file_system_path_container_append (container, entry_name);
        struct kan_virtual_file_system_entry_status_t status;

        if (!kan_virtual_file_system_query_entry (volume, container->path, &status))
        {
            KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                     "Failed to scan entry at virtual path \"%s\": unable to query status.", container->path)
            continue;
        }

        switch (status.type)
        {
        case KAN_VIRTUAL_FILE_SYSTEM_ENTRY_TYPE_UNKNOWN:
            KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                     "Failed to scan entry at virtual path \"%s\": type is unknown.", container->path)
            break;

        case KAN_VIRTUAL_FILE_SYSTEM_ENTRY_TYPE_FILE:
            if (!assigned_package_name)
            {
                KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                         "Failed to scan file at virtual path \"%s\": cannot scan files that are outside of any "
                         "package context.",
                         container->path)
                break;
            }

            scan_file (state, private, volume, container, assigned_package_name);
            break;

        case KAN_VIRTUAL_FILE_SYSTEM_ENTRY_TYPE_DIRECTORY:
            scan_directory (state, private, volume, container, assigned_package_name, entry_name);
            break;
        }
    }
}

static void schedule_transactional_loading (struct resource_provider_state_t *state,
                                            const struct kan_resource_registered_entry_t *entry)
{
    KAN_UMI_INDEXED_INSERT (operation, resource_provider_loading_operation_t)
    operation->entry_id = entry->entry_id;

    if (entry->type)
    {
        operation->state.is_native = true;
        operation->state.native.type = entry->type;
        operation->state.native.binary_reader = KAN_HANDLE_SET_INVALID (kan_serialization_binary_reader_t);
    }
    else
    {
        operation->state.is_native = false;
        operation->state.third_party.read = 0u;
        operation->state.third_party.size = 0u;
    }
}

static void schedule_transaction_loading_from_package (struct resource_provider_state_t *state,
                                                       struct resource_provider_private_singleton_t *private,
                                                       struct kan_resource_package_state_t *package)
{
    KAN_CPU_SCOPED_STATIC_SECTION (schedule_transaction_loading_from_package)
    KAN_UML_VALUE_READ (entry, kan_resource_registered_entry_t, package, &package->name)
    {
        if (entry->type)
        {
            struct resource_provider_resource_type_interface_t *interface =
                query_resource_type_interface (state, entry->type);

            if (interface->source_node->streamed)
            {
                // Not interested in streamed entries.
                continue;
            }
        }

#if defined(KAN_WITH_ASSERT)
        // Validate that no scheduling overlap has happened: one package cannot be normally loaded twice.
        // Hot reload is the only thing that can trigger "loading of loaded package", but hot reload should use
        // its own path for scheduling instead of this one.
        if (entry->type)
        {
            struct resource_provider_resource_type_interface_t *interface =
                query_resource_type_interface (state, entry->type);
            struct kan_repository_indexed_value_read_access_t access = read_loaded_entry (interface, entry->entry_id);

            if (kan_repository_indexed_value_read_access_resolve (&access))
            {
                KAN_ASSERT (false)
                kan_repository_indexed_value_read_access_close (&access);
            }
        }
        else
        {
            KAN_UMI_VALUE_READ_OPTIONAL (loaded, kan_resource_loaded_third_party_entry_t, entry_id, &entry->entry_id)
            KAN_ASSERT (!loaded)
        }
#endif

        schedule_transactional_loading (state, entry);
    }
}

static void send_unload_planned_events (struct resource_provider_state_t *state,
                                        const struct kan_resource_registered_entry_t *entry,
                                        struct resource_provider_resource_type_interface_t *interface)
{
    if (interface)
    {
        struct kan_repository_event_insertion_package_t insert_event =
            kan_repository_event_insert_query_execute (&interface->insert_unload_planned_event);

        struct kan_resource_unload_planned_event_view_t *event =
            kan_repository_event_insertion_package_get (&insert_event);

        if (event)
        {
            event->entry_id = entry->entry_id;
            event->name = entry->name;
            kan_repository_event_insertion_package_submit (&insert_event);
        }
    }
    else
    {
        KAN_UMO_EVENT_INSERT_INIT (kan_resource_third_party_unload_planned_event_t) {.name = entry->name};
    }
}

static void plan_transactional_unload (struct resource_provider_state_t *state,
                                       const struct kan_resource_registered_entry_t *entry)
{
    KAN_UMO_EVENT_INSERT_INIT (resource_provider_transactional_unload_event_t) {
        .entry_id = entry->entry_id,
        .type = entry->type,
    };
}

static void schedule_transaction_unloading_from_package (struct resource_provider_state_t *state,
                                                         struct resource_provider_private_singleton_t *private,
                                                         struct kan_resource_package_state_t *package)
{
    KAN_CPU_SCOPED_STATIC_SECTION (schedule_transaction_unloading_from_package)
    KAN_UML_VALUE_READ (entry, kan_resource_registered_entry_t, package, &package->name)
    {
        struct resource_provider_resource_type_interface_t *interface = NULL;
        if (entry->type)
        {
            interface = query_resource_type_interface (state, entry->type);
            if (interface->source_node->streamed)
            {
                // Not interested in streamed entries.
                continue;
            }
        }

        send_unload_planned_events (state, entry, interface);
        plan_transactional_unload (state, entry);
    }
}

static void start_unconditional_loading_transaction (struct resource_provider_state_t *state,
                                                     struct kan_resource_provider_singleton_t *public,
                                                     struct resource_provider_private_singleton_t *private,
                                                     enum kan_resource_package_level_t level)
{
    KAN_CPU_SCOPED_STATIC_SECTION (start_unconditional_loading_transaction)
    public->transaction_state = KAN_RESOURCE_TRANSACTION_STATE_LOADING;

    KAN_UML_SEQUENCE_UPDATE (package, kan_resource_package_state_t)
    {
        if (package->manifest.level != level)
        {
            continue;
        }

        package->loaded = true;
        schedule_transaction_loading_from_package (state, private, package);
    }
}

static bool start_optional_loading_transaction (struct resource_provider_state_t *state,
                                                struct kan_resource_provider_singleton_t *public,
                                                struct resource_provider_private_singleton_t *private)
{
    KAN_CPU_SCOPED_STATIC_SECTION (start_optional_loading_transaction)
    bool transaction_triggered = false;

    KAN_UML_SEQUENCE_UPDATE (package, kan_resource_package_state_t)
    {
        if (package->manifest.level != KAN_RESOURCE_PACKAGE_LEVEL_OPTIONAL)
        {
            continue;
        }

        bool should_be_loaded = false;
        for (kan_instance_size_t trigger = 0u; trigger < package->manifest.trigger_tags.size; ++trigger)
        {
            kan_interned_string_t trigger_tag =
                ((kan_interned_string_t *) package->manifest.trigger_tags.data)[trigger];

            for (kan_instance_size_t found = 0u; found < public->tags.size; ++found)
            {
                kan_interned_string_t found_tag = ((kan_interned_string_t *) public->tags.data)[found];
                if (found_tag == trigger_tag)
                {
                    should_be_loaded = true;
                    break;
                }
            }

            if (should_be_loaded)
            {
                break;
            }
        }

        if (package->loaded == should_be_loaded)
        {
            continue;
        }

        package->loaded = should_be_loaded;
        transaction_triggered = true;

        if (should_be_loaded)
        {
            schedule_transaction_loading_from_package (state, private, package);
        }
        else
        {
            schedule_transaction_unloading_from_package (state, private, package);
        }
    }

    if (transaction_triggered)
    {
        public->transaction_state = KAN_RESOURCE_TRANSACTION_STATE_LOADING;
    }

    return transaction_triggered;
}

static kan_instance_size_t calculate_streaming_priority (struct resource_provider_state_t *state,
                                                         kan_interned_string_t type,
                                                         kan_interned_string_t name)
{
    kan_instance_size_t priority = 0u;
    KAN_UML_VALUE_READ (request, kan_resource_streaming_request_t, name, &name)
    {
        if (request->type == type)
        {
            priority = KAN_MAX (priority, request->priority);
        }
    }

    return priority;
}

static void start_streaming_operation (struct resource_provider_state_t *state,
                                       struct kan_resource_provider_singleton_t *public,
                                       struct kan_resource_registered_entry_t *registered)
{
    KAN_UMI_INDEXED_INSERT (operation, resource_provider_streaming_operation_t)
    operation->entry_id = registered->entry_id;
    operation->state.is_native = true;
    operation->state.native.type = registered->type;
    operation->state.native.binary_reader = KAN_HANDLE_SET_INVALID (kan_serialization_binary_reader_t);
    operation->priority = calculate_streaming_priority (state, registered->type, registered->name);
    operation->priority_frame_id = public->logic_deduplication_frame_id;
}

static void process_streaming_request_insert (struct resource_provider_state_t *state,
                                              struct kan_resource_provider_singleton_t *public,
                                              kan_interned_string_t type,
                                              kan_interned_string_t name)
{
#if defined(KAN_WITH_ASSERT)
    KAN_ASSERT (type)
    struct resource_provider_resource_type_interface_t *interface = query_resource_type_interface (state, type);
    KAN_ASSERT (interface)
    KAN_ASSERT (interface->source_node->streamed)
#endif

    KAN_UML_VALUE_UPDATE (registered, kan_resource_registered_entry_t, name, &name)
    {
        if (registered->type == type)
        {
            ++registered->streaming_counter;
            if (registered->streaming_counter == 1u)
            {
                start_streaming_operation (state, public, registered);
            }
            else
            {
                KAN_UMI_VALUE_UPDATE_OPTIONAL (operation, resource_provider_streaming_operation_t, entry_id,
                                               &registered->entry_id)

                if (operation && operation->priority_frame_id != public->logic_deduplication_frame_id)
                {
                    operation->priority = calculate_streaming_priority (state, type, name);
                    operation->priority_frame_id = public->logic_deduplication_frame_id;
                }
            }

            return;
        }
    }

    KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
             "Failed to streaming request for \"%s\" of type \"%s\": entry is not found.", name, type)
}

static void process_streaming_request_delete (struct resource_provider_state_t *state,
                                              struct kan_resource_provider_singleton_t *public,
                                              kan_interned_string_t type,
                                              kan_interned_string_t name)
{
    KAN_ASSERT (type)
    KAN_UML_VALUE_UPDATE (registered, kan_resource_registered_entry_t, name, &name)
    {
        if (registered->type == type)
        {
            KAN_ASSERT (registered->streaming_counter > 0u)
            --registered->streaming_counter;

            if (registered->streaming_counter > 0u)
            {
                KAN_UMI_VALUE_UPDATE_OPTIONAL (operation, resource_provider_streaming_operation_t, entry_id,
                                               &registered->entry_id)

                if (operation && operation->priority_frame_id != public->logic_deduplication_frame_id)
                {
                    operation->priority = calculate_streaming_priority (state, type, name);
                    operation->priority_frame_id = public->logic_deduplication_frame_id;
                }
            }
            else
            {
                KAN_UMI_VALUE_DELETE_OPTIONAL (operation, resource_provider_streaming_operation_t, entry_id,
                                               &registered->entry_id)

                if (operation)
                {
                    KAN_UM_ACCESS_DELETE (operation);
                }

                struct resource_provider_resource_type_interface_t *interface =
                    query_resource_type_interface (state, type);
                KAN_ASSERT (interface)
                delete_loaded_entry_by_id (interface, registered->entry_id);
            }

            return;
        }
    }
}

static bool is_changed_file_a_resource_manifest (struct resource_provider_state_t *state,
                                                 const char *path,
                                                 kan_instance_size_t path_length)
{
    const char *name_begin = kan_file_system_path_walk_to_name_begin (path, path_length);
    const char *name_end = path + path_length;

    return (name_end - name_begin) == sizeof (KAN_RESOURCE_PACKAGE_FILE_NAME) - 1u &&
           memcmp (name_begin, KAN_RESOURCE_PACKAGE_FILE_NAME, sizeof (KAN_RESOURCE_PACKAGE_FILE_NAME) - 1u) == 0;
}

static kan_interned_string_t extract_changed_file_package_name (struct resource_provider_state_t *state,
                                                                const char *path,
                                                                kan_instance_size_t path_length)
{
    kan_virtual_file_system_volume_t volume =
        kan_virtual_file_system_get_context_volume_for_read (state->virtual_file_system);
    CUSHION_DEFER { kan_virtual_file_system_close_context_read_access (state->virtual_file_system); }

    struct kan_file_system_path_container_t container;
    kan_file_system_path_container_copy_char_sequence (&container, path, path + path_length);

    while (true)
    {
        const char *name_begin = kan_file_system_path_walk_to_name_begin (container.path, container.length);
        if (name_begin == container.path)
        {
            break;
        }

        kan_file_system_path_container_reset_length (&container, name_begin - container.path - 1u);
        const kan_instance_size_t base_length = container.length;
        kan_file_system_path_container_append (&container, KAN_RESOURCE_PACKAGE_FILE_NAME);
        const bool is_package = kan_virtual_file_system_check_existence (volume, container.path);
        kan_file_system_path_container_reset_length (&container, base_length);

        if (is_package)
        {
            name_begin = kan_file_system_path_walk_to_name_begin (container.path, container.length);
            return kan_string_intern (name_begin);
        }
    }

    return NULL;
}

static void send_resource_updated_event (struct resource_provider_state_t *state,
                                         struct resource_provider_resource_type_interface_t *interface,
                                         kan_resource_entry_id_t entry_id,
                                         kan_interned_string_t name)
{
    if (interface)
    {
        struct kan_repository_event_insertion_package_t insert_event =
            kan_repository_event_insert_query_execute (&interface->insert_updated_event);
        struct kan_resource_updated_event_view_t *event = kan_repository_event_insertion_package_get (&insert_event);

        if (event)
        {
            event->entry_id = entry_id;
            event->name = name;
            kan_repository_event_insertion_package_submit (&insert_event);
        }
    }
    else
    {
        KAN_UMO_EVENT_INSERT_INIT (kan_resource_third_party_updated_event_t) {.name = name};
    }
}

static void cancel_streaming_operation_if_any (struct resource_provider_state_t *state,
                                               struct kan_resource_registered_entry_t *registered,
                                               struct resource_provider_resource_type_interface_t *interface)
{
    KAN_UMI_VALUE_DELETE_OPTIONAL (operation, resource_provider_streaming_operation_t, entry_id, &registered->entry_id)
    if (operation)
    {
        struct kan_repository_indexed_value_update_access_t access =
            update_loaded_entry (interface, registered->entry_id);
        struct kan_resource_loaded_entry_view_t *view = kan_repository_indexed_value_update_access_resolve (&access);

        if (view)
        {
            // We have to clear loading data from previous operation if it was running.
            if (view->loading_data)
            {
                if (interface->source_node->source_resource_type->shutdown)
                {
                    kan_allocation_group_stack_push (view->my_allocation_group);
                    interface->source_node->source_resource_type->shutdown (
                        interface->source_node->source_resource_type->functor_user_data, view->loading_data);
                    kan_allocation_group_stack_pop ();
                }

                kan_free_batched (view->my_allocation_group, view->loading_data);
                view->loading_data = NULL;
            }

            kan_repository_indexed_value_update_access_close (&access);
        }

        KAN_UM_ACCESS_DELETE (operation);
    }
}

static bool is_loading_operation_for_entry_exists (struct resource_provider_state_t *state,
                                                   kan_resource_entry_id_t entry_id)
{
    KAN_UMI_VALUE_READ_OPTIONAL (existent_operation, resource_provider_loading_operation_t, entry_id, &entry_id)
    // Could only happen if we got add-remove-add event chain in one frame and already created
    // operation during the first add.
    return existent_operation != NULL;
}

static bool process_file_added (struct resource_provider_state_t *state,
                                struct kan_resource_provider_singleton_t *public,
                                struct resource_provider_private_singleton_t *private,
                                const char *path,
                                kan_instance_size_t path_length)
{
    if (is_changed_file_a_resource_manifest (state, path, path_length))
    {
        KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                 "File added at virtual path \"%s\" has resource package file name and adding new packages during hot "
                 "reload is not supported.",
                 path)
        return false;
    }

    kan_interned_string_t package_name = extract_changed_file_package_name (state, path, path_length);
    if (!package_name)
    {
        KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                 "File added at virtual path \"%s\" does not belong to any package judging by its path!", path)
        return false;
    }

    KAN_UMI_VALUE_READ_OPTIONAL (package, kan_resource_package_state_t, name, &package_name)
    if (!package)
    {
        KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                 "File added at virtual path \"%s\" does belong to unknown package \"%s\"!", path, package_name)
        return false;
    }

    kan_virtual_file_system_volume_t volume =
        kan_virtual_file_system_get_context_volume_for_read (state->virtual_file_system);
    struct scan_file_internal_result_t scan_result = scan_file_internal (state, private, volume, path_length, path);
    kan_virtual_file_system_close_context_read_access (state->virtual_file_system);

    if (!scan_result.successful)
    {
        KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                 "Failed to process addition at virtual path \"%s\" due to scan failure.", path)
        return false;
    }

    struct resource_provider_resource_type_interface_t *interface = NULL;
    if (scan_result.type)
    {
        interface = query_resource_type_interface (state, scan_result.type);
        if (!interface)
        {
            KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                     "Failed to process addition at virtual path \"%s\": resource type \"%s\" is unknown.", path,
                     scan_result.type)
            return false;
        }
    }

    KAN_UML_VALUE_UPDATE (existent_registered, kan_resource_registered_entry_t, name, &scan_result.name)
    {
        if (existent_registered->type == scan_result.type)
        {
            KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                     "Failed to process addition at virtual path \"%s\" as entry \"%s\" of type \"%s\" already exists "
                     "at \"%s\".",
                     path, existent_registered->name, existent_registered->type, existent_registered->path)
            return false;
        }
    }

    kan_resource_entry_id_t entry_id =
        register_new_entry (state, private, package_name, scan_result.type, scan_result.name, path);

    send_resource_updated_event (state, interface, entry_id, scan_result.name);
    KAN_UMI_VALUE_UPDATE_REQUIRED (registered, kan_resource_registered_entry_t, entry_id, &entry_id)

    if (interface && interface->source_node->streamed)
    {
        KAN_UML_VALUE_READ (request, kan_resource_streaming_request_t, name, &registered->name)
        {
            if (request->type == registered->type)
            {
                ++registered->streaming_counter;
            }
        }

        if (registered->streaming_counter > 0u)
        {
            cancel_streaming_operation_if_any (state, registered, interface);
            start_streaming_operation (state, public, registered);
        }
    }
    else if (package->loaded && !is_loading_operation_for_entry_exists (state, entry_id))
    {
        schedule_transactional_loading (state, registered);
        return true;
    }

    // Returning false as transaction was not triggered, just entry was updated.
    return false;
}

static bool process_file_modified (struct resource_provider_state_t *state,
                                   struct kan_resource_provider_singleton_t *public,
                                   const char *path,
                                   kan_instance_size_t path_length)
{
    if (is_changed_file_a_resource_manifest (state, path, path_length))
    {
        KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                 "File modified at virtual path \"%s\" has resource package file name and modifying package manifests "
                 "during hot reload is not supported.",
                 path)
        return false;
    }

    const kan_hash_t path_hash = kan_string_hash (path);
    KAN_UML_VALUE_UPDATE (registered, kan_resource_registered_entry_t, path_hash, &path_hash)
    {
        if (strcmp (registered->path, path) == 0)
        {
            const char *path_end = path + path_length;
            const bool native = path_length > 4u && *(path_end - 4u) == '.' && *(path_end - 3u) == 'b' &&
                                *(path_end - 2u) == 'i' && *(path_end - 1u) == 'n';

            if (native)
            {
                // Read type header in case if type was modified.
                kan_virtual_file_system_volume_t volume =
                    kan_virtual_file_system_get_context_volume_for_read (state->virtual_file_system);

                struct kan_stream_t *stream = kan_virtual_file_stream_open_for_read (volume, registered->path);
                kan_virtual_file_system_close_context_read_access (state->virtual_file_system);

                if (!stream)
                {
                    KAN_LOG (
                        universe_resource_provider, KAN_LOG_ERROR,
                        "Failed to process modification of entry \"%s\" of type \"%s\": unable to open stream to read "
                        "type header.",
                        registered->name, registered->type)
                    continue;
                }

                stream = kan_random_access_stream_buffer_open_for_read (
                    stream, KAN_UNIVERSE_RESOURCE_PROVIDER_TYPE_HEADER_BUFFER);
                CUSHION_DEFER { stream->operations->close (stream); }
                kan_interned_string_t type;

                if (!kan_serialization_binary_read_type_header (
                        stream, &type,
                        // We expect non-indexed files to be encoded without string registries.
                        KAN_HANDLE_SET_INVALID (kan_serialization_interned_string_registry_t)))
                {
                    KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                             "Failed to process modification of entry \"%s\" of type \"%s\": failed to deserialize "
                             "type header.",
                             registered->name, registered->type)
                    continue;
                }

                if (type != registered->type)
                {
                    KAN_LOG (
                        universe_resource_provider, KAN_LOG_ERROR,
                        "Failed to process modification of entry \"%s\" of type \"%s\": type header has type \"%s\" "
                        "and we do not expect such things to happen as resource builder is expected to deploy files "
                        "into type-based directories.",
                        registered->name, registered->type, type)
                    continue;
                }
            }
            else
            {
                // Native or not check is based on file name, entry could not have been native.
                KAN_ASSERT (!registered->type)
            }

            struct resource_provider_resource_type_interface_t *interface = NULL;
            if (registered->type)
            {
                interface = query_resource_type_interface (state, registered->type);
            }

            KAN_UMI_VALUE_READ_REQUIRED (package, kan_resource_package_state_t, name, &registered->package)
            send_resource_updated_event (state, interface, registered->entry_id, registered->name);

            if (interface && interface->source_node->streamed)
            {
                if (registered->streaming_counter > 0u)
                {
                    cancel_streaming_operation_if_any (state, registered, interface);
                    start_streaming_operation (state, public, registered);
                }
            }
            else if (package->loaded && !is_loading_operation_for_entry_exists (state, registered->entry_id))
            {
                schedule_transactional_loading (state, registered);
                return true;
            }

            // Returning false as transaction was not triggered, just entry was updated.
            return false;
        }
    }

    return false;
}

static bool process_file_removed (struct resource_provider_state_t *state, const char *path)
{
    const kan_hash_t path_hash = kan_string_hash (path);
    KAN_UML_VALUE_WRITE (registered, kan_resource_registered_entry_t, path_hash, &path_hash)
    {
        if (strcmp (registered->path, path) == 0)
        {
            struct resource_provider_resource_type_interface_t *interface = NULL;
            if (registered->type)
            {
                interface = query_resource_type_interface (state, registered->type);
                KAN_ASSERT (interface)
            }

            if (interface)
            {
                struct kan_repository_event_insertion_package_t insert_event =
                    kan_repository_event_insert_query_execute (&interface->insert_unregistered_event);
                struct kan_resource_unregistered_event_view_t *event =
                    kan_repository_event_insertion_package_get (&insert_event);

                if (event)
                {
                    event->entry_id = registered->entry_id;
                    event->name = registered->name;
                    kan_repository_event_insertion_package_submit (&insert_event);
                }
            }
            else
            {
                KAN_UMO_EVENT_INSERT_INIT (kan_resource_third_party_unregistered_event_t) {.name = registered->name};
            }

            if (interface && interface->source_node->streamed)
            {
                KAN_UMI_VALUE_DELETE_OPTIONAL (operation, resource_provider_streaming_operation_t, entry_id,
                                               &registered->entry_id)

                if (operation)
                {
                    KAN_UM_ACCESS_DELETE (operation);
                }

                delete_loaded_entry_by_id (interface, registered->entry_id);
                KAN_UM_ACCESS_DELETE (registered);

                // Unloading streamed entry does not require transaction.
                return false;
            }

            // For non-streamed entries -- just start new transaction with unload planned.
            send_unload_planned_events (state, registered, interface);
            plan_transactional_unload (state, registered);
            KAN_UM_ACCESS_DELETE (registered);
            return true;
        }
    }

    return false;
}

static bool check_file_systems_changes_and_reload (struct resource_provider_state_t *state,
                                                   struct kan_resource_provider_singleton_t *public,
                                                   struct resource_provider_private_singleton_t *private)
{
    KAN_CPU_SCOPED_STATIC_SECTION (resource_file_events)
    bool transaction_triggered = false;
    kan_virtual_file_system_get_context_volume_for_read (state->virtual_file_system);
    const struct kan_virtual_file_system_watcher_event_t *event;

    while ((event = kan_hot_reload_virtual_file_event_provider_get (private->file_event_provider)))
    {
        if (event->entry_type == KAN_VIRTUAL_FILE_SYSTEM_ENTRY_TYPE_FILE)
        {
            const char *path = event->path_container.path;
            kan_instance_size_t length = event->path_container.length;

            // Skip first "/" in order to have same path format for scanned and observed files.
            if (path && path[0u] == '/')
            {
                ++path;
                --length;
            }

            switch (event->event_type)
            {
            case KAN_VIRTUAL_FILE_SYSTEM_EVENT_TYPE_ADDED:
                transaction_triggered |= process_file_added (state, public, private, path, length);
                break;

            case KAN_VIRTUAL_FILE_SYSTEM_EVENT_TYPE_MODIFIED:
                transaction_triggered |= process_file_modified (state, public, path, length);
                break;

            case KAN_VIRTUAL_FILE_SYSTEM_EVENT_TYPE_REMOVED:
                transaction_triggered |= process_file_removed (state, path);
                break;
            }
        }

        kan_hot_reload_virtual_file_event_provider_advance (private->file_event_provider);
    }

    kan_virtual_file_system_close_context_read_access (state->virtual_file_system);
    if (transaction_triggered)
    {
        public->transaction_state = KAN_RESOURCE_TRANSACTION_STATE_LOADING;
    }

    return transaction_triggered;
}

static void flip_native_resource_loaded_entry (struct resource_provider_resource_type_interface_t *interface,
                                               struct kan_resource_loaded_entry_view_t *loaded)
{
    void *inlined_data = (void *) kan_apply_alignment ((kan_memory_size_t) loaded->data_begin,
                                                       interface->source_node->source_resource_type->alignment);

    if (interface->source_node->source_resource_type->shutdown)
    {
        kan_allocation_group_stack_push (loaded->my_allocation_group);
        interface->source_node->source_resource_type->shutdown (
            interface->source_node->source_resource_type->functor_user_data, inlined_data);
        kan_allocation_group_stack_pop ();
    }

    // We operate under several assumptions:
    // - Noone point to the loading data address except for `loaded->loading_data`.
    // - Resource size itself is quite small and heavy data is in child allocations like array.
    // If both of these assumptions are true, then we can just copy from loading data location to inlined data location
    // and deallocate without calling shutdown.

    KAN_ASSERT (loaded->loading_data)
    memcpy (inlined_data, loaded->loading_data, interface->source_node->source_resource_type->size);
    kan_free_batched (loaded->my_allocation_group, loaded->loading_data);
    loaded->loading_data = NULL;
    loaded->data_ready = true;
}

enum resource_provider_serve_operation_status_t
{
    RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_IN_PROGRESS,
    RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_FINISHED,
    RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_FAILED,
};

static inline enum resource_provider_serve_operation_status_t execute_shared_process_native_operation (
    struct resource_provider_state_t *state,
    struct resource_provider_operation_state_t *operation,
    struct kan_resource_loaded_entry_view_t *loaded,
    struct resource_provider_resource_type_interface_t *interface,
    const struct kan_resource_registered_entry_t *registered)
{
    if (!operation->stream)
    {
        kan_virtual_file_system_volume_t volume =
            kan_virtual_file_system_get_context_volume_for_read (state->virtual_file_system);
        operation->stream = kan_virtual_file_stream_open_for_read (volume, registered->path);
        kan_virtual_file_system_close_context_read_access (state->virtual_file_system);

        if (!operation->stream)
        {
            KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                     "Failed to open file at virtual path \"%s\" to read entry \"%s\" of type \"%s\".",
                     registered->path, registered->name, operation->native.type)
            return RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_FAILED;
        }

        operation->stream =
            kan_random_access_stream_buffer_open_for_read (operation->stream, KAN_UNIVERSE_RESOURCE_PROVIDER_IO_BUFFER);

        KAN_UMI_VALUE_READ_REQUIRED (package, kan_resource_package_state_t, name, &registered->package)
        kan_interned_string_t type;

        if (!kan_serialization_binary_read_type_header (operation->stream, &type, package->string_registry))
        {
            KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                     "Failed to check type header while loading entry \"%s\" of type \"%s\": serialization error.",
                     registered->name, operation->native.type)
            return RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_FAILED;
        }

        if (type != operation->native.type)
        {
            KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                     "Failed to check type header while loading entry \"%s\" of type \"%s\": type header has type "
                     "\"%s\".",
                     registered->name, operation->native.type, type)
            return RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_FAILED;
        }
    }

    if (!loaded->loading_data)
    {
        KAN_ASSERT (!KAN_HANDLE_IS_VALID (operation->native.binary_reader))
        loaded->loading_data =
            kan_allocate_batched (loaded->my_allocation_group, interface->source_node->source_resource_type->size);

        if (interface->source_node->source_resource_type->init)
        {
            kan_allocation_group_stack_push (loaded->my_allocation_group);
            interface->source_node->source_resource_type->init (
                interface->source_node->source_resource_type->functor_user_data, loaded->loading_data);
            kan_allocation_group_stack_pop ();
        }
    }

    if (!KAN_HANDLE_IS_VALID (operation->native.binary_reader))
    {
        KAN_UMI_VALUE_READ_REQUIRED (package, kan_resource_package_state_t, name, &registered->package)
        operation->native.binary_reader = kan_serialization_binary_reader_create (
            operation->stream, loaded->loading_data, operation->native.type, state->shared_script_storage,
            package->string_registry, loaded->my_allocation_group);
    }

    enum kan_serialization_state_t serialization_state = KAN_SERIALIZATION_IN_PROGRESS;
    while ((serialization_state = kan_serialization_binary_reader_step (operation->native.binary_reader)) ==
           KAN_SERIALIZATION_IN_PROGRESS)
    {
        if (kan_precise_time_get_elapsed_nanoseconds () > state->execution_shared_state.end_time_ns)
        {
            return RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_IN_PROGRESS;
        }
    }

    if (serialization_state != KAN_SERIALIZATION_FINISHED)
    {
        KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                 "Failed to load entry \"%s\" of type \"%s\": serialization error.", registered->name,
                 operation->native.type)
        return RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_FAILED;
    }

    if (interface->source_node->transitively_loaded)
    {
        // Unload transitively loaded entry after the commit.
        KAN_ASSERT (!interface->source_node->streamed)
        plan_transactional_unload (state, registered);
    }
    else if (interface->source_node->streamed)
    {
        // Streamed operation, flip right away.
        flip_native_resource_loaded_entry (interface, loaded);
    }
    else
    {
        // Transactional operation, flip when transaction finishes.
        KAN_UMO_EVENT_INSERT_INIT (resource_provider_transactional_flip_event_t) {
            .entry_id = registered->entry_id,
            .type = registered->type,
        };
    }

    struct kan_repository_event_insertion_package_t insert_event =
        kan_repository_event_insert_query_execute (&interface->insert_loaded_event);
    struct kan_resource_loaded_event_view_t *event = kan_repository_event_insertion_package_get (&insert_event);

    if (event)
    {
        event->entry_id = registered->entry_id;
        event->name = registered->name;
        kan_repository_event_insertion_package_submit (&insert_event);
    }

    return RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_FINISHED;
}

static inline enum resource_provider_serve_operation_status_t execute_shared_process_third_party_operation (
    struct resource_provider_state_t *state,
    struct resource_provider_operation_state_t *operation,
    struct kan_resource_loaded_third_party_entry_t *loaded,
    const struct kan_resource_registered_entry_t *registered)
{
    if (!operation->stream)
    {
        kan_virtual_file_system_volume_t volume =
            kan_virtual_file_system_get_context_volume_for_read (state->virtual_file_system);
        operation->stream = kan_virtual_file_stream_open_for_read (volume, registered->path);
        kan_virtual_file_system_close_context_read_access (state->virtual_file_system);

        if (!operation->stream)
        {
            KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                     "Failed to open file at virtual path \"%s\" to read entry \"%s\" of type \"%s\".",
                     registered->path, registered->name, operation->native.type)
            return RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_FAILED;
        }

        operation->stream =
            kan_random_access_stream_buffer_open_for_read (operation->stream, KAN_UNIVERSE_RESOURCE_PROVIDER_IO_BUFFER);

        if (!operation->stream->operations->seek (operation->stream, KAN_STREAM_SEEK_END, 0))
        {
            KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                     "Failed to seek to the end of file at virtual path \"%s\" to load third party resource \"%s\".",
                     registered->path, registered->name)
            return RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_FAILED;
        }

        operation->third_party.read = 0u;
        operation->third_party.size = operation->stream->operations->tell (operation->stream);

        if (!operation->stream->operations->seek (operation->stream, KAN_STREAM_SEEK_START, 0))
        {
            KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                     "Failed to seek to the start of file at virtual path \"%s\" to load third party resource \"%s\".",
                     registered->path, registered->name)
            return RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_FAILED;
        }
    }

    KAN_ASSERT (!loaded->loading_data || operation->third_party.read > 0u)
    if (!loaded->loading_data)
    {
        loaded->loading_data_size = operation->third_party.size;
        loaded->loading_data =
            kan_allocate_general (loaded->my_allocation_group,
                                  (kan_instance_size_t) kan_apply_alignment (
                                      loaded->loading_data_size, KAN_RESOURCE_PROVIDER_LOADED_THIRD_PARTY_ALIGNMENT),
                                  KAN_RESOURCE_PROVIDER_LOADED_THIRD_PARTY_ALIGNMENT);
    }

    uint8_t *data_base = loaded->loading_data;
    while (operation->third_party.read < operation->third_party.size)
    {
        if (kan_precise_time_get_elapsed_nanoseconds () > state->execution_shared_state.end_time_ns)
        {
            return RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_IN_PROGRESS;
        }

        const kan_stable_size_t to_read = KAN_MIN (operation->third_party.size - operation->third_party.read,
                                                   KAN_UNIVERSE_RESOURCE_PROVIDER_IO_BUFFER);

        if (operation->stream->operations->read (operation->stream, to_read, data_base + operation->third_party.read) !=
            to_read)
        {
            KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                     "Encountered IO error while data of third party resource \"%s\".", registered->name)
            return RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_FAILED;
        }

        operation->third_party.read += to_read;
    }

    // Always transactional operation, flip when transaction finishes.
    KAN_UMO_EVENT_INSERT_INIT (resource_provider_transactional_flip_event_t) {.entry_id = registered->entry_id,
                                                                              .type = NULL};
    KAN_UMO_EVENT_INSERT_INIT (kan_resource_third_party_loaded_event_t) {.name = registered->name};
    return RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_FINISHED;
}

static inline enum resource_provider_serve_operation_status_t execute_shared_process_operation (
    struct resource_provider_state_t *state,
    kan_resource_entry_id_t entry_id,
    struct resource_provider_operation_state_t *operation)
{
    KAN_UMI_VALUE_READ_REQUIRED (registered, kan_resource_registered_entry_t, entry_id, &entry_id)
    if (operation->is_native)
    {
        enum resource_provider_serve_operation_status_t status;
        struct resource_provider_resource_type_interface_t *interface =
            query_resource_type_interface (state, registered->type);
        KAN_ASSERT (interface)

        struct kan_repository_indexed_value_write_access_t existent_access = write_loaded_entry (interface, entry_id);
        struct kan_resource_loaded_entry_view_t *loaded_view =
            kan_repository_indexed_value_write_access_resolve (&existent_access);

        if (loaded_view)
        {
            status = execute_shared_process_native_operation (state, operation, loaded_view, interface, registered);
            switch (status)
            {
            case RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_IN_PROGRESS:
            case RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_FINISHED:
                kan_repository_indexed_value_write_access_close (&existent_access);
                break;

            case RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_FAILED:
                kan_repository_indexed_value_write_access_delete (&existent_access);
                break;
            }
        }
        else
        {
            struct kan_repository_indexed_insertion_package_t inserted_package =
                kan_repository_indexed_insert_query_execute (&interface->insert_loaded_entry);

            loaded_view = kan_repository_indexed_insertion_package_get (&inserted_package);
            loaded_view->entry_id = registered->entry_id;
            loaded_view->name = registered->name;
            status = execute_shared_process_native_operation (state, operation, loaded_view, interface, registered);

            switch (status)
            {
            case RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_IN_PROGRESS:
            case RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_FINISHED:
                kan_repository_indexed_insertion_package_submit (&inserted_package);
                break;

            case RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_FAILED:
                kan_repository_indexed_insertion_package_undo (&inserted_package);
                break;
            }
        }

        return status;
    }
    else
    {
        KAN_UMI_VALUE_WRITE_OPTIONAL (existent_loaded, kan_resource_loaded_third_party_entry_t, entry_id, &entry_id)
        enum resource_provider_serve_operation_status_t status;

        if (existent_loaded)
        {
            status = execute_shared_process_third_party_operation (state, operation, existent_loaded, registered);
            switch (status)
            {
            case RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_IN_PROGRESS:
            case RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_FINISHED:
                break;

            case RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_FAILED:
                KAN_UM_ACCESS_DELETE (existent_loaded);
                break;
            }
        }
        else
        {
            struct kan_repository_indexed_insertion_package_t insertion =
                kan_repository_indexed_insert_query_execute (&state->insert__kan_resource_loaded_third_party_entry);

            struct kan_resource_loaded_third_party_entry_t *loaded =
                kan_repository_indexed_insertion_package_get (&insertion);

            loaded->entry_id = entry_id;
            loaded->name = registered->name;
            status = execute_shared_process_third_party_operation (state, operation, loaded, registered);

            switch (status)
            {
            case RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_IN_PROGRESS:
            case RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_FINISHED:
                kan_repository_indexed_insertion_package_submit (&insertion);
                break;

            case RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_FAILED:
                kan_repository_indexed_insertion_package_undo (&insertion);
                break;
            }
        }

        return status;
    }
}

static void execute_shared_streaming (kan_memory_size_t user_data)
{
    struct resource_provider_state_t *state = (struct resource_provider_state_t *) user_data;
    bool done_any_work = false;

    while (true)
    {
        if (done_any_work && kan_precise_time_get_elapsed_nanoseconds () > state->execution_shared_state.end_time_ns)
        {
            break;
        }

        done_any_work = true;
        // Retrieve streaming operation.
        kan_atomic_int_lock (&state->execution_shared_state.concurrency_lock);
        struct kan_repository_indexed_interval_write_access_t operation_access =
            kan_repository_indexed_interval_descending_write_cursor_next (
                &state->execution_shared_state.streaming_cursor);
        kan_atomic_int_unlock (&state->execution_shared_state.concurrency_lock);

        struct resource_provider_streaming_operation_t *operation =
            kan_repository_indexed_interval_write_access_resolve (&operation_access);

        if (!operation)
        {
            // Exit: No more items.
            break;
        }

        switch (execute_shared_process_operation (state, operation->entry_id, &operation->state))
        {
        case RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_IN_PROGRESS:
            kan_repository_indexed_interval_write_access_close (&operation_access);
            break;

        case RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_FINISHED:
        case RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_FAILED:
            // No matter the result, operation execution is done, therefore it should be deleted.
            kan_repository_indexed_interval_write_access_delete (&operation_access);
            break;
        }
    }

    if (kan_atomic_int_add (&state->execution_shared_state.workers_left, -1) == 1)
    {
        kan_repository_indexed_interval_descending_write_cursor_close (&state->execution_shared_state.streaming_cursor);
        kan_repository_singleton_write_access_close (&state->execution_shared_state.private_access);
    }
}

static void execute_shared_loading (kan_memory_size_t user_data)
{
    struct resource_provider_state_t *state = (struct resource_provider_state_t *) user_data;
    bool done_any_work = false;

    while (true)
    {
        if (done_any_work && kan_precise_time_get_elapsed_nanoseconds () > state->execution_shared_state.end_time_ns)
        {
            break;
        }

        done_any_work = true;
        // Retrieve loading operation.
        kan_atomic_int_lock (&state->execution_shared_state.concurrency_lock);
        struct kan_repository_indexed_sequence_write_access_t operation_access =
            kan_repository_indexed_sequence_write_cursor_next (&state->execution_shared_state.loading_cursor);
        kan_atomic_int_unlock (&state->execution_shared_state.concurrency_lock);

        struct resource_provider_loading_operation_t *operation =
            kan_repository_indexed_sequence_write_access_resolve (&operation_access);

        if (!operation)
        {
            // Exit: No more items.
            break;
        }

        switch (execute_shared_process_operation (state, operation->entry_id, &operation->state))
        {
        case RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_IN_PROGRESS:
            kan_repository_indexed_sequence_write_access_close (&operation_access);
            break;

        case RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_FINISHED:
        case RESOURCE_PROVIDER_SERVE_OPERATION_STATUS_FAILED:
            // No matter the result, operation execution is done, therefore it should be deleted.
            kan_repository_indexed_sequence_write_access_delete (&operation_access);
            break;
        }
    }

    if (kan_atomic_int_add (&state->execution_shared_state.workers_left, -1) == 1)
    {
        kan_repository_indexed_sequence_write_cursor_close (&state->execution_shared_state.loading_cursor);
        kan_repository_singleton_write_access_close (&state->execution_shared_state.private_access);
    }
}

static void perform_flip_on_transaction_finish (struct resource_provider_state_t *state)
{
    KAN_CPU_SCOPED_STATIC_SECTION (perform_flip_on_transaction_finish)
    KAN_UML_EVENT_FETCH (event, resource_provider_transactional_flip_event_t)
    {
        if (event->type)
        {
            struct resource_provider_resource_type_interface_t *interface =
                query_resource_type_interface (state, event->type);

            KAN_ASSERT (interface)
            struct kan_repository_indexed_value_update_access_t access =
                update_loaded_entry (interface, event->entry_id);

            struct kan_resource_loaded_entry_view_t *view =
                kan_repository_indexed_value_update_access_resolve (&access);

            if (view)
            {
                flip_native_resource_loaded_entry (interface, view);
                kan_repository_indexed_value_update_access_close (&access);
            }
            else
            {
                KAN_UMI_VALUE_READ_REQUIRED (registered, kan_resource_registered_entry_t, entry_id, &event->entry_id)
                KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                         "Failed to confirm loading of resource \"%s\" of type \"%s\" from package \"%s\" as it was "
                         "not loaded.",
                         registered->name, registered->package)
            }
        }
        else
        {
            KAN_UMI_VALUE_UPDATE_OPTIONAL (loaded, kan_resource_loaded_third_party_entry_t, entry_id, &event->entry_id)
            if (loaded)
            {
                if (loaded->loaded_data)
                {
                    kan_free_general (
                        loaded->my_allocation_group, loaded->loaded_data,
                        (kan_instance_size_t) kan_apply_alignment (loaded->loaded_data_size,
                                                                   KAN_RESOURCE_PROVIDER_LOADED_THIRD_PARTY_ALIGNMENT));
                }

                loaded->loaded_data = loaded->loading_data;
                loaded->loaded_data_size = loaded->loading_data_size;

                loaded->loading_data = NULL;
                loaded->loading_data_size = 0u;
            }
            else
            {
                KAN_UMI_VALUE_READ_REQUIRED (registered, kan_resource_registered_entry_t, entry_id, &event->entry_id)
                KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                         "Failed to confirm loading third party resource \"%s\" from package \"%s\" as it was not even "
                         "loaded.",
                         registered->name, registered->package)
            }
        }
    }
}

static void perform_unload_on_transaction_finish (struct resource_provider_state_t *state)
{
    KAN_CPU_SCOPED_STATIC_SECTION (perform_unload_on_transaction_finish)
    KAN_UML_EVENT_FETCH (event, resource_provider_transactional_unload_event_t)
    {
        if (event->type)
        {
            struct resource_provider_resource_type_interface_t *interface =
                query_resource_type_interface (state, event->type);

            KAN_ASSERT (interface)
            KAN_ASSERT (!interface->source_node->streamed)

            if (!delete_loaded_entry_by_id (interface, event->entry_id))
            {
                KAN_UMI_VALUE_READ_OPTIONAL (registered, kan_resource_registered_entry_t, entry_id, &event->entry_id)
                if (registered)
                {
                    KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                             "Failed to unload resource \"%s\" of type \"%s\" from package \"%s\" as it was not even "
                             "loaded.",
                             event->type, registered->name, registered->package)
                }
                else
                {
                    KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                             "Failed to unload some resource of type \"%s\" as it is even unregistered by now.",
                             event->type)
                }
            }
        }
        else
        {
            KAN_UMI_VALUE_DELETE_OPTIONAL (loaded, kan_resource_loaded_third_party_entry_t, entry_id, &event->entry_id)
            if (loaded)
            {
                KAN_UM_ACCESS_DELETE (loaded);
            }
            else
            {
                KAN_UMI_VALUE_READ_OPTIONAL (registered, kan_resource_registered_entry_t, entry_id, &event->entry_id)
                if (registered)
                {
                    KAN_LOG (
                        universe_resource_provider, KAN_LOG_ERROR,
                        "Failed to unload third party resource \"%s\" from package \"%s\" as it was not even loaded.",
                        registered->name, registered->package)
                }
                else
                {
                    KAN_LOG (universe_resource_provider, KAN_LOG_ERROR,
                             "Failed to unload some third party resource as it is even unregistered by now.")
                }
            }
        }
    }
}

UNIVERSE_RESOURCE_PROVIDER_API KAN_UM_MUTATOR_EXECUTE_SIGNATURE (mutator_template_execute_resource_provider,
                                                                 resource_provider_state_t)
{
    KAN_UMI_SINGLETON_WRITE (public, kan_resource_provider_singleton_t)
    KAN_UMI_SINGLETON_WRITE (private, resource_provider_private_singleton_t)
    ++public->logic_deduplication_frame_id;

    if (!public->initial_scan_done)
    {
        KAN_CPU_SCOPED_STATIC_SECTION (scan)
        {
            kan_virtual_file_system_volume_t volume =
                kan_virtual_file_system_get_context_volume_for_read (state->virtual_file_system);
            CUSHION_DEFER { kan_virtual_file_system_close_context_read_access (state->virtual_file_system); };

            struct kan_file_system_path_container_t container;
            kan_file_system_path_container_copy_string (&container, state->configuration.resource_directory_path);
            // Use "resources" as default name, it should never be taken anyway if directory structure is as expected.
            scan_directory (state, private, volume, &container, NULL, "resources");
        }

        if (kan_hot_reload_coordination_system_is_possible () && KAN_HANDLE_IS_VALID (state->hot_reload_system))
        {
            private->file_event_provider = kan_hot_reload_virtual_file_event_provider_create (
                state->hot_reload_system, state->configuration.resource_directory_path);
        }

        public->initial_scan_done = true;
    }

    if (KAN_HANDLE_IS_VALID (state->hot_reload_system) &&
        kan_hot_reload_coordination_system_is_executing (state->hot_reload_system))
    {
        KAN_ASSERT (public->transaction_state == KAN_RESOURCE_TRANSACTION_STATE_NONE)
        // Hot reload is going on, do not start any operation until it is done. All operations prior to hot reload
        // execution would delay it, so we cannot have any ongoing operation if it is already executing.
        return;
    }

    // Planning phase. Check everything, finalizes states, spawn new operations.
    switch (public->transaction_state)
    {
    case KAN_RESOURCE_TRANSACTION_STATE_NONE:
    {
        if (!public->essential_loading_done)
        {
            start_unconditional_loading_transaction (state, public, private, KAN_RESOURCE_PACKAGE_LEVEL_ESSENTIAL);
            break;
        }

        if (public->tags_dirty)
        {
            public->tags_dirty = false;
            if (start_optional_loading_transaction (state, public, private))
            {
                break;
            }
        }

        if (KAN_HANDLE_IS_VALID (private->file_event_provider) &&
            kan_hot_reload_coordination_system_is_reload_allowed (state->hot_reload_system) &&
            check_file_systems_changes_and_reload (state, public, private))
        {
            break;
        }

        // We only start new streaming operations if we don't have ongoing hot reload request.
        // Otherwise, streaming operations might've postponed hot reload indefinitely.
        if (!KAN_HANDLE_IS_VALID (state->hot_reload_system) ||
            !kan_hot_reload_coordination_system_is_scheduled (state->hot_reload_system))
        {
            KAN_CPU_SCOPED_STATIC_SECTION (streaming_request_events)
            KAN_UML_EVENT_FETCH (insert_event, resource_streaming_request_on_insert_event_t)
            {
                process_streaming_request_insert (state, public, insert_event->type, insert_event->name);
            }

            KAN_UML_EVENT_FETCH (delete_event, resource_streaming_request_on_delete_event_t)
            {
                process_streaming_request_delete (state, public, delete_event->type, delete_event->name);
            }
        }

        break;
    }

    case KAN_RESOURCE_TRANSACTION_STATE_LOADING:
    {
        bool has_loading_operations = false;
        KAN_UML_SEQUENCE_READ (lock, resource_provider_loading_operation_t)
        {
            has_loading_operations = true;
            break;
        }

        if (!has_loading_operations)
        {
            KAN_UMO_EVENT_INSERT_INIT (kan_resource_transaction_commit_started_event_t) {.stub = 0u};
            public->transaction_state = KAN_RESOURCE_TRANSACTION_STATE_COMMIT;
            break;
        }

        break;
    }

    case KAN_RESOURCE_TRANSACTION_STATE_COMMIT:
    {
        if (kan_atomic_int_set (&public->commit_locked, 0) != 0)
        {
            // Commit was locked, wait for one more frame.
            break;
        }

        // Transaction is finished now.
        perform_flip_on_transaction_finish (state);
        perform_unload_on_transaction_finish (state);

        KAN_UMO_EVENT_INSERT_INIT (kan_resource_transaction_commit_finished_event_t) {.stub = 0u};
        public->transaction_state = KAN_RESOURCE_TRANSACTION_STATE_NONE;

        if (!public->essential_loading_done)
        {
            KAN_UMO_EVENT_INSERT_INIT (kan_resource_essentials_loaded_event_t) {.stub = 0u};
            public->essential_loading_done = true;
            start_unconditional_loading_transaction (state, public, private, KAN_RESOURCE_PACKAGE_LEVEL_REQUIRED);
            break;
        }

        public->required_loading_done = true;
        break;
    }
    }

    // Intentionally do the delay-check after all the planning work and before management work.
    // Otherwise, it would be possible to still allow hot reload while having operations as new operations
    // would be added during planning.
    if (KAN_HANDLE_IS_VALID (state->hot_reload_system) &&
        kan_hot_reload_coordination_system_is_scheduled (state->hot_reload_system))
    {
        // Delay any hot reload actions until all the loading and streaming is done.
        bool busy = !public->initial_scan_done || !public->essential_loading_done ||
                    public->transaction_state != KAN_RESOURCE_TRANSACTION_STATE_NONE;

        if (!busy)
        {
            // Check for existent streaming operations then.
            struct kan_repository_indexed_interval_descending_write_cursor_t cursor =
                kan_repository_indexed_interval_write_query_execute_descending (
                    &state->write_interval__resource_provider_streaming_operation__priority, NULL, NULL);
            CUSHION_DEFER { kan_repository_indexed_interval_descending_write_cursor_close (&cursor); }

            struct kan_repository_indexed_interval_write_access_t access =
                kan_repository_indexed_interval_descending_write_cursor_next (&cursor);

            if (kan_repository_indexed_interval_write_access_resolve (&access))
            {
                busy = true;
                kan_repository_indexed_interval_write_access_close (&access);
            }
        }

        if (busy)
        {
            kan_hot_reload_coordination_system_delay (state->hot_reload_system);
            KAN_LOG (universe_resource_provider, KAN_LOG_ERROR, "Hot reload is delayed as resource provider is busy.")
        }
    }

    // Management phase. Run operations if any.
    switch (public->transaction_state)
    {
    case KAN_RESOURCE_TRANSACTION_STATE_NONE:
    {
        kan_stack_group_allocator_reset (&state->temporary_allocator);
        const kan_instance_size_t cpu_count = kan_platform_get_cpu_logical_core_count ();
        struct kan_cpu_task_list_node_t *task_list_node = NULL;

        state->execution_shared_state.job = job;
        state->execution_shared_state.end_time_ns =
            kan_precise_time_get_elapsed_nanoseconds () + state->configuration.streaming_budget_ns;

        state->execution_shared_state.workers_left = kan_atomic_int_init ((int) cpu_count);
        state->execution_shared_state.concurrency_lock = kan_atomic_int_init (0);

        state->execution_shared_state.private = private;
        KAN_UM_ACCESS_ESCAPE (state->execution_shared_state.private_access, private);

        state->execution_shared_state.streaming_cursor =
            kan_repository_indexed_interval_write_query_execute_descending (
                &state->write_interval__resource_provider_streaming_operation__priority, NULL, NULL);

        for (kan_memory_size_t worker_index = 0u; worker_index < cpu_count; ++worker_index)
        {
            KAN_CPU_TASK_LIST_USER_VALUE (&task_list_node, &state->temporary_allocator, execute_shared_streaming,
                                          KAN_CPU_STATIC_SECTION_GET (resource_provider_server), state)
        }

        kan_cpu_job_dispatch_and_detach_task_list (state->execution_shared_state.job, task_list_node);
        break;
    }

    case KAN_RESOURCE_TRANSACTION_STATE_LOADING:
    {
        kan_stack_group_allocator_reset (&state->temporary_allocator);
        const kan_instance_size_t cpu_count = kan_platform_get_cpu_logical_core_count ();
        struct kan_cpu_task_list_node_t *task_list_node = NULL;

        state->execution_shared_state.job = job;
        state->execution_shared_state.end_time_ns =
            kan_precise_time_get_elapsed_nanoseconds () + state->configuration.transaction_budget_ns;

        state->execution_shared_state.workers_left = kan_atomic_int_init ((int) cpu_count);
        state->execution_shared_state.concurrency_lock = kan_atomic_int_init (0);

        state->execution_shared_state.private = private;
        KAN_UM_ACCESS_ESCAPE (state->execution_shared_state.private_access, private);

        state->execution_shared_state.loading_cursor = kan_repository_indexed_sequence_write_query_execute (
            &state->write_sequence__resource_provider_loading_operation);

        for (kan_memory_size_t worker_index = 0u; worker_index < cpu_count; ++worker_index)
        {
            KAN_CPU_TASK_LIST_USER_VALUE (&task_list_node, &state->temporary_allocator, execute_shared_loading,
                                          KAN_CPU_STATIC_SECTION_GET (resource_provider_server), state)
        }

        kan_cpu_job_dispatch_and_detach_task_list (state->execution_shared_state.job, task_list_node);
        break;
    }

    case KAN_RESOURCE_TRANSACTION_STATE_COMMIT:
        break;
    }
}

UNIVERSE_RESOURCE_PROVIDER_API KAN_UM_MUTATOR_UNDEPLOY_SIGNATURE (mutator_template_undeploy_resource_provider,
                                                                  resource_provider_state_t)
{
    kan_serialization_binary_script_storage_destroy (state->shared_script_storage);
    kan_stack_group_allocator_reset (&state->temporary_allocator);
}

static void generated_loaded_entry_init (kan_memory_size_t function_user_data, void *data)
{
    struct kan_resource_loaded_entry_view_t *instance = data;
    const struct kan_reflection_struct_t *boxed_type = (const struct kan_reflection_struct_t *) function_user_data;

    instance->entry_id = KAN_TYPED_ID_32_SET_INVALID (kan_resource_entry_id_t);
    instance->name = NULL;
    instance->my_allocation_group = kan_allocation_group_stack_get ();
    instance->loading_data = NULL;
    instance->data_ready = false;

    if (boxed_type->init)
    {
        void *contained_data =
            (void *) kan_apply_alignment ((kan_memory_size_t) instance->data_begin, boxed_type->alignment);

        kan_allocation_group_stack_push (instance->my_allocation_group);
        boxed_type->init (boxed_type->functor_user_data, contained_data);
        kan_allocation_group_stack_pop ();
    }
}

static void generated_loaded_entry_shutdown (kan_memory_size_t function_user_data, void *data)
{
    struct kan_resource_loaded_entry_view_t *instance = data;
    const struct kan_reflection_struct_t *boxed_type = (const struct kan_reflection_struct_t *) function_user_data;

    if (instance->loading_data)
    {
        if (boxed_type->shutdown)
        {
            kan_allocation_group_stack_push (instance->my_allocation_group);
            boxed_type->shutdown (boxed_type->functor_user_data, instance->loading_data);
            kan_allocation_group_stack_pop ();
        }

        kan_free_batched (instance->my_allocation_group, instance->loading_data);
    }

    if (boxed_type->shutdown)
    {
        void *contained_data =
            (void *) kan_apply_alignment ((kan_memory_size_t) instance->data_begin, boxed_type->alignment);

        kan_allocation_group_stack_push (instance->my_allocation_group);
        boxed_type->shutdown (boxed_type->functor_user_data, contained_data);
        kan_allocation_group_stack_pop ();
    }
}

static void generated_mutator_init (kan_memory_size_t function_user_data, void *data)
{
    struct kan_reflection_generator_universe_resource_provider_t *generator =
        (struct kan_reflection_generator_universe_resource_provider_t *) function_user_data;

    struct resource_provider_state_t *instance = data;
    instance->trailing_data_count = generator->nodes_count;
    resource_provider_state_init (instance);
}

static void generated_mutator_shutdown (kan_memory_size_t function_user_data, void *data)
{
    struct resource_provider_state_t *instance = data;
    resource_provider_state_shutdown (instance);
}

static void generated_mutator_deploy (kan_memory_size_t user_data, void *return_address, void *arguments_address)
{
    KAN_UNIVERSE_EXTRACT_DEPLOY_ARGUMENTS (arguments, arguments_address, struct resource_provider_state_t);
    mutator_template_deploy_resource_provider (arguments->universe, arguments->world, arguments->world_repository,
                                               arguments->workflow_node, arguments->state);

    struct kan_reflection_generator_universe_resource_provider_t *generator =
        (struct kan_reflection_generator_universe_resource_provider_t *) user_data;
    kan_reflection_registry_t registry = kan_repository_get_reflection_registry (arguments->world_repository);

    const char *entry_id_name = "entry_id";
    struct kan_repository_field_path_t entry_id_path = {
        .reflection_path_length = 1u,
        &entry_id_name,
    };

    struct universe_resource_provider_generated_node_t *interface_source = generator->first_node;
    for (kan_memory_size_t index = 0u; index < arguments->state->trailing_data_count;
         ++index, interface_source = interface_source->next)
    {
        struct resource_provider_resource_type_interface_t *interface = &arguments->state->trailing_data[index];
        interface->resource_type_name = interface_source->source_resource_type->name;
        interface->source_node = interface_source;

        kan_repository_indexed_storage_t loaded_entry_storage = kan_repository_indexed_storage_open (
            arguments->world_repository, interface->source_node->loaded_entry_type.name);

        kan_repository_indexed_insert_query_init (&interface->insert_loaded_entry, loaded_entry_storage);
        kan_universe_register_indexed_insert_from_mutator (registry, arguments->workflow_node,
                                                           interface->source_node->loaded_entry_type.name);

        kan_repository_indexed_value_read_query_init (&interface->read_loaded_entry_by_id, loaded_entry_storage,
                                                      entry_id_path);
        kan_universe_register_indexed_read_from_mutator (registry, arguments->workflow_node,
                                                         interface->source_node->loaded_entry_type.name);

        kan_repository_indexed_value_update_query_init (&interface->update_loaded_entry_by_id, loaded_entry_storage,
                                                        entry_id_path);
        kan_universe_register_indexed_update_from_mutator (registry, arguments->workflow_node,
                                                           interface->source_node->loaded_entry_type.name);

        kan_repository_indexed_value_delete_query_init (&interface->delete_loaded_entry_by_id, loaded_entry_storage,
                                                        entry_id_path);
        kan_universe_register_indexed_delete_from_mutator (registry, arguments->workflow_node,
                                                           interface->source_node->loaded_entry_type.name);

        kan_repository_indexed_value_write_query_init (&interface->write_loaded_entry_by_id, loaded_entry_storage,
                                                       entry_id_path);
        kan_universe_register_indexed_write_from_mutator (registry, arguments->workflow_node,
                                                          interface->source_node->loaded_entry_type.name);

        kan_repository_event_storage_t registered_storage = kan_repository_event_storage_open (
            arguments->world_repository, interface->source_node->registered_event_type.name);

        kan_repository_event_insert_query_init (&interface->insert_registered_event, registered_storage);
        kan_universe_register_event_insert_from_mutator (registry, arguments->workflow_node,
                                                         interface->source_node->registered_event_type.name);

        kan_repository_event_storage_t updated_storage = kan_repository_event_storage_open (
            arguments->world_repository, interface->source_node->updated_event_type.name);

        kan_repository_event_insert_query_init (&interface->insert_updated_event, updated_storage);
        kan_universe_register_event_insert_from_mutator (registry, arguments->workflow_node,
                                                         interface->source_node->updated_event_type.name);

        kan_repository_event_storage_t loaded_storage = kan_repository_event_storage_open (
            arguments->world_repository, interface->source_node->loaded_event_type.name);

        kan_repository_event_insert_query_init (&interface->insert_loaded_event, loaded_storage);
        kan_universe_register_event_insert_from_mutator (registry, arguments->workflow_node,
                                                         interface->source_node->loaded_event_type.name);

        kan_repository_event_storage_t unload_planned_storage = kan_repository_event_storage_open (
            arguments->world_repository, interface->source_node->unload_planned_event_type.name);

        kan_repository_event_insert_query_init (&interface->insert_unload_planned_event, unload_planned_storage);
        kan_universe_register_event_insert_from_mutator (registry, arguments->workflow_node,
                                                         interface->source_node->unload_planned_event_type.name);

        kan_repository_event_storage_t unregistered_storage = kan_repository_event_storage_open (
            arguments->world_repository, interface->source_node->unregistered_event_type.name);

        kan_repository_event_insert_query_init (&interface->insert_unregistered_event, unregistered_storage);
        kan_universe_register_event_insert_from_mutator (registry, arguments->workflow_node,
                                                         interface->source_node->unregistered_event_type.name);
    }
}

static void generated_mutator_execute (kan_memory_size_t user_data, void *return_address, void *arguments_address)
{
    KAN_UNIVERSE_EXTRACT_EXECUTE_ARGUMENTS (arguments, arguments_address, struct resource_provider_state_t);
    mutator_template_execute_resource_provider (arguments->job, arguments->state);
}

static void generated_mutator_undeploy (kan_memory_size_t user_data, void *return_address, void *arguments_address)
{
    KAN_UNIVERSE_EXTRACT_UNDEPLOY_ARGUMENTS (arguments, arguments_address, struct resource_provider_state_t);
    mutator_template_undeploy_resource_provider (arguments->state);

    for (kan_memory_size_t index = 0u; index < arguments->state->trailing_data_count; ++index)
    {
        struct resource_provider_resource_type_interface_t *interface = &arguments->state->trailing_data[index];
        kan_repository_indexed_insert_query_shutdown (&interface->insert_loaded_entry);
        kan_repository_indexed_value_read_query_shutdown (&interface->read_loaded_entry_by_id);
        kan_repository_indexed_value_update_query_shutdown (&interface->update_loaded_entry_by_id);
        kan_repository_indexed_value_delete_query_shutdown (&interface->delete_loaded_entry_by_id);
        kan_repository_indexed_value_write_query_shutdown (&interface->write_loaded_entry_by_id);

        kan_repository_event_insert_query_shutdown (&interface->insert_registered_event);
        kan_repository_event_insert_query_shutdown (&interface->insert_updated_event);
        kan_repository_event_insert_query_shutdown (&interface->insert_loaded_event);
        kan_repository_event_insert_query_shutdown (&interface->insert_unload_planned_event);
        kan_repository_event_insert_query_shutdown (&interface->insert_unregistered_event);
    }
}

UNIVERSE_RESOURCE_PROVIDER_API void kan_reflection_generator_universe_resource_provider_init (
    struct kan_reflection_generator_universe_resource_provider_t *instance)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_cpu_static_sections_ensure_initialized ();

    instance->generated_reflection_group =
        kan_allocation_group_get_child (kan_allocation_group_stack_get (), "generated_reflection");
    instance->first_node = NULL;
    instance->nodes_count = 0u;
}

UNIVERSE_RESOURCE_PROVIDER_API void kan_reflection_generator_universe_resource_provider_shutdown (
    struct kan_reflection_generator_universe_resource_provider_t *instance)
{
    struct universe_resource_provider_generated_node_t *node = instance->first_node;
    while (node)
    {
        struct universe_resource_provider_generated_node_t *next = node->next;

        // We do not generate visibility data for containers, therefore we can just deallocate fields.

        kan_free_general (instance->generated_reflection_group, node->loaded_entry_type.fields,
                          sizeof (struct kan_reflection_field_t) * node->loaded_entry_type.fields_count);

        kan_free_general (instance->generated_reflection_group, node->registered_event_type.fields,
                          sizeof (struct kan_reflection_field_t) * node->registered_event_type.fields_count);

        kan_free_general (instance->generated_reflection_group, node->updated_event_type.fields,
                          sizeof (struct kan_reflection_field_t) * node->updated_event_type.fields_count);

        kan_free_general (instance->generated_reflection_group, node->loaded_event_type.fields,
                          sizeof (struct kan_reflection_field_t) * node->loaded_event_type.fields_count);

        kan_free_general (instance->generated_reflection_group, node->unload_planned_event_type.fields,
                          sizeof (struct kan_reflection_field_t) * node->unload_planned_event_type.fields_count);

        kan_free_general (instance->generated_reflection_group, node,
                          sizeof (struct universe_resource_provider_generated_node_t));

        kan_free_general (instance->generated_reflection_group, node->unregistered_event_type.fields,
                          sizeof (struct kan_reflection_field_t) * node->unregistered_event_type.fields_count);
        node = next;
    }

    // We do not generate visibility data for containers, therefore we can just deallocate fields.
    kan_free_general (instance->generated_reflection_group, instance->mutator_type.fields,
                      sizeof (struct kan_reflection_field_t) * instance->mutator_type.fields_count);

    kan_free_general (instance->generated_reflection_group, instance->mutator_deploy_function.arguments,
                      sizeof (struct kan_reflection_argument_t) * instance->mutator_deploy_function.arguments_count);

    kan_free_general (instance->generated_reflection_group, instance->mutator_execute_function.arguments,
                      sizeof (struct kan_reflection_argument_t) * instance->mutator_execute_function.arguments_count);

    kan_free_general (instance->generated_reflection_group, instance->mutator_undeploy_function.arguments,
                      sizeof (struct kan_reflection_argument_t) * instance->mutator_undeploy_function.arguments_count);
}

UNIVERSE_RESOURCE_PROVIDER_API void kan_reflection_generator_universe_resource_provider_bootstrap (
    struct kan_reflection_generator_universe_resource_provider_t *instance, kan_memory_size_t bootstrap_iteration)
{
    instance->boostrap_iteration = bootstrap_iteration;
}

static inline bool is_resource_type_already_registered (
    struct kan_reflection_generator_universe_resource_provider_t *instance, kan_interned_string_t type_name)
{
    struct universe_resource_provider_generated_node_t *node = instance->first_node;
    while (node)
    {
        if (node->source_resource_type->name == type_name)
        {
            return true;
        }

        node = node->next;
    }

    return false;
}

static inline void register_resource_type (struct kan_reflection_generator_universe_resource_provider_t *instance,
                                           const struct kan_reflection_struct_t *type,
                                           kan_reflection_system_generation_iterator_t generation_iterator,
                                           const struct kan_resource_type_meta_t *meta)
{
    struct universe_resource_provider_generated_node_t *node = kan_allocate_general (
        instance->generated_reflection_group, sizeof (struct universe_resource_provider_generated_node_t),
        alignof (struct universe_resource_provider_generated_node_t));

    node->source_resource_type = type;
    node->next = instance->first_node;
    instance->first_node = node;
    ++instance->nodes_count;

    node->streamed = meta->flags & KAN_RESOURCE_TYPE_STREAMED;
    node->transitively_loaded = meta->flags & KAN_RESOURCE_TYPE_TRANSITIVELY_LOADED;
    char buffer[256u];

    // Generated loaded entry struct.

    const kan_memory_size_t container_alignment =
        KAN_MAX (alignof (struct kan_resource_loaded_entry_view_t), type->alignment);
    const kan_memory_size_t data_offset =
        kan_apply_alignment (offsetof (struct kan_resource_loaded_entry_view_t, data_begin), container_alignment);

    snprintf (buffer, sizeof (buffer), KAN_RESOURCE_PROVIDER_LOADED_ENTRY_TYPE_FORMAT, type->name);
    node->loaded_entry_type.name = kan_string_intern (buffer);

    node->loaded_entry_type.alignment = (kan_instance_size_t) container_alignment;
    node->loaded_entry_type.size =
        (kan_instance_size_t) kan_apply_alignment (data_offset + type->size, container_alignment);

    node->loaded_entry_type.functor_user_data = (kan_memory_size_t) type;
    node->loaded_entry_type.init = generated_loaded_entry_init;
    node->loaded_entry_type.shutdown = generated_loaded_entry_shutdown;

    node->loaded_entry_type.fields_count = 4u;
    node->loaded_entry_type.fields =
        kan_allocate_general (instance->generated_reflection_group,
                              sizeof (struct kan_reflection_field_t) * node->loaded_entry_type.fields_count,
                              alignof (struct kan_reflection_field_t));

#define ADD_FIELD(TARGET, BASE, TYPE, NAME, ARCHETYPE)                                                                 \
    (TARGET).name = KAN_STATIC_INTERNED_ID_GET (NAME);                                                                 \
    (TARGET).offset = offsetof (struct BASE, NAME);                                                                    \
    (TARGET).size = sizeof (TYPE);                                                                                     \
    (TARGET).archetype = ARCHETYPE;                                                                                    \
    (TARGET).visibility_condition_field = NULL;                                                                        \
    (TARGET).visibility_condition_values_count = 0u;                                                                   \
    (TARGET).visibility_condition_values = NULL

    ADD_FIELD (node->loaded_entry_type.fields[0u], kan_resource_loaded_entry_view_t, kan_resource_entry_id_t, entry_id,
               KAN_REFLECTION_ARCHETYPE_PACKED_ELEMENTAL);
    ADD_FIELD (node->loaded_entry_type.fields[1u], kan_resource_loaded_entry_view_t, kan_interned_string_t, name,
               KAN_REFLECTION_ARCHETYPE_INTERNED_STRING);
    // Fields `my_allocation_group` and `loading_data` are not registered as we never would like to migrate them anyway.
    ADD_FIELD (node->loaded_entry_type.fields[2u], kan_resource_loaded_entry_view_t, bool, data_ready,
               KAN_REFLECTION_ARCHETYPE_UNSIGNED_INT);

    node->loaded_entry_type.fields[3u].name = KAN_STATIC_INTERNED_ID_GET (stored_resource);
    node->loaded_entry_type.fields[3u].offset = (kan_instance_size_t) data_offset;
    node->loaded_entry_type.fields[3u].size = type->size;
    node->loaded_entry_type.fields[3u].archetype = KAN_REFLECTION_ARCHETYPE_STRUCT;
    node->loaded_entry_type.fields[3u].archetype_struct.type_name = type->name;
    node->loaded_entry_type.fields[3u].visibility_condition_field = NULL;
    node->loaded_entry_type.fields[3u].visibility_condition_values_count = 0u;
    node->loaded_entry_type.fields[3u].visibility_condition_values = NULL;
    kan_reflection_system_generation_iterator_add_struct (generation_iterator, &node->loaded_entry_type);

    // Generate registered event type.

    snprintf (buffer, sizeof (buffer), KAN_RESOURCE_PROVIDER_REGISTERED_EVENT_TYPE_FORMAT, type->name);
    node->registered_event_type.name = kan_string_intern (buffer);

    node->registered_event_type.alignment = alignof (struct kan_resource_registered_event_view_t);
    node->registered_event_type.size = sizeof (struct kan_resource_registered_event_view_t);

    node->registered_event_type.functor_user_data = 0u;
    node->registered_event_type.init = NULL;
    node->registered_event_type.shutdown = NULL;

    node->registered_event_type.fields_count = 2u;
    node->registered_event_type.fields =
        kan_allocate_general (instance->generated_reflection_group,
                              sizeof (struct kan_reflection_field_t) * node->registered_event_type.fields_count,
                              alignof (struct kan_reflection_field_t));

    ADD_FIELD (node->registered_event_type.fields[0u], kan_resource_registered_event_view_t, kan_resource_entry_id_t,
               entry_id, KAN_REFLECTION_ARCHETYPE_PACKED_ELEMENTAL);
    ADD_FIELD (node->registered_event_type.fields[1u], kan_resource_registered_event_view_t, kan_interned_string_t,
               name, KAN_REFLECTION_ARCHETYPE_INTERNED_STRING);
    kan_reflection_system_generation_iterator_add_struct (generation_iterator, &node->registered_event_type);

    // Generate updated event type.

    snprintf (buffer, sizeof (buffer), KAN_RESOURCE_PROVIDER_UPDATED_EVENT_TYPE_FORMAT, type->name);
    node->updated_event_type.name = kan_string_intern (buffer);

    node->updated_event_type.alignment = alignof (struct kan_resource_updated_event_view_t);
    node->updated_event_type.size = sizeof (struct kan_resource_updated_event_view_t);

    node->updated_event_type.functor_user_data = 0u;
    node->updated_event_type.init = NULL;
    node->updated_event_type.shutdown = NULL;

    node->updated_event_type.fields_count = 2u;
    node->updated_event_type.fields =
        kan_allocate_general (instance->generated_reflection_group,
                              sizeof (struct kan_reflection_field_t) * node->updated_event_type.fields_count,
                              alignof (struct kan_reflection_field_t));

    ADD_FIELD (node->updated_event_type.fields[0u], kan_resource_updated_event_view_t, kan_resource_entry_id_t,
               entry_id, KAN_REFLECTION_ARCHETYPE_PACKED_ELEMENTAL);
    ADD_FIELD (node->updated_event_type.fields[1u], kan_resource_updated_event_view_t, kan_interned_string_t, name,
               KAN_REFLECTION_ARCHETYPE_INTERNED_STRING);
    kan_reflection_system_generation_iterator_add_struct (generation_iterator, &node->updated_event_type);

    // Generate loaded event type.

    snprintf (buffer, sizeof (buffer), KAN_RESOURCE_PROVIDER_LOADED_EVENT_TYPE_FORMAT, type->name);
    node->loaded_event_type.name = kan_string_intern (buffer);

    node->loaded_event_type.alignment = alignof (struct kan_resource_loaded_event_view_t);
    node->loaded_event_type.size = sizeof (struct kan_resource_loaded_event_view_t);

    node->loaded_event_type.functor_user_data = 0u;
    node->loaded_event_type.init = NULL;
    node->loaded_event_type.shutdown = NULL;

    node->loaded_event_type.fields_count = 2u;
    node->loaded_event_type.fields =
        kan_allocate_general (instance->generated_reflection_group,
                              sizeof (struct kan_reflection_field_t) * node->loaded_event_type.fields_count,
                              alignof (struct kan_reflection_field_t));

    ADD_FIELD (node->loaded_event_type.fields[0u], kan_resource_loaded_event_view_t, kan_resource_entry_id_t, entry_id,
               KAN_REFLECTION_ARCHETYPE_PACKED_ELEMENTAL);
    ADD_FIELD (node->loaded_event_type.fields[1u], kan_resource_loaded_event_view_t, kan_interned_string_t, name,
               KAN_REFLECTION_ARCHETYPE_INTERNED_STRING);
    kan_reflection_system_generation_iterator_add_struct (generation_iterator, &node->loaded_event_type);

    snprintf (buffer, sizeof (buffer), KAN_RESOURCE_PROVIDER_UNLOAD_PLANNED_EVENT_TYPE_FORMAT, type->name);
    node->unload_planned_event_type.name = kan_string_intern (buffer);

    node->unload_planned_event_type.alignment = alignof (struct kan_resource_unload_planned_event_view_t);
    node->unload_planned_event_type.size = sizeof (struct kan_resource_unload_planned_event_view_t);

    node->unload_planned_event_type.functor_user_data = 0u;
    node->unload_planned_event_type.init = NULL;
    node->unload_planned_event_type.shutdown = NULL;

    node->unload_planned_event_type.fields_count = 2u;
    node->unload_planned_event_type.fields =
        kan_allocate_general (instance->generated_reflection_group,
                              sizeof (struct kan_reflection_field_t) * node->unload_planned_event_type.fields_count,
                              alignof (struct kan_reflection_field_t));

    ADD_FIELD (node->unload_planned_event_type.fields[0u], kan_resource_unload_planned_event_view_t,
               kan_resource_entry_id_t, entry_id, KAN_REFLECTION_ARCHETYPE_PACKED_ELEMENTAL);
    ADD_FIELD (node->unload_planned_event_type.fields[1u], kan_resource_unload_planned_event_view_t,
               kan_interned_string_t, name, KAN_REFLECTION_ARCHETYPE_INTERNED_STRING);
    kan_reflection_system_generation_iterator_add_struct (generation_iterator, &node->unload_planned_event_type);

    // Generate unregistered event type.

    snprintf (buffer, sizeof (buffer), KAN_RESOURCE_PROVIDER_UNREGISTERED_EVENT_TYPE_FORMAT, type->name);
    node->unregistered_event_type.name = kan_string_intern (buffer);

    node->unregistered_event_type.alignment = alignof (struct kan_resource_unregistered_event_view_t);
    node->unregistered_event_type.size = sizeof (struct kan_resource_unregistered_event_view_t);

    node->unregistered_event_type.functor_user_data = 0u;
    node->unregistered_event_type.init = NULL;
    node->unregistered_event_type.shutdown = NULL;

    node->unregistered_event_type.fields_count = 2u;
    node->unregistered_event_type.fields =
        kan_allocate_general (instance->generated_reflection_group,
                              sizeof (struct kan_reflection_field_t) * node->unregistered_event_type.fields_count,
                              alignof (struct kan_reflection_field_t));

    ADD_FIELD (node->unregistered_event_type.fields[0u], kan_resource_unregistered_event_view_t,
               kan_resource_entry_id_t, entry_id, KAN_REFLECTION_ARCHETYPE_PACKED_ELEMENTAL);
    ADD_FIELD (node->unregistered_event_type.fields[1u], kan_resource_unregistered_event_view_t, kan_interned_string_t,
               name, KAN_REFLECTION_ARCHETYPE_INTERNED_STRING);
    kan_reflection_system_generation_iterator_add_struct (generation_iterator, &node->unregistered_event_type);

#undef ADD_FIELD
}

UNIVERSE_RESOURCE_PROVIDER_API void kan_reflection_generator_universe_resource_provider_iterate (
    struct kan_reflection_generator_universe_resource_provider_t *instance,
    kan_reflection_registry_t registry,
    kan_reflection_system_generation_iterator_t iterator,
    kan_memory_size_t iteration_index)
{
    // Cannot use inside macro below due to pushes, need to forward declare like that.
    const kan_interned_string_t meta_name = KAN_STATIC_INTERNED_ID_GET (kan_resource_type_meta_t);
    KAN_UNIVERSE_REFLECTION_GENERATOR_STRUCT_META_SCANNER_CORE (universe_resource_provider);

    KAN_UNIVERSE_REFLECTION_GENERATOR_ON_STRUCT_META_SCANNED (kan_resource_type_meta_t, meta_name)
    {
        if (!is_resource_type_already_registered (instance, type->name))
        {
            register_resource_type (instance, type, iterator, meta);
        }
    }
}

UNIVERSE_RESOURCE_PROVIDER_API void kan_reflection_generator_universe_resource_provider_finalize (
    struct kan_reflection_generator_universe_resource_provider_t *instance, kan_reflection_registry_t registry)
{
    if (instance->first_node)
    {
        KAN_UNIVERSE_REFLECTION_GENERATOR_NODE_LIST_TO_TEMPORARY_ARRAY (
            struct universe_resource_provider_generated_node_t, nodes_array, instance->first_node,
            instance->nodes_count, instance->generated_reflection_group)

        {
            struct universe_resource_provider_generated_node_t *temporary;

// We use pointer comparison as strings are interned.
#define LESS(first_index, second_index)                                                                                \
    __CUSHION_PRESERVE__ nodes_array[first_index]->source_resource_type->name <                                        \
        nodes_array[second_index]->source_resource_type->name

#define SWAP(first_index, second_index)                                                                                \
    __CUSHION_PRESERVE__                                                                                               \
    temporary = nodes_array[first_index], nodes_array[first_index] = nodes_array[second_index],                        \
    nodes_array[second_index] = temporary

            QSORT (instance->nodes_count, LESS, SWAP);
#undef LESS
#undef SWAP
#undef AT_INDEX
        }

        instance->first_node = nodes_array[0u];
        KAN_UNIVERSE_REFLECTION_GENERATOR_NODE_REORDER_FROM_ARRAY (nodes_array, instance->nodes_count)
    }

    KAN_UNIVERSE_REFLECTION_GENERATOR_MUTATOR_TYPE (
        instance->mutator_type, instance, generated_resource_provider_state_t, resource_provider_state_t,
        resource_provider_resource_type_interface_t, instance->nodes_count, generated_mutator_init,
        generated_mutator_shutdown, instance->generated_reflection_group);
    kan_reflection_registry_add_struct (registry, &instance->mutator_type);

    KAN_UNIVERSE_REFLECTION_GENERATOR_DEPLOY_FUNCTION (
        instance->mutator_deploy_function, generated_resource_provider_state, generated_resource_provider_state_t,
        generated_mutator_deploy, (kan_memory_size_t) instance, instance->generated_reflection_group);
    kan_reflection_registry_add_function (registry, &instance->mutator_deploy_function);

    KAN_UNIVERSE_REFLECTION_GENERATOR_EXECUTE_FUNCTION (
        instance->mutator_execute_function, generated_resource_provider_state, generated_resource_provider_state_t,
        generated_mutator_execute, instance->generated_reflection_group);
    kan_reflection_registry_add_function (registry, &instance->mutator_execute_function);
    kan_reflection_registry_add_function_meta (registry, instance->mutator_execute_function.name,
                                               KAN_STATIC_INTERNED_ID_GET (kan_universe_mutator_group_meta_t),
                                               &universe_mutator_group_meta_resource_provider);

    KAN_UNIVERSE_REFLECTION_GENERATOR_UNDEPLOY_FUNCTION (
        instance->mutator_undeploy_function, generated_resource_provider_state, generated_resource_provider_state_t,
        generated_mutator_undeploy, instance->generated_reflection_group);
    kan_reflection_registry_add_function (registry, &instance->mutator_undeploy_function);
}

void kan_resource_provider_configuration_init (struct kan_resource_provider_configuration_t *instance)
{
    instance->transaction_budget_ns = 12000000u;
    instance->streaming_budget_ns = 2000000u;
    instance->resource_directory_path = kan_string_intern ("resources");
}

void kan_resource_provider_singleton_init (struct kan_resource_provider_singleton_t *instance)
{
    instance->streaming_id_counter = kan_atomic_int_init (1);
    instance->initial_scan_done = false;
    instance->essential_loading_done = false;
    instance->required_loading_done = false;
    instance->transaction_state = KAN_RESOURCE_TRANSACTION_STATE_NONE;
    instance->logic_deduplication_frame_id = 0u;
    instance->tags_dirty = true;

    kan_dynamic_array_init (&instance->tags, KAN_UNIVERSE_RESOURCE_PROVIDER_TAGS_CAPACITY,
                            sizeof (kan_interned_string_t), alignof (kan_interned_string_t),
                            kan_allocation_group_stack_get ());
    instance->commit_locked = kan_atomic_int_init (0);
}

void kan_resource_provider_singleton_add_tag (struct kan_resource_provider_singleton_t *instance,
                                              kan_interned_string_t tag)
{
    for (kan_instance_size_t index = 0u; index < instance->tags.size; ++index)
    {
        if (((kan_interned_string_t *) instance->tags.data)[index] == tag)
        {
            return;
        }
    }

    kan_interned_string_t *spot = kan_dynamic_array_add_last (&instance->tags);
    if (!spot)
    {
        kan_dynamic_array_set_capacity (&instance->tags, instance->tags.capacity * 2u);
        spot = kan_dynamic_array_add_last (&instance->tags);
    }

    *spot = tag;
    instance->tags_dirty = true;
}

void kan_resource_provider_singleton_remove_tag (struct kan_resource_provider_singleton_t *instance,
                                                 kan_interned_string_t tag)
{
    for (kan_instance_size_t index = 0u; index < instance->tags.size; ++index)
    {
        if (((kan_interned_string_t *) instance->tags.data)[index] == tag)
        {
            kan_dynamic_array_remove_swap_at (&instance->tags, index);
            instance->tags_dirty = true;
            return;
        }
    }
}

void kan_resource_provider_singleton_shutdown (struct kan_resource_provider_singleton_t *instance)
{
    kan_dynamic_array_shutdown (&instance->tags);
}

void kan_resource_package_state_init (struct kan_resource_package_state_t *instance)
{
    instance->name = NULL;
    instance->loaded = false;
    instance->string_registry = KAN_HANDLE_SET_INVALID (kan_serialization_interned_string_registry_t);
    kan_resource_package_init (&instance->manifest);
}

void kan_resource_package_state_shutdown (struct kan_resource_package_state_t *instance)
{
    if (KAN_HANDLE_IS_VALID (instance->string_registry))
    {
        kan_serialization_interned_string_registry_destroy (instance->string_registry);
    }
}

void kan_resource_registered_entry_init (struct kan_resource_registered_entry_t *instance)
{
    instance->entry_id = KAN_TYPED_ID_32_SET_INVALID (kan_resource_entry_id_t);
    instance->package = NULL;
    instance->type = NULL;
    instance->name = NULL;
    instance->streaming_counter = 0u;

    instance->path_hash = 0u;
    instance->path = NULL;
    instance->my_allocation_group = kan_allocation_group_stack_get ();
}

void kan_resource_registered_entry_shutdown (struct kan_resource_registered_entry_t *instance)
{
    if (instance->path)
    {
        kan_free_general (instance->my_allocation_group, instance->path,
                          (kan_instance_size_t) strlen (instance->path) + 1u);
    }
}

void kan_resource_loaded_third_party_entry_init (struct kan_resource_loaded_third_party_entry_t *instance)
{
    instance->entry_id = KAN_TYPED_ID_32_SET_INVALID (kan_resource_entry_id_t);
    instance->name = NULL;
    instance->loaded_data = NULL;
    instance->loading_data = NULL;
    instance->loaded_data_size = 0u;
    instance->loading_data_size = 0u;
    instance->my_allocation_group = kan_allocation_group_stack_get ();
}

void kan_resource_loaded_third_party_entry_shutdown (struct kan_resource_loaded_third_party_entry_t *instance)
{
    if (instance->loaded_data)
    {
        kan_free_general (instance->my_allocation_group, instance->loaded_data,
                          (kan_instance_size_t) kan_apply_alignment (
                              instance->loaded_data_size, KAN_RESOURCE_PROVIDER_LOADED_THIRD_PARTY_ALIGNMENT));
    }

    if (instance->loading_data)
    {
        kan_free_general (instance->my_allocation_group, instance->loading_data,
                          (kan_instance_size_t) kan_apply_alignment (
                              instance->loading_data_size, KAN_RESOURCE_PROVIDER_LOADED_THIRD_PARTY_ALIGNMENT));
    }
}

void kan_resource_streaming_request_init (struct kan_resource_streaming_request_t *instance)
{
    instance->request_id = KAN_TYPED_ID_32_SET_INVALID (kan_resource_streaming_id_t);
    instance->type = NULL;
    instance->name = NULL;
    instance->priority = 0u;
}
