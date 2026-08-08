#include <string.h>

#include <kan/memory/allocation.h>
#include <kan/resource_pipeline/project.h>

static kan_allocation_group_t allocation_group;
static bool statics_initialized = false;

static void ensure_statics_initialized (void)
{
    if (!statics_initialized)
    {
        allocation_group = kan_allocation_group_get_child (kan_allocation_group_root (), "resource_pipeline_project");
        statics_initialized = true;
    }
}

kan_allocation_group_t kan_resource_project_get_allocation_group (void)
{
    ensure_statics_initialized ();
    return allocation_group;
}

void kan_resource_package_init (struct kan_resource_package_t *instance)
{
    ensure_statics_initialized ();
    instance->level = KAN_RESOURCE_PACKAGE_LEVEL_REQUIRED;
    kan_dynamic_array_init (&instance->trigger_tags, 0u, sizeof (kan_interned_string_t),
                            alignof (kan_interned_string_t), allocation_group);
}

void kan_resource_package_shutdown (struct kan_resource_package_t *instance)
{
    kan_dynamic_array_shutdown (&instance->trigger_tags);
}

void kan_resource_project_package_init (struct kan_resource_project_package_t *instance)
{
    instance->name = NULL;
    instance->group = KAN_RESOURCE_PACKAGE_GROUP_CORE;
    instance->plugin = NULL;
    instance->directory = NULL;
}

void kan_resource_project_package_shutdown (struct kan_resource_project_package_t *instance)
{
    if (instance->directory)
    {
        kan_free_general (allocation_group, instance->directory, strlen (instance->directory) + 1u);
    }
}

void kan_resource_project_init (struct kan_resource_project_t *instance)
{
    ensure_statics_initialized ();
    kan_dynamic_array_init (&instance->packages, 0u, sizeof (struct kan_resource_project_package_t),
                            alignof (struct kan_resource_project_package_t), allocation_group);

    instance->workspace_directory = NULL;
    instance->platform_configuration_directory = NULL;

    kan_dynamic_array_init (&instance->platform_configuration_tags, 0u, sizeof (kan_interned_string_t),
                            alignof (kan_interned_string_t), allocation_group);

    instance->plugin_directory_name = NULL;
    kan_dynamic_array_init (&instance->plugins, 0u, sizeof (kan_interned_string_t), alignof (kan_interned_string_t),
                            allocation_group);
}

void kan_resource_project_shutdown (struct kan_resource_project_t *instance)
{
    KAN_DYNAMIC_ARRAY_SHUTDOWN_WITH_ITEMS_AUTO (instance->packages, kan_resource_project_package)
    if (instance->workspace_directory)
    {
        kan_free_general (allocation_group, instance->workspace_directory, strlen (instance->workspace_directory) + 1u);
    }

    if (instance->platform_configuration_directory)
    {
        kan_free_general (allocation_group, instance->platform_configuration_directory,
                          strlen (instance->platform_configuration_directory) + 1u);
    }

    kan_dynamic_array_shutdown (&instance->platform_configuration_tags);
    if (instance->plugin_directory_name)
    {
        kan_free_general (allocation_group, instance->plugin_directory_name,
                          strlen (instance->plugin_directory_name) + 1u);
    }

    kan_dynamic_array_shutdown (&instance->plugins);
}
