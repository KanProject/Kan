#pragma once

#include <universe_ui_api.h>

#include <kan/api_common/c_header.h>
#include <kan/api_common/core_types.h>
#include <kan/context/application_system.h>
#include <kan/inline_math/inline_math.h>
#include <kan/universe_ui/core.h>

/// \file
/// \brief Provides range of utility attachments for UI nodes that are used to build interactable UI widgets.
///
/// \par Motivation
/// \parblock
/// UI core knows nothing about input and behaviors of UI elements, it only handles laying out and rendering.
/// UI controls are special records to be attached to UI nodes in order to add behaviors to them and make it possible
/// to process user input.
/// \endparblock
///
/// \par Input
/// \parblock
/// Input processing is built upon hit boxes: custom records that are attached to nodes that should are either
/// interactable or block input to the nodes that are drawn under them. Hit boxes are added and configured through
/// `kan_ui_node_hit_box_t`. Common input configuration and also a public readable state is available in
/// `kan_ui_input_singleton_t`.
///
/// Mouse press processing routine is exposed to the user through `kan_ui_press_begin_t`, `kan_ui_press_end_t` and also
/// `kan_ui_multi_click_t` events.
/// \endparblock
///
/// \par Behaviors
/// \parblock
/// This file provides several behavior-level records that manage node to which they are attached and are used to
/// implement more complex widgets. These behaviors include:
///
/// - `kan_ui_node_text_behavior_t` for managing synchronization between UI nodes and shaping units.
/// - `kan_ui_node_scroll_behavior_t` for implementing scroll pane widgets.
/// - `kan_ui_node_line_edit_behavior_t` for implementing text line edit widgets.
/// \endparblock

KAN_C_HEADER_BEGIN

/// \brief Group that is used to add all ui controls mutators.
/// \details Must be added to the same world as `KAN_UI_CORE_MUTATOR_GROUP`.
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

/// \brief Checkpoint, after which controls update mutators that must done before rendering nodes out are executed.
/// \details Mutators from this group might depend on text shaping as it provides data that might affect render.
#define KAN_UI_CONTROLS_PRE_RENDER_BEGIN_CHECKPOINT "ui_controls_pre_render_begin"

/// \brief Checkpoint, that is hit after all controls update mutators that must be before rendering nodes
///        have finished execution.
#define KAN_UI_CONTROLS_PRE_RENDER_END_CHECKPOINT "ui_controls_pre_render_end"

/// \brief Singleton that is used to configure input processing and also exposes public input state.
/// \details Should not be accessed outside UI leaf worlds, otherwise processing input from several
///          windows would be impossible.
struct kan_ui_input_singleton_t
{
    /// \brief Event iterator used to get input events inside input processing mutator.
    kan_application_system_event_iterator_t event_iterator;

    /// \brief Handle to a window used to modify its state if needed, for example enabling/disabling text input.
    kan_application_system_window_t linked_window_handle;

    /// \brief Window id is used to filter events received from `event_iterator`.
    /// \details Only events with the same window id will be processed.
    kan_platform_window_id_t linked_window_id;

    /// \brief Offset that is applied to raw mouse position in window events on X axis.
    /// \details Used for the cases when UI is not attached to (0, 0) coordinate of the window.
    kan_instance_offset_t viewport_offset_x;
    
    /// \brief Offset that is applied to raw mouse position in window events on Y axis.
    /// \details Used for the cases when UI is not attached to (0, 0) coordinate of the window.
    kan_instance_offset_t viewport_offset_y;

    /// \brief Id of a node with behavior that aims to consume input if any node like that is selected by the user.
    /// \details Used to indicate that user has selected input consuming control like line edit and that the game
    ///          should not try to consume the input.
    kan_ui_node_id_t input_receiver_id;

    /// \brief Mouse buttons that are currently down as flags.
    uint32_t mouse_button_down_flags;
    
    /// \brief All mouse buttons that were down at any point of current filtered in press.
    uint32_t mouse_button_down_inclusive_flags;
    
    /// \brief Id of an interactable node on which current press has started.
    kan_ui_node_id_t press_started_on_id;
    
    /// \brief True if press interaction has started and is considered valid.
    bool press_filtered_in;

    /// \brief Last read mouse position on X axis including `viewport_offset_x`.
    kan_instance_offset_t last_mouse_x;
    
    /// \brief Last read mouse position on Y axis including `viewport_offset_y`.
    kan_instance_offset_t last_mouse_y;
    
    /// \brief Id of an interactable node which is considered hovered right now.
    kan_ui_node_id_t current_hovered_id;
};

UNIVERSE_UI_API void kan_ui_input_singleton_init (struct kan_ui_input_singleton_t *instance);

/// \brief Indicates that UI node has a hit box that can be either interactable or just blocking.
struct kan_ui_node_hit_box_t
{
    kan_immutable kan_ui_node_id_t id;

    /// \brief If true, hit box can be hovered and can be pressed on, and also supports visuals for that.
    kan_immutable bool interactable;
    
