#pragma once

#include <universe_resource_provider_api.h>

#include <kan/api_common/alignment.h>
#include <kan/api_common/c_header.h>
#include <kan/api_common/core_types.h>
#include <kan/container/interned_string.h>
#include <kan/hash/hash.h>
#include <kan/memory_profiler/allocation_group.h>
#include <kan/reflection/markup.h>
#include <kan/resource_pipeline/project.h>
#include <kan/serialization/binary.h>
#include <kan/threading/atomic.h>
#include <kan/universe/macro.h>

/// \file
/// \brief Provides public API for resource provider extension unit for universe unit.
///
/// \par Definition
/// \parblock
/// Resource provider is designed as a common solution for working with resources that are built using resource pipeline
/// build tool. It manages packages and automatically loads and unloads resources from them using resource transaction
/// mechanism. It also supports loading and unloading streamed resources through streaming requests. When hot reload is
/// enabled, it also detects resource changes and reloads changed resources in separate resource transaction if
/// necessary.
/// \endparblock
///
/// \par Configuration
/// \parblock
/// World-level configuration of type `kan_resource_provider_configuration_t` and with name
/// `KAN_RESOURCE_PROVIDER_CONFIGURATION` is used to provide high-level global configuration for resource provider.
/// \endparblock
///
/// \par Packages
/// \parblock
/// Package system for resources is introduced to simplify management of what should and shouldn't be loaded. Explicitly
/// specifying every resource to load looks like more agile approach at first, but it doesn't scale well enough and
/// makes resource management code too nuanced. That is why packages were introduced in order to make loading logic
/// easier to use by organizing stuff into groups called packages and loading them all at once in a transaction.
///
/// Package is a list of resources that should be loaded at once, which means that code that sets up and/or uses these
/// resources can assert that everything is loaded once loading transaction is finished, so there is no need for
/// additional checking and requesting logic as package integrity is validated on build system level. Package loading
/// rules are described in `kan_resource_package_level_t` and loading transactions are described in
/// "Loading transaction" paragraph below.
/// \endparblock
///
/// \par Entries
/// \parblock
/// After the initial scan, for every visible resource, including third party ones, a `kan_resource_registered_entry_t`
/// record is created. This records can be used by user logic to inspect the list of available resources, however it
/// should not be needed for the most cases as package loading rules should cover most loading scenarios. It also
/// contains stuff needed for internal logic like streaming counter (more on streaming below).
///
/// When native entry is loaded, special loaded entry record is created for it. Every resource type has its own record
/// type generated for it, but in general loaded entries follow the `kan_resource_loaded_entry_view_t` layout and macros
/// like `KAN_UMI_RESOURCE_RETRIEVE_LOADED` and `KAN_UMI_RESOURCE_RETRIEVE_FRESH_LOADED` are provided so the user will
/// not be bothered with excessive details.
///
/// When third party resource is loaded into memory as a blob, `kan_resource_loaded_third_party_entry_t` record with
/// its data is created. As third party resources are untyped, it is always the same record type unlike native ones.
/// \endparblock
///
/// \par Loading transaction
/// \parblock
/// All the non-streamed resources from packages are loaded in transactions. First transaction is essential package
/// loading transaction, second transaction is required package loading transaction and then transactions occur whenever
/// optional packages will be loaded or unloaded due to trigger tag changes.
///
/// Overall transaction state machine looks like that:
/// - Resource provider updates `loaded` flag on `kan_resource_package_state_t` records and schedules loading
///   operations. If it is a hot reload, then operations load the data into separate loading memory without changing
///   the already loaded one.
/// - Transaction state is in `KAN_RESOURCE_TRANSACTION_STATE_LOADING` until all loading operations are finished.
/// - Once all loading operations are done, state changes to `KAN_RESOURCE_TRANSACTION_STATE_COMMIT` and
///   `kan_resource_transaction_commit_started_event_t` is fired.
/// - Post-loading logic mutators should do their work on loaded resources if needed. If one frame is not enough for
///   that work then `kan_resource_transaction_commit_lock_t` record should be created to postpone commit finish until
///   that record removal.
/// - Once commit is finished, all resources marked for unload are unloaded, including resource of types with
///   `KAN_RESOURCE_TYPE_TRANSITIVELY_LOADED` flag, and state is changed to the default
///   `KAN_RESOURCE_TRANSACTION_STATE_NONE` values.
///
/// This simple approach makes post-loading logic for complex resources like materials and materials instance much
/// simpler by ensuring that everything in the package is loaded by the time of post-loading logic execution. However,
/// it is important to use `KAN_UMI_RESOURCE_RETRIEVE_FRESH_LOADED` instead of `KAN_UMI_RESOURCE_RETRIEVE_LOADED`
/// and `KAN_RESOURCE_PROVIDER_LOADED_ENTRY_GET_FRESH` instead of `KAN_RESOURCE_PROVIDER_LOADED_ENTRY_GET` in
/// post-loading logic in order to retrieve just-loaded-and-not-commited yet resource data.
/// \endparblock
///
/// \par Streaming
/// \parblock
/// Streamed resources are not loaded through transactions as they have to be managed on per resource basis. That is
/// why `kan_resource_streaming_request_t` records are used for them. When some logic wants to load streamed resource,
/// it should create such request to inform resource provider. When streamed resource has no associated requests, it
/// will automatically be unloaded.
///
/// Keep in mind that streamed resource loading and unloading is only done when there is no active package loading
/// transaction, which means that streaming will not happen right away if it was requested from some post-loading logic.
/// Furthermore, it is advised to properly separated resources that use streaming into required-parts and optional-parts
/// during resource build: for example keeping always-loaded mips in main texture resource and only separating optional
/// best quality mips into the separate streamed resources.
/// \endparblock
///
/// \par Events
/// \parblock
/// For every resource type, registered event type is created with name that follows
/// `KAN_RESOURCE_PROVIDER_REGISTERED_EVENT_TYPE_FORMAT` and content that follows
/// `kan_resource_registered_event_view_t`. These events are fired when new resource entry is created during scan or
/// when hot reload is detected. Separate event type is created for every resource type as in most cases users only need
/// this events for very specific resource types. Macro `KAN_UML_RESOURCE_REGISTERED_EVENT_FETCH` is advised for
/// fetching these events. Third party resources have `kan_resource_third_party_registered_event_t` with the same
/// behavior.
///
/// For every resource type, updated event type is created with name that follows
/// `KAN_RESOURCE_PROVIDER_UPDATED_EVENT_TYPE_FORMAT` and content that follows `kan_resource_updated_event_view_t`.
/// These events are fired when resource change due to hot reload is detected. However, it only informs that change was
/// detected and it is not connected to the actual loading status of the resources. Separate event type is created for
/// every resource type as users might need to use type-based ordering while processing these events. Macro
/// `KAN_UML_RESOURCE_UPDATED_EVENT_FETCH` is advised for fetching these events. Third party resources have
/// `kan_resource_third_party_updated_event_t` with the same behavior.
///
/// For every resource type, loaded event type is created with name that follows
/// `KAN_RESOURCE_PROVIDER_LOADED_EVENT_TYPE_FORMAT` and content that follows `kan_resource_loaded_event_view_t`.
/// These events are fired when resource entry is loaded either from package transaction or from streaming or from hot
/// reload. Separate event type is created for every resource type as users might need to use type-based ordering while
/// processing these events. Macro `KAN_UML_RESOURCE_LOADED_EVENT_FETCH` is advised for fetching these events. Third
/// party resources have `kan_resource_third_party_loaded_event_t` with the same behavior.
///
/// For every non-streamed resource type, unload planned event type is created with name that follows
/// `KAN_RESOURCE_PROVIDER_UNLOAD_PLANNED_EVENT_TYPE_FORMAT` and content that follows
/// `kan_resource_unload_planned_event_view_t`. These events are fired prior to the transaction commit stage to inform
/// the user which resources will be unloaded at the end of transaction, including transitively loaded resources.
/// Separate event type is created for every resource type as users might need to use type-based ordering while
/// processing these events. Macro `KAN_UML_RESOURCE_UNLOAD_PLANNED_EVENT_FETCH` is advised for fetching these events.
/// Third party resources have `kan_resource_third_party_unload_planned_event_t` with the same behavior.
///
/// For every resource type, unregistered event type is created with name that follows
/// `KAN_RESOURCE_PROVIDER_UNREGISTERED_EVENT_TYPE_FORMAT` and content that follows
/// `kan_resource_unregistered_event_view_t`. These events are fired when resource entry was deleted as hot reload has
/// discovered that file no longer exists. Separate event type is created for every resource type as in most cases users
/// only need this events for very specific resource types. Macro `KAN_UML_RESOURCE_UNREGISTERED_EVENT_FETCH` is advised
/// for fetching these events. Third party resources have `kan_resource_third_party_unregistered_event_t` with the same
/// behavior.
/// \endparblock
///
/// \par Hot reload
/// \parblock
/// When `kan_hot_reload_coordination_system_is_possible` is `true` and hot reload coordination system is present,
/// hot reload will be enabled and resource watcher will be configured to watch mounted resources for changes. Changes
/// to package manifest files will be ignored as changing package settings in runtime is not supported.
///
/// When non-streamed resource changes are detected, new package loading transaction is started, but only changed
/// resources are reloaded. When streamed resource changes are detected, streamed resources will be reloaded inside
/// regular streaming pipeline.
/// \endparblock

