#pragma once

#include <platform_api.h>

#include <stdint.h>

#include <kan/api_common/c_header.h>
#include <kan/api_common/core_types.h>

/// \file
/// \brief Provides basic API for creating child processes.
///
/// \par Environment
/// \parblock
/// `kan_platform_environment_t` makes it possible to retrieve current version of process environment, locally edit
/// variables in it and pass this edited environment to newly created child process if needed. Keep in mind that editing
/// the same environment object from multiple threads is not safe!
/// \endparblock
///
/// \par Argument list
/// \parblock
/// `kan_platform_argument_list_t` provides convenient interface for building list of arguments for process creation.
/// Argument lists are expected to be reused when appropriate, for example for recurrent child process creation.
/// It also supports checkpoint creation: user can create checkpoint, append temporary arguments, create new process
/// and then reset argument list to the state before checkpoint creation, which makes argument list reuse simpler in
/// cases when some arguments change from execution to execution. Keep in mind that editing the same argument list from
/// multiple threads is not safe!
/// \endparblock
///
/// \par Process
/// \parblock
/// `kan_platform_process_t` provides simple API for creating child processes and waiting for their execution if needed.
/// Non-blocking waiting and custom execution environments are supported, making it better choice for executing build
/// and hot reload tasks than just calling `system` function.
/// \endparblock

KAN_C_HEADER_BEGIN

KAN_HANDLE_DEFINE (kan_platform_environment_t);
KAN_HANDLE_DEFINE (kan_platform_argument_list_t);
KAN_HANDLE_DEFINE (kan_platform_argument_list_checkpoint_t);
KAN_HANDLE_DEFINE (kan_platform_process_t);

/// \brief Creates new environment instance that is populated from this process environment.
PLATFORM_API kan_platform_environment_t kan_platform_environment_create_and_retrieve (void);

/// \brief Creates new empty environment instance.
PLATFORM_API kan_platform_environment_t kan_platform_environment_create_empty (void);

/// \brief Retrieves value of variable in the environment instance if any.
PLATFORM_API const char *kan_platform_environment_get (kan_platform_environment_t environment, const char *name);

/// \brief Sets value of variable in the environment instance.
/// \warning Does not change actual process or global environment, only changes this instance
///          so it can be passed to other functions as argument!
PLATFORM_API bool kan_platform_environment_set (kan_platform_environment_t environment,
                                                const char *name,
                                                const char *value);

/// \brief Destroys given environment instance.
PLATFORM_API void kan_platform_environment_destroy (kan_platform_environment_t environment);

/// \brief Creates new empty argument list.
PLATFORM_API kan_platform_argument_list_t kan_platform_argument_list_create (void);

/// \brief Appends copy of given string to the argument list.
PLATFORM_API void kan_platform_argument_list_append (kan_platform_argument_list_t instance, const char *value);

/// \brief Save current last node so it can be used to remove all the arguments appended after.
PLATFORM_API kan_platform_argument_list_checkpoint_t
kan_platform_argument_list_save_checkpoint (kan_platform_argument_list_t instance);

/// \brief Remove all the arguments added after given checkpoint.
/// \warning Invalidates checkpoints that were saved after this one!
PLATFORM_API void kan_platform_argument_list_restore_checkpoint (kan_platform_argument_list_t instance,
                                                                 kan_platform_argument_list_checkpoint_t checkpoint);

/// \brief Destroys given argument list.
PLATFORM_API void kan_platform_argument_list_destroy (kan_platform_argument_list_t instance);

/// \brief Creates new process for the executable at given path.
/// \param path Path to the executable or script to be executed.
/// \param arguments Argument list or invalid handle if no arguments.
/// \param environment Custom execution environment or invalid handle if this process environment should be used.
PLATFORM_API kan_platform_process_t kan_platform_process_start (const char *path,
                                                                kan_platform_argument_list_t arguments,
                                                                kan_platform_environment_t environment);

/// \brief Destroys process object and detaches child process without closing it.
PLATFORM_API void kan_platform_process_detach (kan_platform_process_t process);

/// \brief Blocks until process execution is done. Returns `true` on successful wait.
PLATFORM_API bool kan_platform_process_wait (kan_platform_process_t process, int *return_code_output);

/// \brief Checks if process execution is finished and extracts its return code if it is.
/// \details If process execution is finished, this function will destroy the process and invalidate the handle.
PLATFORM_API bool kan_platform_process_try_finalize (kan_platform_process_t process, int *return_code_output);

/// \brief Helper syntax sugar for the cases when user needs to start process and wait for it to be finished.
static inline int kan_platform_execute_sub_process (const char *path,
                                                    kan_platform_argument_list_t arguments,
                                                    kan_platform_environment_t environment)
{
    kan_platform_process_t process = kan_platform_process_start (path, arguments, environment);
    if (KAN_HANDLE_IS_VALID (process))
    {
        int return_code;
        if (kan_platform_process_wait (process, &return_code))
        {
            return return_code;
        }
    }

    // Return -1 as default error code if something didn't work out for us.
    return -1;
}

KAN_C_HEADER_END
