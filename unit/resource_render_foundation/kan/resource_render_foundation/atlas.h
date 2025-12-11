#pragma once

#include <resource_render_foundation_api.h>

#include <kan/api_common/core_types.h>
#include <kan/container/dynamic_array.h>
#include <kan/container/interned_string.h>
#include <kan/error/critical.h>
#include <kan/reflection/markup.h>

// TODO: Docs.

KAN_C_HEADER_BEGIN

enum kan_resource_atlas_image_type_t
{
    KAN_RESOURCE_ATLAS_IMAGE_TYPE_REGULAR = 0u,
    KAN_RESOURCE_ATLAS_IMAGE_TYPE_NINE_PATCH,
};

struct kan_resource_atlas_image_nine_patch_t
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

struct kan_resource_atlas_image_t
{
    kan_instance_size_t page;
    kan_instance_size_t x;
    kan_instance_size_t y;
    kan_instance_size_t width;
    kan_instance_size_t height;

    enum kan_resource_atlas_image_type_t type;
    union
    {
        KAN_REFLECTION_VISIBILITY_CONDITION_FIELD (type)
        KAN_REFLECTION_VISIBILITY_CONDITION_VALUE (KAN_RESOURCE_ATLAS_IMAGE_TYPE_NINE_PATCH)
        struct kan_resource_atlas_image_nine_patch_t nine_patch;
    };

    /// \brief If not max value of kan_instance_size_t, then image color data should be multiplied by color table entry
    ///        with that index.
    /// \details Global color tables are used for color blindness color remapping. For example, ui buff and debuff icons
    ///          can be grayscale and then multiplied by color at appropriate index: green for normal vision and blue
    ///          in colorblind mode. Also, tables make it easy to support custom color configurations for color
    ///          blindness, which is supported in some other games and applications.
    kan_instance_size_t color_table_multiplier_index;
};

struct kan_resource_atlas_entry_replacement_t
{
    kan_interned_string_t for_locale;
    struct kan_resource_atlas_image_t image;
};

struct kan_resource_atlas_entry_t
{
    kan_interned_string_t name;
    struct kan_resource_atlas_image_t image;

    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_atlas_entry_replacement_t)
    struct kan_dynamic_array_t replacements;
};

RESOURCE_RENDER_FOUNDATION_API void kan_resource_atlas_entry_init (struct kan_resource_atlas_entry_t *instance);

RESOURCE_RENDER_FOUNDATION_API void kan_resource_atlas_entry_shutdown (struct kan_resource_atlas_entry_t *instance);

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