KAN_C_HEADER_BEGIN

/// \brief Group, used to add all the mutators that implement resource provider extension.
#define KAN_RESOURCE_PROVIDER_MUTATOR_GROUP "resource_provider"

/// \brief Name for resource provider configuration object in universe world.
#define KAN_RESOURCE_PROVIDER_CONFIGURATION "resource_provider"

/// \brief Checkpoint, after which resource provider mutators are executed.
#define KAN_RESOURCE_PROVIDER_BEGIN_CHECKPOINT "resource_provider_begin"

/// \brief Checkpoint, that is hit after all resource provider mutators finished execution.
#define KAN_RESOURCE_PROVIDER_END_CHECKPOINT "resource_provider_end"

/// \brief Convenience macro for making resource loaded entry types from their resource types.
#define KAN_RESOURCE_PROVIDER_MAKE_LOADED_ENTRY_TYPE(RESOURCE_TYPE) resource_provider_loaded_entry_##RESOURCE_TYPE

/// \brief Macro that provides formatting string used to create resource provider loaded entry type names.
#define KAN_RESOURCE_PROVIDER_LOADED_ENTRY_TYPE_FORMAT "resource_provider_loaded_entry_%s"

/// \brief Loaded third party data is always aligned by this value.
#define KAN_RESOURCE_PROVIDER_LOADED_THIRD_PARTY_ALIGNMENT 16u

