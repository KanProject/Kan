#include <string.h>

#include <kan/resource_pipeline/log.h>

static kan_allocation_group_t allocation_group;
static bool statics_initialized = false;

static void ensure_statics_initialized (void)
{
    if (!statics_initialized)
    {
        allocation_group = kan_allocation_group_get_child (kan_allocation_group_root (), "resource_pipeline_log");
        statics_initialized = true;
    }
}

kan_allocation_group_t kan_resource_log_get_allocation_group (void)
{
    ensure_statics_initialized ();
    return allocation_group;
}

void kan_resource_log_entry_init (struct kan_resource_log_entry_t *instance)
{
    instance->type = NULL;
    instance->name = NULL;
    instance->version.type_version = 0u;
    instance->version.last_modification_time = 0u;

    kan_dynamic_array_init (&instance->references, 0u, sizeof (struct kan_resource_log_reference_t),
                            alignof (struct kan_resource_log_reference_t), allocation_group);

    instance->saved_directory = KAN_RESOURCE_LOG_SAVED_DIRECTORY_CACHE;
    instance->source = KAN_RESOURCE_LOG_ENTRY_SOURCE_RAW;

    kan_dynamic_array_init (&instance->additional_dependencies, 0u, sizeof (struct kan_resource_log_dependency_t),
                            alignof (struct kan_resource_log_dependency_t), allocation_group);
}

void kan_resource_log_entry_init_copy (struct kan_resource_log_entry_t *instance,
                                       const struct kan_resource_log_entry_t *copy_from)
{
    instance->type = copy_from->type;
    instance->name = copy_from->name;
    instance->version = copy_from->version;

    kan_dynamic_array_init (&instance->references, copy_from->references.size,
                            sizeof (struct kan_resource_log_reference_t), alignof (struct kan_resource_log_reference_t),
                            allocation_group);

    instance->references.size = copy_from->references.size;
    memcpy (instance->references.data, copy_from->references.data,
            sizeof (struct kan_resource_log_reference_t) * instance->references.size);

    instance->saved_directory = copy_from->saved_directory;
    instance->source = copy_from->source;

    switch (instance->source)
    {
    case KAN_RESOURCE_LOG_ENTRY_SOURCE_RAW:
        break;

    case KAN_RESOURCE_LOG_ENTRY_SOURCE_PRIMARY:
        instance->source_primary = copy_from->source_primary;
        break;

    case KAN_RESOURCE_LOG_ENTRY_SOURCE_SECONDARY:
        instance->source_secondary = copy_from->source_secondary;
        break;
    }

    kan_dynamic_array_init (&instance->additional_dependencies, copy_from->additional_dependencies.size,
                            sizeof (struct kan_resource_log_dependency_t),
                            alignof (struct kan_resource_log_dependency_t), allocation_group);

    instance->additional_dependencies.size = copy_from->additional_dependencies.size;
    memcpy (instance->additional_dependencies.data, copy_from->additional_dependencies.data,
            sizeof (struct kan_resource_log_dependency_t) * instance->additional_dependencies.size);
}

void kan_resource_log_entry_shutdown (struct kan_resource_log_entry_t *instance)
{
    kan_dynamic_array_shutdown (&instance->references);
    kan_dynamic_array_shutdown (&instance->additional_dependencies);
}

void kan_resource_log_target_init (struct kan_resource_log_target_t *instance)
{
    instance->name = NULL;
    kan_dynamic_array_init (&instance->entries, 0u, sizeof (struct kan_resource_log_entry_t),
                            alignof (struct kan_resource_log_entry_t), allocation_group);
}

void kan_resource_log_target_init_copy (struct kan_resource_log_target_t *instance,
                                        const struct kan_resource_log_target_t *copy_from)
{
    instance->name = copy_from->name;
    kan_dynamic_array_init (&instance->entries, copy_from->entries.size, sizeof (struct kan_resource_log_entry_t),
                            alignof (struct kan_resource_log_entry_t), allocation_group);

    for (kan_memory_size_t index = 0u; index < copy_from->entries.size; ++index)
    {
        const struct kan_resource_log_entry_t *input =
            &((struct kan_resource_log_entry_t *) copy_from->entries.data)[index];
        struct kan_resource_log_entry_t *output = kan_dynamic_array_add_last (&instance->entries);
        kan_resource_log_entry_init_copy (output, input);
    }
}

void kan_resource_log_target_shutdown (struct kan_resource_log_target_t *instance)
{
    KAN_DYNAMIC_ARRAY_SHUTDOWN_WITH_ITEMS_AUTO (instance->entries, kan_resource_log_entry)
}

void kan_resource_log_init (struct kan_resource_log_t *instance)
{
    kan_dynamic_array_init (&instance->targets, 0u, sizeof (struct kan_resource_log_target_t),
                            alignof (struct kan_resource_log_target_t), allocation_group);
}

void kan_resource_log_shutdown (struct kan_resource_log_t *instance)
{
    KAN_DYNAMIC_ARRAY_SHUTDOWN_WITH_ITEMS_AUTO (instance->targets, kan_resource_log_target)
}
