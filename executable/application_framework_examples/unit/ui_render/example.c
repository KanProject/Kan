#include <application_framework_examples_ui_render_api.h>

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

KAN_LOG_DEFINE_CATEGORY (application_framework_example_ui_render);
KAN_USE_STATIC_INTERNED_IDS

#define KAN_UI_RENDER_MUTATOR_GROUP "ui_example_render"
KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (ui_example_render_update)
KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (ui_example_render_post_render)
APPLICATION_FRAMEWORK_EXAMPLES_UI_RENDER_API KAN_UM_MUTATOR_GROUP_META (ui_render, KAN_UI_RENDER_MUTATOR_GROUP);

#define TEST_WIDTH 1600u
#define TEST_HEIGHT 800u

struct example_ui_render_singleton_t
{
    kan_application_system_window_t window_handle;
    kan_render_surface_t window_surface;
    bool should_rebuild;
    kan_instance_size_t rendered_frames;
};

APPLICATION_FRAMEWORK_EXAMPLES_UI_RENDER_API void example_ui_render_singleton_init (
    struct example_ui_render_singleton_t *instance)
{
    instance->window_handle = KAN_HANDLE_SET_INVALID (kan_application_system_window_t);
    instance->window_surface = KAN_HANDLE_SET_INVALID (kan_render_surface_t);
    instance->should_rebuild = false;
    instance->rendered_frames = 0u;
}

struct ui_example_render_update_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (ui_example_render_update)
    KAN_UM_BIND_STATE (ui_example_render_update, state)

    /// \details Used as hack to force-update showcase ui behavior on hot reload.
    bool just_deployed;

    kan_context_system_t application_system_handle;
    kan_context_system_t application_framework_system_handle;
    kan_context_system_t render_backend_system_handle;
};

APPLICATION_FRAMEWORK_EXAMPLES_UI_RENDER_API KAN_UM_MUTATOR_DEPLOY (ui_example_render_update)
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

