#define _CRT_SECURE_NO_WARNINGS __CUSHION_PRESERVE__

#include <string.h>

#include <qsort.h>

#include <kan/log/logging.h>
#include <kan/precise_time/precise_time.h>
#include <kan/universe/macro.h>
#include <kan/universe_render_foundation/atlas.h>
#include <kan/universe_render_foundation/program.h>
#include <kan/universe_render_foundation/render_graph.h>
#include <kan/universe_resource_provider/provider.h>
#include <kan/universe_ui/core.h>

KAN_LOG_DEFINE_CATEGORY (ui_layout);
KAN_LOG_DEFINE_CATEGORY (ui_bundle_management);
KAN_LOG_DEFINE_CATEGORY (ui_render_graph);
KAN_LOG_DEFINE_CATEGORY (ui_render);

KAN_USE_STATIC_INTERNED_IDS
KAN_USE_STATIC_CPU_SECTIONS

KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (ui_time)
KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (ui_layout)
KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (ui_bundle_management)
KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (ui_render_graph)
KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (ui_render)
UNIVERSE_UI_API KAN_UM_MUTATOR_GROUP_META (ui_core, KAN_UI_CORE_MUTATOR_GROUP);

struct kan_ui_singleton_viewport_on_change_event_t
{
    kan_instance_size_t stub;
};

KAN_REFLECTION_STRUCT_META (kan_ui_singleton_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_on_change_event_t kan_ui_singleton_viewport_on_change_event = {
    .event_type = "kan_ui_singleton_viewport_on_change_event_t",
    .observed_fields_count = 3u,
    .observed_fields =
        (struct kan_repository_field_path_t[]) {
            {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"scale"}},
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

struct kan_ui_node_on_insert_event_t
{
    kan_ui_node_id_t id;
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_on_insert_event_t kan_ui_node_on_insert_event = {
    .event_type = "kan_ui_node_on_insert_event_t",
    .copy_outs_count = 1u,
    .copy_outs =
        (struct kan_repository_copy_out_t[]) {
            {
                .source_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
                .target_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
            },
        },
};

struct kan_ui_node_on_change_event_t
{
    kan_ui_node_id_t id;
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_on_change_event_t kan_ui_node_on_change_event = {
    .event_type = "kan_ui_node_on_change_event_t",
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

struct kan_ui_node_on_delete_event_t
{
    kan_ui_node_id_t id;
    kan_ui_node_id_t parent_id;
};

KAN_REFLECTION_STRUCT_META (kan_ui_node_t)
UNIVERSE_UI_API struct kan_repository_meta_automatic_on_delete_event_t kan_ui_node_on_delete_event = {
    .event_type = "kan_ui_node_on_delete_event_t",
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
UNIVERSE_UI_API struct kan_repository_meta_automatic_cascade_deletion_t kan_ui_node_drawable_cascade_deletion = {
    .parent_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
    .child_type_name = "kan_ui_node_drawable_t",
    .child_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"id"}},
};

struct ui_time_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (ui_time)
    KAN_UM_BIND_STATE (ui_time, state)
};

UNIVERSE_UI_API KAN_UM_MUTATOR_DEPLOY (ui_time)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_cpu_static_sections_ensure_initialized ();

    kan_workflow_graph_node_depend_on (workflow_node, KAN_RENDER_FOUNDATION_FRAME_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_UI_TIME_BEGIN_CHECKPOINT);
    kan_workflow_graph_node_make_dependency_of (workflow_node, KAN_UI_TIME_END_CHECKPOINT);
    kan_workflow_graph_node_make_dependency_of (workflow_node, KAN_UI_LAYOUT_BEGIN_CHECKPOINT);
}

UNIVERSE_UI_API KAN_UM_MUTATOR_EXECUTE (ui_time)
{
    KAN_UMI_SINGLETON_WRITE (public, kan_ui_singleton_t)
    if (public->last_time_ns != KAN_INT_MAX (kan_time_size_t))
    {
        const kan_time_size_t delta_ns = kan_precise_time_get_elapsed_nanoseconds () - public->last_time_ns;
        const float delta_s = 1e-9f * (float) delta_ns;

        public->animation_global_time_s =
            fmodf (public->animation_global_time_s + delta_s, public->animation_global_time_loop_s);
        public->animation_delta_time_s = delta_s;
    }

    public->last_time_ns = kan_precise_time_get_elapsed_nanoseconds ();
}

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

    KAN_REFLECTION_IGNORE
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
    KAN_UMI_VALUE_UPDATE_OPTIONAL (existent_drawable, kan_ui_node_drawable_t, id, &node->id)
    if (existent_drawable && existent_drawable->layout_dirt_level > KAN_UI_LAYOUT_DIRT_LEVEL_NONE)
    {
        // Return invalid as we're already in dirty hierarchy.
        return KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    }

    // Initialize missing drawables as we go.
    if (!existent_drawable)
    {
        KAN_UMO_INDEXED_INSERT (drawable, kan_ui_node_drawable_t) { drawable->id = node->id; }
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

static void mark_dirty_recursive (struct ui_layout_state_t *state,
                                  kan_ui_node_id_t id,
                                  enum kan_ui_layout_dirt_level_t dirt_level,
                                  bool with_root_removal)
{
    KAN_UMI_VALUE_UPDATE_OPTIONAL (existent_drawable, kan_ui_node_drawable_t, id, &id)
    if (existent_drawable)
    {
        if (with_root_removal && existent_drawable->layout_dirt_level != KAN_UI_LAYOUT_DIRT_LEVEL_NONE)
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

                    break;
                }

                previous = dirty_root;
                dirty_root = dirty_root->next;
            }

            // It should always be a known dirty root, if our logic is correct.
            KAN_ASSERT (dirty_root)

            // Root cannot contain more roots, therefore disable root removal.
            with_root_removal = false;
        }

        if (existent_drawable->layout_dirt_level >= dirt_level)
        {
            // No need to recurse as dirt level is already like this or higher.
            return;
        }

        existent_drawable->layout_dirt_level = dirt_level;
    }
    // Initialize missing drawables as we go.
    else
    {
        KAN_UMO_INDEXED_INSERT (drawable, kan_ui_node_drawable_t)
        {
            drawable->id = id;
            drawable->layout_dirt_level = dirt_level;
        }
    }

    KAN_UML_VALUE_READ (child, kan_ui_node_t, parent_id, &id)
    {
        mark_dirty_recursive (state, child->id, dirt_level, with_root_removal);
    }
}

static void mark_dirty_root (struct ui_layout_state_t *state,
                             kan_ui_node_id_t dirty_root,
                             enum kan_ui_layout_dirt_level_t dirt_level)
{
    if (!KAN_TYPED_ID_32_IS_VALID (dirty_root))
    {
        return;
    }

    KAN_UMI_VALUE_UPDATE_OPTIONAL (existent_drawable, kan_ui_node_drawable_t, id, &dirty_root)
    bool add_new_root = true;

    if (existent_drawable)
    {
        add_new_root = existent_drawable->layout_dirt_level == KAN_UI_LAYOUT_DIRT_LEVEL_NONE;
        if (existent_drawable->layout_dirt_level >= dirt_level)
        {
            // Already dirty at that level, nothing to do.
            return;
        }

        existent_drawable->layout_dirt_level = dirt_level;
    }
    // Initialize missing drawables as we go.
    else
    {
        KAN_UMO_INDEXED_INSERT (drawable, kan_ui_node_drawable_t)
        {
            drawable->id = dirty_root;
            drawable->layout_dirt_level = dirt_level;
        }
    }

    if (add_new_root)
    {
        struct ui_layout_dirty_root_t *new_root =
            KAN_STACK_GROUP_ALLOCATOR_ALLOCATE_TYPED (&state->temporary_allocator, struct ui_layout_dirty_root_t);
        new_root->root_node_id = dirty_root;

        new_root->next = state->transient.first_dirty_root;
        state->transient.first_dirty_root = new_root;
    }

    KAN_UML_VALUE_READ (child, kan_ui_node_t, parent_id, &dirty_root)
    {
        mark_dirty_recursive (state, child->id, dirt_level, add_new_root);
    }
}

KAN_REFLECTION_IGNORE
struct layout_child_access_t
{
    const struct kan_ui_node_t *child;
    struct kan_ui_node_drawable_t *drawable;
    struct kan_repository_indexed_value_read_access_t child_access;
    struct kan_repository_indexed_value_update_access_t drawable_access;
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

    // Cached margins are automatically collapsed during layout_whitespace_pass,
    // do not try to collapse them again after that.

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

        KAN_UMI_VALUE_UPDATE_REQUIRED (drawable, kan_ui_node_drawable_t, id, &child->id)
        access->access.child = child;
        KAN_UM_ACCESS_ESCAPE (access->access.child_access, child);

        access->access.drawable = drawable;
        KAN_UM_ACCESS_ESCAPE (access->access.drawable_access, drawable);

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
        ++copy_output;
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

#define UI_COLLAPSE_MARGIN(FIELD, BASELINE) (FIELD) = KAN_MAX ((FIELD) - (BASELINE), 0)

/// \details Returns true when short circuit mode detected and we can go to render finalize pass right away.
static bool layout_base_pass (struct ui_layout_state_t *state,
                              const struct kan_ui_node_t *node,
                              struct kan_ui_node_drawable_t *drawable)
{
    KAN_ASSERT (!drawable->temporary_data)
    struct layout_temporary_data_t *data = layout_temporary_data_create (state, node);
    drawable->temporary_data = data;

    // Short-circuit for non-full updates -- just go to the children and initialize temporary data with accesses.
    if (drawable->layout_dirt_level < KAN_UI_LAYOUT_DIRT_LEVEL_FULL)
    {
        bool short_circuit = true;
        for (kan_loop_size_t index = 0u; index < data->sorted_children_count; ++index)
        {
            const struct layout_child_access_t *access = &data->sorted_children[index];
            short_circuit &= layout_base_pass (state, access->child, access->drawable);
        }

        return short_circuit;
    }

    data->width_px = kan_ui_calculate_coordinate (state->transient.ui, node->element.width);
    data->height_px = kan_ui_calculate_coordinate (state->transient.ui, node->element.height);

    data->cached_margin_left_px = kan_ui_calculate_coordinate (state->transient.ui, node->element.margin.left);
    data->cached_margin_right_px = kan_ui_calculate_coordinate (state->transient.ui, node->element.margin.right);
    data->cached_margin_top_px = kan_ui_calculate_coordinate (state->transient.ui, node->element.margin.top);
    data->cached_margin_bottom_px = kan_ui_calculate_coordinate (state->transient.ui, node->element.margin.bottom);

    data->cached_padding_left_px = kan_ui_calculate_coordinate (state->transient.ui, node->layout.padding.left);
    data->cached_padding_right_px = kan_ui_calculate_coordinate (state->transient.ui, node->layout.padding.right);
    data->cached_padding_top_px = kan_ui_calculate_coordinate (state->transient.ui, node->layout.padding.top);
    data->cached_padding_bottom_px = kan_ui_calculate_coordinate (state->transient.ui, node->layout.padding.bottom);

    for (kan_loop_size_t index = 0u; index < data->sorted_children_count; ++index)
    {
        const struct layout_child_access_t *access = &data->sorted_children[index];
#if defined(KAN_WITH_ASSERT)
        const bool short_circuit =
#endif
            layout_base_pass (state, access->child, access->drawable);
        KAN_ASSERT (!short_circuit)
    }

    return false;
}

static void layout_whitespace_pass (struct ui_layout_state_t *state,
                                    const struct kan_ui_node_t *node,
                                    struct kan_ui_node_drawable_t *drawable)
{
    struct layout_temporary_data_t *data = drawable->temporary_data;

    // Short-circuit for non-full updates -- just go to the children.
    if (drawable->layout_dirt_level < KAN_UI_LAYOUT_DIRT_LEVEL_FULL)
    {
        for (kan_loop_size_t index = 0u; index < data->sorted_children_count; ++index)
        {
            const struct layout_child_access_t *access = &data->sorted_children[index];
            layout_whitespace_pass (state, access->child, access->drawable);
        }

        return;
    }

    // Compound margins are calculated as "real distances to nearest element or border" in here and are used for
    // properly collapsing outer margins for complex layout transitive relations. However, keep in mind that compound
    // margins are calculated between elements in the same layout, there are not designed to add padding between
    // elements of two sibling layouts if that padding didn't exist between those sibling layout nodes. They are only
    // designed to get rid of unnecessary additional paddings in between.

    const kan_instance_offset_t baseline_left = data->cached_padding_left_px + drawable->cached.compound_margin_left;
    const kan_instance_offset_t baseline_right = data->cached_padding_right_px + drawable->cached.compound_margin_right;
    const kan_instance_offset_t baseline_top = data->cached_padding_top_px + drawable->cached.compound_margin_top;
    const kan_instance_offset_t baseline_bottom =
        data->cached_padding_bottom_px + drawable->cached.compound_margin_bottom;

    struct kan_ui_node_drawable_t *previous_child = NULL;
    struct layout_temporary_data_t *previous_child_data = NULL;

