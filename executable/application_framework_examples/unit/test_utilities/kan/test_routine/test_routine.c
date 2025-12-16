#include <string.h>

#include <kan/file_system/path_container.h>
#include <kan/file_system/stream.h>
#include <kan/image/image.h>
#include <kan/log/logging.h>
#include <kan/test_expectation/test_expectation.h>
#include <kan/test_routine/test_routine.h>
#include <kan/universe_render_foundation/render_graph.h>

KAN_USE_STATIC_INTERNED_IDS
KAN_LOG_DEFINE_CATEGORY (application_framework_example_test_routine);

static inline bool check_is_test_mode (kan_context_system_t application_framework_system_handle)
{
    if (KAN_HANDLE_IS_VALID (application_framework_system_handle))
    {
        const kan_instance_size_t arguments_count =
            kan_application_framework_system_get_arguments_count (application_framework_system_handle);
        char **arguments = kan_application_framework_system_get_arguments (application_framework_system_handle);

        for (kan_loop_size_t index = 1u; index < arguments_count; ++index)
        {
            if (strcmp (arguments[index], "--test") == 0)
            {
                return true;
            }
        }
    }

    return false;
}

struct test_routine_private_singleton_t
{
    kan_interned_string_t setup_name;
    kan_resource_usage_id_t setup_usage_id;
    bool setup_read;
    kan_instance_size_t frames_count;
};

TEST_ROUTINE_API void test_routine_private_singleton_init (struct test_routine_private_singleton_t *instance)
{
    instance->setup_name = NULL;
    instance->setup_usage_id = KAN_TYPED_ID_32_SET_INVALID (kan_resource_usage_id_t);
    instance->setup_read = false;
    instance->frames_count = 0u;
}

struct test_routine_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (test_routine)
    KAN_UM_BIND_STATE (test_routine, state)

    kan_context_system_t application_system_handle;
    kan_context_system_t application_framework_system_handle;
    kan_context_system_t render_backend_system_handle;
    bool executed_at_least_once;
};

TEST_ROUTINE_API KAN_UM_MUTATOR_DEPLOY (test_routine)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_context_t context = kan_universe_get_context (universe);

    state->application_system_handle = kan_context_query (context, KAN_CONTEXT_APPLICATION_SYSTEM_NAME);
    state->application_framework_system_handle =
        kan_context_query (context, KAN_CONTEXT_APPLICATION_FRAMEWORK_SYSTEM_NAME);
    state->render_backend_system_handle = kan_context_query (context, KAN_CONTEXT_RENDER_BACKEND_SYSTEM_NAME);

    state->executed_at_least_once = false;
    kan_workflow_graph_node_depend_on (workflow_node, KAN_RESOURCE_PROVIDER_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_RENDER_FOUNDATION_FRAME_END_CHECKPOINT);
}

