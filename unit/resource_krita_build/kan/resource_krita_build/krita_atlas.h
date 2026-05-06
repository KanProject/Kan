#pragma once

#include <resource_krita_build_api.h>

#include <kan/api_common/core_types.h>
#include <kan/container/dynamic_array.h>
#include <kan/container/interned_string.h>
#include <kan/error/critical.h>
#include <kan/reflection/markup.h>
#include <kan/resource_render_foundation_build/atlas.h>

/// \file
/// \brief Contains data structure for defining texture atlases from imported Krita files.
///
/// \par Overview
/// \parblock
/// It is much more convenient to describe Krita files as sources that provide images with particular configuration
/// to the atlas. `kan_resource_krita_atlas_t` makes it possible to do just that: atlas content is described as
/// Krita files and configuration for images imported from them. Layers are automatically exported through Krita import
/// rules, so there is no need to manually specify every image that is going to be added, like it is done for the
/// regular atlases. Locale-specific replacements that were found during import are also properly added to the atlas.
/// \endparblock

KAN_C_HEADER_BEGIN

/// \brief Describes one source of images for the atlas.
/// \details All images that were accepted from given Krita files are added with the configuration specified here.
struct kan_resource_krita_atlas_source_t
{
    /// \brief Common type for all images from that source. See `kan_resource_atlas_image_t::type`.
    enum kan_resource_atlas_image_type_t type;

    union
    {
        /// \brief Common nine slice configuration for all images from that source.
        /// \warning All sizes should be given in source image coordinates!
        ///          Imported image scale factor is applied automatically on top of given coordinates.
        KAN_REFLECTION_VISIBILITY_CONDITION_FIELD (type)
        KAN_REFLECTION_VISIBILITY_CONDITION_VALUE (KAN_RESOURCE_ATLAS_IMAGE_TYPE_NINE_SLICE)
        struct kan_resource_atlas_image_nine_slice_t nine_slice;
    };

    /// \brief Common color table mmultiplier index for all images from that source.
    /// \details See `kan_resource_atlas_image_t::color_table_multiplier_index`.
    kan_instance_size_t color_table_multiplier_index;

    /// \brief List of krita files that have layers to be exported as images for the atlas.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (kan_interned_string_t)
    struct kan_dynamic_array_t krita_files;

    /// \brief Only imported entries with this filter value are accepted.
    /// \details By default, filter value is `NULL` which means that
    ///          only entries without specified filter are filtered in.
    kan_interned_string_t filter;
};

RESOURCE_KRITA_BUILD_API void kan_resource_krita_atlas_source_init (struct kan_resource_krita_atlas_source_t *instance);

RESOURCE_KRITA_BUILD_API void kan_resource_krita_atlas_source_shutdown (
    struct kan_resource_krita_atlas_source_t *instance);

/// \brief Describes how to build atlas from Krita files.
struct kan_resource_krita_atlas_t
{
    /// \brief Fixed width to be used for pages.
    kan_instance_size_t page_width;

    /// \brief Fixed height to be used for pages.
    kan_instance_size_t page_height;

    /// \brief Size of a border line between several entries to avoid color bleeding to neighboring entries.
    kan_instance_size_t border_size;

    /// \brief Color of the border line in linear format.
    struct kan_color_linear_t border;

    /// \brief List of sources used to populate atlas.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_krita_atlas_source_t)
    struct kan_dynamic_array_t sources;
};

RESOURCE_KRITA_BUILD_API void kan_resource_krita_atlas_init (struct kan_resource_krita_atlas_t *instance);

RESOURCE_KRITA_BUILD_API void kan_resource_krita_atlas_shutdown (struct kan_resource_krita_atlas_t *instance);

KAN_C_HEADER_END