    switch (data->cached_layout)
    {
    case KAN_UI_LAYOUT_FRAME:
    {
        // Frame layout has no margins, but we still need to propagate outer margins to the children.
        // For example, frame layout can be used to position internal scrollable vertical container and it its scroll
        // lines: despite the fact that container is inside frame, we'd like its internal elements to know about outer
        // margins and avoid introducing unnecessary whitespace inside that container.
        for (kan_loop_size_t index = 0u; index < data->sorted_children_count; ++index)
        {
            const struct layout_child_access_t *access = &data->sorted_children[index];
            struct kan_ui_node_drawable_t *child = access->drawable;
            struct layout_temporary_data_t *child_data = child->temporary_data;

            UI_COLLAPSE_MARGIN (child_data->cached_margin_left_px, baseline_left);
            UI_COLLAPSE_MARGIN (child_data->cached_margin_right_px, baseline_right);
            UI_COLLAPSE_MARGIN (child_data->cached_margin_top_px, baseline_top);
            UI_COLLAPSE_MARGIN (child_data->cached_margin_bottom_px, baseline_bottom);

            child->cached.compound_margin_left = child_data->cached_margin_left_px + baseline_left;
            child->cached.compound_margin_right = child_data->cached_margin_right_px + baseline_right;
            child->cached.compound_margin_top = child_data->cached_margin_top_px + baseline_top;
            child->cached.compound_margin_bottom = child_data->cached_margin_bottom_px + baseline_bottom;

            previous_child = child;
            previous_child_data = child_data;
        }

        break;
    }

    case KAN_UI_LAYOUT_VERTICAL_CONTAINER:
    {
        kan_instance_offset_t previous_margin = baseline_top;
        for (kan_loop_size_t index = 0u; index < data->sorted_children_count; ++index)
        {
            const struct layout_child_access_t *access = &data->sorted_children[index];
            struct kan_ui_node_drawable_t *child = access->drawable;
            struct layout_temporary_data_t *child_data = child->temporary_data;

            UI_COLLAPSE_MARGIN (child_data->cached_margin_left_px, baseline_left);
            UI_COLLAPSE_MARGIN (child_data->cached_margin_right_px, baseline_right);
            UI_COLLAPSE_MARGIN (child_data->cached_margin_top_px, previous_margin);

            child->cached.compound_margin_left = child_data->cached_margin_left_px + baseline_left;
            child->cached.compound_margin_right = child_data->cached_margin_right_px + baseline_right;
            child->cached.compound_margin_top = child_data->cached_margin_top_px + previous_margin;

            if (previous_child)
            {
                previous_child->cached.compound_margin_bottom = child->cached.compound_margin_top;
            }

            previous_margin = child_data->cached_margin_bottom_px;
            previous_child = child;
            previous_child_data = child_data;
        }

        if (previous_child_data)
        {
            UI_COLLAPSE_MARGIN (previous_child_data->cached_margin_bottom_px, baseline_bottom);
            previous_child->cached.compound_margin_bottom =
                previous_child_data->cached_margin_bottom_px + baseline_bottom;
        }

        break;
    }

    case KAN_UI_LAYOUT_HORIZONTAL_CONTAINER:
    {
        kan_instance_offset_t previous_margin = baseline_left;
        for (kan_loop_size_t index = 0u; index < data->sorted_children_count; ++index)
        {
            const struct layout_child_access_t *access = &data->sorted_children[index];
            struct kan_ui_node_drawable_t *child = access->drawable;
            struct layout_temporary_data_t *child_data = child->temporary_data;

            UI_COLLAPSE_MARGIN (child_data->cached_margin_left_px, previous_margin);
            UI_COLLAPSE_MARGIN (child_data->cached_margin_top_px, baseline_top);
            UI_COLLAPSE_MARGIN (child_data->cached_margin_bottom_px, baseline_bottom);

            child->cached.compound_margin_left = child_data->cached_margin_left_px + previous_margin;
            if (previous_child)
            {
                previous_child->cached.compound_margin_right = child->cached.compound_margin_left;
            }

            child->cached.compound_margin_top = child_data->cached_margin_top_px + baseline_top;
            child->cached.compound_margin_bottom = child_data->cached_margin_bottom_px + baseline_bottom;

            previous_margin = child_data->cached_margin_right_px;
            previous_child = child;
            previous_child_data = child_data;
        }

        if (previous_child_data)
        {
            UI_COLLAPSE_MARGIN (previous_child_data->cached_margin_right_px, baseline_right);
            previous_child->cached.compound_margin_right = previous_child_data->cached_margin_right_px + baseline_right;
        }

        break;
    }
    }

    for (kan_loop_size_t index = 0u; index < data->sorted_children_count; ++index)
    {
        const struct layout_child_access_t *access = &data->sorted_children[index];
        layout_whitespace_pass (state, access->child, access->drawable);
    }
}

static void layout_size_pass (struct ui_layout_state_t *state,
                              const struct kan_ui_node_t *node,
                              struct kan_ui_node_drawable_t *drawable)
{
    struct layout_temporary_data_t *data = drawable->temporary_data;

    // Short-circuit for non-full updates -- just go to the children.
    if (drawable->layout_dirt_level < KAN_UI_LAYOUT_DIRT_LEVEL_FULL)
    {
        for (kan_loop_size_t index = 0u; index < data->sorted_children_count; ++index)
        {
            const struct layout_child_access_t *access = &data->sorted_children[index];
            layout_size_pass (state, access->child, access->drawable);
        }

        return;
    }

