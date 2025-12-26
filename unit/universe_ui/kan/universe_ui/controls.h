#pragma once

#include <universe_ui_api.h>

#include <kan/api_common/c_header.h>
#include <kan/api_common/core_types.h>
#include <kan/context/application_system.h>
#include <kan/inline_math/inline_math.h>
#include <kan/universe_ui/core.h>

// TODO: Docs.

KAN_C_HEADER_BEGIN

/// \brief Group that is used to add all ui controls mutators: both layout and render ones.
#define KAN_UI_CONTROLS_MUTATOR_GROUP "ui_controls"

/// \brief Checkpoint, after which controls input mutators are executed.
#define KAN_UI_CONTROLS_INPUT_BEGIN_CHECKPOINT "ui_controls_input_begin"

/// \brief Checkpoint, that is hit after all controls input mutators have finished execution.
#define KAN_UI_CONTROLS_INPUT_END_CHECKPOINT "ui_controls_input_end"

/// \brief Checkpoint, after which controls update mutators that must be done before laying out are executed.
#define KAN_UI_CONTROLS_PRE_LAYOUT_BEGIN_CHECKPOINT "ui_controls_pre_layout_begin"

/// \brief Checkpoint, that is hit after all controls update mutators that must be done before laying out
///        have finished execution.
#define KAN_UI_CONTROLS_PRE_LAYOUT_END_CHECKPOINT "ui_controls_pre_layout_end"

/// \brief Checkpoint, after which controls update mutators that must be done after laying out are executed.
#define KAN_UI_CONTROLS_POST_LAYOUT_BEGIN_CHECKPOINT "ui_controls_post_layout_begin"

/// \brief Checkpoint, that is hit after all controls update mutators that must be done after laying out
///        have finished execution.
#define KAN_UI_CONTROLS_POST_LAYOUT_END_CHECKPOINT "ui_controls_post_layout_end"

struct kan_ui_input_singleton_t
{
    kan_application_system_event_iterator_t event_iterator;
    kan_platform_window_id_t window_filter;

    kan_instance_offset_t viewport_offset_x;
    kan_instance_offset_t viewport_offset_y;

    uint32_t mouse_button_down_flags;
    uint32_t mouse_button_down_inclusive_flags;
    kan_ui_node_id_t mouse_button_down_on_id;
    bool press_filtered_in;

    kan_instance_offset_t last_mouse_x;
    kan_instance_offset_t last_mouse_y;
    kan_ui_node_id_t current_hovered_id;
};

UNIVERSE_UI_API void kan_ui_input_singleton_init (struct kan_ui_input_singleton_t *instance);

/// \details Fields in this structure are not expected to be edited by the user in any case. They're expected to be
///          filled at UI creation time and never changed later. If changes are needed, node should be deleted and
///          added back.
struct kan_ui_node_hit_box_t
{
    kan_ui_node_id_t id;

    bool interactable;
    kan_interned_string_t interactable_style;
    uint32_t mouse_button_down_flags;

    /// \brief Marks and styles will be propagated to these node drawables as well.
    /// \details Must always be children of the hit box node, otherwise visual mistakes may occur. This rule is used
    ///          for code simplification, because it is difficult to come up with scenarios when it is needed outside
    ///          of hit box children.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (kan_ui_node_id_t)
    struct kan_dynamic_array_t propagate_interaction_visuals;
};

UNIVERSE_UI_API void kan_ui_node_hit_box_init (struct kan_ui_node_hit_box_t *instance);

UNIVERSE_UI_API void kan_ui_node_hit_box_shutdown (struct kan_ui_node_hit_box_t *instance);

struct kan_ui_multi_click_t
{
    kan_ui_node_id_t node_id;
    enum kan_platform_mouse_button_t multi_click_button;
    kan_instance_size_t click_count;
    uint32_t mouse_button_down_flags;
    kan_instance_offset_t at_x;
    kan_instance_offset_t at_y;
};

struct kan_ui_press_begin_t
{
    kan_ui_node_id_t node_id;
    uint32_t mouse_button_down_flags;
    kan_instance_offset_t at_x;
    kan_instance_offset_t at_y;
};

struct kan_ui_press_end_t
{
    kan_ui_node_id_t node_id;
    uint32_t mouse_button_down_inclusive_flags;

    /// \brief True only and if only press began and ended on the same element.
    bool continuous_press;

    kan_instance_offset_t at_x;
    kan_instance_offset_t at_y;
};

/// \invariant Should be inserted in the same frame as connected ui node.
///            Appending to an existent node is not supported.
/// \invariant Shaping unit should also be inserted in the same frame. Shaping unit id edition is not supported.
struct kan_ui_node_text_behavior_t
{
    kan_ui_node_id_t id;
    kan_text_shaping_unit_id_t shaping_unit;

    /// \brief Font size that will be applied to the shaping unit text.
    /// \warning UI coordinate calculation result should be actual font size, not expect pixel height! Therefore, using
    ///          KAN_UI_PX is treated as specifying the size directly.
    struct kan_ui_coordinate_t font_size;

    /// \brief If true, appropriate element size will be used as limit for text shaping unit.
    /// \details Width is used for horizontal text and height for vertical text.
    bool sync_text_limit_from_ui;

    /// \brief If true, text secondary direction size will be applied to the element size.
    /// \details For horizontal text, text height is applied to the element.
    ///          For vertical text, text width is applied to the element.
    bool sync_ui_size_from_text_secondary;
};

UNIVERSE_UI_API void kan_ui_node_text_behavior_init (struct kan_ui_node_text_behavior_t *instance);

KAN_C_HEADER_END
