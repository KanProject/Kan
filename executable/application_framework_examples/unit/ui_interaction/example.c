#include <application_framework_examples_ui_interaction_api.h>

#include <kan/context/all_system_names.h>
#include <kan/context/application_framework_system.h>
#include <kan/context/application_system.h>
#include <kan/inline_math/inline_math.h>
#include <kan/log/logging.h>
#include <kan/precise_time/precise_time.h>
#include <kan/resource_pipeline/meta.h>
#include <kan/test_expectation/test_expectation.h>
#include <kan/test_routine/test_routine.h>
#include <kan/universe/macro.h>
#include <kan/universe_locale/locale.h>
#include <kan/universe_render_foundation/atlas.h>
#include <kan/universe_render_foundation/render_graph.h>
#include <kan/universe_resource_provider/provider.h>
#include <kan/universe_text/text.h>
#include <kan/universe_ui/controls.h>
#include <kan/universe_ui/core.h>

KAN_LOG_DEFINE_CATEGORY (application_framework_example_ui_interaction);
KAN_USE_STATIC_INTERNED_IDS

#define KAN_UI_INTERACTION_MUTATOR_GROUP "ui_example_interaction"
KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (ui_example_interaction_update)
KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (ui_example_interaction_post_render)
APPLICATION_FRAMEWORK_EXAMPLES_UI_INTERACTION_API KAN_UM_MUTATOR_GROUP_META (ui_interaction,
                                                                             KAN_UI_INTERACTION_MUTATOR_GROUP);

#define TEST_WIDTH 1600u
#define TEST_HEIGHT 800u

struct example_ui_interaction_singleton_t
{
    kan_application_system_window_t window_handle;
    kan_render_surface_t window_surface;
    bool should_rebuild;
    bool rendered_once;
};

APPLICATION_FRAMEWORK_EXAMPLES_UI_INTERACTION_API void example_ui_interaction_singleton_init (
    struct example_ui_interaction_singleton_t *instance)
{
    instance->window_handle = KAN_HANDLE_SET_INVALID (kan_application_system_window_t);
    instance->window_surface = KAN_HANDLE_SET_INVALID (kan_render_surface_t);
    instance->should_rebuild = false;
    instance->rendered_once = false;
}

struct ui_example_interaction_update_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (ui_example_interaction_update)
    KAN_UM_BIND_STATE (ui_example_interaction_update, state)

    /// \details Used as hack to force-update showcase ui behavior on hot reload.
    bool just_deployed;

    kan_context_system_t application_system_handle;
    kan_context_system_t application_framework_system_handle;
    kan_context_system_t render_backend_system_handle;
};

APPLICATION_FRAMEWORK_EXAMPLES_UI_INTERACTION_API KAN_UM_MUTATOR_DEPLOY (ui_example_interaction_update)
{
    kan_static_interned_ids_ensure_initialized ();
    state->just_deployed = true;

    kan_context_t context = kan_universe_get_context (universe);
    state->application_system_handle = kan_context_query (context, KAN_CONTEXT_APPLICATION_SYSTEM_NAME);
    state->application_framework_system_handle =
        kan_context_query (context, KAN_CONTEXT_APPLICATION_FRAMEWORK_SYSTEM_NAME);
    state->render_backend_system_handle = kan_context_query (context, KAN_CONTEXT_RENDER_BACKEND_SYSTEM_NAME);

    kan_workflow_graph_node_depend_on (workflow_node, KAN_UI_BUNDLE_MANAGEMENT_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_UI_TIME_END_CHECKPOINT);
    kan_workflow_graph_node_make_dependency_of (workflow_node, KAN_TEXT_SHAPING_BEGIN_CHECKPOINT);
    kan_workflow_graph_node_make_dependency_of (workflow_node, KAN_UI_CONTROLS_INPUT_BEGIN_CHECKPOINT);
}

static void build_playground_ui (struct ui_example_interaction_update_state_t *state,
                                 struct example_ui_interaction_singleton_t *singleton)
{
    KAN_UMI_SINGLETON_READ (text_shaping, kan_text_shaping_singleton_t)
    KAN_UMI_SINGLETON_READ (ui_bundle, kan_ui_bundle_singleton_t)

    if (!ui_bundle->available)
    {
        return;
    }