    for (kan_loop_size_t index = 0u; index < data->sorted_children_count; ++index)
    {
        const struct layout_child_access_t *access = &data->sorted_children[index];
        layout_size_pass (state, access->child, access->drawable);
        struct layout_temporary_data_t *child_data = access->drawable->temporary_data;

        const kan_instance_offset_t occupied_width =
            child_data->width_px + child_data->cached_margin_left_px + child_data->cached_margin_right_px;

        const kan_instance_offset_t occupied_height =
            child_data->height_px + child_data->cached_margin_top_px + child_data->cached_margin_bottom_px;

        switch (data->cached_layout)
        {
        case KAN_UI_LAYOUT_FRAME:
            data->children_width_usage_px = KAN_MAX (data->children_width_usage_px, occupied_width);
            data->children_height_usage_px = KAN_MAX (data->children_height_usage_px, occupied_height);
            break;

        case KAN_UI_LAYOUT_VERTICAL_CONTAINER:
            data->children_width_usage_px = KAN_MAX (data->children_width_usage_px, occupied_width);
            data->children_height_usage_px += occupied_height;
            break;

        case KAN_UI_LAYOUT_HORIZONTAL_CONTAINER:
            data->children_width_usage_px += occupied_width;
            data->children_height_usage_px = KAN_MAX (data->children_height_usage_px, occupied_height);
            break;
        }
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
}

KAN_REFLECTION_IGNORE
struct layout_grow_node_t
{
    struct layout_grow_node_t *next;
    const struct layout_child_access_t *access;
};

static void layout_grow_pass (struct ui_layout_state_t *state,
                              const struct kan_ui_node_t *node,
                              struct kan_ui_node_drawable_t *drawable,
                              bool root)
{
    struct layout_temporary_data_t *data = drawable->temporary_data;

    // Short-circuit for non-full updates -- just go to the children.
    if (drawable->layout_dirt_level < KAN_UI_LAYOUT_DIRT_LEVEL_FULL)
    {
        for (kan_loop_size_t index = 0u; index < data->sorted_children_count; ++index)
        {
            const struct layout_child_access_t *access = &data->sorted_children[index];
            layout_grow_pass (state, access->child, access->drawable, root);
        }

        return;
    }

    if (root)
    {
        // When root, need to grow itself from cached growth.
        if (node->element.width_flags & KAN_UI_SIZE_FLAG_GROW)
        {
            data->width_px += drawable->cached.grow_width;
        }

        if (node->element.height_flags & KAN_UI_SIZE_FLAG_GROW)
        {
            data->height_px += drawable->cached.grow_height;
        }
    }

    // Number of growing children should be relatively small, therefore no need for linearizing and quick sort here.
    struct layout_grow_node_t *first_grow_width_node = NULL;
    struct layout_grow_node_t *first_grow_height_node = NULL;

    for (kan_loop_size_t index = 0u; index < data->sorted_children_count; ++index)
    {
        const struct layout_child_access_t *access = &data->sorted_children[index];
        struct layout_temporary_data_t *child_data = access->drawable->temporary_data;
        access->drawable->cached.grow_width = 0;
        access->drawable->cached.grow_height = 0;

        if (access->child->element.width_flags & KAN_UI_SIZE_FLAG_GROW)
        {
            if (data->cached_layout == KAN_UI_LAYOUT_HORIZONTAL_CONTAINER)
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
        struct layout_temporary_data_t *other_data = push_before->access->drawable->temporary_data;                    \
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
            else
            {
                // Not a horizontal container, therefore width is not shared and we can use all space available.
                const kan_instance_offset_t width_available =
                    data->width_px - data->cached_padding_left_px - data->cached_padding_right_px -
                    child_data->cached_margin_left_px - child_data->cached_margin_right_px;

                if (child_data->width_px < width_available)
                {
                    access->drawable->cached.grow_width = width_available - child_data->width_px;
                    child_data->width_px = width_available;
                }
            }
        }

        if (access->child->element.height_flags & KAN_UI_SIZE_FLAG_GROW)
        {
            if (data->cached_layout == KAN_UI_LAYOUT_VERTICAL_CONTAINER)
            {
                struct layout_grow_node_t *grow_node =
                    KAN_STACK_GROUP_ALLOCATOR_ALLOCATE_TYPED (&state->temporary_allocator, struct layout_grow_node_t);
                grow_node->access = access;

                PUSH_GROW_NODE (height)
#undef PUSH_GROW_NODE
            }
            else
            {
                // Not a vertical container, therefore height is not shared and we can use all space available.
                const kan_instance_offset_t height_available =
                    data->height_px - data->cached_padding_top_px - data->cached_padding_bottom_px -
                    child_data->cached_margin_top_px - child_data->cached_margin_bottom_px;

                if (child_data->height_px < height_available)
                {
                    access->drawable->cached.grow_height = height_available - child_data->height_px;
                    child_data->height_px = height_available;
                }
            }
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
                first_grow_##AXIS_NAME##_node->access->drawable->temporary_data;                                       \
            struct layout_grow_node_t *barrier = first_grow_##AXIS_NAME##_node->next;                                  \
                                                                                                                       \
            while (barrier)                                                                                            \
            {                                                                                                          \
                struct layout_temporary_data_t *second_data = barrier->access->drawable->temporary_data;               \
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
                struct layout_temporary_data_t *barrier_data = barrier->access->drawable->temporary_data;              \
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
                struct layout_temporary_data_t *receiver_data = receiver->access->drawable->temporary_data;            \
                receiver_data->AXIS_NAME##_px += give_every;                                                           \
                receiver->access->drawable->cached.grow_##AXIS_NAME += give_every;                                     \
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
        layout_grow_pass (state, access->child, access->drawable, false);
    }
}

static void layout_position_pass (struct ui_layout_state_t *state,
                                  const struct kan_ui_node_t *node,
                                  struct kan_ui_node_drawable_t *drawable)
{
    struct layout_temporary_data_t *data = drawable->temporary_data;

    // Short-circuit for non-full updates -- just go to the children.
    if (drawable->layout_dirt_level < KAN_UI_LAYOUT_DIRT_LEVEL_FULL)
    {
        for (kan_loop_size_t index = 0u; index < data->sorted_children_count; ++index)
        {
            const struct layout_child_access_t *access = &data->sorted_children[index];
            layout_position_pass (state, access->child, access->drawable);
        }

        return;
    }

    drawable->width = data->width_px;
    drawable->height = data->height_px;

    if (!KAN_TYPED_ID_32_IS_VALID (node->parent_id))
    {
        // Child of root frame, align accordingly.
        drawable->local_x = kan_ui_calculate_coordinate (state->transient.ui, node->element.frame_offset_x);
        drawable->local_y = kan_ui_calculate_coordinate (state->transient.ui, node->element.frame_offset_y);

        switch (node->element.horizontal_alignment)
        {
        case KAN_UI_HORIZONTAL_ALIGNMENT_LEFT:
            break;

        case KAN_UI_HORIZONTAL_ALIGNMENT_CENTER:
            drawable->local_x += state->transient.ui->viewport_width / 2 - data->width_px / 2;
            break;

        case KAN_UI_HORIZONTAL_ALIGNMENT_RIGHT:
            drawable->local_x += state->transient.ui->viewport_width - data->width_px;
            break;
        }

        switch (node->element.vertical_alignment)
        {
        case KAN_UI_VERTICAL_ALIGNMENT_TOP:
            break;

        case KAN_UI_VERTICAL_ALIGNMENT_CENTER:
            drawable->local_y += state->transient.ui->viewport_height / 2 - data->height_px / 2;
            break;

        case KAN_UI_VERTICAL_ALIGNMENT_BOTTOM:
            drawable->local_y += state->transient.ui->viewport_height - data->height_px;
            break;
        }

        drawable->global_x = drawable->local_x;
        drawable->global_y = drawable->local_y;
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
            struct layout_temporary_data_t *child_data = access->drawable->temporary_data;

            access->drawable->local_x =
                pad_left + kan_ui_calculate_coordinate (state->transient.ui, access->child->element.frame_offset_x);

            switch (access->child->element.horizontal_alignment)
            {
            case KAN_UI_HORIZONTAL_ALIGNMENT_LEFT:
                break;

            case KAN_UI_HORIZONTAL_ALIGNMENT_CENTER:
                access->drawable->local_x += available_width / 2 - child_data->width_px / 2;
                break;

            case KAN_UI_HORIZONTAL_ALIGNMENT_RIGHT:
                access->drawable->local_x += available_width - child_data->width_px;
                break;
            }

            access->drawable->local_y =
                pad_top + kan_ui_calculate_coordinate (state->transient.ui, access->child->element.frame_offset_y);

            switch (access->child->element.vertical_alignment)
            {
            case KAN_UI_VERTICAL_ALIGNMENT_TOP:
                break;

            case KAN_UI_VERTICAL_ALIGNMENT_CENTER:
                access->drawable->local_y += available_height / 2 - child_data->height_px / 2;
                break;

            case KAN_UI_VERTICAL_ALIGNMENT_BOTTOM:
                access->drawable->local_y += available_height - child_data->height_px;
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
            struct layout_temporary_data_t *child_data = access->drawable->temporary_data;
            cursor += child_data->cached_margin_top_px;

            access->drawable->local_x = pad_left + child_data->cached_margin_left_px;
            access->drawable->local_y = cursor;
            cursor += child_data->height_px + child_data->cached_margin_bottom_px;

            switch (access->child->element.horizontal_alignment)
            {
            case KAN_UI_HORIZONTAL_ALIGNMENT_LEFT:
                break;

            case KAN_UI_HORIZONTAL_ALIGNMENT_CENTER:
                access->drawable->local_x +=
                    (available_width - child_data->cached_margin_left_px - child_data->cached_margin_right_px) / 2 -
                    child_data->width_px / 2;
                break;

            case KAN_UI_HORIZONTAL_ALIGNMENT_RIGHT:
                access->drawable->local_x += available_width - child_data->cached_margin_left_px -
                                             child_data->cached_margin_right_px - child_data->width_px;
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
            struct layout_temporary_data_t *child_data = access->drawable->temporary_data;
            cursor += child_data->cached_margin_left_px;

            access->drawable->local_x = cursor;
            access->drawable->local_y = pad_top + child_data->cached_margin_top_px;
            cursor += child_data->width_px + child_data->cached_margin_right_px;

            switch (access->child->element.vertical_alignment)
            {
            case KAN_UI_VERTICAL_ALIGNMENT_TOP:
                break;

            case KAN_UI_VERTICAL_ALIGNMENT_CENTER:
                access->drawable->local_y +=
                    (available_height - child_data->cached_margin_top_px - child_data->cached_margin_bottom_px) / 2 -
                    child_data->height_px / 2;
                break;

            case KAN_UI_VERTICAL_ALIGNMENT_BOTTOM:
                access->drawable->local_y += available_height - child_data->cached_margin_top_px -
                                             child_data->cached_margin_bottom_px - child_data->height_px;
                break;
            }
        }

        break;
    }
    }

    for (kan_loop_size_t index = 0u; index < data->sorted_children_count; ++index)
    {
        const struct layout_child_access_t *access = &data->sorted_children[index];
        layout_position_pass (state, access->child, access->drawable);
    }
}

static void layout_render_finalize_pass (struct ui_layout_state_t *state,
                                         const struct kan_ui_node_t *node,
                                         struct kan_ui_node_drawable_t *drawable)
{
    struct layout_temporary_data_t *data = drawable->temporary_data;
    if (node->render.clip)
    {
        struct kan_ui_clip_rect_t my_rect = {
            .x = drawable->global_x,
            .y = drawable->global_y,
            .width = drawable->width,
            .height = drawable->height,
        };

        struct kan_ui_clip_rect_t new_clip_rect = drawable->clip_rect;
        new_clip_rect.x = KAN_MAX (drawable->clip_rect.x, my_rect.x);
        new_clip_rect.y = KAN_MAX (drawable->clip_rect.y, my_rect.y);

        new_clip_rect.width =
            KAN_MIN (drawable->clip_rect.x + drawable->clip_rect.width, my_rect.x + my_rect.width) - new_clip_rect.x;

        new_clip_rect.height =
            KAN_MIN (drawable->clip_rect.y + drawable->clip_rect.height, my_rect.y + my_rect.height) - new_clip_rect.y;
        drawable->clip_rect = new_clip_rect;
    }

    for (kan_loop_size_t index = 0u; index < data->sorted_children_count; ++index)
    {
        struct layout_child_access_t *access = &data->sorted_children[index];
        access->drawable->clip_rect = drawable->clip_rect;

        access->drawable->global_x = drawable->global_x -
                                     kan_ui_calculate_coordinate (state->transient.ui, node->render.scroll_x) +
                                     access->drawable->local_x;

        access->drawable->global_y = drawable->global_y -
                                     kan_ui_calculate_coordinate (state->transient.ui, node->render.scroll_y) +
                                     access->drawable->local_y;

        access->drawable->fully_clipped_out =
            access->drawable->global_x + access->drawable->width < drawable->clip_rect.x ||
            access->drawable->global_x >= drawable->clip_rect.x + drawable->clip_rect.width ||
            access->drawable->global_y + access->drawable->height < drawable->clip_rect.y ||
            access->drawable->global_y >= drawable->clip_rect.y + drawable->clip_rect.height;

        layout_render_finalize_pass (state, access->child, access->drawable);

        // We should no longer need the accesses and we need to close them.
        kan_repository_indexed_value_read_access_close (&access->child_access);
        kan_repository_indexed_value_update_access_close (&access->drawable_access);
    }

    // We should no longer use temporary data.
    drawable->temporary_data = NULL;
    drawable->layout_dirt_level = KAN_UI_LAYOUT_DIRT_LEVEL_NONE;

    if (node->event_on_laid_out)
    {
        KAN_UMO_EVENT_INSERT_INIT (kan_ui_node_laid_out_t) {.node_id = drawable->id};
    }
}

static void recalculate_layout (struct ui_layout_state_t *state, kan_ui_node_id_t root_id)
{
    KAN_UMI_VALUE_READ_REQUIRED (node, kan_ui_node_t, id, &root_id)
    KAN_UMI_VALUE_UPDATE_REQUIRED (drawable, kan_ui_node_drawable_t, id, &root_id)

    const bool short_circuit = layout_base_pass (state, node, drawable);
    if (!short_circuit)
    {
        layout_whitespace_pass (state, node, drawable);
        layout_size_pass (state, node, drawable);
        layout_grow_pass (state, node, drawable, true);
        layout_position_pass (state, node, drawable);
    }

    if (!KAN_TYPED_ID_32_IS_VALID (node->parent_id))
    {
        drawable->clip_rect = (struct kan_ui_clip_rect_t) {
            .x = 0,
            .y = 0,
            .width = state->transient.ui->viewport_width,
            .height = state->transient.ui->viewport_height,
        };
    }

    layout_render_finalize_pass (state, node, drawable);
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
        access->drawable->draw_index = state->transient.reorder_index;
        ++state->transient.reorder_index;
        execute_draw_index_reorder (state, access->child);

        // We should no longer need the accesses and we need to close them.
        kan_repository_indexed_value_read_access_close (&access->child_access);
        kan_repository_indexed_value_update_access_close (&access->drawable_access);
    }
}

UNIVERSE_UI_API KAN_UM_MUTATOR_EXECUTE (ui_layout)
{
    KAN_UMI_SINGLETON_READ (ui, kan_ui_singleton_t)
    state->transient.ui = ui;
    state->transient.reorder_required = false;
    state->transient.first_dirty_root = NULL;
    state->transient.reorder_index = 0u;

    {
        KAN_CPU_SCOPED_STATIC_SECTION (ui_layout_node_lifetime_events)
        KAN_UML_EVENT_FETCH (node_on_insert, kan_ui_node_on_insert_event_t)
        {
            state->transient.reorder_required = true;
            KAN_UMI_VALUE_READ_OPTIONAL (node, kan_ui_node_t, id, &node_on_insert->id)

            if (!node)
            {
                continue;
            }

            // Ensure that drawable exists.
            {
                KAN_UMI_VALUE_READ_OPTIONAL (existent, kan_ui_node_drawable_t, id, &node_on_insert->id)
                if (!existent)
                {
                    KAN_UMO_INDEXED_INSERT (drawable, kan_ui_node_drawable_t) { drawable->id = node->id; }
                }
            }

            KAN_UMI_VALUE_READ_OPTIONAL (parent, kan_ui_node_t, id, &node->parent_id)
            if (parent)
            {
                const kan_ui_node_id_t dirty_root = determine_dirty_root_recursive (state, parent, true);
                mark_dirty_root (state, dirty_root, KAN_UI_LAYOUT_DIRT_LEVEL_FULL);

                // If parents are already marked, our drawable might not receive a mark. Fixup it.
                KAN_UMI_VALUE_UPDATE_REQUIRED (drawable, kan_ui_node_drawable_t, id, &node_on_insert->id)
                drawable->layout_dirt_level = KAN_UI_LAYOUT_DIRT_LEVEL_FULL;
            }
            else
            {
                mark_dirty_root (state, node->id, KAN_UI_LAYOUT_DIRT_LEVEL_FULL);
            }
        }

        KAN_UML_EVENT_FETCH (node_on_delete, kan_ui_node_on_delete_event_t)
        {
            state->transient.reorder_required = true;
            KAN_UMI_VALUE_READ_OPTIONAL (parent, kan_ui_node_t, id, &node_on_delete->parent_id)

            if (parent)
            {
                const kan_ui_node_id_t dirty_root = determine_dirty_root_recursive (state, parent, true);
                mark_dirty_root (state, dirty_root, KAN_UI_LAYOUT_DIRT_LEVEL_FULL);
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
                mark_dirty_root (state, node->id, KAN_UI_LAYOUT_DIRT_LEVEL_FULL);
            }
        }
    }

    {
        KAN_CPU_SCOPED_STATIC_SECTION (ui_layout_node_change_main_events)
        KAN_UML_EVENT_FETCH (node_on_element, kan_ui_node_on_change_event_t)
        {
            KAN_UMI_VALUE_READ_OPTIONAL (node, kan_ui_node_t, id, &node_on_element->id)
            if (node)
            {
                const kan_ui_node_id_t dirty_root = determine_dirty_root_recursive (state, node, false);
                mark_dirty_root (state, dirty_root, KAN_UI_LAYOUT_DIRT_LEVEL_FULL);
            }
        }

        KAN_UML_EVENT_FETCH (node_on_order, kan_ui_node_order_on_change_event_t)
        {
            state->transient.reorder_required = true;
            KAN_UMI_VALUE_READ_OPTIONAL (node, kan_ui_node_t, id, &node_on_order->id)
            if (node)
            {
                const kan_ui_node_id_t dirty_root = determine_dirty_root_recursive (state, node, false);
                mark_dirty_root (state, dirty_root, KAN_UI_LAYOUT_DIRT_LEVEL_FULL);
            }
        }

        KAN_UML_EVENT_FETCH (node_on_layout, kan_ui_node_layout_on_change_event_t)
        {
            KAN_UMI_VALUE_READ_OPTIONAL (node, kan_ui_node_t, id, &node_on_layout->id)
            if (node)
            {
                const kan_ui_node_id_t dirty_root = determine_dirty_root_recursive (state, node, true);
                mark_dirty_root (state, dirty_root, KAN_UI_LAYOUT_DIRT_LEVEL_FULL);
            }
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
                mark_dirty_root (state, node_on_render->id, KAN_UI_LAYOUT_DIRT_LEVEL_ONLY_RENDER);
            }
        }
    }

    {
        KAN_CPU_SCOPED_STATIC_SECTION (ui_layout_recalculate)
        struct ui_layout_dirty_root_t *root = state->transient.first_dirty_root;

        while (root)
        {
            recalculate_layout (state, root->root_node_id);
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

enum ui_bundle_loading_state_t
{
    UI_BUNDLE_LOADING_STATE_INITIAL = 0u,
    UI_BUNDLE_LOADING_STATE_WAITING_MAIN,
    UI_BUNDLE_LOADING_STATE_WAITING_RESOURCES,
    UI_BUNDLE_LOADING_STATE_READY,
};

struct ui_bundle_private_singleton_t
{
    enum ui_bundle_loading_state_t state;
    kan_instance_size_t state_frame_id;

    kan_resource_usage_id_t main_usage_id;
    kan_render_material_instance_usage_id_t loading_image_material_instance_usage_id;
    kan_render_atlas_usage_id_t loading_image_atlas_usage_id;
    kan_render_material_instance_usage_id_t loading_text_sdf_usage_id;
    kan_render_material_instance_usage_id_t loading_text_icon_usage_id;

    kan_render_material_instance_usage_id_t available_image_material_instance_usage_id;
    kan_render_atlas_usage_id_t available_image_atlas_usage_id;
    kan_render_material_instance_usage_id_t available_text_sdf_usage_id;
    kan_render_material_instance_usage_id_t available_text_icon_usage_id;
};

UNIVERSE_UI_API void ui_bundle_private_singleton_init (struct ui_bundle_private_singleton_t *instance)
{
    instance->state = UI_BUNDLE_LOADING_STATE_INITIAL;
    instance->state_frame_id = 0u;

    instance->main_usage_id = KAN_TYPED_ID_32_SET_INVALID (kan_resource_usage_id_t);
    instance->loading_image_material_instance_usage_id =
        KAN_TYPED_ID_32_SET_INVALID (kan_render_material_instance_usage_id_t);
    instance->loading_image_atlas_usage_id = KAN_TYPED_ID_32_SET_INVALID (kan_render_atlas_usage_id_t);
    instance->loading_text_sdf_usage_id = KAN_TYPED_ID_32_SET_INVALID (kan_render_material_instance_usage_id_t);
    instance->loading_text_icon_usage_id = KAN_TYPED_ID_32_SET_INVALID (kan_render_material_instance_usage_id_t);

    instance->available_image_material_instance_usage_id =
        KAN_TYPED_ID_32_SET_INVALID (kan_render_material_instance_usage_id_t);
    instance->available_image_atlas_usage_id = KAN_TYPED_ID_32_SET_INVALID (kan_render_atlas_usage_id_t);
    instance->available_text_sdf_usage_id = KAN_TYPED_ID_32_SET_INVALID (kan_render_material_instance_usage_id_t);
    instance->available_text_icon_usage_id = KAN_TYPED_ID_32_SET_INVALID (kan_render_material_instance_usage_id_t);
}

struct ui_bundle_management_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (ui_bundle_management)
    KAN_UM_BIND_STATE (ui_bundle_management, state)

    kan_interned_string_t default_bundle;
};

UNIVERSE_UI_API KAN_UM_MUTATOR_DEPLOY (ui_bundle_management)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_cpu_static_sections_ensure_initialized ();

    state->default_bundle = NULL;
    const struct kan_ui_configuration_t *configuration =
        kan_universe_world_query_configuration (world, kan_string_intern (KAN_UI_CONFIGURATION));

    if (configuration)
    {
        state->default_bundle = configuration->default_bundle_name;
    }

    kan_workflow_graph_node_depend_on (workflow_node, KAN_RESOURCE_PROVIDER_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_RENDER_FOUNDATION_ATLAS_MANAGEMENT_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_RENDER_FOUNDATION_FRAME_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_RENDER_FOUNDATION_PROGRAM_MANAGEMENT_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_UI_BUNDLE_MANAGEMENT_BEGIN_CHECKPOINT);
    kan_workflow_graph_node_make_dependency_of (workflow_node, KAN_UI_BUNDLE_MANAGEMENT_END_CHECKPOINT);
}

static void advance_bundle_from_initial_state (struct ui_bundle_management_state_t *state,
                                               const struct kan_resource_provider_singleton_t *provider,
                                               struct kan_ui_bundle_singleton_t *public,
                                               struct ui_bundle_private_singleton_t *private);

static void advance_bundle_from_waiting_main_state (struct ui_bundle_management_state_t *state,
                                                    const struct kan_resource_provider_singleton_t *provider,
                                                    struct kan_ui_bundle_singleton_t *public,
                                                    struct ui_bundle_private_singleton_t *private);

static void advance_bundle_from_waiting_resources_state (struct ui_bundle_management_state_t *state,
                                                         const struct kan_resource_provider_singleton_t *provider,
                                                         struct kan_ui_bundle_singleton_t *public,
                                                         struct ui_bundle_private_singleton_t *private);

static void on_bundle_resource_updated (struct ui_bundle_management_state_t *state,
                                        const struct kan_resource_provider_singleton_t *provider,
                                        struct kan_ui_bundle_singleton_t *public,
                                        struct ui_bundle_private_singleton_t *private)
{
    private->state = UI_BUNDLE_LOADING_STATE_INITIAL;
    private->state_frame_id = provider->logic_deduplication_frame_id;

    // Start advancing, having some data loaded is very likely here.
    advance_bundle_from_initial_state (state, provider, public, private);
}

static void clear_bundle_loading_resource_usages (struct ui_bundle_private_singleton_t *private) {}

static void advance_bundle_from_initial_state (struct ui_bundle_management_state_t *state,
                                               const struct kan_resource_provider_singleton_t *provider,
                                               struct kan_ui_bundle_singleton_t *public,
                                               struct ui_bundle_private_singleton_t *private)
{
    KAN_LOG (ui_bundle_management, KAN_LOG_DEBUG,
             "Attempting to advance bundle \"%s\" state from initial to waiting main.", public->bundle_name)

    private->state_frame_id = provider->logic_deduplication_frame_id;
    private->state = UI_BUNDLE_LOADING_STATE_WAITING_MAIN; // We will always advance from initial state.

    // Clear resource usages that are used for loading.

    if (KAN_TYPED_ID_32_IS_VALID (private->main_usage_id))
    {
        KAN_UMI_VALUE_DETACH_REQUIRED (usage, kan_resource_usage_t, usage_id, &private->main_usage_id)
        KAN_UM_ACCESS_DELETE (usage);
    }

    if (KAN_TYPED_ID_32_IS_VALID (private->loading_image_material_instance_usage_id))
    {
        KAN_UMI_VALUE_DETACH_REQUIRED (usage, kan_render_material_instance_usage_t, usage_id,
                                       &private->loading_image_material_instance_usage_id)
        KAN_UM_ACCESS_DELETE (usage);
    }

    if (KAN_TYPED_ID_32_IS_VALID (private->loading_image_atlas_usage_id))
    {
        KAN_UMI_VALUE_DETACH_REQUIRED (usage, kan_render_atlas_usage_t, usage_id,
                                       &private->loading_image_atlas_usage_id)
        KAN_UM_ACCESS_DELETE (usage);
    }

    if (KAN_TYPED_ID_32_IS_VALID (private->loading_text_sdf_usage_id))
    {
        KAN_UMI_VALUE_DETACH_REQUIRED (usage, kan_render_material_instance_usage_t, usage_id,
                                       &private->loading_text_sdf_usage_id)
        KAN_UM_ACCESS_DELETE (usage);
    }

    if (KAN_TYPED_ID_32_IS_VALID (private->loading_text_icon_usage_id))
    {
        KAN_UMI_VALUE_DETACH_REQUIRED (usage, kan_render_material_instance_usage_t, usage_id,
                                       &private->loading_text_icon_usage_id)
        KAN_UM_ACCESS_DELETE (usage);
    }

    private->main_usage_id = kan_next_resource_usage_id (provider);
    KAN_UMO_INDEXED_INSERT (usage, kan_resource_usage_t)
    {
        usage->usage_id = private->main_usage_id;
        usage->type = KAN_STATIC_INTERNED_ID_GET (kan_resource_ui_bundle_t);
        usage->name = public->bundle_name;
        usage->priority = KAN_UNIVERSE_UI_BUNDLE_PRIORITY;
    }

    advance_bundle_from_waiting_main_state (state, provider, public, private);
}

static void advance_bundle_from_waiting_main_state (struct ui_bundle_management_state_t *state,
                                                    const struct kan_resource_provider_singleton_t *provider,
                                                    struct kan_ui_bundle_singleton_t *public,
                                                    struct ui_bundle_private_singleton_t *private)
{
    KAN_LOG (ui_bundle_management, KAN_LOG_DEBUG,
             "Attempting to advance bundle \"%s\" state from waiting main to waiting resources.", public->bundle_name)

    private->state_frame_id = provider->logic_deduplication_frame_id;
    KAN_UMI_RESOURCE_RETRIEVE_IF_LOADED_AND_FRESH (resource, kan_resource_ui_bundle_t, &public->bundle_name)

    if (!resource)
    {
        // Still waiting.
        return;
    }

    KAN_UMI_SINGLETON_READ (atlas_singleton, kan_render_atlas_singleton_t)
    KAN_UMI_SINGLETON_READ (program_singleton, kan_render_program_singleton_t)

    private->state = UI_BUNDLE_LOADING_STATE_WAITING_RESOURCES;
    private->loading_image_material_instance_usage_id = kan_next_material_instance_usage_id (program_singleton);
    private->loading_image_atlas_usage_id = kan_next_atlas_usage_id (atlas_singleton);
    private->loading_text_sdf_usage_id = kan_next_material_instance_usage_id (program_singleton);
    private->loading_text_icon_usage_id = kan_next_material_instance_usage_id (program_singleton);

    KAN_UMO_INDEXED_INSERT (image_material_instance_usage, kan_render_material_instance_usage_t)
    {
        image_material_instance_usage->usage_id = private->loading_image_material_instance_usage_id;
        image_material_instance_usage->name = resource->image_material_instance;
    }

    KAN_UMO_INDEXED_INSERT (atlas_usage, kan_render_atlas_usage_t)
    {
        atlas_usage->usage_id = private->loading_image_atlas_usage_id;
        atlas_usage->name = resource->image_atlas;
    }

    KAN_UMO_INDEXED_INSERT (text_sdf_usage, kan_render_material_instance_usage_t)
    {
        text_sdf_usage->usage_id = private->loading_text_sdf_usage_id;
        text_sdf_usage->name = resource->text_sdf_material_instance;
    }

    KAN_UMO_INDEXED_INSERT (text_icon_usage, kan_render_material_instance_usage_t)
    {
        text_icon_usage->usage_id = private->loading_text_icon_usage_id;
        text_icon_usage->name = resource->text_icon_material_instance;
    }

    advance_bundle_from_waiting_resources_state (state, provider, public, private);
}

static void advance_bundle_from_waiting_resources_state (struct ui_bundle_management_state_t *state,
                                                         const struct kan_resource_provider_singleton_t *provider,
                                                         struct kan_ui_bundle_singleton_t *public,
                                                         struct ui_bundle_private_singleton_t *private)
{
    KAN_LOG (ui_bundle_management, KAN_LOG_DEBUG,
             "Attempting to advance bundle \"%s\" state from waiting resources to ready.", public->bundle_name)

    private->state_frame_id = provider->logic_deduplication_frame_id;
    KAN_UMI_RESOURCE_RETRIEVE_IF_LOADED_AND_FRESH (resource, kan_resource_ui_bundle_t, &public->bundle_name)
    KAN_ASSERT (resource)

    KAN_UMI_VALUE_READ_OPTIONAL (image_material_instance, kan_render_material_instance_loaded_t, name,
                                 &resource->image_material_instance)

    if (!image_material_instance)
    {
        return;
    }

    KAN_UMI_VALUE_READ_OPTIONAL (pass, kan_render_foundation_pass_loaded_t, name, &resource->pass)
    if (!pass)
    {
        return;
    }

    KAN_UMI_VALUE_READ_OPTIONAL (image_atlas, kan_render_atlas_loaded_t, name, &resource->image_atlas)
    if (!image_atlas)
    {
        return;
    }

    KAN_UMI_VALUE_READ_OPTIONAL (text_sdf_material_instance, kan_render_material_instance_loaded_t, name,
                                 &resource->text_sdf_material_instance)

    if (!text_sdf_material_instance)
    {
        return;
    }

    KAN_UMI_VALUE_READ_OPTIONAL (text_icon_material_instance, kan_render_material_instance_loaded_t, name,
                                 &resource->text_icon_material_instance)

    if (!text_icon_material_instance)
    {
        return;
    }

    // Everything is loaded, so we can finalize the loading now.

    private->state = UI_BUNDLE_LOADING_STATE_READY;
    public->available = true;
    public->available_bundle.pass = resource->pass;
    public->available_bundle.image_material_instance = resource->image_material_instance;
    public->available_bundle.image_atlas = resource->image_atlas;
    public->available_bundle.text_sdf_material_instance = resource->text_sdf_material_instance;
    public->available_bundle.text_icon_material_instance = resource->text_icon_material_instance;

    public->available_bundle.hit_box_interaction_styles.size = 0u;
    kan_dynamic_array_set_capacity (&public->available_bundle.hit_box_interaction_styles,
                                    resource->hit_box_interaction_styles.size);
    public->available_bundle.hit_box_interaction_styles.size = resource->hit_box_interaction_styles.size;

    memcpy (public->available_bundle.hit_box_interaction_styles.data, resource->hit_box_interaction_styles.data,
            sizeof (struct kan_resource_ui_hit_box_interaction_style_t) * resource->hit_box_interaction_styles.size);

    // Remove usages of old available data.

    if (KAN_TYPED_ID_32_IS_VALID (private->available_image_material_instance_usage_id))
    {
        KAN_UMI_VALUE_DETACH_REQUIRED (usage, kan_render_material_instance_usage_t, usage_id,
                                       &private->available_image_material_instance_usage_id)
        KAN_UM_ACCESS_DELETE (usage);
    }

    if (KAN_TYPED_ID_32_IS_VALID (private->available_image_atlas_usage_id))
    {
        KAN_UMI_VALUE_DETACH_REQUIRED (usage, kan_render_atlas_usage_t, usage_id,
                                       &private->available_image_atlas_usage_id)
        KAN_UM_ACCESS_DELETE (usage);
    }

    if (KAN_TYPED_ID_32_IS_VALID (private->available_text_sdf_usage_id))
    {
        KAN_UMI_VALUE_DETACH_REQUIRED (usage, kan_render_material_instance_usage_t, usage_id,
                                       &private->available_text_sdf_usage_id)
        KAN_UM_ACCESS_DELETE (usage);
    }

    if (KAN_TYPED_ID_32_IS_VALID (private->available_text_icon_usage_id))
    {
        KAN_UMI_VALUE_DETACH_REQUIRED (usage, kan_render_material_instance_usage_t, usage_id,
                                       &private->available_text_icon_usage_id)
        KAN_UM_ACCESS_DELETE (usage);
    }

    // Move loading usages to available.

    private->available_image_material_instance_usage_id = private->loading_image_material_instance_usage_id;
    private->available_image_atlas_usage_id = private->loading_image_atlas_usage_id;
    private->available_text_sdf_usage_id = private->loading_text_sdf_usage_id;
    private->available_text_icon_usage_id = private->loading_text_icon_usage_id;

    private->loading_image_material_instance_usage_id =
        KAN_TYPED_ID_32_SET_INVALID (kan_render_material_instance_usage_id_t);
    private->loading_image_atlas_usage_id = KAN_TYPED_ID_32_SET_INVALID (kan_render_atlas_usage_id_t);
    private->loading_text_sdf_usage_id = KAN_TYPED_ID_32_SET_INVALID (kan_render_material_instance_usage_id_t);
    private->loading_text_icon_usage_id = KAN_TYPED_ID_32_SET_INVALID (kan_render_material_instance_usage_id_t);

    // Remove usage to resource that is no longer needed.
    KAN_UMI_VALUE_DETACH_REQUIRED (main_usage, kan_resource_usage_t, usage_id, &private->main_usage_id)
    KAN_UM_ACCESS_DELETE (main_usage);
    private->main_usage_id = KAN_TYPED_ID_32_SET_INVALID (kan_resource_usage_id_t);

    KAN_UMO_EVENT_INSERT_INIT (kan_ui_bundle_updated_t) {.stub = 0u};
    KAN_LOG (ui_bundle_management, KAN_LOG_DEBUG, "Advanced bundle \"%s\" state to ready.", public->bundle_name)
}

UNIVERSE_UI_API KAN_UM_MUTATOR_EXECUTE (ui_bundle_management)
{
    KAN_UMI_SINGLETON_READ (resource_provider, kan_resource_provider_singleton_t)
    if (!resource_provider->scan_done)
    {
        return;
    }

    KAN_UMI_SINGLETON_WRITE (public, kan_ui_bundle_singleton_t)
    KAN_UMI_SINGLETON_WRITE (private, ui_bundle_private_singleton_t)
    bool should_reinitialize = public->selection_dirty;

    if (!public->bundle_name && state->default_bundle)
    {
        public->bundle_name = state->default_bundle;
        should_reinitialize = true;
    }

    if (should_reinitialize)
    {
        // The same as resource update.
        on_bundle_resource_updated (state, resource_provider, public, private);
        public->selection_dirty = false;
    }

    KAN_UML_RESOURCE_UPDATED_EVENT_FETCH (updated_event, kan_resource_ui_bundle_t)
    {
        if (!should_reinitialize && updated_event->name == public->bundle_name)
        {
            on_bundle_resource_updated (state, resource_provider, public, private);
        }
    }

    KAN_UML_RESOURCE_LOADED_EVENT_FETCH (bundle_loaded_event, kan_resource_ui_bundle_t)
    {
        if (bundle_loaded_event->name == public->bundle_name &&
            private->state_frame_id != resource_provider->logic_deduplication_frame_id)
        {
            switch (private->state)
            {
            case UI_BUNDLE_LOADING_STATE_INITIAL:
            case UI_BUNDLE_LOADING_STATE_WAITING_RESOURCES:
            case UI_BUNDLE_LOADING_STATE_READY:
                KAN_ASSERT_FORMATTED (false,
                                      "Bundle \"%s\" in state %u received main resource loaded event, which is totally "
                                      "unexpected in this state.",
                                      public->bundle_name, (unsigned int) private->state)
                break;

            case UI_BUNDLE_LOADING_STATE_WAITING_MAIN:
                advance_bundle_from_waiting_main_state (state, resource_provider, public, private);
                break;
            }
        }
    }

    KAN_UML_EVENT_FETCH (pass_updated_event, kan_render_foundation_pass_updated_event_t)
    {
        if (private->state_frame_id != resource_provider->logic_deduplication_frame_id &&
            // Bundle coherence is not broken by separate pass reloading,
            // therefore we just check if we need to advance just in case.
            private->state == UI_BUNDLE_LOADING_STATE_WAITING_RESOURCES)
        {
            advance_bundle_from_waiting_resources_state (state, resource_provider, public, private);
        }
    }

    KAN_UML_EVENT_FETCH (material_instance_updated_event, kan_render_material_instance_updated_event_t)
    {
        if (private->state_frame_id != resource_provider->logic_deduplication_frame_id &&
            // Bundle coherence is not broken by separate material instance reloading,
            // therefore we just check if we need to advance just in case.
            private->state == UI_BUNDLE_LOADING_STATE_WAITING_RESOURCES)
        {
            advance_bundle_from_waiting_resources_state (state, resource_provider, public, private);
        }
    }

    KAN_UML_EVENT_FETCH (atlas_updated_event, kan_render_atlas_updated_event_t)
    {
        if (private->state_frame_id != resource_provider->logic_deduplication_frame_id &&
            // Bundle coherence is not broken by separate atlas reloading,
            // therefore we just check if we need to advance just in case.
            private->state == UI_BUNDLE_LOADING_STATE_WAITING_RESOURCES)
        {
            advance_bundle_from_waiting_resources_state (state, resource_provider, public, private);
        }
    }
}

struct ui_render_graph_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (ui_render_graph)
    KAN_UM_BIND_STATE (ui_render_graph, state)
};

UNIVERSE_UI_API KAN_UM_MUTATOR_DEPLOY (ui_render_graph)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_cpu_static_sections_ensure_initialized ();