    /// \brief If true, this hit box will ignored when processing mouse scroll event in order to pass this event to
    ///        scroll pane hit box below if such hit box exists.
    kan_immutable bool scroll_passthrough;

    /// \brief Name of the `kan_resource_ui_hit_box_interaction_style_t` in bundle for hover and press visualization.
    /// \details Only used when `interactable`.
    kan_interned_string_t interactable_style;
    
    /// \brief Press is only registered if hit box is `interactable` and mouse button was found in this mask.
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

/// \brief Event that is sent when multiple successive clicks were detected, for example double click.
/// \details In case of multiple clicks, for example double click, press begin and end events for the first click will
///          be sent as regular, and then multi click will be sent as well on top of that.
struct kan_ui_multi_click_t
{
    kan_ui_node_id_t node_id;
    enum kan_platform_mouse_button_t multi_click_button;
    kan_instance_size_t click_count;
    
    /// \brief Mask of mouse buttons that were down during the click.
    /// \details Useful for special cases, for example when left button was double clicked while right button was down.
    uint32_t mouse_button_down_flags;
    
    kan_instance_offset_t at_x;
    kan_instance_offset_t at_y;
};

/// \brief Event that is sent when valid press interaction was started.
/// \details Press interaction is only valid when mouse button was down on top of interactable hit box with matching
///          `mouse_button_down_flags` mask or when there is no hit box under the mouse at all (which is processed as
///          valid press on potential game render area).
struct kan_ui_press_begin_t
{
    kan_ui_node_id_t node_id;
    uint32_t mouse_button_down_flags;
    kan_instance_offset_t at_x;
    kan_instance_offset_t at_y;
};

/// \brief Event that is sent when valid press interaction was finished.
struct kan_ui_press_end_t
{
    kan_ui_node_id_t node_id;
    uint32_t mouse_button_down_inclusive_flags;

    /// \brief True only and if only press began and ended on the same element.
    bool continuous_press;

    kan_instance_offset_t at_x;
    kan_instance_offset_t at_y;
};

/// \brief Synchronizes behavior of UI node and text shaping unit, useful for nodes that display text.
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

/// \brief Provides behavior that implements scroll pane widget interactions.
/// \invariant Should be inserted in the same frame as connected ui node.
///            Appending to an existent node is not supported.
/// \invariant Should be attached to scroll pane outer node, not container node.
/// \invariant Must have hit box in order to process user scroll input.
struct kan_ui_node_scroll_behavior_t
{
    kan_immutable kan_ui_node_id_t id;
    
    /// \brief Id of a node that contains actual elements.
    /// \details In most cases, container node uses KAN_UI_SIZE_FLAG_FIT_CHILDREN to get automatically resized to
    ///          children size on required direction.
    kan_immutable kan_ui_node_id_t container_id;

    /// \brief If horizontal visual line is needed, then it is an id of a node that represents it.
    /// \invariant `horizontal` must be `true`.
    /// \invariant Must have interactable hit box in order to process user input.
    /// \invariant `horizontal_knob_id` must be a valid id.
    kan_immutable kan_ui_node_id_t horizontal_line_id;
    
    /// \brief If `horizontal_line_id` is valid id, then this node represents knob on that line.
    kan_immutable kan_ui_node_id_t horizontal_knob_id;

    /// \brief If vertical visual line is needed, then it is an id of a node that represents it.
    /// \invariant `vertical` must be `true`.
    /// \invariant Must have interactable hit box in order to process user input.
    /// \invariant `vertical_knob_id` must be a valid id.
    kan_immutable kan_ui_node_id_t vertical_line_id;
    
    /// \brief If `vertical_line_id` is valid id, then this node represents knob on that line.
    kan_immutable kan_ui_node_id_t vertical_knob_id;

    /// \brief If true, then horizontal scrolling is supported.
    kan_immutable bool horizontal;
    
    /// \brief If true, then vertical scrolling is supported.
    kan_immutable bool vertical;

    /// \brief Coordinate type used to represent horizontal scroll offset.
    /// \details Needed to properly preserve scrolling when resize happens. Only user knows which coordinate type is
    ///          primarily used in container, therefore user should set this value to that coordinate type.
    kan_immutable enum kan_ui_coordinate_type_t offset_coordinate_type_x;
    
    /// \brief Coordinate type used to represent vertical scroll offset.
    /// \details Needed to properly preserve scrolling when resize happens. Only user knows which coordinate type is
    ///          primarily used in container, therefore user should set this value to that coordinate type.
    kan_immutable enum kan_ui_coordinate_type_t offset_coordinate_type_y;

    /// \brief Horizontal scroll speed for the mouse wheel.
    kan_immutable struct kan_ui_coordinate_t mouse_speed_x;
    
    /// \brief Vertical scroll speed for the mouse wheel.
    kan_immutable struct kan_ui_coordinate_t mouse_speed_y;

