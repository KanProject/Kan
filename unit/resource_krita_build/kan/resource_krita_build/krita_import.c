#include <kan/file_system/entry.h>
#include <kan/file_system/path_container.h>
#include <kan/file_system/stream.h>
#include <kan/log/logging.h>
#include <kan/platform/process.h>
#include <kan/resource_krita_build/krita_import.h>
#include <kan/resource_pipeline/meta.h>
#include <kan/serialization/readable_data.h>
#include <kan/stream/random_access_stream_buffer.h>

KAN_LOG_DEFINE_CATEGORY (resource_krita_import);

KAN_REFLECTION_STRUCT_FIELD_META (kan_resource_krita_header_entry_t, file)
RESOURCE_KRITA_BUILD_API struct kan_resource_reference_meta_t kan_resource_krita_header_entry_reference_file = {
    .type_name = NULL,
    .flags = 0u,
};

KAN_REFLECTION_STRUCT_FIELD_META (kan_resource_krita_header_entry_t, locale)
RESOURCE_KRITA_BUILD_API struct kan_resource_reference_meta_t kan_resource_krita_header_entry_reference_locale = {
    .type_name = "kan_resource_locale_t",
    .flags = KAN_RESOURCE_REFERENCE_META_NULLABLE,
};

KAN_REFLECTION_STRUCT_META (kan_resource_krita_header_t)
RESOURCE_KRITA_BUILD_API struct kan_resource_type_meta_t kan_resource_krita_header_resource_type = {
    .flags = 0u,
    .version = CUSHION_START_NS_X64,
    .move = NULL,
    .reset = NULL,
};

void kan_resource_krita_header_entry_init (struct kan_resource_krita_header_entry_t *instance)
{
    instance->name = NULL;
    instance->file = NULL;
    instance->locale = NULL;
    instance->filter = NULL;
}

void kan_resource_krita_header_init (struct kan_resource_krita_header_t *instance)
{
    kan_dynamic_array_init (&instance->entries, 0u, sizeof (struct kan_resource_krita_header_entry_t),
                            alignof (struct kan_resource_krita_header_entry_t), kan_allocation_group_stack_get ());
}

void kan_resource_krita_header_shutdown (struct kan_resource_krita_header_t *instance)
{
    kan_dynamic_array_shutdown (&instance->entries);
}

static enum kan_resource_build_rule_result_t krita_import_build (struct kan_resource_build_rule_context_t *context);

KAN_REFLECTION_STRUCT_META (kan_resource_krita_header_t)
RESOURCE_KRITA_BUILD_API struct kan_resource_build_rule_t kan_resource_krita_header_build_rule = {
    .primary_input_type = NULL,
    .platform_configuration_type = NULL,
    .secondary_types_count = 0u,
    .secondary_types = NULL,
    .functor = krita_import_build,
    .version = CUSHION_START_NS_X64,
};

enum kan_resource_build_rule_result_t krita_import_build (struct kan_resource_build_rule_context_t *context)
{
    const char *input_path = context->primary_third_party_path;
    struct kan_resource_krita_header_t *output = context->primary_output;

    struct kan_file_system_path_container_t path_container;
    kan_file_system_path_container_copy_string (&path_container, __FILE__);
    KAN_ASSERT (path_container.length > sizeof ("krita_import.c"))
    kan_file_system_path_container_reset_length (&path_container,
                                                 path_container.length + 1u - sizeof ("krita_import.c"));

    kan_platform_environment_t environment = kan_platform_environment_create_and_retrieve ();
    CUSHION_DEFER { kan_platform_environment_destroy (environment); }
    kan_platform_environment_set (environment, "PYTHONPATH", path_container.path);

    kan_platform_argument_list_t arguments = kan_platform_argument_list_create ();
    CUSHION_DEFER { kan_platform_argument_list_destroy (arguments); }

    kan_platform_argument_list_append (arguments, "-s");
    kan_platform_argument_list_append (arguments, "krita_import");
    kan_platform_argument_list_append (arguments, input_path);
    kan_platform_argument_list_append (arguments, context->temporary_workspace);

