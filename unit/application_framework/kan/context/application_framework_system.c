#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <kan/container/trivial_string_buffer.h>
#include <kan/context/all_system_names.h>
#include <kan/context/application_framework_system.h>
#include <kan/context/application_system.h>
#include <kan/context/hot_reload_coordination_system.h>
#include <kan/context/update_system.h>
#include <kan/cpu_profiler/markup.h>
#include <kan/file_system/entry.h>
#include <kan/log/logging.h>
#include <kan/memory/allocation.h>
#include <kan/platform/process.h>
#include <kan/precise_time/precise_time.h>
#include <kan/threading/atomic.h>
#include <kan/threading/thread.h>

KAN_LOG_DEFINE_CATEGORY (context_application_framework_system);
KAN_USE_STATIC_CPU_SECTIONS

enum application_framework_auto_build_state_t
{
    APPLICATION_FRAMEWORK_AUTO_BUILD_STATE_NONE = 0u,
    APPLICATION_FRAMEWORK_AUTO_BUILD_STATE_SCHEDULED,
    APPLICATION_FRAMEWORK_AUTO_BUILD_STATE_EXECUTING,
};

struct application_framework_system_t
{
    kan_context_t context;
    kan_allocation_group_t group;

    kan_instance_size_t outer_arguments_count;
    char **outer_arguments;

    bool enable_auto_build;
    char *auto_build_cmake;
    kan_platform_argument_list_t auto_build_arguments;
    char *auto_build_lock_file;
    kan_stable_size_t auto_build_delay_ns;

    bool exit_requested;
    int exit_code;

    kan_stable_size_t min_frame_time_ns;
    kan_application_system_event_iterator_t event_iterator;

    enum application_framework_auto_build_state_t auto_build_state;
    kan_stable_size_t auto_build_run_after_ns;
    kan_platform_process_t auto_build_process;

    kan_context_system_t hot_reload_coordination_system;
};

kan_context_system_t application_framework_system_create (kan_allocation_group_t group, void *user_config)
{
    kan_cpu_static_sections_ensure_initialized ();
    struct application_framework_system_t *system = kan_allocate_general (
        group, sizeof (struct application_framework_system_t), alignof (struct application_framework_system_t));
    system->group = group;

    if (user_config)
    {
        struct kan_application_framework_system_config_t *config = user_config;
        system->outer_arguments_count = config->arguments_count;
        system->outer_arguments = config->arguments;
        system->enable_auto_build = config->enable_auto_build;
        system->auto_build_delay_ns = config->auto_build_delay_ns;

        if (system->enable_auto_build)
        {
            const kan_instance_size_t cmake_length = (kan_instance_size_t) strlen (config->auto_build_cmake);
            system->auto_build_cmake = kan_allocate_general (group, cmake_length + 1u, alignof (char));
            memcpy (system->auto_build_cmake, config->auto_build_cmake, cmake_length + 1u);

            system->auto_build_arguments = kan_platform_argument_list_create ();
            kan_platform_argument_list_append (system->auto_build_arguments, "--build");
            kan_platform_argument_list_append (system->auto_build_arguments, config->auto_build_directory);
            kan_platform_argument_list_append (system->auto_build_arguments, "--target");
            kan_platform_argument_list_append (system->auto_build_arguments, config->auto_build_target);
            kan_platform_argument_list_append (system->auto_build_arguments, "--config");
            kan_platform_argument_list_append (system->auto_build_arguments, config->auto_build_config);
            kan_platform_argument_list_append (system->auto_build_arguments, "--");
            kan_platform_argument_list_append (system->auto_build_arguments, "--quiet");

            const kan_instance_size_t lock_file_length = (kan_instance_size_t) strlen (config->auto_build_lock_file);
            system->auto_build_lock_file = kan_allocate_general (group, lock_file_length + 1u, alignof (char));
            memcpy (system->auto_build_lock_file, config->auto_build_lock_file, lock_file_length + 1u);
        }
        else
        {
            system->auto_build_cmake = NULL;
            system->auto_build_arguments = KAN_HANDLE_SET_INVALID (kan_platform_argument_list_t);
            system->auto_build_lock_file = NULL;
        }
    }
    else
    {
        system->outer_arguments_count = 0u;
        system->outer_arguments = NULL;
        system->enable_auto_build = false;
        system->auto_build_cmake = NULL;
        system->auto_build_arguments = KAN_HANDLE_SET_INVALID (kan_platform_argument_list_t);
        system->auto_build_lock_file = NULL;
        // It is not enabled anyway.
        system->auto_build_delay_ns = 0u;
    }

    system->exit_requested = false;
    system->exit_code = 0;
    system->min_frame_time_ns = KAN_APPLICATION_FRAMEWORK_DEFAULT_MIN_FRAME_TIME_NS;

    system->auto_build_state = APPLICATION_FRAMEWORK_AUTO_BUILD_STATE_NONE;
    system->auto_build_run_after_ns = system->auto_build_delay_ns;
    system->auto_build_process = KAN_HANDLE_SET_INVALID (kan_platform_process_t);

    system->hot_reload_coordination_system = KAN_HANDLE_SET_INVALID (kan_context_system_t);
    return KAN_HANDLE_SET (kan_context_system_t, system);
}

