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

struct kan_ui_node_down_mark_t
{
    kan_ui_node_id_t id;
    float down_until_s;
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

struct kan_ui_node_hit_box_on_style_change_event_t
{
    kan_ui_node_id_t id;
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_hit_box_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_on_change_event_t kan_ui_node_hit_box_on_style_change_event = {
    .event_type = "kan_ui_node_hit_box_on_style_change_event_t",
    .observed_fields_count = 1u,
    .observed_fields =
        (struct kan_repository_field_path_t[]) {
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
    float visible_until_s;
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
    kan_workflow_graph_node_depend_on (workflow_node, KAN_UI_TIME_BEGIN_CHECKPOINT);
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
        if (!drawable || drawable->fully_clipped_out || (found && found_index > drawable->draw_index))
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

#define FOCUS_FLAGS_MASK (KAN_UI_DEFAULT_MARK_FLAG_HOVERED | KAN_UI_DEFAULT_MARK_FLAG_DOWN)
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

    for (kan_loop_size_t index = 0u; index < drawable->additional_draw_commands.size; ++index)
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
};

static inline enum hit_box_interaction_flags_t calculate_hit_box_interaction_flags (
    struct ui_controls_input_state_t *state,
    const struct kan_ui_input_singleton_t *public,
    kan_ui_node_id_t hit_box_id,
    bool force_not_down)
{
    enum hit_box_interaction_flags_t flags = 0u;
    if (KAN_TYPED_ID_32_IS_EQUAL (public->current_hovered_id, hit_box_id))
    {
        flags |= FOCUS_STATE_HOVERED;
    }

    if (!force_not_down)
    {
        KAN_UMI_VALUE_READ_OPTIONAL (down_mark, kan_ui_node_down_mark_t, id, &hit_box_id)
        if (down_mark)
        {
            flags |= FOCUS_STATE_DOWN;
        }
    }

    return flags;
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

    for (kan_loop_size_t index = 0u; index < bundle->available_bundle.hit_box_interaction_styles.size; ++index)
    {
        const struct kan_resource_ui_hit_box_interaction_style_t *style =
            &((struct kan_resource_ui_hit_box_interaction_style_t *)
                  bundle->available_bundle.hit_box_interaction_styles.data)[index];

        if (style->name == style_name)
        {
            if (flags & FOCUS_STATE_DOWN)
            {
                return query_image (state, bundle, style->down_image);
            }
            else if (flags & FOCUS_STATE_HOVERED)
            {
                return query_image (state, bundle, style->hovered_image);
            }

            return query_image (state, bundle, style->regular_image);
        }
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
        KAN_UMI_VALUE_UPDATE_OPTIONAL (line_drawable, kan_ui_node_drawable_t, id, &line_state->id)
        if (line_drawable)
        {
            line_drawable->hidden = false;
        }

        KAN_UMI_VALUE_UPDATE_OPTIONAL (knob_drawable, kan_ui_node_drawable_t, id, &line_state->knob_id)
        if (knob_drawable)
        {
            knob_drawable->hidden = false;
        }
    }

    if (behavior->lines_always_visible || interacted_with_hit_box ||
        KAN_TYPED_ID_32_IS_EQUAL (line_state->id, public->mouse_button_down_on_id))
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
        calculate_hit_box_interaction_flags (state, public, hit_box->id, force_not_down);

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

    use_hit_box_interaction_visuals (state, hit_box->id, ui_mark_flags, image_index);
    for (kan_loop_size_t index = 0u; index < hit_box->propagate_interaction_visuals.size; ++index)
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
                                           float scroll_value_px)
{
    KAN_UMI_VALUE_READ_OPTIONAL (horizontal_line_drawable, kan_ui_node_drawable_t, id, &behavior->horizontal_line_id)
    KAN_UMI_VALUE_UPDATE_OPTIONAL (horizontal_knob_node, kan_ui_node_t, id, &behavior->horizontal_knob_id)

    if (horizontal_line_drawable && horizontal_knob_node)
    {
        const float knob_size_ratio = KAN_MIN (1.0f, (float) main_drawable->width / (float) container_drawable->width);
        const float knob_size_px = knob_size_ratio * (float) horizontal_line_drawable->width;
        horizontal_knob_node->element.width = KAN_UI_VALUE_PX (knob_size_px);

        const float knob_offset_ratio = scroll_value_px / (float) container_drawable->width;
        const float knob_offset_px = knob_offset_ratio * (float) horizontal_line_drawable->width;
        horizontal_knob_node->element.frame_offset_x = KAN_UI_VALUE_PX (knob_offset_px);
    }
}

static void update_scroll_vertical_knob (struct ui_controls_input_state_t *state,
                                         const struct kan_ui_node_scroll_behavior_t *behavior,
                                         const struct kan_ui_node_drawable_t *main_drawable,
                                         const struct kan_ui_node_drawable_t *container_drawable,
                                         float scroll_value_px)
{
    KAN_UMI_VALUE_READ_OPTIONAL (vertical_line_drawable, kan_ui_node_drawable_t, id, &behavior->vertical_line_id)
    KAN_UMI_VALUE_UPDATE_OPTIONAL (vertical_knob_node, kan_ui_node_t, id, &behavior->vertical_knob_id)

    if (vertical_line_drawable && vertical_knob_node)
    {
        const float knob_size_ratio =
            KAN_MIN (1.0f, (float) main_drawable->height / (float) container_drawable->height);
        const float knob_size_px = knob_size_ratio * (float) vertical_line_drawable->height;
        vertical_knob_node->element.height = KAN_UI_VALUE_PX (knob_size_px);

        const float knob_offset_ratio = scroll_value_px / (float) container_drawable->height;
        const float knob_offset_px = knob_offset_ratio * (float) vertical_line_drawable->height;
        vertical_knob_node->element.frame_offset_y = KAN_UI_VALUE_PX (knob_offset_px);
    }
}

static inline void update_horizontal_scroll_absolute (struct ui_controls_input_state_t *state,
                                                      const struct kan_ui_singleton_t *ui,
                                                      struct kan_ui_node_scroll_behavior_t *behavior,
                                                      const struct kan_ui_node_drawable_t *main_drawable,
                                                      struct kan_ui_node_t *container_node,
                                                      const struct kan_ui_node_drawable_t *container_drawable,
                                                      float value_px)
{
    const float new_scroll_px =
        KAN_CLAMP (value_px, 0.0f, (float) (KAN_MAX (0, container_drawable->width - main_drawable->width)));
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
                                                    float value_px)
{
    const float new_scroll_px =
        KAN_CLAMP (value_px, 0.0f, (float) (KAN_MAX (0, container_drawable->height - main_drawable->height)));
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
        const float current_scroll_px = kan_ui_calculate_coordinate_floating (ui, container_node->render.scroll_x);
        const float new_scroll_px = KAN_CLAMP (current_scroll_px, 0.0f,
                                               (float) (KAN_MAX (0, container_drawable->width - main_drawable->width)));

        container_node->render.scroll_x =
            kan_ui_coordinate_from_pixels (ui, behavior->offset_coordinate_type_x, new_scroll_px);
        update_scroll_horizontal_knob (state, behavior, main_drawable, container_drawable, new_scroll_px);
    }

    if (behavior->vertical)
    {
        const float current_scroll_px = kan_ui_calculate_coordinate_floating (ui, container_node->render.scroll_y);
        const float new_scroll_px = KAN_CLAMP (
            current_scroll_px, 0.0f, (float) (KAN_MAX (0, container_drawable->height - main_drawable->height)));

        container_node->render.scroll_y =
            kan_ui_coordinate_from_pixels (ui, behavior->offset_coordinate_type_y, new_scroll_px);
        update_scroll_vertical_knob (state, behavior, main_drawable, container_drawable, new_scroll_px);
    }
}

static void hide_scroll_line (struct ui_controls_input_state_t *state,
                              struct kan_ui_node_scroll_line_state_t *line_state)
{
    KAN_UMI_VALUE_UPDATE_OPTIONAL (line_drawable, kan_ui_node_drawable_t, id, &line_state->id)
    if (line_drawable)
    {
        line_drawable->hidden = true;
    }

