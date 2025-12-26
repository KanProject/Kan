#include <kan/context/all_system_names.h>
#include <kan/log/logging.h>
#include <kan/precise_time/precise_time.h>
#include <kan/universe/macro.h>
#include <kan/universe_locale/locale.h>
#include <kan/universe_render_foundation/atlas.h>
#include <kan/universe_ui/controls.h>

KAN_USE_STATIC_INTERNED_IDS
KAN_USE_STATIC_CPU_SECTIONS

KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (ui_controls_input)
KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (ui_controls_pre_layout)
KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (ui_controls_post_layout)
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
        .parent_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"shaping_unit"}},
        .child_type_name = "kan_text_shaping_unit_t",
        .child_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
};

struct kan_ui_node_down_mark_t
{
    kan_ui_node_id_t id;
    float down_until_s;
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_cascade_deletion_t
    kan_ui_node_down_animation_mark_cascade_deletion = {
        .parent_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
        .child_type_name = "kan_ui_node_down_animation_mark_t",
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

static const struct kan_ui_node_hit_box_t *find_hit_box_at (
    struct ui_controls_input_state_t *state,
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
                             struct kan_ui_bundle_singleton_t *bundle,
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
                                                             struct kan_ui_bundle_singleton_t *bundle,
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

static void apply_hit_box_interaction_visuals (struct ui_controls_input_state_t *state,
                                               struct kan_ui_input_singleton_t *public,
                                               struct kan_ui_bundle_singleton_t *bundle,
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
}

static void prolong_hit_box_down_visuals (struct ui_controls_input_state_t *state,
                                          struct kan_ui_input_singleton_t *public,
                                          struct kan_ui_bundle_singleton_t *bundle,
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

static void update_mouse (struct ui_controls_input_state_t *state,
                          struct kan_ui_input_singleton_t *public,
                          struct kan_ui_bundle_singleton_t *bundle,
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
            if (KAN_TYPED_ID_32_IS_EQUAL (event->window.id, public->window_filter))
            {
                mouse_hit_box_update = INPUT_HIT_BOX_MOUSE_UPDATE_MODE_RESET;
            }

            break;

        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_MOUSE_MOTION:
            if (KAN_TYPED_ID_32_IS_EQUAL (event->mouse_motion.window_id, public->window_filter))
            {
                mouse_hit_box_update = INPUT_HIT_BOX_MOUSE_UPDATE_MODE_EXECUTE;
                public->last_mouse_x = (kan_instance_offset_t) event->mouse_motion.window_x;
                public->last_mouse_y = (kan_instance_offset_t) event->mouse_motion.window_y;
            }

            break;

        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_MOUSE_BUTTON_DOWN:
            if (KAN_TYPED_ID_32_IS_EQUAL (event->mouse_button.window_id, public->window_filter))
            {
                public->last_mouse_x = (kan_instance_offset_t) event->mouse_button.window_x;
                public->last_mouse_y = (kan_instance_offset_t) event->mouse_button.window_y;

                const uint32_t flag = 1u << event->mouse_button.button;
                const bool new_press = public->mouse_button_down_flags == 0u;
                const bool multi_click = event->mouse_button.clicks > 1u;

                public->mouse_button_down_flags |= flag;
                public->mouse_button_down_inclusive_flags |= flag;

                struct kan_repository_indexed_sequence_read_access_t element_access;
                const struct kan_ui_node_hit_box_t *element =
                    find_hit_box_at (state, public->viewport_offset_x + public->last_mouse_x,
                                     public->viewport_offset_y + public->last_mouse_y, &element_access);

                public->press_filtered_in = !element || ((new_press || multi_click) && element->interactable &&
                                                         (element->mouse_button_down_flags & flag));

                public->mouse_button_down_on_id =
                    element && public->press_filtered_in ? element->id : KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);

                if (multi_click)
                {
                    if (public->press_filtered_in)
                    {
                        KAN_UMO_EVENT_INSERT (multi_click_event, kan_ui_multi_click_t)
                        {
                            multi_click_event->node_id = public->mouse_button_down_on_id;
                            multi_click_event->multi_click_button = event->mouse_button.button;
                            multi_click_event->click_count = event->mouse_button.clicks;
                            multi_click_event->mouse_button_down_flags = public->mouse_button_down_flags;
                            multi_click_event->at_x = public->last_mouse_x;
                            multi_click_event->at_y = public->last_mouse_y;
                        }

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
                    KAN_UMO_EVENT_INSERT (press_begin_event, kan_ui_press_begin_t)
                    {
                        press_begin_event->node_id = public->mouse_button_down_on_id;
                        press_begin_event->mouse_button_down_flags = public->mouse_button_down_flags;
                        press_begin_event->at_x = public->last_mouse_x;
                        press_begin_event->at_y = public->last_mouse_y;
                    }
                }

                if (element)
                {
                    kan_repository_indexed_sequence_read_access_close (&element_access);
                }
            }

            break;

        case KAN_PLATFORM_APPLICATION_EVENT_TYPE_MOUSE_BUTTON_UP:
            if (KAN_TYPED_ID_32_IS_EQUAL (event->mouse_button.window_id, public->window_filter))
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
                        const struct kan_ui_node_hit_box_t *element =
                            find_hit_box_at (state, public->viewport_offset_x + public->last_mouse_x,
                                             public->viewport_offset_y + public->last_mouse_y, &element_access);

                        const kan_ui_node_id_t end_node_id =
                            element ? element->id : KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);

                        KAN_UMO_EVENT_INSERT (press_end_event, kan_ui_press_end_t)
                        {
                            press_end_event->node_id = public->mouse_button_down_on_id;
                            press_end_event->mouse_button_down_inclusive_flags =
                                public->mouse_button_down_inclusive_flags;

                            press_end_event->continuous_press =
                                KAN_TYPED_ID_32_IS_EQUAL (end_node_id, public->mouse_button_down_on_id);

                            press_end_event->at_x = public->last_mouse_x;
                            press_end_event->at_y = public->last_mouse_y;
                        }

                        if (element)
                        {
                            kan_repository_indexed_sequence_read_access_close (&element_access);
                        }
                    }

                    public->mouse_button_down_inclusive_flags = 0u;
                    public->mouse_button_down_on_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
                    public->press_filtered_in = false;
                }
            }

