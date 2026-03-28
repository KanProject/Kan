#define _CRT_SECURE_NO_WARNINGS __CUSHION_PRESERVE__

#include <string.h>

#include <kan/context/all_system_names.h>
#include <kan/log/logging.h>
#include <kan/universe/macro.h>
#include <kan/universe_locale/locale.h>
#include <kan/universe_render_foundation/atlas.h>
#include <kan/universe_ui/controls.h>

KAN_USE_STATIC_INTERNED_IDS
KAN_USE_STATIC_CPU_SECTIONS

KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (ui_controls_input)
KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (ui_controls_pre_layout)
KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (ui_controls_post_layout)
KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (ui_controls_pre_render)
UNIVERSE_UI_API KAN_UM_MUTATOR_GROUP_META (ui_controls, KAN_UI_CONTROLS_MUTATOR_GROUP);

KAN_REFLECTION_STRUCT_META (kan_ui_node_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_cascade_deletion_t kan_ui_node_hit_box_cascade_deletion = {
    .parent_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
    .child_type_name = "kan_ui_node_hit_box_t",
    .child_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_cascade_deletion_t kan_ui_node_key_binding_cascade_deletion = {
    .parent_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
    .child_type_name = "kan_ui_node_key_binding_t",
    .child_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_cascade_deletion_t kan_ui_node_text_behavior_cascade_deletion = {
    .parent_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
    .child_type_name = "kan_ui_node_text_behavior_t",
    .child_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_text_behavior_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_cascade_deletion_t
    kan_ui_node_text_behavior_shaping_unit_cascade_deletion = {
        .parent_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"shaping_unit_id"}},
        .child_type_name = "kan_text_shaping_unit_t",
        .child_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_cascade_deletion_t kan_ui_node_scroll_behavior_cascade_deletion = {
    .parent_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
    .child_type_name = "kan_ui_node_scroll_behavior_t",
    .child_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_cascade_deletion_t
    kan_ui_node_line_edit_behavior_cascade_deletion = {
        .parent_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
        .child_type_name = "kan_ui_node_line_edit_behavior_t",
        .child_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_cascade_deletion_t kan_ui_node_map_behavior_cascade_deletion = {
    .parent_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
    .child_type_name = "kan_ui_node_map_behavior_t",
    .child_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_cascade_deletion_t kan_ui_node_map_pin_cascade_deletion = {
    .parent_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
    .child_type_name = "kan_ui_node_map_pin_t",
    .child_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_cascade_deletion_t kan_ui_node_popup_behavior_cascade_deletion = {
    .parent_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
    .child_type_name = "kan_ui_node_popup_behavior_t",
    .child_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_cascade_deletion_t
    kan_ui_node_hide_popup_on_press_cascade_deletion = {
        .parent_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
        .child_type_name = "kan_ui_node_hide_popup_on_press_t",
        .child_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
};

struct kan_ui_node_down_mark_t
{
    kan_ui_node_id_t id;
    kan_floating_t down_until_s;
    enum kan_platform_scan_code_t down_from_key;
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_cascade_deletion_t kan_ui_node_down_mark_cascade_deletion = {
    .parent_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
    .child_type_name = "kan_ui_node_down_mark_t",
    .child_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
};

struct kan_ui_node_hit_box_on_insert_event_t
{
    kan_ui_node_id_t id;
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_hit_box_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_on_insert_event_t kan_ui_node_hit_box_on_insert_event = {
    .event_type = "kan_ui_node_hit_box_on_insert_event_t",
    .copy_outs_count = 1u,
    .copy_outs =
        (struct kan_repository_copy_out_t[]) {
            {
                .source_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
                .target_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
            },
        },
};

struct kan_ui_node_hit_box_on_state_change_event_t
{
    kan_ui_node_id_t id;
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_hit_box_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_on_change_event_t kan_ui_node_hit_box_on_state_change_event = {
    .event_type = "kan_ui_node_hit_box_on_state_change_event_t",
    .observed_fields_count = 2u,
    .observed_fields =
        (struct kan_repository_field_path_t[]) {
            {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"disabled"}},
            {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"interactable_style"}},
        },
    .unchanged_copy_outs_count = 0u,
    .unchanged_copy_outs = NULL,
    .changed_copy_outs_count = 1u,
    .changed_copy_outs =
        (struct kan_repository_copy_out_t[]) {
            {
                .source_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
                .target_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
            },
        },
};

struct kan_ui_node_text_behavior_on_insert_event_t
{
    kan_ui_node_id_t id;
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_text_behavior_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_on_insert_event_t kan_ui_node_text_behavior_on_insert_event = {
    .event_type = "kan_ui_node_text_behavior_on_insert_event_t",
    .copy_outs_count = 1u,
    .copy_outs =
        (struct kan_repository_copy_out_t[]) {
            {
                .source_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
                .target_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
            },
        },
};

struct kan_ui_node_scroll_behavior_on_insert_event_t
{
    kan_ui_node_id_t id;
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_scroll_behavior_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_on_insert_event_t kan_ui_node_scroll_behavior_on_insert_event = {
    .event_type = "kan_ui_node_scroll_behavior_on_insert_event_t",
    .copy_outs_count = 1u,
    .copy_outs =
        (struct kan_repository_copy_out_t[]) {
            {
                .source_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
                .target_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
            },
        },
};

struct kan_ui_node_line_edit_behavior_on_insert_event_t
{
    kan_ui_node_id_t id;
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_line_edit_behavior_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_on_insert_event_t kan_ui_node_line_edit_behavior_on_insert_event =
    {
        .event_type = "kan_ui_node_line_edit_behavior_on_insert_event_t",
        .copy_outs_count = 1u,
        .copy_outs =
            (struct kan_repository_copy_out_t[]) {
                {
                    .source_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
                    .target_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
                },
            },
};

struct kan_ui_node_popup_behavior_on_insert_event_t
{
    kan_ui_node_id_t id;
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_popup_behavior_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_on_insert_event_t kan_ui_node_popup_behavior_on_insert_event = {
    .event_type = "kan_ui_node_popup_behavior_on_insert_event_t",
    .copy_outs_count = 1u,
    .copy_outs =
        (struct kan_repository_copy_out_t[]) {
            {
                .source_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
                .target_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
            },
        },
};

enum kan_ui_node_scroll_line_class_t
{
    KAN_UI_NODE_SCROLL_LINE_CLASS_HORIZONTAL = 0u,
    KAN_UI_NODE_SCROLL_LINE_CLASS_VERTICAL,
};

struct kan_ui_node_scroll_line_state_t
{
    kan_ui_node_id_t id;
    kan_ui_node_id_t behavior_id;
    kan_ui_node_id_t knob_id;
    enum kan_ui_node_scroll_line_class_t class;
    kan_floating_t visible_until_s;
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_cascade_deletion_t kan_ui_node_scroll_line_state_cascade_deletion =
    {
        .parent_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
        .child_type_name = "kan_ui_node_scroll_line_state_t",
        .child_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
};

UNIVERSE_UI_API void kan_ui_node_scroll_line_state_init (struct kan_ui_node_scroll_line_state_t *instance)
{
    instance->id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    instance->behavior_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    instance->class = KAN_UI_NODE_SCROLL_LINE_CLASS_HORIZONTAL;
    instance->visible_until_s = 0.0f;
}

enum kan_ui_node_popup_action_t
{
    KAN_UI_NODE_POPUP_ACTION_NONE = 0u,
    KAN_UI_NODE_POPUP_ACTION_SHOW,
    KAN_UI_NODE_POPUP_ACTION_HIDE,
};

struct kan_ui_node_popup_state_t
{
    kan_immutable kan_ui_node_id_t id;
    enum kan_ui_node_popup_action_t pending_action;
    kan_floating_t pending_action_time_s;
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_cascade_deletion_t kan_ui_node_popup_state_cascade_deletion = {
    .parent_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
    .child_type_name = "kan_ui_node_popup_state_t",
    .child_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
};

struct ui_controls_input_private_singleton_t
{
    kan_instance_offset_t press_knob_offset;

    bool input_receiver_requested_text_input;

    kan_instance_size_t line_edit_press_start_content_location;
    bool line_edit_selected_this_press;
    bool line_edit_press_moved;
};

UNIVERSE_UI_API void ui_controls_input_private_singleton_init (struct ui_controls_input_private_singleton_t *instance)
{
    instance->press_knob_offset = 0;
    instance->input_receiver_requested_text_input = false;
    instance->line_edit_press_start_content_location = KAN_INT_MAX (kan_instance_size_t);
    instance->line_edit_selected_this_press = false;
    instance->line_edit_press_moved = false;
}

struct ui_controls_input_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (ui_controls_input)
    KAN_UM_BIND_STATE (ui_controls_input, state)
    kan_context_system_t application_system_handle;
};

UNIVERSE_UI_API KAN_UM_MUTATOR_DEPLOY (ui_controls_input)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_cpu_static_sections_ensure_initialized ();

    kan_context_t context = kan_universe_get_context (universe);
    state->application_system_handle = kan_context_query (context, KAN_CONTEXT_APPLICATION_SYSTEM_NAME);

    kan_workflow_graph_node_depend_on (workflow_node, KAN_UI_BUNDLE_MANAGEMENT_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_UI_TIME_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_UI_CONTROLS_INPUT_BEGIN_CHECKPOINT);
    kan_workflow_graph_node_make_dependency_of (workflow_node, KAN_UI_CONTROLS_INPUT_END_CHECKPOINT);
    kan_workflow_graph_node_make_dependency_of (workflow_node, KAN_UI_CONTROLS_PRE_LAYOUT_BEGIN_CHECKPOINT);
}

KAN_REFLECTION_IGNORE
enum input_hit_box_mouse_update_mode_t
{
    INPUT_HIT_BOX_MOUSE_UPDATE_MODE_NONE = 0u,
    INPUT_HIT_BOX_MOUSE_UPDATE_MODE_RESET,
    INPUT_HIT_BOX_MOUSE_UPDATE_MODE_EXECUTE,
};

KAN_REFLECTION_IGNORE
enum hit_box_search_mode_t
{
    /// \brief Mode that processes results appropriately for pointer events like mouse pointer.
    HIT_BOX_SEARCH_MODE_POINTER = 0u,

    /// \brief Mode that filters results appropriate for scroll events like mouse scroll.
    HIT_BOX_SEARCH_MODE_SCROLL,
};

static const struct kan_ui_node_hit_box_t *find_hit_box_at (
    struct ui_controls_input_state_t *state,
    enum hit_box_search_mode_t mode,
    kan_instance_offset_t at_x,
    kan_instance_offset_t at_y,
    struct kan_repository_indexed_sequence_read_access_t *output_access)
{
    kan_instance_size_t found_index = KAN_INT_MAX (kan_instance_size_t);
    const struct kan_ui_node_hit_box_t *found = NULL;

    KAN_UML_SEQUENCE_READ (hit_box, kan_ui_node_hit_box_t)
    {
        KAN_UMI_VALUE_READ_OPTIONAL (drawable, kan_ui_node_drawable_t, id, &hit_box->id)
        if (!drawable || drawable->hidden_permanently || drawable->hidden_temporary ||
            (found && found_index > drawable->draw_index))
        {
            continue;
        }

        bool early_filter_out = false;
        switch (mode)
        {
        case HIT_BOX_SEARCH_MODE_POINTER:
            break;

        case HIT_BOX_SEARCH_MODE_SCROLL:
            if (hit_box->scroll_passthrough)
            {
                early_filter_out = true;
            }

            break;
        }

        if (early_filter_out)
        {
            continue;
        }

        // Check if we intersect with bounds.
        if (at_x < drawable->global_x || at_x >= drawable->global_x + drawable->width || at_y < drawable->global_y ||
            at_y >= drawable->global_y + drawable->height)
        {
            continue;
        }

        // Check that intersection is in clip rect as well.
        if (at_x < drawable->clip_rect.x || at_x >= drawable->clip_rect.x + drawable->clip_rect.width ||
            at_y < drawable->clip_rect.y || at_y >= drawable->clip_rect.y + drawable->clip_rect.height)
        {
            continue;
        }

        if (found)
        {
            kan_repository_indexed_sequence_read_access_close (output_access);
        }

        found_index = drawable->draw_index;
        found = hit_box;
        KAN_UM_ACCESS_ESCAPE (*output_access, hit_box);
    }

    return found;
}

static const struct kan_ui_node_hit_box_t *find_hit_box_from_key_binding (
    struct ui_controls_input_state_t *state,
    enum kan_platform_scan_code_t scan_code,
    struct kan_repository_indexed_value_read_access_t *output_access)
{
    kan_instance_size_t found_index = KAN_INT_MAX (kan_instance_size_t);
    const struct kan_ui_node_hit_box_t *found = NULL;

    KAN_UML_SEQUENCE_READ (binding, kan_ui_node_key_binding_t)
    {
        if (binding->scan_code != scan_code)
        {
            continue;
        }

        KAN_UMI_VALUE_READ_OPTIONAL (hit_box, kan_ui_node_hit_box_t, id, &binding->id)
        KAN_UMI_VALUE_READ_OPTIONAL (drawable, kan_ui_node_drawable_t, id, &binding->id)

        if (!hit_box || !drawable || drawable->hidden_permanently || drawable->hidden_temporary ||
            (found && found_index > drawable->draw_index))
        {
            continue;
        }

        if (found)
        {
            kan_repository_indexed_value_read_access_close (output_access);
        }

        found_index = drawable->draw_index;
        found = hit_box;
        KAN_UM_ACCESS_ESCAPE (*output_access, hit_box);
    }

    return found;
}

#define FOCUS_FLAGS_MASK                                                                                               \
    (KAN_UI_DEFAULT_MARK_FLAG_HOVERED | KAN_UI_DEFAULT_MARK_FLAG_DOWN | KAN_UI_DEFAULT_MARK_FLAG_DISABLED)
#define SET_FOCUS_FLAGS(TARGET, FLAGS) (TARGET) = ((TARGET) & ~FOCUS_FLAGS_MASK) | (FLAGS)

static inline void use_hit_box_interaction_visuals (struct ui_controls_input_state_t *state,
                                                    kan_ui_node_id_t node_id,
                                                    uint32_t flags,
                                                    uint32_t regular_image_index)
{
    KAN_UMI_VALUE_UPDATE_OPTIONAL (drawable, kan_ui_node_drawable_t, id, &node_id)
    if (!drawable)
    {
        return;
    }

    SET_FOCUS_FLAGS (drawable->main_draw_command.ui_mark, flags);
    if (regular_image_index != KAN_INT_MAX (uint32_t) &&
        drawable->main_draw_command.type == KAN_UI_DRAW_COMMAND_IMAGE &&
        drawable->main_draw_command.image.allow_override)
    {
        drawable->main_draw_command.image.record_index = regular_image_index;
    }

    for (kan_memory_size_t index = 0u; index < drawable->additional_draw_commands.size; ++index)
    {
        struct kan_ui_draw_command_data_t *command =
            &((struct kan_ui_draw_command_data_t *) drawable->additional_draw_commands.data)[index];
        SET_FOCUS_FLAGS (command->ui_mark, flags);

        if (regular_image_index != KAN_INT_MAX (uint32_t) && command->type == KAN_UI_DRAW_COMMAND_IMAGE &&
            command->image.allow_override)
        {
            command->image.record_index = regular_image_index;
        }
    }
}

static uint32_t query_image (struct ui_controls_input_state_t *state,
                             const struct kan_ui_bundle_singleton_t *bundle,
                             kan_interned_string_t name)
{
    KAN_UMI_SINGLETON_READ (locale, kan_locale_singleton_t)
    KAN_UMI_VALUE_READ_REQUIRED (atlas, kan_render_atlas_loaded_t, name, &bundle->available_bundle.image_atlas)
    return kan_render_atlas_loaded_query (atlas, name, locale->selected_locale);
}

KAN_REFLECTION_IGNORE
enum hit_box_interaction_flags_t
{
    FOCUS_STATE_HOVERED = 1u << 0u,
    FOCUS_STATE_DOWN = 1u << 1u,
    FOCUS_STATE_DISABLED = 1u << 2u,
};

static inline enum hit_box_interaction_flags_t calculate_hit_box_interaction_flags (
    struct ui_controls_input_state_t *state,
    const struct kan_ui_input_singleton_t *public,
    const struct kan_ui_node_hit_box_t *hit_box,
    bool force_not_down)
{
    enum hit_box_interaction_flags_t flags = 0u;
    if (hit_box->disabled)
    {
        flags |= FOCUS_STATE_DISABLED;
        // If disabled, then cannot be hovered or down by definition.
        return flags;
    }

    if (KAN_TYPED_ID_32_IS_EQUAL (public->current_hovered_id, hit_box->id))
    {
        flags |= FOCUS_STATE_HOVERED;
    }

    if (!force_not_down)
    {
        KAN_UMI_VALUE_READ_OPTIONAL (down_mark, kan_ui_node_down_mark_t, id, &hit_box->id)
        if (down_mark)
        {
            flags |= FOCUS_STATE_DOWN;
        }
    }

    return flags;
}

static inline const struct kan_resource_ui_hit_box_interaction_style_t *find_interactable_style (
    const struct kan_ui_bundle_singleton_t *bundle, kan_interned_string_t style_name)
{
    for (kan_memory_size_t index = 0u; index < bundle->available_bundle.hit_box_interaction_styles.size; ++index)
    {
        const struct kan_resource_ui_hit_box_interaction_style_t *style =
            &((struct kan_resource_ui_hit_box_interaction_style_t *)
                  bundle->available_bundle.hit_box_interaction_styles.data)[index];

        if (style->name == style_name)
        {
            return style;
        }
    }

    return NULL;
}

static inline uint32_t select_image_for_hit_box_interaction (struct ui_controls_input_state_t *state,
                                                             const struct kan_ui_bundle_singleton_t *bundle,
                                                             kan_interned_string_t style_name,
                                                             enum hit_box_interaction_flags_t flags)
{
    if (!style_name)
    {
        return KAN_INT_MAX (uint32_t);
    }

    const struct kan_resource_ui_hit_box_interaction_style_t *style = find_interactable_style (bundle, style_name);
    if (style)
    {
        if (flags & FOCUS_STATE_DOWN)
        {
            return query_image (state, bundle, style->down_image);
        }
        else if (flags & FOCUS_STATE_HOVERED)
        {
            return query_image (state, bundle, style->hovered_image);
        }
        else if (flags & FOCUS_STATE_DISABLED)
        {
            return query_image (state, bundle, style->disabled_image);
        }

        return query_image (state, bundle, style->regular_image);
    }

    return KAN_INT_MAX (uint32_t);
}

static void update_interacted_scroll_line_visibility (struct ui_controls_input_state_t *state,
                                                      const struct kan_ui_input_singleton_t *public,
                                                      struct kan_ui_node_scroll_line_state_t *line_state,
                                                      const struct kan_ui_node_scroll_behavior_t *behavior,
                                                      bool interacted_with_hit_box)
{
    if (KAN_FLOATING_IS_NEAR (line_state->visible_until_s, 0.0f))
    {
        // If was invisible, make visible again.
        KAN_UMI_VALUE_UPDATE_OPTIONAL (line_node, kan_ui_node_t, id, &line_state->id)
        if (line_node)
        {
            line_node->render.hide_children = false;
        }
    }

    if (behavior->lines_always_visible || interacted_with_hit_box ||
        KAN_TYPED_ID_32_IS_EQUAL (line_state->id, public->press_started_on_id))
    {
        line_state->visible_until_s = FLT_MAX;
        return;
    }

    KAN_UMI_SINGLETON_READ (ui, kan_ui_singleton_t)
    line_state->visible_until_s = ui->animation_global_time_s + behavior->line_visibility_s;
}

static void apply_hit_box_interaction_visuals (struct ui_controls_input_state_t *state,
                                               const struct kan_ui_input_singleton_t *public,
                                               const struct kan_ui_bundle_singleton_t *bundle,
                                               const struct kan_ui_node_hit_box_t *hit_box,
                                               bool force_not_down)
{
    KAN_ASSERT (hit_box->interactable)
    const enum hit_box_interaction_flags_t flags =
        calculate_hit_box_interaction_flags (state, public, hit_box, force_not_down);

    const uint32_t image_index =
        select_image_for_hit_box_interaction (state, bundle, hit_box->interactable_style, flags);
    uint32_t ui_mark_flags = 0u;

    if (flags & FOCUS_STATE_DOWN)
    {
        ui_mark_flags |= KAN_UI_DEFAULT_MARK_FLAG_DOWN;
    }

    if (flags & FOCUS_STATE_HOVERED)
    {
        ui_mark_flags |= KAN_UI_DEFAULT_MARK_FLAG_HOVERED;
    }

    if (flags & FOCUS_STATE_DISABLED)
    {
        ui_mark_flags |= KAN_UI_DEFAULT_MARK_FLAG_DISABLED;
    }

    use_hit_box_interaction_visuals (state, hit_box->id, ui_mark_flags, image_index);
    for (kan_memory_size_t index = 0u; index < hit_box->propagate_interaction_visuals.size; ++index)
    {
        use_hit_box_interaction_visuals (state,
                                         ((kan_ui_node_id_t *) hit_box->propagate_interaction_visuals.data)[index],
                                         ui_mark_flags, image_index);
    }

    KAN_UMI_VALUE_UPDATE_OPTIONAL (scroll_line_state, kan_ui_node_scroll_line_state_t, id, &hit_box->id)
    if (scroll_line_state)
    {
        KAN_UMI_VALUE_READ_REQUIRED (behavior, kan_ui_node_scroll_behavior_t, id, &scroll_line_state->behavior_id)
        update_interacted_scroll_line_visibility (state, public, scroll_line_state, behavior, flags != 0u);
    }
}

static void update_scroll_horizontal_knob (struct ui_controls_input_state_t *state,
                                           const struct kan_ui_node_scroll_behavior_t *behavior,
                                           const struct kan_ui_node_drawable_t *main_drawable,
                                           const struct kan_ui_node_drawable_t *container_drawable,
                                           kan_floating_t scroll_value_px)
{
    KAN_UMI_VALUE_READ_OPTIONAL (horizontal_line_drawable, kan_ui_node_drawable_t, id, &behavior->horizontal_line_id)
    KAN_UMI_VALUE_UPDATE_OPTIONAL (horizontal_knob_node, kan_ui_node_t, id, &behavior->horizontal_knob_id)

    if (horizontal_line_drawable && horizontal_knob_node)
    {
        const kan_floating_t knob_size_ratio =
            KAN_MIN (1.0f, (kan_floating_t) main_drawable->width / (kan_floating_t) container_drawable->width);
        const kan_floating_t knob_size_px = knob_size_ratio * (kan_floating_t) horizontal_line_drawable->width;
        horizontal_knob_node->element.width = KAN_UI_VALUE_PX (knob_size_px);

        const kan_floating_t knob_offset_ratio = scroll_value_px / (kan_floating_t) container_drawable->width;
        const kan_floating_t knob_offset_px = knob_offset_ratio * (kan_floating_t) horizontal_line_drawable->width;
        horizontal_knob_node->element.frame_offset_x = KAN_UI_VALUE_PX (knob_offset_px);
    }
}

static void update_scroll_vertical_knob (struct ui_controls_input_state_t *state,
                                         const struct kan_ui_node_scroll_behavior_t *behavior,
                                         const struct kan_ui_node_drawable_t *main_drawable,
                                         const struct kan_ui_node_drawable_t *container_drawable,
                                         kan_floating_t scroll_value_px)
{
    KAN_UMI_VALUE_READ_OPTIONAL (vertical_line_drawable, kan_ui_node_drawable_t, id, &behavior->vertical_line_id)
    KAN_UMI_VALUE_UPDATE_OPTIONAL (vertical_knob_node, kan_ui_node_t, id, &behavior->vertical_knob_id)

    if (vertical_line_drawable && vertical_knob_node)
    {
        const kan_floating_t knob_size_ratio =
            KAN_MIN (1.0f, (kan_floating_t) main_drawable->height / (kan_floating_t) container_drawable->height);
        const kan_floating_t knob_size_px = knob_size_ratio * (kan_floating_t) vertical_line_drawable->height;
        vertical_knob_node->element.height = KAN_UI_VALUE_PX (knob_size_px);

        const kan_floating_t knob_offset_ratio = scroll_value_px / (kan_floating_t) container_drawable->height;
        const kan_floating_t knob_offset_px = knob_offset_ratio * (kan_floating_t) vertical_line_drawable->height;
        vertical_knob_node->element.frame_offset_y = KAN_UI_VALUE_PX (knob_offset_px);
    }
}

static inline void update_horizontal_scroll_absolute (struct ui_controls_input_state_t *state,
                                                      const struct kan_ui_singleton_t *ui,
                                                      struct kan_ui_node_scroll_behavior_t *behavior,
                                                      const struct kan_ui_node_drawable_t *main_drawable,
                                                      struct kan_ui_node_t *container_node,
                                                      const struct kan_ui_node_drawable_t *container_drawable,
                                                      kan_floating_t value_px)
{
    const kan_floating_t new_scroll_px =
        KAN_CLAMP (value_px, 0.0f, (kan_floating_t) (KAN_MAX (0, container_drawable->width - main_drawable->width)));
    container_node->render.scroll_x =
        kan_ui_coordinate_from_pixels (ui, behavior->offset_coordinate_type_x, new_scroll_px);
    update_scroll_horizontal_knob (state, behavior, main_drawable, container_drawable, new_scroll_px);
}

static inline void update_vertical_scroll_absolute (struct ui_controls_input_state_t *state,
                                                    const struct kan_ui_singleton_t *ui,
                                                    struct kan_ui_node_scroll_behavior_t *behavior,
                                                    const struct kan_ui_node_drawable_t *main_drawable,
                                                    struct kan_ui_node_t *container_node,
                                                    const struct kan_ui_node_drawable_t *container_drawable,
                                                    kan_floating_t value_px)
{
    const kan_floating_t new_scroll_px =
        KAN_CLAMP (value_px, 0.0f, (kan_floating_t) (KAN_MAX (0, container_drawable->height - main_drawable->height)));
    container_node->render.scroll_y =
        kan_ui_coordinate_from_pixels (ui, behavior->offset_coordinate_type_y, new_scroll_px);
    update_scroll_vertical_knob (state, behavior, main_drawable, container_drawable, new_scroll_px);
}

static void ensure_scroll_is_in_limits (struct ui_controls_input_state_t *state,
                                        const struct kan_ui_singleton_t *ui,
                                        const struct kan_ui_node_scroll_behavior_t *behavior)
{
    KAN_UMI_VALUE_READ_OPTIONAL (main_drawable, kan_ui_node_drawable_t, id, &behavior->id)
    KAN_UMI_VALUE_UPDATE_OPTIONAL (container_node, kan_ui_node_t, id, &behavior->container_id)
    KAN_UMI_VALUE_READ_OPTIONAL (container_drawable, kan_ui_node_drawable_t, id, &behavior->container_id)

    if (!main_drawable || !container_node || !container_drawable)
    {
        return;
    }

    if (behavior->horizontal)
    {
        const kan_floating_t current_scroll_px =
            kan_ui_calculate_coordinate_floating (ui, container_node->render.scroll_x);
        const kan_floating_t new_scroll_px = KAN_CLAMP (
            current_scroll_px, 0.0f, (kan_floating_t) (KAN_MAX (0, container_drawable->width - main_drawable->width)));

        container_node->render.scroll_x =
            kan_ui_coordinate_from_pixels (ui, behavior->offset_coordinate_type_x, new_scroll_px);
        update_scroll_horizontal_knob (state, behavior, main_drawable, container_drawable, new_scroll_px);
    }

    if (behavior->vertical)
    {
        const kan_floating_t current_scroll_px =
            kan_ui_calculate_coordinate_floating (ui, container_node->render.scroll_y);
        const kan_floating_t new_scroll_px =
            KAN_CLAMP (current_scroll_px, 0.0f,
                       (kan_floating_t) (KAN_MAX (0, container_drawable->height - main_drawable->height)));

        container_node->render.scroll_y =
            kan_ui_coordinate_from_pixels (ui, behavior->offset_coordinate_type_y, new_scroll_px);
        update_scroll_vertical_knob (state, behavior, main_drawable, container_drawable, new_scroll_px);
    }
}

static void hide_scroll_line (struct ui_controls_input_state_t *state,
                              struct kan_ui_node_scroll_line_state_t *line_state)
{
    KAN_UMI_VALUE_UPDATE_OPTIONAL (line_node, kan_ui_node_t, id, &line_state->id)
    if (line_node)
    {
        line_node->render.hide_children = true;
    }

    line_state->visible_until_s = 0.0f;
}

static bool process_hit_box_insertion (struct ui_controls_input_state_t *state,
                                       struct kan_ui_input_singleton_t *public,
                                       const struct kan_ui_bundle_singleton_t *bundle)
{
    bool hit_boxes_changed = false;
    KAN_UML_EVENT_FETCH (hit_box_inserted_event, kan_ui_node_hit_box_on_insert_event_t)
    {
        KAN_UMI_VALUE_READ_OPTIONAL (hit_box, kan_ui_node_hit_box_t, id, &hit_box_inserted_event->id)
        if (!hit_box)
        {
            continue;
        }

        KAN_UMI_VALUE_UPDATE_OPTIONAL (node, kan_ui_node_t, id, &hit_box_inserted_event->id)
        if (node)
        {
            node->event_on_laid_out = true;
            hit_boxes_changed = true;
        }

        // We have to initialize visuals.
        if (hit_box->interactable)
        {
            apply_hit_box_interaction_visuals (state, public, bundle, hit_box, false);
        }
    }

    return hit_boxes_changed;
}

static void process_scroll_behavior_insertion (struct ui_controls_input_state_t *state,
                                               const struct kan_ui_singleton_t *ui)
{
    KAN_UML_EVENT_FETCH (scroll_behavior_inserted_event, kan_ui_node_scroll_behavior_on_insert_event_t)
    {
        KAN_UMI_VALUE_READ_OPTIONAL (scroll_behavior, kan_ui_node_scroll_behavior_t, id,
                                     &scroll_behavior_inserted_event->id)

        if (!scroll_behavior)
        {
            continue;
        }

        KAN_UMI_VALUE_UPDATE_REQUIRED (main_node, kan_ui_node_t, id, &scroll_behavior->id)
        main_node->event_on_laid_out = true;
        ensure_scroll_is_in_limits (state, ui, scroll_behavior);

        if (scroll_behavior->horizontal && KAN_TYPED_ID_32_IS_VALID (scroll_behavior->horizontal_line_id))
        {
            KAN_UMO_INDEXED_INSERT (line_state, kan_ui_node_scroll_line_state_t)
            {
                line_state->id = scroll_behavior->horizontal_line_id;
                line_state->behavior_id = scroll_behavior->id;
                line_state->knob_id = scroll_behavior->horizontal_knob_id;
                line_state->class = KAN_UI_NODE_SCROLL_LINE_CLASS_HORIZONTAL;

                if (scroll_behavior->lines_always_visible)
                {
                    line_state->visible_until_s = FLT_MAX;
                }
                else
                {
                    line_state->visible_until_s = 0.0f;
                    hide_scroll_line (state, line_state);
                }
            }
        }

        if (scroll_behavior->vertical && KAN_TYPED_ID_32_IS_VALID (scroll_behavior->vertical_line_id))
        {
            KAN_UMO_INDEXED_INSERT (line_state, kan_ui_node_scroll_line_state_t)
            {
                line_state->id = scroll_behavior->vertical_line_id;
                line_state->behavior_id = scroll_behavior->id;
                line_state->knob_id = scroll_behavior->vertical_knob_id;
                line_state->class = KAN_UI_NODE_SCROLL_LINE_CLASS_VERTICAL;

                if (scroll_behavior->lines_always_visible)
                {
                    line_state->visible_until_s = FLT_MAX;
                }
                else
                {
                    line_state->visible_until_s = 0.0f;
                    hide_scroll_line (state, line_state);
                }
            }
        }
    }
}

static void process_line_edit_behavior_insertion (struct ui_controls_input_state_t *state)
{
    KAN_UML_EVENT_FETCH (line_edit_behavior_inserted_event, kan_ui_node_line_edit_behavior_on_insert_event_t)
    {
        KAN_UMI_VALUE_READ_OPTIONAL (behavior, kan_ui_node_line_edit_behavior_t, id,
                                     &line_edit_behavior_inserted_event->id)

        if (!behavior)
        {
            continue;
        }

        KAN_UMI_VALUE_UPDATE_REQUIRED (hit_box, kan_ui_node_hit_box_t, id, &behavior->id)
        hit_box->interactable_style = behavior->interactable_style_regular;

        KAN_UMI_VALUE_UPDATE_REQUIRED (text_node, kan_ui_node_t, id, &behavior->text_id)
        text_node->event_on_laid_out = true;

#if defined(KAN_WITH_ASSERT)
        KAN_UMI_VALUE_READ_REQUIRED (text_behavior, kan_ui_node_text_behavior_t, id, &behavior->text_id)
        KAN_ASSERT (KAN_TYPED_ID_32_IS_EQUAL (text_behavior->shaping_unit_id, behavior->shaping_unit_id))
#endif

        KAN_UMI_VALUE_UPDATE_REQUIRED (shaping_unit, kan_text_shaping_unit_t, id, &behavior->shaping_unit_id)
        shaping_unit->request.allow_breaks = false;
        shaping_unit->request.generate_edition_markup = true;
        shaping_unit->dirty = true;
        shaping_unit->stable = true;
    }
}

static void process_popup_behavior_insertion (struct ui_controls_input_state_t *state)
{
    KAN_UML_EVENT_FETCH (popup_behavior_inserted_event, kan_ui_node_popup_behavior_on_insert_event_t)
    {
        KAN_UMI_VALUE_READ_OPTIONAL (behavior, kan_ui_node_popup_behavior_t, id, &popup_behavior_inserted_event->id)
        if (!behavior)
        {
            continue;
        }

        KAN_UMI_VALUE_UPDATE_REQUIRED (node, kan_ui_node_t, id, &popup_behavior_inserted_event->id)
        node->render.hidden = true;
    }
}

static void clear_line_edit_selection_visuals (struct ui_controls_input_state_t *state,
                                               struct kan_ui_node_line_edit_behavior_t *behavior)
{
    behavior->cursor_content_location = KAN_INT_MAX (kan_instance_size_t);
    behavior->selection_content_min = KAN_INT_MAX (kan_instance_size_t);
    behavior->selection_content_max = KAN_INT_MAX (kan_instance_size_t);
    behavior->text_visuals_dirty = true;

    KAN_UMI_VALUE_UPDATE_REQUIRED (hit_box, kan_ui_node_hit_box_t, id, &behavior->id)
    hit_box->interactable_style = behavior->interactable_style_regular;
}

static void line_edit_sanitize_content_on_node_deselection (struct kan_ui_node_line_edit_behavior_t *behavior)
{
    switch (behavior->content_type)
    {
    case KAN_UI_NODE_LINE_EDIT_CONTENT_TYPE_ANY:
        break;

    case KAN_UI_NODE_LINE_EDIT_CONTENT_TYPE_UINT:
    case KAN_UI_NODE_LINE_EDIT_CONTENT_TYPE_SINT:
    case KAN_UI_NODE_LINE_EDIT_CONTENT_TYPE_FLOAT:
        // Size is 1 for zero terminator.
        if (behavior->content_utf8.size <= 1u)
        {
            // Do not leave numeric content at "unparseable empty string" state, add zero.
            kan_ui_node_line_edit_behavior_set_content (behavior, "0", behavior->content_style, behavior->content_mark);
        }

        break;
    }
}

static inline void deselect_input_receiver_behavior (struct ui_controls_input_state_t *state,
                                                     struct kan_ui_input_singleton_t *public,
                                                     struct ui_controls_input_private_singleton_t *private)
{
    KAN_UMI_VALUE_UPDATE_OPTIONAL (line_edit_behavior, kan_ui_node_line_edit_behavior_t, id, &public->input_receiver_id)
    if (line_edit_behavior)
    {
        clear_line_edit_selection_visuals (state, line_edit_behavior);
        line_edit_sanitize_content_on_node_deselection (line_edit_behavior);
    }

    if (private->input_receiver_requested_text_input && KAN_HANDLE_IS_VALID (public->linked_window_handle))
    {
        kan_application_window_remove_text_listener (state->application_system_handle, public->linked_window_handle);
    }

    public->input_receiver_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    private->input_receiver_requested_text_input = false;
}

static void sanitize_input_receiver_selection (struct ui_controls_input_state_t *state,
                                               struct kan_ui_input_singleton_t *public)
{
    KAN_UMI_SINGLETON_WRITE (private, ui_controls_input_private_singleton_t)
    if (KAN_TYPED_ID_32_IS_VALID (public->input_receiver_id))
    {
        KAN_UMI_VALUE_READ_OPTIONAL (node, kan_ui_node_t, id, &public->input_receiver_id)
        if (!node)
        {
            deselect_input_receiver_behavior (state, public, private);
            return;
        }

        KAN_UMI_VALUE_READ_OPTIONAL (drawable, kan_ui_node_drawable_t, id, &public->input_receiver_id)
        if (!drawable || drawable->hidden_permanently || drawable->hidden_temporary)
        {
            deselect_input_receiver_behavior (state, public, private);
            return;
        }

        KAN_UMI_VALUE_READ_OPTIONAL (hit_box, kan_ui_node_hit_box_t, id, &public->input_receiver_id)
        if (hit_box && hit_box->disabled)
        {
            deselect_input_receiver_behavior (state, public, private);
            return;
        }
    }
}

static void process_line_edit_content_dirty_outer (struct ui_controls_input_state_t *state,
                                                   struct kan_ui_input_singleton_t *public)
{
    // If we got here, then line edit content was changed from outside.
    // It means that we should reset anything related to the edition, including line edit element selection.
    // Dirty flag is not cleared as we also need to apply regular on-content-dirty routine.
    KAN_UMI_SINGLETON_WRITE (private, ui_controls_input_private_singleton_t)

    KAN_UML_SIGNAL_UPDATE (behavior, kan_ui_node_line_edit_behavior_t, content_dirty, true)
    {
        behavior->cursor_content_location = KAN_INT_MAX (kan_instance_size_t);
        behavior->selection_content_min = KAN_INT_MAX (kan_instance_size_t);
        behavior->selection_content_max = KAN_INT_MAX (kan_instance_size_t);

        if (KAN_TYPED_ID_32_IS_EQUAL (public->input_receiver_id, behavior->id))
        {
            public->input_receiver_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
            clear_line_edit_selection_visuals (state, behavior);
            line_edit_sanitize_content_on_node_deselection (behavior);
        }
    }
}

static void line_edit_erase_subsequence (struct kan_ui_node_line_edit_behavior_t *behavior,
                                         kan_instance_size_t min,
                                         kan_instance_size_t max)
{
    if (min == max)
    {
        return;
    }

    KAN_ASSERT (min < max)
    KAN_ASSERT (max < behavior->content_utf8.size)
    const kan_instance_size_t size = max - min;

    // If not full erase, move data.
    if (behavior->content_utf8.size - 1u != size)
    {
        for (kan_memory_size_t index = min; index < behavior->content_utf8.size - size - 1u; ++index)
        {
            if (index + size < behavior->content_utf8.size - 1u)
            {
                behavior->content_utf8.data[index] = behavior->content_utf8.data[index + size];
            }
        }
    }

    behavior->content_utf8.size -= size;
    behavior->content_utf8.data[behavior->content_utf8.size - 1u] = '\0';
    behavior->content_dirty = true;
}

static inline void line_edit_erase_selection (struct kan_ui_node_line_edit_behavior_t *behavior)
{
    line_edit_erase_subsequence (behavior, behavior->selection_content_min, behavior->selection_content_max);
    behavior->cursor_content_location = behavior->selection_content_min;
    behavior->selection_content_min = KAN_INT_MAX (kan_instance_size_t);
    behavior->selection_content_max = KAN_INT_MAX (kan_instance_size_t);
    behavior->content_dirty = true;
}

static void line_edit_paste_text (struct kan_ui_node_line_edit_behavior_t *behavior, const char *text)
{
    if (!text)
    {
        return;
    }

    if (behavior->selection_content_min != KAN_INT_MAX (kan_instance_size_t) &&
        behavior->selection_content_max != KAN_INT_MAX (kan_instance_size_t))
    {
        line_edit_erase_selection (behavior);
    }

    if (behavior->cursor_content_location == KAN_INT_MAX (kan_instance_size_t))
    {
        // Just a failsafe, should never happen normally.
        behavior->cursor_content_location = behavior->content_utf8.size - 1u;
    }

    const uint8_t *text_utf8 = (uint8_t *) text;
    const kan_instance_size_t input_length = (kan_instance_size_t) strlen (text);

    const uint8_t *text_iterator = text_utf8;
    kan_instance_size_t filtered_size = 0u;

    // Currently, we only ignore ASCII control characters.
#define IS_CODEPOINT_IGNORED(CODEPOINT) ((CODEPOINT) < 32u)

    while (true)
    {
        kan_unicode_codepoint_t codepoint = 0u;
        const kan_instance_size_t offset_before = (kan_instance_size_t) (text_iterator - text_utf8);

        if (!(codepoint = kan_text_utf8_next (&text_iterator, text_utf8 + input_length)))
        {
            break;
        }

        if (IS_CODEPOINT_IGNORED (codepoint))
        {
            continue;
        }

        const kan_instance_size_t offset_after = (kan_instance_size_t) (text_iterator - text_utf8);
        filtered_size += offset_after - offset_before;
    }

    if (behavior->content_utf8.size + filtered_size > behavior->content_utf8.capacity)
    {
        kan_dynamic_array_set_capacity (&behavior->content_utf8, KAN_MAX (behavior->content_utf8.size * 2u,
                                                                          behavior->content_utf8.size + filtered_size));
    }

    for (kan_memory_size_t index = behavior->content_utf8.size - 1u;
         index >= behavior->cursor_content_location && index != KAN_INT_MAX (kan_memory_size_t); --index)
    {
        behavior->content_utf8.data[index + filtered_size] = behavior->content_utf8.data[index];
    }

    text_iterator = text_utf8;
    while (true)
    {
        kan_unicode_codepoint_t codepoint = 0u;
        const kan_instance_size_t offset_before = (kan_instance_size_t) (text_iterator - text_utf8);

        if (!(codepoint = kan_text_utf8_next (&text_iterator, text_utf8 + input_length)))
        {
            break;
        }

        if (IS_CODEPOINT_IGNORED (codepoint))
        {
            continue;
        }

        const kan_instance_size_t offset_after = (kan_instance_size_t) (text_iterator - text_utf8);
        switch (offset_after - offset_before)
        {
        case 4u:
            behavior->content_utf8.data[behavior->cursor_content_location + 3u] = text_utf8[offset_before + 3u];

        case 3u:
            behavior->content_utf8.data[behavior->cursor_content_location + 2u] = text_utf8[offset_before + 2u];

        case 2u:
            behavior->content_utf8.data[behavior->cursor_content_location + 1u] = text_utf8[offset_before + 1u];

        case 1u:
            behavior->content_utf8.data[behavior->cursor_content_location] = text_utf8[offset_before];
            break;

        default:
            KAN_ASSERT (false);
            break;
        }

        behavior->cursor_content_location += offset_after - offset_before;
    }

#undef IS_CODEPOINT_IGNORED
    behavior->content_utf8.size += filtered_size;
    behavior->content_utf8.data[behavior->content_utf8.size - 1u] = '\0';
    behavior->content_dirty = true;
}

static void line_edit_process_horizontal_arrow (struct kan_ui_node_line_edit_behavior_t *behavior,
                                                struct kan_text_shaping_unit_t *shaping_unit,
                                                bool positive_direction)
{
    if (behavior->selection_content_min != KAN_INT_MAX (kan_instance_size_t) &&
        behavior->selection_content_max != KAN_INT_MAX (kan_instance_size_t))
    {
        behavior->cursor_content_location =
            positive_direction ? behavior->selection_content_max : behavior->selection_content_min;

        behavior->selection_content_min = KAN_INT_MAX (kan_instance_size_t);
        behavior->selection_content_max = KAN_INT_MAX (kan_instance_size_t);
        return;
    }

    for (kan_memory_size_t sequence_index = 0u; sequence_index < shaping_unit->shaped_edition_sequences.size;
         ++sequence_index)
    {
        const struct kan_text_shaped_edition_sequence_data_t *sequence =
            &((struct kan_text_shaped_edition_sequence_data_t *)
                  shaping_unit->shaped_edition_sequences.data)[sequence_index];

        if (behavior->cursor_content_location == sequence->end_at_index)
        {
            if (positive_direction)
            {
                goto go_to_the_next_cluster_in_sequences_after;
            }
            else
            {
                if (sequence->clusters.size > 0u)
                {
                    behavior->cursor_content_location = ((struct kan_text_shaped_edition_cluster_data_t *)
                                                             sequence->clusters.data)[sequence->clusters.size - 1u]
                                                            .start_at_index;
                }
                else
                {
                    goto go_to_the_previous_cluster_in_sequences_before;
                }
            }

            return;
        }

        for (kan_memory_size_t cluster_index = 0u; cluster_index < sequence->clusters.size; ++cluster_index)
        {
            const struct kan_text_shaped_edition_cluster_data_t *cluster =
                &((struct kan_text_shaped_edition_cluster_data_t *) sequence->clusters.data)[cluster_index];

            if (cluster->start_at_index == behavior->cursor_content_location)
            {
                if (positive_direction)
                {
                    if (cluster_index + 1u < sequence->clusters.size)
                    {
                        behavior->cursor_content_location = ((struct kan_text_shaped_edition_cluster_data_t *)
                                                                 sequence->clusters.data)[cluster_index + 1u]
                                                                .start_at_index;
                    }
                    else
                    {
                        goto go_to_the_next_cluster_in_sequences_after;
                    }
                }
                else
                {
                    if (cluster_index > 0u)
                    {
                        behavior->cursor_content_location = ((struct kan_text_shaped_edition_cluster_data_t *)
                                                                 sequence->clusters.data)[cluster_index - 1u]
                                                                .start_at_index;
                    }
                    else
                    {
                        goto go_to_the_previous_cluster_in_sequences_before;
                    }
                }

                return;
            }
        }

        continue;

    go_to_the_next_cluster_in_sequences_after:
    {
        ++sequence_index;
        while (sequence_index < shaping_unit->shaped_edition_sequences.size)
        {
            sequence = &((struct kan_text_shaped_edition_sequence_data_t *)
                             shaping_unit->shaped_edition_sequences.data)[sequence_index];

            if (sequence->clusters.size > 0u)
            {
                behavior->cursor_content_location =
                    ((struct kan_text_shaped_edition_cluster_data_t *) sequence->clusters.data)[0u].start_at_index;
                return;
            }

            ++sequence_index;
        }

        // Failsafe.
        behavior->cursor_content_location = behavior->content_utf8.size - 1u;
        return;
    }

    go_to_the_previous_cluster_in_sequences_before:
    {
        --sequence_index;
        while (sequence_index != KAN_INT_MAX (kan_memory_size_t))
        {
            sequence = &((struct kan_text_shaped_edition_sequence_data_t *)
                             shaping_unit->shaped_edition_sequences.data)[sequence_index];

            if (sequence->clusters.size > 0u)
            {
                behavior->cursor_content_location = ((struct kan_text_shaped_edition_cluster_data_t *)
                                                         sequence->clusters.data)[sequence->clusters.size - 1u]
                                                        .start_at_index;
                return;
            }

            --sequence_index;
        }

        // Failsafe.
        behavior->cursor_content_location = 0u;
        return;
    }
    }

    // Failsafe.
    if (positive_direction)
    {
        behavior->cursor_content_location = behavior->content_utf8.size - 1u;
    }
    else
    {
        behavior->cursor_content_location = 0u;
    }
}

static void prolong_hit_box_down_visuals (struct ui_controls_input_state_t *state,
                                          struct kan_ui_input_singleton_t *public,
                                          const struct kan_ui_bundle_singleton_t *bundle,
                                          const struct kan_ui_node_hit_box_t *hit_box)
{
    if (!hit_box->interactable_style)
    {
        return;
    }

    const struct kan_resource_ui_hit_box_interaction_style_t *selected_style =
        find_interactable_style (bundle, hit_box->interactable_style);

    if (!selected_style)
    {
        // No style -> no animation.
        return;
    }

    KAN_UMI_SINGLETON_READ (ui, kan_ui_singleton_t)
    KAN_UMI_VALUE_UPDATE_OPTIONAL (existent_mark, kan_ui_node_down_mark_t, id, &hit_box->id)

    if (existent_mark)
    {
        existent_mark->down_until_s = ui->animation_global_time_s + selected_style->down_state_s;
        // Already had mark, no visual update needed.
        return;
    }

    KAN_UMO_INDEXED_INSERT (new_mark, kan_ui_node_down_mark_t)
    {
        new_mark->id = hit_box->id;
        new_mark->down_until_s = ui->animation_global_time_s + selected_style->down_state_s;
        new_mark->down_from_key = KAN_PLATFORM_SCAN_CODE_UNKNOWN;
    }

    apply_hit_box_interaction_visuals (state, public, bundle, hit_box, false);
}

static void on_press_begin_internal (struct ui_controls_input_state_t *state,
                                     struct kan_ui_input_singleton_t *public,
                                     const struct kan_ui_singleton_t *ui,
                                     kan_ui_node_id_t press_id);

static void simulate_press_begin_from_key_binding (struct ui_controls_input_state_t *state,
                                                   struct kan_ui_input_singleton_t *public,
                                                   const struct kan_ui_singleton_t *ui,
                                                   const struct kan_ui_bundle_singleton_t *bundle,
                                                   const struct kan_platform_application_event_t *event)
{
    struct kan_repository_indexed_value_read_access_t access;
    const struct kan_ui_node_hit_box_t *hit_box =
        find_hit_box_from_key_binding (state, event->keyboard.scan_code, &access);

    const kan_ui_node_id_t hit_box_id = hit_box ? hit_box->id : KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    const bool filtered = hit_box && hit_box->interactable && !hit_box->disabled;

    if (filtered)
    {
        KAN_UMO_EVENT_INSERT_INIT (kan_ui_press_begin_t) {
            .node_id = hit_box->id,
            .mouse_button_down_flags = 0u,
            .at_x = public->last_mouse_x,
            .at_y = public->last_mouse_y,
        };

        KAN_UMI_VALUE_UPDATE_OPTIONAL (existent_mark, kan_ui_node_down_mark_t, id, &hit_box->id)
        if (existent_mark)
        {
            existent_mark->down_from_key = event->keyboard.scan_code;
        }
        else
        {
            KAN_UMO_INDEXED_INSERT (new_mark, kan_ui_node_down_mark_t)
            {
                new_mark->id = hit_box->id;
                new_mark->down_until_s = 0.0f;
                new_mark->down_from_key = event->keyboard.scan_code;
            }

            apply_hit_box_interaction_visuals (state, public, bundle, hit_box, false);
        }
    }

    if (hit_box)
    {
        kan_repository_indexed_value_read_access_close (&access);
    }

    if (filtered)
    {
        on_press_begin_internal (state, public, ui, hit_box_id);
    }
}

static void process_key_down_internal (struct ui_controls_input_state_t *state,
                                       struct kan_ui_input_singleton_t *public,
                                       const struct kan_ui_singleton_t *ui,
                                       const struct kan_ui_bundle_singleton_t *bundle,
                                       const struct kan_platform_application_event_t *event)
{
    if (!KAN_TYPED_ID_32_IS_VALID (public->input_receiver_id))
    {
        simulate_press_begin_from_key_binding (state, public, ui, bundle, event);
        return;
    }

    // We do not process keyboard input for input receivers while mouse is down.
    if (public->mouse_button_down_flags != 0u)
    {
        return;
    }

    KAN_UMI_SINGLETON_WRITE (private, ui_controls_input_private_singleton_t)
    KAN_UMI_VALUE_UPDATE_OPTIONAL (line_edit_behavior, kan_ui_node_line_edit_behavior_t, id, &public->input_receiver_id)

    if (line_edit_behavior)
    {
        switch (event->keyboard.scan_code)
        {
        case KAN_PLATFORM_SCAN_CODE_LEFT:
        case KAN_PLATFORM_SCAN_CODE_RIGHT:
        {
            KAN_UMI_VALUE_UPDATE_OPTIONAL (shaping_unit, kan_text_shaping_unit_t, id,
                                           &line_edit_behavior->shaping_unit_id)

            if (!shaping_unit || !shaping_unit->shaped || shaping_unit->dirty ||
                shaping_unit->shaped_edition_sequences.size == 0u)
            {
                break;
            }

            const bool forward_base_direction =
                shaping_unit->request.reading_direction == KAN_TEXT_READING_DIRECTION_LEFT_TO_RIGHT;

            const bool positive_direction = forward_base_direction ?
                                                event->keyboard.scan_code == KAN_PLATFORM_SCAN_CODE_RIGHT :
                                                event->keyboard.scan_code == KAN_PLATFORM_SCAN_CODE_LEFT;

            line_edit_process_horizontal_arrow (line_edit_behavior, shaping_unit, positive_direction);
            line_edit_behavior->text_visuals_dirty = true;
            break;
        }

        case KAN_PLATFORM_SCAN_CODE_BACKSPACE:
        case KAN_PLATFORM_SCAN_CODE_DELETE:
            if (line_edit_behavior->selection_content_min != KAN_INT_MAX (kan_instance_size_t) &&
                line_edit_behavior->selection_content_max != KAN_INT_MAX (kan_instance_size_t))
            {
                line_edit_erase_selection (line_edit_behavior);
            }
            else if (line_edit_behavior->cursor_content_location != KAN_INT_MAX (kan_instance_size_t))
            {
                if (event->keyboard.scan_code == KAN_PLATFORM_SCAN_CODE_BACKSPACE)
                {
                    const uint8_t *previous_position = kan_text_utf8_find_previous (
                        line_edit_behavior->content_utf8.data + line_edit_behavior->cursor_content_location,
                        line_edit_behavior->content_utf8.data);

                    if (previous_position)
                    {
                        const kan_instance_size_t previous_at =
                            (kan_instance_size_t) (previous_position - line_edit_behavior->content_utf8.data);

                        line_edit_erase_subsequence (line_edit_behavior, previous_at,
                                                     line_edit_behavior->cursor_content_location);
                        line_edit_behavior->cursor_content_location = previous_at;
                    }
                }
                else
                {
                    const uint8_t *iterator =
                        line_edit_behavior->content_utf8.data + line_edit_behavior->cursor_content_location;

                    if (kan_text_utf8_next (&iterator, line_edit_behavior->content_utf8.data +
                                                           line_edit_behavior->content_utf8.size - 1u))
                    {
                        const kan_instance_size_t next_at =
                            (kan_instance_size_t) (iterator - line_edit_behavior->content_utf8.data);
                        line_edit_erase_subsequence (line_edit_behavior, line_edit_behavior->cursor_content_location,
                                                     next_at);
                    }
                }
            }

            break;

        case KAN_PLATFORM_SCAN_CODE_C:
            if (event->keyboard.modifiers & KAN_PLATFORM_MODIFIER_MASK_ANY_CONTROL)
            {
                if (line_edit_behavior->selection_content_min != KAN_INT_MAX (kan_instance_size_t) &&
                    line_edit_behavior->selection_content_max != KAN_INT_MAX (kan_instance_size_t))
                {
                    kan_application_system_clipboard_set_text_sequence (
                        state->application_system_handle,
                        (char *) (line_edit_behavior->content_utf8.data + line_edit_behavior->selection_content_min),
                        (char *) (line_edit_behavior->content_utf8.data + line_edit_behavior->selection_content_max));
                }
            }

            break;

        case KAN_PLATFORM_SCAN_CODE_V:
            if (event->keyboard.modifiers & KAN_PLATFORM_MODIFIER_MASK_ANY_CONTROL)
            {
                line_edit_paste_text (line_edit_behavior,
                                      kan_application_system_clipboard_get_text (state->application_system_handle));
            }

            break;

        default:
            // Do not care for other keys, we process text input events as well.
            break;
        }
    }
}

static void on_press_end_internal (struct ui_controls_input_state_t *state,
                                   struct kan_ui_input_singleton_t *public,
                                   const struct kan_ui_singleton_t *ui,
                                   kan_ui_node_id_t press_id,
                                   bool continuous);

static void simulate_press_end_from_key_binding (struct ui_controls_input_state_t *state,
                                                 struct kan_ui_input_singleton_t *public,
                                                 const struct kan_ui_singleton_t *ui,
                                                 const struct kan_ui_bundle_singleton_t *bundle,
                                                 const struct kan_platform_application_event_t *event)
{
    KAN_UML_SEQUENCE_UPDATE (down_mark, kan_ui_node_down_mark_t)
    {
        if (down_mark->down_from_key != event->keyboard.scan_code)
        {
            continue;
        }

        down_mark->down_from_key = KAN_PLATFORM_SCAN_CODE_UNKNOWN;
        KAN_UMI_VALUE_READ_OPTIONAL (hit_box, kan_ui_node_hit_box_t, id, &down_mark->id)

        if (hit_box)
        {
            KAN_UMO_EVENT_INSERT_INIT (kan_ui_press_end_t) {
                .node_id = hit_box->id,
                .mouse_button_down_inclusive_flags = 0u,
                .continuous_press = true,
                .at_x = public->last_mouse_x,
                .at_y = public->last_mouse_y,
            };

            const struct kan_resource_ui_hit_box_interaction_style_t *selected_style =
                find_interactable_style (bundle, hit_box->interactable_style);

            if (selected_style)
            {
                down_mark->down_until_s = ui->animation_global_time_s + selected_style->down_state_s;
            }

            KAN_UM_ACCESS_CLOSE_IMMEDIATELY (hit_box);
            on_press_end_internal (state, public, ui, down_mark->id, true);
        }
    }
}

static void process_key_up_internal (struct ui_controls_input_state_t *state,
                                     struct kan_ui_input_singleton_t *public,
                                     const struct kan_ui_singleton_t *ui,
                                     const struct kan_ui_bundle_singleton_t *bundle,
                                     const struct kan_platform_application_event_t *event)
{
    if (!KAN_TYPED_ID_32_IS_VALID (public->input_receiver_id))
    {
        simulate_press_end_from_key_binding (state, public, ui, bundle, event);
        return;
    }

    // We do not process keyboard input for input receivers while mouse is down.
    if (public->mouse_button_down_flags != 0u)
    {
        return;
    }
}

static void process_text_input_internal (struct ui_controls_input_state_t *state,
                                         struct kan_ui_input_singleton_t *public,
                                         const struct kan_ui_singleton_t *ui,
                                         const struct kan_platform_application_event_t *event)
{
    KAN_UMI_SINGLETON_WRITE (private, ui_controls_input_private_singleton_t)
    if (!KAN_TYPED_ID_32_IS_VALID (public->input_receiver_id) || !private->input_receiver_requested_text_input ||
        // We do not process text input while mouse is down.
        public->mouse_button_down_flags != 0u)
    {
        return;
    }

    KAN_UMI_VALUE_UPDATE_OPTIONAL (line_edit_behavior, kan_ui_node_line_edit_behavior_t, id, &public->input_receiver_id)
    if (line_edit_behavior)
    {
        line_edit_paste_text (line_edit_behavior, event->text_input.text);
    }
}

static void apply_scroll_relative_input (struct ui_controls_input_state_t *state,
                                         struct kan_ui_input_singleton_t *public,
                                         const struct kan_ui_singleton_t *ui,
                                         const struct kan_ui_node_scroll_behavior_t *behavior,
                                         kan_floating_t delta_x_px,
                                         kan_floating_t delta_y_px,
                                         bool allow_y_to_x)
{
    KAN_UMI_VALUE_READ_OPTIONAL (main_drawable, kan_ui_node_drawable_t, id, &behavior->id)
    KAN_UMI_VALUE_UPDATE_REQUIRED (container_node, kan_ui_node_t, id, &behavior->container_id)
    KAN_UMI_VALUE_READ_OPTIONAL (container_drawable, kan_ui_node_drawable_t, id, &behavior->container_id)

    if (!main_drawable || !container_drawable)
    {
        return;
    }

    // Redirect input Y to input X if there is no X input, but user might expect Y to X redirection.
    if (allow_y_to_x && KAN_FLOATING_IS_NEAR (delta_x_px, 0.0f) && behavior->horizontal && !behavior->vertical)
    {
        delta_x_px = delta_y_px;
    }

    if (behavior->horizontal && !KAN_FLOATING_IS_NEAR (delta_x_px, 0.0f))
    {
        const kan_floating_t current_scroll_px =
            kan_ui_calculate_coordinate_floating (ui, container_node->render.scroll_x);
        const kan_floating_t new_scroll_px =
            KAN_CLAMP (current_scroll_px + delta_x_px, 0.0f,
                       (kan_floating_t) (KAN_MAX (0, container_drawable->width - main_drawable->width)));

        container_node->render.scroll_x =
            kan_ui_coordinate_from_pixels (ui, behavior->offset_coordinate_type_x, new_scroll_px);
        update_scroll_horizontal_knob (state, behavior, main_drawable, container_drawable, new_scroll_px);

        if (KAN_TYPED_ID_32_IS_VALID (behavior->horizontal_line_id))
        {
            KAN_UMI_VALUE_UPDATE_REQUIRED (line_state, kan_ui_node_scroll_line_state_t, id,
                                           &behavior->horizontal_line_id)
            update_interacted_scroll_line_visibility (state, public, line_state, behavior, false);
        }
    }

    if (behavior->vertical && !KAN_FLOATING_IS_NEAR (delta_y_px, 0.0f))
    {
        const kan_floating_t current_scroll_px =
            kan_ui_calculate_coordinate_floating (ui, container_node->render.scroll_y);
        const kan_floating_t new_scroll_px =
            KAN_CLAMP (current_scroll_px + delta_y_px, 0.0f,
                       (kan_floating_t) (KAN_MAX (0, container_drawable->height - main_drawable->height)));

        container_node->render.scroll_y =
            kan_ui_coordinate_from_pixels (ui, behavior->offset_coordinate_type_y, new_scroll_px);
        update_scroll_vertical_knob (state, behavior, main_drawable, container_drawable, new_scroll_px);

        if (KAN_TYPED_ID_32_IS_VALID (behavior->vertical_line_id))
        {
            KAN_UMI_VALUE_UPDATE_REQUIRED (line_state, kan_ui_node_scroll_line_state_t, id, &behavior->vertical_line_id)
            update_interacted_scroll_line_visibility (state, public, line_state, behavior, false);
        }
    }
}

static void apply_scroll_absolute_horizontal (struct ui_controls_input_state_t *state,
                                              struct kan_ui_input_singleton_t *public,
                                              const struct kan_ui_singleton_t *ui,
                                              const struct kan_ui_node_scroll_behavior_t *behavior,
                                              struct kan_ui_node_scroll_line_state_t *line_state,
                                              kan_floating_t value_px)
{
    KAN_UMI_VALUE_READ_OPTIONAL (main_drawable, kan_ui_node_drawable_t, id, &behavior->id)
    KAN_UMI_VALUE_UPDATE_REQUIRED (container_node, kan_ui_node_t, id, &behavior->container_id)
    KAN_UMI_VALUE_READ_OPTIONAL (container_drawable, kan_ui_node_drawable_t, id, &behavior->container_id)

    if (!main_drawable || !container_drawable)
    {
        return;
    }

    if (behavior->horizontal)
    {
        const kan_floating_t new_scroll_px = KAN_CLAMP (
            value_px, 0.0f, (kan_floating_t) (KAN_MAX (0, container_drawable->width - main_drawable->width)));

        container_node->render.scroll_x =
            kan_ui_coordinate_from_pixels (ui, behavior->offset_coordinate_type_x, new_scroll_px);

        update_scroll_horizontal_knob (state, behavior, main_drawable, container_drawable, new_scroll_px);
        update_interacted_scroll_line_visibility (state, public, line_state, behavior, false);
    }
}

static void apply_scroll_absolute_vertical (struct ui_controls_input_state_t *state,
                                            struct kan_ui_input_singleton_t *public,
                                            const struct kan_ui_singleton_t *ui,
                                            const struct kan_ui_node_scroll_behavior_t *behavior,
                                            struct kan_ui_node_scroll_line_state_t *line_state,
                                            kan_floating_t value_px)
{
    KAN_UMI_VALUE_READ_OPTIONAL (main_drawable, kan_ui_node_drawable_t, id, &behavior->id)
    KAN_UMI_VALUE_UPDATE_REQUIRED (container_node, kan_ui_node_t, id, &behavior->container_id)
    KAN_UMI_VALUE_READ_OPTIONAL (container_drawable, kan_ui_node_drawable_t, id, &behavior->container_id)

    if (!main_drawable || !container_drawable)
    {
        return;
    }

    if (behavior->vertical)
    {
        const kan_floating_t new_scroll_px = KAN_CLAMP (
            value_px, 0.0f, (kan_floating_t) (KAN_MAX (0, container_drawable->height - main_drawable->height)));

        container_node->render.scroll_y =
            kan_ui_coordinate_from_pixels (ui, behavior->offset_coordinate_type_y, new_scroll_px);

        update_scroll_vertical_knob (state, behavior, main_drawable, container_drawable, new_scroll_px);
        update_interacted_scroll_line_visibility (state, public, line_state, behavior, false);
    }
}

static void place_scroll_line_knob_at_press (struct ui_controls_input_state_t *state,
                                             struct kan_ui_input_singleton_t *public,
                                             struct ui_controls_input_private_singleton_t *private,
                                             const struct kan_ui_singleton_t *ui,
                                             struct kan_ui_node_scroll_line_state_t *line_state)
{
    KAN_UMI_VALUE_READ_REQUIRED (behavior, kan_ui_node_scroll_behavior_t, id, &line_state->behavior_id)
    KAN_UMI_VALUE_READ_OPTIONAL (container_drawable, kan_ui_node_drawable_t, id, &behavior->container_id)
    KAN_UMI_VALUE_READ_OPTIONAL (line_drawable, kan_ui_node_drawable_t, id, &line_state->id)

    if (line_drawable)
    {
        switch (line_state->class)
        {
        case KAN_UI_NODE_SCROLL_LINE_CLASS_HORIZONTAL:
        {
            const kan_instance_offset_t local_offset =
                KAN_MAX (0, public->last_mouse_x - line_drawable->global_x - private->press_knob_offset);
            const kan_floating_t target_px = (kan_floating_t) local_offset *
                                             (kan_floating_t) container_drawable->width /
                                             (kan_floating_t) line_drawable->width;

            // Due to access from inside interacted visibility update, a little bit ad-hoc.
            KAN_UM_ACCESS_CLOSE_IMMEDIATELY (line_drawable);
            apply_scroll_absolute_horizontal (state, public, ui, behavior, line_state, target_px);
            break;
        }

        case KAN_UI_NODE_SCROLL_LINE_CLASS_VERTICAL:
        {
            const kan_instance_offset_t local_offset =
                KAN_MAX (0, public->last_mouse_y - line_drawable->global_y - private->press_knob_offset);
            const kan_floating_t target_px = (kan_floating_t) local_offset *
                                             (kan_floating_t) container_drawable->height /
                                             (kan_floating_t) line_drawable->height;

            // Due to access from inside interacted visibility update, a little bit ad-hoc.
            KAN_UM_ACCESS_CLOSE_IMMEDIATELY (line_drawable);
            apply_scroll_absolute_vertical (state, public, ui, behavior, line_state, target_px);
            break;
        }
        }
    }
}

static kan_instance_size_t calculate_content_position_on_shaped_text (struct ui_controls_input_state_t *state,
                                                                      kan_ui_node_id_t container_node_id,
                                                                      kan_text_shaping_unit_id_t shaping_unit_id,
                                                                      kan_instance_offset_t global_x,
                                                                      kan_instance_offset_t global_y)
{
    KAN_UMI_VALUE_READ_OPTIONAL (drawable, kan_ui_node_drawable_t, id, &container_node_id)
    KAN_UMI_VALUE_UPDATE_OPTIONAL (shaping_unit, kan_text_shaping_unit_t, id, &shaping_unit_id)

    if (!drawable || !shaping_unit || !shaping_unit->shaped || shaping_unit->dirty ||
        shaping_unit->shaped_edition_sequences.size == 0u)
    {
        return KAN_INT_MAX (kan_instance_size_t);
    }

    const kan_instance_offset_t local_x = global_x - drawable->global_x - drawable->draw_offset_x;
    const kan_instance_offset_t local_y = global_y - drawable->global_y - drawable->draw_offset_y;

    kan_instance_offset_t primary_coordinate = 0;
    kan_instance_offset_t secondary_coordinate = 0;

    switch (shaping_unit->request.orientation)
    {
    case KAN_TEXT_ORIENTATION_HORIZONTAL:
        primary_coordinate = local_x;
        secondary_coordinate = local_y;
        break;

    case KAN_TEXT_ORIENTATION_VERTICAL:
        primary_coordinate = local_y;
        secondary_coordinate = local_x;
        break;
    }

    // Sequence selection grabs first sequence that is not higher that secondary coordinate or last available sequence.
    // Sequences without clusters are excluded. It is the most logical and useful sequence selection method as things
    // like mouse pointer can travel anywhere.
    const struct kan_text_shaped_edition_sequence_data_t *selected_sequence = NULL;

    for (kan_memory_size_t index = 0u; index < shaping_unit->shaped_edition_sequences.size; ++index)
    {
        const struct kan_text_shaped_edition_sequence_data_t *sequence =
            &((struct kan_text_shaped_edition_sequence_data_t *) shaping_unit->shaped_edition_sequences.data)[index];

        if (sequence->clusters.size == 0u)
        {
            continue;
        }

        selected_sequence = sequence;
        if (secondary_coordinate < sequence->baseline - sequence->descender)
        {
            break;
        }
    }

    if (!selected_sequence)
    {
        return KAN_INT_MAX (kan_instance_size_t);
    }

    const bool forward_base_direction =
        shaping_unit->request.reading_direction == KAN_TEXT_READING_DIRECTION_LEFT_TO_RIGHT;
    kan_instance_size_t cluster_index = 0u;

    while (cluster_index < selected_sequence->clusters.size)
    {
        const struct kan_text_shaped_edition_cluster_data_t *cluster =
            &((struct kan_text_shaped_edition_cluster_data_t *) selected_sequence->clusters.data)[cluster_index];

        // Separate out-of-order processing for bidi fragments.
        if (!cluster->matching_reading_direction)
        {
            kan_instance_size_t run_ends_at = selected_sequence->clusters.size;
            for (kan_instance_size_t scan_index = cluster_index + 1u; scan_index < selected_sequence->clusters.size;
                 ++scan_index)
            {
                const struct kan_text_shaped_edition_cluster_data_t *scan_cluster =
                    &((struct kan_text_shaped_edition_cluster_data_t *) selected_sequence->clusters.data)[scan_index];

                if (scan_cluster->matching_reading_direction)
                {
                    run_ends_at = scan_index;
                    break;
                }
            }

            for (kan_memory_size_t run_index = run_ends_at - 1u;
                 // Overflow guard is important here.
                 run_index >= cluster_index && run_index != KAN_INT_MAX (kan_memory_size_t); --run_index)
            {
                const struct kan_text_shaped_edition_cluster_data_t *run_cluster =
                    &((struct kan_text_shaped_edition_cluster_data_t *) selected_sequence->clusters.data)[run_index];

                const kan_instance_offset_t cluster_middle = (run_cluster->visual_min + run_cluster->visual_max) / 2;
                const bool inside =
                    primary_coordinate >= run_cluster->visual_min && primary_coordinate < run_cluster->visual_max;

                // Starting half is reversed as direction does not match.
                const bool starting_half = forward_base_direction ? primary_coordinate >= cluster_middle :
                                                                    primary_coordinate <= cluster_middle;

                if (starting_half)
                {
                    return run_cluster->start_at_index;
                }
                else if (inside)
                {
                    // Find next cluster start index, be aware of bidi bounds.
                    if (run_index - 1u >= cluster_index)
                    {
                        // Still part of the run, therefore solution is easy.
                        return ((struct kan_text_shaped_edition_cluster_data_t *)
                                    selected_sequence->clusters.data)[run_index - 1u]
                            .start_at_index;
                    }

                    // And if it is an end of the run, we should not need any additional logic as when we exit back to
                    // the matching routine position of the proper next cluster will be taken automatically through
                    // starting half check.
                    break;
                }
            }

            cluster_index = run_ends_at;
            continue;
        }

        const kan_instance_offset_t cluster_middle = (cluster->visual_min + cluster->visual_max) / 2;
        const bool inside = primary_coordinate >= cluster->visual_min && primary_coordinate < cluster->visual_max;
        const bool starting_half =
            forward_base_direction ? primary_coordinate <= cluster_middle : primary_coordinate >= cluster_middle;

        if (starting_half)
        {
            return cluster->start_at_index;
        }
        else if (inside)
        {
            if (cluster_index + 1u < selected_sequence->clusters.size)
            {
                const struct kan_text_shaped_edition_cluster_data_t *next_cluster =
                    &((struct kan_text_shaped_edition_cluster_data_t *)
                          selected_sequence->clusters.data)[cluster_index + 1u];

                if (next_cluster->matching_reading_direction)
                {
                    return next_cluster->start_at_index;
                }

                // If next cluster has non-matching direction, we need to look ahead till the end of the run in order
                // to get the first cluster of bidi inverted direction.
                kan_instance_size_t run_ends_at = selected_sequence->clusters.size;

                for (kan_instance_size_t scan_index = cluster_index + 2u; scan_index < selected_sequence->clusters.size;
                     ++scan_index)
                {
                    const struct kan_text_shaped_edition_cluster_data_t *scan_cluster = &(
                        (struct kan_text_shaped_edition_cluster_data_t *) selected_sequence->clusters.data)[scan_index];

                    if (scan_cluster->matching_reading_direction)
                    {
                        run_ends_at = scan_index;
                        break;
                    }
                }

                return ((struct kan_text_shaped_edition_cluster_data_t *)
                            selected_sequence->clusters.data)[run_ends_at - 1u]
                    .start_at_index;
            }

            // Otherwise we're at the end of sequence and that will be processed below the loop.
        }

        ++cluster_index;
    }

    return selected_sequence->end_at_index;
}

static void on_map_behavior_press_motion (struct ui_controls_input_state_t *state,
                                          struct kan_ui_node_map_behavior_t *behavior,
                                          kan_floating_t x_relative,
                                          kan_floating_t y_relative)
{
    KAN_UMI_VALUE_READ_OPTIONAL (drawable, kan_ui_node_drawable_t, id, &behavior->id)
    if (!behavior->movement_enabled || !drawable || drawable->height <= 0 || behavior->camera_half_height <= 0.0f)
    {
        return;
    }

    const kan_floating_t px_to_unit = behavior->camera_half_height * 2.0f / (kan_floating_t) drawable->height;
    // As we're dragging the map by pressing and moving pointer, direction is inverted.
    behavior->camera_origin.x -= x_relative * px_to_unit;
    behavior->camera_origin.y -= y_relative * px_to_unit;
    behavior->dirty = true;
}

static void on_press_motion_internal (struct ui_controls_input_state_t *state,
                                      struct kan_ui_input_singleton_t *public,
                                      const struct kan_ui_singleton_t *ui,
                                      // Press id might be different for simulated pressed from key bindings.
                                      kan_ui_node_id_t press_id,
                                      kan_floating_t x_relative,
                                      kan_floating_t y_relative)
{
    KAN_UMI_SINGLETON_WRITE (private, ui_controls_input_private_singleton_t)
    KAN_UMI_VALUE_UPDATE_OPTIONAL (scroll_line_state, kan_ui_node_scroll_line_state_t, id, &press_id)

    if (scroll_line_state)
    {
        place_scroll_line_knob_at_press (state, public, private, ui, scroll_line_state);
    }

    KAN_UMI_VALUE_UPDATE_OPTIONAL (line_edit_behavior, kan_ui_node_line_edit_behavior_t, id, &press_id)
    if (line_edit_behavior)
    {
        private->line_edit_press_moved = true;
        const kan_instance_size_t current_content_location = calculate_content_position_on_shaped_text (
            state, line_edit_behavior->text_id, line_edit_behavior->shaping_unit_id, public->last_mouse_x,
            public->last_mouse_y);

        line_edit_behavior->cursor_content_location = KAN_INT_MAX (kan_instance_size_t);
        line_edit_behavior->selection_content_min =
            KAN_MIN (private->line_edit_press_start_content_location, current_content_location);

        line_edit_behavior->selection_content_max =
            KAN_MAX (private->line_edit_press_start_content_location, current_content_location);
        line_edit_behavior->text_visuals_dirty = true;
    }

    KAN_UMI_VALUE_UPDATE_OPTIONAL (map_behavior, kan_ui_node_map_behavior_t, id, &press_id)
    if (map_behavior)
    {
        on_map_behavior_press_motion (state, map_behavior, x_relative, y_relative);
    }
}

static bool is_in_child_hierarchy_of (struct ui_controls_input_state_t *state,
                                      kan_ui_node_id_t id_to_check,
                                      kan_ui_node_id_t hierarchy_id)
{
    while (KAN_TYPED_ID_32_IS_VALID (id_to_check))
    {
        if (KAN_TYPED_ID_32_IS_EQUAL (id_to_check, hierarchy_id))
        {
            return true;
        }

        // It is possible to get incorrect initial id from press end interaction if element was deleted during
        // interaction. For the sake of simplicity, we just use optional here instead of checking input with
        // optional first and then using required queries.
        KAN_UMI_VALUE_READ_OPTIONAL (node, kan_ui_node_t, id, &id_to_check)

        if (!node)
        {
            return false;
        }

        id_to_check = node->parent_id;
    }

    return false;
}

static void hide_popups_due_to_outside_interaction (struct ui_controls_input_state_t *state,
                                                    kan_ui_node_id_t interacted_id)
{
    KAN_UML_SEQUENCE_UPDATE (popup_state, kan_ui_node_popup_state_t)
    {
        KAN_UMI_VALUE_READ_REQUIRED (behavior, kan_ui_node_popup_behavior_t, id, &popup_state->id)
        if (behavior->hide_flags & KAN_UI_NODE_POPUP_BEHAVIOR_HIDE_FLAG_OUTSIDE_INTERACTION)
        {
            if (!is_in_child_hierarchy_of (state, interacted_id, popup_state->id))
            {
                popup_state->pending_action = KAN_UI_NODE_POPUP_ACTION_HIDE;
                popup_state->pending_action_time_s = 0.0f;
            }
        }
    }
}

static void on_press_begin_internal (struct ui_controls_input_state_t *state,
                                     struct kan_ui_input_singleton_t *public,
                                     const struct kan_ui_singleton_t *ui,
                                     // Press id might be different for simulated pressed from key bindings.
                                     kan_ui_node_id_t press_id)
{
    KAN_UMI_SINGLETON_WRITE (private, ui_controls_input_private_singleton_t)
    KAN_UMI_VALUE_UPDATE_OPTIONAL (scroll_line_state, kan_ui_node_scroll_line_state_t, id, &press_id)

    if (scroll_line_state)
    {
        bool need_to_update_knob = true;
        {
            KAN_UMI_VALUE_READ_OPTIONAL (knob_drawable, kan_ui_node_drawable_t, id, &scroll_line_state->knob_id)
            if (knob_drawable)
            {
                switch (scroll_line_state->class)
                {
                case KAN_UI_NODE_SCROLL_LINE_CLASS_HORIZONTAL:
                    if (public->last_mouse_x >= knob_drawable->global_x &&
                        public->last_mouse_x < knob_drawable->global_x + knob_drawable->width)
                    {
                        private->press_knob_offset = public->last_mouse_x - knob_drawable->global_x;
                        need_to_update_knob = false;
                    }
                    else
                    {
                        private->press_knob_offset = knob_drawable->width / 2;
                    }

                    break;

                case KAN_UI_NODE_SCROLL_LINE_CLASS_VERTICAL:
                    if (public->last_mouse_y >= knob_drawable->global_y &&
                        public->last_mouse_y < knob_drawable->global_y + knob_drawable->height)
                    {
                        private->press_knob_offset = public->last_mouse_y - knob_drawable->global_y;
                        need_to_update_knob = false;
                    }
                    else
                    {
                        private->press_knob_offset = knob_drawable->height / 2;
                    }

                    break;
                }
            }
        }

        if (need_to_update_knob)
        {
            place_scroll_line_knob_at_press (state, public, private, ui, scroll_line_state);
        }
    }

    if (KAN_TYPED_ID_32_IS_VALID (public->input_receiver_id) &&
        !KAN_TYPED_ID_32_IS_EQUAL (public->input_receiver_id, press_id))
    {
        deselect_input_receiver_behavior (state, public, private);
    }

    KAN_UMI_VALUE_UPDATE_OPTIONAL (line_edit_behavior, kan_ui_node_line_edit_behavior_t, id, &press_id)
    if (line_edit_behavior && !line_edit_behavior->content_dirty)
    {
        private->line_edit_selected_this_press =
            !KAN_TYPED_ID_32_IS_EQUAL (public->input_receiver_id, line_edit_behavior->id);

        public->input_receiver_id = line_edit_behavior->id;
        private->line_edit_press_moved = false;

        KAN_UMI_VALUE_UPDATE_REQUIRED (hit_box, kan_ui_node_hit_box_t, id, &line_edit_behavior->id)
        hit_box->interactable_style = line_edit_behavior->interactable_style_selected;

        if (!line_edit_behavior->content_dirty)
        {
            private->line_edit_press_start_content_location = calculate_content_position_on_shaped_text (
                state, line_edit_behavior->text_id, line_edit_behavior->shaping_unit_id, public->last_mouse_x,
                public->last_mouse_y);
        }
        else
        {
            private->line_edit_press_start_content_location = KAN_INT_MAX (kan_instance_size_t);
        }
    }
}

static void on_press_begin_filtered_out_internal (struct ui_controls_input_state_t *state,
                                                  struct kan_ui_input_singleton_t *public,
                                                  const struct kan_ui_singleton_t *ui,
                                                  kan_ui_node_id_t filtered_out_element_id)
{
    KAN_UMI_SINGLETON_WRITE (private, ui_controls_input_private_singleton_t)
    if (KAN_TYPED_ID_32_IS_VALID (public->input_receiver_id))
    {
        deselect_input_receiver_behavior (state, public, private);
    }

    hide_popups_due_to_outside_interaction (state, filtered_out_element_id);
}

static void on_multi_click_internal (struct ui_controls_input_state_t *state,
                                     struct kan_ui_input_singleton_t *public,
                                     const struct kan_ui_singleton_t *ui,
                                     kan_ui_node_id_t multi_click_id,
                                     kan_instance_size_t clicks_count)
{
    if (KAN_TYPED_ID_32_IS_EQUAL (multi_click_id, public->input_receiver_id))
    {
        KAN_UMI_VALUE_UPDATE_OPTIONAL (line_edit_behavior, kan_ui_node_line_edit_behavior_t, id,
                                       &public->input_receiver_id)
        if (line_edit_behavior && clicks_count == 2u)
        {
            line_edit_behavior->cursor_content_location = KAN_INT_MAX (kan_instance_size_t);
            line_edit_behavior->selection_content_min = 0u;
            // Minus 1 due to null terminator inside content.
            line_edit_behavior->selection_content_max = line_edit_behavior->content_utf8.size - 1u;
            line_edit_behavior->text_visuals_dirty = true;
        }
    }

    hide_popups_due_to_outside_interaction (state, multi_click_id);
}

static inline void schedule_popup_show (struct ui_controls_input_state_t *state,
                                        const struct kan_ui_singleton_t *ui,
                                        kan_ui_node_id_t popup_id,
                                        kan_floating_t show_after_s)
{
    bool state_updated = false;
    KAN_UML_SEQUENCE_UPDATE (popup_state, kan_ui_node_popup_state_t)
    {
        if (KAN_TYPED_ID_32_IS_EQUAL (popup_state->id, popup_id))
        {
            // We have existing state. We only need to update show action time if any.
            if (popup_state->pending_action == KAN_UI_NODE_POPUP_ACTION_SHOW)
            {
                popup_state->pending_action_time_s = KAN_MIN (popup_state->pending_action_time_s, show_after_s);
            }

            state_updated = true;
            break;
        }
    }

    if (!state_updated)
    {
        KAN_UMI_INDEXED_INSERT (new_popup_state, kan_ui_node_popup_state_t)
        new_popup_state->id = popup_id;
        new_popup_state->pending_action = KAN_UI_NODE_POPUP_ACTION_SHOW;
        new_popup_state->pending_action_time_s = show_after_s;
    }
}

static void on_press_end_internal (struct ui_controls_input_state_t *state,
                                   struct kan_ui_input_singleton_t *public,
                                   const struct kan_ui_singleton_t *ui,
                                   // Press id might be different for simulated pressed from key bindings.
                                   kan_ui_node_id_t press_id,
                                   bool continuous)
{
    KAN_UMI_SINGLETON_WRITE (private, ui_controls_input_private_singleton_t)
    if (KAN_TYPED_ID_32_IS_EQUAL (public->input_receiver_id, press_id))
    {
        KAN_UMI_VALUE_UPDATE_OPTIONAL (line_edit_behavior, kan_ui_node_line_edit_behavior_t, id, &press_id)
        if (line_edit_behavior)
        {
            if (!private->line_edit_press_moved ||
                (line_edit_behavior->selection_content_min != KAN_INT_MAX (kan_instance_size_t) &&
                 line_edit_behavior->selection_content_min == line_edit_behavior->selection_content_max))
            {
                line_edit_behavior->cursor_content_location = calculate_content_position_on_shaped_text (
                    state, line_edit_behavior->text_id, line_edit_behavior->shaping_unit_id, public->last_mouse_x,
                    public->last_mouse_y);

                line_edit_behavior->selection_content_min = KAN_INT_MAX (kan_instance_size_t);
                line_edit_behavior->selection_content_max = KAN_INT_MAX (kan_instance_size_t);
                line_edit_behavior->text_visuals_dirty = true;
            }

            if (private->line_edit_selected_this_press && !private->input_receiver_requested_text_input &&
                KAN_HANDLE_IS_VALID (public->linked_window_handle))
            {
                kan_application_window_add_text_listener (state->application_system_handle,
                                                          public->linked_window_handle);
                private->input_receiver_requested_text_input = true;
            }
        }
    }

    // Must be done before triggering popups in order to avoid hide-triggered interaction.
    hide_popups_due_to_outside_interaction (state, press_id);

    if (continuous)
    {
        KAN_UML_VALUE_READ (popup_behavior, kan_ui_node_popup_behavior_t, trigger_id, &press_id)
        {
            if (popup_behavior->trigger_flags & KAN_UI_NODE_POPUP_BEHAVIOR_TRIGGER_FLAG_PRESS_END)
            {
                // Use zero time to show as soon as possible.
                schedule_popup_show (state, ui, popup_behavior->id, 0.0f);
            }
        }

        KAN_UML_VALUE_READ (hide_popup, kan_ui_node_hide_popup_on_press_t, id, &press_id)
        {
            KAN_UML_SEQUENCE_UPDATE (popup_state, kan_ui_node_popup_state_t)
            {
                if (KAN_TYPED_ID_32_IS_EQUAL (popup_state->id, hide_popup->popup_id))
                {
                    popup_state->pending_action = KAN_UI_NODE_POPUP_ACTION_HIDE;
                    popup_state->pending_action_time_s = 0.0f;
                }
            }
        }
    }
}

static void on_map_behavior_zoom (struct ui_controls_input_state_t *state,
                                  struct kan_ui_input_singleton_t *public,
                                  const struct kan_ui_singleton_t *ui,
                                  struct kan_ui_node_map_behavior_t *behavior,
                                  kan_floating_t zoom)
{
    KAN_UMI_VALUE_READ_OPTIONAL (drawable, kan_ui_node_drawable_t, id, &behavior->id)
    if (!behavior->zoom_enabled || !drawable || drawable->height <= 0 || behavior->camera_half_height <= 0.0f)
    {
        return;
    }

    // We'd like to cache pre-zoom mouse position in order to move camera after zoom to keep mouse position relation to
    // the map, as it is usually done on web maps.
    kan_floating_t px_to_unit = behavior->camera_half_height * 2.0f / (kan_floating_t) drawable->height;
    const kan_instance_offset_t mouse_relative_x = public->last_mouse_x - drawable->global_x - drawable->width / 2;
    const kan_instance_offset_t mouse_relative_y = public->last_mouse_y - drawable->global_y - drawable->height / 2;

    const kan_floating_t mouse_location_x = behavior->camera_origin.x + px_to_unit * (kan_floating_t) mouse_relative_x;
    const kan_floating_t mouse_location_y = behavior->camera_origin.y + px_to_unit * (kan_floating_t) mouse_relative_y;

    // Apply zoom value to half height.
    const kan_floating_t strength = KAN_MAX (0.0f, 1.0f - zoom * behavior->scroll_zoom_speed);

    behavior->camera_half_height = KAN_CLAMP (behavior->camera_half_height * strength, behavior->camera_min_half_height,
                                              behavior->camera_max_half_height);

    // Now calculate origin back from mouse location.
    px_to_unit = behavior->camera_half_height * 2.0f / (kan_floating_t) drawable->height;
    behavior->camera_origin.x = mouse_location_x - px_to_unit * (kan_floating_t) mouse_relative_x;
    behavior->camera_origin.y = mouse_location_y - px_to_unit * (kan_floating_t) mouse_relative_y;
    behavior->dirty = true;
}

static void trigger_popups_on_new_hovered_node (struct ui_controls_input_state_t *state,
                                                struct kan_ui_input_singleton_t *public,
                                                const struct kan_ui_singleton_t *ui)
{
    KAN_UML_VALUE_READ (behavior, kan_ui_node_popup_behavior_t, trigger_id, &public->current_hovered_id)
    {
        if (behavior->trigger_flags & KAN_UI_NODE_POPUP_BEHAVIOR_TRIGGER_FLAG_HOVER_TIMER)
        {
            schedule_popup_show (state, ui, behavior->id, ui->animation_global_time_s + behavior->hover_timer_s);
        }
    }
}

static inline bool is_popup_triggered (struct ui_controls_input_state_t *state,
                                       struct kan_ui_input_singleton_t *public,
                                       const struct kan_ui_node_popup_behavior_t *behavior)
{
    if ((behavior->trigger_flags & KAN_UI_NODE_POPUP_BEHAVIOR_TRIGGER_FLAG_HOVER_TIMER) &&
        KAN_TYPED_ID_32_IS_VALID (behavior->trigger_id) &&
        KAN_TYPED_ID_32_IS_EQUAL (public->current_hovered_id, behavior->trigger_id))
    {
        return true;
    }

    return false;
}

static bool is_popup_preserved (struct ui_controls_input_state_t *state,
                                struct kan_ui_input_singleton_t *public,
                                const struct kan_ui_node_popup_behavior_t *behavior)
{
    if (behavior->hide_flags & KAN_UI_NODE_POPUP_BEHAVIOR_HIDE_FLAG_PRESERVE_WHILE_POINTED)
    {
        // Preservation under pointer implementation is not optimized right now at all.
        // If this simple implementation starts to cause issues, it should definitely be optimized and refactored.
        kan_ui_node_id_t under_cursor_id = public->current_hovered_id;

        if (!KAN_TYPED_ID_32_IS_VALID (under_cursor_id))
        {
            // Find what is under cursor right now as we have no quick info from hover.
            struct kan_repository_indexed_sequence_read_access_t hit_box_access;
            const struct kan_ui_node_hit_box_t *hit_box = find_hit_box_at (
                state, HIT_BOX_SEARCH_MODE_POINTER, public->last_mouse_x, public->last_mouse_y, &hit_box_access);

            if (hit_box)
            {
                under_cursor_id = hit_box->id;
                kan_repository_indexed_sequence_read_access_close (&hit_box_access);
            }
        }

        if (is_in_child_hierarchy_of (state, under_cursor_id, behavior->id))
        {
            return true;
        }
    }

    return false;
}

static void update_popup_states (struct ui_controls_input_state_t *state,
                                 struct kan_ui_input_singleton_t *public,
                                 const struct kan_ui_singleton_t *ui)
{
    KAN_UML_SEQUENCE_WRITE (popup_state, kan_ui_node_popup_state_t)
    {
        KAN_UMI_VALUE_READ_REQUIRED (behavior, kan_ui_node_popup_behavior_t, id, &popup_state->id)
        switch (popup_state->pending_action)
        {
        case KAN_UI_NODE_POPUP_ACTION_NONE:
            break;

        case KAN_UI_NODE_POPUP_ACTION_SHOW:
            // Check the timer first as show action might've been sent this frame from logic that has no persistent
            // triggers: for example press end.
            if (popup_state->pending_action_time_s <= ui->animation_global_time_s ||
                // Check for animation time overflow loop.
                ui->animation_global_time_s < ui->animation_delta_time_s)
            {
                KAN_UMI_VALUE_UPDATE_REQUIRED (node, kan_ui_node_t, id, &popup_state->id)
                node->render.hidden = false;

                if (behavior->hide_flags & KAN_UI_NODE_POPUP_BEHAVIOR_HIDE_FLAG_TIMER)
                {
                    popup_state->pending_action = KAN_UI_NODE_POPUP_ACTION_HIDE;
                    popup_state->pending_action_time_s = ui->animation_global_time_s + behavior->hide_timer_s;
                }
                else
                {
                    popup_state->pending_action = KAN_UI_NODE_POPUP_ACTION_NONE;
                }

                break;
            }

            if (!is_popup_triggered (state, public, behavior))
            {
                // Reasons to show are no longer valid.
                KAN_UM_ACCESS_DELETE (popup_state);
                break;
            }

            break;

        case KAN_UI_NODE_POPUP_ACTION_HIDE:
            if (is_popup_triggered (state, public, behavior) || is_popup_preserved (state, public, behavior))
            {
                if (behavior->hide_flags & KAN_UI_NODE_POPUP_BEHAVIOR_HIDE_FLAG_TIMER)
                {
                    popup_state->pending_action_time_s = ui->animation_global_time_s + behavior->hide_timer_s;
                }

                break;
            }

            if (popup_state->pending_action_time_s <= ui->animation_global_time_s ||
                // Check for animation time overflow loop.
                ui->animation_global_time_s < ui->animation_delta_time_s)
            {
                KAN_UMI_VALUE_UPDATE_REQUIRED (node, kan_ui_node_t, id, &popup_state->id)
                node->render.hidden = true;
                KAN_UM_ACCESS_DELETE (popup_state);
            }

            break;
        }
    }
}

static void process_events (struct ui_controls_input_state_t *state,
                            struct kan_ui_input_singleton_t *public,
                            const struct kan_ui_singleton_t *ui,
                            const struct kan_ui_bundle_singleton_t *bundle,
                            bool visuals_changed,
                            bool hit_boxes_changed)
{
    bool do_mouse_hit_box_full_update = visuals_changed || hit_boxes_changed;
    const struct kan_platform_application_event_t *event;

    while (
        (event = kan_application_system_event_iterator_get (state->application_system_handle, public->event_iterator)))
    {
        switch (event->type)
        {
            // Clear interaction when receiving these events just in case
            // to avoid possibility of incorrectly preserved state.
        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_WINDOW_SHOWN:
        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_WINDOW_HIDDEN:
        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_WINDOW_MOVED:
        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_WINDOW_RESIZED:
        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_WINDOW_PIXEL_SIZE_CHANGED:
        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_WINDOW_MINIMIZED:
        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_WINDOW_MAXIMIZED:
        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_WINDOW_RESTORED:
        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_WINDOW_MOUSE_ENTER:
        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_WINDOW_MOUSE_LEAVE:
        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_WINDOW_FOCUS_GAINED:
        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_WINDOW_FOCUS_LOST:
        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_WINDOW_ENTER_FULLSCREEN:
        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_WINDOW_LEAVE_FULLSCREEN:
            if (KAN_TYPED_ID_32_IS_EQUAL (event->window.id, public->linked_window_id))
            {
                do_mouse_hit_box_full_update = true;
                // Also end press just in case.
                public->mouse_button_down_flags = 0u;

                if (public->press_filtered_in)
                {
                    KAN_UMO_EVENT_INSERT_INIT (kan_ui_press_end_t) {
                        .node_id = public->press_started_on_id,
                        .mouse_button_down_inclusive_flags = public->mouse_button_down_inclusive_flags,
                        .continuous_press = false,
                        .at_x = public->last_mouse_x,
                        .at_y = public->last_mouse_y,
                    };

                    on_press_end_internal (state, public, ui, public->press_started_on_id, false);
                }

                // Reset key dependencies on down marks as we might never get the key up event due to losing focus.
                KAN_UML_SEQUENCE_UPDATE (down_mark, kan_ui_node_down_mark_t)
                {
                    down_mark->down_from_key = KAN_PLATFORM_SCAN_CODE_UNKNOWN;
                }

                public->mouse_button_down_inclusive_flags = 0u;
                public->press_started_on_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
                public->press_filtered_in = false;
            }

            break;

        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_KEY_DOWN:
            if (KAN_TYPED_ID_32_IS_EQUAL (event->keyboard.window_id, public->linked_window_id))
            {
                process_key_down_internal (state, public, ui, bundle, event);
            }

            break;

        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_KEY_UP:
            if (KAN_TYPED_ID_32_IS_EQUAL (event->keyboard.window_id, public->linked_window_id))
            {
                process_key_up_internal (state, public, ui, bundle, event);
            }

            break;

        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_TEXT_INPUT:
            if (KAN_TYPED_ID_32_IS_EQUAL (event->text_input.window_id, public->linked_window_id))
            {
                process_text_input_internal (state, public, ui, event);
            }

            break;

        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_MOUSE_MOTION:
            if (KAN_TYPED_ID_32_IS_EQUAL (event->mouse_motion.window_id, public->linked_window_id))
            {
                do_mouse_hit_box_full_update = true;
                public->last_mouse_x = public->viewport_offset_x + (kan_instance_offset_t) event->mouse_motion.window_x;
                public->last_mouse_y = public->viewport_offset_y + (kan_instance_offset_t) event->mouse_motion.window_y;

                if (public->press_filtered_in)
                {
                    KAN_UMO_EVENT_INSERT_INIT (kan_ui_press_motion_t) {
                        .node_id = public->press_started_on_id,
                        .mouse_button_down_flags = public->mouse_button_down_flags,
                        .mouse_button_down_inclusive_flags = public->mouse_button_down_inclusive_flags,
                        .at_x = public->last_mouse_x,
                        .at_y = public->last_mouse_y,
                        .delta_x = (kan_instance_offset_t) event->mouse_motion.window_x_relative,
                        .delta_y = (kan_instance_offset_t) event->mouse_motion.window_y_relative,
                    };

                    on_press_motion_internal (state, public, ui, public->press_started_on_id,
                                              event->mouse_motion.window_x_relative,
                                              event->mouse_motion.window_y_relative);
                }
            }

            break;

        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_MOUSE_BUTTON_DOWN:
            if (KAN_TYPED_ID_32_IS_EQUAL (event->mouse_button.window_id, public->linked_window_id))
            {
                public->last_mouse_x = public->viewport_offset_x + (kan_instance_offset_t) event->mouse_button.window_x;
                public->last_mouse_y = public->viewport_offset_y + (kan_instance_offset_t) event->mouse_button.window_y;

                const uint32_t flag = 1u << event->mouse_button.button;
                const bool new_press = public->mouse_button_down_flags == 0u;
                const bool multi_click = event->mouse_button.clicks > 1u;

                public->mouse_button_down_flags |= flag;
                public->mouse_button_down_inclusive_flags |= flag;

                struct kan_repository_indexed_sequence_read_access_t element_access;
                const struct kan_ui_node_hit_box_t *element = find_hit_box_at (
                    state, HIT_BOX_SEARCH_MODE_POINTER, public->last_mouse_x, public->last_mouse_y, &element_access);

                public->press_filtered_in =
                    !element || ((new_press || multi_click) && element->interactable && !element->disabled &&
                                 (element->mouse_button_down_flags & flag));

                const kan_ui_node_id_t element_id =
                    element ? element->id : KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);

                public->press_started_on_id =
                    element && public->press_filtered_in ? element->id : KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);

                if (multi_click)
                {
                    if (public->press_filtered_in)
                    {
                        KAN_UMO_EVENT_INSERT_INIT (kan_ui_multi_click_t) {
                            .node_id = public->press_started_on_id,
                            .multi_click_button = event->mouse_button.button,
                            .click_count = event->mouse_button.clicks,
                            .mouse_button_down_flags = public->mouse_button_down_flags,
                            .at_x = public->last_mouse_x,
                            .at_y = public->last_mouse_y,
                        };

                        if (element)
                        {
                            prolong_hit_box_down_visuals (state, public, bundle, element);
                        }
                    }

                    public->mouse_button_down_flags &= ~flag;
                    if (new_press)
                    {
                        KAN_UMO_EVENT_INSERT_INIT (kan_ui_press_end_t) {
                            .node_id = public->press_started_on_id,
                            .mouse_button_down_inclusive_flags = public->mouse_button_down_inclusive_flags,
                            .continuous_press = true,
                            .at_x = public->last_mouse_x,
                            .at_y = public->last_mouse_y,
                        };

                        public->press_filtered_in = false;
                        public->press_started_on_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
                    }
                }
                else if (new_press && public->press_filtered_in)
                {
                    KAN_UMO_EVENT_INSERT_INIT (kan_ui_press_begin_t) {
                        .node_id = public->press_started_on_id,
                        .mouse_button_down_flags = public->mouse_button_down_flags,
                        .at_x = public->last_mouse_x,
                        .at_y = public->last_mouse_y,
                    };
                }

                if (element)
                {
                    kan_repository_indexed_sequence_read_access_close (&element_access);
                }

                // We process press internally after closing access to the hit box as
                // it might be needed to modify hit box while processing the press.
                if (!multi_click && new_press)
                {
                    if (public->press_filtered_in)
                    {
                        on_press_begin_internal (state, public, ui, public->press_started_on_id);
                    }
                    else
                    {
                        // Special callback for the cases when we need to reset press-based selection.
                        on_press_begin_filtered_out_internal (state, public, ui, element_id);
                    }
                }

                if (multi_click)
                {
                    on_multi_click_internal (state, public, ui, element_id, event->mouse_button.clicks);
                }
            }

            break;

        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_MOUSE_BUTTON_UP:
            if (KAN_TYPED_ID_32_IS_EQUAL (event->mouse_button.window_id, public->linked_window_id))
            {
                public->last_mouse_x = public->viewport_offset_x + (kan_instance_offset_t) event->mouse_button.window_x;
                public->last_mouse_y = public->viewport_offset_y + (kan_instance_offset_t) event->mouse_button.window_y;

                const uint32_t flag = 1u << event->mouse_button.button;
                const bool end_press = public->mouse_button_down_flags == flag;
                public->mouse_button_down_flags &= ~flag;

                if (end_press)
                {
                    if (public->press_filtered_in)
                    {
                        struct kan_repository_indexed_sequence_read_access_t element_access;
                        const struct kan_ui_node_hit_box_t *element =
                            find_hit_box_at (state, HIT_BOX_SEARCH_MODE_POINTER, public->last_mouse_x,
                                             public->last_mouse_y, &element_access);

                        const kan_ui_node_id_t end_node_id =
                            element ? element->id : KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
                        const bool continuous = KAN_TYPED_ID_32_IS_EQUAL (end_node_id, public->press_started_on_id);

                        KAN_UMO_EVENT_INSERT_INIT (kan_ui_press_end_t) {
                            .node_id = public->press_started_on_id,
                            .mouse_button_down_inclusive_flags = public->mouse_button_down_inclusive_flags,
                            .continuous_press = continuous,
                            .at_x = public->last_mouse_x,
                            .at_y = public->last_mouse_y,
                        };

                        if (element)
                        {
                            kan_repository_indexed_sequence_read_access_close (&element_access);
                        }

                        // We process press internally after closing access to the hit box as
                        // it might be needed to modify hit box while processing the press.
                        on_press_end_internal (state, public, ui, public->press_started_on_id, continuous);
                    }

                    public->mouse_button_down_inclusive_flags = 0u;
                    public->press_started_on_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
                    public->press_filtered_in = false;
                }
            }

            break;

        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_MOUSE_WHEEL:
        {
            if (KAN_TYPED_ID_32_IS_EQUAL (event->mouse_wheel.window_id, public->linked_window_id))
            {
                public->last_mouse_x = public->viewport_offset_x + (kan_instance_offset_t) event->mouse_wheel.window_x;
                public->last_mouse_y = public->viewport_offset_y + (kan_instance_offset_t) event->mouse_wheel.window_y;

                struct kan_repository_indexed_sequence_read_access_t element_access;
                const struct kan_ui_node_hit_box_t *element = find_hit_box_at (
                    state, HIT_BOX_SEARCH_MODE_SCROLL, public->last_mouse_x, public->last_mouse_y, &element_access);

                if (!element || (element->interactable && !element->disabled))
                {
                    KAN_UMO_EVENT_INSERT_INIT (kan_ui_scroll_t) {
                        .node_id = element ? element->id : KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t),
                        .scroll_x = event->mouse_wheel.wheel_x,
                        .scroll_y = event->mouse_wheel.wheel_y,
                        .delta_time_s = ui->animation_delta_time_s,
                        .at_x = public->last_mouse_x,
                        .at_y = public->last_mouse_y,
                    };

                    hide_popups_due_to_outside_interaction (
                        state, element ? element->id : KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t));
                }

                bool processed_input = false;
                if (element && element->interactable && !element->disabled)
                {
                    KAN_UMI_VALUE_READ_OPTIONAL (scroll_behaviour, kan_ui_node_scroll_behavior_t, id, &element->id)
                    if (scroll_behaviour)
                    {
                        processed_input = true;
                        const kan_floating_t speed_x =
                            kan_ui_calculate_coordinate_floating (ui, scroll_behaviour->mouse_speed_x);
                        const kan_floating_t speed_y =
                            kan_ui_calculate_coordinate_floating (ui, scroll_behaviour->mouse_speed_y);

                        apply_scroll_relative_input (state, public, ui, scroll_behaviour,
                                                     event->mouse_wheel.wheel_x * speed_x * ui->animation_delta_time_s,
                                                     event->mouse_wheel.wheel_y * speed_y * ui->animation_delta_time_s,
                                                     true);
                    }

                    KAN_UMI_VALUE_UPDATE_OPTIONAL (map_behavior, kan_ui_node_map_behavior_t, id, &element->id)
                    if (map_behavior)
                    {
                        processed_input = true;
                        on_map_behavior_zoom (state, public, ui, map_behavior,
                                              event->mouse_wheel.wheel_y * ui->animation_delta_time_s);
                    }
                }

                if (element)
                {
                    kan_repository_indexed_sequence_read_access_close (&element_access);
                }

                // If mouse wheel input was passed to some behavior, deselect input receiver so the user won't
                // unexpectedly input anything into it.
                if (processed_input && KAN_TYPED_ID_32_IS_VALID (public->input_receiver_id))
                {
                    KAN_UMI_SINGLETON_WRITE (private, ui_controls_input_private_singleton_t)
                    deselect_input_receiver_behavior (state, public, private);
                }
            }

            break;
        }

        default:
            // Do not care about other event types.
            break;
        }

        public->event_iterator = kan_application_system_event_iterator_advance (public->event_iterator);
    }

    if (do_mouse_hit_box_full_update)
    {
        struct kan_repository_indexed_sequence_read_access_t new_hovered_access;
        const struct kan_ui_node_hit_box_t *new_hovered = find_hit_box_at (
            state, HIT_BOX_SEARCH_MODE_POINTER, public->last_mouse_x, public->last_mouse_y, &new_hovered_access);
        const bool new_hovered_applicable = new_hovered && new_hovered->interactable && !new_hovered->disabled;

        if (!new_hovered_applicable || !KAN_TYPED_ID_32_IS_EQUAL (public->current_hovered_id, new_hovered->id))
        {
            KAN_UMI_VALUE_READ_OPTIONAL (old_hovered, kan_ui_node_hit_box_t, id, &public->current_hovered_id)
            public->current_hovered_id =
                new_hovered_applicable ? new_hovered->id : KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);

            if (old_hovered)
            {
                apply_hit_box_interaction_visuals (state, public, bundle, old_hovered, false);
            }

            if (new_hovered_applicable)
            {
                apply_hit_box_interaction_visuals (state, public, bundle, new_hovered, false);
                trigger_popups_on_new_hovered_node (state, public, ui);
            }
        }

        if (new_hovered)
        {
            kan_repository_indexed_sequence_read_access_close (&new_hovered_access);
        }
    }
    // Just check focused hit box validity.
    else if (KAN_TYPED_ID_32_IS_VALID (public->current_hovered_id))
    {
        KAN_UMI_VALUE_READ_OPTIONAL (old_hovered, kan_ui_node_hit_box_t, id, &public->current_hovered_id)
        const bool reset = !old_hovered || old_hovered->disabled;

        if (reset)
        {
            public->current_hovered_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
            if (old_hovered)
            {
                apply_hit_box_interaction_visuals (state, public, bundle, old_hovered, false);
            }
        }
    }

    update_popup_states (state, public, ui);
    if (public->press_filtered_in && KAN_TYPED_ID_32_IS_VALID (public->press_started_on_id))
    {
        KAN_UMI_VALUE_READ_OPTIONAL (hit_box, kan_ui_node_hit_box_t, id, &public->press_started_on_id)
        if (hit_box)
        {
            prolong_hit_box_down_visuals (state, public, bundle, hit_box);
        }
    }
}

/// \details Just a helper that skips whitespaces in user input,
static inline kan_unicode_codepoint_t line_edit_parse_numeric_next_codepoint (const uint8_t **iterator,
                                                                              const uint8_t *boundary)
{
    while (true)
    {
        const kan_unicode_codepoint_t codepoint = kan_text_utf8_next (iterator, boundary);
        if (codepoint == ' ' || codepoint == '\t')
        {
            continue;
        }

        return codepoint;
    }
}

static inline kan_stable_size_t line_edit_parse_positive_decimal (const uint8_t **iterator,
                                                                  const uint8_t *boundary,
                                                                  bool *output_success)
{
    kan_stable_size_t result = 0u;
    *output_success = false;
    kan_unicode_codepoint_t codepoint = line_edit_parse_numeric_next_codepoint (iterator, boundary);

    if (!codepoint)
    {
        // Empty input.
        return 0u;
    }

    while (codepoint)
    {
        if (codepoint < '0' || codepoint > '9')
        {
            return 0u;
        }

        const kan_stable_size_t new_result = result * 10u + (codepoint - '0');
        if (new_result <= result && result != 0u)
        {
            // Overflow.
            return 0u;
        }

        result = new_result;
        codepoint = line_edit_parse_numeric_next_codepoint (iterator, boundary);
    }

    *output_success = true;
    return result;
}

static inline kan_stable_size_t line_edit_parse_binary_after_prefix (const uint8_t **iterator,
                                                                     const uint8_t *boundary,
                                                                     bool *output_success)
{
    kan_stable_size_t result = 0u;
    *output_success = false;
    kan_unicode_codepoint_t codepoint = line_edit_parse_numeric_next_codepoint (iterator, boundary);

    if (!codepoint)
    {
        // Empty input.
        return 0u;
    }

    while (codepoint)
    {
        if (codepoint != '0' && codepoint != '1')
        {
            return 0u;
        }

        const kan_stable_size_t new_result = result * 2u + (codepoint - '0');
        if (new_result <= result && result != 0u)
        {
            // Overflow.
            return 0u;
        }

        result = new_result;
        codepoint = line_edit_parse_numeric_next_codepoint (iterator, boundary);
    }

    *output_success = true;
    return result;
}

static inline kan_stable_size_t line_edit_parse_hex_after_prefix (const uint8_t **iterator,
                                                                  const uint8_t *boundary,
                                                                  bool *output_success)
{
    kan_stable_size_t result = 0u;
    *output_success = false;
    kan_unicode_codepoint_t codepoint = line_edit_parse_numeric_next_codepoint (iterator, boundary);

    if (!codepoint)
    {
        // Empty input.
        return 0u;
    }

    while (codepoint)
    {
        kan_stable_size_t digit = 0u;
        switch (codepoint)
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            digit = codepoint - '0';
            break;

        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            digit = 10u + (codepoint - 'a');
            break;

        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            digit = 10u + (codepoint - 'A');
            break;

        default:
            return 0u;
        }

        const kan_stable_size_t new_result = result * 16u + digit;
        if (new_result <= result && result != 0u)
        {
            // Overflow.
            return 0u;
        }

        result = new_result;
        codepoint = line_edit_parse_numeric_next_codepoint (iterator, boundary);
    }

    *output_success = true;
    return result;
}

static bool line_edit_parse_uint (struct kan_ui_node_line_edit_behavior_t *behavior)
{
    behavior->content_uint.has_parsed_content = false;
    const uint8_t *iterator = behavior->content_utf8.data;
    const uint8_t *boundary = behavior->content_utf8.data + behavior->content_utf8.size;

    if (!iterator)
    {
        return false;
    }

    const uint8_t *pre_lookup_iterator = iterator;
    kan_unicode_codepoint_t codepoint = line_edit_parse_numeric_next_codepoint (&iterator, boundary);

    if (codepoint == '0')
    {
        codepoint = line_edit_parse_numeric_next_codepoint (&iterator, boundary);
        switch (codepoint)
        {
        case 'x':
        case 'X':
            behavior->content_uint.parsed_content =
                line_edit_parse_hex_after_prefix (&iterator, boundary, &behavior->content_uint.has_parsed_content);
            break;

        case 'b':
        case 'B':
            behavior->content_uint.parsed_content =
                line_edit_parse_binary_after_prefix (&iterator, boundary, &behavior->content_uint.has_parsed_content);
            break;

        default:
            // Lookup didn't find prefix, parse as regular decimal.
            iterator = pre_lookup_iterator;
            behavior->content_uint.parsed_content =
                line_edit_parse_positive_decimal (&iterator, boundary, &behavior->content_uint.has_parsed_content);
            break;
        }
    }
    else
    {
        // No prefix, parse as regular decimal.
        iterator = pre_lookup_iterator;
        behavior->content_uint.parsed_content =
            line_edit_parse_positive_decimal (&iterator, boundary, &behavior->content_uint.has_parsed_content);
    }

    if (behavior->content_uint.has_parsed_content && behavior->content_uint.has_limits)
    {
        behavior->content_uint.has_parsed_content =
            behavior->content_uint.parsed_content >= behavior->content_uint.min &&
            behavior->content_uint.parsed_content < behavior->content_uint.max;
    }

    return behavior->content_uint.has_parsed_content;
}

static bool line_edit_parse_sint (struct kan_ui_node_line_edit_behavior_t *behavior)
{
    behavior->content_sint.has_parsed_content = false;
    const uint8_t *iterator = behavior->content_utf8.data;
    const uint8_t *boundary = behavior->content_utf8.data + behavior->content_utf8.size;

    if (!iterator)
    {
        return false;
    }

    const uint8_t *pre_lookup_iterator = iterator;
    kan_unicode_codepoint_t codepoint = line_edit_parse_numeric_next_codepoint (&iterator, boundary);

    if (codepoint == '-')
    {
        // Negative value, parse as positive and invert.
        const kan_stable_size_t positive_value =
            line_edit_parse_positive_decimal (&iterator, boundary, &behavior->content_sint.has_parsed_content);

        if (behavior->content_sint.has_parsed_content)
        {
            const kan_stable_size_t inverted_value = KAN_INT_MAX (kan_stable_size_t) - positive_value + 1u;
            behavior->content_sint.has_parsed_content = inverted_value > KAN_INT_MAX (kan_stable_offset_t);
            behavior->content_sint.parsed_content = (kan_stable_offset_t) inverted_value;
        }
    }
    else
    {
        // Regular positive number as lookup didn't find anything.
        iterator = pre_lookup_iterator;
        const kan_stable_size_t positive_value =
            line_edit_parse_positive_decimal (&iterator, boundary, &behavior->content_sint.has_parsed_content);

        if (behavior->content_sint.has_parsed_content)
        {
            behavior->content_sint.has_parsed_content = positive_value <= KAN_INT_MAX (kan_stable_offset_t);
            behavior->content_sint.parsed_content = (kan_stable_offset_t) positive_value;
        }
    }

    if (behavior->content_sint.has_parsed_content && behavior->content_sint.has_limits)
    {
        behavior->content_sint.has_parsed_content =
            behavior->content_sint.parsed_content >= behavior->content_sint.min &&
            behavior->content_sint.parsed_content < behavior->content_sint.max;
    }

    return behavior->content_sint.has_parsed_content;
}

static inline kan_floating_t line_edit_parse_positive_floating (const uint8_t **iterator,
                                                                const uint8_t *boundary,
                                                                bool *output_success)
{
    kan_floating_t result = 0.0f;
    *output_success = false;
    kan_unicode_codepoint_t codepoint = line_edit_parse_numeric_next_codepoint (iterator, boundary);

    if (!codepoint)
    {
        // Empty input.
        return 0u;
    }

    bool reading_fractional = false;
    kan_floating_t fractional_modifier = 0.1f;

    while (codepoint)
    {
        if (codepoint == '.')
        {
            if (reading_fractional)
            {
                // Second dot, error.
                return 0.0f;
            }

            reading_fractional = true;
            codepoint = line_edit_parse_numeric_next_codepoint (iterator, boundary);
            continue;
        }

        if (reading_fractional)
        {
            result += fractional_modifier * (kan_floating_t) (codepoint - '0');
            fractional_modifier *= 0.1f;
        }
        else
        {
            result = result * 10.0f + (kan_floating_t) (codepoint - '0');
        }

        codepoint = line_edit_parse_numeric_next_codepoint (iterator, boundary);
    }

    *output_success = true;
    return result;
}

static bool line_edit_parse_float (struct kan_ui_node_line_edit_behavior_t *behavior)
{
    behavior->content_float.has_parsed_content = false;
    const uint8_t *iterator = behavior->content_utf8.data;
    const uint8_t *boundary = behavior->content_utf8.data + behavior->content_utf8.size;

    if (!iterator)
    {
        return false;
    }

    const uint8_t *pre_lookup_iterator = iterator;
    kan_unicode_codepoint_t codepoint = line_edit_parse_numeric_next_codepoint (&iterator, boundary);

    if (codepoint == '-')
    {
        // Negative value, parse as positive and invert.
        behavior->content_float.parsed_content =
            line_edit_parse_positive_floating (&iterator, boundary, &behavior->content_float.has_parsed_content);
        behavior->content_float.parsed_content = -behavior->content_float.parsed_content;
    }
    else
    {
        // Regular positive number as lookup didn't find anything.
        iterator = pre_lookup_iterator;
        behavior->content_float.parsed_content =
            line_edit_parse_positive_floating (&iterator, boundary, &behavior->content_float.has_parsed_content);
    }

    if (behavior->content_float.has_parsed_content && behavior->content_float.has_limits)
    {
        behavior->content_float.has_parsed_content =
            behavior->content_float.parsed_content >= behavior->content_float.min &&
            behavior->content_float.parsed_content < behavior->content_float.max;
    }

    return behavior->content_float.has_parsed_content;
}

static void process_line_edit_content_dirty_inner (struct ui_controls_input_state_t *state)
{
    KAN_UMI_SINGLETON_READ (locale_singleton, kan_locale_singleton_t)
    KAN_UMI_VALUE_READ_OPTIONAL (locale, kan_locale_t, name, &locale_singleton->selected_locale)

    if (!locale)
    {
        // Can't create text with proper bidi while locale is not available.
        return;
    }

    KAN_UML_SIGNAL_UPDATE (behavior, kan_ui_node_line_edit_behavior_t, content_dirty, true)
    {
        KAN_UMI_VALUE_UPDATE_REQUIRED (shaping_unit, kan_text_shaping_unit_t, id, &behavior->shaping_unit_id)
        kan_dynamic_array_set_capacity (&behavior->content_utf8, behavior->content_utf8.size);

        if (KAN_HANDLE_IS_VALID (shaping_unit->request.text))
        {
            kan_text_destroy (shaping_unit->request.text);
        }

        bool content_valid = true;
        switch (behavior->content_type)
        {
        case KAN_UI_NODE_LINE_EDIT_CONTENT_TYPE_ANY:
            break;

        case KAN_UI_NODE_LINE_EDIT_CONTENT_TYPE_UINT:
            content_valid = line_edit_parse_uint (behavior);
            break;

        case KAN_UI_NODE_LINE_EDIT_CONTENT_TYPE_SINT:
            content_valid = line_edit_parse_sint (behavior);
            break;

        case KAN_UI_NODE_LINE_EDIT_CONTENT_TYPE_FLOAT:
            content_valid = line_edit_parse_float (behavior);
            break;
        }

        struct kan_text_item_t new_text_items[] = {
            KAN_INIT_TEXT_ITEM_STYLE (content_valid ? behavior->content_style : behavior->content_style_when_invalid,
                                      content_valid ? behavior->content_mark : behavior->content_mark_when_invalid),
            KAN_INIT_TEXT_ITEM_UTF8 ((const char *) behavior->content_utf8.data),
        };

        struct kan_text_description_t new_text_description = {
            .items_count = sizeof (new_text_items) / sizeof (new_text_items[0u]),
            .items = new_text_items,
            .guide_bidi_with_direction = false,
            .direction_to_guide_bidi = KAN_TEXT_READING_DIRECTION_LEFT_TO_RIGHT,
        };

        // Using macro primarily to highlight possible code reuse.
        KAN_NEW_TEXT_SHAPING_UNIT_APPLY_LOCALE (new, locale)

        // Recreating text that way is not very effective, but text line edit should not be a bottleneck for us anyway.
        shaping_unit->request.text = kan_text_create (&new_text_description);
        shaping_unit->dirty = true;

        behavior->content_dirty = false;
        behavior->text_visuals_dirty = true;
        KAN_UMO_EVENT_INSERT_INIT (kan_line_edit_content_changed_t) {.node_id = behavior->id};
    }
}

static void process_interactable_state_changes (struct ui_controls_input_state_t *state,
                                                const struct kan_ui_input_singleton_t *public,
                                                const struct kan_ui_bundle_singleton_t *bundle)
{
    KAN_UML_EVENT_FETCH (changed_event, kan_ui_node_hit_box_on_state_change_event_t)
    {
        KAN_UMI_VALUE_READ_OPTIONAL (hit_box, kan_ui_node_hit_box_t, id, &changed_event->id)
        if (hit_box && hit_box->interactable)
        {
            apply_hit_box_interaction_visuals (state, public, bundle, hit_box, false);
        }
    }
}

static void clear_old_down_marks (struct ui_controls_input_state_t *state,
                                  const struct kan_ui_singleton_t *ui,
                                  const struct kan_ui_input_singleton_t *public,
                                  const struct kan_ui_bundle_singleton_t *bundle)
{
    KAN_UML_SEQUENCE_DELETE (down_mark, kan_ui_node_down_mark_t)
    {
        if (down_mark->down_from_key != KAN_PLATFORM_SCAN_CODE_UNKNOWN)
        {
            continue;
        }

        if (ui->animation_global_time_s > down_mark->down_until_s ||
            // Check for animation time overflow loop.
            ui->animation_global_time_s < ui->animation_delta_time_s)
        {
            KAN_UMI_VALUE_READ_OPTIONAL (hit_box, kan_ui_node_hit_box_t, id, &down_mark->id)
            if (hit_box)
            {
                apply_hit_box_interaction_visuals (state, public, bundle, hit_box, true);
            }

            KAN_UM_ACCESS_DELETE (down_mark);
        }
    }
}

static void update_scroll_line_visibility (struct ui_controls_input_state_t *state, const struct kan_ui_singleton_t *ui)
{
    // We should not have many scroll lines at once, therefore plain iteration should be fine,
    // but we'll add profiler section just in case.
    KAN_CPU_SCOPED_STATIC_SECTION (update_scroll_line_visibility)

    KAN_UML_SEQUENCE_UPDATE (line_state, kan_ui_node_scroll_line_state_t)
    {
        if (line_state->visible_until_s != FLT_MAX && !KAN_FLOATING_IS_NEAR (line_state->visible_until_s, 0.0f) &&
            (ui->animation_global_time_s > line_state->visible_until_s ||
             // Check for animation time overflow loop.
             ui->animation_global_time_s < ui->animation_delta_time_s))
        {
            KAN_UMI_VALUE_UPDATE_OPTIONAL (line_node, kan_ui_node_t, id, &line_state->id)
            if (line_node)
            {
                line_node->render.hide_children = true;
            }

            line_state->visible_until_s = 0.0f;
        }
    }
}

UNIVERSE_UI_API KAN_UM_MUTATOR_EXECUTE (ui_controls_input)
{
    KAN_UMI_SINGLETON_WRITE (public, kan_ui_input_singleton_t)
    if (!KAN_HANDLE_IS_VALID (public->event_iterator))
    {
        return;
    }

    KAN_UMI_SINGLETON_READ (bundle, kan_ui_bundle_singleton_t)
    if (!bundle->available)
    {
        // As long as there is no bundle, there is also no render: no sense to update anything, just skip events.
        while (kan_application_system_event_iterator_get (state->application_system_handle, public->event_iterator))
        {
            public->event_iterator = kan_application_system_event_iterator_advance (public->event_iterator);
        }

        return;
    }

    KAN_UMI_SINGLETON_READ (ui, kan_ui_singleton_t)
    bool visuals_changed = false;

    KAN_UML_EVENT_FETCH (bundle_updated_event, kan_ui_bundle_updated_t)
    {
        visuals_changed = INPUT_HIT_BOX_MOUSE_UPDATE_MODE_EXECUTE;
    }

    bool hit_boxes_changed = process_hit_box_insertion (state, public, bundle);
    process_scroll_behavior_insertion (state, ui);
    process_line_edit_behavior_insertion (state);
    process_popup_behavior_insertion (state);

    sanitize_input_receiver_selection (state, public);
    process_line_edit_content_dirty_outer (state, public);

    KAN_UML_EVENT_FETCH (laid_out_event, kan_ui_node_laid_out_t)
    {
        KAN_UMI_VALUE_READ_OPTIONAL (hit_box, kan_ui_node_hit_box_t, id, &laid_out_event->node_id)
        if (hit_box)
        {
            hit_boxes_changed = true;
        }

        KAN_UMI_VALUE_READ_OPTIONAL (scroll_behavior, kan_ui_node_scroll_behavior_t, id, &laid_out_event->node_id)
        if (scroll_behavior)
        {
            // Results in incorrect scroll behavior when container has text behaviors with
            // `sync_ui_size_from_text_secondary` flag, as their height update is delayed.
            // Not a priority right now, but might be an additional reason to think about
            // better text sizing logic later.
            ensure_scroll_is_in_limits (state, ui, scroll_behavior);
        }
    }

    process_events (state, public, ui, bundle, visuals_changed, hit_boxes_changed);
    process_line_edit_content_dirty_inner (state);

    process_interactable_state_changes (state, public, bundle);
    clear_old_down_marks (state, ui, public, bundle);
    update_scroll_line_visibility (state, ui);
}

struct ui_controls_pre_layout_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (ui_controls_pre_layout)
    KAN_UM_BIND_STATE (ui_controls_pre_layout, state)
};

UNIVERSE_UI_API KAN_UM_MUTATOR_DEPLOY (ui_controls_pre_layout)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_cpu_static_sections_ensure_initialized ();

    kan_workflow_graph_node_depend_on (workflow_node, KAN_UI_CONTROLS_PRE_LAYOUT_BEGIN_CHECKPOINT);
    kan_workflow_graph_node_make_dependency_of (workflow_node, KAN_UI_CONTROLS_PRE_LAYOUT_END_CHECKPOINT);
    kan_workflow_graph_node_make_dependency_of (workflow_node, KAN_UI_LAYOUT_BEGIN_CHECKPOINT);
}

static void sync_ui_size_from_text_secondary (struct ui_controls_pre_layout_state_t *state,
                                              struct kan_ui_node_t *node,
                                              const struct kan_ui_node_text_behavior_t *text_behavior)
{
    KAN_UMI_VALUE_UPDATE_OPTIONAL (shaping_unit, kan_text_shaping_unit_t, id, &text_behavior->shaping_unit_id)
    if (!shaping_unit)
    {
        return;
    }

    if (shaping_unit->shaped)
    {
        switch (shaping_unit->request.orientation)
        {
        case KAN_TEXT_ORIENTATION_HORIZONTAL:
            node->element.height = KAN_UI_VALUE_PX ((kan_floating_t) shaping_unit->shaped_secondary_size);
            break;

        case KAN_TEXT_ORIENTATION_VERTICAL:
            node->element.width = KAN_UI_VALUE_PX ((kan_floating_t) shaping_unit->shaped_secondary_size);
            break;
        }
    }

    KAN_UMI_VALUE_UPDATE_REQUIRED (drawable, kan_ui_node_drawable_t, id, &node->id)
    drawable->hidden_temporary = false;
}

/// \details Intentionally not dependant on mutator as adjustment can be needed on pre render during
///          rare laid out cases.
static inline void map_behavior_sanitize_camera (struct kan_ui_node_map_behavior_t *map_behavior,
                                                 const struct kan_ui_node_drawable_t *drawable)
{
    if (drawable->width <= 0 || drawable->height <= 0)
    {
        return;
    }

    KAN_ASSERT (map_behavior->camera_min_half_height > 0.0f)
    const kan_floating_t ratio = (kan_floating_t) drawable->width / (kan_floating_t) drawable->height;

    map_behavior->camera_half_height = KAN_CLAMP (
        map_behavior->camera_half_height, map_behavior->camera_min_half_height, map_behavior->camera_max_half_height);

    // Limit half height to avoid situation when map does not touch any of the borders when user provided infinitely
    // high max half height (which is expected for the cases when whole map should be visible).
    const kan_floating_t visible_height = map_behavior->camera_half_height * 2.0f;
    const kan_floating_t visible_width = ratio * visible_height;

    if (visible_width > map_behavior->width && visible_height > map_behavior->height)
    {
        const kan_floating_t fit_height_value = map_behavior->height * 0.5f;
        const kan_floating_t fit_width_value = map_behavior->width * 0.5f / ratio;
        map_behavior->camera_half_height = KAN_MAX (fit_height_value, fit_width_value);
    }

    // Adjust origin to prevent camera from flying outside of borders (when possible with current half height).
    const kan_floating_t camera_half_height = map_behavior->camera_half_height;
    const kan_floating_t camera_half_width = ratio * camera_half_height;

    if (camera_half_width * 2.0f >= map_behavior->width)
    {
        // Rare whole map case, place origin in the map center.
        map_behavior->camera_origin.x = map_behavior->width * 0.5f;
    }
    else if (map_behavior->camera_origin.x - camera_half_width < 0.0f)
    {
        map_behavior->camera_origin.x = camera_half_width;
    }
    else if (map_behavior->camera_origin.x + camera_half_width > map_behavior->width)
    {
        map_behavior->camera_origin.x = map_behavior->width - camera_half_width;
    }

    if (camera_half_height * 2.0f >= map_behavior->height)
    {
        // Rare whole map case, place origin in the map center.
        map_behavior->camera_origin.y = map_behavior->height * 0.5f;
    }
    else if (map_behavior->camera_origin.y - camera_half_height < 0.0f)
    {
        map_behavior->camera_origin.y = camera_half_height;
    }
    else if (map_behavior->camera_origin.y + camera_half_height > map_behavior->height)
    {
        map_behavior->camera_origin.y = map_behavior->height - camera_half_height;
    }
}

static void make_map_pin_hierarchy_visible (struct ui_controls_pre_layout_state_t *state,
                                            const struct kan_ui_node_t *node)
{
    KAN_UMI_VALUE_UPDATE_OPTIONAL (drawable, kan_ui_node_drawable_t, id, &node->id)
    if (drawable)
    {
        drawable->hidden_temporary = false;
    }

    KAN_UML_VALUE_READ (child, kan_ui_node_t, parent_id, &node->id) { make_map_pin_hierarchy_visible (state, child); }
}

static void resolve_dirty_map_behavior (struct ui_controls_pre_layout_state_t *state,
                                        struct kan_ui_node_map_behavior_t *map_behavior)
{
    KAN_UMI_VALUE_READ_OPTIONAL (drawable, kan_ui_node_drawable_t, id, &map_behavior->id)
    if (!drawable || drawable->width <= 0 || drawable->height <= 0 || map_behavior->width <= 0.0f ||
        map_behavior->height <= 0.0f)
    {
        return;
    }

    map_behavior_sanitize_camera (map_behavior, drawable);
    const kan_floating_t unit_to_px = (kan_floating_t) drawable->height / (map_behavior->camera_half_height * 2.0f);

    KAN_UML_VALUE_READ (pin, kan_ui_node_map_pin_t, map_id, &map_behavior->id)
    {
        KAN_UMI_VALUE_UPDATE_REQUIRED (node, kan_ui_node_t, id, &pin->id)
        if (pin->sync_location)
        {
            node->element.frame_offset_x = KAN_UI_VALUE_PX (pin->location.x * unit_to_px);
            node->element.frame_offset_y = KAN_UI_VALUE_PX (pin->location.y * unit_to_px);
        }

        if (pin->sync_size)
        {
            node->element.width = KAN_UI_VALUE_PX (pin->size.x * unit_to_px);
            node->element.width = KAN_UI_VALUE_PX (pin->size.y * unit_to_px);
        }

        make_map_pin_hierarchy_visible (state, node);
    }

    map_behavior->dirty = false;
}

UNIVERSE_UI_API KAN_UM_MUTATOR_EXECUTE (ui_controls_pre_layout)
{
    KAN_UML_EVENT_FETCH (text_behavior_on_insert, kan_ui_node_text_behavior_on_insert_event_t)
    {
        KAN_UMI_VALUE_READ_OPTIONAL (text_behavior, kan_ui_node_text_behavior_t, id, &text_behavior_on_insert->id)
        if (!text_behavior)
        {
            continue;
        }

        KAN_UMI_VALUE_UPDATE_REQUIRED (node, kan_ui_node_t, id, &text_behavior_on_insert->id)
        node->event_on_laid_out = true;

        if (text_behavior->sync_ui_size_from_text_secondary)
        {
            sync_ui_size_from_text_secondary (state, node, text_behavior);
        }
    }

    KAN_UML_EVENT_FETCH (text_shaped, kan_text_shaped_t)
    {
        KAN_UMI_VALUE_READ_OPTIONAL (text_behavior, kan_ui_node_text_behavior_t, shaping_unit_id, &text_shaped->id)
        if (!text_behavior)
        {
            continue;
        }

        if (text_behavior->sync_ui_size_from_text_secondary)
        {
            KAN_UMI_VALUE_UPDATE_REQUIRED (node, kan_ui_node_t, id, &text_behavior->id)
            sync_ui_size_from_text_secondary (state, node, text_behavior);
        }
    }

    KAN_UML_SIGNAL_UPDATE (map_behavior, kan_ui_node_map_behavior_t, dirty, true)
    {
        resolve_dirty_map_behavior (state, map_behavior);
    }
}

struct ui_controls_post_layout_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (ui_controls_post_layout)
    KAN_UM_BIND_STATE (ui_controls_post_layout, state)
};

UNIVERSE_UI_API KAN_UM_MUTATOR_DEPLOY (ui_controls_post_layout)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_cpu_static_sections_ensure_initialized ();

    kan_workflow_graph_node_depend_on (workflow_node, KAN_UI_LAYOUT_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_UI_CONTROLS_POST_LAYOUT_BEGIN_CHECKPOINT);
    kan_workflow_graph_node_make_dependency_of (workflow_node, KAN_UI_CONTROLS_POST_LAYOUT_END_CHECKPOINT);
    kan_workflow_graph_node_make_dependency_of (workflow_node, KAN_TEXT_SHAPING_BEGIN_CHECKPOINT);
}

static void text_behavior_post_laid_out (struct ui_controls_post_layout_state_t *state,
                                         const struct kan_ui_singleton_t *ui,
                                         struct kan_ui_node_drawable_t *drawable,
                                         const struct kan_ui_node_text_behavior_t *text_behavior)
{
    KAN_UMI_VALUE_UPDATE_OPTIONAL (shaping_unit, kan_text_shaping_unit_t, id, &text_behavior->shaping_unit_id)
    if (!shaping_unit)
    {
        return;
    }

    if (text_behavior->sync_text_limit_from_ui)
    {
        if (shaping_unit)
        {
            switch (shaping_unit->request.orientation)
            {
            case KAN_TEXT_ORIENTATION_HORIZONTAL:
                if (shaping_unit->request.primary_axis_limit != (kan_instance_size_t) drawable->width)
                {
                    shaping_unit->request.primary_axis_limit = (kan_instance_size_t) drawable->width;
                    shaping_unit->dirty = true;
                }

                break;

            case KAN_TEXT_ORIENTATION_VERTICAL:
                if (shaping_unit->request.primary_axis_limit != (kan_instance_size_t) drawable->height)
                {
                    shaping_unit->request.primary_axis_limit = (kan_instance_size_t) drawable->height;
                    shaping_unit->dirty = true;
                }

                break;
            }
        }
    }

    // It is safe to only update font size from coordinate value after laid out event, as changing source values for
    // coordinates always triggers full layout calculation and laid out events will be sent as well.
    const kan_instance_size_t font_size =
        (kan_instance_size_t) kan_ui_calculate_coordinate (ui, text_behavior->font_size);

    if (shaping_unit->request.font_size != font_size)
    {
        shaping_unit->request.font_size = font_size;
        shaping_unit->dirty = true;
    }

    if (shaping_unit->dirty && text_behavior->sync_ui_size_from_text_secondary)
    {
        // Will be shown again once more.
        drawable->hidden_temporary = true;
    }
}

static void make_map_pin_hierarchy_hidden (struct ui_controls_post_layout_state_t *state,
                                           const struct kan_ui_node_t *node)
{
    KAN_UMI_VALUE_UPDATE_OPTIONAL (drawable, kan_ui_node_drawable_t, id, &node->id)
    if (drawable)
    {
        drawable->hidden_temporary = true;
    }

    KAN_UML_VALUE_READ (child, kan_ui_node_t, parent_id, &node->id) { make_map_pin_hierarchy_hidden (state, child); }
}

static void map_behavior_post_laid_out (struct ui_controls_post_layout_state_t *state,
                                        const struct kan_ui_node_drawable_t *drawable,
                                        struct kan_ui_node_map_behavior_t *map_behavior)
{
    map_behavior_sanitize_camera (map_behavior, drawable);
    map_behavior->dirty = true;

    // When map was laid out, it means that we cannot update pins properly this frame and need to hide them for one
    // frame. It is technically not the best decision, but the easiest one due to the fact that we do not expect maps
    // to be laid out often -- only when initializing UI and when UI scale was changed, therefore it was decided to
    // stick to this decision as the easiest one.
    KAN_UML_VALUE_READ (pin, kan_ui_node_map_pin_t, map_id, &map_behavior->id)
    {
        KAN_UMI_VALUE_READ_REQUIRED (node, kan_ui_node_t, id, &pin->id)
        make_map_pin_hierarchy_hidden (state, node);
    }
}

UNIVERSE_UI_API KAN_UM_MUTATOR_EXECUTE (ui_controls_post_layout)
{
    KAN_UMI_SINGLETON_READ (ui, kan_ui_singleton_t)
    KAN_UML_EVENT_FETCH (laid_out_event, kan_ui_node_laid_out_t)
    {
        KAN_UMI_VALUE_UPDATE_OPTIONAL (drawable, kan_ui_node_drawable_t, id, &laid_out_event->node_id)
        if (!drawable)
        {
            continue;
        }

        KAN_UMI_VALUE_READ_OPTIONAL (text_behavior, kan_ui_node_text_behavior_t, id, &laid_out_event->node_id)
        if (text_behavior)
        {
            text_behavior_post_laid_out (state, ui, drawable, text_behavior);
        }

        KAN_UMI_VALUE_UPDATE_OPTIONAL (line_edit_behavior, kan_ui_node_line_edit_behavior_t, id,
                                       &laid_out_event->node_id)

        if (line_edit_behavior)
        {
            // Need to update text edition visuals for the new text size even if shaping was not triggered by this.
            // Mostly a safeguard logic.
            line_edit_behavior->text_visuals_dirty = true;
        }

        KAN_UMI_VALUE_UPDATE_OPTIONAL (map_behavior, kan_ui_node_map_behavior_t, id, &laid_out_event->node_id)
        if (map_behavior)
        {
            map_behavior_post_laid_out (state, drawable, map_behavior);
        }
    }
}

struct ui_controls_pre_render_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (ui_controls_pre_render)
    KAN_UM_BIND_STATE (ui_controls_pre_render, state)
};

UNIVERSE_UI_API KAN_UM_MUTATOR_DEPLOY (ui_controls_pre_render)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_cpu_static_sections_ensure_initialized ();

    kan_workflow_graph_node_depend_on (workflow_node, KAN_TEXT_SHAPING_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_UI_CONTROLS_POST_LAYOUT_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_UI_CONTROLS_PRE_RENDER_BEGIN_CHECKPOINT);
    kan_workflow_graph_node_make_dependency_of (workflow_node, KAN_UI_CONTROLS_PRE_RENDER_END_CHECKPOINT);
    kan_workflow_graph_node_make_dependency_of (workflow_node, KAN_UI_RENDER_BEGIN_CHECKPOINT);
}

static void regenerate_line_edit_behavior_text_visuals (struct ui_controls_pre_render_state_t *state,
                                                        const struct kan_ui_singleton_t *ui,
                                                        const struct kan_ui_node_line_edit_behavior_t *behavior)
{
    KAN_UMI_VALUE_UPDATE_REQUIRED (drawable, kan_ui_node_drawable_t, id, &behavior->text_id)
    KAN_UMI_VALUE_READ_REQUIRED (shaping_unit, kan_text_shaping_unit_t, id, &behavior->shaping_unit_id)

    // Clear all previous commands, we expect all additional commands to be driven by this behavior.
    drawable->additional_draw_commands.size = 0u;
    KAN_ASSERT (drawable->main_draw_command.type == KAN_UI_DRAW_COMMAND_TEXT)
    drawable->main_draw_command.text.handle_alignment_on_overflow = false;

#define ADJUST_IMAGE_RECT_TO_ORIENTATION(COMMAND)                                                                      \
    switch (shaping_unit->request.orientation)                                                                         \
    {                                                                                                                  \
    case KAN_TEXT_ORIENTATION_HORIZONTAL:                                                                              \
        /* No need to flip. */                                                                                         \
        break;                                                                                                         \
                                                                                                                       \
    case KAN_TEXT_ORIENTATION_VERTICAL:                                                                                \
    {                                                                                                                  \
        /* Flip as basic calculations are written for horizontal. */                                                   \
        kan_instance_offset_t temp = COMMAND->image.custom_x_offset;                                                   \
        COMMAND->image.custom_x_offset = COMMAND->image.custom_y_offset;                                               \
        COMMAND->image.custom_y_offset = temp;                                                                         \
                                                                                                                       \
        temp = COMMAND->image.custom_width;                                                                            \
        COMMAND->image.custom_width = COMMAND->image.custom_height;                                                    \
        COMMAND->image.custom_height = temp;                                                                           \
        break;                                                                                                         \
    }                                                                                                                  \
    }

    kan_instance_offset_t primary_draw_offset = 0;

    // If shaped data is too wide, adjust offsets to handle overflow according to alignment.
    if (shaping_unit->request.primary_axis_limit < shaping_unit->shaped_primary_size)
    {
        switch (shaping_unit->request.alignment)
        {
        case KAN_TEXT_SHAPING_ALIGNMENT_LEFT:
            // Nothing to do.
            break;

        case KAN_TEXT_SHAPING_ALIGNMENT_CENTER:
            primary_draw_offset = shaping_unit->request.primary_axis_limit / 2 - shaping_unit->shaped_primary_size / 2;
            break;

        case KAN_TEXT_SHAPING_ALIGNMENT_RIGHT:
            primary_draw_offset = shaping_unit->request.primary_axis_limit - shaping_unit->shaped_primary_size;
            break;
        }
    }

    if (behavior->cursor_content_location != KAN_INT_MAX (kan_instance_size_t) &&
        behavior->cursor_image_index != KAN_INT_MAX (uint32_t))
    {
        // We use x/y here for simplicity as we can later flip orientation anyway.
        // No need for additional complication in naming with primary/secondary due to that flip.
        kan_instance_offset_t cursor_x = KAN_INT_MAX (kan_instance_offset_t);
        kan_instance_offset_t cursor_y_min = 0;
        kan_instance_offset_t cursor_y_max = 0;

        for (kan_memory_size_t sequence_index = 0u; sequence_index < shaping_unit->shaped_edition_sequences.size;
             ++sequence_index)
        {
            const struct kan_text_shaped_edition_sequence_data_t *sequence =
                &((struct kan_text_shaped_edition_sequence_data_t *)
                      shaping_unit->shaped_edition_sequences.data)[sequence_index];

            if (behavior->cursor_content_location == sequence->end_at_index && sequence->clusters.size > 0u)
            {
                // End of sequence, use last cluster visual max as cursor position.
                cursor_x = ((struct kan_text_shaped_edition_cluster_data_t *)
                                sequence->clusters.data)[sequence->clusters.size - 1u]
                               .visual_max;

                cursor_y_min = sequence->baseline - sequence->ascender;
                cursor_y_max = sequence->baseline - sequence->descender;
                break;
            }

            for (kan_memory_size_t cluster_index = 0u; cluster_index < sequence->clusters.size; ++cluster_index)
            {
                const struct kan_text_shaped_edition_cluster_data_t *cluster =
                    &((struct kan_text_shaped_edition_cluster_data_t *) sequence->clusters.data)[cluster_index];

                if (cluster->start_at_index == behavior->cursor_content_location)
                {
                    cursor_x = cluster->visual_cursor_position;
                    cursor_y_min = sequence->baseline - sequence->ascender;
                    cursor_y_max = sequence->baseline - sequence->descender;
                    break;
                }
            }
        }

        if (cursor_x != KAN_INT_MAX (kan_instance_offset_t))
        {
            struct kan_ui_draw_command_data_t *command =
                kan_dynamic_array_add_last (&drawable->additional_draw_commands);

            if (!command)
            {
                kan_dynamic_array_set_capacity (&drawable->additional_draw_commands,
                                                KAN_MAX (1u, drawable->additional_draw_commands.size * 2u));
                command = kan_dynamic_array_add_last (&drawable->additional_draw_commands);
            }

            command->ui_mark = behavior->cursor_ui_mark;
            command->animation_start_time_s = ui->animation_global_time_s;
            command->early = false;

            command->type = KAN_UI_DRAW_COMMAND_IMAGE;
            command->image.record_index = behavior->cursor_image_index;
            command->image.allow_override = false;
            command->image.custom_rect = true;

            const kan_instance_offset_t cursor_width = kan_ui_calculate_coordinate (ui, behavior->cursor_width);
            const kan_instance_offset_t cursor_safe_space =
                kan_ui_calculate_coordinate (ui, behavior->cursor_safe_space);

            command->image.custom_x_offset = cursor_x - cursor_width / 2;
            command->image.custom_y_offset = cursor_y_min;
            command->image.custom_width = cursor_width;
            command->image.custom_height = cursor_y_max - cursor_y_min;
            ADJUST_IMAGE_RECT_TO_ORIENTATION (command)

            const kan_instance_offset_t cursor_x_with_offset = cursor_x + primary_draw_offset;
            if (cursor_x_with_offset < cursor_safe_space)
            {
                primary_draw_offset = KAN_MIN (cursor_safe_space - cursor_x, cursor_width / 2);
            }
            else if (cursor_x_with_offset + cursor_safe_space >
                     (kan_instance_offset_t) shaping_unit->request.primary_axis_limit)
            {
                const kan_instance_offset_t offset_right_border_limit =
                    (kan_instance_offset_t) shaping_unit->request.primary_axis_limit -
                    (kan_instance_offset_t) shaping_unit->shaped_primary_size - cursor_width / 2;

                primary_draw_offset = KAN_MAX ((kan_instance_offset_t) shaping_unit->request.primary_axis_limit -
                                                   (cursor_x + cursor_safe_space + cursor_width / 2),
                                               offset_right_border_limit);
            }
        }
    }

    if (behavior->selection_content_min != KAN_INT_MAX (kan_instance_size_t) &&
        behavior->selection_content_max != KAN_INT_MAX (kan_instance_size_t) &&
        behavior->selection_image_index != KAN_INT_MAX (uint32_t))
    {
        const kan_instance_offset_t selection_leeway = kan_ui_calculate_coordinate (ui, behavior->selection_leeway);
        for (kan_memory_size_t sequence_index = 0u; sequence_index < shaping_unit->shaped_edition_sequences.size;
             ++sequence_index)
        {
            const struct kan_text_shaped_edition_sequence_data_t *sequence =
                &((struct kan_text_shaped_edition_sequence_data_t *)
                      shaping_unit->shaped_edition_sequences.data)[sequence_index];

            kan_instance_offset_t selection_min = KAN_INT_MAX (kan_instance_offset_t);
            kan_instance_offset_t selection_max = KAN_INT_MAX (kan_instance_offset_t);

#define PUSH_SELECTION_DRAW_COMMAND                                                                                    \
    {                                                                                                                  \
        struct kan_ui_draw_command_data_t *command = kan_dynamic_array_add_last (&drawable->additional_draw_commands); \
        if (!command)                                                                                                  \
        {                                                                                                              \
            kan_dynamic_array_set_capacity (&drawable->additional_draw_commands,                                       \
                                            KAN_MAX (1u, drawable->additional_draw_commands.size * 2u));               \
            command = kan_dynamic_array_add_last (&drawable->additional_draw_commands);                                \
        }                                                                                                              \
                                                                                                                       \
        command->ui_mark = behavior->selection_ui_mark;                                                                \
        command->animation_start_time_s = ui->animation_global_time_s;                                                 \
        command->early = true;                                                                                         \
                                                                                                                       \
        command->type = KAN_UI_DRAW_COMMAND_IMAGE;                                                                     \
        command->image.record_index = behavior->selection_image_index;                                                 \
        command->image.allow_override = false;                                                                         \
        command->image.custom_rect = true;                                                                             \
                                                                                                                       \
        command->image.custom_x_offset = selection_min - selection_leeway;                                             \
        command->image.custom_y_offset = sequence->baseline - sequence->ascender;                                      \
        command->image.custom_width = selection_max - selection_min + selection_leeway * 2u;                           \
        command->image.custom_height = sequence->ascender - sequence->descender;                                       \
                                                                                                                       \
        ADJUST_IMAGE_RECT_TO_ORIENTATION (command)                                                                     \
        selection_min = KAN_INT_MAX (kan_instance_offset_t);                                                           \
        selection_max = KAN_INT_MAX (kan_instance_offset_t);                                                           \
    }

            for (kan_memory_size_t cluster_index = 0u; cluster_index < sequence->clusters.size; ++cluster_index)
            {
                const struct kan_text_shaped_edition_cluster_data_t *cluster =
                    &((struct kan_text_shaped_edition_cluster_data_t *) sequence->clusters.data)[cluster_index];

                if (cluster->start_at_index >= behavior->selection_content_min &&
                    cluster->start_at_index < behavior->selection_content_max)
                {
                    if (selection_min == KAN_INT_MAX (kan_instance_offset_t))
                    {
                        selection_min = cluster->visual_min;
                        selection_max = cluster->visual_max;
                    }
                    else
                    {
                        selection_min = KAN_MIN (selection_min, cluster->visual_min);
                        selection_max = KAN_MAX (selection_min, cluster->visual_max);
                    }
                }
                else if (selection_min != KAN_INT_MAX (kan_instance_offset_t))
                {
                    PUSH_SELECTION_DRAW_COMMAND
                }
            }

            if (selection_min != KAN_INT_MAX (kan_instance_offset_t))
            {
                PUSH_SELECTION_DRAW_COMMAND
            }
#undef PUSH_SELECTION_DRAW_COMMAND
        }
    }

#undef ADJUST_IMAGE_RECT_TO_ORIENTATION

    // During selection, we freeze draw offset as a simplistic solution for line edits with text size overflows.
    // It is not a full-fledged solution like solutions in browsers, but should be fine right now.
    const bool primary_draw_offset_frozen = behavior->selection_content_min != KAN_INT_MAX (kan_instance_size_t) &&
                                            behavior->selection_content_max != KAN_INT_MAX (kan_instance_size_t);

    if (!primary_draw_offset_frozen)
    {
        switch (shaping_unit->request.orientation)
        {
        case KAN_TEXT_ORIENTATION_HORIZONTAL:
            drawable->draw_offset_x = primary_draw_offset;
            drawable->draw_offset_y = 0;
            break;

        case KAN_TEXT_ORIENTATION_VERTICAL:
            drawable->draw_offset_x = 0;
            drawable->draw_offset_y = primary_draw_offset;
            break;
        }
    }

    // Make sure that we do not use more memory than needed.
    kan_dynamic_array_set_capacity (&drawable->additional_draw_commands, drawable->additional_draw_commands.size);
}

UNIVERSE_UI_API KAN_UM_MUTATOR_EXECUTE (ui_controls_pre_render)
{
    KAN_UMI_SINGLETON_READ (ui, kan_ui_singleton_t)
    KAN_UML_EVENT_FETCH (text_shaped, kan_text_shaped_t)
    {
        KAN_UMI_VALUE_UPDATE_OPTIONAL (line_edit_behavior, kan_ui_node_line_edit_behavior_t, shaping_unit_id,
                                       &text_shaped->id)

        if (line_edit_behavior)
        {
            regenerate_line_edit_behavior_text_visuals (state, ui, line_edit_behavior);
            line_edit_behavior->text_visuals_dirty = false;
        }
    }

    KAN_UML_SIGNAL_UPDATE (line_edit_behavior, kan_ui_node_line_edit_behavior_t, text_visuals_dirty, true)
    {
        regenerate_line_edit_behavior_text_visuals (state, ui, line_edit_behavior);
        line_edit_behavior->text_visuals_dirty = false;
    }
}

void kan_ui_input_singleton_init (struct kan_ui_input_singleton_t *instance)
{
    instance->event_iterator = KAN_HANDLE_SET_INVALID (kan_application_system_event_iterator_t);
    instance->linked_window_handle = KAN_HANDLE_SET_INVALID (kan_application_system_window_t);
    instance->linked_window_id = KAN_TYPED_ID_32_SET_INVALID (kan_platform_window_id_t);

    instance->viewport_offset_x = 0;
    instance->viewport_offset_y = 0;

    instance->input_receiver_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);

    instance->mouse_button_down_flags = 0u;
    instance->mouse_button_down_inclusive_flags = 0u;
    instance->press_started_on_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    instance->press_filtered_in = false;

    instance->last_mouse_x = 0;
    instance->last_mouse_y = 0;
    instance->current_hovered_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
}