    KAN_UMI_VALUE_READ_OPTIONAL (atlas, kan_render_atlas_loaded_t, name, &ui_bundle->available_bundle.image_atlas)
    if (!atlas)
    {
        return;
    }

    const kan_instance_size_t image_button =
        kan_render_atlas_loaded_query (atlas, KAN_STATIC_INTERNED_ID_GET (button), NULL);

    const kan_instance_size_t image_window =
        kan_render_atlas_loaded_query (atlas, KAN_STATIC_INTERNED_ID_GET (window), NULL);

    const kan_instance_size_t image_scroll_line =
        kan_render_atlas_loaded_query (atlas, KAN_STATIC_INTERNED_ID_GET (scroll_line_background), NULL);

    const kan_instance_size_t image_text_edit_cursor =
        kan_render_atlas_loaded_query (atlas, KAN_STATIC_INTERNED_ID_GET (text_edit_cursor), NULL);

    const kan_instance_size_t image_text_edit_selection =
        kan_render_atlas_loaded_query (atlas, KAN_STATIC_INTERNED_ID_GET (text_edit_selection), NULL);

    // Clear old ones if any.
    const kan_ui_node_id_t no_parent_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    KAN_UML_VALUE_DELETE (ui_root, kan_ui_node_t, parent_id, &no_parent_id) { KAN_UM_ACCESS_DELETE (ui_root); }

#define ADD_TEXT_SHAPING_UNIT(NAME, ALIGNMENT, LITERAL)                                                                \
    const kan_text_shaping_unit_id_t NAME##_id = kan_next_text_shaping_unit_id (text_shaping);                         \
    KAN_UMO_INDEXED_INSERT (NAME##_unit, kan_text_shaping_unit_t)                                                      \
    {                                                                                                                  \
        NAME##_unit->id = NAME##_id;                                                                                   \
        NAME##_unit->request.font_size = 24u;                                                                          \
        NAME##_unit->request.render_format = KAN_FONT_GLYPH_RENDER_FORMAT_SDF;                                         \
        NAME##_unit->request.orientation = KAN_TEXT_ORIENTATION_HORIZONTAL;                                            \
        NAME##_unit->request.reading_direction = KAN_TEXT_READING_DIRECTION_LEFT_TO_RIGHT;                             \
        NAME##_unit->request.alignment = ALIGNMENT;                                                                    \
        NAME##_unit->request.primary_axis_limit = 0u;                                                                  \
                                                                                                                       \
        struct kan_text_item_t text_items[] = {                                                                        \
            {                                                                                                          \
                .type = KAN_TEXT_ITEM_STYLE,                                                                           \
                .style =                                                                                               \
                    {                                                                                                  \
                        .style = NULL,                                                                                 \
                        .mark = KAN_UI_DEFAULT_TEXT_MAKE_MARK (0u, KAN_UI_DEFAULT_TEXT_MARK_FLAG_OUTLINE),             \
                    },                                                                                                 \
            },                                                                                                         \
            {                                                                                                          \
                .type = KAN_TEXT_ITEM_UTF8,                                                                            \
                .utf8 = LITERAL,                                                                                       \
            },                                                                                                         \
        };                                                                                                             \
                                                                                                                       \
        NAME##_unit->request.allow_breaks = true;                                                                      \
        NAME##_unit->request.generate_edition_markup = false;                                                          \
        NAME##_unit->request.text = kan_text_create (sizeof (text_items) / sizeof (text_items[0u]), text_items);       \
        NAME##_unit->stable = true;                                                                                    \
    }

    KAN_UMI_SINGLETON_READ (ui, kan_ui_singleton_t)
    KAN_UMO_INDEXED_INSERT (left_window_node, kan_ui_node_t)
    {
        left_window_node->id = kan_next_ui_node_id (ui);
        left_window_node->element.width = KAN_UI_VALUE_VW (0.5f);
        left_window_node->element.height = KAN_UI_VALUE_VH (0.4f);

        left_window_node->element.horizontal_alignment = KAN_UI_HORIZONTAL_ALIGNMENT_LEFT;
        left_window_node->element.vertical_alignment = KAN_UI_VERTICAL_ALIGNMENT_CENTER;
        left_window_node->local_element_order = 0;

        left_window_node->layout.layout = KAN_UI_LAYOUT_VERTICAL_CONTAINER;
        left_window_node->layout.padding = KAN_UI_RECT_PT (16.0f, 16.0f, 16.0f, 16.0f);
        left_window_node->render.clip = true;

        KAN_UMO_INDEXED_INSERT (window_drawable, kan_ui_node_drawable_t)
        {
            window_drawable->id = left_window_node->id;
            window_drawable->main_draw_command.type = KAN_UI_DRAW_COMMAND_IMAGE;
            window_drawable->main_draw_command.image = KAN_UI_IMAGE_COMMAND_DEFAULT (image_window);
        }

        KAN_UMO_INDEXED_INSERT (window_hit_box, kan_ui_node_hit_box_t)
        {
            window_hit_box->id = left_window_node->id;
            window_hit_box->interactable = false;
        }

        for (kan_loop_size_t button = 0u; button < 6u; ++button)
        {
            KAN_UMO_INDEXED_INSERT (button_node, kan_ui_node_t)
            {
                button_node->id = kan_next_ui_node_id (ui);
                button_node->parent_id = left_window_node->id;
                button_node->element.width_flags |= KAN_UI_SIZE_FLAG_GROW;
                button_node->element.height = KAN_UI_VALUE_VH (0.05f);
                button_node->element.margin = KAN_UI_RECT_PT (16.0f, 16.0f, 16.0f, 16.0f);

                button_node->element.horizontal_alignment = KAN_UI_HORIZONTAL_ALIGNMENT_CENTER;
                button_node->element.vertical_alignment = KAN_UI_VERTICAL_ALIGNMENT_CENTER;
                button_node->local_element_order = (kan_instance_offset_t) button;

                KAN_UMO_INDEXED_INSERT (button_drawable, kan_ui_node_drawable_t)
                {
                    button_drawable->id = button_node->id;
                    button_drawable->main_draw_command.type = KAN_UI_DRAW_COMMAND_IMAGE;
                    button_drawable->main_draw_command.image = KAN_UI_IMAGE_COMMAND_DEFAULT (image_button);
                    button_drawable->main_draw_command.image.allow_override = true;
                }

                KAN_UMO_INDEXED_INSERT (button_text_node, kan_ui_node_t)
                {
                    button_text_node->id = kan_next_ui_node_id (ui);
                    button_text_node->parent_id = button_node->id;

                    button_text_node->element.width_flags |= KAN_UI_SIZE_FLAG_GROW;
                    button_text_node->element.horizontal_alignment = KAN_UI_HORIZONTAL_ALIGNMENT_CENTER;
                    button_text_node->element.vertical_alignment = KAN_UI_VERTICAL_ALIGNMENT_CENTER;

                    ADD_TEXT_SHAPING_UNIT (node_text, KAN_TEXT_SHAPING_ALIGNMENT_CENTER, "Hello, world!")
                    KAN_UMO_INDEXED_INSERT (button_text_drawable, kan_ui_node_drawable_t)
                    {
                        button_text_drawable->id = button_text_node->id;
                        button_text_drawable->main_draw_command.type = KAN_UI_DRAW_COMMAND_TEXT;
                        button_text_drawable->main_draw_command.text.shaping_unit = node_text_id;
                    }

                    KAN_UMO_INDEXED_INSERT (text_behavior, kan_ui_node_text_behavior_t)
                    {
                        text_behavior->id = button_text_node->id;
                        text_behavior->shaping_unit_id = node_text_id;
                        text_behavior->font_size = KAN_UI_VALUE_PX (24.0f);
                        text_behavior->sync_text_limit_from_ui = true;
                        text_behavior->sync_ui_size_from_text_secondary = true;
                    }

                    KAN_UMO_INDEXED_INSERT (button_hit_box, kan_ui_node_hit_box_t)
                    {
                        button_hit_box->id = button_node->id;
                        button_hit_box->interactable = true;
                        button_hit_box->interactable_style = KAN_STATIC_INTERNED_ID_GET (default);
                        kan_dynamic_array_set_capacity (&button_hit_box->propagate_interaction_visuals, 1u);

                        *(kan_ui_node_id_t *) kan_dynamic_array_add_last (
                            &button_hit_box->propagate_interaction_visuals) = button_text_node->id;
                    }
                }
            }
        }
    }

    KAN_UMO_INDEXED_INSERT (center_window_node, kan_ui_node_t)
    {
        center_window_node->id = kan_next_ui_node_id (ui);
        center_window_node->element.width = KAN_UI_VALUE_VW (0.5f);
        center_window_node->element.height = KAN_UI_VALUE_VH (0.5f);

        center_window_node->element.horizontal_alignment = KAN_UI_HORIZONTAL_ALIGNMENT_CENTER;
        center_window_node->element.vertical_alignment = KAN_UI_VERTICAL_ALIGNMENT_CENTER;
        center_window_node->local_element_order = 1;

        center_window_node->layout.layout = KAN_UI_LAYOUT_VERTICAL_CONTAINER;
        center_window_node->layout.padding = KAN_UI_RECT_PT (16.0f, 16.0f, 16.0f, 16.0f);
        center_window_node->render.clip = true;

        KAN_UMO_INDEXED_INSERT (window_drawable, kan_ui_node_drawable_t)
        {
            window_drawable->id = center_window_node->id;
            window_drawable->main_draw_command.type = KAN_UI_DRAW_COMMAND_IMAGE;
            window_drawable->main_draw_command.image = KAN_UI_IMAGE_COMMAND_DEFAULT (image_window);
        }

        KAN_UMO_INDEXED_INSERT (window_hit_box, kan_ui_node_hit_box_t)
        {
            window_hit_box->id = center_window_node->id;
            window_hit_box->interactable = false;
        }

        KAN_UMO_INDEXED_INSERT (scroll_outer_node, kan_ui_node_t)
        {
            scroll_outer_node->id = kan_next_ui_node_id (ui);
            scroll_outer_node->parent_id = center_window_node->id;

            scroll_outer_node->element.width_flags = KAN_UI_SIZE_FLAG_GROW;
            scroll_outer_node->element.height_flags = KAN_UI_SIZE_FLAG_GROW;
            scroll_outer_node->local_element_order = 0;
            scroll_outer_node->layout.layout = KAN_UI_LAYOUT_FRAME;
            scroll_outer_node->render.clip = true;

            KAN_UMO_INDEXED_INSERT (scroll_outer_hit_box, kan_ui_node_hit_box_t)
            {
                scroll_outer_hit_box->id = scroll_outer_node->id;
                scroll_outer_hit_box->interactable = false;
            }

            const kan_ui_node_id_t container_id = kan_next_ui_node_id (ui);
            const kan_ui_node_id_t line_id = kan_next_ui_node_id (ui);
            const kan_ui_node_id_t knob_id = kan_next_ui_node_id (ui);

            KAN_UMO_INDEXED_INSERT (scroll_behavior, kan_ui_node_scroll_behavior_t)
            {
                scroll_behavior->id = scroll_outer_node->id;
                scroll_behavior->container_id = container_id;
                scroll_behavior->vertical_line_id = line_id;
                scroll_behavior->vertical_knob_id = knob_id;
                scroll_behavior->horizontal = false;
                scroll_behavior->vertical = true;
            }

            KAN_UMO_INDEXED_INSERT (scroll_container_node, kan_ui_node_t)
            {
                scroll_container_node->id = container_id;
                scroll_container_node->parent_id = scroll_outer_node->id;

                scroll_container_node->element.width_flags = KAN_UI_SIZE_FLAG_GROW;
                scroll_container_node->element.height_flags = KAN_UI_SIZE_FLAG_GROW | KAN_UI_SIZE_FLAG_FIT_CHILDREN;
                scroll_container_node->local_element_order = 0;
                scroll_container_node->layout.layout = KAN_UI_LAYOUT_VERTICAL_CONTAINER;

                const char *big_text =
                    "Robert Guiscard also referred to as Robert de Hauteville, was a Norman adventurer remembered for "
                    "his conquest of southern Italy and Sicily in the 11th century.\n"
                    "\n"
                    "Robert was born into the Hauteville family in Normandy, the sixth son of Tancred de Hauteville "
                    "and his wife Fressenda. He inherited the County of Apulia and Calabria from his brother in 1057, "
                    "and in 1059 he was made Duke of Apulia and Calabria and Lord of Sicily by Pope Nicholas II. He "
                    "was also briefly Prince of Benevento (1078–1081), before returning the title to the papacy.";

                KAN_UMO_INDEXED_INSERT (inner_text_node, kan_ui_node_t)
                {
                    inner_text_node->id = kan_next_ui_node_id (ui);
                    inner_text_node->parent_id = scroll_container_node->id;

                    inner_text_node->element.width_flags |= KAN_UI_SIZE_FLAG_GROW;
                    inner_text_node->element.horizontal_alignment = KAN_UI_HORIZONTAL_ALIGNMENT_CENTER;
                    inner_text_node->element.vertical_alignment = KAN_UI_VERTICAL_ALIGNMENT_CENTER;
                    inner_text_node->element.margin = KAN_UI_RECT_PT (16.0f, 16.0f, 16.0f, 16.0f);

                    ADD_TEXT_SHAPING_UNIT (node_text, KAN_TEXT_SHAPING_ALIGNMENT_CENTER, big_text)
                    KAN_UMO_INDEXED_INSERT (inner_text_drawable, kan_ui_node_drawable_t)
                    {
                        inner_text_drawable->id = inner_text_node->id;
                        inner_text_drawable->main_draw_command.type = KAN_UI_DRAW_COMMAND_TEXT;
                        inner_text_drawable->main_draw_command.text.shaping_unit = node_text_id;
                    }

                    KAN_UMO_INDEXED_INSERT (text_behavior, kan_ui_node_text_behavior_t)
                    {
                        text_behavior->id = inner_text_node->id;
                        text_behavior->shaping_unit_id = node_text_id;
                        text_behavior->font_size = KAN_UI_VALUE_PX (36.0f);
                        text_behavior->sync_text_limit_from_ui = true;
                        text_behavior->sync_ui_size_from_text_secondary = true;
                    }
                }

                KAN_UMO_INDEXED_INSERT (line_edit_node, kan_ui_node_t)
                {
                    line_edit_node->id = kan_next_ui_node_id (ui);
                    const kan_ui_node_id_t edit_text_id = kan_next_ui_node_id (ui);
                    line_edit_node->parent_id = scroll_container_node->id;

                    line_edit_node->element.width_flags |= KAN_UI_SIZE_FLAG_GROW;
                    line_edit_node->element.height_flags |= KAN_UI_SIZE_FLAG_FIT_CHILDREN;
                    line_edit_node->element.horizontal_alignment = KAN_UI_HORIZONTAL_ALIGNMENT_CENTER;
                    line_edit_node->element.vertical_alignment = KAN_UI_VERTICAL_ALIGNMENT_CENTER;
                    line_edit_node->element.margin = KAN_UI_RECT_PT (16.0f, 16.0f, 16.0f, 16.0f);
                    line_edit_node->local_element_order = 1;

                    line_edit_node->layout.layout = KAN_UI_LAYOUT_VERTICAL_CONTAINER;
                    line_edit_node->layout.padding = KAN_UI_RECT_PT (4.0f, 4.0f, 4.0f, 4.0f);
                    line_edit_node->layout.padding = KAN_UI_RECT_PT (4.0f, 4.0f, 4.0f, 4.0f);
                    line_edit_node->render.clip = true;

                    KAN_UMO_INDEXED_INSERT (line_edit_drawable, kan_ui_node_drawable_t)
                    {
                        line_edit_drawable->id = line_edit_node->id;
                        line_edit_drawable->main_draw_command.type = KAN_UI_DRAW_COMMAND_IMAGE;
                        line_edit_drawable->main_draw_command.image = KAN_UI_IMAGE_COMMAND_NONE;
                        line_edit_drawable->main_draw_command.image.allow_override = true;
                    }

                    ADD_TEXT_SHAPING_UNIT (node_text, KAN_TEXT_SHAPING_ALIGNMENT_CENTER, "<Temp value>")
                    KAN_UMO_INDEXED_INSERT (line_edit_behavior, kan_ui_node_line_edit_behavior_t)
                    {
                        line_edit_behavior->id = line_edit_node->id;
                        line_edit_behavior->text_id = edit_text_id;
                        line_edit_behavior->shaping_unit_id = node_text_id;

                        line_edit_behavior->interactable_style_regular = KAN_STATIC_INTERNED_ID_GET (line_edit_regular);
                        line_edit_behavior->interactable_style_selected =
                            KAN_STATIC_INTERNED_ID_GET (line_edit_selected);

                        line_edit_behavior->cursor_image_index = image_text_edit_cursor;
                        line_edit_behavior->cursor_width = KAN_UI_VALUE_PT (3.0f);

                        line_edit_behavior->selection_image_index = image_text_edit_selection;
                        line_edit_behavior->selection_leeway = KAN_UI_VALUE_PT (3.0f);

                        kan_ui_node_line_edit_behavior_set_content (
                            line_edit_behavior, "Hello, world!", NULL,
                            KAN_UI_DEFAULT_TEXT_MAKE_MARK (0u, KAN_UI_DEFAULT_TEXT_MARK_FLAG_OUTLINE));
                    }

                    KAN_UMO_INDEXED_INSERT (edit_hit_box, kan_ui_node_hit_box_t)
                    {
                        edit_hit_box->id = line_edit_node->id;
                        edit_hit_box->interactable = true;
                        edit_hit_box->scroll_passthrough = true;

                        kan_dynamic_array_set_capacity (&edit_hit_box->propagate_interaction_visuals, 1u);
                        *(kan_ui_node_id_t *) kan_dynamic_array_add_last (
                            &edit_hit_box->propagate_interaction_visuals) = edit_text_id;
                    }

                    KAN_UMO_INDEXED_INSERT (edit_text_node, kan_ui_node_t)
                    {
                        edit_text_node->id = edit_text_id;
                        edit_text_node->parent_id = line_edit_node->id;

                        edit_text_node->element.width_flags |= KAN_UI_SIZE_FLAG_GROW;
                        // A little bit ad-hoc with height. Used font height is a little bit higher that default
                        // "size / 0.75" and also we're applying outline which also increases visual size a little bit.
                        edit_text_node->element.height = KAN_UI_VALUE_PT (36.0f / 0.75f + 3.0f);
                        edit_text_node->element.horizontal_alignment = KAN_UI_HORIZONTAL_ALIGNMENT_CENTER;

                        KAN_UMO_INDEXED_INSERT (edit_text_drawable, kan_ui_node_drawable_t)
                        {
                            edit_text_drawable->id = edit_text_node->id;
                            edit_text_drawable->main_draw_command.type = KAN_UI_DRAW_COMMAND_TEXT;
                            edit_text_drawable->main_draw_command.text.shaping_unit = node_text_id;
                        }

                        KAN_UMO_INDEXED_INSERT (text_behavior, kan_ui_node_text_behavior_t)
                        {
                            text_behavior->id = edit_text_node->id;
                            text_behavior->shaping_unit_id = node_text_id;
                            text_behavior->font_size = KAN_UI_VALUE_PX (36.0f);
                            text_behavior->sync_text_limit_from_ui = true;
                            text_behavior->sync_ui_size_from_text_secondary = false;
                        }
                    }
                }
            }

            KAN_UMO_INDEXED_INSERT (scroll_line, kan_ui_node_t)
            {
                scroll_line->id = line_id;
                scroll_line->parent_id = scroll_outer_node->id;

                scroll_line->element.width = KAN_UI_VALUE_PT (32.0f);
                scroll_line->element.height_flags = KAN_UI_SIZE_FLAG_GROW;
                scroll_line->element.horizontal_alignment = KAN_UI_HORIZONTAL_ALIGNMENT_RIGHT;
                scroll_line->element.vertical_alignment = KAN_UI_VERTICAL_ALIGNMENT_TOP;

                scroll_line->local_element_order = 1000;
                scroll_line->layout.layout = KAN_UI_LAYOUT_FRAME;

                KAN_UMO_INDEXED_INSERT (line_drawable, kan_ui_node_drawable_t)
                {
                    line_drawable->id = scroll_line->id;
                    line_drawable->main_draw_command.type = KAN_UI_DRAW_COMMAND_IMAGE;
                    line_drawable->main_draw_command.image = KAN_UI_IMAGE_COMMAND_DEFAULT (image_scroll_line);
                }

                KAN_UMO_INDEXED_INSERT (hit_box, kan_ui_node_hit_box_t)
                {
                    hit_box->id = scroll_line->id;
                    hit_box->interactable = true;
                    hit_box->scroll_passthrough = true;

                    hit_box->interactable_style = KAN_STATIC_INTERNED_ID_GET (default);
                    kan_dynamic_array_set_capacity (&hit_box->propagate_interaction_visuals, 1u);

                    *(kan_ui_node_id_t *) kan_dynamic_array_add_last (&hit_box->propagate_interaction_visuals) =
                        knob_id;
                }

                KAN_UMO_INDEXED_INSERT (scroll_knob, kan_ui_node_t)
                {
                    scroll_knob->id = knob_id;
                    scroll_knob->parent_id = scroll_line->id;

                    scroll_knob->element.width_flags = KAN_UI_SIZE_FLAG_GROW;
                    scroll_knob->element.height = KAN_UI_VALUE_PT (64.0f);

                    KAN_UMO_INDEXED_INSERT (knob_drawable, kan_ui_node_drawable_t)
                    {
                        knob_drawable->id = scroll_knob->id;
                        knob_drawable->main_draw_command.type = KAN_UI_DRAW_COMMAND_IMAGE;
                        knob_drawable->main_draw_command.image = KAN_UI_IMAGE_COMMAND_DEFAULT (image_button);
                        knob_drawable->main_draw_command.image.allow_override = true;
                    }
                }
            }
        }
    }

    singleton->should_rebuild = false;
}

APPLICATION_FRAMEWORK_EXAMPLES_UI_INTERACTION_API KAN_UM_MUTATOR_EXECUTE (ui_example_interaction_update)
{
    // Write access just to write color table values once, okay for the example.
    KAN_UMI_SINGLETON_WRITE (render_context, kan_render_context_singleton_t)
    KAN_UMI_SINGLETON_READ (resource_provider, kan_resource_provider_singleton_t)
    KAN_UMI_SINGLETON_WRITE (test, test_routine_singleton_t)
    KAN_UMI_SINGLETON_WRITE (singleton, example_ui_interaction_singleton_t)

    if (!KAN_HANDLE_IS_VALID (render_context->render_context))
    {
        return;
    }

    if (!KAN_HANDLE_IS_VALID (singleton->window_handle))
    {
        enum kan_platform_window_flag_t flags = kan_render_get_required_window_flags ();
        if (test->test_mode_enabled)
        {
            flags |= KAN_PLATFORM_WINDOW_FLAG_HIDDEN;
        }
        else
        {
            flags |= KAN_PLATFORM_WINDOW_FLAG_RESIZABLE;
        }

        singleton->window_handle = kan_application_system_window_create (state->application_system_handle,
                                                                         "application_framework_example_ui_interaction",
                                                                         TEST_WIDTH, TEST_HEIGHT, flags);

        enum kan_render_surface_present_mode_t present_modes[] = {
            KAN_RENDER_SURFACE_PRESENT_MODE_MAILBOX,
            KAN_RENDER_SURFACE_PRESENT_MODE_IMMEDIATE,
            KAN_RENDER_SURFACE_PRESENT_MODE_INVALID,
        };

        singleton->window_surface =
            kan_render_backend_system_create_surface (state->render_backend_system_handle, singleton->window_handle,
                                                      present_modes, KAN_STATIC_INTERNED_ID_GET (window_surface));

        if (!KAN_HANDLE_IS_VALID (singleton->window_surface))
        {
            KAN_LOG (application_framework_example_ui_interaction, KAN_LOG_ERROR, "Failed to create surface.")
            kan_application_framework_system_request_exit (state->application_framework_system_handle, 1);
            return;
        }

        kan_application_system_window_raise (state->application_system_handle, singleton->window_handle);

        // Initialize locale here as well, because this will only be executed once.
        KAN_UMI_SINGLETON_WRITE (locale, kan_locale_singleton_t)
        locale->selected_locale = kan_string_intern ("en");

        KAN_UMI_SINGLETON_WRITE (ui_render_graph, kan_ui_render_graph_singleton_t)
        ui_render_graph->clear_color = kan_color_linear_to_srgb (kan_make_color_linear (0.0f, 1.0f, 1.0f, 1.0f));
    }

    if (test->test_mode_enabled && !KAN_HANDLE_IS_VALID (test->expectation_read_back_buffer))
    {
        test->expectation_read_back_buffer = kan_render_buffer_create (
            render_context->render_context, KAN_RENDER_BUFFER_TYPE_READ_BACK_STORAGE, TEST_WIDTH * TEST_HEIGHT * 4u,
            NULL, KAN_STATIC_INTERNED_ID_GET (test_read_back_buffer));
    }

    if (KAN_HANDLE_IS_VALID (singleton->window_handle))
    {
        const struct kan_application_system_window_info_t *window_info =
            kan_application_system_get_window_info_from_handle (state->application_system_handle,
                                                                singleton->window_handle);

        KAN_UMI_SINGLETON_WRITE (ui, kan_ui_singleton_t)
        ui->scale = 1.0f;
        ui->viewport_width = window_info->width_for_render;
        ui->viewport_height = window_info->height_for_render;

        KAN_UMI_SINGLETON_WRITE (ui_input, kan_ui_input_singleton_t)
        ui_input->linked_window_handle = window_info->handle;
        ui_input->linked_window_id = window_info->id;

        if (!KAN_HANDLE_IS_VALID (ui_input->event_iterator))
        {
            ui_input->event_iterator = kan_application_system_event_iterator_create (state->application_system_handle);
        }
    }

    if (state->just_deployed)
    {
        singleton->should_rebuild = true;
        state->just_deployed = false;
    }

    KAN_UML_EVENT_FETCH (atlas_event, kan_render_atlas_updated_event_t)
    {
        // We don't have other atlases here, so do it like this for simplicity.
        singleton->should_rebuild = true;
    }

    if (singleton->should_rebuild)
    {
        build_playground_ui (state, singleton);
    }
}

struct ui_example_interaction_post_render_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (ui_example_interaction_post_render)
    KAN_UM_BIND_STATE (ui_example_interaction_post_render, state)
    kan_context_system_t application_system_handle;
};

