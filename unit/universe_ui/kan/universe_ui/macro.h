#pragma once

#include <kan/universe_ui/controls.h>
#include <kan/universe_ui/core.h>

/// \file
/// \brief Provides foundation-level macros for building UI layouts.
///
/// \par Motivation
/// \parblock
/// UI implementation provides lots of configuration capabilities for the elements and their behavior, however regular
/// UI construction code needs to construct lots of similar elements, which would result in giant unmaintainable pile of
/// code if we just copy-paste element configuration. This file aims to solve that by introducing set of
/// foundation-level macros to simplify UI layout construction. However, it does not force values for parameters that
/// can vary greatly in different cases, like sizes, flags, alignments and many more, therefore user is expected to wrap
/// these macros inside project-specific macros for their projects and use cases.
///
/// Warning: all UI macros expect `kan_ui_singleton_t` to be provided in read access through `ui` variable!
/// \endparblock
///
/// \par Hierarchy
/// \parblock
/// UI hierarchy creation can be simplified through specialized set of macros:
///
/// - `KAN_UIM_PREPARE_ROOT` and `KAN_UIM_PREPARE_CHILD` must be used to prepare function-scope variables that provide
///   root context for the UI that you're building in that function.
///
/// - `KAN_UIM_NEW_NODE` and `KAN_UIM_NEW_NODE_WITHOUT_ORDER` query function-scope variables in order to get attached to
///   a proper parent and calculate proper local element order if needed.
///
/// - `KAN_UIM_CHILDREN` and `KAN_UIM_CHILDREN_OF_ID` are used to mark everything created with the new node macros
///   in the code block after them as children of their argument.
///
/// Utilizing these macros makes hierarchy construction a lot easier and every complex macro in this file is built
/// on top of these macros.
/// \endparblock
///
/// \par Utility
/// \parblock
/// This file also provides macros for creating and filling most of the utility records that are attached to UI nodes.
/// \endparblock
///
/// \par Widgets
/// \parblock
/// Combinations of UI nodes and utility records that form more complex logical elements together are provided as widget
/// construction macros with `KAN_UIM_WIDGET_` prefix. Widgets could be as simple as combination of a node and several
/// utility records. Or they could be as complex as scroll panes or line edits that create several nodes and manage
/// them in specific way in order to keep complex element behavior correct.
/// \endparblock

KAN_C_HEADER_BEGIN

/// \brief Helper macro that deletes all ui nodes in the world.
/// \details When implementing hot-reloadable UI, it is common to clear everything and rebuild the ui on hot reload.
#define KAN_UIM_CLEAR_EVERYTHING                                                                                       \
    {                                                                                                                  \
        const kan_ui_node_id_t no_parent_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);                          \
        KAN_UML_VALUE_DELETE (ui_root, kan_ui_node_t, parent_id, &no_parent_id) { KAN_UM_ACCESS_DELETE (ui_root); }    \
    }

/// \brief Internal helper structure for keeping hierarchy and ordering data.
KAN_REFLECTION_IGNORE
struct kan_uim_parent_stack_info_t
{
    kan_ui_node_id_t parent_node_id;
    kan_instance_offset_t monotone_ascending_order;
};

/// \brief Prepares function-scope variables for building UI that is attached directly to the UI root.
#define KAN_UIM_PREPARE_ROOT                                                                                           \
    struct kan_uim_parent_stack_info_t kan_uim_parent_stack_info = {KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t), 0}

/// \brief Prepares function-scope variables for building UI that is attached to a node with given id.
#define KAN_UIM_PREPARE_CHILD(BASE_ID) struct kan_uim_parent_stack_info_t kan_uim_parent_stack_info = {(BASE_ID), 0}

#if defined(CMAKE_UNIT_FRAMEWORK_HIGHLIGHT)
#    define KAN_UIM_CHILDREN_OF_ID(ID)                                                                                 \
        kan_uim_parent_stack_info.parent_node_id = (ID);                                                               \
        if (true)