/// \brief Convenience macro for making resource typed registered event types from their resource types.
#define KAN_RESOURCE_PROVIDER_MAKE_REGISTERED_EVENT_TYPE(RESOURCE_TYPE)                                                \
    resource_provider_registered_event_##RESOURCE_TYPE

/// \brief Macro that provides formatting string used to create resource provider typed registered event type names.
#define KAN_RESOURCE_PROVIDER_REGISTERED_EVENT_TYPE_FORMAT "resource_provider_registered_event_%s"

/// \brief Convenience macro for making resource typed updated event types from their resource types.
#define KAN_RESOURCE_PROVIDER_MAKE_UPDATED_EVENT_TYPE(RESOURCE_TYPE) resource_provider_updated_event_##RESOURCE_TYPE

/// \brief Macro that provides formatting string used to create resource provider typed updated event type names.
#define KAN_RESOURCE_PROVIDER_UPDATED_EVENT_TYPE_FORMAT "resource_provider_updated_event_%s"

/// \brief Convenience macro for making resource typed loaded event types from their resource types.
#define KAN_RESOURCE_PROVIDER_MAKE_LOADED_EVENT_TYPE(RESOURCE_TYPE) resource_provider_loaded_event_##RESOURCE_TYPE

/// \brief Macro that provides formatting string used to create resource provider typed loaded event type names.
#define KAN_RESOURCE_PROVIDER_LOADED_EVENT_TYPE_FORMAT "resource_provider_loaded_event_%s"

/// \brief Convenience macro for making resource typed unload planned event types from their resource types.
#define KAN_RESOURCE_PROVIDER_MAKE_UNLOAD_PLANNED_EVENT_TYPE(RESOURCE_TYPE)                                            \
    resource_provider_unload_planned_event_##RESOURCE_TYPE

/// \brief Macro that provides formatting string used to create resource provider typed unload planned event type names.
#define KAN_RESOURCE_PROVIDER_UNLOAD_PLANNED_EVENT_TYPE_FORMAT "resource_provider_unload_planned_event_%s"

/// \brief Convenience macro for making resource typed unregistered event types from their resource types.
#define KAN_RESOURCE_PROVIDER_MAKE_UNREGISTERED_EVENT_TYPE(RESOURCE_TYPE)                                              \
    resource_provider_unregistered_event_##RESOURCE_TYPE

/// \brief Macro that provides formatting string used to create resource provider typed unregistered event type names.
#define KAN_RESOURCE_PROVIDER_UNREGISTERED_EVENT_TYPE_FORMAT "resource_provider_unregistered_event_%s"

KAN_TYPED_ID_32_DEFINE (kan_resource_entry_id_t);
KAN_TYPED_ID_32_DEFINE (kan_resource_streaming_id_t);