void kan_ui_node_hit_box_init (struct kan_ui_node_hit_box_t *instance)
{
    instance->id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);

    instance->interactable = false;
    instance->scroll_passthrough = false;
    instance->disabled = false;

    instance->interactable_style = NULL;
    instance->mouse_button_down_flags = 1u << KAN_PLATFORM_MOUSE_BUTTON_LEFT;

    kan_dynamic_array_init (&instance->propagate_interaction_visuals, 0u, sizeof (kan_ui_node_id_t),
                            alignof (kan_ui_node_id_t), kan_allocation_group_stack_get ());
}

void kan_ui_node_hit_box_shutdown (struct kan_ui_node_hit_box_t *instance)
{
    kan_dynamic_array_shutdown (&instance->propagate_interaction_visuals);
}

void kan_ui_node_text_behavior_init (struct kan_ui_node_text_behavior_t *instance)
{
    instance->id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    instance->shaping_unit_id = KAN_TYPED_ID_32_SET_INVALID (kan_text_shaping_unit_id_t);
    instance->font_size = KAN_UI_VALUE_PX (14.0f);
    instance->sync_text_limit_from_ui = false;
    instance->sync_ui_size_from_text_secondary = false;
}

void kan_ui_node_scroll_behavior_init (struct kan_ui_node_scroll_behavior_t *instance)
{
    instance->id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    instance->container_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);

    instance->horizontal_line_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    instance->horizontal_knob_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);

    instance->vertical_line_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    instance->vertical_knob_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);

    instance->horizontal = false;
    instance->vertical = false;

    instance->offset_coordinate_type_x = KAN_UI_PT;
    instance->offset_coordinate_type_y = KAN_UI_PT;

    instance->mouse_speed_x.type = KAN_UI_PT;
    instance->mouse_speed_x.value = 5000.0f;

    instance->mouse_speed_y.type = KAN_UI_PT;
    instance->mouse_speed_y.value = -5000.0f;

    instance->lines_always_visible = false;
    instance->line_visibility_s = 0.5f;
}