    KAN_UMI_VALUE_UPDATE_OPTIONAL (knob_drawable, kan_ui_node_drawable_t, id, &line_state->knob_id)
    if (knob_drawable)
    {
        knob_drawable->hidden = true;
    }

    line_state->visible_until_s = 0.0f;
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

static void process_line_edit_behavior_lifetime (struct ui_controls_input_state_t *state)
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

static void sanitize_input_receiver_selection (struct ui_controls_input_state_t *state,
                                               struct kan_ui_input_singleton_t *public)
{
    KAN_UMI_SINGLETON_WRITE (private, ui_controls_input_private_singleton_t)
    if (KAN_TYPED_ID_32_IS_VALID (public->input_receiver_id))
    {
        KAN_UMI_VALUE_READ_OPTIONAL (node, kan_ui_node_t, id, &public->input_receiver_id)
        if (!node)
        {
            if (private->input_receiver_requested_text_input && KAN_HANDLE_IS_VALID (public->linked_window_handle))
            {
                kan_application_window_remove_text_listener (state->application_system_handle,
                                                             public->linked_window_handle);
            }

            public->input_receiver_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
            private->input_receiver_requested_text_input = false;
        }
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
        }
    }
}

static void line_edit_erase_subsequence (struct kan_ui_node_line_edit_behavior_t *behavior,
                                         kan_instance_size_t min,
                                         kan_instance_size_t max)
{
    KAN_ASSERT (min < max)
    KAN_ASSERT (max < behavior->content_utf8.size)
    const kan_instance_size_t size = max - min;

    // If not full erase, move data.
    if (behavior->content_utf8.size - 1u != size)
    {
        for (kan_loop_size_t index = min; index < behavior->content_utf8.size - size - 1u; ++index)
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
    const kan_instance_size_t input_length = strlen (text);

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

    for (kan_loop_size_t index = behavior->content_utf8.size - 1u;
         index >= behavior->cursor_content_location && index != KAN_INT_MAX (kan_loop_size_t); --index)
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

    for (kan_loop_size_t sequence_index = 0u; sequence_index < shaping_unit->shaped_edition_sequences.size;
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

        for (kan_loop_size_t cluster_index = 0u; cluster_index < sequence->clusters.size; ++cluster_index)
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
        while (sequence_index != KAN_INT_MAX (kan_loop_size_t))
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

static void process_key_down_internal (struct ui_controls_input_state_t *state,
                                       struct kan_ui_input_singleton_t *public,
                                       const struct kan_ui_singleton_t *ui,
                                       const struct kan_platform_application_event_t *event)
{
    KAN_UMI_SINGLETON_WRITE (private, ui_controls_input_private_singleton_t)
    if (!KAN_TYPED_ID_32_IS_VALID (public->input_receiver_id) ||
        // We do not process keyboard input while mouse is down.
        public->mouse_button_down_flags != 0u)
    {
        return;
    }

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
                    uint8_t *iterator =
                        line_edit_behavior->content_utf8.data + line_edit_behavior->cursor_content_location;

                    if (kan_text_utf8_next (&iterator, line_edit_behavior->content_utf8.data +
                                                           line_edit_behavior->content_utf8.size - 1u))
                    {
                        const kan_instance_size_t next_at = iterator - line_edit_behavior->content_utf8.data;
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

static void prolong_hit_box_down_visuals (struct ui_controls_input_state_t *state,
                                          struct kan_ui_input_singleton_t *public,
                                          const struct kan_ui_bundle_singleton_t *bundle,
                                          const struct kan_ui_node_hit_box_t *hit_box)
{
    const struct kan_resource_ui_hit_box_interaction_style_t *selected_style = NULL;

    for (kan_loop_size_t index = 0u; index < bundle->available_bundle.hit_box_interaction_styles.size; ++index)
    {
        const struct kan_resource_ui_hit_box_interaction_style_t *style =
            &((struct kan_resource_ui_hit_box_interaction_style_t *)
                  bundle->available_bundle.hit_box_interaction_styles.data)[index];

        if (style->name == hit_box->interactable_style)
        {
            selected_style = style;
            break;
        }
    }

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
    }

    apply_hit_box_interaction_visuals (state, public, bundle, hit_box, false);
}

static void apply_scroll_relative_input (struct ui_controls_input_state_t *state,
                                         struct kan_ui_input_singleton_t *public,
                                         const struct kan_ui_singleton_t *ui,
                                         const struct kan_ui_node_scroll_behavior_t *behavior,
                                         float delta_x_px,
                                         float delta_y_px,
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
        const float current_scroll_px = kan_ui_calculate_coordinate_floating (ui, container_node->render.scroll_x);
        const float new_scroll_px = KAN_CLAMP (current_scroll_px + delta_x_px, 0.0f,
                                               (float) (KAN_MAX (0, container_drawable->width - main_drawable->width)));

        container_node->render.scroll_x =
            kan_ui_coordinate_from_pixels (ui, behavior->offset_coordinate_type_x, new_scroll_px);
        update_scroll_horizontal_knob (state, behavior, main_drawable, container_drawable, new_scroll_px);

        KAN_UMI_VALUE_UPDATE_REQUIRED (line_state, kan_ui_node_scroll_line_state_t, id, &behavior->horizontal_line_id)
        update_interacted_scroll_line_visibility (state, public, line_state, behavior, false);
    }

    if (behavior->vertical && !KAN_FLOATING_IS_NEAR (delta_y_px, 0.0f))
    {
        const float current_scroll_px = kan_ui_calculate_coordinate_floating (ui, container_node->render.scroll_y);
        const float new_scroll_px =
            KAN_CLAMP (current_scroll_px + delta_y_px, 0.0f,
                       (float) (KAN_MAX (0, container_drawable->height - main_drawable->height)));

        container_node->render.scroll_y =
            kan_ui_coordinate_from_pixels (ui, behavior->offset_coordinate_type_y, new_scroll_px);
        update_scroll_vertical_knob (state, behavior, main_drawable, container_drawable, new_scroll_px);

        KAN_UMI_VALUE_UPDATE_REQUIRED (line_state, kan_ui_node_scroll_line_state_t, id, &behavior->vertical_line_id)
        update_interacted_scroll_line_visibility (state, public, line_state, behavior, false);
    }
}

static void apply_scroll_absolute_horizontal (struct ui_controls_input_state_t *state,
                                              struct kan_ui_input_singleton_t *public,
                                              const struct kan_ui_singleton_t *ui,
                                              const struct kan_ui_node_scroll_behavior_t *behavior,
                                              struct kan_ui_node_scroll_line_state_t *line_state,
                                              float value_px)
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
        const float new_scroll_px =
            KAN_CLAMP (value_px, 0.0f, (float) (KAN_MAX (0, container_drawable->width - main_drawable->width)));

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
                                            float value_px)
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
        const float new_scroll_px =
            KAN_CLAMP (value_px, 0.0f, (float) (KAN_MAX (0, container_drawable->height - main_drawable->height)));

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
            const float target_px =
                (float) local_offset * (float) container_drawable->width / (float) line_drawable->width;

            // Due to access from inside interacted visibility update, a little bit ad-hoc.
            KAN_UM_ACCESS_CLOSE_IMMEDIATELY (line_drawable);
            apply_scroll_absolute_horizontal (state, public, ui, behavior, line_state, target_px);
            break;
        }

        case KAN_UI_NODE_SCROLL_LINE_CLASS_VERTICAL:
        {
            const kan_instance_offset_t local_offset =
                KAN_MAX (0, public->last_mouse_y - line_drawable->global_y - private->press_knob_offset);
            const float target_px =
                (float) local_offset * (float) container_drawable->height / (float) line_drawable->height;

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

    for (kan_loop_size_t index = 0u; index < shaping_unit->shaped_edition_sequences.size; ++index)
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
    kan_loop_size_t cluster_index = 0u;

    while (cluster_index < selected_sequence->clusters.size)
    {
        const struct kan_text_shaped_edition_cluster_data_t *cluster =
            &((struct kan_text_shaped_edition_cluster_data_t *) selected_sequence->clusters.data)[cluster_index];

        // Separate out-of-order processing for bidi fragments.
        if (!cluster->matching_reading_direction)
        {
            kan_instance_size_t run_ends_at = selected_sequence->clusters.size;
            for (kan_loop_size_t scan_index = cluster_index + 1u; scan_index < selected_sequence->clusters.size;
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

            for (kan_loop_size_t run_index = run_ends_at - 1u;
                 // Overflow guard is important here.
                 run_index >= cluster_index && run_index != KAN_INT_MAX (kan_loop_size_t); --run_index)
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

                for (kan_loop_size_t scan_index = cluster_index + 2u; scan_index < selected_sequence->clusters.size;
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

static void on_press_motion_internal (struct ui_controls_input_state_t *state,
                                      struct kan_ui_input_singleton_t *public,
                                      const struct kan_ui_singleton_t *ui)
{
    KAN_UMI_SINGLETON_WRITE (private, ui_controls_input_private_singleton_t)
    KAN_UMI_VALUE_UPDATE_OPTIONAL (scroll_line_state, kan_ui_node_scroll_line_state_t, id,
                                   &public->mouse_button_down_on_id)

    if (scroll_line_state)
    {
        place_scroll_line_knob_at_press (state, public, private, ui, scroll_line_state);
    }

    KAN_UMI_VALUE_UPDATE_OPTIONAL (line_edit_behavior, kan_ui_node_line_edit_behavior_t, id, &public->input_receiver_id)
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
}

static inline void deselect_input_receiver_behavior (struct ui_controls_input_state_t *state,
                                                     struct kan_ui_input_singleton_t *public,
                                                     struct ui_controls_input_private_singleton_t *private)
{
    KAN_UMI_VALUE_UPDATE_OPTIONAL (line_edit_behavior, kan_ui_node_line_edit_behavior_t, id, &public->input_receiver_id)
    if (line_edit_behavior)
    {
        clear_line_edit_selection_visuals (state, line_edit_behavior);
    }

    if (private->input_receiver_requested_text_input && KAN_HANDLE_IS_VALID (public->linked_window_handle))
    {
        kan_application_window_remove_text_listener (state->application_system_handle, public->linked_window_handle);
    }

    public->input_receiver_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    private->input_receiver_requested_text_input = false;
}

static void on_press_begin_internal (struct ui_controls_input_state_t *state,
                                     struct kan_ui_input_singleton_t *public,
                                     const struct kan_ui_singleton_t *ui)
{
    KAN_UMI_SINGLETON_WRITE (private, ui_controls_input_private_singleton_t)
    KAN_UMI_VALUE_UPDATE_OPTIONAL (scroll_line_state, kan_ui_node_scroll_line_state_t, id,
                                   &public->mouse_button_down_on_id)

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
        !KAN_TYPED_ID_32_IS_EQUAL (public->input_receiver_id, public->mouse_button_down_on_id))
    {
        deselect_input_receiver_behavior (state, public, private);
    }

    KAN_UMI_VALUE_UPDATE_OPTIONAL (line_edit_behavior, kan_ui_node_line_edit_behavior_t, id,
                                   &public->mouse_button_down_on_id)

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
                                                  const struct kan_ui_singleton_t *ui)
{
    KAN_UMI_SINGLETON_WRITE (private, ui_controls_input_private_singleton_t)
    if (KAN_TYPED_ID_32_IS_VALID (public->input_receiver_id))
    {
        deselect_input_receiver_behavior (state, public, private);
    }
}

static void on_multi_click_internal (struct ui_controls_input_state_t *state,
                                     struct kan_ui_input_singleton_t *public,
                                     const struct kan_ui_singleton_t *ui,
                                     kan_instance_size_t clicks_count)
{
    KAN_UMI_VALUE_UPDATE_OPTIONAL (line_edit_behavior, kan_ui_node_line_edit_behavior_t, id, &public->input_receiver_id)
    if (line_edit_behavior && clicks_count == 2u)
    {
        line_edit_behavior->cursor_content_location = KAN_INT_MAX (kan_instance_size_t);
        line_edit_behavior->selection_content_min = 0u;
        // Minus 1 due to null terminator inside content.
        line_edit_behavior->selection_content_max = line_edit_behavior->content_utf8.size - 1u;
        line_edit_behavior->text_visuals_dirty = true;
    }
}

static void on_press_end_internal (struct ui_controls_input_state_t *state,
                                   struct kan_ui_input_singleton_t *public,
                                   const struct kan_ui_singleton_t *ui,
                                   bool continuous)
{
    KAN_UMI_SINGLETON_WRITE (private, ui_controls_input_private_singleton_t)
    KAN_UMI_VALUE_UPDATE_OPTIONAL (line_edit_behavior, kan_ui_node_line_edit_behavior_t, id, &public->input_receiver_id)

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
            kan_application_window_add_text_listener (state->application_system_handle, public->linked_window_handle);
            private->input_receiver_requested_text_input = true;
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
    enum input_hit_box_mouse_update_mode_t mouse_hit_box_update = visuals_changed || hit_boxes_changed ?
                                                                      INPUT_HIT_BOX_MOUSE_UPDATE_MODE_EXECUTE :
                                                                      INPUT_HIT_BOX_MOUSE_UPDATE_MODE_NONE;

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
                mouse_hit_box_update = INPUT_HIT_BOX_MOUSE_UPDATE_MODE_RESET;
                // Also end press just in case.
                public->mouse_button_down_flags = 0u;

                if (public->press_filtered_in)
                {
                    KAN_UMO_EVENT_INSERT_INIT (kan_ui_press_end_t) {
                        .node_id = public->mouse_button_down_on_id,
                        .mouse_button_down_inclusive_flags = public->mouse_button_down_inclusive_flags,
                        .continuous_press = false,
                        .at_x = public->last_mouse_x,
                        .at_y = public->last_mouse_y,
                    };

                    on_press_end_internal (state, public, ui, false);
                }

                public->mouse_button_down_inclusive_flags = 0u;
                public->mouse_button_down_on_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
                public->press_filtered_in = false;
            }

            break;

        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_KEY_DOWN:
            if (KAN_TYPED_ID_32_IS_EQUAL (event->mouse_motion.window_id, public->linked_window_id))
            {
                process_key_down_internal (state, public, ui, event);
            }

            break;

        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_TEXT_INPUT:
            if (KAN_TYPED_ID_32_IS_EQUAL (event->mouse_motion.window_id, public->linked_window_id))
            {
                process_text_input_internal (state, public, ui, event);
            }

            break;

        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_MOUSE_MOTION:
            if (KAN_TYPED_ID_32_IS_EQUAL (event->mouse_motion.window_id, public->linked_window_id))
            {
                mouse_hit_box_update = INPUT_HIT_BOX_MOUSE_UPDATE_MODE_EXECUTE;
                public->last_mouse_x = (kan_instance_offset_t) event->mouse_motion.window_x;
                public->last_mouse_y = (kan_instance_offset_t) event->mouse_motion.window_y;

                if (public->press_filtered_in)
                {
                    on_press_motion_internal (state, public, ui);
                }
            }

            break;

        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_MOUSE_BUTTON_DOWN:
            if (KAN_TYPED_ID_32_IS_EQUAL (event->mouse_button.window_id, public->linked_window_id))
            {
                public->last_mouse_x = (kan_instance_offset_t) event->mouse_button.window_x;
                public->last_mouse_y = (kan_instance_offset_t) event->mouse_button.window_y;

                const uint32_t flag = 1u << event->mouse_button.button;
                const bool new_press = public->mouse_button_down_flags == 0u;
                const bool multi_click = event->mouse_button.clicks > 1u;

                public->mouse_button_down_flags |= flag;
                public->mouse_button_down_inclusive_flags |= flag;

                struct kan_repository_indexed_sequence_read_access_t element_access;
                const struct kan_ui_node_hit_box_t *element = find_hit_box_at (
                    state, HIT_BOX_SEARCH_MODE_POINTER, public->viewport_offset_x + public->last_mouse_x,
                    public->viewport_offset_y + public->last_mouse_y, &element_access);

                public->press_filtered_in = !element || ((new_press || multi_click) && element->interactable &&
                                                         (element->mouse_button_down_flags & flag));

                public->mouse_button_down_on_id =
                    element && public->press_filtered_in ? element->id : KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);

                if (multi_click)
                {
                    if (public->press_filtered_in)
                    {
                        KAN_UMO_EVENT_INSERT_INIT (kan_ui_multi_click_t) {
                            .node_id = public->mouse_button_down_on_id,
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
                        public->press_filtered_in = false;
                        public->mouse_button_down_on_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
                    }
                }
                else if (new_press && public->press_filtered_in)
                {
                    KAN_UMO_EVENT_INSERT_INIT (kan_ui_press_begin_t) {
                        .node_id = public->mouse_button_down_on_id,
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
                        on_press_begin_internal (state, public, ui);
                    }
                    else
                    {
                        // Special callback for the cases when we need to reset press-based selection.
                        on_press_begin_filtered_out_internal (state, public, ui);
                    }
                }

                if (multi_click)
                {
                    on_multi_click_internal (state, public, ui, event->mouse_button.clicks);
                }
            }

            break;

        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_MOUSE_BUTTON_UP:
            if (KAN_TYPED_ID_32_IS_EQUAL (event->mouse_button.window_id, public->linked_window_id))
            {
                public->last_mouse_x = (kan_instance_offset_t) event->mouse_button.window_x;
                public->last_mouse_y = (kan_instance_offset_t) event->mouse_button.window_y;

                const uint32_t flag = 1u << event->mouse_button.button;
                const bool end_press = public->mouse_button_down_flags == flag;
                public->mouse_button_down_flags &= ~flag;

                if (end_press)
                {
                    if (public->press_filtered_in)
                    {
                        struct kan_repository_indexed_sequence_read_access_t element_access;
                        const struct kan_ui_node_hit_box_t *element = find_hit_box_at (
                            state, HIT_BOX_SEARCH_MODE_POINTER, public->viewport_offset_x + public->last_mouse_x,
                            public->viewport_offset_y + public->last_mouse_y, &element_access);

                        const kan_ui_node_id_t end_node_id =
                            element ? element->id : KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
                        const bool continuous = KAN_TYPED_ID_32_IS_EQUAL (end_node_id, public->mouse_button_down_on_id);

                        KAN_UMO_EVENT_INSERT_INIT (kan_ui_press_end_t) {
                            .node_id = public->mouse_button_down_on_id,
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
                        on_press_end_internal (state, public, ui, continuous);
                    }

                    public->mouse_button_down_inclusive_flags = 0u;
                    public->mouse_button_down_on_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
                    public->press_filtered_in = false;
                }
            }

            break;

        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_MOUSE_WHEEL:
        {
            if (KAN_TYPED_ID_32_IS_EQUAL (event->mouse_wheel.window_id, public->linked_window_id))
            {
                public->last_mouse_x = (kan_instance_offset_t) event->mouse_wheel.window_x;
                public->last_mouse_y = (kan_instance_offset_t) event->mouse_wheel.window_y;

                struct kan_repository_indexed_sequence_read_access_t element_access;
                const struct kan_ui_node_hit_box_t *element = find_hit_box_at (
                    state, HIT_BOX_SEARCH_MODE_SCROLL, public->viewport_offset_x + public->last_mouse_x,
                    public->viewport_offset_y + public->last_mouse_y, &element_access);

                if (element)
                {
                    KAN_UMI_VALUE_READ_OPTIONAL (scroll_behaviour, kan_ui_node_scroll_behavior_t, id, &element->id)
                    if (scroll_behaviour)
                    {
                        const float speed_x =
                            kan_ui_calculate_coordinate_floating (ui, scroll_behaviour->mouse_speed_x);
                        const float speed_y =
                            kan_ui_calculate_coordinate_floating (ui, scroll_behaviour->mouse_speed_y);

                        apply_scroll_relative_input (state, public, ui, scroll_behaviour,
                                                     event->mouse_wheel.wheel_x * speed_x * ui->animation_delta_time_s,
                                                     event->mouse_wheel.wheel_y * speed_y * ui->animation_delta_time_s,
                                                     true);
                    }

                    kan_repository_indexed_sequence_read_access_close (&element_access);

                    // If mouse wheel input was passed to scroll behavior, deselect line edit so the user won't
                    // unexpectedly input anything into it.
                    if (scroll_behaviour)
                    {
                        if (KAN_TYPED_ID_32_IS_VALID (public->input_receiver_id))
                        {
                            KAN_UMI_SINGLETON_WRITE (private, ui_controls_input_private_singleton_t)
                            deselect_input_receiver_behavior (state, public, private);
                        }
                    }
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

    switch (mouse_hit_box_update)
    {
    case INPUT_HIT_BOX_MOUSE_UPDATE_MODE_NONE:
        break;

    case INPUT_HIT_BOX_MOUSE_UPDATE_MODE_RESET:
    {
        KAN_UMI_VALUE_READ_OPTIONAL (old_hovered, kan_ui_node_hit_box_t, id, &public->current_hovered_id)
        public->current_hovered_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
        public->last_mouse_x = 0;
        public->last_mouse_y = 0;

        if (old_hovered)
        {
            apply_hit_box_interaction_visuals (state, public, bundle, old_hovered, false);
        }

        break;
    }

    case INPUT_HIT_BOX_MOUSE_UPDATE_MODE_EXECUTE:
    {
        struct kan_repository_indexed_sequence_read_access_t new_hovered_access;
        const struct kan_ui_node_hit_box_t *new_hovered =
            find_hit_box_at (state, HIT_BOX_SEARCH_MODE_POINTER, public->viewport_offset_x + public->last_mouse_x,
                             public->viewport_offset_y + public->last_mouse_y, &new_hovered_access);
        const bool new_hovered_applicable = new_hovered && new_hovered->interactable;

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
            }
        }

        if (new_hovered)
        {
            kan_repository_indexed_sequence_read_access_close (&new_hovered_access);
        }

        break;
    }
    }

    if (public->press_filtered_in && KAN_TYPED_ID_32_IS_VALID (public->mouse_button_down_on_id))
    {
        KAN_UMI_VALUE_READ_OPTIONAL (hit_box, kan_ui_node_hit_box_t, id, &public->mouse_button_down_on_id)
        if (hit_box)
        {
            prolong_hit_box_down_visuals (state, public, bundle, hit_box);
        }
    }
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

        struct kan_text_item_t text_items[] = {
            {
                .type = KAN_TEXT_ITEM_STYLE,
                .style =
                    {
                        .style = behavior->content_style,
                        .mark = behavior->content_mark,
                    },
            },
            {
                .type = KAN_TEXT_ITEM_UTF8,
                .utf8 = (const char *) behavior->content_utf8.data,
            },
        };

        if (KAN_HANDLE_IS_VALID (shaping_unit->request.text))
        {
            kan_text_destroy (shaping_unit->request.text);
        }

        struct kan_text_description_t description = {
            .items_count = sizeof (text_items) / sizeof (text_items[0u]),
            .items = text_items,
            .guide_bidi_with_direction = true,
            .direction_to_guide_bidi =
                locale->resource.preferred_direction == KAN_LOCALE_PREFERRED_TEXT_DIRECTION_LEFT_TO_RIGHT ?
                    KAN_TEXT_READING_DIRECTION_LEFT_TO_RIGHT :
                    KAN_TEXT_READING_DIRECTION_RIGHT_TO_LEFT,
        };

        // Recreating text that way is not very effective, but text line edit should not be a bottleneck for us anyway.
        shaping_unit->request.text = kan_text_create (&description);
        shaping_unit->dirty = true;

        behavior->content_dirty = false;
        behavior->text_visuals_dirty = true;
    }
}

static void process_interactable_style_changes (struct ui_controls_input_state_t *state,
                                                const struct kan_ui_input_singleton_t *public,
                                                const struct kan_ui_bundle_singleton_t *bundle)
{
    KAN_UML_EVENT_FETCH (changed_event, kan_ui_node_hit_box_on_style_change_event_t)
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
        if (ui->animation_global_time_s > down_mark->down_until_s ||
            // Check for animation time overflow loop.
            ui->animation_global_time_s < ui->animation_delta_time_s)
        {
            KAN_UMI_VALUE_UPDATE_OPTIONAL (hit_box, kan_ui_node_hit_box_t, id, &down_mark->id)
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
            KAN_UMI_VALUE_UPDATE_OPTIONAL (line_drawable, kan_ui_node_drawable_t, id, &line_state->id)
            if (line_drawable)
            {
                line_drawable->hidden = true;
            }

            KAN_UMI_VALUE_UPDATE_OPTIONAL (knob_drawable, kan_ui_node_drawable_t, id, &line_state->knob_id)
            if (knob_drawable)
            {
                knob_drawable->hidden = true;
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
    bool hit_boxes_changed = false;

    KAN_UML_EVENT_FETCH (bundle_updated_event, kan_ui_bundle_updated_t)
    {
        visuals_changed = INPUT_HIT_BOX_MOUSE_UPDATE_MODE_EXECUTE;
    }

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

    process_scroll_behavior_insertion (state, ui);
    process_line_edit_behavior_lifetime (state);
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

    process_interactable_style_changes (state, public, bundle);
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
            node->element.height = KAN_UI_VALUE_PX ((float) shaping_unit->shaped_secondary_size);
            break;

        case KAN_TEXT_ORIENTATION_VERTICAL:
            node->element.width = KAN_UI_VALUE_PX ((float) shaping_unit->shaped_secondary_size);
            break;
        }
    }

    KAN_UMI_VALUE_UPDATE_REQUIRED (drawable, kan_ui_node_drawable_t, id, &node->id)
    drawable->hidden = false;
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
        // Will be shown again once
        drawable->hidden = true;
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

        for (kan_loop_size_t sequence_index = 0u; sequence_index < shaping_unit->shaped_edition_sequences.size;
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

            for (kan_loop_size_t cluster_index = 0u; cluster_index < sequence->clusters.size; ++cluster_index)
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
                primary_draw_offset += cursor_safe_space - cursor_x_with_offset;
            }
            else if (cursor_x_with_offset + cursor_safe_space >
                     (kan_instance_offset_t) shaping_unit->request.primary_axis_limit)
            {
                primary_draw_offset -= cursor_x_with_offset -
                                       (kan_instance_offset_t) shaping_unit->request.primary_axis_limit +
                                       cursor_safe_space;
            }
        }
    }

    if (behavior->selection_content_min != KAN_INT_MAX (kan_instance_size_t) &&
        behavior->selection_content_max != KAN_INT_MAX (kan_instance_size_t) &&
        behavior->selection_image_index != KAN_INT_MAX (uint32_t))
    {
        const kan_instance_offset_t selection_leeway = kan_ui_calculate_coordinate (ui, behavior->selection_leeway);
        for (kan_loop_size_t sequence_index = 0u; sequence_index < shaping_unit->shaped_edition_sequences.size;
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

            for (kan_loop_size_t cluster_index = 0u; cluster_index < sequence->clusters.size; ++cluster_index)
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
    instance->mouse_button_down_on_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
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
}

void kan_ui_node_line_edit_behavior_set_content (struct kan_ui_node_line_edit_behavior_t *instance,
                                                 const char *null_terminated_utf8_content,
                                                 kan_interned_string_t content_style,
                                                 uint32_t content_mark)
{
    const kan_instance_size_t input_length = strlen (null_terminated_utf8_content);
    instance->content_utf8.size = 0u;
    kan_dynamic_array_set_capacity (&instance->content_utf8, input_length + 1u);
    instance->content_utf8.size = input_length + 1u;
    memcpy (instance->content_utf8.data, null_terminated_utf8_content, input_length + 1u);

    instance->content_style = content_style;
    instance->content_mark = content_mark;
    instance->content_dirty = true;
}

void kan_ui_node_line_edit_behavior_shutdown (struct kan_ui_node_line_edit_behavior_t *instance)
{
    kan_dynamic_array_shutdown (&instance->content_utf8);
}
