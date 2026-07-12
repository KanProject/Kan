#pragma once

#include <resource_pipeline_api.h>

#include <kan/api_common/c_header.h>
#include <kan/api_common/core_types.h>
#include <kan/container/dynamic_array.h>
#include <kan/container/interned_string.h>
#include <kan/reflection/markup.h>

/// \file
/// \brief Contains resource project format declaration for resource pipeline tools.

KAN_C_HEADER_BEGIN

/// \brief Fixed name for package setup file that is used in raw resources for assigning directories to packages.
#define KAN_RESOURCE_PACKAGE_RAW_FILE_NAME ".package.rd"

/// \brief Fixed name for package setup file in binary format in built resources.
#define KAN_RESOURCE_PACKAGE_FILE_NAME ".package"

/// \brief Returns allocation group that is used for allocating everything connected to resource project.
RESOURCE_PIPELINE_API kan_allocation_group_t kan_resource_project_get_allocation_group (void);

enum kan_resource_package_level_t
{
    /// \brief Essential packages are needed to start booting the app, for example to show the loading screen.
    /// \details These packages are always loaded before anything else is loaded, which is important for stuff like
    ///          loading screen UI. Size and count of essential packages should be as small as possible, because the
    ///          whole reason of separating them is to load them fast and show something to the user before loading the
    ///          main bulk of potentially heavy packages.
    KAN_RESOURCE_PACKAGE_LEVEL_ESSENTIAL = 0u,

    /// \brief Resources that should always be loaded, however their loading
    ///        only starts after essential level is fully loaded.
    KAN_RESOURCE_PACKAGE_LEVEL_REQUIRED,

    /// \brief Packages that should only be loaded if their loading conditions are met.
    /// \details Depending on your game genre and resource size, you're likely to have most of the packages on this
    ///          level. Strategy games and smaller games are likely to put most of their stuff into required level, but
    ///          most other genres operate on quite large sets of content and should not do so.
    KAN_RESOURCE_PACKAGE_LEVEL_OPTIONAL,
};

/// \brief Data structure for describing the package configuration in both raw and built resources.
struct kan_resource_package_t
{
    enum kan_resource_package_level_t level;

    /// \brief Enumerates tags that trigger loading for `KAN_RESOURCE_PACKAGE_LEVEL_OPTIONAL` packages.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (kan_interned_string_t)
    struct kan_dynamic_array_t trigger_tags;
};

RESOURCE_PIPELINE_API void kan_resource_package_init (struct kan_resource_package_t *instance);

RESOURCE_PIPELINE_API void kan_resource_package_shutdown (struct kan_resource_package_t *instance);

enum kan_resource_package_group_t
{
    /// \brief Package belongs to core group which means that it is visible to every other package around.
    KAN_RESOURCE_PACKAGE_GROUP_CORE = 0u,

    /// \brief Package belongs to the plugin group which means that it is
    ///        visible only to other packages in the same plugin.
    KAN_RESOURCE_PACKAGE_GROUP_PLUGIN,
};

/// \brief Defines one package that is used as target for resource build.
struct kan_resource_project_package_t
{
    /// \brief Name of this package, must be unique among packages.
    kan_interned_string_t name;

    /// \brief Group to which this package belongs.
    enum kan_resource_package_group_t group;

    /// \brief Plugin to which this package belongs if `KAN_RESOURCE_PACKAGE_GROUP_PLUGIN`.
    kan_interned_string_t plugin;

    /// \brief Resource directory to which this package is assigned. Absolute path.
    char *directory;
};

RESOURCE_PIPELINE_API void kan_resource_project_package_init (struct kan_resource_project_package_t *instance);

RESOURCE_PIPELINE_API void kan_resource_project_package_shutdown (struct kan_resource_project_package_t *instance);

/// \brief Deployed resources for every package must be stored in "<workspace>/deploy/<package_name>".
#define KAN_RESOURCE_PROJECT_WORKSPACE_DEPLOY_DIRECTORY "deploy"

/// \brief Cached resources for every package must be stored in "<workspace>/cache/<package_name>".
#define KAN_RESOURCE_PROJECT_WORKSPACE_CACHE_DIRECTORY "cache"

/// \brief Temporary data for every package must be stored in "<workspace>/temporary/<package_name>".
#define KAN_RESOURCE_PROJECT_WORKSPACE_TEMPORARY_DIRECTORY "temporary"

/// \brief Defines project format for application framework tools.
struct kan_resource_project_t
{
    /// \brief List of resource packages to built.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_project_package_t)
    struct kan_dynamic_array_t packages;

    /// \brief Path to directory to be used as workspace.
    char *workspace_directory;

    /// \brief Path to directory with platform configuration for this resource project.
    char *platform_configuration_directory;

    /// \brief List of enabled tags for calculating platform configuration.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (kan_interned_string_t)
    struct kan_dynamic_array_t platform_configuration_tags;

    /// \brief Name of the directory that should contain plugins to be loaded.
    /// \details Plugin directory is expected to be located in the same directory as resource tool executable.
    char *plugin_directory_name;

    /// \brief List of plugin names to be loaded.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (kan_interned_string_t)
    struct kan_dynamic_array_t plugins;
};

RESOURCE_PIPELINE_API void kan_resource_project_init (struct kan_resource_project_t *instance);

RESOURCE_PIPELINE_API void kan_resource_project_shutdown (struct kan_resource_project_t *instance);

KAN_C_HEADER_END
