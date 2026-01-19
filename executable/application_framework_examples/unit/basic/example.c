#include <application_framework_examples_basic_api.h>

#include <stdio.h>
#include <string.h>

#include <kan/context/all_system_names.h>
#include <kan/context/application_framework_system.h>
#include <kan/context/application_system.h>
#include <kan/log/logging.h>
#include <kan/precise_time/precise_time.h>
#include <kan/resource_pipeline/meta.h>
#include <kan/test_routine/test_routine.h>
#include <kan/universe/macro.h>
#include <kan/universe/universe.h>
#include <kan/universe_resource_provider/provider.h>

KAN_LOG_DEFINE_CATEGORY (application_framework_examples_basic);
KAN_USE_STATIC_INTERNED_IDS

struct basic_data_type_t
{
    kan_instance_size_t x;
    kan_instance_size_t y;
};

KAN_REFLECTION_STRUCT_META (basic_data_type_t)
APPLICATION_FRAMEWORK_EXAMPLES_BASIC_API struct kan_resource_type_meta_t basic_data_type_resource_type = {
    .flags = KAN_RESOURCE_TYPE_ROOT,
    .version = CUSHION_START_NS_X64,
    .move = NULL,
    .reset = NULL,
};

struct example_basic_singleton_t
{
    kan_application_system_window_t window_handle;
    bool test_usage_added;
    kan_resource_usage_id_t test_usage_id;
    kan_time_size_t last_frame_time_ns;
};

APPLICATION_FRAMEWORK_EXAMPLES_BASIC_API void example_basic_singleton_init (struct example_basic_singleton_t *instance)
{
    instance->window_handle = KAN_HANDLE_SET_INVALID (kan_application_system_window_t);
    instance->test_usage_added = false;
    instance->test_usage_id = KAN_TYPED_ID_32_SET_INVALID (kan_resource_usage_id_t);
    instance->last_frame_time_ns = kan_precise_time_get_elapsed_nanoseconds ();
}

struct example_basic_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (example_basic)
    KAN_UM_BIND_STATE (example_basic, state)
    kan_context_system_t application_system_handle;
};

APPLICATION_FRAMEWORK_EXAMPLES_BASIC_API KAN_UM_MUTATOR_DEPLOY (example_basic)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_context_t context = kan_universe_get_context (universe);
    state->application_system_handle = kan_context_query (context, KAN_CONTEXT_APPLICATION_SYSTEM_NAME);
}

APPLICATION_FRAMEWORK_EXAMPLES_BASIC_API KAN_UM_MUTATOR_EXECUTE (example_basic)
{
    KAN_UMI_SINGLETON_WRITE (test, test_routine_singleton_t)
    KAN_UMI_SINGLETON_WRITE (singleton, example_basic_singleton_t)

    if (!KAN_HANDLE_IS_VALID (singleton->window_handle))
    {
        enum kan_platform_window_flag_t flags = KAN_PLATFORM_WINDOW_FLAG_SUPPORTS_VULKAN;
        if (test->test_mode_enabled)
        {
            flags |= KAN_PLATFORM_WINDOW_FLAG_HIDDEN;
        }

        singleton->window_handle = kan_application_system_window_create (state->application_system_handle,
                                                                         "Title placeholder", 600u, 400u, flags);
        kan_application_system_window_raise (state->application_system_handle, singleton->window_handle);
    }

    if (!singleton->test_usage_added)
    {
        KAN_UMI_SINGLETON_READ (provider, kan_resource_provider_singleton_t)
        KAN_UMO_INDEXED_INSERT (usage, kan_resource_usage_t)
        {
            usage->usage_id = kan_next_resource_usage_id (provider);
            usage->type = KAN_STATIC_INTERNED_ID_GET (basic_data_type_t);
            usage->name = KAN_STATIC_INTERNED_ID_GET (test);
            usage->priority = 0u;
            singleton->test_usage_id = usage->usage_id;
        }

        singleton->test_usage_added = true;
    }

    kan_instance_size_t x = 0;
    kan_instance_size_t y = 0;

    {
        const kan_interned_string_t name = KAN_STATIC_INTERNED_ID_GET (test);
        KAN_UMI_RESOURCE_RETRIEVE_IF_LOADED (loaded, basic_data_type_t, &name)

        if (loaded)
        {
            x = loaded->x;
            y = loaded->y;

            if (x != 3u || y != 5u)
            {
                test->manual_result = TEST_ROUTINE_MANUAL_RESULT_FAILED;
                KAN_LOG (application_framework_examples_basic, KAN_LOG_INFO, "Unexpected x or y.")
            }
            else
            {
                test->manual_result = TEST_ROUTINE_MANUAL_RESULT_PASSED;
            }
        }
    }

#define TITLE_BUFFER_SIZE 256u
    char buffer[TITLE_BUFFER_SIZE];
    const kan_time_size_t current_time_ns = kan_precise_time_get_elapsed_nanoseconds ();

    snprintf (buffer, TITLE_BUFFER_SIZE, "Time: %.3f seconds. Visual delta: %.3f seconds. X: %llu. Y: %llu.",
              (float) current_time_ns / 1e9f, (float) (singleton->last_frame_time_ns - current_time_ns) / 1e9f,
              (unsigned long long) x, (unsigned long long) y);

    singleton->last_frame_time_ns = current_time_ns;
    kan_application_system_window_set_title (state->application_system_handle, singleton->window_handle, buffer);
#undef TITLE_BUFFER_SIZE
}