/// \brief Structure that contains configuration for resource provider.
struct kan_resource_provider_configuration_t
{
    /// \brief How much time in nanoseconds should be spent loading resources during package transaction.
    kan_stable_size_t transaction_budget_ns;

    /// \brief How much time in nanoseconds should be spent loading streamed resources during update.
    kan_stable_size_t streaming_budget_ns;

    /// \brief Path to virtual directory with resources, that is used as resource root directory.
    kan_interned_string_t resource_directory_path;
};

UNIVERSE_RESOURCE_PROVIDER_API void kan_resource_provider_configuration_init (
    struct kan_resource_provider_configuration_t *instance);

/// \brief Enumerates states of package loading transaction for resource provider.
enum kan_resource_transaction_state_t
{
    /// \brief No transaction is currently going on.
    KAN_RESOURCE_TRANSACTION_STATE_NONE = 0u,

    /// \brief Non-streamed resources from loaded packages are being loaded into memory.
    KAN_RESOURCE_TRANSACTION_STATE_LOADING,

    /// \brief Post-loading user logic is being executed on freshly loaded non-streamed resources.
    KAN_RESOURCE_TRANSACTION_STATE_COMMIT,
};

/// \brief Contains resource provider publicly accessible state.
struct kan_resource_provider_singleton_t
{
    /// \brief Atomic counter for assigning streaming request ids. Safe to be modified from different threads.
    struct kan_atomic_int_t streaming_id_counter;

    /// \brief Whether initial resource scan was done.
    bool initial_scan_done;

    /// \brief Whether essential package loading was finished.
    bool essential_loading_done;

    /// \brief Whether required package loading was finished.
    bool required_loading_done;

    /// \brief Current package loading transaction state.
    enum kan_resource_transaction_state_t transaction_state;

    /// \brief Frame id that can be used by resources to avoid recalculating the
    ///        same values several times during one frame.
    /// \invariant Guaranteed to be different every frame. Guaranteed to have the same value for the whole frame.
    /// \details There is a significant class of event-triggered merge-based logic routines, for example merging
    ///          priorities from all the usages when new usage is inserted. This logic needs to be done only once
    ///          for its logical group per frame, for example when we merge priorities for the resource we will always
    ///          get the same result if we do it several times during the same frame, therefore we would like to avoid
    ///          calculating it several times. But we might get several usage insertion events that can trigger
    ///          recalculation. This id is designed for that use case: its value can be saved into special field and
    ///          recalculation could be avoid if saved value is equal to the value in singleton.
    kan_instance_size_t logic_deduplication_frame_id;

    /// \brief Whether ::tags were modified and modification was not yet applied.
    /// \details `kan_resource_provider_singleton_add_tag` and `kan_resource_provider_singleton_remove_tag`
    ///          set this flag automatically.
    bool tags_dirty;

    /// \brief List of currently enabled tags for optional resource package loading.
    /// \details Should be modified using `kan_resource_provider_singleton_add_tag` and
    ///          `kan_resource_provider_singleton_remove_tag` as this functions manages ::tags_dirty flag automatically.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (kan_interned_string_t)
    struct kan_dynamic_array_t tags;
};

UNIVERSE_RESOURCE_PROVIDER_API void kan_resource_provider_singleton_init (
    struct kan_resource_provider_singleton_t *instance);

/// \brief Adds new trigger tag to tags array unless it is already there.
UNIVERSE_RESOURCE_PROVIDER_API void kan_resource_provider_singleton_add_tag (
    struct kan_resource_provider_singleton_t *instance, kan_interned_string_t tag);

/// \brief Removes trigger tag from tags array if it is found there.
UNIVERSE_RESOURCE_PROVIDER_API void kan_resource_provider_singleton_remove_tag (
    struct kan_resource_provider_singleton_t *instance, kan_interned_string_t tag);

UNIVERSE_RESOURCE_PROVIDER_API void kan_resource_provider_singleton_shutdown (
    struct kan_resource_provider_singleton_t *instance);

/// \brief Inline helper for generation of resource streaming ids.
static inline kan_resource_streaming_id_t kan_next_resource_streaming_id (
    const struct kan_resource_provider_singleton_t *resource_provider)
{
    // Intentionally uses const and de-const it to show that it is multithreading-safe function.
    return KAN_TYPED_ID_32_SET (
        kan_resource_streaming_id_t,
        (kan_id_32_t) kan_atomic_int_add ((struct kan_atomic_int_t *) &resource_provider->streaming_id_counter, 1));
}

