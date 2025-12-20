#include <kan/log/logging.h>
#include <kan/precise_time/precise_time.h>
#include <kan/universe/macro.h>
#include <kan/universe_ui/controls.h>

KAN_USE_STATIC_INTERNED_IDS
KAN_USE_STATIC_CPU_SECTIONS

KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (ui_controls_pre_layout)
KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (ui_controls_post_layout)
UNIVERSE_UI_API KAN_UM_MUTATOR_GROUP_META (ui_controls, KAN_UI_CONTROLS_MUTATOR_GROUP);

KAN_REFLECTION_STRUCT_META (kan_ui_node_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_cascade_deletion_t kan_ui_node_interactable_cascade_deletion = {
    .parent_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
    .child_type_name = "kan_ui_node_interactable_t",
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
        drawable->skip_render_once = true;
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

void kan_ui_node_interactable_init (struct kan_ui_node_interactable_t *instance)
{
    instance->id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);

    instance->enabled = false;
    instance->focusable = false;
    instance->selectable = KAN_UI_NODE_SELECTABLE_NONE;

    instance->regular_style = NULL;
    instance->style_when_selected = NULL;

    kan_dynamic_array_init (&instance->propagate_visuals, 0u, sizeof (kan_ui_node_id_t), alignof (kan_ui_node_id_t),
                            kan_allocation_group_stack_get ());
}

void kan_ui_node_interactable_shutdown (struct kan_ui_node_interactable_t *instance)
{
    kan_dynamic_array_shutdown (&instance->propagate_visuals);
}

void kan_ui_node_text_behavior_init (struct kan_ui_node_text_behavior_t *instance)
{
    instance->id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    instance->shaping_unit = KAN_TYPED_ID_32_SET_INVALID (kan_text_shaping_unit_id_t);
    instance->font_size = KAN_UI_VALUE_PX (14.0f);
    instance->sync_text_limit_from_ui = false;
    instance->sync_ui_size_from_text_secondary = false;
}