    kan_workflow_graph_node_depend_on (workflow_node, KAN_RENDER_FOUNDATION_FRAME_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_UI_LAYOUT_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_UI_BUNDLE_MANAGEMENT_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_UI_TIME_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_UI_RENDER_GRAPH_BEGIN_CHECKPOINT);
    kan_workflow_graph_node_make_dependency_of (workflow_node, KAN_UI_RENDER_GRAPH_END_CHECKPOINT);
}

UNIVERSE_UI_API KAN_UM_MUTATOR_EXECUTE (ui_render_graph)
{
    KAN_UMI_SINGLETON_WRITE (public, kan_ui_render_graph_singleton_t)

    // Clear out previous frame data.
    public->allocation = NULL;
    public->final_pass_instance = KAN_HANDLE_SET_INVALID (kan_render_pass_instance_t);
    public->final_image = KAN_HANDLE_SET_INVALID (kan_render_image_t);

    KAN_UMI_SINGLETON_READ (render_context, kan_render_context_singleton_t)
    if (!render_context->frame_scheduled)
    {
        return;
    }

    KAN_UMI_SINGLETON_READ (bundle, kan_ui_bundle_singleton_t)
    if (!bundle->available)
    {
        return;
    }

    KAN_UMI_SINGLETON_READ (ui_singleton, kan_ui_singleton_t)
    if (ui_singleton->viewport_width <= 0 || ui_singleton->viewport_height <= 0)
    {
        // Cannot allocate and render for zero-sized viewport.
        return;
    }

    KAN_UMI_SINGLETON_READ (render_graph, kan_render_graph_resource_management_singleton_t)
    KAN_UMI_VALUE_READ_REQUIRED (pass, kan_render_foundation_pass_loaded_t, name, &bundle->available_bundle.pass)

    if (pass->attachments.size != 1u)
    {
        KAN_LOG (ui_render_graph, KAN_LOG_ERROR,
                 "Failed to allocate render graph pass as it has unexpected count of attachments: %u.",
                 (unsigned int) pass->attachments.size)
        return;
    }

    const struct kan_render_foundation_pass_attachment_t *attachment =
        &((struct kan_render_foundation_pass_attachment_t *) pass->attachments.data)[0u];

    struct kan_render_graph_resource_image_request_t viewport_image_request = {
        .description =
            {
                .format = attachment->format,
                .width = (kan_instance_size_t) ui_singleton->viewport_width,
                .height = (kan_instance_size_t) ui_singleton->viewport_height,
                .depth = 1u,
                .layers = 1u,
                .mips = 1u,
                .render_target = true,
                .supports_sampling = false,
                .always_treat_as_layered = false,
                .tracking_name = KAN_STATIC_INTERNED_ID_GET (ui_render_target),
            },
        .internal = true,
    };

    struct kan_render_graph_resource_frame_buffer_request_t frame_buffer_request = {
        .pass = pass->pass,
        .attachments_count = 1u,
        .attachments =
            (struct kan_render_graph_resource_frame_buffer_request_attachment_t[]) {
                {
                    .image_index = 0u,
                    .image_layer = 0u,
                },
            },
    };

    struct kan_render_graph_resource_request_t graph_request = {
        .context = render_context->render_context,
        .dependant_count = 0u,
        .dependant = NULL,
        .images_count = 1u,
        .images = &viewport_image_request,
        .frame_buffers_count = 1u,
        .frame_buffers = &frame_buffer_request,
    };

    public->allocation = kan_render_graph_resource_management_singleton_request (render_graph, &graph_request);
    if (!public->allocation)
    {
        KAN_LOG (ui_render_graph, KAN_LOG_ERROR,
                 "Failed to allocate render graph pass: allocation declined by render graph resource manager.")
        return;
    }

    public->final_image = public->allocation->images[0u];
    struct kan_render_viewport_bounds_t viewport_bounds = {
        .x = 0.0f,
        .y = 0.0f,
        .width = (float) ui_singleton->viewport_width,
        .height = (float) ui_singleton->viewport_height,
        .depth_min = 0.0f,
        .depth_max = 1.0f,
    };

    struct kan_render_integer_region_2d_t scissor = {
        .x = 0,
        .y = 0,
        .width = (kan_instance_size_t) ui_singleton->viewport_width,
        .height = (kan_instance_size_t) ui_singleton->viewport_height,
    };

    struct kan_render_clear_value_t clear_values[] = {
        {
            .color =
                {
                    public->clear_color.r,
                    public->clear_color.g,
                    public->clear_color.b,
                    public->clear_color.a,
                },
        },
    };

    public->final_pass_instance = kan_render_pass_instantiate (pass->pass, public->allocation->frame_buffers[0u],
                                                               &viewport_bounds, &scissor, clear_values);
}

KAN_REFLECTION_IGNORE
struct ui_pass_view_data_t
{
    struct kan_float_matrix_4x4_t projection_view;
};

struct ui_render_private_singleton_t
{
    kan_render_pipeline_parameter_set_t pass_parameter_set;
    kan_render_buffer_t pass_view_data_buffer;

