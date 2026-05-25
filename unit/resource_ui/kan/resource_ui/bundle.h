#pragma once

#include <resource_ui_api.h>

#include <kan/api_common/core_types.h>
#include <kan/container/dynamic_array.h>
#include <kan/container/interned_string.h>
#include <kan/reflection/markup.h>

/// \file
/// \brief This file stores runtime representation of ui bundle resource.
///
/// \par Overview
/// \parblock
/// UI bundle is a root resource that enumerates other resources needed to setup and render the UI along with some
/// additional information for the setup. Bundle makes it possible to specify how UI looks on the world level by
/// providing one resource name.
/// \endparblock

KAN_C_HEADER_BEGIN

/// \brief Named style for rendering interactable hit box nodes.
struct kan_resource_ui_hit_box_interaction_style_t
{
    kan_interned_string_t name;
    kan_interned_string_t regular_image;
    kan_interned_string_t hovered_image;
    kan_interned_string_t down_image;
    kan_interned_string_t disabled_image;

    /// \brief For how much time hit box should be rendered as down after being pressed.
    kan_floating_t down_state_s;
};

RESOURCE_UI_API void kan_resource_ui_hit_box_interaction_style_init (
    struct kan_resource_ui_hit_box_interaction_style_t *instance);

/// \brief UI bundle material instance names packed into one struct for easier array-like processing.
struct kan_resource_ui_bundle_material_instances_t
{
    /// \brief Material instance used for rendering UI images.
    kan_interned_string_t image;

    /// \brief Material instance used for rendering text glyphs in SDF format.
    kan_interned_string_t text_sdf;

    /// \brief Material instance used for rendering text icons.
    kan_interned_string_t text_icon;
};

/// \brief Count of material instances in `kan_resource_ui_bundle_material_instances_t`.
#define KAN_RESOURCE_UI_BUNDLE_MATERIAL_INSTANCE_COUNT                                                                 \
    ((sizeof (struct kan_resource_ui_bundle_material_instances_t) / sizeof (kan_interned_string_t)))

/// \brief Describes ui bundle resource.
struct kan_resource_ui_bundle_t
{
    /// \brief Pass name for UI render.
    kan_interned_string_t pass;

    /// \brief Atlas that contains UI images.
    kan_interned_string_t image_atlas;

    union
    {
        struct kan_resource_ui_bundle_material_instances_t material_instances;
        kan_interned_string_t material_instances_array[KAN_RESOURCE_UI_BUNDLE_MATERIAL_INSTANCE_COUNT];
    };

    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_ui_hit_box_interaction_style_t)
    struct kan_dynamic_array_t hit_box_interaction_styles;
};

RESOURCE_UI_API void kan_resource_ui_bundle_init (struct kan_resource_ui_bundle_t *instance);

RESOURCE_UI_API void kan_resource_ui_bundle_shutdown (struct kan_resource_ui_bundle_t *instance);

KAN_C_HEADER_END