static void application_framework_system_update (kan_context_system_t handle)
{
    struct application_framework_system_t *system = KAN_HANDLE_GET (handle);
    KAN_CPU_SCOPED_STATIC_SECTION (context_application_framework_system_update)
    kan_context_system_t application_system = kan_context_query (system->context, KAN_CONTEXT_APPLICATION_SYSTEM_NAME);

    if (KAN_HANDLE_IS_VALID (application_system))
    {
        const struct kan_platform_application_event_t *event;
        while ((event = kan_application_system_event_iterator_get (application_system, system->event_iterator)))
        {
            if (event->type == KAN_PLATFORM_APPLICATION_EVENT_TYPE_QUIT)
            {
                if (!system->exit_requested)
                {
                    system->exit_requested = true;
                    system->exit_code = 0;
                }
            }

            system->event_iterator = kan_application_system_event_iterator_advance (system->event_iterator);
        }
    }

    if (system->enable_auto_build && !system->exit_requested &&
        KAN_HANDLE_IS_VALID (system->hot_reload_coordination_system) &&
        kan_hot_reload_coordination_system_is_possible ())
    {
        switch (system->auto_build_state)
        {
        case APPLICATION_FRAMEWORK_AUTO_BUILD_STATE_NONE:
            if (system->auto_build_run_after_ns < kan_precise_time_get_elapsed_nanoseconds ())
            {
                // Schedule new auto build execution.
                if (kan_file_system_lock_file_create (
                        system->auto_build_lock_file,
                        KAN_FILE_SYSTEM_LOCK_FILE_FILE_PATH | KAN_FILE_SYSTEM_LOCK_FILE_QUIET))
                {
                    kan_hot_reload_coordination_system_schedule (system->hot_reload_coordination_system);
                    system->auto_build_state = APPLICATION_FRAMEWORK_AUTO_BUILD_STATE_SCHEDULED;
                }
                else
                {
                    // Unable to schedule right now, we'll try again later.
                    goto auto_build_schedule_next_run;
                }
            }

            break;

        case APPLICATION_FRAMEWORK_AUTO_BUILD_STATE_SCHEDULED:
            if (!kan_hot_reload_coordination_system_is_scheduled (system->hot_reload_coordination_system))
            {
                if (kan_hot_reload_coordination_system_is_executing (system->hot_reload_coordination_system))
                {
                    KAN_ASSERT (!KAN_HANDLE_IS_VALID (system->auto_build_process))
                    system->auto_build_process =
                        kan_platform_process_start (system->auto_build_cmake, system->auto_build_arguments,
                                                    KAN_HANDLE_SET_INVALID (kan_platform_environment_t));
                    system->auto_build_state = APPLICATION_FRAMEWORK_AUTO_BUILD_STATE_EXECUTING;
                }
                else
                {
                    // Cancelled, unlock and try again later.
                    goto auto_build_unlock;
                }
            }

            break;

        case APPLICATION_FRAMEWORK_AUTO_BUILD_STATE_EXECUTING:
        {
            KAN_ASSERT (KAN_HANDLE_IS_VALID (system->auto_build_process))
            int exit_code;

            if (kan_platform_process_try_finalize (system->auto_build_process, &exit_code))
            {
                system->auto_build_process = KAN_HANDLE_SET_INVALID (kan_platform_process_t);
                if (exit_code != 0)
                {
                    KAN_LOG (context_application_framework_system, KAN_LOG_ERROR,
                             "Failed to execute auto build command, its return code is %d.", exit_code)
                }

                kan_hot_reload_coordination_system_finish (system->hot_reload_coordination_system);
                goto auto_build_unlock;
            }

            break;
        }

        auto_build_unlock:
            kan_file_system_lock_file_destroy (system->auto_build_lock_file,
                                               KAN_FILE_SYSTEM_LOCK_FILE_FILE_PATH | KAN_FILE_SYSTEM_LOCK_FILE_QUIET);
            // Intentional fallthrough.

        auto_build_schedule_next_run:
            system->auto_build_state = APPLICATION_FRAMEWORK_AUTO_BUILD_STATE_NONE;
            system->auto_build_run_after_ns = kan_precise_time_get_elapsed_nanoseconds () + system->auto_build_delay_ns;
            break;
        }
    }
}

void application_framework_system_connect (kan_context_system_t handle, kan_context_t context)
{
    struct application_framework_system_t *system = KAN_HANDLE_GET (handle);
    system->context = context;

    kan_context_system_t update_system = kan_context_query (system->context, KAN_CONTEXT_UPDATE_SYSTEM_NAME);
    if (KAN_HANDLE_IS_VALID (update_system))
    {
        kan_update_system_connect_on_run (update_system, handle, application_framework_system_update, 0u, NULL, 0u,
                                          NULL);
    }
}