    if (kan_platform_execute_sub_process ("kritarunner", arguments, environment) != 0)
    {
        KAN_LOG (resource_krita_import, KAN_LOG_ERROR,
                 "Failed to execute kritarunner for file \"%s\" for read in order to import images from \"%s\".",
                 input_path, context->primary_name)
        return KAN_RESOURCE_BUILD_RULE_FAILURE;
    }

    kan_file_system_path_container_copy_string (&path_container, context->temporary_workspace);
    const kan_instance_size_t path_base_length = path_container.length;
    kan_file_system_path_container_append (&path_container, "success.txt");

    if (!kan_file_system_check_existence (path_container.path))
    {
        KAN_LOG (resource_krita_import, KAN_LOG_ERROR,
                 "Krita script execution for file \"%s\" failed as it didn't produce \"success.txt\".", input_path)
        return KAN_RESOURCE_BUILD_RULE_FAILURE;
    }

    kan_file_system_path_container_reset_length (&path_container, path_base_length);
    kan_file_system_path_container_append (&path_container, "header.rd");

    if (!kan_file_system_check_existence (path_container.path))
    {
        KAN_LOG (resource_krita_import, KAN_LOG_ERROR,
                 "Krita script execution for file \"%s\" didn't produce \"header.rd\".", input_path)
        return KAN_RESOURCE_BUILD_RULE_FAILURE;
    }

    struct kan_stream_t *input_stream = kan_direct_file_stream_open_for_read (path_container.path, false);
    if (!input_stream)
    {
        KAN_LOG_WITH_BUFFER (KAN_FILE_SYSTEM_MAX_PATH_LENGTH * 2u, resource_krita_import, KAN_LOG_ERROR,
                             "Failed to open \"%s\" to read import result for \"%s\".", input_path,
                             context->primary_name)
        return KAN_RESOURCE_BUILD_RULE_FAILURE;
    }

    input_stream = kan_random_access_stream_buffer_open_for_read (input_stream, KAN_RESOURCE_KRITA_IO_BUFFER);
    CUSHION_DEFER { input_stream->operations->close (input_stream); }
    kan_file_system_path_container_reset_length (&path_container, path_base_length);

    kan_serialization_rd_reader_t reader =
        kan_serialization_rd_reader_create (input_stream, output, kan_string_intern ("kan_resource_krita_header_t"),
                                            context->reflection_registry, output->entries.allocation_group);

    CUSHION_DEFER { kan_serialization_rd_reader_destroy (reader); }
    enum kan_serialization_state_t serialization_state;

    while ((serialization_state = kan_serialization_rd_reader_step (reader)) == KAN_SERIALIZATION_IN_PROGRESS)
    {
    }

    if (serialization_state == KAN_SERIALIZATION_FAILED)
    {
        KAN_LOG_WITH_BUFFER (KAN_FILE_SYSTEM_MAX_PATH_LENGTH * 2u, resource_krita_import, KAN_LOG_ERROR,
                             "Failed to deserialize \"%s\" to read import result for \"%s\".", input_path,
                             context->primary_name)
        return KAN_RESOURCE_BUILD_RULE_FAILURE;
    }

    // We need to register exported files as third party secondary products.
    bool successful = true;

    for (kan_instance_size_t index = 0u; index < output->entries.size; ++index)
    {
        struct kan_resource_krita_header_entry_t *entry =
            &((struct kan_resource_krita_header_entry_t *) output->entries.data)[index];

        kan_file_system_path_container_append (&path_container, entry->file);
        CUSHION_DEFER { kan_file_system_path_container_reset_length (&path_container, path_base_length); }

        kan_interned_string_t produced_name = NULL;
        if (!context->produce_third_party_secondary_output (context->interface, path_container.path, &produced_name))
        {
            KAN_LOG_WITH_BUFFER (
                KAN_FILE_SYSTEM_MAX_PATH_LENGTH * 2u, resource_krita_import, KAN_LOG_ERROR,
                "Failed to register \"%s\" as third party secondary output while importing it from \"%s\".",
                path_container.path, context->primary_name)
            successful = false;
            continue;
        }

        entry->file = produced_name;
    }

    return successful ? KAN_RESOURCE_BUILD_RULE_SUCCESS : KAN_RESOURCE_BUILD_RULE_FAILURE;
}
