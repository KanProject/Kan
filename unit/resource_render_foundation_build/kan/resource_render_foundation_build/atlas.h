#pragma once

#include <resource_render_foundation_build_api.h>

#include <kan/api_common/core_types.h>
#include <kan/container/dynamic_array.h>
#include <kan/container/interned_string.h>
#include <kan/error/critical.h>
#include <kan/reflection/markup.h>
#include <kan/resource_render_foundation/atlas.h>

// TODO: Docs.

KAN_C_HEADER_BEGIN

struct kan_resource_atlas_image_header_t
{
    kan_interned_string_t source;

    enum kan_resource_atlas_image_type_t type;

    union
    {
        KAN_REFLECTION_VISIBILITY_CONDITION_FIELD (type)
        KAN_REFLECTION_VISIBILITY_CONDITION_VALUE (KAN_RESOURCE_ATLAS_IMAGE_TYPE_NINE_PATCH)
        struct kan_resource_atlas_image_nine_patch_t nine_patch;
    };

    kan_instance_offset_t color_table_multiplicator_index;
};

struct kan_resource_atlas_entry_replacement_header_t
{
    kan_interned_string_t for_locale;
    struct kan_resource_atlas_image_header_t image;
};

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

struct kan_resource_atlas_header_t
{
    kan_instance_size_t page_width;
    kan_instance_size_t page_height;

    kan_instance_size_t border_size;
    float border_linear_r;
    float border_linear_g;
    float border_linear_b;
    float border_linear_a;

    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_atlas_entry_header_t)
    struct kan_dynamic_array_t entries;
};

RESOURCE_RENDER_FOUNDATION_BUILD_API void kan_resource_atlas_header_init (struct kan_resource_atlas_header_t *instance);

RESOURCE_RENDER_FOUNDATION_BUILD_API void kan_resource_atlas_header_shutdown (
    struct kan_resource_atlas_header_t *instance);

KAN_C_HEADER_END
