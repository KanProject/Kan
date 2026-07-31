#pragma once

#include <universe_render_foundation_api.h>

#include <kan/api_common/c_header.h>
#include <kan/api_common/core_types.h>
#include <kan/context/render_backend_system.h>
#include <kan/resource_render_foundation/texture.h>
#include <kan/threading/atomic.h>
#include <kan/universe/universe.h>

/// \file
/// \brief Provides API for interacting with render foundation texture management implementation.
///
/// \par Definition
/// \parblock
/// Render foundation texture management automatically loads and unloads textures through resource package transaction
/// routine. When texture primary resource is loaded, `kan_render_texture_loaded_t` instance is created with appropriate
/// render image with inlined mips only. When package is unloaded, `kan_render_texture_loaded_t` is automatically
/// deleted. Streamed mips loading is managed through `kan_render_texture_quality_t` records where every user may create
/// its quality requirement record and then texture management will merge them internally.
/// \endparblock

KAN_C_HEADER_BEGIN

/// \brief Group that is used to add all render foundation texture management mutators.
#define KAN_RENDER_FOUNDATION_TEXTURE_MANAGEMENT_MUTATOR_GROUP "render_foundation_texture_management"

/// \brief Checkpoint, after which render foundation texture management mutators are executed.
#define KAN_RENDER_FOUNDATION_TEXTURE_MANAGEMENT_BEGIN_CHECKPOINT "render_foundation_texture_management_begin"

/// \brief Checkpoint, that is hit after all render foundation texture management mutators have finished execution.
#define KAN_RENDER_FOUNDATION_TEXTURE_MANAGEMENT_END_CHECKPOINT "render_foundation_texture_management_end"

KAN_TYPED_ID_32_DEFINE (kan_render_texture_quality_id_t);

/// \brief Used to tell texture management that streaming up to given mip is advised for the texture.
struct kan_render_texture_quality_t
{
    /// \brief This quality requirement unique id, must be generated from `kan_next_texture_quality_id`.
    kan_immutable kan_render_texture_quality_id_t quality_id;

    /// \brief Name of the texture resource.
    kan_immutable kan_interned_string_t name;

    /// \brief Advised mip value to be loaded.
    /// \details It is a soft requirement as if there is not enough memory texture management is allowed to ignore too
    ///          detailed streamed mip requirements.
    kan_immutable kan_instance_size_t best_advised_mip;
};

UNIVERSE_RENDER_FOUNDATION_API void kan_render_texture_quality_init (struct kan_render_texture_quality_t *instance);

/// \brief Singleton for texture management quality id generation.
struct kan_render_texture_singleton_t
{
    struct kan_atomic_int_t quality_id_counter;
};

UNIVERSE_RENDER_FOUNDATION_API void kan_render_texture_singleton_init (struct kan_render_texture_singleton_t *instance);

/// \brief Inline helper for generation of texture quality ids.
static inline kan_render_texture_quality_id_t kan_next_texture_quality_id (
    const struct kan_render_texture_singleton_t *texture_singleton)
{
    // Intentionally request const and de-const it to show that it is multithreading-safe function.
    return KAN_TYPED_ID_32_SET (
        kan_render_texture_quality_id_t,
        (kan_id_32_t) kan_atomic_int_add ((struct kan_atomic_int_t *) &texture_singleton->quality_id_counter, 1));
}

/// \brief Contains render image with data from texture with given name.
/// \details Image mips are not the same as texture mips. For example, if texture has 5 mips and we only need mips
///          starting from 2, image would have 3 mips and image mip 0 will be the texture mip 2 and so on.
struct kan_render_texture_loaded_t
{
    kan_immutable kan_interned_string_t name;
    kan_render_image_t image;

    /// \brief Names of the streamed mip resources, used internally.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (kan_interned_string_t)
    struct kan_dynamic_array_t streamed_mips;

    kan_instance_size_t width;
    kan_instance_size_t height;
    kan_instance_size_t depth;
    enum kan_resource_texture_format_t format;
    kan_instance_size_t inlined_mips_count;

    kan_instance_size_t current_best_mip;
    kan_instance_size_t requested_best_mip;
    kan_instance_size_t requested_best_mip_frame_id;
    kan_instance_size_t streaming_frame_id;
};

UNIVERSE_RENDER_FOUNDATION_API void kan_render_texture_loaded_init (struct kan_render_texture_loaded_t *instance);

UNIVERSE_RENDER_FOUNDATION_API void kan_render_texture_loaded_shutdown (struct kan_render_texture_loaded_t *instance);

/// \brief Event that is being sent when `kan_render_texture_loaded_t` is inserted or its image is updated.
struct kan_render_texture_updated_event_t
{
    kan_interned_string_t name;
};

KAN_C_HEADER_END