    kan_render_frame_lifetime_buffer_allocator_t frame_lifetime_allocator;
    kan_instance_size_t previous_frame_instanced_images;

    kan_render_buffer_t ui_rect_vertices;
    kan_render_buffer_t ui_rect_indices;

    kan_interned_string_t used_pass_name;
    kan_render_image_t bound_glyph_sdf_atlas;

    kan_instance_size_t binding_pass_view_data;
    kan_instance_size_t binding_image_sampler;
    kan_instance_size_t binding_image_atlas;
    kan_instance_size_t binding_image_entries;
    kan_instance_size_t binding_glyph_sampler;
    kan_instance_size_t binding_glyph_sdf_atlas;
    kan_instance_size_t binding_color_table;
};

UNIVERSE_UI_API void ui_render_private_singleton_init (struct ui_render_private_singleton_t *instance)
{
    instance->pass_parameter_set = KAN_HANDLE_SET_INVALID (kan_render_pipeline_parameter_set_t);
    instance->pass_view_data_buffer = KAN_HANDLE_SET_INVALID (kan_render_buffer_t);

    instance->frame_lifetime_allocator = KAN_HANDLE_SET_INVALID (kan_render_frame_lifetime_buffer_allocator_t);
    instance->previous_frame_instanced_images = 0u;

    instance->ui_rect_vertices = KAN_HANDLE_SET_INVALID (kan_render_buffer_t);
    instance->ui_rect_indices = KAN_HANDLE_SET_INVALID (kan_render_buffer_t);

    instance->used_pass_name = NULL;
    instance->bound_glyph_sdf_atlas = KAN_HANDLE_SET_INVALID (kan_render_image_t);

    instance->binding_pass_view_data = 0u;
    instance->binding_image_sampler = 0u;
    instance->binding_image_atlas = 0u;
    instance->binding_image_entries = 0u;
    instance->binding_glyph_sampler = 0u;
    instance->binding_glyph_sdf_atlas = 0u;
    instance->binding_color_table = 0u;
}

UNIVERSE_UI_API void ui_render_private_singleton_shutdown (struct ui_render_private_singleton_t *instance)
{
    if (KAN_HANDLE_IS_VALID (instance->pass_parameter_set))
    {
        kan_render_pipeline_parameter_set_destroy (instance->pass_parameter_set);
    }

    if (KAN_HANDLE_IS_VALID (instance->pass_view_data_buffer))
    {
        kan_render_buffer_destroy (instance->pass_view_data_buffer);
    }

    if (KAN_HANDLE_IS_VALID (instance->frame_lifetime_allocator))
    {
        kan_render_frame_lifetime_buffer_allocator_destroy (instance->frame_lifetime_allocator);
    }

    if (KAN_HANDLE_IS_VALID (instance->ui_rect_vertices))
    {
        kan_render_buffer_destroy (instance->ui_rect_vertices);
    }

    if (KAN_HANDLE_IS_VALID (instance->ui_rect_indices))
    {
        kan_render_buffer_destroy (instance->ui_rect_indices);
    }
}

KAN_REFLECTION_IGNORE
enum ui_bound_pipeline_t
{
    UI_BOUND_PIPELINE_NONE = 0u,
    UI_BOUND_PIPELINE_IMAGE,
    UI_BOUND_PIPELINE_TEXT_SDF,
    UI_BOUND_PIPELINE_TEXT_ICON,
};

KAN_REFLECTION_IGNORE
struct image_instanced_data_t
{
    struct kan_float_vector_2_t offset;
    struct kan_float_vector_2_t size;
    uint32_t image_index;
    uint32_t ui_mark;
    float local_time;
};

KAN_REFLECTION_IGNORE
struct ui_render_transient_state_t
{
    const struct kan_ui_singleton_t *ui;
    struct ui_render_private_singleton_t *private;
    const struct kan_ui_render_graph_singleton_t *ui_render_graph;

    enum ui_bound_pipeline_t bound_pipeline;
    struct kan_ui_clip_rect_t clip_rect;

    struct image_instanced_data_t *image_bulk_run_begin;
    struct image_instanced_data_t *image_bulk_next;
    struct image_instanced_data_t *image_bulk_end;

    struct kan_render_allocated_slice_t image_bulk_allocation;
    struct image_instanced_data_t *image_bulk_slice_begin;

    kan_instance_size_t this_frame_instanced_images;

    kan_render_graphics_pipeline_t image_pipeline;
    kan_render_pipeline_parameter_set_t image_pipeline_material_parameters;

    kan_render_graphics_pipeline_t text_sdf_pipeline;
    kan_render_pipeline_parameter_set_t text_sdf_pipeline_material_parameters;

    kan_render_graphics_pipeline_t text_icon_pipeline;
    kan_render_pipeline_parameter_set_t text_icon_pipeline_material_parameters;
};

struct ui_render_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (ui_render)
    KAN_UM_BIND_STATE (ui_render, state)

    KAN_REFLECTION_IGNORE
    struct ui_render_transient_state_t transient;
};

UNIVERSE_UI_API KAN_UM_MUTATOR_DEPLOY (ui_render)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_cpu_static_sections_ensure_initialized ();

    kan_workflow_graph_node_depend_on (workflow_node, KAN_RENDER_FOUNDATION_FRAME_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_TEXT_SHAPING_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_UI_LAYOUT_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_UI_RENDER_GRAPH_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_UI_RENDER_BEGIN_CHECKPOINT);
    kan_workflow_graph_node_make_dependency_of (workflow_node, KAN_UI_RENDER_END_CHECKPOINT);
}

static const struct kan_float_vector_2_t ui_rect_vertices[] = {
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f},
};

static const uint16_t ui_rect_indices[] = {
    0u, 1u, 2u, 2u, 3u, 0u,
};

static void ensure_ui_rect_ready (struct ui_render_private_singleton_t *private,
                                  const struct kan_render_context_singleton_t *render_context)
{
    if (!KAN_HANDLE_IS_VALID (private->ui_rect_vertices))
    {
        private->ui_rect_vertices = kan_render_buffer_create (
            render_context->render_context, KAN_RENDER_BUFFER_TYPE_ATTRIBUTE, sizeof (ui_rect_vertices),
            ui_rect_vertices, kan_string_intern ("ui_rect_vertices"));
    }

    if (!KAN_HANDLE_IS_VALID (private->ui_rect_indices))
    {
        private->ui_rect_indices =
            kan_render_buffer_create (render_context->render_context, KAN_RENDER_BUFFER_TYPE_INDEX_16,
                                      sizeof (ui_rect_indices), ui_rect_indices, kan_string_intern ("ui_rect_indices"));
    }
}

