#pragma once

#include <resource_pipeline_api.h>

#include <kan/api_common/c_header.h>
#include <kan/api_common/core_types.h>
#include <kan/container/dynamic_array.h>
#include <kan/container/interned_string.h>
#include <kan/reflection/markup.h>
#include <kan/resource_pipeline/meta.h>

/// \file
/// \brief Contains data structures for storing resource build action log for up-to-date checks during resource build.
///
/// \par Overview
/// \parblock
/// Resource action log is needed to properly store information about built resources, so we can invalidate them
/// properly. Just checking timestamps is usually not enough: when only timestamps are used, game code becomes one
/// big timestamp on shared library that is impossible to decouple, which will result in full rebuilds when they are
/// not really necessary as most resources still have the same structure. Also, when resources are deemed up-to-date,
/// log makes it possible to follow resource references without trying to load actual resources, which makes full
/// resource hierarchy check faster.
/// \endparblock

KAN_C_HEADER_BEGIN

RESOURCE_PIPELINE_API kan_allocation_group_t kan_resource_log_get_allocation_group (void);

/// \brief Flags for detected resource reference.
/// \details We use different flags for meta and log as meta flags should provide the most obvious defaults for the
///          user and log flags should be mergeable through bitwise or in a logical way, which results in logical
///          conflicts in some cases.
KAN_REFLECTION_FLAGS
enum kan_resource_reference_flags_t
{
    /// \brief Enabled if any of the actual references does not have KAN_RESOURCE_REFERENCE_META_PLATFORM_OPTIONAL flag.
    KAN_RESOURCE_REFERENCE_REQUIRED = 1u << 0u,

    /// \brief Enabled if any of the actual references does not have
    ///        KAN_RESOURCE_REFERENCE_META_LOADING_NOT_REQUIRED flag.
    KAN_RESOURCE_REFERENCE_LOADING_REQUIRED = 1u << 1u,
};

/// \brief Describes resource reference stored inside resource build action log.
struct kan_resource_log_reference_t
{
    kan_interned_string_t type;
    kan_interned_string_t name;
    enum kan_resource_reference_flags_t flags;
};

/// \brief Contains full resource version: type version in code and file timestamp.
/// \details Third party resources always have zero type version.
struct kan_resource_log_version_t
{
    kan_resource_version_t type_version;
    kan_stable_size_t last_modification_time;
};

/// \brief Returns true if logged version is decided new enough to not cause a rebuild compared to detected version.
static inline bool kan_resource_log_version_is_up_to_date (struct kan_resource_log_version_t logged,
                                                           struct kan_resource_log_version_t detected)
{
    return logged.type_version == detected.type_version &&
           logged.last_modification_time == detected.last_modification_time;
}

/// \brief Enumerates directories where built resource that is mentioned in the log is stored.
enum kan_resource_log_saved_directory_t
{
    /// \brief Resource is saved into deploy directory.
    KAN_RESOURCE_LOG_SAVED_DIRECTORY_DEPLOY = 0u,

    /// \brief Resource is saved into cache directory.
    KAN_RESOURCE_LOG_SAVED_DIRECTORY_CACHE,

    /// \brief Special value for platform unsupported resources that we still need to record in the log file.
    KAN_RESOURCE_LOG_SAVED_DIRECTORY_UNSUPPORTED,
};

/// \brief Enumerates possible source that provided current logged version of resource.
enum kan_resource_log_entry_source_t
{
    /// \brief Retrieved from raw resources directory.
    KAN_RESOURCE_LOG_ENTRY_SOURCE_RAW = 0u,

    /// \brief Built as build rule primary output.
    KAN_RESOURCE_LOG_ENTRY_SOURCE_PRIMARY,

    /// \brief Built as build rule secondary output.
    KAN_RESOURCE_LOG_ENTRY_SOURCE_SECONDARY,
};

/// \brief Contains additional information for entry that was built as build rule primary output.
struct kan_resource_log_entry_source_primary_t
{
    /// \brief Primary input type is used to uniquely identify producer build rule.
    kan_interned_string_t primary_input_type;

    kan_stable_size_t platform_configuration_time;
    kan_resource_version_t rule_version;
    struct kan_resource_log_version_t primary_input_version;
};

/// \brief Contains additional information for entry that was built as build rule secondary output.
struct kan_resource_log_entry_source_secondary_t
{
    kan_interned_string_t producer_type;
    kan_interned_string_t producer_name;
    struct kan_resource_log_version_t producer_version;
};

/// \brief Contains information about additional built entry source dependency.
/// \details Can point to any native entry or third party entry, in which case `type` is `NULL`.
struct kan_resource_log_dependency_t
{
    kan_interned_string_t type;
    kan_interned_string_t name;
    struct kan_resource_log_version_t version;
};

/// \brief Contains full information about resource.
/// \details Both native and third party resources are logged as such entries.
///          Third party resources have `NULL` instead of `type` as they do not have any native type.
struct kan_resource_log_entry_t
{
    kan_interned_string_t package;
    kan_interned_string_t type;
    kan_interned_string_t name;
    struct kan_resource_log_version_t version;

    /// \brief List of resource references that were found in this resource.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_log_reference_t)
    struct kan_dynamic_array_t references;

    enum kan_resource_log_saved_directory_t saved_directory;
    enum kan_resource_log_entry_source_t source;

    union
    {
        KAN_REFLECTION_VISIBILITY_CONDITION_FIELD (source)
        KAN_REFLECTION_VISIBILITY_CONDITION_VALUE (KAN_RESOURCE_LOG_ENTRY_SOURCE_PRIMARY)
        struct kan_resource_log_entry_source_primary_t source_primary;

        KAN_REFLECTION_VISIBILITY_CONDITION_FIELD (source)
        KAN_REFLECTION_VISIBILITY_CONDITION_VALUE (KAN_RESOURCE_LOG_ENTRY_SOURCE_SECONDARY)
        struct kan_resource_log_entry_source_secondary_t source_secondary;
    };

    /// \brief Additional dependencies for checking whether resource is up to data.
    /// \details Normally, should only be populated with secondary inputs when `source` is
    ///          `KAN_RESOURCE_LOG_ENTRY_SOURCE_PRIMARY`. However, we cannot put this under visibility condition as
    ///          having visibility condition based initialization is not supported in serialization, therefore we cannot
    ///          init the array properly there. Moreover, supporting such behavior in serialization would result in
    ///          particularly complex logic as serialization would need to initialize parts of structure based on
    ///          visibility conditions while reading these serialized visibility conditions.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_log_dependency_t)
    struct kan_dynamic_array_t additional_dependencies;
};

RESOURCE_PIPELINE_API void kan_resource_log_entry_init (struct kan_resource_log_entry_t *instance);

RESOURCE_PIPELINE_API void kan_resource_log_entry_init_copy (struct kan_resource_log_entry_t *instance,
                                                             const struct kan_resource_log_entry_t *copy_from);

RESOURCE_PIPELINE_API void kan_resource_log_entry_shutdown (struct kan_resource_log_entry_t *instance);

/// \brief Default name for resource log file.
#define KAN_RESOURCE_LOG_DEFAULT_NAME ".resource_log"

/// \brief Resource build action log root data structure.
struct kan_resource_log_t
{
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_log_entry_t)
    struct kan_dynamic_array_t entries;
};

RESOURCE_PIPELINE_API void kan_resource_log_init (struct kan_resource_log_t *instance);

RESOURCE_PIPELINE_API void kan_resource_log_shutdown (struct kan_resource_log_t *instance);

KAN_C_HEADER_END
