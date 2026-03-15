#define _CRT_SECURE_NO_WARNINGS __CUSHION_PRESERVE__
#include <kan/api_common/mute_warnings.h>

KAN_MUTE_THIRD_PARTY_WARNINGS_BEGIN
#include <SDL3/SDL_process.h>
KAN_MUTE_THIRD_PARTY_WARNINGS_END

#include <kan/container/list.h>
#include <kan/log/logging.h>
#include <kan/memory/allocation.h>
#include <kan/platform/process.h>

KAN_LOG_DEFINE_CATEGORY (platform_process);

static bool statics_initialized = false;
static kan_allocation_group_t platform_process_allocation_group;
static kan_allocation_group_t arguments_allocation_group;
static kan_allocation_group_t temporary_allocation_group;

static void ensure_statics_initialized (void)
{
    if (!statics_initialized)
    {
        platform_process_allocation_group =
            kan_allocation_group_get_child (kan_allocation_group_root (), "platform_process");
        arguments_allocation_group = kan_allocation_group_get_child (kan_allocation_group_root (), "arguments");
        temporary_allocation_group = kan_allocation_group_get_child (kan_allocation_group_root (), "temporary");
        statics_initialized = true;
    }
}

kan_platform_environment_t kan_platform_environment_create_and_retrieve (void)
{
    ensure_statics_initialized ();
    SDL_Environment *environment = SDL_CreateEnvironment (true);

    if (!environment)
    {
        KAN_LOG (platform_process, KAN_LOG_ERROR, "Failed create and retrieve environment, backend error: %s",
                 SDL_GetError ())
        return KAN_HANDLE_SET_INVALID (kan_platform_environment_t);
    }

    return KAN_HANDLE_SET (kan_platform_environment_t, environment);
}

kan_platform_environment_t kan_platform_environment_create_empty (void)
{
    ensure_statics_initialized ();
    SDL_Environment *environment = SDL_CreateEnvironment (true);

    if (!environment)
    {
        KAN_LOG (platform_process, KAN_LOG_ERROR, "Failed create empty environment, backend error: %s", SDL_GetError ())
        return KAN_HANDLE_SET_INVALID (kan_platform_environment_t);
    }

    return KAN_HANDLE_SET (kan_platform_environment_t, environment);
}

const char *kan_platform_environment_get (kan_platform_environment_t environment, const char *name)
{
    SDL_Environment *instance = KAN_HANDLE_GET (environment);
    return SDL_GetEnvironmentVariable (instance, name);
}

bool kan_platform_environment_set (kan_platform_environment_t environment, const char *name, const char *value)
{
    SDL_Environment *instance = KAN_HANDLE_GET (environment);
    return SDL_SetEnvironmentVariable (instance, name, value, true);
}

void kan_platform_environment_destroy (kan_platform_environment_t environment)
{
    SDL_DestroyEnvironment (KAN_HANDLE_GET (environment));
}

struct argument_list_node_t
{
    struct kan_bd_list_node_t node;
    char *value;
};

kan_platform_argument_list_t kan_platform_argument_list_create (void)
{
    ensure_statics_initialized ();
    // We might use more memory-optimized structure in the future if we need to.
    // For now, we use simple list and separate allocations for simplicity as
    // we do not expect it to be performance bottleneck.
    struct kan_bd_list_t *list = kan_allocate_batched (arguments_allocation_group, sizeof (struct kan_bd_list_t));
    kan_bd_list_init (list);
    return KAN_HANDLE_SET (kan_platform_argument_list_t, list);
}

void kan_platform_argument_list_append (kan_platform_argument_list_t instance, const char *value)
{
    struct kan_bd_list_t *list = KAN_HANDLE_GET (instance);
    struct argument_list_node_t *new_node =
        kan_allocate_batched (arguments_allocation_group, sizeof (struct argument_list_node_t));
    kan_bd_list_add (list, NULL, &new_node->node);

    const kan_instance_size_t length = strlen (value);
    new_node->value = kan_allocate_general (arguments_allocation_group, length + 1u, alignof (char));
    memcpy (new_node->value, value, length + 1u);
}

