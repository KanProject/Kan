#define _CRT_SECURE_NO_WARNINGS __CUSHION_PRESERVE__

#include <qsort.h>

#include <kan/log/logging.h>
#include <kan/universe/macro.h>
#include <kan/universe_render_foundation/program.h>
#include <kan/universe_render_foundation/render_graph.h>
#include <kan/universe_ui/core.h>

KAN_LOG_DEFINE_CATEGORY (ui_layout);
KAN_USE_STATIC_INTERNED_IDS
KAN_USE_STATIC_CPU_SECTIONS

KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (ui_layout)
UNIVERSE_UI_API KAN_UM_MUTATOR_GROUP_META (text_management, KAN_UI_CORE_MUTATOR_GROUP);

struct kan_ui_singleton_viewport_on_change_event_t
{
    kan_instance_size_t stub;
};

KAN_REFLECTION_STRUCT_META (kan_ui_singleton_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_on_change_event_t kan_ui_singleton_viewport_on_change_event = {
    .event_type = "kan_ui_singleton_viewport_on_change_event_t",
    .observed_fields_count = 5u,
    .observed_fields =
        (struct kan_repository_field_path_t[]) {
            {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"scale"}},
            {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"viewport_x"}},
            {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"viewport_y"}},
            {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"viewport_width"}},
            {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"viewport_height"}},
        },
    .unchanged_copy_outs_count = 0u,
    .unchanged_copy_outs = NULL,
    .changed_copy_outs_count = 0u,
    .changed_copy_outs = NULL,
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_cascade_deletion_t kan_ui_node_hierarchy_cascade_deletion = {
    .parent_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
    .child_type_name = "kan_ui_node_t",
    .child_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"parent_id"}},
};