            break;

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
            find_hit_box_at (state, public->viewport_offset_x + public->last_mouse_x,
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

static void clear_old_down_marks (struct ui_controls_input_state_t *state,
                                  struct kan_ui_singleton_t *ui,
                                  struct kan_ui_input_singleton_t *public,
                                  struct kan_ui_bundle_singleton_t *bundle)
{
    KAN_UML_SEQUENCE_DELETE (down_mark, kan_ui_node_down_mark_t)
    {
        if (ui->animation_global_time_s > down_mark->down_until_s)
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

    KAN_UML_EVENT_FETCH (laid_out_event, kan_ui_node_laid_out_t)
    {
        KAN_UMI_VALUE_READ_OPTIONAL (hit_box, kan_ui_node_hit_box_t, id, &laid_out_event->node_id)
        if (hit_box)
        {
            hit_boxes_changed = true;
        }
    }

    KAN_UMI_SINGLETON_READ (ui, kan_ui_singleton_t)
    update_mouse (state, public, bundle, visuals_changed, hit_boxes_changed);
    clear_old_down_marks (state, ui, public, bundle);
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
    KAN_UMI_VALUE_UPDATE_OPTIONAL (shaping_unit, kan_text_shaping_unit_t, id, &text_behavior->shaping_unit)
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
        KAN_UMI_VALUE_READ_OPTIONAL (text_behavior, kan_ui_node_text_behavior_t, shaping_unit, &text_shaped->id)
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
    KAN_UMI_VALUE_UPDATE_OPTIONAL (shaping_unit, kan_text_shaping_unit_t, id, &text_behavior->shaping_unit)
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
    }
}

void kan_ui_input_singleton_init (struct kan_ui_input_singleton_t *instance)
{
    instance->event_iterator = KAN_HANDLE_SET_INVALID (kan_application_system_event_iterator_t);
    instance->window_filter = KAN_TYPED_ID_32_SET_INVALID (kan_platform_window_id_t);

    instance->viewport_offset_x = 0;
    instance->viewport_offset_y = 0;

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
    instance->shaping_unit = KAN_TYPED_ID_32_SET_INVALID (kan_text_shaping_unit_id_t);
    instance->font_size = KAN_UI_VALUE_PX (14.0f);
    instance->sync_text_limit_from_ui = false;
    instance->sync_ui_size_from_text_secondary = false;
}