APPLICATION_FRAMEWORK_EXAMPLES_UI_INTERACTION_API KAN_UM_MUTATOR_DEPLOY (ui_example_interaction_post_render)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_context_t context = kan_universe_get_context (universe);
    state->application_system_handle = kan_context_query (context, KAN_CONTEXT_APPLICATION_SYSTEM_NAME);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_UI_RENDER_END_CHECKPOINT);
}

APPLICATION_FRAMEWORK_EXAMPLES_UI_INTERACTION_API KAN_UM_MUTATOR_EXECUTE (ui_example_interaction_post_render)
{
    KAN_UMI_SINGLETON_READ (ui_render_graph, kan_ui_render_graph_singleton_t)
    KAN_UMI_SINGLETON_READ (resource_provider, kan_resource_provider_singleton_t)
    KAN_UMI_SINGLETON_WRITE (test, test_routine_singleton_t)
    KAN_UMI_SINGLETON_WRITE (singleton, example_ui_interaction_singleton_t)

    if (KAN_HANDLE_IS_VALID (ui_render_graph->final_pass_instance) && KAN_HANDLE_IS_VALID (singleton->window_surface))
    {
        const struct kan_application_system_window_info_t *window_info =
            kan_application_system_get_window_info_from_handle (state->application_system_handle,
                                                                singleton->window_handle);

        struct kan_render_integer_region_2d_t region = {
            .x = 0,
            .y = 0,
            .width = window_info->width_for_render,
            .height = window_info->height_for_render,
        };

        kan_render_backend_system_present_image_on_surface (singleton->window_surface, ui_render_graph->final_image, 0u,
                                                            region, region, ui_render_graph->final_pass_instance);

        if (singleton->rendered_once && test->test_mode_enabled && test->expectation_read_back_statuses.size == 0u)
        {
            kan_dynamic_array_set_capacity (&test->expectation_read_back_statuses, 1u);
            *(kan_render_read_back_status_t *) kan_dynamic_array_add_last (&test->expectation_read_back_statuses) =
                kan_render_request_read_back_from_image (ui_render_graph->final_image, 0u, 0u,
                                                         test->expectation_read_back_buffer, 0u,
                                                         ui_render_graph->final_pass_instance);
        }

        // We'd like to capture the second frame, not first one,
        // as text-height-to-ui sync will only be applied on second frame and onward.
        singleton->rendered_once = true;
    }
}
