#pragma once

#include <resource_render_foundation_build_api.h>

#include <kan/api_common/core_types.h>
#include <kan/container/dynamic_array.h>
#include <kan/container/interned_string.h>
#include <kan/error/critical.h>
#include <kan/reflection/markup.h>
#include <kan/resource_render_foundation/atlas.h>

/// \file
/// \brief This file stores data structures for defining texture atlases to be built for runtime usage.
///
/// \par Overview
/// \parblock
/// Texture atlas should be defined through `kan_resource_atlas_header_t` resource that lists all atlas entries and
/// their replacements as well as general configuration. Instances of `kan_resource_atlas_image_header_t` struct should
/// reference images in third party formats (preferably png) to be packed into atlas. If several instances specify the
/// same image, then this image would be packed into atlas only once, but several entries with their own configuration
/// would reference that image position on atlas.
///
/// Currently, we use simple sorted ladder-pin-like algorithm, but we might switch to something else later.
/// \endparblock

KAN_C_HEADER_BEGIN

/// \brief Describes image that should be packed to atlas along with its configuration.
struct kan_resource_atlas_image_header_t
{
    /// \brief Name of the original image file in third party format, preferably png.
    /// \details If several entries on atlas reference the same file, it will not be
    ///          duplicated and will only be packed once.
    kan_interned_string_t source;

    /// \brief See `kan_resource_atlas_image_t::type`.
    enum kan_resource_atlas_image_type_t type;

    union
    {
        KAN_REFLECTION_VISIBILITY_CONDITION_FIELD (type)
        KAN_REFLECTION_VISIBILITY_CONDITION_VALUE (KAN_RESOURCE_ATLAS_IMAGE_TYPE_NINE_SLICE)
        struct kan_resource_atlas_image_nine_slice_t nine_slice;
    };

    /// \brief See `kan_resource_atlas_image_t::color_table_multiplier_index`.
    kan_instance_size_t color_table_multiplier_index;
};

RESOURCE_RENDER_FOUNDATION_BUILD_API void kan_resource_atlas_image_header_init (
    struct kan_resource_atlas_image_header_t *instance);

/// \brief Describes how to build atlas entry replacement for particular locale.
struct kan_resource_atlas_entry_replacement_header_t
{
    kan_interned_string_t for_locale;
    struct kan_resource_atlas_image_header_t image;
};

RESOURCE_RENDER_FOUNDATION_BUILD_API void kan_resource_atlas_entry_replacement_header_init (
    struct kan_resource_atlas_entry_replacement_header_t *instance);

/// \brief Describes how to build atlas primary image entry.
struct kan_resource_atlas_entry_header_t
{
    kan_interned_string_t name;
    struct kan_resource_atlas_image_header_t image;

    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_atlas_entry_replacement_header_t)
    struct kan_dynamic_array_t replacements;
};

RESOURCE_RENDER_FOUNDATION_BUILD_API void kan_resource_atlas_entry_header_init (
    struct kan_resource_atlas_entry_header_t *instance);

RESOURCE_RENDER_FOUNDATION_BUILD_API void kan_resource_atlas_entry_header_shutdown (
    struct kan_resource_atlas_entry_header_t *instance);

/// \brief Describes how to build texture atlas resource.
struct kan_resource_atlas_header_t
{
    /// \brief Fixed width to be used for pages.
    kan_instance_size_t page_width;

    /// \brief Fixed height to be used for pages.
    kan_instance_size_t page_height;

    /// \brief Size of a border line between several entries to avoid color bleeding to neighboring entries.
    kan_instance_size_t border_size;

    /// \brief Color of the border line in linear format.
    struct kan_color_linear_t border;

    /// \brief List of entries to pack into the atlas.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_atlas_entry_header_t)
    struct kan_dynamic_array_t entries;
};

RESOURCE_RENDER_FOUNDATION_BUILD_API void kan_resource_atlas_header_init (struct kan_resource_atlas_header_t *instance);

RESOURCE_RENDER_FOUNDATION_BUILD_API void kan_resource_atlas_header_shutdown (
    struct kan_resource_atlas_header_t *instance);

KAN_C_HEADER_END