TEST_ROUTINE_API KAN_UM_MUTATOR_EXECUTE (test_routine)
{
    KAN_UMI_SINGLETON_WRITE (public, test_routine_singleton_t)
    KAN_UMI_SINGLETON_WRITE (private, test_routine_private_singleton_t)

    if (!state->executed_at_least_once)
    {
        public->test_mode_enabled = check_is_test_mode (state->application_framework_system_handle);
        state->executed_at_least_once = true;
    }

    if (!public->test_mode_enabled)
    {
        return;
    }

    KAN_UMI_SINGLETON_READ (resource_provider, kan_resource_provider_singleton_t)
    if (!resource_provider->scan_done)
    {
        return;
    }

    ++private->frames_count;
    if (!KAN_TYPED_ID_32_IS_VALID (private->setup_usage_id))
    {
        private->setup_name = NULL;
        KAN_UML_SEQUENCE_READ (node, kan_resource_generic_entry_t)
        {
            if (node->type == KAN_STATIC_INTERNED_ID_GET (test_setup_t))
            {
                private->setup_name = node->name;
                break;
            }
        }

        KAN_ASSERT_FORMATTED (private->setup_name,
                              "Test setup must be present in resources in order for test mode to work%s.", "");

        KAN_UMO_INDEXED_INSERT (request, kan_resource_usage_t)
        {
            request->usage_id = kan_next_resource_usage_id (resource_provider);
            private->setup_usage_id = request->usage_id;
            request->type = KAN_STATIC_INTERNED_ID_GET (test_setup_t);
            request->name = private->setup_name;
        }
    }

    switch (public->manual_result)
    {
    case TEST_ROUTINE_MANUAL_RESULT_NOT_SET:
        break;

    case TEST_ROUTINE_MANUAL_RESULT_PASSED:
        KAN_LOG (application_framework_example_test_routine, KAN_LOG_INFO,
                 "Marked test as finished through manual switch.")
        kan_application_framework_system_request_exit (state->application_framework_system_handle, 0);
        return;

    case TEST_ROUTINE_MANUAL_RESULT_FAILED:
        KAN_LOG (application_framework_example_test_routine, KAN_LOG_ERROR,
                 "Marked test as failed through manual switch.")
        kan_application_framework_system_request_exit (state->application_framework_system_handle, 1);
    }

    KAN_UMI_RESOURCE_RETRIEVE_IF_LOADED (setup, test_setup_t, &private->setup_name)
    if (setup && !private->setup_read)
    {
        for (kan_loop_size_t index = 0u; index < setup->expectations.size; ++index)
        {
            KAN_UMO_INDEXED_INSERT (request, kan_resource_usage_t)
            {
                request->usage_id = kan_next_resource_usage_id (resource_provider);
                request->type = KAN_STATIC_INTERNED_ID_GET (test_expectation_t);
                request->name = ((kan_interned_string_t *) setup->expectations.data)[index];
            }
        }
    }

    if (setup && setup->expectations.size > 0u && KAN_HANDLE_IS_VALID (public->expectation_read_back_buffer) &&
        public->expectation_read_back_statuses.size > 0u)
    {
        if (setup->expectations.size == public->expectation_read_back_statuses.size)
        {
            bool ready_for_testing = true;
            for (kan_loop_size_t index = 0u; index < setup->expectations.size; ++index)
            {
                if (kan_read_read_back_status_get (
                        ((kan_render_read_back_status_t *) public->expectation_read_back_statuses.data)[index]) !=
                    KAN_RENDER_READ_BACK_STATE_FINISHED)
                {
                    ready_for_testing = false;
                    break;
                }

                const kan_interned_string_t expectation_name =
                    ((kan_interned_string_t *) setup->expectations.data)[index];
                KAN_UMI_RESOURCE_RETRIEVE_IF_LOADED (expectation, test_expectation_t, &expectation_name)

                if (!expectation)
                {
                    ready_for_testing = false;
                    break;
                }
            }

            if (ready_for_testing)
            {
                int exit_code = 0;
                KAN_LOG (application_framework_example_test_routine, KAN_LOG_INFO, "Shutting down in test mode...")

                const uint8_t *read_back_data = kan_render_buffer_read (public->expectation_read_back_buffer);
                const uint8_t *current_read_back_data = read_back_data;
                struct kan_file_system_path_container_t output_path_container;

                KAN_ASSERT_FORMATTED (setup->expectations.size <= 9u,
                                      "%u expectations is too many for simple name generation.",
                                      (unsigned int) setup->expectations.size);

                for (kan_loop_size_t index = 0u; index < setup->expectations.size;
                     ++index, ++output_path_container.path[12u])
                {
                    const kan_interned_string_t expectation_name =
                        ((kan_interned_string_t *) setup->expectations.data)[index];

                    KAN_UMI_RESOURCE_RETRIEVE_IF_LOADED (expectation, test_expectation_t, &expectation_name)
                    KAN_ASSERT (expectation)

                    struct kan_image_raw_data_t frame_raw_data;
                    frame_raw_data.width = expectation->width;
                    frame_raw_data.height = expectation->height;
                    frame_raw_data.data = (uint8_t *) current_read_back_data;
                    current_read_back_data += expectation->width * expectation->height * 4u;

                    kan_file_system_path_container_copy_string (&output_path_container, setup->screenshot_base_name);
                    kan_file_system_path_container_add_suffix (&output_path_container, "_0");
                    output_path_container.path[output_path_container.length - 1u] += index;
                    kan_file_system_path_container_add_suffix (&output_path_container, ".png");

                    struct kan_stream_t *output_stream =
                        kan_direct_file_stream_open_for_write (output_path_container.path, true);

                    if (output_stream)
                    {
                        if (!kan_image_save (output_stream, KAN_IMAGE_SAVE_FORMAT_PNG, &frame_raw_data))
                        {
                            KAN_LOG (application_framework_example_test_routine, KAN_LOG_ERROR,
                                     "Failed to write result %lu.", (unsigned long) index)
                            exit_code = 1;
                        }

                        output_stream->operations->close (output_stream);
                    }
                    else
                    {
                        KAN_LOG (application_framework_example_test_routine, KAN_LOG_ERROR,
                                 "Failed to open file to write result %lu.", (unsigned long) index)
                        exit_code = 1;
                    }

                    const uint32_t *frame_data = (const uint32_t *) frame_raw_data.data;
                    const uint32_t *expectation_data = (const uint32_t *) expectation->rgba_data.data;

                    const kan_loop_size_t pixel_count = frame_raw_data.width * frame_raw_data.height;
                    kan_loop_size_t error_count = 0u;
                    // Not more than 1% of errors.
                    kan_loop_size_t max_error_count = pixel_count / 100u;

                    for (kan_loop_size_t pixel_index = 0u; pixel_index < pixel_count; ++pixel_index)
                    {
                        if (kan_are_colors_different (frame_data[pixel_index], expectation_data[pixel_index], 3u))
                        {
                            ++error_count;
                        }
                    }

                    if (error_count > max_error_count)
                    {
                        KAN_LOG (application_framework_example_test_routine, KAN_LOG_ERROR,
                                 "Frame and expectation have different data at view %lu: different %.3f%%.",
                                 (unsigned long) index, 100.0f * (float) error_count / (float) pixel_count)
                        exit_code = 1;
                    }
                }

                kan_application_framework_system_request_exit (state->application_framework_system_handle, exit_code);
                return;
            }
        }
        else
        {
            KAN_LOG (
                application_framework_example_test_routine, KAN_LOG_WARNING,
                "Not checking expectations yet as read back status array size is not equal to expectations array size.")
        }
    }

    if (setup && private->frames_count > setup->frames_to_wait)
    {
        KAN_LOG (application_framework_example_test_routine, KAN_LOG_INFO, "Shutting down in test mode...")
        KAN_LOG (application_framework_example_test_routine, KAN_LOG_ERROR,
                 "Time elapsed, but wasn't able to do any testing.")
        kan_application_framework_system_request_exit (state->application_framework_system_handle, 1);
        return;
    }
}

void test_routine_singleton_init (struct test_routine_singleton_t *instance)
{
    instance->test_mode_enabled = false;
    instance->expectation_read_back_buffer = KAN_HANDLE_SET_INVALID (kan_render_buffer_t);
    kan_dynamic_array_init (&instance->expectation_read_back_statuses, 0u, sizeof (kan_render_read_back_status_t),
                            alignof (kan_render_read_back_status_t), kan_allocation_group_stack_get ());
    instance->manual_result = TEST_ROUTINE_MANUAL_RESULT_NOT_SET;
}

void test_routine_singleton_shutdown (struct test_routine_singleton_t *instance)
{
    if (KAN_HANDLE_IS_VALID (instance->expectation_read_back_buffer))
    {
        kan_render_buffer_destroy (instance->expectation_read_back_buffer);
    }

    for (kan_loop_size_t index = 0u; index < instance->expectation_read_back_statuses.size; ++index)
    {
        kan_render_read_back_status_destroy (
            ((kan_render_read_back_status_t *) instance->expectation_read_back_statuses.data)[index]);
    }

    kan_dynamic_array_shutdown (&instance->expectation_read_back_statuses);
}