void kan_ui_node_line_edit_behavior_init (struct kan_ui_node_line_edit_behavior_t *instance)
{
    instance->id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    instance->text_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    instance->shaping_unit_id = KAN_TYPED_ID_32_SET_INVALID (kan_text_shaping_unit_id_t);

    instance->interactable_style_regular = NULL;
    instance->interactable_style_selected = NULL;

    // Always dirty until initial processing.
    instance->content_dirty = true;
    instance->text_visuals_dirty = true;

    kan_dynamic_array_init (&instance->content_utf8, 1u, sizeof (uint8_t), 1u, kan_allocation_group_stack_get ());
    instance->content_utf8.size = 1u;
    instance->content_utf8.data[0u] = '\0';

    instance->content_style = NULL;
    instance->content_mark = 0u;

    instance->cursor_image_index = KAN_INT_MAX (uint32_t);
    instance->cursor_ui_mark = KAN_UI_DEFAULT_COMMAND_MAKE_MARK (0u, KAN_UI_DEFAULT_MARK_FLAG_BLINK);
    instance->cursor_width = KAN_UI_VALUE_PT (5.0f);
    instance->cursor_safe_space = KAN_UI_VALUE_PT (0.0f);

    instance->selection_image_index = KAN_INT_MAX (uint32_t);
    instance->selection_ui_mark = KAN_UI_DEFAULT_COMMAND_MAKE_MARK (0u, KAN_UI_DEFAULT_MARK_FLAG_NONE);
    instance->selection_leeway = KAN_UI_VALUE_PT (0.0f);

    instance->cursor_content_location = KAN_INT_MAX (kan_instance_size_t);
    instance->selection_content_min = KAN_INT_MAX (kan_instance_size_t);
    instance->selection_content_max = KAN_INT_MAX (kan_instance_size_t);

    instance->content_type = KAN_UI_NODE_LINE_EDIT_CONTENT_TYPE_ANY;
    instance->content_style_when_invalid = NULL;
    instance->content_mark_when_invalid = 0u;
}