kan_platform_argument_list_checkpoint_t kan_platform_argument_list_save_checkpoint (
    kan_platform_argument_list_t instance)
{
    struct kan_bd_list_t *list = KAN_HANDLE_GET (instance);
    return KAN_HANDLE_SET (kan_platform_argument_list_checkpoint_t, list->last);
}

void kan_platform_argument_list_restore_checkpoint (kan_platform_argument_list_t instance,
                                                    kan_platform_argument_list_checkpoint_t checkpoint)
{
    struct kan_bd_list_t *list = KAN_HANDLE_GET (instance);
    struct argument_list_node_t *remove_since = KAN_HANDLE_GET (checkpoint);

    if (remove_since)
    {
        remove_since = (struct argument_list_node_t *) remove_since->node.next;
    }
    else
    {
        remove_since = (struct argument_list_node_t *) list->first;
    }

    while (remove_since)
    {
        struct argument_list_node_t *next = (struct argument_list_node_t *) remove_since->node.next;
        kan_bd_list_remove (list, &remove_since->node);
        kan_free_general (arguments_allocation_group, remove_since->value, strlen (remove_since->value) + 1u);
        kan_free_batched (arguments_allocation_group, remove_since);
        remove_since = next;
    }
}

void kan_platform_argument_list_destroy (kan_platform_argument_list_t instance)
{
    struct kan_bd_list_t *list = KAN_HANDLE_GET (instance);
    struct argument_list_node_t *node = (struct argument_list_node_t *) list->first;

    while (node)
    {
        struct argument_list_node_t *next = (struct argument_list_node_t *) node->node.next;
        kan_free_general (arguments_allocation_group, node->value, strlen (node->value) + 1u);
        kan_free_batched (arguments_allocation_group, node);
        node = next;
    }

    kan_free_batched (arguments_allocation_group, list);
}

kan_platform_process_t kan_platform_process_start (const char *path,
                                                   kan_platform_argument_list_t arguments,
                                                   kan_platform_environment_t environment)
{
    ensure_statics_initialized ();
    struct kan_bd_list_t *arguments_list = KAN_HANDLE_GET (arguments);

    const kan_instance_size_t arguments_count = 1u + (arguments_list ? arguments_list->size : 0u);
    const char **arguments_array = kan_allocate_general (
        temporary_allocation_group, sizeof (const char *) * (arguments_count + 1u), alignof (const char *));

    CUSHION_DEFER
    {
        kan_free_general (temporary_allocation_group, arguments_array, sizeof (const char *) * (arguments_count + 1u));
    }

    arguments_array[0u] = path;
    arguments_array[arguments_count] = NULL;

    struct argument_list_node_t *argument_node =
        (struct argument_list_node_t *) (arguments_list ? arguments_list->first : NULL);
    const char **argument_output = &arguments_array[1u];

    while (argument_node)
    {
        *argument_output = argument_node->value;
        ++argument_output;
        argument_node = (struct argument_list_node_t *) argument_node->node.next;
    }

    SDL_PropertiesID properties = SDL_CreateProperties ();
    CUSHION_DEFER { SDL_DestroyProperties (properties); }
    SDL_SetPointerProperty (properties, SDL_PROP_PROCESS_CREATE_ARGS_POINTER, arguments_array);

    if (KAN_HANDLE_IS_VALID (environment))
    {
        SDL_SetPointerProperty (properties, SDL_PROP_PROCESS_CREATE_ENVIRONMENT_POINTER, KAN_HANDLE_GET (environment));
    }

    SDL_Process *process = SDL_CreateProcessWithProperties (properties);
    if (!process)
    {
        KAN_LOG (platform_process, KAN_LOG_ERROR, "Failed create new process, backend error: %s", SDL_GetError ())
        return KAN_HANDLE_SET_INVALID (kan_platform_process_t);
    }

    return KAN_HANDLE_SET (kan_platform_process_t, process);
}

void kan_platform_process_detach (kan_platform_process_t process) { SDL_DestroyProcess (KAN_HANDLE_GET (process)); }

bool kan_platform_process_wait (kan_platform_process_t process, int *return_code_output)
{
    return SDL_WaitProcess (KAN_HANDLE_GET (process), true, return_code_output);
}

bool kan_platform_process_try_finalize (kan_platform_process_t process, int *return_code_output)
{
    return SDL_WaitProcess (KAN_HANDLE_GET (process), false, return_code_output);
}