static bool ensure_pass_parameter_set_ready (struct ui_render_state_t *state,
                                             struct ui_render_private_singleton_t *private,
                                             const struct kan_ui_bundle_singleton_t *bundle,
                                             const struct kan_text_shaping_singleton_t *text_shaping,
                                             const struct kan_render_context_singleton_t *render_context)
{
    if (!KAN_HANDLE_IS_VALID (private->pass_parameter_set))
    {
        KAN_UMI_VALUE_READ_REQUIRED (pass_loaded, kan_render_foundation_pass_loaded_t, name,
                                     &bundle->available_bundle.pass)
        KAN_UMI_VALUE_READ_REQUIRED (atlas_loaded, kan_render_atlas_loaded_t, name,
                                     &bundle->available_bundle.image_atlas)

        private->binding_pass_view_data = KAN_INT_MAX (kan_instance_size_t);
        private->binding_image_sampler = KAN_INT_MAX (kan_instance_size_t);
        private->binding_image_atlas = KAN_INT_MAX (kan_instance_size_t);
        private->binding_image_entries = KAN_INT_MAX (kan_instance_size_t);
        private->binding_glyph_sampler = KAN_INT_MAX (kan_instance_size_t);
        private->binding_glyph_sdf_atlas = KAN_INT_MAX (kan_instance_size_t);
        private->binding_color_table = KAN_INT_MAX (kan_instance_size_t);

        if (pass_loaded->variants.size > 1u)
        {
            KAN_LOG (ui_render, KAN_LOG_ERROR, "Expected only 1 variant in pass \"%s\", but got %u.", pass_loaded->name,
                     (unsigned int) pass_loaded->variants.size)
            return false;
        }

        const struct kan_render_foundation_pass_variant_t *variant =
            &((struct kan_render_foundation_pass_variant_t *) pass_loaded->variants.data)[0u];

        for (kan_loop_size_t index = 0; index < variant->pass_parameter_set_bindings.buffers.size; ++index)
        {
            const struct kan_rpl_meta_buffer_t *buffer =
                &((struct kan_rpl_meta_buffer_t *) variant->pass_parameter_set_bindings.buffers.data)[index];

            if (buffer->name == KAN_STATIC_INTERNED_ID_GET (pass_view_data))
            {
                private->binding_pass_view_data = buffer->binding;
                if (!KAN_HANDLE_IS_VALID (private->pass_view_data_buffer) ||
                    kan_render_buffer_get_full_size (private->pass_view_data_buffer) != buffer->main_size)
                {
                    if (KAN_HANDLE_IS_VALID (private->pass_view_data_buffer))
                    {
                        kan_render_buffer_destroy (private->pass_view_data_buffer);
                    }

                    private->pass_view_data_buffer = kan_render_buffer_create (
                        render_context->render_context, KAN_RENDER_BUFFER_TYPE_UNIFORM, buffer->main_size, NULL,
                        KAN_STATIC_INTERNED_ID_GET (ui_pass_view_data));
                }
            }
            else if (buffer->name == KAN_STATIC_INTERNED_ID_GET (image_entries))
            {
                private->binding_image_entries = buffer->binding;
            }
            else if (buffer->name == KAN_STATIC_INTERNED_ID_GET (color_table))
            {
                private->binding_color_table = buffer->binding;
            }
        }

        for (kan_loop_size_t index = 0; index < variant->pass_parameter_set_bindings.samplers.size; ++index)
        {
            const struct kan_rpl_meta_sampler_t *sampler =
                &((struct kan_rpl_meta_sampler_t *) variant->pass_parameter_set_bindings.samplers.data)[index];

            if (sampler->name == KAN_STATIC_INTERNED_ID_GET (image_sampler))
            {
                private->binding_image_sampler = sampler->binding;
            }
            else if (sampler->name == KAN_STATIC_INTERNED_ID_GET (glyph_sampler))
            {
                private->binding_glyph_sampler = sampler->binding;
            }
        }

        for (kan_loop_size_t index = 0; index < variant->pass_parameter_set_bindings.images.size; ++index)
        {
            const struct kan_rpl_meta_image_t *image =
                &((struct kan_rpl_meta_image_t *) variant->pass_parameter_set_bindings.images.data)[index];

            if (image->name == KAN_STATIC_INTERNED_ID_GET (image_atlas))
            {
                private->binding_image_atlas = image->binding;
            }
            else if (image->name == KAN_STATIC_INTERNED_ID_GET (glyph_sdf_atlas))
            {
                private->binding_glyph_sdf_atlas = image->binding;
            }
        }

        if (private->binding_pass_view_data == KAN_INT_MAX (kan_instance_size_t) ||
            private->binding_image_sampler == KAN_INT_MAX (kan_instance_size_t) ||
            private->binding_image_atlas == KAN_INT_MAX (kan_instance_size_t) ||
            private->binding_image_entries == KAN_INT_MAX (kan_instance_size_t) ||
            private->binding_glyph_sampler == KAN_INT_MAX (kan_instance_size_t) ||
            private->binding_glyph_sdf_atlas == KAN_INT_MAX (kan_instance_size_t) ||
            private->binding_color_table == KAN_INT_MAX (kan_instance_size_t))
        {
            KAN_LOG (ui_render, KAN_LOG_ERROR, "Failed to find expected bindings in pass \"%s\".", pass_loaded->name)
            return false;
        }

        kan_instance_size_t image_atlas_layers;
        kan_render_image_get_sizes (atlas_loaded->image, NULL, NULL, NULL, &image_atlas_layers);

        private->bound_glyph_sdf_atlas = text_shaping->font_library_sdf_atlas;
        kan_instance_size_t glyph_atlas_layers = 0u;

        if (KAN_HANDLE_IS_VALID (private->bound_glyph_sdf_atlas))
        {
            kan_render_image_get_sizes (private->bound_glyph_sdf_atlas, NULL, NULL, NULL, &glyph_atlas_layers);
        }

        struct kan_render_parameter_update_description_t updates[] = {
            {
                .binding = private->binding_pass_view_data,
                .buffer_binding =
                    {
                        .buffer = private->pass_view_data_buffer,
                        .offset = 0u,
                        .range = kan_render_buffer_get_full_size (private->pass_view_data_buffer),
                    },
            },
            {
                .binding = private->binding_image_sampler,
                .sampler_binding =
                    {
                        .sampler =
                            {
                                .mag_filter = KAN_RENDER_FILTER_MODE_LINEAR,
                                .min_filter = KAN_RENDER_FILTER_MODE_LINEAR,
                                .mip_map_mode = KAN_RENDER_MIP_MAP_MODE_NEAREST,
                                .address_mode_u = KAN_RENDER_ADDRESS_MODE_REPEAT,
                                .address_mode_v = KAN_RENDER_ADDRESS_MODE_REPEAT,
                                .address_mode_w = KAN_RENDER_ADDRESS_MODE_REPEAT,
                            },
                    },
            },
            {
                .binding = private->binding_image_atlas,
                .image_binding =
                    {
                        .image = atlas_loaded->image,
                        .array_index = 0u,
                        .layer_offset = 0u,
                        .layer_count = image_atlas_layers,
                    },
            },
            {
                .binding = private->binding_image_entries,
                .buffer_binding =
                    {
                        .buffer = atlas_loaded->entry_buffer,
                        .offset = 0u,
                        .range = kan_render_buffer_get_full_size (atlas_loaded->entry_buffer),
                    },
            },
            {
                .binding = private->binding_glyph_sampler,
                .sampler_binding =
                    {
                        .sampler =
                            {
                                .mag_filter = KAN_RENDER_FILTER_MODE_LINEAR,
                                .min_filter = KAN_RENDER_FILTER_MODE_LINEAR,
                                .mip_map_mode = KAN_RENDER_MIP_MAP_MODE_NEAREST,
                                .address_mode_u = KAN_RENDER_ADDRESS_MODE_CLAMP_TO_EDGE,
                                .address_mode_v = KAN_RENDER_ADDRESS_MODE_CLAMP_TO_EDGE,
                                .address_mode_w = KAN_RENDER_ADDRESS_MODE_CLAMP_TO_EDGE,
                            },
                    },
            },
            {
                .binding = private->binding_color_table,
                .buffer_binding =
                    {
                        .buffer = render_context->color_table_buffer,
                        .offset = 0u,
                        .range = kan_render_buffer_get_full_size (render_context->color_table_buffer),
                    },
            },
            {
                .binding = private->binding_glyph_sdf_atlas,
                .image_binding =
                    {
                        .image = private->bound_glyph_sdf_atlas,
                        .array_index = 0u,
                        .layer_offset = 0u,
                        .layer_count = glyph_atlas_layers,
                    },
            },
        };

        kan_instance_size_t updates_count = sizeof (updates) / sizeof (updates[0u]);
        if (!KAN_HANDLE_IS_VALID (private->bound_glyph_sdf_atlas))
        {
            // Skip last binding as it would be invalid.
            --updates_count;
        }

        struct kan_render_pipeline_parameter_set_description_t set_description = {
            .layout = variant->pass_parameter_set_layout,
            .stable_binding = true,
            .tracking_name = KAN_STATIC_INTERNED_ID_GET (ui_pass_set),
            .initial_bindings_count = updates_count,
            .initial_bindings = updates,
        };

        private->pass_parameter_set =
            kan_render_pipeline_parameter_set_create (render_context->render_context, &set_description);

        if (!KAN_HANDLE_IS_VALID (private->pass_parameter_set))
        {
            KAN_LOG (ui_render, KAN_LOG_ERROR, "Failed to allocate pass parameter set for pass \"%s\".",
                     pass_loaded->name)
            return false;
        }
    }

    return true;
}

#define UI_RECT_ATTRIBUTE_BINDING 0u
#define UI_INSTANCED_BINDING 1u

static bool cache_material_instance_data (struct ui_render_state_t *state,
                                          kan_interned_string_t pass_name,
                                          kan_interned_string_t material_instance_name,
                                          kan_render_graphics_pipeline_t *output_pipeline,
                                          kan_render_pipeline_parameter_set_t *output_parameters)
{
    KAN_UMI_VALUE_READ_REQUIRED (material_instance, kan_render_material_instance_loaded_t, name,
                                 &material_instance_name)

    *output_parameters = material_instance->parameter_set;
    KAN_UMI_VALUE_READ_REQUIRED (material, kan_render_material_loaded_t, name, &material_instance->material_name)

    for (kan_loop_size_t index = 0u; index < material->pipelines.size; ++index)
    {
        const struct kan_render_material_pipeline_t *pipeline =
            &((struct kan_render_material_pipeline_t *) material->pipelines.data)[index];

        if (pipeline->pass_name == pass_name && pipeline->variant_name == KAN_STATIC_INTERNED_ID_GET (default))
        {
            *output_pipeline = pipeline->pipeline;
            if (material->vertex_attribute_sources.size != 1u)
            {
                KAN_LOG (ui_render, KAN_LOG_ERROR,
                         "Material \"%s\" should have only one vertex attribute source in order to be usable with ui.",
                         material->name)
                return false;
            }

            const struct kan_rpl_meta_attribute_source_t *attribute_source =
                &((struct kan_rpl_meta_attribute_source_t *) material->vertex_attribute_sources.data)[0u];

            if (attribute_source->binding != UI_RECT_ATTRIBUTE_BINDING)
            {
                KAN_LOG (ui_render, KAN_LOG_ERROR,
                         "Material \"%s\" vertex attribute source for ui rect must be bound at %u, but has binding "
                         "index %u.",
                         material->name, (unsigned int) UI_RECT_ATTRIBUTE_BINDING,
                         (unsigned int) attribute_source->binding)
                return false;
            }

            if (!material->has_instanced_attribute_source)
            {
                KAN_LOG (ui_render, KAN_LOG_ERROR,
                         "Material \"%s\" should have instanced attribute source as it is used as general ui material "
                         "(either image, text sdf glyph or text icon material).",
                         material->name)
                return false;
            }

            if (material->instanced_attribute_source.binding != UI_INSTANCED_BINDING)
            {
                KAN_LOG (
                    ui_render, KAN_LOG_ERROR,
                    "Material \"%s\" instanced attribute source for general instanced data must be bound at %u, "
                    "but has binding index %u. Material is used for either image, text sdf glyph or text icon render.",
                    material->name, (unsigned int) UI_INSTANCED_BINDING,
                    (unsigned int) material->instanced_attribute_source.binding)
                return false;
            }

            return true;
        }
    }

    KAN_LOG (ui_render, KAN_LOG_ERROR,
             "Failed to find pipeline for pass \"%s\" and variant \"default\" in material \"%s\".", pass_name,
             material->name)
    return false;
}

static void flush_instanced_ui_images (struct ui_render_state_t *state)
{
    if (!state->transient.image_bulk_run_begin)
    {
        return;
    }

    const kan_instance_size_t instance_count = state->transient.image_bulk_next - state->transient.image_bulk_run_begin;
    kan_render_pass_instance_t pass_instance = state->transient.ui_render_graph->final_pass_instance;

    if (state->transient.bound_pipeline != UI_BOUND_PIPELINE_IMAGE &&
        kan_render_pass_instance_graphics_pipeline (pass_instance, state->transient.image_pipeline))
    {
        kan_render_pipeline_parameter_set_t sets[] = {
            state->transient.private->pass_parameter_set,
            state->transient.image_pipeline_material_parameters,
        };

        kan_render_pass_instance_pipeline_parameter_sets (pass_instance, KAN_RPL_SET_PASS,
                                                          sizeof (sets) / sizeof (sets[0u]), sets);

        kan_render_pass_instance_indices (pass_instance, state->transient.private->ui_rect_indices);
        kan_render_pass_instance_attributes (pass_instance, UI_RECT_ATTRIBUTE_BINDING, 1u,
                                             &state->transient.private->ui_rect_vertices, NULL);

        state->transient.bound_pipeline = UI_BOUND_PIPELINE_IMAGE;
    }

    // Check again as bind might have failed.
    if (state->transient.bound_pipeline == UI_BOUND_PIPELINE_IMAGE)
    {
        const kan_instance_size_t begin_index =
            state->transient.image_bulk_run_begin - state->transient.image_bulk_slice_begin;

        const kan_instance_size_t full_offset =
            state->transient.image_bulk_allocation.slice_offset + begin_index * sizeof (struct image_instanced_data_t);

        kan_render_pass_instance_attributes (pass_instance, UI_INSTANCED_BINDING, 1u,
                                             &state->transient.image_bulk_allocation.buffer, &full_offset);

        kan_render_pass_instance_draw (pass_instance, 0u, sizeof (ui_rect_indices) / sizeof (ui_rect_indices[0u]), 0u,
                                       0u, instance_count);
    }

    state->transient.image_bulk_run_begin = NULL;
    state->transient.this_frame_instanced_images += instance_count;
}

static inline void apply_clip_rect (struct ui_render_state_t *state, const struct kan_ui_clip_rect_t *clip_rect)
{
    if (clip_rect->x != state->transient.clip_rect.x || clip_rect->y != state->transient.clip_rect.y ||
        clip_rect->width != state->transient.clip_rect.width || clip_rect->height != state->transient.clip_rect.height)
    {
        flush_instanced_ui_images (state);
        state->transient.clip_rect = *clip_rect;

        struct kan_render_integer_region_2d_t scissor = {
            .x = clip_rect->x,
            .y = clip_rect->y,
            .width = (kan_instance_size_t) clip_rect->width,
            .height = (kan_instance_size_t) clip_rect->height,
        };

        kan_render_pass_instance_override_scissor (state->transient.ui_render_graph->final_pass_instance, &scissor);
    }
}

