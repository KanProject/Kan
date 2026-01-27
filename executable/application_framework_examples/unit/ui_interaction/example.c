#define _CRT_SECURE_NO_WARNINGS __CUSHION_PRESERVE__

#include <application_framework_examples_ui_interaction_api.h>

#include <string.h>

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
#include <kan/universe_ui/macro.h>

KAN_LOG_DEFINE_CATEGORY (application_framework_example_ui_interaction);
KAN_USE_STATIC_INTERNED_IDS

#define KAN_UI_INTERACTION_MUTATOR_GROUP "ui_example_interaction"
KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (ui_example_interaction_update)
KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (ui_example_interaction_post_render)
APPLICATION_FRAMEWORK_EXAMPLES_UI_INTERACTION_API KAN_UM_MUTATOR_GROUP_META (ui_interaction,
                                                                             KAN_UI_INTERACTION_MUTATOR_GROUP);

#define TEST_WIDTH 800u
#define TEST_HEIGHT 800u

enum example_ui_interaction_test_stage_t
{
    EXAMPLE_UI_INTERACTION_TEST_STAGE_START = 0u,
    EXAMPLE_UI_INTERACTION_TEST_STAGE_HOVER_CHECK_1,
    EXAMPLE_UI_INTERACTION_TEST_STAGE_PRESS_CHECK,
    EXAMPLE_UI_INTERACTION_TEST_STAGE_HOVER_CHECK_2,
    EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_SCROLL,
    EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_TEXT_SELECTION,
    EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_TEXT_INPUT_1,
    EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_TEXT_CURSOR_CLICK,
    EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_TEXT_INPUT_2,
    EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_TEXT_RIGHT,
    EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_TEXT_BACKSPACE,
    EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_TEXT_LEFT,
    EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_TEXT_DELETE,
    EXAMPLE_UI_INTERACTION_TEST_STAGE_FINISHED,
    EXAMPLE_UI_INTERACTION_TEST_STAGE_ERRED,
};

struct example_ui_interaction_singleton_t
{
    kan_application_system_window_t window_handle;
    kan_render_surface_t window_surface;
    bool should_rebuild;
    enum example_ui_interaction_test_stage_t test_stage;

    kan_ui_node_id_t second_button_in_list_id;
    kan_ui_node_id_t scroll_container_id;
    kan_ui_node_id_t line_edit_id;

    float test_last_mouse_x;
    float test_last_mouse_y;
};

APPLICATION_FRAMEWORK_EXAMPLES_UI_INTERACTION_API void example_ui_interaction_singleton_init (
    struct example_ui_interaction_singleton_t *instance)
{
    instance->window_handle = KAN_HANDLE_SET_INVALID (kan_application_system_window_t);
    instance->window_surface = KAN_HANDLE_SET_INVALID (kan_render_surface_t);
    instance->should_rebuild = false;
    instance->test_stage = EXAMPLE_UI_INTERACTION_TEST_STAGE_START;