/// \brief Event that is sent when resource provider transaction goes into commit state from loading state.
struct kan_resource_transaction_commit_started_event_t
{
    kan_instance_size_t stub;
};

/// \brief Used by resource commit mutators to put lock unto transaction until multi-frame commit is done.
struct kan_resource_transaction_commit_lock_t
{
    /// \brief Needed only for the creating mutators so they could identify their locks.
    kan_interned_string_t owner_name;
};

/// \brief Event that is sent when resource provider transaction commit is done and transaction is finished.
struct kan_resource_transaction_commit_finished_event_t
{
    kan_instance_size_t stub;
};

/// \brief Event that is sent essential packages loading transaction is finished.
struct kan_resource_essentials_loaded_event_t
{
    kan_instance_size_t stub;
};

/// \brief Contains information about particular resource package state.
struct kan_resource_package_state_t
{
    kan_interned_string_t name;
    bool loaded;
    kan_serialization_interned_string_registry_t string_registry;
    struct kan_resource_package_t manifest;
};

UNIVERSE_RESOURCE_PROVIDER_API void kan_resource_package_state_init (struct kan_resource_package_state_t *instance);

UNIVERSE_RESOURCE_PROVIDER_API void kan_resource_package_state_shutdown (struct kan_resource_package_state_t *instance);

/// \brief Contains common information about resource entry including third party ones.
struct kan_resource_registered_entry_t
{
    kan_immutable kan_resource_entry_id_t entry_id;
    kan_immutable kan_interned_string_t package;
    kan_immutable kan_interned_string_t type;
    kan_immutable kan_interned_string_t name;

    /// \brief Count of streaming requests if resource type is streamed.
    /// \details Left at zero for non-streaming resources.
    kan_instance_size_t streaming_counter;

    kan_hash_t path_hash;
    char *path;
    kan_allocation_group_t my_allocation_group;
};

UNIVERSE_RESOURCE_PROVIDER_API void kan_resource_registered_entry_init (
    struct kan_resource_registered_entry_t *instance);

UNIVERSE_RESOURCE_PROVIDER_API void kan_resource_registered_entry_shutdown (
    struct kan_resource_registered_entry_t *instance);

/// \brief Describes information that is stored in special typed entry for loaded native resources.
/// \details Making loaded entries typed also makes it easy to search for the by resource name.
KAN_REFLECTION_IGNORE
struct kan_resource_loaded_entry_view_t
{
    kan_immutable kan_resource_entry_id_t entry_id;
    kan_immutable kan_interned_string_t name;
    kan_immutable kan_allocation_group_t my_allocation_group;

    /// \brief Allocated memory for the loading data buffer.
    /// \invariant Should never be accessed outside of commit stage for non-streamed resources.
    void *loading_data;

    /// \brief True if loaded data is ready to be accessed.
    /// \details Loaded entry is created when loading is started, therefore it would sit with default-constructed data
    ///          for some time until loading data is flipped into it after the transaction end.
    bool data_ready;

    uint8_t data_begin[];
};

/// \brief Helper macro for extracting loaded data pointer from typed loaded entry.
#define KAN_RESOURCE_PROVIDER_LOADED_ENTRY_GET(TYPE_NAME, ENTRY)                                                       \
    (((struct kan_resource_loaded_entry_view_t *) ENTRY)->data_ready ?                                                 \
         ((const struct TYPE_NAME *) kan_apply_alignment (                                                             \
             (kan_memory_size_t) ((struct kan_resource_loaded_entry_view_t *) ENTRY)->data_begin,                      \
             alignof (struct TYPE_NAME))) :                                                                            \
         NULL)

/// \brief Helper macro for extracting either loaded data or loading data during commit phase of transaction.
/// \details Should only be used by resource commit systems to access loading data to be commited before flip happens!
#define KAN_RESOURCE_PROVIDER_LOADED_ENTRY_GET_FRESH(TYPE_NAME, ENTRY)                                                 \
    (((const struct TYPE_NAME *) ((struct kan_resource_loaded_entry_view_t *) ENTRY)->loading_data) ?                  \
         ((const struct TYPE_NAME *) ((struct kan_resource_loaded_entry_view_t *) ENTRY)->loading_data) :              \
         KAN_RESOURCE_PROVIDER_LOADED_ENTRY_GET (TYPE_NAME, ENTRY))

/// \brief Stores data of loaded third party entry.
struct kan_resource_loaded_third_party_entry_t
{
    kan_immutable kan_resource_entry_id_t entry_id;
    kan_immutable kan_interned_string_t name;

    /// \brief Pointer to the loaded resource data.
    void *loaded_data;