static void allocate_new_image_bulk_region (struct ui_render_state_t *state)
{
    kan_instance_size_t allocation_size = 0u;
    if (state->transient.private->previous_frame_instanced_images == 0u)
    {
        // Unknown amount of images, allocate 25% of a page.
        allocation_size =
            kan_apply_alignment (KAN_UNIVERSE_UI_RENDER_INSTANCED_PAGE / 4u, alignof (struct image_instanced_data_t));
    }
    else
    {
        static_assert (KAN_UNIVERSE_UI_RENDER_INSTANCED_PAGE % alignof (struct image_instanced_data_t) == 0u,
                       "Page size is aligned to image data.");

        allocation_size =
            state->transient.private->previous_frame_instanced_images * sizeof (struct image_instanced_data_t);
        allocation_size = KAN_MIN (KAN_UNIVERSE_UI_RENDER_INSTANCED_PAGE, allocation_size);
    }

    state->transient.image_bulk_allocation = kan_render_frame_lifetime_buffer_allocator_allocate (
        state->transient.private->frame_lifetime_allocator, allocation_size, alignof (struct image_instanced_data_t));

    if (!KAN_HANDLE_IS_VALID (state->transient.image_bulk_allocation.buffer))
    {
        kan_error_critical ("Failed to allocate bulk image instanced data, critical error.", __FILE__, __LINE__);
    }

    state->transient.image_bulk_slice_begin =
        kan_render_buffer_patch (state->transient.image_bulk_allocation.buffer,
                                 state->transient.image_bulk_allocation.slice_offset, allocation_size);

    state->transient.image_bulk_run_begin = NULL;
    state->transient.image_bulk_next = state->transient.image_bulk_slice_begin;
    state->transient.image_bulk_end =
        state->transient.image_bulk_slice_begin + allocation_size / sizeof (struct image_instanced_data_t);
}

KAN_REFLECTION_IGNORE
struct text_push_constant_layout_t
{
    struct kan_float_vector_2_t offset;
    float local_time;
    float unused_padding_1;

    uint32_t ui_mark;
    uint32_t unused_padding_2[3u];
};

static void execute_draw_text_command (struct ui_render_state_t *state,
                                       const struct kan_ui_node_drawable_t *drawable,
                                       const struct kan_ui_draw_command_data_t *command)
{
    KAN_UMI_VALUE_READ_OPTIONAL (shaping_unit, kan_text_shaping_unit_t, id, &command->text.shaping_unit)
    if (!shaping_unit || !shaping_unit->shaped)
    {
        return;
    }

    kan_render_pass_instance_t pass_instance = state->transient.ui_render_graph->final_pass_instance;
    const kan_instance_size_t glyph_count = shaping_unit->shaped_as_stable ? shaping_unit->shaped_stable.glyphs_count :
                                                                             shaping_unit->shaped_unstable.glyphs.size;

    const kan_instance_size_t icon_count = shaping_unit->shaped_as_stable ? shaping_unit->shaped_stable.icons_count :
                                                                            shaping_unit->shaped_unstable.icons.size;

    struct text_push_constant_layout_t push_constant;
    push_constant.offset.x = (float) (drawable->global_x + drawable->draw_offset_x);
    push_constant.offset.y = (float) (drawable->global_y + drawable->draw_offset_y);

    switch (shaping_unit->request.alignment)
    {
    case KAN_TEXT_SHAPING_ALIGNMENT_LEFT:
        break;

    case KAN_TEXT_SHAPING_ALIGNMENT_CENTER:
        push_constant.offset.x += 0.5f * (float) drawable->width - 0.5f * (float) shaping_unit->shaped_primary_size;
        break;

    case KAN_TEXT_SHAPING_ALIGNMENT_RIGHT:
        push_constant.offset.x += (float) drawable->width - (float) shaping_unit->shaped_primary_size;
        break;
    }

    push_constant.local_time = state->transient.ui->animation_global_time_s - command->animation_start_time_s;
    push_constant.ui_mark = command->ui_mark;

    while (push_constant.local_time < 0.0f)
    {
        push_constant.local_time += state->transient.ui->animation_global_time_loop_s;
    }

    if (glyph_count > 0u)
    {
        if (state->transient.bound_pipeline != UI_BOUND_PIPELINE_TEXT_SDF &&
            kan_render_pass_instance_graphics_pipeline (pass_instance, state->transient.text_sdf_pipeline))
        {
            kan_render_pipeline_parameter_set_t sets[] = {
                state->transient.private->pass_parameter_set,
                state->transient.text_sdf_pipeline_material_parameters,
            };

            kan_render_pass_instance_pipeline_parameter_sets (pass_instance, KAN_RPL_SET_PASS,
                                                              sizeof (sets) / sizeof (sets[0u]), sets);

            kan_render_pass_instance_indices (pass_instance, state->transient.private->ui_rect_indices);
            kan_render_pass_instance_attributes (pass_instance, UI_RECT_ATTRIBUTE_BINDING, 1u,
                                                 &state->transient.private->ui_rect_vertices, NULL);

            state->transient.bound_pipeline = UI_BOUND_PIPELINE_TEXT_SDF;
        }

        // Check again as bind might have failed.
        if (state->transient.bound_pipeline == UI_BOUND_PIPELINE_TEXT_SDF)
        {
            kan_render_pass_instance_push_constant (pass_instance, &push_constant);
            if (shaping_unit->shaped_as_stable)
            {
                // Otherwise might technically trigger "discards const" error.
                kan_render_buffer_t attributes[] = {shaping_unit->shaped_stable.glyphs};
                kan_render_pass_instance_attributes (pass_instance, UI_INSTANCED_BINDING, 1u, attributes, NULL);

                kan_render_pass_instance_draw (
                    pass_instance, 0u, sizeof (ui_rect_indices) / sizeof (ui_rect_indices[0u]), 0u, 0u, glyph_count);
            }
            else
            {
                // Handle page overflows for very large texts.
                // Should generally never happen in reality as huge texts never be unstable.
                kan_instance_size_t glyphs_rendered = 0u;
                const kan_instance_size_t max_to_render_in_one_call =
                    KAN_UNIVERSE_UI_RENDER_INSTANCED_PAGE / sizeof (struct kan_text_shaped_glyph_instance_data_t);

                while (glyphs_rendered < glyph_count)
                {
                    const kan_instance_size_t to_render =
                        KAN_MIN (glyph_count - glyphs_rendered, max_to_render_in_one_call);

                    struct kan_render_allocated_slice_t slice = kan_render_frame_lifetime_buffer_allocator_allocate (
                        state->transient.private->frame_lifetime_allocator,
                        sizeof (struct kan_text_shaped_glyph_instance_data_t) * to_render,
                        alignof (struct kan_text_shaped_glyph_instance_data_t));

                    void *text_instanced_memory =
                        kan_render_buffer_patch (slice.buffer, slice.slice_offset,
                                                 sizeof (struct kan_text_shaped_glyph_instance_data_t) * to_render);

                    KAN_ASSERT (text_instanced_memory)
                    memcpy (text_instanced_memory,
                            shaping_unit->shaped_unstable.glyphs.data +
                                sizeof (struct kan_text_shaped_glyph_instance_data_t) * glyphs_rendered,
                            sizeof (struct kan_text_shaped_glyph_instance_data_t) * to_render);

                    kan_render_pass_instance_attributes (pass_instance, UI_INSTANCED_BINDING, 1u, &slice.buffer,
                                                         &slice.slice_offset);
                    glyphs_rendered += to_render;
                }
            }
        }
    }

    if (icon_count > 0u)
    {
        if (state->transient.bound_pipeline != UI_BOUND_PIPELINE_TEXT_ICON &&
            kan_render_pass_instance_graphics_pipeline (pass_instance, state->transient.text_icon_pipeline))
        {
            kan_render_pipeline_parameter_set_t sets[] = {
                state->transient.private->pass_parameter_set,
                state->transient.text_icon_pipeline_material_parameters,
            };

            kan_render_pass_instance_pipeline_parameter_sets (pass_instance, KAN_RPL_SET_PASS,
                                                              sizeof (sets) / sizeof (sets[0u]), sets);

            kan_render_pass_instance_indices (pass_instance, state->transient.private->ui_rect_indices);
            kan_render_pass_instance_attributes (pass_instance, UI_RECT_ATTRIBUTE_BINDING, 1u,
                                                 &state->transient.private->ui_rect_vertices, NULL);

            state->transient.bound_pipeline = UI_BOUND_PIPELINE_TEXT_ICON;
        }

        // Check again as bind might have failed.
        if (state->transient.bound_pipeline == UI_BOUND_PIPELINE_TEXT_ICON)
        {
            kan_render_pass_instance_push_constant (pass_instance, &push_constant);
            if (shaping_unit->shaped_as_stable)
            {
                // Otherwise might technically trigger "discards const" error.
                kan_render_buffer_t attributes[] = {shaping_unit->shaped_stable.icons};
                kan_render_pass_instance_attributes (pass_instance, UI_INSTANCED_BINDING, 1u, attributes, NULL);

                kan_render_pass_instance_draw (
                    pass_instance, 0u, sizeof (ui_rect_indices) / sizeof (ui_rect_indices[0u]), 0u, 0u, icon_count);
            }
            else
            {
                // Handle page overflows for very large texts.
                // Should generally never happen in reality as huge texts never be unstable.
                kan_instance_size_t icons_rendered = 0u;
                const kan_instance_size_t max_to_render_in_one_call =
                    KAN_UNIVERSE_UI_RENDER_INSTANCED_PAGE / sizeof (struct kan_text_shaped_icon_instance_data_t);

                while (icons_rendered < icon_count)
                {
                    const kan_instance_size_t to_render =
                        KAN_MIN (icon_count - icons_rendered, max_to_render_in_one_call);

                    struct kan_render_allocated_slice_t slice = kan_render_frame_lifetime_buffer_allocator_allocate (
                        state->transient.private->frame_lifetime_allocator,
                        sizeof (struct kan_text_shaped_icon_instance_data_t) * to_render,
                        alignof (struct kan_text_shaped_icon_instance_data_t));

                    void *text_instanced_memory =
                        kan_render_buffer_patch (slice.buffer, slice.slice_offset,
                                                 sizeof (struct kan_text_shaped_icon_instance_data_t) * to_render);

                    KAN_ASSERT (text_instanced_memory)
                    memcpy (text_instanced_memory,
                            shaping_unit->shaped_unstable.icons.data +
                                sizeof (struct kan_text_shaped_icon_instance_data_t) * icons_rendered,
                            sizeof (struct kan_text_shaped_icon_instance_data_t) * to_render);

                    kan_render_pass_instance_attributes (pass_instance, UI_INSTANCED_BINDING, 1u, &slice.buffer,
                                                         &slice.slice_offset);
                    icons_rendered += to_render;
                }
            }
        }
    }
}

static void execute_draw_custom_command (struct ui_render_state_t *state,
                                         const struct kan_ui_node_drawable_t *drawable,
                                         const struct kan_ui_draw_command_data_t *command)
{
    KAN_UMI_VALUE_READ_OPTIONAL (material_instance, kan_render_material_instance_loaded_t, name,
                                 &command->custom.material_instance_name)

    if (!material_instance)
    {
        return;
    }

    KAN_UMI_VALUE_READ_REQUIRED (material, kan_render_material_loaded_t, name, &material_instance->material_name)
    kan_render_graphics_pipeline_t selected_pipeline = KAN_HANDLE_SET_INVALID (kan_render_graphics_pipeline_t);

    for (kan_loop_size_t index = 0u; index < material->pipelines.size; ++index)
    {
        const struct kan_render_material_pipeline_t *pipeline =
            &((struct kan_render_material_pipeline_t *) material->pipelines.data)[index];

        if (pipeline->pass_name == state->transient.private->used_pass_name &&
            pipeline->variant_name == KAN_STATIC_INTERNED_ID_GET (default))
        {
            selected_pipeline = pipeline->pipeline;
            if (material->vertex_attribute_sources.size != 1u)
            {
                KAN_LOG (ui_render, KAN_LOG_ERROR,
                         "Material \"%s\" should have only one vertex attribute source in order to be usable with ui "
                         "as custom material.",
                         material->name)
                return;
            }

            const struct kan_rpl_meta_attribute_source_t *attribute_source =
                &((struct kan_rpl_meta_attribute_source_t *) material->vertex_attribute_sources.data)[0u];

            if (attribute_source->binding != UI_RECT_ATTRIBUTE_BINDING)
            {
                KAN_LOG (ui_render, KAN_LOG_ERROR,
                         "Material \"%s\" vertex attribute source for ui rect must be bound at %u, but has binding "
                         "index %u.",
                         material->name, (unsigned int) UI_RECT_ATTRIBUTE_BINDING,
                         (unsigned int) attribute_source->binding)
                return;
            }

            if (material->has_instanced_attribute_source)
            {
                KAN_LOG (ui_render, KAN_LOG_ERROR,
                         "Material \"%s\" should not have instanced attribute source in order to be usable in ui as "
                         "custom draw material.",
                         material->name)
                return;
            }

            break;
        }
    }

    if (!KAN_HANDLE_IS_VALID (selected_pipeline))
    {
        KAN_LOG (ui_render, KAN_LOG_ERROR,
                 "Failed to find pipeline for pass \"%s\" and variant \"default\" in material \"%s\".",
                 state->transient.private->used_pass_name, material->name)
    }

    kan_render_pass_instance_t pass_instance = state->transient.ui_render_graph->final_pass_instance;
    if (kan_render_pass_instance_graphics_pipeline (pass_instance, selected_pipeline))
    {
        kan_render_pipeline_parameter_set_t sets[] = {
            state->transient.private->pass_parameter_set,
            state->transient.image_pipeline_material_parameters,
            command->custom.object_set,
            command->custom.shared_set,
        };

        kan_render_pass_instance_pipeline_parameter_sets (pass_instance, KAN_RPL_SET_PASS,
                                                          sizeof (sets) / sizeof (sets[0u]), sets);

        kan_render_pass_instance_indices (pass_instance, state->transient.private->ui_rect_indices);
        kan_render_pass_instance_attributes (pass_instance, UI_RECT_ATTRIBUTE_BINDING, 1u,
                                             &state->transient.private->ui_rect_vertices, NULL);

        struct kan_ui_draw_command_custom_push_layout_t push;
        push.size.x = (float) (drawable->global_x + drawable->draw_offset_x);
        push.size.y = (float) (drawable->global_y + drawable->draw_offset_y);
        push.offset.x = (float) drawable->width;
        push.offset.y = (float) drawable->height;
        push.ui_mark = command->ui_mark;
        push.local_time = state->transient.ui->animation_global_time_s - command->animation_start_time_s;
        kan_render_pass_instance_push_constant (pass_instance, &push);

        kan_render_pass_instance_draw (pass_instance, 0u, sizeof (ui_rect_indices) / sizeof (ui_rect_indices[0u]), 0u,
                                       0u, 1u);
        state->transient.bound_pipeline = UI_BOUND_PIPELINE_NONE;
    }
}

