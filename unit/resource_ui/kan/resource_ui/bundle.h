#pragma once

#include <resource_ui_api.h>

#include <kan/api_common/core_types.h>
#include <kan/container/dynamic_array.h>
#include <kan/container/interned_string.h>
#include <kan/reflection/markup.h>

// TODO: Docs.

KAN_C_HEADER_BEGIN

struct kan_resource_ui_button_style_t
{
    kan_interned_string_t name;
    kan_interned_string_t enabled_image;
    kan_interned_string_t disabled_image;
    kan_interned_string_t hovered_image;
    kan_interned_string_t down_image;
};

struct kan_resource_ui_bundle_t
{
    /// \brief Material instance used for rendering UI images.
    kan_interned_string_t image_material_instance;

    /// \brief Atlas that contains UI images.
    kan_interned_string_t image_atlas;

    /// \brief Material instance used for rendering text glyphs in SDF format.
    kan_interned_string_t text_sdf_material_instance;

    /// \brief Material instance used for rendering text icons.
    kan_interned_string_t text_icon_material_instance;

    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_ui_button_style_t)
    struct kan_dynamic_array_t button_styles;
};

RESOURCE_UI_API void kan_resource_ui_bundle_init (struct kan_resource_ui_bundle_t *instance);

RESOURCE_UI_API void kan_resource_ui_bundle_shutdown (struct kan_resource_ui_bundle_t *instance);

KAN_C_HEADER_END
