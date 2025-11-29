#pragma once

#include <universe_render_foundation_api.h>

#include <kan/api_common/c_header.h>
#include <kan/api_common/core_types.h>
#include <kan/context/render_backend_system.h>
#include <kan/threading/atomic.h>
#include <kan/universe/universe.h>

// TODO: Docs.

KAN_C_HEADER_BEGIN

/// \brief Group that is used to add all render foundation atlas management mutators.
#define KAN_RENDER_FOUNDATION_ATLAS_MANAGEMENT_MUTATOR_GROUP "render_foundation_atlas_management"

/// \brief Checkpoint, after which render foundation atlas management mutators are executed.
#define KAN_RENDER_FOUNDATION_ATLAS_MANAGEMENT_BEGIN_CHECKPOINT "render_foundation_atlas_management_begin"

/// \brief Checkpoint, that is hit after all render foundation atlas management mutators have finished execution.
#define KAN_RENDER_FOUNDATION_ATLAS_MANAGEMENT_END_CHECKPOINT "render_foundation_atlas_management_end"

KAN_TYPED_ID_32_DEFINE (kan_render_atlas_usage_id_t);

struct kan_render_atlas_usage_t
{
    /// \brief This usage unique id, must be generated from `kan_next_atlas_usage_id`.
    kan_render_atlas_usage_id_t usage_id;

    /// \brief Name of the atlas asset to be loaded.
    kan_interned_string_t name;
};

/// \brief Singleton for atlas management, primary used to assign atlas usage ids.
struct kan_render_atlas_singleton_t
{
    struct kan_atomic_int_t usage_id_counter;

    /// \brief Count of atlases that are currently being loaded.
    kan_instance_size_t loading_counter;
};

UNIVERSE_RENDER_FOUNDATION_API void kan_render_atlas_singleton_init (struct kan_render_atlas_singleton_t *instance);

/// \brief Inline helper for generation of atlas usage ids.
static inline kan_render_atlas_usage_id_t kan_next_atlas_usage_id (
    const struct kan_render_atlas_singleton_t *atlas_singleton)
{
    // Intentionally request const and de-const it to show that it is multithreading-safe function.
    return KAN_TYPED_ID_32_SET (
        kan_render_atlas_usage_id_t,
        (kan_id_32_t) kan_atomic_int_add ((struct kan_atomic_int_t *) &atlas_singleton->usage_id_counter, 1));
}

struct kan_atlas_loaded_entry_mapping_t
{
    kan_interned_string_t entry_name;
    kan_instance_size_t match_start;
};

struct kan_atlas_loaded_t
{
    kan_interned_string_t name;
    kan_render_image_t image;
    kan_render_buffer_t entry_buffer;

    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_atlas_loaded_entry_mapping_t)
    struct kan_dynamic_array_t mapping;

    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (kan_interned_string_t)
    struct kan_dynamic_array_t locale_requirements;
};

UNIVERSE_RENDER_FOUNDATION_API void kan_atlas_loaded_init (struct kan_atlas_loaded_t *instance);

/// \details Returns maximum value of kan_instance_size_t when query has failed.
UNIVERSE_RENDER_FOUNDATION_API kan_instance_size_t kan_atlas_loaded_query (struct kan_atlas_loaded_t *instance,
                                                                           kan_interned_string_t entry_name,
                                                                           kan_interned_string_t locale_name);

UNIVERSE_RENDER_FOUNDATION_API void kan_atlas_loaded_shutdown (struct kan_atlas_loaded_t *instance);

/// \brief Event that is being sent when `kan_render_atlas_loaded_t` is inserted or its contents are updated.
struct kan_render_atlas_updated_event_t
{
    kan_interned_string_t name;
};

KAN_C_HEADER_END