void kan_ui_node_line_edit_behavior_set_content (struct kan_ui_node_line_edit_behavior_t *instance,
                                                 const char *null_terminated_utf8_content,
                                                 kan_interned_string_t content_style,
                                                 uint32_t content_mark)
{
    const kan_instance_size_t input_length = (kan_instance_size_t) strlen (null_terminated_utf8_content);
    instance->content_utf8.size = 0u;
    kan_dynamic_array_set_capacity (&instance->content_utf8, input_length + 1u);
    instance->content_utf8.size = input_length + 1u;
    memcpy (instance->content_utf8.data, null_terminated_utf8_content, input_length + 1u);

    instance->content_style = content_style;
    instance->content_mark = content_mark;
    instance->content_dirty = true;
}

void kan_ui_node_line_edit_behavior_set_content_type (struct kan_ui_node_line_edit_behavior_t *instance,
                                                      enum kan_ui_node_line_edit_content_type_t content_type,
                                                      kan_interned_string_t content_style_when_invalid,
                                                      uint32_t content_mark_when_invalid)
{
    instance->content_type = content_type;
    instance->content_style_when_invalid = content_style_when_invalid;
    instance->content_mark_when_invalid = content_mark_when_invalid;

    switch (content_type)
    {
    case KAN_UI_NODE_LINE_EDIT_CONTENT_TYPE_ANY:
        break;

    case KAN_UI_NODE_LINE_EDIT_CONTENT_TYPE_UINT:
        instance->content_uint.has_parsed_content = false;
        instance->content_uint.has_limits = false;
        instance->content_uint.parsed_content = 0u;
        instance->content_uint.min = 0u;
        instance->content_uint.max = 0u;
        break;

    case KAN_UI_NODE_LINE_EDIT_CONTENT_TYPE_SINT:
        instance->content_sint.has_parsed_content = false;
        instance->content_sint.has_limits = false;
        instance->content_sint.parsed_content = 0;
        instance->content_sint.min = 0;
        instance->content_sint.max = 0;
        break;

    case KAN_UI_NODE_LINE_EDIT_CONTENT_TYPE_FLOAT:
        instance->content_float.has_parsed_content = false;
        instance->content_float.has_limits = false;
        instance->content_float.parsed_content = 0.0f;
        instance->content_float.min = 0.0f;
        instance->content_float.max = 0.0f;
        break;
    }

    instance->content_dirty = true;
}