static void process_draw_command (struct ui_render_state_t *state,
                                  const struct kan_ui_node_drawable_t *drawable,
                                  const struct kan_ui_draw_command_data_t *command)
{
    switch (command->type)
    {
    case KAN_UI_DRAW_COMMAND_NONE:
        break;

    case KAN_UI_DRAW_COMMAND_IMAGE:
    {
        if (command->image.record_index == KAN_INT_MAX (uint32_t))
        {
            // Command is not yet initialized with image index, skip it.
            break;
        }

        if (!state->transient.image_bulk_slice_begin)
        {
            allocate_new_image_bulk_region (state);
        }

        if (state->transient.image_bulk_next >= state->transient.image_bulk_end)
        {
            flush_instanced_ui_images (state);
            allocate_new_image_bulk_region (state);
        }

        if (!state->transient.image_bulk_run_begin)
        {
            state->transient.image_bulk_run_begin = state->transient.image_bulk_next;
        }

        struct image_instanced_data_t *data = state->transient.image_bulk_next;
        ++state->transient.image_bulk_next;

        kan_instance_offset_t x = drawable->global_x + drawable->draw_offset_x;
        kan_instance_offset_t y = drawable->global_y + drawable->draw_offset_y;

        if (command->image.custom_rect)
        {
            x += command->image.custom_x_offset;
            y += command->image.custom_y_offset;
        }

        data->offset.x = (float) x;
        data->offset.y = (float) y;
        data->size.x = (float) (command->image.custom_rect ? command->image.custom_width : drawable->width);
        data->size.y = (float) (command->image.custom_rect ? command->image.custom_height : drawable->height);

        data->image_index = command->image.record_index;
        data->ui_mark = command->ui_mark;
        data->local_time = state->transient.ui->animation_global_time_s - command->animation_start_time_s;
        break;
    }

    case KAN_UI_DRAW_COMMAND_TEXT:
    {
        flush_instanced_ui_images (state);
        execute_draw_text_command (state, drawable, command);
        break;
    }

    case KAN_UI_DRAW_COMMAND_CUSTOM:
    {
        flush_instanced_ui_images (state);
        execute_draw_custom_command (state, drawable, command);
        break;
    }
    }
}

UNIVERSE_UI_API KAN_UM_MUTATOR_EXECUTE (ui_render)
{
    KAN_UMI_SINGLETON_WRITE (private, ui_render_private_singleton_t)
    KAN_UMI_SINGLETON_READ (bundle, kan_ui_bundle_singleton_t)

    if (!bundle->available)
    {
        // If bundle is not yet available, then there is nothing to do at all.
        return;
    }

    // We need to check all the updates before checking frame: for the rare cases when frame was not scheduled,
    // but the bound resources were changed and must be unbound,

    KAN_UML_EVENT_FETCH (bundle_updated_event, kan_ui_bundle_updated_t)
    {
        if (KAN_HANDLE_IS_VALID (private->pass_parameter_set))
        {
            kan_render_pipeline_parameter_set_destroy (private->pass_parameter_set);
            private->pass_parameter_set = KAN_HANDLE_SET_INVALID (kan_render_pipeline_parameter_set_t);
        }

        private->used_pass_name = NULL;
        private->bound_glyph_sdf_atlas = KAN_HANDLE_SET_INVALID (kan_render_image_t);
    }

    KAN_UML_EVENT_FETCH (pass_updated_event, kan_render_foundation_pass_updated_event_t)
    {
        if (pass_updated_event->name == private->used_pass_name)
        {
            if (KAN_HANDLE_IS_VALID (private->pass_parameter_set))
            {
                kan_render_pipeline_parameter_set_destroy (private->pass_parameter_set);
                private->pass_parameter_set = KAN_HANDLE_SET_INVALID (kan_render_pipeline_parameter_set_t);
            }

            private->used_pass_name = NULL;
            private->bound_glyph_sdf_atlas = KAN_HANDLE_SET_INVALID (kan_render_image_t);
        }
    }

    KAN_UML_EVENT_FETCH (atlas_updated_event, kan_render_atlas_updated_event_t)
    {
        if (atlas_updated_event->name == bundle->available_bundle.image_atlas &&
            KAN_HANDLE_IS_VALID (private->pass_parameter_set))
        {
            KAN_UMI_VALUE_READ_REQUIRED (atlas_loaded, kan_render_atlas_loaded_t, name, &atlas_updated_event->name)
            kan_instance_size_t atlas_layers;
            kan_render_image_get_sizes (atlas_loaded->image, NULL, NULL, NULL, &atlas_layers);

            struct kan_render_parameter_update_description_t updates[] = {
                {
                    .binding = private->binding_image_atlas,
                    .image_binding =
                        {
                            .image = atlas_loaded->image,
                            .array_index = 0u,
                            .layer_offset = 0u,
                            .layer_count = atlas_layers,
                        },
                },
                {
                    .binding = private->binding_image_entries,
                    .buffer_binding =
                        {
                            .buffer = atlas_loaded->entry_buffer,
                            .offset = 0u,
                            .range = kan_render_buffer_get_full_size (atlas_loaded->entry_buffer),
                        },
                },
            };

            kan_render_pipeline_parameter_set_update (private->pass_parameter_set,
                                                      sizeof (updates) / sizeof (updates[0u]), updates);
        }
    }

    KAN_UMI_SINGLETON_READ (text_shaping, kan_text_shaping_singleton_t)
    if (KAN_HANDLE_IS_VALID (private->pass_parameter_set) &&
        !KAN_HANDLE_IS_EQUAL (private->bound_glyph_sdf_atlas, text_shaping->font_library_sdf_atlas) &&
        KAN_HANDLE_IS_VALID (text_shaping->font_library_sdf_atlas))
    {
        kan_instance_size_t atlas_layers;
        kan_render_image_get_sizes (text_shaping->font_library_sdf_atlas, NULL, NULL, NULL, &atlas_layers);

        struct kan_render_parameter_update_description_t updates[] = {
            {
                .binding = private->binding_glyph_sdf_atlas,
                .image_binding =
                    {
                        .image = text_shaping->font_library_sdf_atlas,
                        .array_index = 0u,
                        .layer_offset = 0u,
                        .layer_count = atlas_layers,
                    },
            },
        };

        kan_render_pipeline_parameter_set_update (private->pass_parameter_set, sizeof (updates) / sizeof (updates[0u]),
                                                  updates);
    }

    KAN_UMI_SINGLETON_READ (render_context, kan_render_context_singleton_t)
    KAN_UML_EVENT_FETCH (color_table_updated, kan_render_color_table_buffer_updated_t)
    {
        if (KAN_HANDLE_IS_VALID (private->pass_parameter_set))
        {
            struct kan_render_parameter_update_description_t updates[] = {
                {
                    .binding = private->binding_color_table,
                    .buffer_binding =
                        {
                            .buffer = render_context->color_table_buffer,
                            .offset = 0u,
                            .range = kan_render_buffer_get_full_size (render_context->color_table_buffer),
                        },
                },
            };

            kan_render_pipeline_parameter_set_update (private->pass_parameter_set,
                                                      sizeof (updates) / sizeof (updates[0u]), updates);
        }
    }

    if (!render_context->frame_scheduled)
    {
        return;
    }

    KAN_UMI_SINGLETON_READ (ui, kan_ui_singleton_t)
    KAN_UMI_SINGLETON_READ (ui_render_graph, kan_ui_render_graph_singleton_t)

    if (!KAN_HANDLE_IS_VALID (ui_render_graph->final_pass_instance))
    {
        return;
    }

    if (!KAN_HANDLE_IS_VALID (private->frame_lifetime_allocator))
    {
        private->frame_lifetime_allocator = kan_render_frame_lifetime_buffer_allocator_create (
            render_context->render_context, KAN_RENDER_BUFFER_TYPE_ATTRIBUTE, KAN_UNIVERSE_UI_RENDER_INSTANCED_PAGE,
            false, KAN_STATIC_INTERNED_ID_GET (ui_render_instanced_allocator));
    }

    ensure_ui_rect_ready (private, render_context);
    if (!ensure_pass_parameter_set_ready (state, private, bundle, text_shaping, render_context) ||
        !cache_material_instance_data (
            state, bundle->available_bundle.pass, bundle->available_bundle.image_material_instance,
            &state->transient.image_pipeline, &state->transient.image_pipeline_material_parameters) ||
        !cache_material_instance_data (
            state, bundle->available_bundle.pass, bundle->available_bundle.text_sdf_material_instance,
            &state->transient.text_sdf_pipeline, &state->transient.text_sdf_pipeline_material_parameters) ||
        !cache_material_instance_data (
            state, bundle->available_bundle.pass, bundle->available_bundle.text_icon_material_instance,
            &state->transient.text_icon_pipeline, &state->transient.text_icon_pipeline_material_parameters))
    {
        return;
    }

    struct ui_pass_view_data_t *pass_view_data =
        kan_render_buffer_patch (private->pass_view_data_buffer, 0u, sizeof (struct ui_pass_view_data_t));

    pass_view_data->projection_view = kan_orthographic_projection (0.0f, (float) ui->viewport_width,
                                                                   (float) ui->viewport_height, 0.0f, 0.01f, 100.0f);

    state->transient.ui = ui;
    state->transient.private = private;
    state->transient.ui_render_graph = ui_render_graph;

    state->transient.bound_pipeline = UI_BOUND_PIPELINE_NONE;
    state->transient.clip_rect.x = 0;
    state->transient.clip_rect.y = 0;
    state->transient.clip_rect.width = ui->viewport_width;
    state->transient.clip_rect.height = ui->viewport_height;

    state->transient.image_bulk_run_begin = NULL;
    state->transient.image_bulk_next = NULL;
    state->transient.image_bulk_end = NULL;
    state->transient.image_bulk_slice_begin = NULL;
    state->transient.this_frame_instanced_images = 0u;

    KAN_UML_INTERVAL_ASCENDING_READ (node, kan_ui_node_drawable_t, draw_index, NULL, NULL)
    {
        if (node->fully_clipped_out || node->hidden)
        {
            continue;
        }

        apply_clip_rect (state, &node->clip_rect);
        process_draw_command (state, node, &node->main_draw_command);

        for (kan_loop_size_t index = 0; index < node->additional_draw_commands.size; ++index)
        {
            const struct kan_ui_draw_command_data_t *command =
                &((struct kan_ui_draw_command_data_t *) node->additional_draw_commands.data)[index];
            process_draw_command (state, node, command);
        }
    }

    flush_instanced_ui_images (state);
    private->previous_frame_instanced_images = state->transient.this_frame_instanced_images;
}

void kan_ui_singleton_init (struct kan_ui_singleton_t *instance)
{
    instance->node_id_counter = kan_atomic_int_init (1);
    instance->scale = 1.0f;
    instance->viewport_width = 0;
    instance->viewport_height = 0;
    instance->animation_global_time_s = 0.0f;
    instance->animation_delta_time_s = 0.0f;
    instance->animation_global_time_loop_s = 24.0f * 60.0f * 60.0f;
    instance->last_time_ns = KAN_INT_MAX (kan_time_size_t);
}

void kan_ui_bundle_singleton_init (struct kan_ui_bundle_singleton_t *instance)
{
    instance->bundle_name = NULL;
    instance->selection_dirty = false;
    instance->available = false;
    kan_resource_ui_bundle_init (&instance->available_bundle);
}

void kan_ui_bundle_singleton_shutdown (struct kan_ui_bundle_singleton_t *instance)
{
    kan_resource_ui_bundle_shutdown (&instance->available_bundle);
}

void kan_ui_node_init (struct kan_ui_node_t *instance)
{
    instance->id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    instance->parent_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    instance->event_on_laid_out = false;

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

    instance->render.clip = false;
    instance->render.scroll_x = KAN_UI_VALUE_PX (0.0f);
    instance->render.scroll_y = KAN_UI_VALUE_PX (0.0f);
}

void kan_ui_node_drawable_init (struct kan_ui_node_drawable_t *instance)
{
    instance->id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    instance->draw_index = 0u;

    instance->fully_clipped_out = false;
    instance->hidden = false;

    instance->clip_rect.x = 0.0f;
    instance->clip_rect.y = 0.0f;
    instance->clip_rect.width = 0.0f;
    instance->clip_rect.height = 0.0f;

    instance->main_draw_command.ui_mark = KAN_UI_DEFAULT_MARK_FLAG_NONE;
    instance->main_draw_command.animation_start_time_s = 0.0f;
    instance->main_draw_command.type = KAN_UI_DRAW_COMMAND_NONE;

    kan_dynamic_array_init (&instance->additional_draw_commands, 0u, sizeof (struct kan_ui_draw_command_data_t),
                            alignof (struct kan_ui_draw_command_data_t), kan_allocation_group_stack_get ());

    instance->local_x = 0;
    instance->local_y = 0;
    instance->width = 0;
    instance->height = 0;
    instance->global_x = 0;
    instance->global_y = 0;
    instance->draw_offset_x = 0;
    instance->draw_offset_y = 0;

    instance->cached.grow_width = 0;
    instance->cached.grow_height = 0;
    instance->cached.compound_margin_left = 0;
    instance->cached.compound_margin_right = 0;
    instance->cached.compound_margin_top = 0;
    instance->cached.compound_margin_bottom = 0;

    instance->temporary_data = NULL;
    instance->layout_dirt_level = KAN_UI_LAYOUT_DIRT_LEVEL_NONE;
}

void kan_ui_node_drawable_shutdown (struct kan_ui_node_drawable_t *instance)
{
    kan_dynamic_array_shutdown (&instance->additional_draw_commands);
}

void kan_ui_render_graph_singleton_init (struct kan_ui_render_graph_singleton_t *instance)
{
    instance->allocation = NULL;
    instance->final_pass_instance = KAN_HANDLE_SET_INVALID (kan_render_pass_instance_t);
    instance->final_image = KAN_HANDLE_SET_INVALID (kan_render_image_t);
    instance->clear_color.r = 0.0f;
    instance->clear_color.g = 0.0f;
    instance->clear_color.b = 0.0f;
    instance->clear_color.a = 0.0f;
}