    /// \brief Loading data pointer that is only not NULL during transaction commit.
    void *loading_data;

    kan_instance_size_t loaded_data_size;
    kan_instance_size_t loading_data_size;
    kan_allocation_group_t my_allocation_group;
};

UNIVERSE_RESOURCE_PROVIDER_API void kan_resource_loaded_third_party_entry_init (
    struct kan_resource_loaded_third_party_entry_t *instance);

UNIVERSE_RESOURCE_PROVIDER_API void kan_resource_loaded_third_party_entry_shutdown (
    struct kan_resource_loaded_third_party_entry_t *instance);

/// \brief Record that informs resource provider that streamed resource should be loaded.
struct kan_resource_streaming_request_t
{
    kan_immutable kan_resource_streaming_id_t request_id;
    kan_immutable kan_interned_string_t type;
    kan_immutable kan_interned_string_t name;
    kan_immutable kan_instance_size_t priority;
};

UNIVERSE_RESOURCE_PROVIDER_API void kan_resource_streaming_request_init (
    struct kan_resource_streaming_request_t *instance);

/// \brief Describes layout of typed event that is fired when new resource is registered,
///        including initial resource registration.
KAN_REFLECTION_IGNORE
struct kan_resource_registered_event_view_t
{
    kan_resource_entry_id_t entry_id;
    kan_interned_string_t name;
};

/// \brief Describes layout of typed event that is fired when resource file update was detected, for example due to new
///        resource build tool execution.
/// \warning Does not mean that resource state loaded in memory is changed!
/// \details Hot reload coordination protocol guarantees that changes from hot reload rebuild action will be seen all
///          at once in the same frame, therefore all events of this type will be sent during one frame. This guarantee
///          should simplify top level user code as it should make it much easier to process updates of interconnected
///          top level resources when some of their files have changed.
KAN_REFLECTION_IGNORE
struct kan_resource_updated_event_view_t
{
    kan_resource_entry_id_t entry_id;
    kan_interned_string_t name;
};

/// \brief Describes layout of typed event that is fired when resource is fully loaded in memory,
///        both for transaction-loaded resources and for streamed ones.
KAN_REFLECTION_IGNORE
struct kan_resource_loaded_event_view_t
{
    kan_resource_entry_id_t entry_id;
    kan_interned_string_t name;
};

/// \brief Describes layout of typed event that is fired when non-streamed resource is planned to be unloaded after
///        transaction commit happens.
KAN_REFLECTION_IGNORE
struct kan_resource_unload_planned_event_view_t
{
    kan_resource_entry_id_t entry_id;
    kan_interned_string_t name;
};

/// \brief Describes layout of typed event that is fired resource was unregistered by hot reload routine.
KAN_REFLECTION_IGNORE
struct kan_resource_unregistered_event_view_t
{
    kan_resource_entry_id_t entry_id;
    kan_interned_string_t name;
};

/// \brief Event that is sent when third party resource is registered, like `kan_resource_registered_event_view_t`.
struct kan_resource_third_party_registered_event_t
{
    kan_interned_string_t name;
};

/// \brief Event that is sent when third party resource file is updated, like `kan_resource_updated_event_view_t`.
struct kan_resource_third_party_updated_event_t
{
    kan_interned_string_t name;
};

/// \brief Event that is sent when third party resource is loaded, like `kan_resource_loaded_event_view_t`.
struct kan_resource_third_party_loaded_event_t
{
    kan_interned_string_t name;
};

/// \brief Event that is sent when third party resource will be unloaded at the end of this transaction commit,
///        like `kan_resource_unload_planned_event_view_t`.
struct kan_resource_third_party_unload_planned_event_t
{
    kan_interned_string_t name;
};

/// \brief Event that is sent when third party resource is registered, like `kan_resource_unregistered_event_view_t`.
struct kan_resource_third_party_unregistered_event_t
{
    kan_interned_string_t name;
};

#if defined(CMAKE_UNIT_FRAMEWORK_HIGHLIGHT)
#    define KAN_UMI_RESOURCE_RETRIEVE_LOADED(NAME, RESOURCE_TYPE, RESOURCE_NAME_POINTER)                               \
        /* Highlight-autocomplete replacement. */                                                                      \
        const struct RESOURCE_TYPE *NAME = NULL;                                                                       \
        /* Add this useless pointer so IDE highlight would never consider argument unused. */                          \
        const void *argument_pointer_for_highlight_##NAME = RESOURCE_NAME_POINTER;