struct kan_ui_node_element_on_insert_event_t
{
    kan_ui_node_id_t id;
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_on_insert_event_t kan_ui_node_on_insert_event = {
    .event_type = "kan_ui_node_element_on_insert_event_t",
    .copy_outs_count = 1u,
    .copy_outs =
        (struct kan_repository_copy_out_t[]) {
            {
                .source_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
                .target_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
            },
        },
};

struct kan_ui_node_element_on_change_event_t
{
    kan_ui_node_id_t id;
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_on_change_event_t kan_ui_node_element_on_change_event = {
    .event_type = "kan_ui_node_element_on_change_event_t",
    .observed_fields_count = 1u,
    .observed_fields =
        (struct kan_repository_field_path_t[]) {
            {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"element"}},
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

struct kan_ui_node_order_on_change_event_t
{
    kan_ui_node_id_t id;
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_on_change_event_t kan_ui_node_order_on_change_event = {
    .event_type = "kan_ui_node_order_on_change_event_t",
    .observed_fields_count = 1u,
    .observed_fields =
        (struct kan_repository_field_path_t[]) {
            {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"local_element_order"}},
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

struct kan_ui_node_layout_on_change_event_t
{
    kan_ui_node_id_t id;
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_on_change_event_t kan_ui_node_layout_on_change_event = {
    .event_type = "kan_ui_node_layout_on_change_event_t",
    .observed_fields_count = 1u,
    .observed_fields =
        (struct kan_repository_field_path_t[]) {
            {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"layout"}},
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

struct kan_ui_node_render_on_change_event_t
{
    kan_ui_node_id_t id;
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_on_change_event_t kan_ui_node_render_on_change_event = {
    .event_type = "kan_ui_node_render_on_change_event_t",
    .observed_fields_count = 1u,
    .observed_fields =
        (struct kan_repository_field_path_t[]) {
            {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"render"}},
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

struct kan_ui_node_element_on_delete_event_t
{
    kan_ui_node_id_t id;
    kan_ui_node_id_t parent_id;
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_on_delete_event_t kan_ui_node_on_delete_event = {
    .event_type = "kan_ui_node_element_on_delete_event_t",
    .copy_outs_count = 2u,
    .copy_outs =
        (struct kan_repository_copy_out_t[]) {
            {
                .source_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
                .target_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
            },
            {
                .source_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"parent_id"}},
                .target_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"parent_id"}},
            },
        },
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_cascade_deletion_t kan_ui_node_laid_out_cascade_deletion = {
    .parent_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
    .child_type_name = "kan_ui_node_laid_out_t",
    .child_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
};

/// \details Currently, we use very simplistic invalidation algorithm: we find topmost node in hierarchy that cannot
///          be affected by the change (for example, does not use "fit children" size flag), form array of that nodes
///          and then redo layout for whole their hierarchy. Might not be optimal and might be improved later.
KAN_REFLECTION_IGNORE
struct ui_layout_dirty_root_t
{
    struct ui_layout_dirty_root_t *next;
    kan_ui_node_id_t root_node_id;
};

KAN_REFLECTION_IGNORE
struct ui_layout_transient_state_t
{
    /// \details We save ui singleton pointer here for ease of us: we don't want to pass it around all the recursive
    ///          calculation functions.
    const struct kan_ui_singleton_t *ui;

    bool reorder_required;
    struct ui_layout_dirty_root_t *first_dirty_root;
    kan_instance_size_t reorder_index;
};

struct ui_layout_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (ui_layout)
    KAN_UM_BIND_STATE (ui_layout, state)

    struct kan_stack_group_allocator_t temporary_allocator;

    KAN_REFLECTION_IGNORE
    struct ui_layout_transient_state_t transient;
};

UNIVERSE_UI_API void ui_layout_state_init (struct ui_layout_state_t *instance)
{
    kan_stack_group_allocator_init (&instance->temporary_allocator,
                                    kan_allocation_group_get_child (kan_allocation_group_stack_get (), "temporary"),
                                    KAN_UNIVERSE_UI_LAYOUT_TEMPORARY_PAGE);
}

UNIVERSE_UI_API void ui_layout_state_shutdown (struct ui_layout_state_t *instance)
{
    kan_stack_group_allocator_shutdown (&instance->temporary_allocator);
}

UNIVERSE_UI_API KAN_UM_MUTATOR_DEPLOY (ui_layout)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_cpu_static_sections_ensure_initialized ();

    // Should usually be in different worlds, but add dependency just in case.
    kan_workflow_graph_node_depend_on (workflow_node, KAN_RENDER_FOUNDATION_FRAME_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_RENDER_FOUNDATION_PROGRAM_MANAGEMENT_END_CHECKPOINT);

    kan_workflow_graph_node_depend_on (workflow_node, KAN_UI_LAYOUT_BEGIN_CHECKPOINT);
    kan_workflow_graph_node_make_dependency_of (workflow_node, KAN_UI_LAYOUT_END_CHECKPOINT);
}

static kan_ui_node_id_t determine_dirty_root_recursive (struct ui_layout_state_t *state,
                                                        const struct kan_ui_node_t *node,
                                                        bool only_layout_data_changed)
{
    KAN_UMI_VALUE_READ_REQUIRED (laid_out, kan_ui_node_laid_out_t, id, &node->id)
    if (laid_out->dirty_for_laying_out)
    {
        // Return invalid as we're already in dirty hierarchy.
        return KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    }

    // Check if we just can't ascend anymore.
    if (!KAN_TYPED_ID_32_IS_VALID (node->parent_id))
    {
        // Can't ascend anymore.
        return node->id;
    }

    // Check if we don't need to ascend as only layout data is changed and this layout is stable and does not propagate
    // dirty flag to its parent.
    if (only_layout_data_changed)
    {
        const bool frame = node->layout.layout == KAN_UI_LAYOUT_FRAME;
        const bool stable_size = (node->element.width_flags & KAN_UI_SIZE_FLAG_FIT_CHILDREN) == 0u &&
                                 (node->element.height_flags & KAN_UI_SIZE_FLAG_FIT_CHILDREN) == 0u;

        if (frame || stable_size)
        {
            // Considered to be stable, no need to propagate update to parent.
            return node->id;
        }
    }

    KAN_UMI_VALUE_READ_REQUIRED (parent, kan_ui_node_t, id, &node->parent_id)
    // For parent, it is always "only layout data changed".
    return determine_dirty_root_recursive (state, parent, true);
}

static void mark_dirty_recursive (struct ui_layout_state_t *state, kan_ui_node_id_t id)
{
    KAN_UMI_VALUE_UPDATE_REQUIRED (laid_out, kan_ui_node_laid_out_t, id, &id)
    if (laid_out->dirty_for_laying_out)
    {
        // Should be another dirty root, if it is already marked dirty and we're in recursive chain (if we're in
        // recursive chain, than chain root was not dirty).
        struct ui_layout_dirty_root_t *previous = NULL;
        struct ui_layout_dirty_root_t *dirty_root = state->transient.first_dirty_root;

        while (dirty_root)
        {
            if (KAN_TYPED_ID_32_IS_EQUAL (dirty_root->root_node_id, id))
            {
                if (previous)
                {
                    previous->next = dirty_root->next;
                }
                else
                {
                    state->transient.first_dirty_root = dirty_root->next;
                }

                return;
            }

            previous = dirty_root;
            dirty_root = dirty_root->next;
        }

        // It should always be a known dirty root, if our logic is correct.
        KAN_ASSERT (false)
        return;
    }

    laid_out->dirty_for_laying_out = true;
    KAN_UML_VALUE_READ (child, kan_ui_node_t, parent_id, &id) { mark_dirty_recursive (state, child->id); }
}

static void mark_dirty_root (struct ui_layout_state_t *state, kan_ui_node_id_t dirty_root)
{
    KAN_UMI_VALUE_UPDATE_REQUIRED (laid_out, kan_ui_node_laid_out_t, id, &dirty_root)
    if (laid_out->dirty_for_laying_out)
    {
        // Already dirty, nothing to do.
        return;
    }

    laid_out->dirty_for_laying_out = true;
    KAN_UML_VALUE_READ (child, kan_ui_node_t, parent_id, &dirty_root) { mark_dirty_recursive (state, child->id); }
}

KAN_REFLECTION_IGNORE
struct layout_child_access_t
{
    const struct kan_ui_node_t *child;
    struct kan_ui_node_laid_out_t *laid_out;
    struct kan_repository_indexed_value_read_access_t child_access;
    struct kan_repository_indexed_value_update_access_t laid_out_access;
};

KAN_REFLECTION_IGNORE
struct layout_child_access_node_t
{
    struct layout_child_access_node_t *next;
    struct layout_child_access_t access;
};

KAN_REFLECTION_IGNORE
struct layout_temporary_data_t
{
    kan_instance_size_t sorted_children_count;
    struct layout_child_access_t *sorted_children;

    enum kan_ui_layout_t cached_layout;

    // Offsets is used to reduce amount of casts in calculations as coordinates are calculated into offsets either way.

    kan_instance_offset_t width_px;
    kan_instance_offset_t height_px;

    kan_instance_offset_t children_width_usage_px;
    kan_instance_offset_t children_height_usage_px;
    kan_instance_offset_t layout_previous_margin;

    // Cached margins are automatically collapsed during layout_size_pass, do not try to collapse them again after that.

    kan_instance_offset_t cached_margin_left_px;
    kan_instance_offset_t cached_margin_right_px;
    kan_instance_offset_t cached_margin_top_px;
    kan_instance_offset_t cached_margin_bottom_px;

    kan_instance_offset_t cached_padding_left_px;
    kan_instance_offset_t cached_padding_right_px;
    kan_instance_offset_t cached_padding_top_px;
    kan_instance_offset_t cached_padding_bottom_px;
};

static inline void read_and_sort_children_into (struct ui_layout_state_t *state,
                                                kan_ui_node_id_t parent_id,
                                                kan_instance_size_t *count_output,
                                                struct layout_child_access_t **output)
{
    *count_output = 0u;
    struct layout_child_access_node_t *first_child = NULL;

    KAN_UML_VALUE_READ (child, kan_ui_node_t, parent_id, &parent_id)
    {
        struct layout_child_access_node_t *access =
            KAN_STACK_GROUP_ALLOCATOR_ALLOCATE_TYPED (&state->temporary_allocator, struct layout_child_access_node_t);

        access->access.child = child;
        KAN_UM_ACCESS_ESCAPE (access->access.child_access, child);

        KAN_UMI_VALUE_UPDATE_REQUIRED (laid_out, kan_ui_node_laid_out_t, id, &child->id)
        access->access.laid_out = laid_out;
        KAN_UM_ACCESS_ESCAPE (access->access.laid_out_access, laid_out);

        ++*count_output;
        access->next = first_child;
        first_child = access;
    }

    *output = kan_stack_group_allocator_allocate (&state->temporary_allocator,
                                                  sizeof (struct layout_child_access_t) * *count_output,
                                                  alignof (struct layout_child_access_t));

    struct layout_child_access_t *copy_output = *output;
    while (first_child)
    {
        *copy_output = first_child->access;
        ++output;
        first_child = first_child->next;
    }

    struct layout_child_access_t temporary;
#define LESS(FIRST_INDEX, SECOND_INDEX)                                                                                \
    __CUSHION_PRESERVE__ (*output)                                                                                     \
    [FIRST_INDEX].child->local_element_order < (*output)[SECOND_INDEX].child->local_element_order
#define SWAP(FIRST_INDEX, SECOND_INDEX)                                                                                \
    __CUSHION_PRESERVE__                                                                                               \
    temporary = (*output)[FIRST_INDEX], (*output)[FIRST_INDEX] = (*output)[SECOND_INDEX],                              \
    (*output)[SECOND_INDEX] = temporary

    QSORT (*count_output, LESS, SWAP);
#undef LESS
#undef SWAP
#undef AT_INDEX
}

static struct layout_temporary_data_t *layout_temporary_data_create (struct ui_layout_state_t *state,
                                                                     const struct kan_ui_node_t *node)
{
    struct layout_temporary_data_t *data =
        KAN_STACK_GROUP_ALLOCATOR_ALLOCATE_TYPED (&state->temporary_allocator, struct layout_temporary_data_t);

    data->sorted_children_count = 0u;
    data->sorted_children = NULL;
    data->cached_layout = node->layout.layout;

    data->width_px = 0;
    data->height_px = 0;

    data->children_width_usage_px = 0;
    data->children_height_usage_px = 0;
    data->layout_previous_margin = 0;

    data->cached_margin_left_px = 0;
    data->cached_margin_right_px = 0;
    data->cached_margin_top_px = 0;
    data->cached_margin_bottom_px = 0;

    data->cached_padding_left_px = 0;
    data->cached_padding_right_px = 0;
    data->cached_padding_top_px = 0;
    data->cached_padding_bottom_px = 0;

    read_and_sort_children_into (state, node->id, &data->sorted_children_count, &data->sorted_children);
    return data;
}

static inline kan_instance_offset_t calculate_ui_coordinate (const struct kan_ui_singleton_t *ui,
                                                             struct kan_ui_coordinate_t coordinate)
{
    float floating_value = 0.0f;
    switch (coordinate.type)
    {
    case KAN_UI_PT:
        floating_value = coordinate.value * ui->scale;
        break;

    case KAN_UI_PX:
        floating_value = coordinate.value;
        break;

    case KAN_UI_WH:
        floating_value = coordinate.value * (float) ui->viewport_height;
        break;

    case KAN_UI_WW:
        floating_value = coordinate.value * (float) ui->viewport_width;
        break;
    }

    return (kan_instance_offset_t) roundf (floating_value);
}

#define UI_COLLAPSE_MARGIN(FIELD, BASELINE) (FIELD) = KAN_MAX ((FIELD) - (BASELINE), 0)

static void layout_size_pass (struct ui_layout_state_t *state,
                              const struct kan_ui_node_t *node,
                              struct kan_ui_node_laid_out_t *laid_out,
                              struct layout_temporary_data_t *parent_data)
{
    KAN_ASSERT (!laid_out->temporary_data)
    struct layout_temporary_data_t *data = layout_temporary_data_create (state, node);
    laid_out->temporary_data = data;

    data->width_px = calculate_ui_coordinate (state->transient.ui, node->element.width);
    data->height_px = calculate_ui_coordinate (state->transient.ui, node->element.height);

    data->cached_margin_left_px = calculate_ui_coordinate (state->transient.ui, node->element.margin.left);
    data->cached_margin_right_px = calculate_ui_coordinate (state->transient.ui, node->element.margin.right);
    data->cached_margin_top_px = calculate_ui_coordinate (state->transient.ui, node->element.margin.top);
    data->cached_margin_bottom_px = calculate_ui_coordinate (state->transient.ui, node->element.margin.bottom);

    data->cached_padding_left_px = calculate_ui_coordinate (state->transient.ui, node->layout.padding.left);
    data->cached_padding_right_px = calculate_ui_coordinate (state->transient.ui, node->layout.padding.right);
    data->cached_padding_top_px = calculate_ui_coordinate (state->transient.ui, node->layout.padding.top);
    data->cached_padding_bottom_px = calculate_ui_coordinate (state->transient.ui, node->layout.padding.bottom);

    // Padding is an initial margin, technically.
    data->layout_previous_margin = data->cached_layout == KAN_UI_LAYOUT_HORIZONTAL_CONTAINER ?
                                       data->cached_padding_left_px :
                                       data->cached_padding_top_px;

    for (kan_loop_size_t index = 0u; index < data->sorted_children_count; ++index)
    {
        const struct layout_child_access_t *access = &data->sorted_children[index];
        layout_size_pass (state, access->child, access->laid_out, data);
    }

    data->children_width_usage_px += data->cached_padding_left_px;
    data->children_width_usage_px += data->cached_padding_right_px;
    data->children_height_usage_px += data->cached_padding_top_px;
    data->children_height_usage_px += data->cached_padding_bottom_px;

    if (data->cached_layout != KAN_UI_LAYOUT_FRAME)
    {
        if (node->element.width_flags & KAN_UI_SIZE_FLAG_FIT_CHILDREN)
        {
            data->width_px = KAN_MAX (data->width_px, data->children_width_usage_px);
        }

        if (node->element.height_flags & KAN_UI_SIZE_FLAG_FIT_CHILDREN)
        {
            data->height_px = KAN_MAX (data->height_px, data->children_height_usage_px);
        }
    }

    if (parent_data)
    {
        kan_instance_offset_t occupied_width = data->width_px;
        kan_instance_offset_t occupied_height = data->height_px;

        switch (parent_data->cached_layout)
        {
        case KAN_UI_LAYOUT_FRAME:
            // Frame does not need additional calculations as it does not position children one after another.
            break;

        case KAN_UI_LAYOUT_VERTICAL_CONTAINER:
            UI_COLLAPSE_MARGIN (data->cached_margin_left_px, parent_data->cached_padding_left_px);
            UI_COLLAPSE_MARGIN (data->cached_margin_right_px, parent_data->cached_padding_right_px);
            occupied_width += data->cached_margin_left_px;
            occupied_width += data->cached_margin_right_px;

            UI_COLLAPSE_MARGIN (data->cached_margin_top_px, parent_data->layout_previous_margin);
            occupied_height += data->cached_margin_top_px;
            occupied_height += data->cached_margin_bottom_px;
            parent_data->layout_previous_margin = data->cached_margin_bottom_px;

            parent_data->children_width_usage_px = KAN_MAX (parent_data->children_width_usage_px, occupied_width);
            parent_data->children_height_usage_px += occupied_height;
            break;

        case KAN_UI_LAYOUT_HORIZONTAL_CONTAINER:
            UI_COLLAPSE_MARGIN (data->cached_margin_left_px, parent_data->layout_previous_margin);
            occupied_width += data->cached_margin_left_px;
            occupied_width += data->cached_margin_right_px;
            parent_data->layout_previous_margin = data->cached_margin_right_px;

            UI_COLLAPSE_MARGIN (data->cached_margin_top_px, parent_data->cached_padding_top_px);
            UI_COLLAPSE_MARGIN (data->cached_margin_bottom_px, parent_data->cached_padding_bottom_px);
            occupied_height += data->cached_margin_top_px;
            occupied_height += data->cached_margin_bottom_px;

            parent_data->children_width_usage_px += occupied_width;
            parent_data->children_height_usage_px = KAN_MAX (parent_data->children_height_usage_px, occupied_height);
            break;
        }
    }
}

KAN_REFLECTION_IGNORE
struct layout_grow_node_t
{
    struct layout_grow_node_t *next;
    const struct layout_child_access_t *access;
};

static void layout_grow_pass (struct ui_layout_state_t *state,
                              const struct kan_ui_node_t *node,
                              struct kan_ui_node_laid_out_t *laid_out,
                              bool root)
{
    struct layout_temporary_data_t *data = laid_out->temporary_data;
    if (root)
    {
        // When root, need to grow itself from cached growth.
        if (node->element.width_flags & KAN_UI_SIZE_FLAG_GROW)
        {
            data->width_px += laid_out->cached_grow_width;
        }

        if (node->element.height_flags & KAN_UI_SIZE_FLAG_GROW)
        {
            data->height_px += laid_out->cached_grow_height;
        }
    }

    // Number of growing children should be relatively small, therefore no need for linearizing and quick sort here.
    struct layout_grow_node_t *first_grow_width_node = NULL;
    struct layout_grow_node_t *first_grow_height_node = NULL;

    for (kan_loop_size_t index = 0u; index < data->sorted_children_count; ++index)
    {
        const struct layout_child_access_t *access = &data->sorted_children[index];
        struct layout_temporary_data_t *child_data = access->laid_out->temporary_data;
        access->laid_out->cached_grow_width = 0;
        access->laid_out->cached_grow_height = 0;

        if (access->child->element.width_flags & KAN_UI_SIZE_FLAG_GROW)
        {
            struct layout_grow_node_t *grow_node =
                KAN_STACK_GROUP_ALLOCATOR_ALLOCATE_TYPED (&state->temporary_allocator, struct layout_grow_node_t);
            grow_node->access = access;

#define PUSH_GROW_NODE(AXIS_NAME)                                                                                      \
    struct layout_grow_node_t *push_after = NULL;                                                                      \
    struct layout_grow_node_t *push_before = first_grow_##AXIS_NAME##_node;                                            \
                                                                                                                       \
    while (push_before)                                                                                                \
    {                                                                                                                  \
        struct layout_temporary_data_t *other_data = push_before->access->laid_out->temporary_data;                    \
        if (child_data->AXIS_NAME##_px < other_data->AXIS_NAME##_px)                                                   \
        {                                                                                                              \
            if (push_after)                                                                                            \
            {                                                                                                          \
                push_after->next = grow_node;                                                                          \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                first_grow_##AXIS_NAME##_node = grow_node;                                                             \
            }                                                                                                          \
                                                                                                                       \
            grow_node->next = push_before;                                                                             \
            break;                                                                                                     \
        }                                                                                                              \
                                                                                                                       \
        push_after = push_before;                                                                                      \
        push_before = push_before->next;                                                                               \
    }                                                                                                                  \
                                                                                                                       \
    if (!push_before)                                                                                                  \
    {                                                                                                                  \
        if (push_after)                                                                                                \
        {                                                                                                              \
            push_after->next = grow_node;                                                                              \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            first_grow_##AXIS_NAME##_node = grow_node;                                                                 \
        }                                                                                                              \
                                                                                                                       \
        grow_node->next = NULL;                                                                                        \
    }

            PUSH_GROW_NODE (width)
        }

        if (access->child->element.height_flags & KAN_UI_SIZE_FLAG_GROW)
        {
            struct layout_grow_node_t *grow_node =
                KAN_STACK_GROUP_ALLOCATOR_ALLOCATE_TYPED (&state->temporary_allocator, struct layout_grow_node_t);
            grow_node->access = access;

            PUSH_GROW_NODE (height)
#undef PUSH_GROW_NODE
        }
    }

#define DO_GROW_ON_AXIS(AXIS_NAME)                                                                                     \
    if (first_grow_##AXIS_NAME##_node)                                                                                 \
    {                                                                                                                  \
        kan_instance_offset_t left_to_give = data->AXIS_NAME##_px - data->children_##AXIS_NAME##_usage_px;             \
        while (left_to_give > 0)                                                                                       \
        {                                                                                                              \
            kan_instance_offset_t candidate_count = 1;                                                                 \
            struct layout_temporary_data_t *first_data =                                                               \
                first_grow_##AXIS_NAME##_node->access->laid_out->temporary_data;                                       \
            struct layout_grow_node_t *barrier = first_grow_##AXIS_NAME##_node->next;                                  \
                                                                                                                       \
            while (barrier)                                                                                            \
            {                                                                                                          \
                struct layout_temporary_data_t *second_data = barrier->access->laid_out->temporary_data;               \
                if (second_data->AXIS_NAME##_px > first_data->AXIS_NAME##_px)                                          \
                {                                                                                                      \
                    break;                                                                                             \
                }                                                                                                      \
                                                                                                                       \
                ++candidate_count;                                                                                     \
                barrier = barrier->next;                                                                               \
            }                                                                                                          \
                                                                                                                       \
            if (left_to_give < candidate_count)                                                                        \
            {                                                                                                          \
                /* Cannot grow more without making children "a little bit not equal". */                               \
                break;                                                                                                 \
            }                                                                                                          \
                                                                                                                       \
            kan_instance_offset_t max_to_give;                                                                         \
            if (barrier)                                                                                               \
            {                                                                                                          \
                struct layout_temporary_data_t *barrier_data = barrier->access->laid_out->temporary_data;              \
                max_to_give = barrier_data->AXIS_NAME##_px - first_data->AXIS_NAME##_px;                               \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                max_to_give = left_to_give;                                                                            \
            }                                                                                                          \
                                                                                                                       \
            kan_instance_offset_t give_every = KAN_MIN (left_to_give / candidate_count, max_to_give);                  \
            struct layout_grow_node_t *receiver = first_grow_##AXIS_NAME##_node;                                       \
                                                                                                                       \
            while (receiver != barrier)                                                                                \
            {                                                                                                          \
                struct layout_temporary_data_t *receiver_data = receiver->access->laid_out->temporary_data;            \
                receiver_data->AXIS_NAME##_px += give_every;                                                           \
                receiver->access->laid_out->cached_grow_##AXIS_NAME += give_every;                                     \
                receiver = receiver->next;                                                                             \
            }                                                                                                          \
                                                                                                                       \
            left_to_give -= give_every * candidate_count;                                                              \
        }                                                                                                              \
    }

    DO_GROW_ON_AXIS (width)
    DO_GROW_ON_AXIS (height)

    for (kan_loop_size_t index = 0u; index < data->sorted_children_count; ++index)
    {
        const struct layout_child_access_t *access = &data->sorted_children[index];
        layout_grow_pass (state, access->child, access->laid_out, false);
    }
}

static void layout_position_pass (struct ui_layout_state_t *state,
                                  const struct kan_ui_node_t *node,
                                  struct kan_ui_node_laid_out_t *laid_out)
{
    struct layout_temporary_data_t *data = laid_out->temporary_data;
    laid_out->width = data->width_px;
    laid_out->height = data->height_px;

    if (!KAN_TYPED_ID_32_IS_VALID (node->parent_id))
    {
        // Child of root frame, align accordingly.
        laid_out->local_x = state->transient.ui->viewport_x +
                            calculate_ui_coordinate (state->transient.ui, node->element.frame_offset_x);

        laid_out->local_y = state->transient.ui->viewport_y +
                            calculate_ui_coordinate (state->transient.ui, node->element.frame_offset_y);

        switch (node->element.horizontal_alignment)
        {
        case KAN_UI_HORIZONTAL_ALIGNMENT_LEFT:
            break;

        case KAN_UI_HORIZONTAL_ALIGNMENT_CENTER:
            laid_out->local_x += state->transient.ui->viewport_width / 2 - data->width_px / 2;
            break;

        case KAN_UI_HORIZONTAL_ALIGNMENT_RIGHT:
            laid_out->local_x += state->transient.ui->viewport_height - data->width_px;
            break;
        }

        switch (node->element.vertical_alignment)
        {
        case KAN_UI_VERTICAL_ALIGNMENT_TOP:
            break;

        case KAN_UI_VERTICAL_ALIGNMENT_CENTER:
            laid_out->local_y += state->transient.ui->viewport_height / 2 - data->height_px / 2;
            break;

        case KAN_UI_VERTICAL_ALIGNMENT_BOTTOM:
            laid_out->local_y += state->transient.ui->viewport_height - data->height_px;
            break;
        }

        laid_out->global_x = laid_out->local_x;
        laid_out->global_y = laid_out->local_y;
    }

    const kan_instance_size_t pad_left = data->cached_padding_left_px;
    const kan_instance_size_t pad_right = data->cached_padding_right_px;
    const kan_instance_size_t pad_top = data->cached_padding_top_px;
    const kan_instance_size_t pad_bottom = data->cached_padding_bottom_px;
    const kan_instance_offset_t available_width = data->width_px - pad_left - pad_right;
    const kan_instance_offset_t available_height = data->height_px - pad_top - pad_bottom;

    switch (data->cached_layout)
    {
    case KAN_UI_LAYOUT_FRAME:
        for (kan_loop_size_t index = 0u; index < data->sorted_children_count; ++index)
        {
            const struct layout_child_access_t *access = &data->sorted_children[index];
            struct layout_temporary_data_t *child_data = access->laid_out->temporary_data;

            access->laid_out->local_x =
                pad_left + calculate_ui_coordinate (state->transient.ui, access->child->element.frame_offset_x);

            switch (access->child->element.horizontal_alignment)
            {
            case KAN_UI_HORIZONTAL_ALIGNMENT_LEFT:
                break;

            case KAN_UI_HORIZONTAL_ALIGNMENT_CENTER:
                access->laid_out->local_x += available_width / 2 - child_data->width_px / 2;
                break;

            case KAN_UI_HORIZONTAL_ALIGNMENT_RIGHT:
                access->laid_out->local_x += available_width - child_data->width_px;
                break;
            }

            access->laid_out->local_y =
                pad_top + calculate_ui_coordinate (state->transient.ui, access->child->element.frame_offset_y);

            switch (access->child->element.vertical_alignment)
            {
            case KAN_UI_VERTICAL_ALIGNMENT_TOP:
                break;

            case KAN_UI_VERTICAL_ALIGNMENT_CENTER:
                access->laid_out->local_y += available_height / 2 - child_data->height_px / 2;
                break;

            case KAN_UI_VERTICAL_ALIGNMENT_BOTTOM:
                access->laid_out->local_y += available_height - child_data->height_px;
                break;
            }
        }

        break;

    case KAN_UI_LAYOUT_VERTICAL_CONTAINER:
    {
        kan_instance_offset_t cursor = pad_top;
        for (kan_loop_size_t index = 0u; index < data->sorted_children_count; ++index)
        {
            const struct layout_child_access_t *access = &data->sorted_children[index];
            struct layout_temporary_data_t *child_data = access->laid_out->temporary_data;
            cursor += child_data->cached_margin_top_px;

            access->laid_out->local_x = pad_left;
            access->laid_out->local_y = cursor;
            cursor += child_data->cached_margin_bottom_px;

            switch (access->child->element.horizontal_alignment)
            {
            case KAN_UI_HORIZONTAL_ALIGNMENT_LEFT:
                break;

            case KAN_UI_HORIZONTAL_ALIGNMENT_CENTER:
                access->laid_out->local_x += available_width / 2 - child_data->width_px / 2;
                break;

            case KAN_UI_HORIZONTAL_ALIGNMENT_RIGHT:
                access->laid_out->local_x += available_width - child_data->width_px;
                break;
            }
        }

        break;
    }

    case KAN_UI_LAYOUT_HORIZONTAL_CONTAINER:
    {
        kan_instance_offset_t cursor = pad_top;
        for (kan_loop_size_t index = 0u; index < data->sorted_children_count; ++index)
        {
            const struct layout_child_access_t *access = &data->sorted_children[index];
            struct layout_temporary_data_t *child_data = access->laid_out->temporary_data;
            cursor += child_data->cached_margin_left_px;

            access->laid_out->local_x = cursor;
            access->laid_out->local_y = pad_top;
            cursor += child_data->cached_margin_right_px;

            switch (access->child->element.vertical_alignment)
            {
            case KAN_UI_VERTICAL_ALIGNMENT_TOP:
                break;

            case KAN_UI_VERTICAL_ALIGNMENT_CENTER:
                access->laid_out->local_y += available_height / 2 - child_data->height_px / 2;
                break;

            case KAN_UI_VERTICAL_ALIGNMENT_BOTTOM:
                access->laid_out->local_y += available_height - child_data->height_px;
                break;
            }
        }

        break;
    }
    }

    for (kan_loop_size_t index = 0u; index < data->sorted_children_count; ++index)
    {
        const struct layout_child_access_t *access = &data->sorted_children[index];
        layout_position_pass (state, node, access->laid_out);
    }
}

static void recalculate_layout_primary (struct ui_layout_state_t *state, kan_ui_node_id_t root_id)
{
    KAN_UMI_VALUE_READ_REQUIRED (node, kan_ui_node_t, id, &root_id)
    KAN_UMI_VALUE_UPDATE_REQUIRED (laid_out, kan_ui_node_laid_out_t, id, &root_id)

    // Dirty roots are selected in a way that their size should never change and therefore should never affect parent.
    // Which means that we can pass NULL instead of parent data.
    layout_size_pass (state, node, laid_out, NULL);

    layout_grow_pass (state, node, laid_out, true);
    layout_position_pass (state, node, laid_out);
}

static void layout_render_finalize_pass (struct ui_layout_state_t *state,
                                         const struct kan_ui_node_t *node,
                                         struct kan_ui_node_laid_out_t *laid_out)
{
    struct layout_temporary_data_t *data = laid_out->temporary_data;
    struct kan_ui_clip_rect_t children_clip_rect = laid_out->clip_rect;

    if (node->render.clip_children)
    {
        struct kan_ui_clip_rect_t my_clip_rect = {
            .x = laid_out->global_x,
            .y = laid_out->global_y,
            .width = laid_out->width,
            .height = laid_out->height,
        };

        children_clip_rect.x = KAN_MAX (laid_out->clip_rect.x, my_clip_rect.x);
        children_clip_rect.y = KAN_MAX (laid_out->clip_rect.y, my_clip_rect.y);

        children_clip_rect.width =
            KAN_MIN (laid_out->clip_rect.x + laid_out->clip_rect.width, my_clip_rect.x + my_clip_rect.width) -
            children_clip_rect.x;

        children_clip_rect.height =
            KAN_MIN (laid_out->clip_rect.y + laid_out->clip_rect.height, my_clip_rect.y + my_clip_rect.height) -
            children_clip_rect.y;
    }

    for (kan_loop_size_t index = 0u; index < data->sorted_children_count; ++index)
    {
        struct layout_child_access_t *access = &data->sorted_children[index];
        access->laid_out->clip_rect = children_clip_rect;
        access->laid_out->global_x = laid_out->global_x - node->render.scroll_x_px + access->laid_out->local_x;
        access->laid_out->global_y = laid_out->global_y - node->render.scroll_y_px + access->laid_out->local_y;

        access->laid_out->fully_clipped_out =
            access->laid_out->global_x + access->laid_out->width < children_clip_rect.x ||
            access->laid_out->global_x >= children_clip_rect.x + children_clip_rect.width ||
            access->laid_out->global_y + access->laid_out->height < children_clip_rect.y ||
            access->laid_out->global_y >= children_clip_rect.y + children_clip_rect.height;

        layout_render_finalize_pass (state, access->child, access->laid_out);

        // We should no longer need the accesses and we need to close them.
        kan_repository_indexed_value_read_access_close (&access->child_access);
        kan_repository_indexed_value_update_access_close (&access->laid_out_access);
    }

    // We should no longer use temporary data.
    laid_out->temporary_data = NULL;
    laid_out->dirty_for_laying_out = false;
}

static void recalculate_layout_finalize (struct ui_layout_state_t *state, kan_ui_node_id_t root_id)
{
    KAN_UMI_VALUE_READ_REQUIRED (node, kan_ui_node_t, id, &root_id)
    KAN_UMI_VALUE_UPDATE_REQUIRED (laid_out, kan_ui_node_laid_out_t, id, &root_id)

    if (!KAN_TYPED_ID_32_IS_VALID (node->parent_id))
    {
        laid_out->clip_rect = (struct kan_ui_clip_rect_t) {
            .x = state->transient.ui->viewport_x,
            .y = state->transient.ui->viewport_y,
            .width = state->transient.ui->viewport_width,
            .height = state->transient.ui->viewport_height,
        };
    }

    layout_render_finalize_pass (state, node, laid_out);
}

static void execute_draw_index_reorder (struct ui_layout_state_t *state, const struct kan_ui_node_t *parent)
{
    const kan_ui_node_id_t parent_id = parent ? parent->id : KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    kan_instance_size_t sorted_children_count;
    struct layout_child_access_t *sorted_children;
    read_and_sort_children_into (state, parent_id, &sorted_children_count, &sorted_children);

    for (kan_loop_size_t index = 0u; index < sorted_children_count; ++index)
    {
        struct layout_child_access_t *access = &sorted_children[index];
        access->laid_out->draw_index = state->transient.reorder_index;
        ++state->transient.reorder_index;
        execute_draw_index_reorder (state, access->child);

        // We should no longer need the accesses and we need to close them.
        kan_repository_indexed_value_read_access_close (&access->child_access);
        kan_repository_indexed_value_update_access_close (&access->laid_out_access);
    }
}

KAN_UM_MUTATOR_EXECUTE (ui_layout)
{
    KAN_UMI_SINGLETON_READ (ui, kan_ui_singleton_t)
    state->transient.ui = ui;
    state->transient.reorder_required = false;
    state->transient.first_dirty_root = NULL;
    state->transient.reorder_index = 0u;

    {
        KAN_CPU_SCOPED_STATIC_SECTION (ui_layout_node_lifetime_events)
        KAN_UML_EVENT_FETCH (node_on_insert, kan_ui_node_element_on_insert_event_t)
        {
            state->transient.reorder_required = true;
            KAN_UMI_VALUE_READ_OPTIONAL (node, kan_ui_node_t, id, &node_on_insert->id)

            if (!node)
            {
                continue;
            }

            KAN_UMO_INDEXED_INSERT (laid_out, kan_ui_node_laid_out_t)
            {
                laid_out->id = node->id;
                // Must be seen and set to false during mark dirty below.
                laid_out->dirty_for_laying_out = false;
            }

            KAN_UMI_VALUE_READ_OPTIONAL (parent, kan_ui_node_t, id, &node->parent_id)
            if (parent)
            {
                const kan_ui_node_id_t dirty_root = determine_dirty_root_recursive (state, parent, true);
                mark_dirty_root (state, dirty_root);
            }
            else
            {
                mark_dirty_root (state, node->id);
            }
        }

        KAN_UML_EVENT_FETCH (node_on_delete, kan_ui_node_element_on_delete_event_t)
        {
            state->transient.reorder_required = true;
            KAN_UMI_VALUE_READ_OPTIONAL (parent, kan_ui_node_t, id, &node_on_delete->parent_id)

            if (parent)
            {
                const kan_ui_node_id_t dirty_root = determine_dirty_root_recursive (state, parent, true);
                mark_dirty_root (state, dirty_root);
            }
        }
    }

    {
        KAN_CPU_SCOPED_STATIC_SECTION (ui_layout_viewport_change_events)
        bool changed = false;

        KAN_UML_EVENT_FETCH (viewport_on_change, kan_ui_singleton_viewport_on_change_event_t) { changed = true; }

        if (changed)
        {
            const kan_ui_node_id_t child_of_root_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
            KAN_UML_VALUE_READ (node, kan_ui_node_t, parent_id, &child_of_root_id)
            {
                mark_dirty_root (state, node->id);
            }
        }
    }

    {
        KAN_CPU_SCOPED_STATIC_SECTION (ui_layout_node_change_main_events)
        KAN_UML_EVENT_FETCH (node_on_element, kan_ui_node_element_on_change_event_t)
        {
            KAN_UMI_VALUE_READ_OPTIONAL (node, kan_ui_node_t, id, &node_on_element->id)
            if (node)
            {
                const kan_ui_node_id_t dirty_root = determine_dirty_root_recursive (state, node, false);
                mark_dirty_root (state, dirty_root);
            }
        }

        KAN_UML_EVENT_FETCH (node_on_order, kan_ui_node_order_on_change_event_t)
        {
            state->transient.reorder_required = true;
            KAN_UMI_VALUE_READ_OPTIONAL (node, kan_ui_node_t, id, &node_on_order->id)
            if (node)
            {
                const kan_ui_node_id_t dirty_root = determine_dirty_root_recursive (state, node, false);
                mark_dirty_root (state, dirty_root);
            }
        }

        KAN_UML_EVENT_FETCH (node_on_layout, kan_ui_node_layout_on_change_event_t)
        {
            KAN_UMI_VALUE_READ_OPTIONAL (node, kan_ui_node_t, id, &node_on_layout->id)
            if (node)
            {
                const kan_ui_node_id_t dirty_root = determine_dirty_root_recursive (state, node, true);
                mark_dirty_root (state, dirty_root);
            }
        }
    }

    {
        KAN_CPU_SCOPED_STATIC_SECTION (ui_layout_recalculate_main)
        struct ui_layout_dirty_root_t *root = state->transient.first_dirty_root;

        while (root)
        {
            recalculate_layout_primary (state, root->root_node_id);
            root = root->next;
        }
    }

    {
        KAN_CPU_SCOPED_STATIC_SECTION (ui_layout_node_change_render_events)
        KAN_UML_EVENT_FETCH (node_on_render, kan_ui_node_render_on_change_event_t)
        {
            KAN_UMI_VALUE_READ_OPTIONAL (node, kan_ui_node_t, id, &node_on_render->id)
            if (node)
            {
                // Render-based changes never propagate.
                mark_dirty_root (state, node_on_render->id);
            }
        }
    }

    {
        KAN_CPU_SCOPED_STATIC_SECTION (ui_layout_recalculate_render_and_global)
        struct ui_layout_dirty_root_t *root = state->transient.first_dirty_root;

        while (root)
        {
            recalculate_layout_finalize (state, root->root_node_id);
            root = root->next;
        }
    }

    if (state->transient.reorder_required)
    {
        KAN_CPU_SCOPED_STATIC_SECTION (ui_layout_reorder_draw)
        execute_draw_index_reorder (state, NULL);
    }

    kan_stack_group_allocator_shrink (&state->temporary_allocator);
    kan_stack_group_allocator_reset (&state->temporary_allocator);
}

void kan_ui_singleton_init (struct kan_ui_singleton_t *instance)
{
    instance->node_id_counter = kan_atomic_int_init (1);
    instance->scale = 1.0f;
    instance->viewport_x = 0;
    instance->viewport_y = 0;
    instance->viewport_width = 0;
    instance->viewport_height = 0;
}

void kan_ui_node_init (struct kan_ui_node_t *instance)
{
    instance->id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    instance->parent_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);

    instance->element.width_flags = KAN_UI_SIZE_FLAG_NONE;
    instance->element.height_flags = KAN_UI_SIZE_FLAG_NONE;

    instance->element.width = KAN_UI_VALUE_PT (0.0f);
    instance->element.height = KAN_UI_VALUE_PT (0.0f);
    instance->element.margin = KAN_UI_RECT_PT (0.0f, 0.0f, 0.0f, 0.0f);

    instance->element.horizontal_alignment = KAN_UI_HORIZONTAL_ALIGNMENT_LEFT;
    instance->element.vertical_alignment = KAN_UI_VERTICAL_ALIGNMENT_TOP;

    instance->element.frame_offset_x = KAN_UI_VALUE_PT (0.0f);
    instance->element.frame_offset_y = KAN_UI_VALUE_PT (0.0f);

    instance->local_element_order = 0;

    instance->layout.layout = KAN_UI_LAYOUT_FRAME;
    instance->layout.padding = KAN_UI_RECT_PT (0.0f, 0.0f, 0.0f, 0.0f);

    instance->render.clip_children = false;
    instance->render.scroll_x_px = 0;
    instance->render.scroll_y_px = 0;
}

void kan_ui_node_laid_out_init (struct kan_ui_node_laid_out_t *instance)
{
    instance->id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    instance->draw_index = 0u;

    instance->clip_rect.x = 0.0f;
    instance->clip_rect.y = 0.0f;
    instance->clip_rect.width = 0.0f;
    instance->clip_rect.height = 0.0f;
    instance->fully_clipped_out = false;

    instance->local_x = 0;
    instance->local_y = 0;
    instance->width = 0;
    instance->height = 0;
    instance->global_x = 0;
    instance->global_y = 0;

    instance->cached_grow_width = 0;
    instance->cached_grow_height = 0;

    instance->temporary_data = NULL;
    instance->dirty_for_laying_out = false;
}