void kan_ui_node_line_edit_behavior_set_content_uint_limits (struct kan_ui_node_line_edit_behavior_t *instance,
                                                             kan_instance_size_t min,
                                                             kan_instance_size_t max)
{
    KAN_ASSERT (instance->content_type == KAN_UI_NODE_LINE_EDIT_CONTENT_TYPE_UINT)
    instance->content_uint.has_limits = true;
    instance->content_uint.min = min;
    instance->content_uint.max = max;
    instance->content_dirty = true;
}

void kan_ui_node_line_edit_behavior_set_content_sint_limits (struct kan_ui_node_line_edit_behavior_t *instance,
                                                             kan_instance_offset_t min,
                                                             kan_instance_offset_t max)
{
    KAN_ASSERT (instance->content_type == KAN_UI_NODE_LINE_EDIT_CONTENT_TYPE_SINT)
    instance->content_sint.has_limits = true;
    instance->content_sint.min = min;
    instance->content_sint.max = max;
    instance->content_dirty = true;
}

void kan_ui_node_line_edit_behavior_set_content_float_limits (struct kan_ui_node_line_edit_behavior_t *instance,
                                                              kan_floating_t min,
                                                              kan_floating_t max)
{
    KAN_ASSERT (instance->content_type == KAN_UI_NODE_LINE_EDIT_CONTENT_TYPE_FLOAT)
    instance->content_float.has_limits = true;
    instance->content_float.min = min;
    instance->content_float.max = max;
    instance->content_dirty = true;
}