#else
#    define KAN_UMI_RESOURCE_RETRIEVE_LOADED(NAME, RESOURCE_TYPE, RESOURCE_NAME_POINTER)                               \
        KAN_UM_INTERNAL_VALUE_OPTIONAL (resource_provider_loaded_entry_##NAME,                                         \
                                        KAN_RESOURCE_PROVIDER_MAKE_LOADED_ENTRY_TYPE (RESOURCE_TYPE), name,            \
                                        RESOURCE_NAME_POINTER, read, read, const)                                      \
        const struct RESOURCE_TYPE *NAME = NULL;                                                                       \
                                                                                                                       \
        if (resource_provider_loaded_entry_##NAME)                                                                     \
        {                                                                                                              \
            NAME = KAN_RESOURCE_PROVIDER_LOADED_ENTRY_GET (RESOURCE_TYPE, resource_provider_loaded_entry_##NAME);      \
        }
#endif

#if defined(CMAKE_UNIT_FRAMEWORK_HIGHLIGHT)
#    define KAN_UMI_RESOURCE_RETRIEVE_FRESH_LOADED(NAME, RESOURCE_TYPE, RESOURCE_NAME_POINTER)                         \
        /* Highlight-autocomplete replacement. */                                                                      \
        const struct RESOURCE_TYPE *NAME = NULL;                                                                       \
        /* Add this useless pointer so IDE highlight would never consider argument unused. */                          \
        const void *argument_pointer_for_highlight_##NAME = RESOURCE_NAME_POINTER;
#else
#    define KAN_UMI_RESOURCE_RETRIEVE_FRESH_LOADED(NAME, RESOURCE_TYPE, RESOURCE_NAME_POINTER)                         \
        KAN_UM_INTERNAL_VALUE_OPTIONAL (resource_provider_loaded_entry_##NAME,                                         \
                                        KAN_RESOURCE_PROVIDER_MAKE_LOADED_ENTRY_TYPE (RESOURCE_TYPE), name,            \
                                        RESOURCE_NAME_POINTER, read, read, const)                                      \
        const struct RESOURCE_TYPE *NAME = NULL;                                                                       \
                                                                                                                       \
        if (resource_provider_loaded_entry_##NAME)                                                                     \
        {                                                                                                              \
            NAME =                                                                                                     \
                KAN_RESOURCE_PROVIDER_LOADED_ENTRY_GET_FRESH (RESOURCE_TYPE, resource_provider_loaded_entry_##NAME);   \
        }
#endif

#define KAN_UMI_RESOURCE_RETRIEVE_LOADED_THIRD_PARTY(NAME, RESOURCE_NAME_POINTER)                                      \
    KAN_UMI_VALUE_READ_OPTIONAL (resource_provider_third_party_##NAME, kan_resource_loaded_third_party_entry_t, name,  \
                                 RESOURCE_NAME_POINTER)                                                                \
                                                                                                                       \
    void *NAME = NULL;                                                                                                 \
    kan_instance_size_t NAME##_size = 0u;                                                                              \
                                                                                                                       \
    if (resource_provider_third_party_##NAME)                                                                          \
    {                                                                                                                  \
        NAME = resource_provider_third_party_##NAME->loaded_data;                                                      \
        NAME##_size = resource_provider_third_party_##NAME->loaded_data_size;                                          \
    }

#define KAN_UMI_RESOURCE_RETRIEVE_FRESH_LOADED_THIRD_PARTY(NAME, RESOURCE_NAME_POINTER)                                \
    KAN_UMI_VALUE_READ_OPTIONAL (resource_provider_third_party_##NAME, kan_resource_loaded_third_party_entry_t, name,  \
                                 RESOURCE_NAME_POINTER)                                                                \
                                                                                                                       \
    void *NAME = NULL;                                                                                                 \
    kan_instance_size_t NAME##_size = 0u;                                                                              \
                                                                                                                       \
    if (resource_provider_third_party_##NAME)                                                                          \
    {                                                                                                                  \
        if (resource_provider_third_party_##NAME->loading_data)                                                        \
        {                                                                                                              \
            NAME = resource_provider_third_party_##NAME->loading_data;                                                 \
            NAME##_size = resource_provider_third_party_##NAME->loading_data_size;                                     \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            NAME = resource_provider_third_party_##NAME->loaded_data;                                                  \
            NAME##_size = resource_provider_third_party_##NAME->loaded_data_size;                                      \
        }                                                                                                              \
    }

#if defined(CMAKE_UNIT_FRAMEWORK_HIGHLIGHT)
#    define KAN_UML_RESOURCE_REGISTERED_EVENT_FETCH(NAME, RESOURCE_TYPE)                                               \
        /* Highlight-autocomplete replacement. */                                                                      \
        const struct kan_resource_registered_event_view_t *NAME = NULL;                                                \
        for (kan_memory_size_t fake_index_##NAME = 0u; fake_index_##NAME < 1u; ++fake_index_##NAME)
#else
#    define KAN_UML_RESOURCE_REGISTERED_EVENT_FETCH(NAME, RESOURCE_TYPE)                                               \
        KAN_UM_INTERNAL_EVENT_FETCH (NAME, KAN_RESOURCE_PROVIDER_MAKE_REGISTERED_EVENT_TYPE (RESOURCE_TYPE),           \
                                     kan_resource_registered_event_view_t)
#endif

#if defined(CMAKE_UNIT_FRAMEWORK_HIGHLIGHT)
#    define KAN_UML_RESOURCE_UPDATED_EVENT_FETCH(NAME, RESOURCE_TYPE)                                                  \
        /* Highlight-autocomplete replacement. */                                                                      \
        const struct kan_resource_updated_event_view_t *NAME = NULL;                                                   \
        for (kan_memory_size_t fake_index_##NAME = 0u; fake_index_##NAME < 1u; ++fake_index_##NAME)
#else
#    define KAN_UML_RESOURCE_UPDATED_EVENT_FETCH(NAME, RESOURCE_TYPE)                                                  \
        KAN_UM_INTERNAL_EVENT_FETCH (NAME, KAN_RESOURCE_PROVIDER_MAKE_UPDATED_EVENT_TYPE (RESOURCE_TYPE),              \
                                     kan_resource_updated_event_view_t)
#endif

#if defined(CMAKE_UNIT_FRAMEWORK_HIGHLIGHT)
#    define KAN_UML_RESOURCE_LOADED_EVENT_FETCH(NAME, RESOURCE_TYPE)                                                   \
        /* Highlight-autocomplete replacement. */                                                                      \
        const struct kan_resource_loaded_event_view_t *NAME = NULL;                                                    \
        for (kan_memory_size_t fake_index_##NAME = 0u; fake_index_##NAME < 1u; ++fake_index_##NAME)
#else
#    define KAN_UML_RESOURCE_LOADED_EVENT_FETCH(NAME, RESOURCE_TYPE)                                                   \
        KAN_UM_INTERNAL_EVENT_FETCH (NAME, KAN_RESOURCE_PROVIDER_MAKE_LOADED_EVENT_TYPE (RESOURCE_TYPE),               \
                                     kan_resource_loaded_event_view_t)
#endif

#if defined(CMAKE_UNIT_FRAMEWORK_HIGHLIGHT)
#    define KAN_UML_RESOURCE_UNLOAD_PLANNED_EVENT_FETCH(NAME, RESOURCE_TYPE)                                           \
        /* Highlight-autocomplete replacement. */                                                                      \
        const struct kan_resource_unload_planned_event_view_t *NAME = NULL;                                            \
        for (kan_memory_size_t fake_index_##NAME = 0u; fake_index_##NAME < 1u; ++fake_index_##NAME)
#else
#    define KAN_UML_RESOURCE_UNLOAD_PLANNED_EVENT_FETCH(NAME, RESOURCE_TYPE)                                           \
        KAN_UM_INTERNAL_EVENT_FETCH (NAME, KAN_RESOURCE_PROVIDER_MAKE_UNLOAD_PLANNED_EVENT_TYPE (RESOURCE_TYPE),       \
                                     kan_resource_unload_planned_event_view_t)
#endif

#if defined(CMAKE_UNIT_FRAMEWORK_HIGHLIGHT)
#    define KAN_UML_RESOURCE_UNREGISTERED_EVENT_FETCH(NAME, RESOURCE_TYPE)                                             \
        /* Highlight-autocomplete replacement. */                                                                      \
        const struct kan_resource_unregistered_event_view_t *NAME = NULL;                                              \
        for (kan_memory_size_t fake_index_##NAME = 0u; fake_index_##NAME < 1u; ++fake_index_##NAME)
#else
#    define KAN_UML_RESOURCE_UNREGISTERED_EVENT_FETCH(NAME, RESOURCE_TYPE)                                             \
        KAN_UM_INTERNAL_EVENT_FETCH (NAME, KAN_RESOURCE_PROVIDER_MAKE_UNREGISTERED_EVENT_TYPE (RESOURCE_TYPE),         \
                                     kan_resource_unregistered_event_view_t)
#endif

KAN_C_HEADER_END