    /// \brief If true, then scroll lines are always visible instead of only being visible due to interaction.
    kan_immutable bool lines_always_visible;
    
    /// \brief Scroll line are visible for this amount of seconds after interaction.
    kan_immutable float line_visibility_s;
};

UNIVERSE_UI_API void kan_ui_node_scroll_behavior_init (struct kan_ui_node_scroll_behavior_t *instance);

/// \brief Provides behavior that implements line edit widget interactions.
/// \invariant Should be inserted in the same frame as connected ui node.
///            Appending to an existent node is not supported.
/// \invariant Shaping unit should also be inserted in the same frame or earlier, but not after that frame.
///            Shaping unit content is discarded and overridden by behavior content.
/// \invariant Must have hit box in order to process user input.
struct kan_ui_node_line_edit_behavior_t
{
    kan_immutable kan_ui_node_id_t id;
    
    /// \brief Id of a child node that contains text that is being edited.
    /// \details That node should have `kan_ui_node_text_behavior_t` that points to the same shaping unit as
    ///          `shaping_unit_id` of this behavior.
    kan_immutable kan_ui_node_id_t text_id;
    
    /// \brief Shaping unit that is used to shape edited text.
    kan_immutable kan_text_shaping_unit_id_t shaping_unit_id;

    /// \brief When not selected, this style is applied to hit box.
    kan_immutable kan_interned_string_t interactable_style_regular;
    
    /// \brief When selected for text edition, this style is applied to hit box.
    kan_immutable kan_interned_string_t interactable_style_selected;

    /// \brief User should set this to true when manually modifying content from outside.
    /// \details It is advised to call `kan_ui_node_line_edit_behavior_set_content` that sets this flag automatically.
    bool content_dirty;

    /// \brief True if visual data should be updated, usually set internally, but should also be manually set when user
    ///        changes configuration for text edition elements like cursor or selection.
    bool text_visuals_dirty;

    /// \brief Line edit content string in utf8 encoding.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (uint8_t)
    struct kan_dynamic_array_t content_utf8;

    /// \brief Style that is applied to content text object prior to shaping.
    kan_interned_string_t content_style;
    
    /// \brief Mark value that is applied to content text object prior to shaping.
    uint32_t content_mark;

    /// \brief Index of an image in UI atlas that should be used for cursor draw command.
    uint32_t cursor_image_index;
    
    /// \brief UI draw mark that should be passed to cursor draw command.
    uint32_t cursor_ui_mark;
    
    /// \brief Width for cursor draw rect.
    /// \details Height is calculated automatically from shaped text ascent and descent.
    struct kan_ui_coordinate_t cursor_width;

    /// \brief Cursor visuals will always be at least this distance from the borders set through primary axis limit.
    /// \details Guaranteed to not "detach" text from borders: for example, being at the beginning of left-to-right
    ///          line will not create whitespace before the beginning of text with the safe of safe space.
    struct kan_ui_coordinate_t cursor_safe_space;

    /// \brief Index of an image in UI atlas that should be used for background selection draw command.
    uint32_t selection_image_index;
    
    /// \brief UI draw mark that should be passed to background selection draw command.
    uint32_t selection_ui_mark;

    /// \brief Selection is extended to both directions on primary axis by this value.
    /// \details Glyph sizes could be affected by render techniques, therefore some leeway might be required for proper
    ///          selection positioning. Also, in some cases tightly fitting selection looks worse than selection with
    ///          leeway of several pixels.
    struct kan_ui_coordinate_t selection_leeway;

    /// \brief Current location of user cursor inside content in bytes if any.
    /// \details `KAN_INT_MAX (kan_instance_size_t)` if no cursor right now.
    /// \invariant Should not be edited by user, expected to be only modified by inner logic.
    kan_instance_size_t cursor_content_location;
    
    /// \brief Current location of selection minimum border inside content in bytes if any.
    /// \details `KAN_INT_MAX (kan_instance_size_t)` if no selection right now.
    /// \invariant Should not be edited by user, expected to be only modified by inner logic.
    kan_instance_size_t selection_content_min;
    
    /// \brief Current location of selection maximum border inside content in bytes if any.
    /// \details `KAN_INT_MAX (kan_instance_size_t)` if no selection right now.
    /// \invariant Should not be edited by user, expected to be only modified by inner logic.
    kan_instance_size_t selection_content_max;
};

UNIVERSE_UI_API void kan_ui_node_line_edit_behavior_init (struct kan_ui_node_line_edit_behavior_t *instance);

/// \brief Helper for setting line edit content from outside.
UNIVERSE_UI_API void kan_ui_node_line_edit_behavior_set_content (struct kan_ui_node_line_edit_behavior_t *instance,
                                                                 const char *null_terminated_utf8_content,
                                                                 kan_interned_string_t content_style,
                                                                 uint32_t content_mark);

UNIVERSE_UI_API void kan_ui_node_line_edit_behavior_shutdown (struct kan_ui_node_line_edit_behavior_t *instance);

KAN_C_HEADER_END