void kan_ui_node_line_edit_behavior_shutdown (struct kan_ui_node_line_edit_behavior_t *instance)
{
    kan_dynamic_array_shutdown (&instance->content_utf8);
}

void kan_ui_node_map_behavior_init (struct kan_ui_node_map_behavior_t *instance)
{
    instance->id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    instance->movement_enabled = true;
    instance->zoom_enabled = true;
    instance->dirty = true;

    instance->camera_origin = kan_make_float_vector_2_t (0.0f, 0.0f);
    instance->camera_half_height = 1.0f;
    instance->camera_min_half_height = 1.0f;
    instance->camera_max_half_height = 1.0f;

    instance->scroll_zoom_speed = 10.0f;
    instance->width = 1.0f;
    instance->height = 1.0f;
}

void kan_ui_node_map_pin_init (struct kan_ui_node_map_pin_t *instance)
{
    instance->id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    instance->map_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);

    instance->location = kan_make_float_vector_2_t (0.0f, 0.0f);
    instance->size = kan_make_float_vector_2_t (0.0f, 0.0f);

    instance->sync_location = true;
    instance->sync_size = false;
}

void kan_ui_node_popup_behavior_init (struct kan_ui_node_popup_behavior_t *instance)
{
    instance->id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    instance->trigger_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    instance->trigger_flags = 0u;
    instance->hide_flags = 0u;
    instance->hover_timer_s = 1.0f;
    instance->hide_timer_s = 0.5f;
}

void kan_ui_node_hide_popup_on_press_init (struct kan_ui_node_hide_popup_on_press_t *instance)
{
    instance->id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    instance->popup_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
}
