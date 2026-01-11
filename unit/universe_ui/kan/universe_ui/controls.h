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
    kan_application_system_window_t linked_window_handle;
    kan_platform_window_id_t linked_window_id;

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

struct kan_ui_node_hit_box_t
{
    kan_immutable kan_ui_node_id_t id;

    kan_immutable bool interactable;
    kan_immutable bool scroll_passthrough;

    kan_interned_string_t interactable_style;
    kan_immutable uint32_t mouse_button_down_flags;

    /// \brief Marks and styles will be propagated to these node drawables as well.
    /// \details Must always be children of the hit box node, otherwise visual mistakes may occur. This rule is used
    ///          for code simplification, because it is difficult to come up with scenarios when it is needed outside
    ///          of hit box children.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (kan_ui_node_id_t)
    kan_immutable struct kan_dynamic_array_t propagate_interaction_visuals;
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
/// \invariant Shaping unit should also be inserted in the same frame or earlier, but not after that frame.
struct kan_ui_node_text_behavior_t
{
    kan_immutable kan_ui_node_id_t id;
    kan_immutable kan_text_shaping_unit_id_t shaping_unit_id;

    /// \brief Font size that will be applied to the shaping unit text.
    /// \warning UI coordinate calculation result should be actual font size, not expect pixel height! Therefore, using
    ///          KAN_UI_PX is treated as specifying the size directly.
    kan_immutable struct kan_ui_coordinate_t font_size;

    /// \brief If true, appropriate element size will be used as limit for text shaping unit.
    /// \details Width is used for horizontal text and height for vertical text.
    kan_immutable bool sync_text_limit_from_ui;

    /// \brief If true, text secondary direction size will be applied to the element size.
    /// \details For horizontal text, text height is applied to the element.
    ///          For vertical text, text width is applied to the element.
    kan_immutable bool sync_ui_size_from_text_secondary;
};

UNIVERSE_UI_API void kan_ui_node_text_behavior_init (struct kan_ui_node_text_behavior_t *instance);

struct kan_ui_node_scroll_behavior_t
{
    kan_immutable kan_ui_node_id_t id;
    kan_immutable kan_ui_node_id_t container_id;

    kan_immutable kan_ui_node_id_t horizontal_line_id;
    kan_immutable kan_ui_node_id_t horizontal_knob_id;

    kan_immutable kan_ui_node_id_t vertical_line_id;
    kan_immutable kan_ui_node_id_t vertical_knob_id;

    kan_immutable bool horizontal;
    kan_immutable bool vertical;

    kan_immutable enum kan_ui_coordinate_type_t offset_coordinate_type_x;
    kan_immutable enum kan_ui_coordinate_type_t offset_coordinate_type_y;

    kan_immutable struct kan_ui_coordinate_t mouse_speed_x;
    kan_immutable struct kan_ui_coordinate_t mouse_speed_y;

    kan_immutable bool lines_always_visible;
    kan_immutable float line_visibility_s;
};

UNIVERSE_UI_API void kan_ui_node_scroll_behavior_init (struct kan_ui_node_scroll_behavior_t *instance);

struct kan_ui_node_line_edit_behavior_t
{
    kan_immutable kan_ui_node_id_t id;
    kan_immutable kan_ui_node_id_t text_id;
    kan_immutable kan_text_shaping_unit_id_t shaping_unit_id;

    kan_immutable kan_interned_string_t interactable_style_regular;
    kan_immutable kan_interned_string_t interactable_style_selected;

    /// \brief User should set this to true when manually modifying content from outside.
    bool content_dirty;

    /// \brief True if visual data should be updated, used internally and should not be edited by the user.
    /// \details Cursor and selection visuals are updated not only when text is changed and reshaped, it can also be
    ///          triggered by user actions like moving cursor or changing selection.
    bool text_visuals_dirty;

    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (uint8_t)
    struct kan_dynamic_array_t content_utf8;

    kan_interned_string_t content_style;
    uint32_t content_mark;

    // TODO: Image indices for selection and cursor?
    //       And then change docs so user can set text_visuals_dirty to true when image indices are changed.

    kan_instance_size_t cursor_content_location;
    kan_instance_size_t selection_content_min;
    kan_instance_size_t selection_content_max;
};

UNIVERSE_UI_API void kan_ui_node_line_edit_behavior_init (struct kan_ui_node_line_edit_behavior_t *instance);

/// \brief Helper for setting line edit content from outside.
UNIVERSE_UI_API void kan_ui_node_line_edit_behavior_set_content (struct kan_ui_node_line_edit_behavior_t *instance,
                                                                 const char *null_terminated_utf8_content,
                                                                 kan_interned_string_t content_style,
                                                                 uint32_t content_mark);

UNIVERSE_UI_API void kan_ui_node_line_edit_behavior_shutdown (struct kan_ui_node_line_edit_behavior_t *instance);

// TODO: Line edit in input:
//       - outer dirty processing: clean cursor/selection for dirty, do not clear dirty flag
//       - process all the input
//       - update dirty content
// TODO: After shaping: update cursor position (based on data index), update selection visuals.
// TODO: When searching for a cluster to get cursor position, we need to find a cluster that starts at content index and
//       then "step back" to get cluster from which position should be retrieved. Should keep in mind that stepping
//       back must know about pointer inversion: step back from inverted is actually a step forward. Step back from
//       non-inverted to inverted must search for the first inverted in the inverted run. Step back (forward) from
//       inverted to non-inverted should search for the first non-inverted cluster before the current inverted run.
//       It can be visually improved by also taking into account whether user has clicked on inverted or non-inverted
//       cluster, but I think we can leave it out for now. Bidi text edition is a difficult topic anyway.

KAN_C_HEADER_END