static void build_playground_ui (struct ui_example_render_update_state_t *state,
                                 struct example_ui_render_singleton_t *singleton)
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

    const kan_instance_size_t image_button_negative =
        kan_render_atlas_loaded_query (atlas, KAN_STATIC_INTERNED_ID_GET (button_negative), NULL);

    const kan_instance_size_t image_button_positive =
        kan_render_atlas_loaded_query (atlas, KAN_STATIC_INTERNED_ID_GET (button_positive), NULL);

    const kan_instance_size_t image_button_neutral =
        kan_render_atlas_loaded_query (atlas, KAN_STATIC_INTERNED_ID_GET (button_neutral), NULL);

    const kan_instance_size_t image_window =
        kan_render_atlas_loaded_query (atlas, KAN_STATIC_INTERNED_ID_GET (window), NULL);

    const kan_instance_size_t image_shield =
        kan_render_atlas_loaded_query (atlas, KAN_STATIC_INTERNED_ID_GET (shield), NULL);

    const kan_instance_size_t image_tiled_slice =
        kan_render_atlas_loaded_query (atlas, KAN_STATIC_INTERNED_ID_GET (tiled_slice), NULL);

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
        NAME##_unit->request.text = kan_text_create (sizeof (text_items) / sizeof (text_items[0u]), text_items);       \
        NAME##_unit->stable = true;                                                                                    \
    }

    const kan_instance_size_t button_images[] = {image_button_negative, image_button_positive, image_button_neutral};
    const float button_sizes[] = {0.25f, 0.20f, 0.15f};
    const char *button_texts[] = {"Evil color!", "Good color!", "Neutral!"};

    KAN_UMI_SINGLETON_READ (ui, kan_ui_singleton_t)
    KAN_UMO_INDEXED_INSERT (window_node, kan_ui_node_t)
    {
        window_node->id = kan_next_ui_node_id (ui);
        window_node->element.width_flags |= KAN_UI_SIZE_FLAG_FIT_CHILDREN;
        window_node->element.height_flags |= KAN_UI_SIZE_FLAG_FIT_CHILDREN;

        window_node->element.width = KAN_UI_VALUE_VH (0.9f);
        window_node->element.height = KAN_UI_VALUE_VH (0.3f);

        window_node->element.horizontal_alignment = KAN_UI_HORIZONTAL_ALIGNMENT_CENTER;
        window_node->element.vertical_alignment = KAN_UI_VERTICAL_ALIGNMENT_CENTER;
        window_node->local_element_order = 0;

        window_node->layout.layout = KAN_UI_LAYOUT_VERTICAL_CONTAINER;
        window_node->layout.padding = KAN_UI_RECT_PT (16.0f, 16.0f, 16.0f, 16.0f);
        window_node->render.clip = true;

        KAN_UMO_INDEXED_INSERT (window_drawable, kan_ui_node_drawable_t)
        {
            window_drawable->id = window_node->id;
            window_drawable->main_draw_command.type = KAN_UI_DRAW_COMMAND_IMAGE;
            window_drawable->main_draw_command.image = KAN_UI_IMAGE_COMMAND_DEFAULT (image_window);
        }

        KAN_UMO_INDEXED_INSERT (top_text_node, kan_ui_node_t)
        {
            top_text_node->id = kan_next_ui_node_id (ui);
            top_text_node->parent_id = window_node->id;

            top_text_node->element.width_flags |= KAN_UI_SIZE_FLAG_GROW;
            top_text_node->local_element_order = -999; // Small value to always be on top.

            ADD_TEXT_SHAPING_UNIT (node_text, KAN_TEXT_SHAPING_ALIGNMENT_LEFT,
                                   "Centered window with nine-slice buttons for tests.")

            KAN_UMO_INDEXED_INSERT (top_text_drawable, kan_ui_node_drawable_t)
            {
                top_text_drawable->id = top_text_node->id;
                top_text_drawable->main_draw_command.type = KAN_UI_DRAW_COMMAND_TEXT;
                top_text_drawable->main_draw_command.text.shaping_unit = node_text_id;
                top_text_drawable->main_draw_command.text.animation_start_time_s = 0.0f;
            }

            KAN_UMO_INDEXED_INSERT (text_behavior, kan_ui_node_text_behavior_t)
            {
                text_behavior->id = top_text_node->id;
                text_behavior->shaping_unit = node_text_id;
                text_behavior->font_size = KAN_UI_VALUE_PX (24.0f);
                text_behavior->sync_text_limit_from_ui = true;
                text_behavior->sync_ui_size_from_text_secondary = true;
            }
        }

        KAN_UMO_INDEXED_INSERT (remark_text_node, kan_ui_node_t)
        {
            remark_text_node->id = kan_next_ui_node_id (ui);
            remark_text_node->parent_id = window_node->id;

            remark_text_node->element.width_flags |= KAN_UI_SIZE_FLAG_GROW;
            remark_text_node->local_element_order = -100;

            ADD_TEXT_SHAPING_UNIT (node_text, KAN_TEXT_SHAPING_ALIGNMENT_LEFT,
                                   "Button colors are selected from color table by the way.")

            KAN_UMO_INDEXED_INSERT (remark_text_drawable, kan_ui_node_drawable_t)
            {
                remark_text_drawable->id = remark_text_node->id;
                remark_text_drawable->main_draw_command.type = KAN_UI_DRAW_COMMAND_TEXT;
                remark_text_drawable->main_draw_command.text.shaping_unit = node_text_id;
                remark_text_drawable->main_draw_command.text.animation_start_time_s = 0.0f;
            }

            KAN_UMO_INDEXED_INSERT (text_behavior, kan_ui_node_text_behavior_t)
            {
                text_behavior->id = remark_text_node->id;
                text_behavior->shaping_unit = node_text_id;
                text_behavior->font_size = KAN_UI_VALUE_PX (24.0f);
                text_behavior->sync_text_limit_from_ui = true;
                text_behavior->sync_ui_size_from_text_secondary = true;
            }
        }

        KAN_UMO_INDEXED_INSERT (icons_test_text_node, kan_ui_node_t)
        {
            icons_test_text_node->id = kan_next_ui_node_id (ui);
            icons_test_text_node->parent_id = window_node->id;

            icons_test_text_node->element.width_flags |= KAN_UI_SIZE_FLAG_GROW;
            icons_test_text_node->local_element_order = 100;

            const kan_text_shaping_unit_id_t node_text_id = kan_next_text_shaping_unit_id (text_shaping);
            KAN_UMO_INDEXED_INSERT (icons_test_text_unit, kan_text_shaping_unit_t)
            {
                icons_test_text_unit->id = node_text_id;
                icons_test_text_unit->request.font_size = 24u;
                icons_test_text_unit->request.render_format = KAN_FONT_GLYPH_RENDER_FORMAT_SDF;
                icons_test_text_unit->request.orientation = KAN_TEXT_ORIENTATION_HORIZONTAL;
                icons_test_text_unit->request.reading_direction = KAN_TEXT_READING_DIRECTION_LEFT_TO_RIGHT;
                icons_test_text_unit->request.alignment = KAN_TEXT_SHAPING_ALIGNMENT_CENTER;
                icons_test_text_unit->request.primary_axis_limit = 0u;

                struct kan_text_item_t text_items[] = {
                    {
                        .type = KAN_TEXT_ITEM_ICON,
                        .icon =
                            {
                                .icon_index = image_shield,
                                .x_scale = 0.8f,
                                .y_scale = 0.8f,
                            },
                    },
                    {
                        .type = KAN_TEXT_ITEM_ICON,
                        .icon =
                            {
                                .icon_index = image_shield,
                                .x_scale = 0.8f,
                                .y_scale = 0.8f,
                            },
                    },
                    {
                        .type = KAN_TEXT_ITEM_ICON,
                        .icon =
                            {
                                .icon_index = image_shield,
                                .x_scale = 0.8f,
                                .y_scale = 0.8f,
                            },
                    },
                    {
                        .type = KAN_TEXT_ITEM_STYLE,
                        .style =
                            {
                                .style = NULL,
                                .mark = KAN_UI_DEFAULT_TEXT_MAKE_MARK (0u, KAN_UI_DEFAULT_TEXT_MARK_FLAG_OUTLINE),
                            },
                    },
                    {
                        .type = KAN_TEXT_ITEM_UTF8,
                        .utf8 = " this text tests icons ",
                    },
                    {
                        .type = KAN_TEXT_ITEM_ICON,
                        .icon =
                            {
                                .icon_index = image_shield,
                                .x_scale = 0.8f,
                                .y_scale = 0.8f,
                            },
                    },
                    {
                        .type = KAN_TEXT_ITEM_ICON,
                        .icon =
                            {
                                .icon_index = image_shield,
                                .x_scale = 0.8f,
                                .y_scale = 0.8f,
                            },
                    },
                    {
                        .type = KAN_TEXT_ITEM_ICON,
                        .icon =
                            {
                                .icon_index = image_shield,
                                .x_scale = 0.8f,
                                .y_scale = 0.8f,
                            },
                    },
                };

                icons_test_text_unit->request.text =
                    kan_text_create (sizeof (text_items) / sizeof (text_items[0u]), text_items);
                icons_test_text_unit->stable = true;
            }

            KAN_UMO_INDEXED_INSERT (icons_test_text_drawable, kan_ui_node_drawable_t)
            {
                icons_test_text_drawable->id = icons_test_text_node->id;
                icons_test_text_drawable->main_draw_command.type = KAN_UI_DRAW_COMMAND_TEXT;
                icons_test_text_drawable->main_draw_command.text.shaping_unit = node_text_id;
                icons_test_text_drawable->main_draw_command.text.animation_start_time_s = 0.0f;
            }

            KAN_UMO_INDEXED_INSERT (text_behavior, kan_ui_node_text_behavior_t)
            {
                text_behavior->id = icons_test_text_node->id;
                text_behavior->shaping_unit = node_text_id;
                text_behavior->font_size = KAN_UI_VALUE_PX (24.0f);
                text_behavior->sync_text_limit_from_ui = true;
                text_behavior->sync_ui_size_from_text_secondary = true;
            }
        }

        KAN_UMO_INDEXED_INSERT (bottom_text_node, kan_ui_node_t)
        {
            bottom_text_node->id = kan_next_ui_node_id (ui);
            bottom_text_node->parent_id = window_node->id;

            bottom_text_node->element.width_flags |= KAN_UI_SIZE_FLAG_GROW;
            bottom_text_node->local_element_order = 999; // Big value to always be on bottom.

            ADD_TEXT_SHAPING_UNIT (node_text, KAN_TEXT_SHAPING_ALIGNMENT_RIGHT,
                                   "This text should always be at the bottom.")

            KAN_UMO_INDEXED_INSERT (bottom_text_drawable, kan_ui_node_drawable_t)
            {
                bottom_text_drawable->id = bottom_text_node->id;
                bottom_text_drawable->main_draw_command.type = KAN_UI_DRAW_COMMAND_TEXT;
                bottom_text_drawable->main_draw_command.text.shaping_unit = node_text_id;
                bottom_text_drawable->main_draw_command.text.animation_start_time_s = 0.0f;
            }

            KAN_UMO_INDEXED_INSERT (text_behavior, kan_ui_node_text_behavior_t)
            {
                text_behavior->id = bottom_text_node->id;
                text_behavior->shaping_unit = node_text_id;
                text_behavior->font_size = KAN_UI_VALUE_PX (24.0f);
                text_behavior->sync_text_limit_from_ui = true;
                text_behavior->sync_ui_size_from_text_secondary = true;
            }
        }

        KAN_UMO_INDEXED_INSERT (horizontal_node, kan_ui_node_t)
        {
            horizontal_node->id = kan_next_ui_node_id (ui);
            horizontal_node->parent_id = window_node->id;

            horizontal_node->element.width_flags |= KAN_UI_SIZE_FLAG_FIT_CHILDREN | KAN_UI_SIZE_FLAG_GROW;
            horizontal_node->element.height_flags |= KAN_UI_SIZE_FLAG_FIT_CHILDREN;
            horizontal_node->local_element_order = 0;
            horizontal_node->layout.layout = KAN_UI_LAYOUT_HORIZONTAL_CONTAINER;

            for (kan_loop_size_t column = 0u; column < 3u; ++column)
            {
                KAN_UMO_INDEXED_INSERT (column_node, kan_ui_node_t)
                {
                    column_node->id = kan_next_ui_node_id (ui);
                    column_node->parent_id = horizontal_node->id;

                    column_node->element.width_flags |= KAN_UI_SIZE_FLAG_FIT_CHILDREN;
                    column_node->element.height_flags |= KAN_UI_SIZE_FLAG_FIT_CHILDREN;

                    if (column == 1u)
                    {
                        column_node->element.width_flags |= KAN_UI_SIZE_FLAG_GROW;
                    }

                    column_node->element.margin = KAN_UI_RECT_PT (16.0f, 16.0f, 0.0f, 0.0f);
                    column_node->local_element_order = (kan_instance_offset_t) column;
                    column_node->layout.layout = KAN_UI_LAYOUT_VERTICAL_CONTAINER;
                    const kan_instance_size_t button_type_count = sizeof (button_images) / sizeof (button_images[0u]);

                    for (kan_loop_size_t button = 0u; button < 6u; ++button)
                    {
                        KAN_UMO_INDEXED_INSERT (button_node, kan_ui_node_t)
                        {
                            button_node->id = kan_next_ui_node_id (ui);
                            button_node->parent_id = column_node->id;

                            if (column == 1u)
                            {
                                button_node->element.width_flags |= KAN_UI_SIZE_FLAG_GROW;
                            }

                            button_node->element.width = KAN_UI_VALUE_VH (button_sizes[button % button_type_count]);
                            button_node->element.height = KAN_UI_VALUE_VH (0.05f);
                            button_node->element.margin = KAN_UI_RECT_PT (16.0f, 16.0f, 16.0f, 16.0f);

                            button_node->element.horizontal_alignment = KAN_UI_HORIZONTAL_ALIGNMENT_CENTER;
                            button_node->element.vertical_alignment = KAN_UI_VERTICAL_ALIGNMENT_CENTER;
                            button_node->local_element_order = (kan_instance_offset_t) button;

                            KAN_UMO_INDEXED_INSERT (button_drawable, kan_ui_node_drawable_t)
                            {
                                button_drawable->id = button_node->id;
                                button_drawable->main_draw_command.type = KAN_UI_DRAW_COMMAND_IMAGE;
                                button_drawable->main_draw_command.image =
                                    KAN_UI_IMAGE_COMMAND_DEFAULT (button_images[button % button_type_count]);
                            }

                            KAN_UMO_INDEXED_INSERT (button_text_node, kan_ui_node_t)
                            {
                                button_text_node->id = kan_next_ui_node_id (ui);
                                button_text_node->parent_id = button_node->id;

                                button_text_node->element.width_flags |= KAN_UI_SIZE_FLAG_GROW;
                                button_text_node->element.horizontal_alignment = KAN_UI_HORIZONTAL_ALIGNMENT_CENTER;
                                button_text_node->element.vertical_alignment = KAN_UI_VERTICAL_ALIGNMENT_CENTER;

                                ADD_TEXT_SHAPING_UNIT (node_text, KAN_TEXT_SHAPING_ALIGNMENT_CENTER,
                                                       button_texts[button % button_type_count])

                                KAN_UMO_INDEXED_INSERT (button_text_drawable, kan_ui_node_drawable_t)
                                {
                                    button_text_drawable->id = button_text_node->id;
                                    button_text_drawable->main_draw_command.type = KAN_UI_DRAW_COMMAND_TEXT;
                                    button_text_drawable->main_draw_command.text.shaping_unit = node_text_id;
                                    button_text_drawable->main_draw_command.text.animation_start_time_s = 0.0f;
                                }

                                KAN_UMO_INDEXED_INSERT (text_behavior, kan_ui_node_text_behavior_t)
                                {
                                    text_behavior->id = button_text_node->id;
                                    text_behavior->shaping_unit = node_text_id;
                                    text_behavior->font_size = KAN_UI_VALUE_PX (24.0f);
                                    text_behavior->sync_text_limit_from_ui = true;
                                    text_behavior->sync_ui_size_from_text_secondary = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    KAN_UMO_INDEXED_INSERT (status_row_node, kan_ui_node_t)
    {
        status_row_node->id = kan_next_ui_node_id (ui);
        status_row_node->element.width_flags |= KAN_UI_SIZE_FLAG_FIT_CHILDREN;
        status_row_node->element.height_flags |= KAN_UI_SIZE_FLAG_FIT_CHILDREN;

        status_row_node->element.horizontal_alignment = KAN_UI_HORIZONTAL_ALIGNMENT_CENTER;
        status_row_node->element.vertical_alignment = KAN_UI_VERTICAL_ALIGNMENT_BOTTOM;
        status_row_node->local_element_order = 1;

        status_row_node->layout.layout = KAN_UI_LAYOUT_HORIZONTAL_CONTAINER;
        status_row_node->render.clip = true;

        for (kan_loop_size_t index = 0u; index < 9u; ++index)
        {
            KAN_UMO_INDEXED_INSERT (status_node, kan_ui_node_t)
            {
                status_node->id = kan_next_ui_node_id (ui);
                status_node->parent_id = status_row_node->id;

                status_node->element.width = KAN_UI_VALUE_VH (0.075f);
                status_node->element.height = KAN_UI_VALUE_VH (0.075f);
                status_node->element.margin = KAN_UI_RECT_PT (3.0f, 3.0f, 3.0f, 3.0f);

                status_node->element.horizontal_alignment = KAN_UI_HORIZONTAL_ALIGNMENT_CENTER;
                status_node->element.vertical_alignment = KAN_UI_VERTICAL_ALIGNMENT_CENTER;
                status_node->local_element_order = (kan_instance_offset_t) index;

                KAN_UMO_INDEXED_INSERT (status_drawable, kan_ui_node_drawable_t)
                {
                    status_drawable->id = status_node->id;
                    status_drawable->main_draw_command.type = KAN_UI_DRAW_COMMAND_IMAGE;
                    status_drawable->main_draw_command.image = KAN_UI_IMAGE_COMMAND_DEFAULT (image_shield);
                }
            }
        }
    }

    KAN_UMO_INDEXED_INSERT (tiled_column_node, kan_ui_node_t)
    {
        tiled_column_node->id = kan_next_ui_node_id (ui);
        tiled_column_node->element.width_flags |= KAN_UI_SIZE_FLAG_FIT_CHILDREN;
        tiled_column_node->element.height_flags |= KAN_UI_SIZE_FLAG_FIT_CHILDREN;

        tiled_column_node->element.horizontal_alignment = KAN_UI_HORIZONTAL_ALIGNMENT_RIGHT;
        tiled_column_node->element.vertical_alignment = KAN_UI_VERTICAL_ALIGNMENT_TOP;
        tiled_column_node->local_element_order = -1; // Below window.

        tiled_column_node->layout.layout = KAN_UI_LAYOUT_VERTICAL_CONTAINER;
        tiled_column_node->render.clip = true;

        const float widths[] = {0.8f, 0.4f, 0.2f};
        const float heights[] = {0.2f, 0.1f, 0.4f};

        for (kan_loop_size_t index = 0u; index < sizeof (widths) / sizeof (widths[0u]); ++index)
        {
            KAN_UMO_INDEXED_INSERT (tiled_node, kan_ui_node_t)
            {
                tiled_node->id = kan_next_ui_node_id (ui);
                tiled_node->parent_id = tiled_column_node->id;

                tiled_node->element.width = KAN_UI_VALUE_VH (widths[index]);
                tiled_node->element.height = KAN_UI_VALUE_VH (heights[index]);
                tiled_node->element.margin = KAN_UI_RECT_PT (3.0f, 3.0f, 32.0f, 32.0f);

                tiled_node->element.horizontal_alignment = KAN_UI_HORIZONTAL_ALIGNMENT_RIGHT;
                tiled_node->element.vertical_alignment = KAN_UI_VERTICAL_ALIGNMENT_TOP;
                tiled_node->local_element_order = (kan_instance_offset_t) index;

                KAN_UMO_INDEXED_INSERT (tiled_drawable, kan_ui_node_drawable_t)
                {
                    tiled_drawable->id = tiled_node->id;
                    tiled_drawable->main_draw_command.type = KAN_UI_DRAW_COMMAND_IMAGE;
                    tiled_drawable->main_draw_command.image = KAN_UI_IMAGE_COMMAND_DEFAULT (image_tiled_slice);
                }
            }
        }
    };

    singleton->should_rebuild = false;
}

APPLICATION_FRAMEWORK_EXAMPLES_UI_RENDER_API KAN_UM_MUTATOR_EXECUTE (ui_example_render_update)
{
    // Write access just to write color table values once, okay for the example.
    KAN_UMI_SINGLETON_WRITE (render_context, kan_render_context_singleton_t)
    KAN_UMI_SINGLETON_READ (resource_provider, kan_resource_provider_singleton_t)
    KAN_UMI_SINGLETON_WRITE (test, test_routine_singleton_t)
    KAN_UMI_SINGLETON_WRITE (singleton, example_ui_render_singleton_t)

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
                                                                         "application_framework_example_ui_render",
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
            KAN_LOG (application_framework_example_ui_render, KAN_LOG_ERROR, "Failed to create surface.")
            kan_application_framework_system_request_exit (state->application_framework_system_handle, 1);
            return;
        }

        kan_application_system_window_raise (state->application_system_handle, singleton->window_handle);

        // Initialize locale here as well, because this will only be executed once.
        KAN_UMI_SINGLETON_WRITE (locale, kan_locale_singleton_t)
        locale->selected_locale = kan_string_intern ("en");

        KAN_UMI_SINGLETON_WRITE (ui_render_graph, kan_ui_render_graph_singleton_t)
        ui_render_graph->clear_color = kan_color_linear_to_srgb (kan_make_color_linear (0.0f, 1.0f, 1.0f, 1.0f));

        render_context->color_table_values_dirty = true;
        render_context->color_table_values.size = 0u;
        kan_dynamic_array_set_capacity (&render_context->color_table_values, 3u);

        struct kan_color_linear_t *color_negative = kan_dynamic_array_add_last (&render_context->color_table_values);
        struct kan_color_linear_t *color_positive = kan_dynamic_array_add_last (&render_context->color_table_values);
        struct kan_color_linear_t *color_neutral = kan_dynamic_array_add_last (&render_context->color_table_values);

        *color_negative = kan_make_color_linear (0.8f, 0.0f, 0.0f, 1.0);
        *color_positive = kan_make_color_linear (0.8f, 0.8f, 0.0f, 1.0f);
        *color_neutral = kan_make_color_linear (0.0f, 0.8f, 0.8f, 1.0f);
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

struct ui_example_render_post_render_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (ui_example_render_post_render)
    KAN_UM_BIND_STATE (ui_example_render_post_render, state)
    kan_context_system_t application_system_handle;
};

APPLICATION_FRAMEWORK_EXAMPLES_UI_RENDER_API KAN_UM_MUTATOR_DEPLOY (ui_example_render_post_render)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_context_t context = kan_universe_get_context (universe);
    state->application_system_handle = kan_context_query (context, KAN_CONTEXT_APPLICATION_SYSTEM_NAME);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_UI_RENDER_END_CHECKPOINT);
}

APPLICATION_FRAMEWORK_EXAMPLES_UI_RENDER_API KAN_UM_MUTATOR_EXECUTE (ui_example_render_post_render)
{
    KAN_UMI_SINGLETON_READ (ui_render_graph, kan_ui_render_graph_singleton_t)
    KAN_UMI_SINGLETON_READ (resource_provider, kan_resource_provider_singleton_t)
    KAN_UMI_SINGLETON_WRITE (test, test_routine_singleton_t)
    KAN_UMI_SINGLETON_WRITE (singleton, example_ui_render_singleton_t)

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

        if (singleton->rendered_frames >= 5u && test->test_mode_enabled &&
            test->expectation_read_back_statuses.size == 0u)
        {
            kan_dynamic_array_set_capacity (&test->expectation_read_back_statuses, 1u);
            *(kan_render_read_back_status_t *) kan_dynamic_array_add_last (&test->expectation_read_back_statuses) =
                kan_render_request_read_back_from_image (ui_render_graph->final_image, 0u, 0u,
                                                         test->expectation_read_back_buffer, 0u,
                                                         ui_render_graph->final_pass_instance);
        }

        // We don't want to capture first available frames, as proper text shaping and positioning might not be ready
        // by that time, therefore text would be invisible and test will fail.
        ++singleton->rendered_frames;
    }
}