    instance->second_button_in_list_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    instance->scroll_container_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);
    instance->line_edit_id = KAN_TYPED_ID_32_SET_INVALID (kan_ui_node_id_t);

    instance->test_last_mouse_x = 0.0f;
    instance->test_last_mouse_y = 0.0f;
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

    const kan_instance_size_t image_window =
        kan_render_atlas_loaded_query (atlas, KAN_STATIC_INTERNED_ID_GET (window), NULL);

    const kan_instance_size_t image_scroll_line =
        kan_render_atlas_loaded_query (atlas, KAN_STATIC_INTERNED_ID_GET (scroll_line_background), NULL);

    const kan_instance_size_t image_text_edit_cursor =
        kan_render_atlas_loaded_query (atlas, KAN_STATIC_INTERNED_ID_GET (text_edit_cursor), NULL);

    const kan_instance_size_t image_text_edit_selection =
        kan_render_atlas_loaded_query (atlas, KAN_STATIC_INTERNED_ID_GET (text_edit_selection), NULL);

    KAN_UIM_CLEAR_EVERYTHING
    KAN_UMI_SINGLETON_READ (ui, kan_ui_singleton_t)
    KAN_UIM_PREPARE_ROOT;

    const uint32_t default_text_mark = KAN_UI_DEFAULT_TEXT_MAKE_MARK (0u, KAN_UI_DEFAULT_TEXT_MARK_FLAG_OUTLINE);
    const float outline_enlarge_factor = 1.0625f;

    KAN_UIM_WIDGET_IMAGE (left_window, KAN_UI_IMAGE_COMMAND_DEFAULT (image_window));
    KAN_UIM_HIT_BOX_BLOCKING (left_window);

    left_window_node->element.width = KAN_UI_VALUE_VW (0.5f);
    left_window_node->element.height = KAN_UI_VALUE_VH (0.4f);

    left_window_node->element.horizontal_alignment = KAN_UI_HORIZONTAL_ALIGNMENT_LEFT;
    left_window_node->element.vertical_alignment = KAN_UI_VERTICAL_ALIGNMENT_CENTER;

    left_window_node->layout.layout = KAN_UI_LAYOUT_VERTICAL_CONTAINER;
    left_window_node->layout.padding = KAN_UI_RECT_PT (16.0f, 16.0f, 16.0f, 16.0f);
    left_window_node->render.clip = true;

    KAN_UIM_CHILDREN (left_window)
    {
        for (kan_loop_size_t button = 0u; button < 6u; ++button)
        {
            KAN_NEW_TEXT_SHAPING_UNIT_FROM_LITERAL (button, "Hello, world!", default_text_mark);
            button_shaping_unit->request.alignment = KAN_TEXT_SHAPING_ALIGNMENT_CENTER;

            KAN_UIM_WIDGET_TEXT_BUTTON (button, KAN_STATIC_INTERNED_ID_GET (default), KAN_UI_VALUE_PT (24.0f),
                                        outline_enlarge_factor);

            if (button == 1u)
            {
                singleton->second_button_in_list_id = button_node->id;
            }

            button_node->element.width_flags |= KAN_UI_SIZE_FLAG_GROW;
            button_node->element.height = KAN_UI_VALUE_VH (0.05f);
            button_node->element.margin = KAN_UI_RECT_PT (16.0f, 16.0f, 16.0f, 16.0f);

            button_node->element.horizontal_alignment = KAN_UI_HORIZONTAL_ALIGNMENT_CENTER;
            button_node->element.vertical_alignment = KAN_UI_VERTICAL_ALIGNMENT_CENTER;
        }
    }

    KAN_UIM_WIDGET_IMAGE (center_window, KAN_UI_IMAGE_COMMAND_DEFAULT (image_window));
    KAN_UIM_HIT_BOX_BLOCKING (center_window);

    center_window_node->element.width = KAN_UI_VALUE_VW (0.5f);
    center_window_node->element.height = KAN_UI_VALUE_VH (0.5f);

    center_window_node->element.horizontal_alignment = KAN_UI_HORIZONTAL_ALIGNMENT_CENTER;
    center_window_node->element.vertical_alignment = KAN_UI_VERTICAL_ALIGNMENT_CENTER;
    center_window_node->local_element_order = 1;

    center_window_node->layout.layout = KAN_UI_LAYOUT_VERTICAL_CONTAINER;
    center_window_node->layout.padding = KAN_UI_RECT_PT (16.0f, 16.0f, 16.0f, 16.0f);
    center_window_node->render.clip = true;

    KAN_UIM_CHILDREN (center_window)
    {
        KAN_UIM_WIDGET_SCROLL_PANE (info);
        info_outer_node->element.width_flags = KAN_UI_SIZE_FLAG_GROW;
        info_outer_node->element.height_flags = KAN_UI_SIZE_FLAG_GROW;

        info_container_node->layout.layout = KAN_UI_LAYOUT_VERTICAL_CONTAINER;
        singleton->scroll_container_id = info_container_node->id;

        KAN_UIM_WIDGET_SCROLL_PANE_ENABLE_VERTICAL (info);
        KAN_UIM_WIDGET_SCROLL_PANE_VERTICAL_LINE (info, KAN_STATIC_INTERNED_ID_GET (default),
                                                  KAN_UI_IMAGE_COMMAND_DEFAULT (image_scroll_line),
                                                  KAN_UI_VALUE_PT (32.0f));

        KAN_UIM_CHILDREN (info_container)
        {
            const char *big_text =
                "Robert Guiscard also referred to as Robert de Hauteville, was a Norman adventurer remembered for "
                "his conquest of southern Italy and Sicily in the 11th century.\n"
                "\n"
                "Robert was born into the Hauteville family in Normandy, the sixth son of Tancred de Hauteville "
                "and his wife Fressenda. He inherited the County of Apulia and Calabria from his brother in 1057, "
                "and in 1059 he was made Duke of Apulia and Calabria and Lord of Sicily by Pope Nicholas II. He "
                "was also briefly Prince of Benevento (1078–1081), before returning the title to the papacy.";

            KAN_NEW_TEXT_SHAPING_UNIT_FROM_LITERAL (big_text, big_text, default_text_mark);
            big_text_shaping_unit->request.alignment = KAN_TEXT_SHAPING_ALIGNMENT_CENTER;

            KAN_UIM_WIDGET_TEXT (big_text, KAN_UI_VALUE_PT (36.0f));
            big_text_node->element.width_flags |= KAN_UI_SIZE_FLAG_GROW;
            big_text_node->element.horizontal_alignment = KAN_UI_HORIZONTAL_ALIGNMENT_CENTER;
            big_text_node->element.vertical_alignment = KAN_UI_VERTICAL_ALIGNMENT_CENTER;
            big_text_node->element.margin = KAN_UI_RECT_PT (16.0f, 16.0f, 16.0f, 16.0f);

            KAN_UIM_WIDGET_LINE_EDIT (input, KAN_UI_VALUE_PT (36.0f), KAN_STATIC_INTERNED_ID_GET (line_edit_regular),
                                      KAN_STATIC_INTERNED_ID_GET (line_edit_selected));

            singleton->line_edit_id = input_node->id;
            input_node->element.width_flags |= KAN_UI_SIZE_FLAG_GROW;
            input_node->element.horizontal_alignment = KAN_UI_HORIZONTAL_ALIGNMENT_CENTER;
            input_node->element.vertical_alignment = KAN_UI_VERTICAL_ALIGNMENT_CENTER;
            input_node->element.margin = KAN_UI_RECT_PT (16.0f, 16.0f, 16.0f, 16.0f);
            input_node->layout.padding = KAN_UI_RECT_PT (4.0f, 4.0f, 4.0f, 4.0f);

            input_shaping_unit->request.alignment = KAN_TEXT_SHAPING_ALIGNMENT_CENTER;
            input_line_edit_behavior->cursor_image_index = image_text_edit_cursor;
            input_line_edit_behavior->cursor_width = KAN_UI_VALUE_PT (3.0f);
            input_line_edit_behavior->selection_image_index = image_text_edit_selection;

            kan_ui_node_line_edit_behavior_set_content (
                input_line_edit_behavior, "Hello, world!", NULL,
                KAN_UI_DEFAULT_TEXT_MAKE_MARK (0u, KAN_UI_DEFAULT_TEXT_MARK_FLAG_OUTLINE));

            input_inner_node->element.width_flags |= KAN_UI_SIZE_FLAG_GROW;
            input_inner_node->element.height_flags |= KAN_UI_SIZE_FLAG_GROW;

            KAN_UIM_WIDGET_LINE_EDIT_CALCULATE_LEEWAY (input, outline_enlarge_factor);
            KAN_UIM_WIDGET_LINE_EDIT_CALCULATE_HEIGHT (input, outline_enlarge_factor);
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
        ui_render_graph->clear_color = kan_make_color_linear (0.0f, 1.0f, 1.0f, 1.0f);
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
    KAN_UMI_SINGLETON_READ (ui_input, kan_ui_input_singleton_t)
    KAN_UMI_SINGLETON_READ (ui_render_graph, kan_ui_render_graph_singleton_t)
    KAN_UMI_SINGLETON_READ (resource_provider, kan_resource_provider_singleton_t)
    KAN_UMI_SINGLETON_WRITE (test, test_routine_singleton_t)
    KAN_UMI_SINGLETON_WRITE (singleton, example_ui_interaction_singleton_t)

    const struct kan_application_system_window_info_t *window_info =
        kan_application_system_get_window_info_from_handle (state->application_system_handle, singleton->window_handle);

    if (KAN_HANDLE_IS_VALID (ui_render_graph->final_pass_instance) && KAN_HANDLE_IS_VALID (singleton->window_surface))
    {
        struct kan_render_integer_region_2d_t region = {
            .x = 0,
            .y = 0,
            .width = window_info->width_for_render,
            .height = window_info->height_for_render,
        };

        kan_render_backend_system_present_image_on_surface (singleton->window_surface, ui_render_graph->final_image, 0u,
                                                            region, region, ui_render_graph->final_pass_instance);
    }

    if (!test->test_mode_enabled || singleton->should_rebuild)
    {
        return;
    }

#define TEST_PUSH_EVENT(...)                                                                                           \
    {                                                                                                                  \
        struct kan_platform_application_event_t event = __VA_ARGS__;                                                   \
        kan_application_system_push_fake_event (state->application_system_handle, &event);                             \
    }

#define TEST_MOUSE_MOVE(TO_X, TO_Y)                                                                                    \
    TEST_PUSH_EVENT ({                                                                                                 \
        .type = KAN_PLATFORM_APPLICATION_EVENT_TYPE_MOUSE_MOTION,                                                      \
        .time_ns = 0u,                                                                                                 \
        .mouse_motion =                                                                                                \
            {                                                                                                          \
                .window_id = window_info->id,                                                                          \
                .button_state = 0u,                                                                                    \
                .window_x = (TO_X),                                                                                    \
                .window_y = (TO_Y),                                                                                    \
                .window_x_relative = singleton->test_last_mouse_x - (TO_X),                                            \
                .window_y_relative = singleton->test_last_mouse_y - (TO_Y),                                            \
            },                                                                                                         \
    })                                                                                                                 \
    singleton->test_last_mouse_x = TO_X;                                                                               \
    singleton->test_last_mouse_y = TO_Y;

#define TEST_MOUSE_DOWN(BUTTON)                                                                                        \
    TEST_PUSH_EVENT ({                                                                                                 \
        .type = KAN_PLATFORM_APPLICATION_EVENT_TYPE_MOUSE_BUTTON_DOWN,                                                 \
        .time_ns = 0u,                                                                                                 \
        .mouse_button =                                                                                                \
            {                                                                                                          \
                .window_id = window_info->id,                                                                          \
                .button = BUTTON,                                                                                      \
                .clicks = 1u,                                                                                          \
                .window_x = singleton->test_last_mouse_x,                                                              \
                .window_y = singleton->test_last_mouse_y,                                                              \
            },                                                                                                         \
    })

#define TEST_MOUSE_UP(BUTTON)                                                                                          \
    TEST_PUSH_EVENT ({                                                                                                 \
        .type = KAN_PLATFORM_APPLICATION_EVENT_TYPE_MOUSE_BUTTON_UP,                                                   \
        .time_ns = 0u,                                                                                                 \
        .mouse_button =                                                                                                \
            {                                                                                                          \
                .window_id = window_info->id,                                                                          \
                .button = BUTTON,                                                                                      \
                .clicks = 1u,                                                                                          \
                .window_x = singleton->test_last_mouse_x,                                                              \
                .window_y = singleton->test_last_mouse_y,                                                              \
            },                                                                                                         \
    })

#define TEST_INPUT_TEXT(LITERAL)                                                                                       \
    {                                                                                                                  \
        char *event_text = kan_allocate_general (kan_platform_application_get_events_allocation_group (),              \
                                                 sizeof (LITERAL), alignof (char));                                    \
        memcpy (event_text, LITERAL, sizeof (LITERAL));                                                                \
                                                                                                                       \
        TEST_PUSH_EVENT ({                                                                                             \
            .type = KAN_PLATFORM_APPLICATION_EVENT_TYPE_TEXT_INPUT,                                                    \
            .time_ns = 0u,                                                                                             \
            .text_input =                                                                                              \
                {                                                                                                      \
                    .window_id = window_info->id,                                                                      \
                    .text = event_text,                                                                                \
                },                                                                                                     \
        })                                                                                                             \
    }

#define TEST_KEY_DOWN(SCAN, KEY)                                                                                       \
    TEST_PUSH_EVENT ({                                                                                                 \
        .type = KAN_PLATFORM_APPLICATION_EVENT_TYPE_KEY_DOWN,                                                          \
        .time_ns = 0u,                                                                                                 \
        .keyboard =                                                                                                    \
            {                                                                                                          \
                .window_id = window_info->id,                                                                          \
                .repeat = false,                                                                                       \
                .scan_code = SCAN,                                                                                     \
                .key_code = KEY,                                                                                       \
                .modifiers = 0u,                                                                                       \
            },                                                                                                         \
    })

#define TEST_KEY_UP(SCAN, KEY)                                                                                         \
    TEST_PUSH_EVENT ({                                                                                                 \
        .type = KAN_PLATFORM_APPLICATION_EVENT_TYPE_KEY_UP,                                                            \
        .time_ns = 0u,                                                                                                 \
        .keyboard =                                                                                                    \
            {                                                                                                          \
                .window_id = window_info->id,                                                                          \
                .repeat = false,                                                                                       \
                .scan_code = SCAN,                                                                                     \
                .key_code = KEY,                                                                                       \
                .modifiers = 0u,                                                                                       \
            },                                                                                                         \
    })

#define TEST_CHECK_EXPECTATION(...)                                                                                    \
    if (!(__VA_ARGS__))                                                                                                \
    {                                                                                                                  \
        KAN_LOG (application_framework_example_ui_interaction, KAN_LOG_ERROR, "Test expectation failed: %s",           \
                 #__VA_ARGS__);                                                                                        \
        test->manual_result = TEST_ROUTINE_MANUAL_RESULT_FAILED;                                                       \
        singleton->test_stage = EXAMPLE_UI_INTERACTION_TEST_STAGE_ERRED;                                               \
    }

    switch (singleton->test_stage)
    {
    case EXAMPLE_UI_INTERACTION_TEST_STAGE_START:
        singleton->test_stage = EXAMPLE_UI_INTERACTION_TEST_STAGE_HOVER_CHECK_1;
        TEST_MOUSE_MOVE (120.0f, 340.0f)
        break;

    case EXAMPLE_UI_INTERACTION_TEST_STAGE_HOVER_CHECK_1:
        singleton->test_stage = EXAMPLE_UI_INTERACTION_TEST_STAGE_PRESS_CHECK;

        TEST_CHECK_EXPECTATION (
            KAN_TYPED_ID_32_IS_EQUAL (ui_input->current_hovered_id, singleton->second_button_in_list_id))

        TEST_MOUSE_DOWN (KAN_PLATFORM_MOUSE_BUTTON_LEFT)
        TEST_MOUSE_UP (KAN_PLATFORM_MOUSE_BUTTON_LEFT)
        break;

    case EXAMPLE_UI_INTERACTION_TEST_STAGE_PRESS_CHECK:
    {
        singleton->test_stage = EXAMPLE_UI_INTERACTION_TEST_STAGE_HOVER_CHECK_2;
        bool press_begin_detected = false;
        bool press_end_detected = false;

        KAN_UML_EVENT_FETCH (begin_event, kan_ui_press_begin_t)
        {
            TEST_CHECK_EXPECTATION (
                KAN_TYPED_ID_32_IS_EQUAL (begin_event->node_id, singleton->second_button_in_list_id))
            TEST_CHECK_EXPECTATION (begin_event->mouse_button_down_flags == (1u << KAN_PLATFORM_MOUSE_BUTTON_LEFT))
            TEST_CHECK_EXPECTATION (begin_event->at_x == 120)
            TEST_CHECK_EXPECTATION (begin_event->at_y == 340)
            press_begin_detected = true;
        }

        KAN_UML_EVENT_FETCH (end_event, kan_ui_press_end_t)
        {
            TEST_CHECK_EXPECTATION (KAN_TYPED_ID_32_IS_EQUAL (end_event->node_id, singleton->second_button_in_list_id))
            TEST_CHECK_EXPECTATION (end_event->mouse_button_down_inclusive_flags ==
                                    (1u << KAN_PLATFORM_MOUSE_BUTTON_LEFT))
            TEST_CHECK_EXPECTATION (end_event->continuous_press)
            TEST_CHECK_EXPECTATION (end_event->at_x == 120)
            TEST_CHECK_EXPECTATION (end_event->at_y == 340)
            press_end_detected = true;
        }

        TEST_CHECK_EXPECTATION (press_begin_detected)
        TEST_CHECK_EXPECTATION (press_end_detected)

        TEST_MOUSE_MOVE (240.0f, 340.0f)
        break;
    }

    case EXAMPLE_UI_INTERACTION_TEST_STAGE_HOVER_CHECK_2:
    {
        singleton->test_stage = EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_SCROLL;
        TEST_CHECK_EXPECTATION (!KAN_TYPED_ID_32_IS_VALID (ui_input->current_hovered_id))

        KAN_UMI_VALUE_READ_REQUIRED (scroll_container_node, kan_ui_node_t, id, &singleton->scroll_container_id)
        TEST_CHECK_EXPECTATION (scroll_container_node->render.scroll_y.type == KAN_UI_PT)
        TEST_CHECK_EXPECTATION (KAN_FLOATING_IS_NEAR (scroll_container_node->render.scroll_y.value, 0.0f))

        TEST_MOUSE_MOVE (575.0f, 240.0f)
        TEST_MOUSE_DOWN (KAN_PLATFORM_MOUSE_BUTTON_LEFT)
        TEST_MOUSE_MOVE (575.0f, 600.0f)
        TEST_MOUSE_UP (KAN_PLATFORM_MOUSE_BUTTON_LEFT)
        break;
    }

    case EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_SCROLL:
    {
        singleton->test_stage = EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_TEXT_SELECTION;
        KAN_UMI_VALUE_READ_REQUIRED (scroll_container_node, kan_ui_node_t, id, &singleton->scroll_container_id)
        TEST_CHECK_EXPECTATION (scroll_container_node->render.scroll_y.type == KAN_UI_PT)
        TEST_CHECK_EXPECTATION (KAN_FLOATING_IS_NEAR (scroll_container_node->render.scroll_y.value, 1207.0f))

        KAN_UMI_VALUE_READ_REQUIRED (behavior, kan_ui_node_line_edit_behavior_t, id, &singleton->line_edit_id)
        TEST_CHECK_EXPECTATION (behavior->cursor_content_location == KAN_INT_MAX (kan_instance_size_t))
        TEST_CHECK_EXPECTATION (behavior->selection_content_min == KAN_INT_MAX (kan_instance_size_t))
        TEST_CHECK_EXPECTATION (behavior->selection_content_max == KAN_INT_MAX (kan_instance_size_t))

        TEST_MOUSE_MOVE (290.0f, 565.0f)
        TEST_MOUSE_DOWN (KAN_PLATFORM_MOUSE_BUTTON_LEFT)
        TEST_MOUSE_MOVE (378.0f, 565.0f)
        TEST_MOUSE_UP (KAN_PLATFORM_MOUSE_BUTTON_LEFT)
        break;
    }

    case EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_TEXT_SELECTION:
    {
        singleton->test_stage = EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_TEXT_INPUT_1;
        KAN_UMI_VALUE_READ_REQUIRED (behavior, kan_ui_node_line_edit_behavior_t, id, &singleton->line_edit_id)
        TEST_CHECK_EXPECTATION (behavior->cursor_content_location == KAN_INT_MAX (kan_instance_size_t))
        TEST_CHECK_EXPECTATION (behavior->selection_content_min == 0u)
        TEST_CHECK_EXPECTATION (behavior->selection_content_max == 5u)
        TEST_INPUT_TEXT ("Дратуте")
        break;
    }

    case EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_TEXT_INPUT_1:
    {
        singleton->test_stage = EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_TEXT_CURSOR_CLICK;
        KAN_UMI_VALUE_READ_REQUIRED (behavior, kan_ui_node_line_edit_behavior_t, id, &singleton->line_edit_id)
        TEST_CHECK_EXPECTATION (behavior->cursor_content_location == 14u)
        TEST_CHECK_EXPECTATION (behavior->selection_content_min == KAN_INT_MAX (kan_instance_size_t))
        TEST_CHECK_EXPECTATION (behavior->selection_content_max == KAN_INT_MAX (kan_instance_size_t))
        TEST_CHECK_EXPECTATION (strcmp ("Дратуте, world!", (char *) behavior->content_utf8.data) == 0)

        TEST_MOUSE_MOVE (526.0f, 565.0f)
        TEST_MOUSE_DOWN (KAN_PLATFORM_MOUSE_BUTTON_LEFT)
        TEST_MOUSE_UP (KAN_PLATFORM_MOUSE_BUTTON_LEFT)
        break;
    }

    case EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_TEXT_CURSOR_CLICK:
    {
        singleton->test_stage = EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_TEXT_INPUT_2;
        KAN_UMI_VALUE_READ_REQUIRED (behavior, kan_ui_node_line_edit_behavior_t, id, &singleton->line_edit_id)
        TEST_CHECK_EXPECTATION (behavior->cursor_content_location == 21u)
        TEST_CHECK_EXPECTATION (behavior->selection_content_min == KAN_INT_MAX (kan_instance_size_t))
        TEST_CHECK_EXPECTATION (behavior->selection_content_max == KAN_INT_MAX (kan_instance_size_t))
        TEST_INPUT_TEXT (" бяБЯбя")
        break;
    }

    case EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_TEXT_INPUT_2:
    {
        singleton->test_stage = EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_TEXT_RIGHT;
        KAN_UMI_VALUE_READ_REQUIRED (behavior, kan_ui_node_line_edit_behavior_t, id, &singleton->line_edit_id)
        TEST_CHECK_EXPECTATION (behavior->cursor_content_location == 34u)
        TEST_CHECK_EXPECTATION (behavior->selection_content_min == KAN_INT_MAX (kan_instance_size_t))
        TEST_CHECK_EXPECTATION (behavior->selection_content_max == KAN_INT_MAX (kan_instance_size_t))
        TEST_CHECK_EXPECTATION (strcmp ("Дратуте, world бяБЯбя!", (char *) behavior->content_utf8.data) == 0)

        TEST_KEY_DOWN (KAN_PLATFORM_SCAN_CODE_RIGHT, 0u)
        TEST_KEY_UP (KAN_PLATFORM_SCAN_CODE_RIGHT, 0u)
        TEST_KEY_DOWN (KAN_PLATFORM_SCAN_CODE_RIGHT, 0u)
        TEST_KEY_UP (KAN_PLATFORM_SCAN_CODE_RIGHT, 0u)
        break;
    }

    case EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_TEXT_RIGHT:
    {
        singleton->test_stage = EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_TEXT_BACKSPACE;
        KAN_UMI_VALUE_READ_REQUIRED (behavior, kan_ui_node_line_edit_behavior_t, id, &singleton->line_edit_id)
        TEST_CHECK_EXPECTATION (behavior->cursor_content_location == 35u)

        TEST_KEY_DOWN (KAN_PLATFORM_SCAN_CODE_BACKSPACE, 0u)
        TEST_KEY_UP (KAN_PLATFORM_SCAN_CODE_BACKSPACE, 0u)
        break;
    }

    case EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_TEXT_BACKSPACE:
    {
        singleton->test_stage = EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_TEXT_LEFT;
        KAN_UMI_VALUE_READ_REQUIRED (behavior, kan_ui_node_line_edit_behavior_t, id, &singleton->line_edit_id)
        TEST_CHECK_EXPECTATION (behavior->cursor_content_location == 34u)
        TEST_CHECK_EXPECTATION (strcmp ("Дратуте, world бяБЯбя", (char *) behavior->content_utf8.data) == 0)

        TEST_KEY_DOWN (KAN_PLATFORM_SCAN_CODE_LEFT, 0u)
        TEST_KEY_UP (KAN_PLATFORM_SCAN_CODE_LEFT, 0u)
        TEST_KEY_DOWN (KAN_PLATFORM_SCAN_CODE_LEFT, 0u)
        TEST_KEY_UP (KAN_PLATFORM_SCAN_CODE_LEFT, 0u)
        break;
    }

    case EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_TEXT_LEFT:
    {
        singleton->test_stage = EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_TEXT_DELETE;
        KAN_UMI_VALUE_READ_REQUIRED (behavior, kan_ui_node_line_edit_behavior_t, id, &singleton->line_edit_id)
        TEST_CHECK_EXPECTATION (behavior->cursor_content_location == 30u)

        TEST_KEY_DOWN (KAN_PLATFORM_SCAN_CODE_DELETE, 0u)
        TEST_KEY_UP (KAN_PLATFORM_SCAN_CODE_DELETE, 0u)
        break;
    }

    case EXAMPLE_UI_INTERACTION_TEST_STAGE_CHECK_TEXT_DELETE:
    {
        singleton->test_stage = EXAMPLE_UI_INTERACTION_TEST_STAGE_FINISHED;
        KAN_UMI_VALUE_READ_REQUIRED (behavior, kan_ui_node_line_edit_behavior_t, id, &singleton->line_edit_id)
        TEST_CHECK_EXPECTATION (behavior->cursor_content_location == 30u)
        TEST_CHECK_EXPECTATION (strcmp ("Дратуте, world бяБЯя", (char *) behavior->content_utf8.data) == 0)
        break;
    }

    case EXAMPLE_UI_INTERACTION_TEST_STAGE_FINISHED:
        test->manual_result = TEST_ROUTINE_MANUAL_RESULT_PASSED;
        break;

    case EXAMPLE_UI_INTERACTION_TEST_STAGE_ERRED:
        break;
    }

#undef TEST_PUSH_EVENT
#undef TEST_MOUSE_MOVE
#undef TEST_MOUSE_DOWN
#undef TEST_MOUSE_UP
#undef TEST_CHECK_EXPECTATION
}
