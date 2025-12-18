#pragma once

#include <resource_render_foundation_api.h>

#include <kan/api_common/core_types.h>
#include <kan/container/dynamic_array.h>
#include <kan/container/interned_string.h>
#include <kan/error/critical.h>
#include <kan/reflection/markup.h>

/// \file
/// \brief This file stores runtime representation of texture atlas resource.
///
/// \par Overview
/// \parblock
/// Texture atlas is a collection of textures without mip maps that are packed into single big layered texture. This
/// approach simplifies 2d rendering, making instancing a lot easier and overall 2d render more performant. In addition,
/// per entry configuration is supported, which makes it possible to setup nine slice or color table usage independently
/// for every entry texture of the atlas. Also, locale-based entry replacement is supported as well.
/// \endparblock

KAN_C_HEADER_BEGIN

/// \brief Atlas entry image type is used to tell user pipeline how to properly sample this entry.
enum kan_resource_atlas_image_type_t
{
    /// \brief Regular image without any special sampling requirements.
    KAN_RESOURCE_ATLAS_IMAGE_TYPE_REGULAR = 0u,

    /// \brief Image should be sampled as a nine slice, either stretched or tiled one.
    KAN_RESOURCE_ATLAS_IMAGE_TYPE_NINE_SLICE,
};

/// \brief Describes how nine slice image should be sampled.
struct kan_resource_atlas_image_nine_slice_t
{
    /// \brief If true, center is tiled horizontally instead of being stretched.
    bool tiled_x;

    /// \brief If true, center is tiled vertically instead of being stretched.
    bool tiled_y;

    /// \brief Distance from left corner to the end of left static area.
    kan_instance_size_t left;

    /// \brief Distance from the right corner to the beginning of the right static area.
    kan_instance_size_t right;

    /// \brief Distance from the top corner to the end of the top static area.
    kan_instance_size_t top;

    /// \brief Distance from the bottom corner to the beginning of the bottom static area.
    kan_instance_size_t bottom;
};

/// \brief Describes one image on atlas, either primary entry or replacement.
struct kan_resource_atlas_image_t
{
    kan_instance_size_t page;
    kan_instance_size_t x;
    kan_instance_size_t y;
    kan_instance_size_t width;
    kan_instance_size_t height;

    /// \brief Type of the image that should be used for proper sampling.
    enum kan_resource_atlas_image_type_t type;

    union
    {
        KAN_REFLECTION_VISIBILITY_CONDITION_FIELD (type)
        KAN_REFLECTION_VISIBILITY_CONDITION_VALUE (KAN_RESOURCE_ATLAS_IMAGE_TYPE_NINE_SLICE)
        struct kan_resource_atlas_image_nine_slice_t nine_slice;
    };

    /// \brief If not max value of kan_instance_size_t, then image color data should be multiplied by color table entry
    ///        with that index.
    /// \details Global color tables are used for color blindness color remapping. For example, ui buff and debuff icons
    ///          can be grayscale and then multiplied by color at appropriate index: green for normal vision and blue
    ///          in colorblind mode. Also, tables make it easy to support custom color configurations for color
    ///          blindness, which is supported in some other games and applications.
    kan_instance_size_t color_table_multiplier_index;
};

/// \brief Describes atlas entry replacement for particular locale.
struct kan_resource_atlas_entry_replacement_t
{
    kan_interned_string_t for_locale;
    struct kan_resource_atlas_image_t image;
};

/// \brief Describes atlas primary image entry.
struct kan_resource_atlas_entry_t
{
    kan_interned_string_t name;
    struct kan_resource_atlas_image_t image;

    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_atlas_entry_replacement_t)
    struct kan_dynamic_array_t replacements;
};

RESOURCE_RENDER_FOUNDATION_API void kan_resource_atlas_entry_init (struct kan_resource_atlas_entry_t *instance);

RESOURCE_RENDER_FOUNDATION_API void kan_resource_atlas_entry_shutdown (struct kan_resource_atlas_entry_t *instance);

/// \brief Describes built texture atlas and stores its data as well as entries.
struct kan_resource_atlas_t
{
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_atlas_entry_t)
    struct kan_dynamic_array_t entries;

    /// \brief Total count of entries plus their replacements.
    /// \details Mostly needed for convenience during loading.
    kan_instance_size_t total_entries;

    kan_instance_size_t page_count;
    kan_instance_size_t page_width;
    kan_instance_size_t page_height;

    /// \brief 32-bit RGBA data in SRGB colorspace, page after page.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (uint8_t)
    struct kan_dynamic_array_t data;
};

RESOURCE_RENDER_FOUNDATION_API void kan_resource_atlas_init (struct kan_resource_atlas_t *instance);

RESOURCE_RENDER_FOUNDATION_API void kan_resource_atlas_shutdown (struct kan_resource_atlas_t *instance);

KAN_C_HEADER_END