#else
#    define KAN_UIM_CHILDREN_OF_ID(ID)                                                                                 \
        {                                                                                                              \
            struct kan_uim_parent_stack_info_t kan_uim_previous_stack_info_##__CUSHION_REPLACEMENT_INDEX__ =           \
                kan_uim_parent_stack_info;                                                                             \
            kan_uim_parent_stack_info.parent_node_id = (ID);                                                           \
            kan_uim_parent_stack_info.monotone_ascending_order = 0;                                                    \
            CUSHION_DEFER { kan_uim_parent_stack_info = kan_uim_previous_stack_info_##__CUSHION_REPLACEMENT_INDEX__; } \
            __CUSHION_WRAPPED__                                                                                        \
        }
#endif

/// \brief Wrapper macro that makes everything created through `KAN_UIM_NEW_NODE` in wrapped block
///        children of node with given name.
#define KAN_UIM_CHILDREN(NAME) KAN_UIM_CHILDREN_OF_ID (NAME##_node->id)

/// \brief Same as `KAN_UIM_NEW_NODE`, but without local element order generation.
#define KAN_UIM_NEW_NODE_WITHOUT_ORDER(NAME)                                                                           \
    KAN_UMI_INDEXED_INSERT (NAME##_node, kan_ui_node_t)                                                                \
    NAME##_node->id = kan_next_ui_node_id (ui);                                                                        \
    NAME##_node->parent_id = kan_uim_parent_stack_info.parent_node_id;

/// \brief Uses inplace insert to create new UI node, attach it to proper parent and assign next local element order.
#define KAN_UIM_NEW_NODE(NAME)                                                                                         \
    KAN_UIM_NEW_NODE_WITHOUT_ORDER (NAME);                                                                             \
    NAME##_node->local_element_order = kan_uim_parent_stack_info.monotone_ascending_order;                             \
    ++kan_uim_parent_stack_info.monotone_ascending_order

/// \brief Creates new drawable record attached to a node with given name.
#define KAN_UIM_DRAWABLE(NAME)                                                                                         \
    KAN_UMI_INDEXED_INSERT (NAME##_drawable, kan_ui_node_drawable_t)                                                   \
    NAME##_drawable->id = NAME##_node->id

/// \brief Creates new drawable record and sets it up as image drawable with given image configuration.
#define KAN_UIM_DRAWABLE_IMAGE(NAME, ...)                                                                              \
    KAN_UIM_DRAWABLE (NAME);                                                                                           \
    NAME##_drawable->main_draw_command.type = KAN_UI_DRAW_COMMAND_IMAGE;                                               \
    NAME##_drawable->main_draw_command.image = __VA_ARGS__

/// \brief Creates new drawable record and sets it up as text drawable.
/// \invariant Expects shaping unit with name `NAME##_shaping_unit` to be created prior to this call.
#define KAN_UIM_DRAWABLE_TEXT(NAME)                                                                                    \
    KAN_UIM_DRAWABLE (NAME);                                                                                           \
    NAME##_drawable->main_draw_command.type = KAN_UI_DRAW_COMMAND_TEXT;                                                \
    NAME##_drawable->main_draw_command.text.shaping_unit = NAME##_shaping_unit->id;                                    \
    NAME##_drawable->main_draw_command.text.handle_alignment_on_overflow = true

/// \brief Creates new text behavior record attached to a node with given name.
/// \invariant Expects shaping unit with name `NAME##_shaping_unit` to be created prior to this call.
#define KAN_UIM_TEXT_BEHAVIOR(NAME)                                                                                    \
    KAN_UMI_INDEXED_INSERT (NAME##_text_behavior, kan_ui_node_text_behavior_t)                                         \
    NAME##_text_behavior->id = NAME##_node->id;                                                                        \
    NAME##_text_behavior->shaping_unit_id = NAME##_shaping_unit->id

/// \brief Creates new blocking no-action hit box record attached to a node with given name.
#define KAN_UIM_HIT_BOX_BLOCKING(NAME)                                                                                 \
    KAN_UMI_INDEXED_INSERT (NAME##_hit_box, kan_ui_node_hit_box_t)                                                     \
    NAME##_hit_box->id = NAME##_node->id;                                                                              \
    NAME##_hit_box->interactable = false

/// \brief Creates new interactable hit box record with given style attached to a node with given name.
#define KAN_UIM_HIT_BOX_INTERACTABLE(NAME, STYLE)                                                                      \
    KAN_UMI_INDEXED_INSERT (NAME##_hit_box, kan_ui_node_hit_box_t)                                                     \
    NAME##_hit_box->id = NAME##_node->id;                                                                              \
    NAME##_hit_box->interactable = true;                                                                               \
    NAME##_hit_box->interactable_style = STYLE

/// \brief Helper for attaching other nodes to a hit box for visual effect propagation.
#define KAN_UIM_PROPAGATE_HIT_BOX_VISUALS(HIT_BOX_NAME, TARGET_NAME)                                                   \
    /* We do not expect a lot of propagation, therefore we can get greedy with allocations. */                         \
    kan_dynamic_array_set_capacity (&HIT_BOX_NAME##_hit_box->propagate_interaction_visuals,                            \
                                    KAN_MAX (HIT_BOX_NAME##_hit_box->propagate_interaction_visuals.size + 1u,          \
                                             HIT_BOX_NAME##_hit_box->propagate_interaction_visuals.capacity));         \
    *(kan_ui_node_id_t *) kan_dynamic_array_add_last (&HIT_BOX_NAME##_hit_box->propagate_interaction_visuals) =        \
        TARGET_NAME##_node->id

/// \brief Image widget is simply a shortcut for a node with image drawable.
/// \warning Has no hit box by default!
#define KAN_UIM_WIDGET_IMAGE(NAME, ...)                                                                                \
    KAN_UIM_NEW_NODE (NAME);                                                                                           \
    KAN_UIM_DRAWABLE_IMAGE (NAME, __VA_ARGS__)

/// \brief Label widget is used for creating one-line texts that already have known secondary size from layout.
/// \warning Has no hit box by default!
#define KAN_UIM_WIDGET_LABEL(NAME, FONT_SIZE)                                                                          \
    KAN_UIM_NEW_NODE (NAME);                                                                                           \
    KAN_UIM_DRAWABLE_TEXT (NAME);                                                                                      \
    KAN_UIM_TEXT_BEHAVIOR (NAME);                                                                                      \
    NAME##_text_behavior->font_size = (FONT_SIZE);                                                                     \
    NAME##_text_behavior->sync_text_limit_from_ui = true

/// \brief Text widget is a generic multiline or single line text that does not know its secondary size from layout and
///        needs to grow to that size once it is shaped.
/// \warning Has no hit box by default!
#define KAN_UIM_WIDGET_TEXT(NAME, FONT_SIZE)                                                                           \
    KAN_UIM_NEW_NODE (NAME);                                                                                           \
    KAN_UIM_DRAWABLE_TEXT (NAME);                                                                                      \
    KAN_UIM_TEXT_BEHAVIOR (NAME);                                                                                      \
    NAME##_text_behavior->font_size = (FONT_SIZE);                                                                     \
    NAME##_text_behavior->sync_text_limit_from_ui = true;                                                              \
    NAME##_text_behavior->sync_ui_size_from_text_secondary = true

/// \brief Button widget is just an interactable hit box with an image animated through interactable style.
/// \details Other important stuff like button text should be added as child nodes of the button node.
#define KAN_UIM_WIDGET_BUTTON(NAME, STYLE)                                                                             \
    KAN_UIM_WIDGET_IMAGE (NAME, KAN_UI_IMAGE_COMMAND_NONE);                                                            \
    KAN_UIM_HIT_BOX_INTERACTABLE (NAME, STYLE);                                                                        \
    NAME##_hit_box->scroll_passthrough = true

/// \brief Button widget with text label added as a child.
/// \details Font enlarge factor is used to properly calculate text height if enlarging effects like outline are used.
/// \invariant Expects shaping unit with name `NAME##_shaping_unit` to be created prior to this call.
#define KAN_UIM_WIDGET_TEXT_BUTTON(NAME, STYLE, FONT_SIZE, FONT_ENLARGE_FACTOR)                                        \
    KAN_UIM_WIDGET_BUTTON (NAME, STYLE);                                                                               \
    KAN_UIM_NEW_NODE_WITHOUT_ORDER (NAME##_container);                                                                 \
    KAN_UIM_PROPAGATE_HIT_BOX_VISUALS (NAME, NAME##_container);                                                        \
                                                                                                                       \
    NAME##_container_node->parent_id = NAME##_node->id;                                                                \
    NAME##_container_node->element.width_flags |= KAN_UI_SIZE_FLAG_GROW;                                               \
    NAME##_container_node->element.height = (FONT_SIZE);                                                               \
    NAME##_container_node->element.height.value *= (FONT_ENLARGE_FACTOR) / 0.75f;                                      \
    NAME##_container_node->element.horizontal_alignment = KAN_UI_HORIZONTAL_ALIGNMENT_CENTER;                          \
    NAME##_container_node->element.vertical_alignment = KAN_UI_VERTICAL_ALIGNMENT_CENTER;                              \
                                                                                                                       \
    KAN_UIM_DRAWABLE (NAME##_container);                                                                               \
    NAME##_container_drawable->main_draw_command.type = KAN_UI_DRAW_COMMAND_TEXT;                                      \
    NAME##_container_drawable->main_draw_command.text.shaping_unit = NAME##_shaping_unit->id;                          \
    NAME##_container_drawable->main_draw_command.text.handle_alignment_on_overflow = true;                             \
                                                                                                                       \
    KAN_UMI_INDEXED_INSERT (NAME##_container_text_behavior, kan_ui_node_text_behavior_t)                               \
    NAME##_container_text_behavior->id = NAME##_node->id;                                                              \
    NAME##_container_text_behavior->shaping_unit_id = NAME##_shaping_unit->id;                                         \
    NAME##_container_text_behavior->font_size = (FONT_SIZE);                                                           \
    NAME##_container_text_behavior->sync_text_limit_from_ui = true

/// \brief Scroll pane widget consists of outer node that is placed in parent layout and container node that is child
///        of the outer node and which contains content to be scrolled.
/// \details This macro setups foundation for scroll pane, see macros below for proper configuration.
#define KAN_UIM_WIDGET_SCROLL_PANE(NAME)                                                                               \
    KAN_UIM_NEW_NODE (NAME##_outer);                                                                                   \
    NAME##_outer_node->layout.layout = KAN_UI_LAYOUT_FRAME;                                                            \
    NAME##_outer_node->render.clip = true;                                                                             \
    KAN_UIM_HIT_BOX_BLOCKING (NAME##_outer);                                                                           \
                                                                                                                       \
    KAN_UIM_NEW_NODE_WITHOUT_ORDER (NAME##_container);                                                                 \
    NAME##_container_node->parent_id = NAME##_outer_node->id;                                                          \
    NAME##_container_node->element.width_flags = KAN_UI_SIZE_FLAG_GROW;                                                \
    NAME##_container_node->element.height_flags = KAN_UI_SIZE_FLAG_GROW;                                               \
    NAME##_container_node->local_element_order = 0;                                                                    \
                                                                                                                       \
    KAN_UMI_INDEXED_INSERT (NAME##_scroll_behavior, kan_ui_node_scroll_behavior_t);                                    \
    NAME##_scroll_behavior->id = NAME##_outer_node->id;                                                                \
    NAME##_scroll_behavior->container_id = NAME##_container_node->id

/// \brief Enables horizontal scroll for given scroll pane.
#define KAN_UIM_WIDGET_SCROLL_PANE_ENABLE_HORIZONTAL(NAME)                                                             \
    NAME##_container_node->element.width_flags |= KAN_UI_SIZE_FLAG_FIT_CHILDREN;                                       \
    NAME##_scroll_behavior->horizontal = true

/// \brief Enables vertical scroll for given scroll pane.
#define KAN_UIM_WIDGET_SCROLL_PANE_ENABLE_VERTICAL(NAME)                                                               \
    NAME##_container_node->element.height_flags |= KAN_UI_SIZE_FLAG_FIT_CHILDREN;                                      \
    NAME##_scroll_behavior->vertical = true

/// \brief Constructs scroll line and knob for horizontal scrolling for given scroll pane.
/// \param NAME Scroll pane element name.
/// \param STYLE Style to be used for knob button visuals.
/// \param LINE_IMAGE Line image to be shown under the knob when users scrolls on that axis.
/// \param LINE_HEIGHT Fixed height for scrolling line and its knob.
#define KAN_UIM_WIDGET_SCROLL_PANE_HORIZONTAL_LINE(NAME, STYLE, LINE_IMAGE, LINE_HEIGHT)                               \
    KAN_UIM_NEW_NODE_WITHOUT_ORDER (NAME##_horizontal_line);                                                           \
    KAN_UIM_DRAWABLE_IMAGE (NAME##_horizontal_line, (LINE_IMAGE));                                                     \
    KAN_UIM_HIT_BOX_INTERACTABLE (NAME##_horizontal_line, (STYLE));                                                    \
    NAME##_horizontal_line_hit_box->scroll_passthrough = true;                                                         \
    NAME##_horizontal_line_node->parent_id = NAME##_outer_node->id;                                                    \
                                                                                                                       \
    NAME##_horizontal_line_node->element.width_flags = KAN_UI_SIZE_FLAG_GROW;                                          \
    NAME##_horizontal_line_node->element.height = (LINE_HEIGHT);                                                       \
    NAME##_horizontal_line_node->element.horizontal_alignment = KAN_UI_HORIZONTAL_ALIGNMENT_LEFT;                      \
    NAME##_horizontal_line_node->element.vertical_alignment = KAN_UI_VERTICAL_ALIGNMENT_BOTTOM;                        \
    NAME##_horizontal_line_node->local_element_order = 1;                                                              \
    NAME##_horizontal_line_node->layout.layout = KAN_UI_LAYOUT_FRAME;                                                  \
                                                                                                                       \
    KAN_UIM_NEW_NODE_WITHOUT_ORDER (NAME##_horizontal_knob);                                                           \
    NAME##_horizontal_knob_node->parent_id = NAME##_horizontal_line_node->id;                                          \
    NAME##_horizontal_knob_node->element.height_flags = KAN_UI_SIZE_FLAG_GROW;                                         \
    KAN_UIM_DRAWABLE_IMAGE (NAME##_horizontal_knob, KAN_UI_IMAGE_COMMAND_NONE);                                        \
    KAN_UIM_PROPAGATE_HIT_BOX_VISUALS (NAME##_horizontal_line, NAME##_horizontal_knob);                                \
                                                                                                                       \
    NAME##_scroll_behavior->horizontal_line_id = NAME##_horizontal_line_node->id;                                      \
    NAME##_scroll_behavior->horizontal_knob_id = NAME##_horizontal_knob_node->id

/// \brief Constructs scroll line and knob for vertical scrolling for given scroll pane.
/// \param NAME Scroll pane element name.
/// \param STYLE Style to be used for knob button visuals.
/// \param LINE_IMAGE Line image to be shown under the knob when users scrolls on that axis.
/// \param LINE_WIDTH Fixed width for scrolling line and its knob.
#define KAN_UIM_WIDGET_SCROLL_PANE_VERTICAL_LINE(NAME, STYLE, LINE_IMAGE, LINE_WIDTH)                                  \
    KAN_UIM_NEW_NODE_WITHOUT_ORDER (NAME##_vertical_line);                                                             \
    KAN_UIM_DRAWABLE_IMAGE (NAME##_vertical_line, (LINE_IMAGE));                                                       \
    KAN_UIM_HIT_BOX_INTERACTABLE (NAME##_vertical_line, (STYLE));                                                      \
    NAME##_vertical_line_hit_box->scroll_passthrough = true;                                                           \
    NAME##_vertical_line_node->parent_id = NAME##_outer_node->id;                                                      \
                                                                                                                       \
    NAME##_vertical_line_node->element.width = (LINE_WIDTH);                                                           \
    NAME##_vertical_line_node->element.height_flags = KAN_UI_SIZE_FLAG_GROW;                                           \
    NAME##_vertical_line_node->element.horizontal_alignment = KAN_UI_HORIZONTAL_ALIGNMENT_RIGHT;                       \
    NAME##_vertical_line_node->element.vertical_alignment = KAN_UI_VERTICAL_ALIGNMENT_TOP;                             \
    NAME##_vertical_line_node->local_element_order = 2;                                                                \
    NAME##_vertical_line_node->layout.layout = KAN_UI_LAYOUT_FRAME;                                                    \
                                                                                                                       \
    KAN_UIM_NEW_NODE_WITHOUT_ORDER (NAME##_vertical_knob);                                                             \
    NAME##_vertical_knob_node->parent_id = NAME##_vertical_line_node->id;                                              \
    NAME##_vertical_knob_node->element.width_flags = KAN_UI_SIZE_FLAG_GROW;                                            \
    KAN_UIM_DRAWABLE_IMAGE (NAME##_vertical_knob, KAN_UI_IMAGE_COMMAND_NONE);                                          \
    KAN_UIM_PROPAGATE_HIT_BOX_VISUALS (NAME##_vertical_line, NAME##_vertical_knob);                                    \
                                                                                                                       \
    NAME##_scroll_behavior->vertical_line_id = NAME##_vertical_line_node->id;                                          \
    NAME##_scroll_behavior->vertical_knob_id = NAME##_vertical_knob_node->id

/// \brief Line edit widget provides ability for inputting and editing single line text data.
/// \details Consists of two nodes: primary node and inner text node.
#define KAN_UIM_WIDGET_LINE_EDIT(NAME, FONT_SIZE, STYLE_REGULAR, STYLE_SELECTED)                                       \
    KAN_UIM_NEW_NODE (NAME);                                                                                           \
    KAN_UIM_DRAWABLE_IMAGE (NAME, KAN_UI_IMAGE_COMMAND_NONE);                                                          \
    KAN_UIM_HIT_BOX_INTERACTABLE (NAME, NULL);                                                                         \
                                                                                                                       \
    NAME##_node->layout.layout = KAN_UI_LAYOUT_FRAME;                                                                  \
    NAME##_hit_box->scroll_passthrough = true;                                                                         \
                                                                                                                       \
    KAN_NEW_TEXT_SHAPING_UNIT (NAME);                                                                                  \
    KAN_UIM_NEW_NODE_WITHOUT_ORDER (NAME##_inner);                                                                     \
    KAN_UIM_DRAWABLE (NAME##_inner);                                                                                   \
    KAN_UIM_PROPAGATE_HIT_BOX_VISUALS (NAME, NAME##_inner);                                                            \
                                                                                                                       \
    NAME##_inner_node->parent_id = NAME##_node->id;                                                                    \
    NAME##_inner_node->render.clip = true;                                                                             \
                                                                                                                       \
    NAME##_inner_drawable->main_draw_command.type = KAN_UI_DRAW_COMMAND_TEXT;                                          \
    NAME##_inner_drawable->main_draw_command.text.shaping_unit = NAME##_shaping_unit->id;                              \
    NAME##_inner_drawable->main_draw_command.text.handle_alignment_on_overflow = false;                                \
                                                                                                                       \
    KAN_UMI_INDEXED_INSERT (NAME##_text_behavior, kan_ui_node_text_behavior_t)                                         \
    NAME##_text_behavior->id = NAME##_inner_node->id;                                                                  \
    NAME##_text_behavior->shaping_unit_id = NAME##_shaping_unit->id;                                                   \
    NAME##_text_behavior->font_size = (FONT_SIZE);                                                                     \
    NAME##_text_behavior->sync_text_limit_from_ui = true;                                                              \
                                                                                                                       \
    KAN_UMI_INDEXED_INSERT (NAME##_line_edit_behavior, kan_ui_node_line_edit_behavior_t)                               \
    NAME##_line_edit_behavior->id = NAME##_node->id;                                                                   \
    NAME##_line_edit_behavior->text_id = NAME##_inner_node->id;                                                        \
    NAME##_line_edit_behavior->shaping_unit_id = NAME##_shaping_unit->id;                                              \
    NAME##_line_edit_behavior->interactable_style_regular = (STYLE_REGULAR);                                           \
    NAME##_line_edit_behavior->interactable_style_selected = (STYLE_SELECTED);                                         \
                                                                                                                       \
    /* Usually these variable values are connected. */                                                                 \
    NAME##_line_edit_behavior->cursor_safe_space = (FONT_SIZE);

/// \brief Helper for calculating line edit selection leeway when enlarging effect like outline is used.
#define KAN_UIM_WIDGET_LINE_EDIT_CALCULATE_LEEWAY(NAME, ENLARGE_FACTOR)                                                \
    NAME##_line_edit_behavior->selection_leeway = NAME##_text_behavior->font_size;                                     \
    NAME##_line_edit_behavior->selection_leeway.value *= (ENLARGE_FACTOR) - 1.0f;

/// \brief Helper for calculating line edit height so text would fit inside properly.
#define KAN_UIM_WIDGET_LINE_EDIT_CALCULATE_HEIGHT(NAME, ENLARGE_FACTOR)                                                \
    KAN_ASSERT ((NAME##_node->layout.padding.top.type == NAME##_text_behavior->font_size.type &&                       \
                 NAME##_node->layout.padding.bottom.type == NAME##_text_behavior->font_size.type) ||                   \
                (KAN_FLOATING_IS_NEAR (0.0f, NAME##_node->layout.padding.top.value) &&                                 \
                 KAN_FLOATING_IS_NEAR (0.0f, NAME##_node->layout.padding.bottom.value)))                               \
                                                                                                                       \
    NAME##_node->element.height = NAME##_text_behavior->font_size;                                                     \
    NAME##_node->element.height.value *= (ENLARGE_FACTOR) / 0.75f;                                                     \
    NAME##_node->element.height.value +=                                                                               \
        NAME##_node->layout.padding.top.value + NAME##_node->layout.padding.bottom.value;

KAN_C_HEADER_END
