#pragma once

#include <universe_ui_api.h>

#include <kan/api_common/c_header.h>
#include <kan/api_common/core_types.h>
#include <kan/inline_math/inline_math.h>
#include <kan/universe_ui/core.h>

// TODO: Docs.

KAN_C_HEADER_BEGIN

/// \brief Group that is used to add all ui controls mutators: both layout and render ones.
#define KAN_UI_CONTROLS_MUTATOR_GROUP "ui_controls"

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

enum kan_ui_node_selectable_t
{
    /// \brief Not selectable.
    KAN_UI_NODE_SELECTABLE_NONE = 0u,

    /// \brief Only one node of this type can be selected at once and should be known to everyone outside.
    KAN_UI_NODE_SELECTABLE_EDIT,

    /// \brief Simple selection that can be enabled and disabled.
    KAN_UI_NODE_SELECTABLE_CHECK,

    /// \brief Only one node can be selected among the radio group.
    KAN_UI_NODE_SELECTABLE_RADIO,
};

struct kan_ui_node_interactable_t
{
    kan_ui_node_id_t id;

    bool enabled;
    bool focusable;
    enum kan_ui_node_selectable_t selectable;

    kan_interned_string_t regular_style;
    kan_interned_string_t style_when_selected;

    /// \brief Marks and styles will be propagated to these node drawables as well.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (kan_ui_node_id_t)
    struct kan_dynamic_array_t propagate_visuals;

    // TODO: Draft, not implemented.
};

UNIVERSE_UI_API void kan_ui_node_interactable_init (struct kan_ui_node_interactable_t *instance);

UNIVERSE_UI_API void kan_ui_node_interactable_shutdown (struct kan_ui_node_interactable_t *instance);

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