void application_framework_system_init (kan_context_system_t handle)
{
    struct application_framework_system_t *system = KAN_HANDLE_GET (handle);
    kan_context_system_t application_system = kan_context_query (system->context, KAN_CONTEXT_APPLICATION_SYSTEM_NAME);

    if (KAN_HANDLE_IS_VALID (application_system))
    {
        system->event_iterator = kan_application_system_event_iterator_create (application_system);
    }

    system->hot_reload_coordination_system =
        kan_context_query (system->context, KAN_CONTEXT_HOT_RELOAD_COORDINATION_SYSTEM_NAME);
}

void application_framework_system_shutdown (kan_context_system_t handle)
{
    struct application_framework_system_t *system = KAN_HANDLE_GET (handle);
    kan_context_system_t application_system = kan_context_query (system->context, KAN_CONTEXT_APPLICATION_SYSTEM_NAME);

    if (KAN_HANDLE_IS_VALID (application_system))
    {
        kan_application_system_event_iterator_destroy (application_system, system->event_iterator);
    }

    if (KAN_HANDLE_IS_VALID (system->auto_build_process))
    {
        int exit_code;
        kan_platform_process_wait (system->auto_build_process, &exit_code);
    }
}

void application_framework_system_disconnect (kan_context_system_t handle)
{
    struct application_framework_system_t *system = KAN_HANDLE_GET (handle);
    kan_context_system_t update_system = kan_context_query (system->context, KAN_CONTEXT_UPDATE_SYSTEM_NAME);

    if (KAN_HANDLE_IS_VALID (update_system))
    {
        kan_update_system_disconnect_on_run (update_system, handle);
    }
}

void application_framework_system_destroy (kan_context_system_t handle)
{
    struct application_framework_system_t *system = KAN_HANDLE_GET (handle);
    if (system->auto_build_cmake)
    {
        const kan_instance_size_t cmake_length = (kan_instance_size_t) strlen (system->auto_build_cmake);
        kan_free_general (system->group, system->auto_build_cmake, cmake_length + 1u);
    }

    if (KAN_HANDLE_IS_VALID (system->auto_build_arguments))
    {
        kan_platform_argument_list_destroy (system->auto_build_arguments);
    }

    if (system->auto_build_lock_file)
    {
        const kan_instance_size_t lock_file_length = (kan_instance_size_t) strlen (system->auto_build_lock_file);
        kan_free_general (system->group, system->auto_build_lock_file, lock_file_length + 1u);
    }

    kan_free_general (system->group, system, sizeof (struct application_framework_system_t));
}

APPLICATION_FRAMEWORK_API struct kan_context_system_api_t KAN_CONTEXT_SYSTEM_API_NAME (
    application_framework_system_t) = {
    .name = "application_framework_system_t",
    .create = application_framework_system_create,
    .connect = application_framework_system_connect,
    .connected_init = application_framework_system_init,
    .connected_shutdown = application_framework_system_shutdown,
    .disconnect = application_framework_system_disconnect,
    .destroy = application_framework_system_destroy,
};

kan_instance_size_t kan_application_framework_system_get_arguments_count (
    kan_context_system_t application_framework_system)
{
    struct application_framework_system_t *system = KAN_HANDLE_GET (application_framework_system);
    return system->outer_arguments_count;
}

char **kan_application_framework_system_get_arguments (kan_context_system_t application_framework_system)
{
    struct application_framework_system_t *system = KAN_HANDLE_GET (application_framework_system);
    return system->outer_arguments;
}

kan_stable_size_t kan_application_framework_get_min_frame_time_ns (kan_context_system_t application_framework_system)
{
    struct application_framework_system_t *system = KAN_HANDLE_GET (application_framework_system);
    return system->min_frame_time_ns;
}

void kan_application_framework_set_min_frame_time_ns (kan_context_system_t application_framework_system,
                                                      kan_stable_size_t min_frame_time_ns)
{
    struct application_framework_system_t *system = KAN_HANDLE_GET (application_framework_system);
    system->min_frame_time_ns = min_frame_time_ns;
}

void kan_application_framework_system_request_exit (kan_context_system_t application_framework_system, int exit_code)
{
    struct application_framework_system_t *system = KAN_HANDLE_GET (application_framework_system);
    if (!system->exit_requested)
    {
        system->exit_requested = true;
        system->exit_code = exit_code;
    }
}

bool kan_application_framework_system_is_exit_requested (kan_context_system_t application_framework_system,
                                                         int *exit_code_output)
{
    struct application_framework_system_t *system = KAN_HANDLE_GET (application_framework_system);
    if (system->exit_requested)
    {
        *exit_code_output = system->exit_code;
    }

    return system->exit_requested;
}
