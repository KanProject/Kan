#define _CRT_SECURE_NO_WARNINGS __CUSHION_PRESERVE__

#include <test_universe_resource_provider_api.h>

#include <stddef.h>

#include <kan/context/all_system_names.h>
#include <kan/context/hot_reload_coordination_system.h>
#include <kan/context/reflection_system.h>
#include <kan/context/universe_system.h>
#include <kan/context/update_system.h>
#include <kan/context/virtual_file_system.h>
#include <kan/file_system/entry.h>
#include <kan/file_system/stream.h>
#include <kan/reflection/generated_reflection.h>
#include <kan/resource_pipeline/build.h>
#include <kan/resource_pipeline/meta.h>
#include <kan/resource_pipeline/platform_configuration.h>
#include <kan/serialization/readable_data.h>
#include <kan/testing/testing.h>
#include <kan/universe/macro.h>
#include <kan/universe/universe.h>
#include <kan/universe_resource_provider/provider.h>
#include <kan/universe_trivial_scheduler/universe_trivial_scheduler.h>

#define WORKSPACE_DIRECTORY "workspace"
#define RAW_DIRECTORY "raw"
#define PLATFORM_CONFIGURATION_DIRECTORY "platform_configuration"
#define RESOURCE_MOUNT_PATH "resources"

KAN_LOG_DEFINE_CATEGORY (test_universe_resource_provider);
KAN_USE_STATIC_INTERNED_IDS
static bool global_test_finished = false;

struct first_resource_type_t
{
    uint64_t some_integer;
    bool flag_1;
    bool flag_2;
    bool flag_3;
    bool flag_4;
};

KAN_REFLECTION_STRUCT_META (first_resource_type_t)
TEST_UNIVERSE_RESOURCE_PROVIDER_API struct kan_resource_type_meta_t first_resource_type_meta = {
    .flags = KAN_RESOURCE_TYPE_ROOT,
    .version = CUSHION_START_NS_X64,
    .move = NULL,
    .reset = NULL,
};

struct second_resource_type_t
{
    kan_interned_string_t first_id;
    kan_interned_string_t second_id;
};

KAN_REFLECTION_STRUCT_META (second_resource_type_t)
TEST_UNIVERSE_RESOURCE_PROVIDER_API struct kan_resource_type_meta_t second_resource_type_meta = {
    .flags = KAN_RESOURCE_TYPE_ROOT,
    .version = CUSHION_START_NS_X64,
    .move = NULL,
    .reset = NULL,
};

struct third_party_reference_resource_type_t
{
    kan_interned_string_t third_party_name;
};

KAN_REFLECTION_STRUCT_META (third_party_reference_resource_type_t)
TEST_UNIVERSE_RESOURCE_PROVIDER_API struct kan_resource_type_meta_t third_party_reference_resource_type_meta = {
    .flags = KAN_RESOURCE_TYPE_ROOT,
    .version = CUSHION_START_NS_X64,
    .move = NULL,
    .reset = NULL,
};

KAN_REFLECTION_STRUCT_FIELD_META (third_party_reference_resource_type_t, third_party_name)
TEST_UNIVERSE_RESOURCE_PROVIDER_API struct kan_resource_reference_meta_t
    third_party_reference_resource_type_reference_third_party_name = {
        .type_name = NULL,
        .flags = 0u,
};

struct streamed_resource_type_t
{
    kan_instance_size_t value;
};

KAN_REFLECTION_STRUCT_META (streamed_resource_type_t)
TEST_UNIVERSE_RESOURCE_PROVIDER_API struct kan_resource_type_meta_t streamed_resource_type_meta = {
    .flags = KAN_RESOURCE_TYPE_ROOT | KAN_RESOURCE_TYPE_STREAMED,
    .version = CUSHION_START_NS_X64,
    .move = NULL,
    .reset = NULL,
};

struct transitive_resource_type_t
{
    kan_instance_size_t value;
};

KAN_REFLECTION_STRUCT_META (transitive_resource_type_t)
TEST_UNIVERSE_RESOURCE_PROVIDER_API struct kan_resource_type_meta_t transitive_resource_type_meta = {
    .flags = KAN_RESOURCE_TYPE_ROOT | KAN_RESOURCE_TYPE_TRANSITIVELY_LOADED,
    .version = CUSHION_START_NS_X64,
    .move = NULL,
    .reset = NULL,
};

static struct first_resource_type_t resource_one = {
    64u, true, false, true, false,
};

static struct first_resource_type_t resource_two = {
    129u, true, true, true, true,
};

static struct first_resource_type_t resource_three = {
    81u, false, true, false, true,
};

static struct first_resource_type_t resource_four = {
    168u, true, true, false, false,
};

static struct second_resource_type_t resource_players;
static struct second_resource_type_t resource_characters;

static const char *third_party_content = "Hello, world!";

static struct streamed_resource_type_t resource_streamed_a = {42u};
static struct streamed_resource_type_t resource_streamed_b = {169u};

static struct transitive_resource_type_t resource_transitive = {255u};

static void initialize_resources (void)
{
    resource_players = (struct second_resource_type_t) {
        .first_id = kan_string_intern ("konrad"),
        .second_id = kan_string_intern ("alfred"),
    };

    resource_characters = (struct second_resource_type_t) {
        .first_id = kan_string_intern ("warrior"),
        .second_id = kan_string_intern ("trader"),
    };
}

static void save_rd (const char *path, void *instance, kan_interned_string_t type, kan_reflection_registry_t registry)
{
    struct kan_stream_t *stream = kan_direct_file_stream_open_for_write (path, true);
    KAN_TEST_ASSERT (stream)
    KAN_TEST_ASSERT (kan_serialization_rd_write_type_header (stream, type))
    CUSHION_DEFER { stream->operations->close (stream); }

    kan_serialization_rd_writer_t writer = kan_serialization_rd_writer_create (stream, instance, type, registry);
    CUSHION_DEFER { kan_serialization_rd_writer_destroy (writer); }
    enum kan_serialization_state_t state;

    while ((state = kan_serialization_rd_writer_step (writer)) == KAN_SERIALIZATION_IN_PROGRESS)
    {
    }

    KAN_TEST_ASSERT (state == KAN_SERIALIZATION_FINISHED);
}

static void save_third_party (const char *path, const void *data, kan_memory_size_t length)
{
    struct kan_stream_t *stream = kan_direct_file_stream_open_for_write (path, true);
    KAN_TEST_ASSERT (stream)
    KAN_TEST_ASSERT (stream->operations->write (stream, length, data))
    stream->operations->close (stream);
}

static void initialize_platform_configuration (kan_reflection_registry_t registry)
{
    kan_file_system_remove_directory_with_content (PLATFORM_CONFIGURATION_DIRECTORY);
    KAN_TEST_CHECK (kan_file_system_make_directory (PLATFORM_CONFIGURATION_DIRECTORY))

    struct kan_file_system_path_container_t path;
    kan_file_system_path_container_copy_string (&path, PLATFORM_CONFIGURATION_DIRECTORY);
    kan_file_system_path_container_append (&path, KAN_RESOURCE_PLATFORM_CONFIGURATION_SETUP_FILE);

    struct kan_resource_platform_configuration_setup_t setup;
    kan_resource_platform_configuration_setup_init (&setup);
    CUSHION_DEFER { kan_resource_platform_configuration_setup_shutdown (&setup); }

    kan_dynamic_array_set_capacity (&setup.layers, 1u);
    *(kan_interned_string_t *) kan_dynamic_array_add_last (&setup.layers) = KAN_STATIC_INTERNED_ID_GET (base);
    save_rd (path.path, &setup, KAN_STATIC_INTERNED_ID_GET (kan_resource_platform_configuration_setup_t), registry);
}

#define TEST_TRIGGER_TAG_ONE KAN_STATIC_INTERNED_ID_GET (one)
#define TEST_TRIGGER_TAG_TWO KAN_STATIC_INTERNED_ID_GET (two)

static void setup_default_raw_resources (kan_reflection_registry_t registry)
{
    initialize_resources ();
    kan_file_system_make_directory (RAW_DIRECTORY);

    struct kan_resource_package_t manifest;
    kan_resource_package_init (&manifest);
    CUSHION_DEFER { kan_resource_package_shutdown (&manifest); }

    kan_file_system_make_directory (RAW_DIRECTORY "/essential");
    manifest.level = KAN_RESOURCE_PACKAGE_LEVEL_ESSENTIAL;
    save_rd (RAW_DIRECTORY "/essential/" KAN_RESOURCE_PACKAGE_RAW_FILE_NAME, &manifest,
             KAN_STATIC_INTERNED_ID_GET (kan_resource_package_t), registry);
    save_rd (RAW_DIRECTORY "/essential/one.rd", &resource_one, KAN_STATIC_INTERNED_ID_GET (first_resource_type_t),
             registry);
    save_rd (RAW_DIRECTORY "/essential/transitive.rd", &resource_transitive,
             KAN_STATIC_INTERNED_ID_GET (transitive_resource_type_t), registry);

    kan_file_system_make_directory (RAW_DIRECTORY "/required");
    manifest.level = KAN_RESOURCE_PACKAGE_LEVEL_REQUIRED;
    save_rd (RAW_DIRECTORY "/required/" KAN_RESOURCE_PACKAGE_RAW_FILE_NAME, &manifest,
             KAN_STATIC_INTERNED_ID_GET (kan_resource_package_t), registry);
    save_rd (RAW_DIRECTORY "/required/two.rd", &resource_two, KAN_STATIC_INTERNED_ID_GET (first_resource_type_t),
             registry);
    save_rd (RAW_DIRECTORY "/required/players.rd", &resource_players,
             KAN_STATIC_INTERNED_ID_GET (second_resource_type_t), registry);

    kan_file_system_make_directory (RAW_DIRECTORY "/opt_one");
    manifest.level = KAN_RESOURCE_PACKAGE_LEVEL_OPTIONAL;
    kan_dynamic_array_set_capacity (&manifest.trigger_tags, 1u);
    *(kan_interned_string_t *) kan_dynamic_array_add_last (&manifest.trigger_tags) = TEST_TRIGGER_TAG_ONE;
    save_rd (RAW_DIRECTORY "/opt_one/" KAN_RESOURCE_PACKAGE_RAW_FILE_NAME, &manifest,
             KAN_STATIC_INTERNED_ID_GET (kan_resource_package_t), registry);
    save_rd (RAW_DIRECTORY "/opt_one/three.rd", &resource_three, KAN_STATIC_INTERNED_ID_GET (first_resource_type_t),
             registry);
    save_rd (RAW_DIRECTORY "/opt_one/characters.rd", &resource_characters,
             KAN_STATIC_INTERNED_ID_GET (second_resource_type_t), registry);
    save_rd (RAW_DIRECTORY "/opt_one/streamed_a.rd", &resource_streamed_a,
             KAN_STATIC_INTERNED_ID_GET (streamed_resource_type_t), registry);

    kan_file_system_make_directory (RAW_DIRECTORY "/opt_two");
    ((kan_interned_string_t *) manifest.trigger_tags.data)[0u] = TEST_TRIGGER_TAG_TWO;
    save_rd (RAW_DIRECTORY "/opt_two/" KAN_RESOURCE_PACKAGE_RAW_FILE_NAME, &manifest,
             KAN_STATIC_INTERNED_ID_GET (kan_resource_package_t), registry);
    save_rd (RAW_DIRECTORY "/opt_two/four.rd", &resource_four, KAN_STATIC_INTERNED_ID_GET (first_resource_type_t),
             registry);

    struct third_party_reference_resource_type_t reference;
    reference.third_party_name = kan_string_intern ("data.txt");
    save_rd (RAW_DIRECTORY "/opt_two/third_party_reference.rd", &reference,
             KAN_STATIC_INTERNED_ID_GET (third_party_reference_resource_type_t), registry);
    save_third_party (RAW_DIRECTORY "/opt_two/data.txt", third_party_content, strlen (third_party_content) + 1u);
    save_rd (RAW_DIRECTORY "/opt_two/streamed_b.rd", &resource_streamed_b,
             KAN_STATIC_INTERNED_ID_GET (streamed_resource_type_t), registry);
}

KAN_REFLECTION_IGNORE
enum setup_context_flags_t
{
    SETUP_CONTEXT_WITH_HOT_RELOAD = 1u << 0u,
    SETUP_CONTEXT_MOUNT_DEPLOY = 1u << 1u,
};

static kan_context_t setup_context (enum setup_context_flags_t flags)
{
    kan_context_t context =
        kan_context_create (kan_allocation_group_get_child (kan_allocation_group_root (), "context"));

    struct kan_hot_reload_coordination_system_config_t hot_reload_config;
    kan_hot_reload_coordination_system_config_init (&hot_reload_config);

    if (flags & SETUP_CONTEXT_WITH_HOT_RELOAD)
    {
        KAN_TEST_CHECK (
            kan_context_request_system (context, KAN_CONTEXT_HOT_RELOAD_COORDINATION_SYSTEM_NAME, &hot_reload_config))
    }

    KAN_TEST_CHECK (kan_context_request_system (context, KAN_CONTEXT_REFLECTION_SYSTEM_NAME, NULL))
    KAN_TEST_CHECK (kan_context_request_system (context, KAN_CONTEXT_UNIVERSE_SYSTEM_NAME, NULL))
    KAN_TEST_CHECK (kan_context_request_system (context, KAN_CONTEXT_UPDATE_SYSTEM_NAME, NULL))

    struct kan_virtual_file_system_config_t virtual_file_system_config;
    kan_virtual_file_system_config_init (&virtual_file_system_config);
    CUSHION_DEFER { kan_virtual_file_system_config_shutdown (&virtual_file_system_config); }

    struct kan_file_system_path_container_t path_container;
    kan_file_system_path_container_copy_string (&path_container, WORKSPACE_DIRECTORY);

    if (flags & SETUP_CONTEXT_MOUNT_DEPLOY)
    {
        kan_dynamic_array_set_capacity (&virtual_file_system_config.mount_real, 1u);

        struct kan_virtual_file_system_config_mount_real_t *workspace =
            kan_dynamic_array_add_last (&virtual_file_system_config.mount_real);

        KAN_ASSERT (workspace)
        workspace->mount_path = kan_string_intern (RESOURCE_MOUNT_PATH);
        kan_file_system_path_container_append (&path_container, KAN_RESOURCE_PROJECT_WORKSPACE_DEPLOY_DIRECTORY);
        workspace->real_path = kan_string_intern (path_container.path);
    }

    KAN_TEST_CHECK (
        kan_context_request_system (context, KAN_CONTEXT_VIRTUAL_FILE_SYSTEM_NAME, &virtual_file_system_config))

    kan_context_assembly (context);
    return context;
}

static void execute_resource_build (kan_reflection_registry_t registry, enum kan_resource_build_pack_mode_t pack_mode)
{
    struct kan_resource_project_t project;
    kan_resource_project_init (&project);
    CUSHION_DEFER { kan_resource_project_shutdown (&project); }
    kan_dynamic_array_set_capacity (&project.packages, 4u);

    struct kan_resource_project_package_t *essential_package = kan_dynamic_array_add_last (&project.packages);
    kan_resource_project_package_init (essential_package);
    essential_package->name = KAN_STATIC_INTERNED_ID_GET (essential);
    essential_package->group = KAN_RESOURCE_PACKAGE_GROUP_CORE;

    struct kan_file_system_path_container_t container;
    KAN_TEST_ASSERT (kan_file_system_to_absolute_path (RAW_DIRECTORY "/essential", &container))
    essential_package->directory =
        kan_allocate_general (kan_resource_project_get_allocation_group (), container.length + 1u, alignof (char));
    memcpy (essential_package->directory, container.path, container.length + 1u);

    struct kan_resource_project_package_t *required_package = kan_dynamic_array_add_last (&project.packages);
    kan_resource_project_package_init (required_package);
    required_package->name = KAN_STATIC_INTERNED_ID_GET (required);
    required_package->group = KAN_RESOURCE_PACKAGE_GROUP_CORE;

    KAN_TEST_ASSERT (kan_file_system_to_absolute_path (RAW_DIRECTORY "/required", &container))
    required_package->directory =
        kan_allocate_general (kan_resource_project_get_allocation_group (), container.length + 1u, alignof (char));
    memcpy (required_package->directory, container.path, container.length + 1u);

    struct kan_resource_project_package_t *optional_one_package = kan_dynamic_array_add_last (&project.packages);
    kan_resource_project_package_init (optional_one_package);
    optional_one_package->name = KAN_STATIC_INTERNED_ID_GET (optional_one);
    optional_one_package->group = KAN_RESOURCE_PACKAGE_GROUP_CORE;

    KAN_TEST_ASSERT (kan_file_system_to_absolute_path (RAW_DIRECTORY "/opt_one", &container))
    optional_one_package->directory =
        kan_allocate_general (kan_resource_project_get_allocation_group (), container.length + 1u, alignof (char));
    memcpy (optional_one_package->directory, container.path, container.length + 1u);

    struct kan_resource_project_package_t *optional_two_package = kan_dynamic_array_add_last (&project.packages);
    kan_resource_project_package_init (optional_two_package);
    optional_two_package->name = KAN_STATIC_INTERNED_ID_GET (optional_two);
    optional_two_package->group = KAN_RESOURCE_PACKAGE_GROUP_CORE;

    KAN_TEST_ASSERT (kan_file_system_to_absolute_path (RAW_DIRECTORY "/opt_two", &container))
    optional_two_package->directory =
        kan_allocate_general (kan_resource_project_get_allocation_group (), container.length + 1u, alignof (char));
    memcpy (optional_two_package->directory, container.path, container.length + 1u);

    KAN_TEST_ASSERT (kan_file_system_to_absolute_path (WORKSPACE_DIRECTORY, &container))
    project.workspace_directory =
        kan_allocate_general (kan_resource_project_get_allocation_group (), container.length + 1u, alignof (char));
    memcpy (project.workspace_directory, container.path, container.length + 1u);

    KAN_TEST_ASSERT (kan_file_system_to_absolute_path (PLATFORM_CONFIGURATION_DIRECTORY, &container))
    project.platform_configuration_directory =
        kan_allocate_general (kan_resource_project_get_allocation_group (), container.length + 1u, alignof (char));
    memcpy (project.platform_configuration_directory, container.path, container.length + 1u);

    struct kan_resource_reflected_data_storage_t reflected_data;
    kan_resource_reflected_data_storage_build (&reflected_data, registry);
    CUSHION_DEFER { kan_resource_reflected_data_storage_shutdown (&reflected_data); }

    struct kan_resource_build_setup_t setup;
    kan_resource_build_setup_init (&setup);
    CUSHION_DEFER { kan_resource_build_setup_shutdown (&setup); };

    setup.project = &project;
    setup.reflected_data = &reflected_data;
    setup.pack_mode = pack_mode;
    setup.log_verbosity = KAN_LOG_VERBOSE;

    const enum kan_resource_build_result_t result = kan_resource_build (&setup);
    KAN_TEST_ASSERT (result == KAN_RESOURCE_BUILD_RESULT_SUCCESS)
}

static void run_test_loop (kan_context_t context,
                           kan_instance_size_t mutator_names_count,
                           kan_interned_string_t *mutator_names)
{
    kan_context_system_t universe_system_handle = kan_context_query (context, KAN_CONTEXT_UNIVERSE_SYSTEM_NAME);
    KAN_TEST_ASSERT (KAN_HANDLE_IS_VALID (universe_system_handle))

    kan_universe_t universe = kan_universe_system_get_universe (universe_system_handle);
    KAN_TEST_ASSERT (KAN_HANDLE_IS_VALID (universe))

    kan_context_system_t update_system = kan_context_query (context, KAN_CONTEXT_UPDATE_SYSTEM_NAME);
    KAN_TEST_ASSERT (KAN_HANDLE_IS_VALID (update_system))

    kan_context_system_t reflection_system = kan_context_query (context, KAN_CONTEXT_REFLECTION_SYSTEM_NAME);
    KAN_TEST_ASSERT (KAN_HANDLE_IS_VALID (reflection_system))
    kan_reflection_registry_t registry = kan_reflection_system_get_registry (reflection_system);

    struct kan_universe_world_definition_t definition;
    kan_universe_world_definition_init (&definition);
    definition.world_name = KAN_STATIC_INTERNED_ID_GET (root_world);
    definition.scheduler_name = kan_string_intern (KAN_UNIVERSE_TRIVIAL_SCHEDULER_NAME);

    struct kan_resource_provider_configuration_t resource_provider_configuration = {
        .transaction_budget_ns = 14000000u,
        .streaming_budget_ns = 2000000u,
        .resource_directory_path = kan_string_intern (RESOURCE_MOUNT_PATH),
    };

    kan_reflection_patch_builder_t patch_builder = kan_reflection_patch_builder_create ();
    kan_reflection_patch_builder_add_chunk (patch_builder, KAN_REFLECTION_PATCH_BUILDER_SECTION_ROOT, 0u,
                                            sizeof (struct kan_resource_provider_configuration_t),
                                            &resource_provider_configuration);
    kan_reflection_patch_t resource_provider_configuration_patch = kan_reflection_patch_builder_build (
        patch_builder, registry,
        kan_reflection_registry_query_struct (registry,
                                              KAN_STATIC_INTERNED_ID_GET (kan_resource_provider_configuration_t)));
    kan_reflection_patch_builder_destroy (patch_builder);

    kan_dynamic_array_set_capacity (&definition.configuration, 1u);
    struct kan_universe_world_configuration_t *configuration = kan_dynamic_array_add_last (&definition.configuration);
    kan_universe_world_configuration_init (configuration);
    configuration->name = kan_string_intern (KAN_RESOURCE_PROVIDER_CONFIGURATION);
    kan_dynamic_array_set_capacity (&configuration->layers, 1u);

    struct kan_universe_world_configuration_layer_t *variant = kan_dynamic_array_add_last (&configuration->layers);
    kan_universe_world_configuration_layer_init (variant);
    variant->data = resource_provider_configuration_patch;

    kan_dynamic_array_set_capacity (&definition.pipelines, 1u);
    struct kan_universe_world_pipeline_definition_t *update_pipeline =
        kan_dynamic_array_add_last (&definition.pipelines);

    kan_universe_world_pipeline_definition_init (update_pipeline);
    update_pipeline->name = kan_string_intern (KAN_UNIVERSE_TRIVIAL_SCHEDULER_PIPELINE_NAME);

    kan_dynamic_array_set_capacity (&update_pipeline->mutators, mutator_names_count);
    update_pipeline->mutators.size = mutator_names_count;
    memcpy (update_pipeline->mutators.data, mutator_names, sizeof (kan_interned_string_t) * mutator_names_count);

    kan_dynamic_array_set_capacity (&update_pipeline->mutator_groups, 1u);
    *(kan_interned_string_t *) kan_dynamic_array_add_last (&update_pipeline->mutator_groups) =
        kan_string_intern (KAN_RESOURCE_PROVIDER_MUTATOR_GROUP);

    kan_universe_deploy_root (universe, &definition);
    kan_universe_world_definition_shutdown (&definition);

    while (!global_test_finished)
    {
        kan_update_system_run (update_system);
    }
}

struct load_everything_test_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (load_everything_test_state)
    KAN_UM_BIND_STATE (load_everything_test_state, state)
};

TEST_UNIVERSE_RESOURCE_PROVIDER_API KAN_UM_MUTATOR_DEPLOY (load_everything_test)
{
    kan_workflow_graph_node_depend_on (workflow_node, KAN_RESOURCE_PROVIDER_END_CHECKPOINT);
}

TEST_UNIVERSE_RESOURCE_PROVIDER_API KAN_UM_MUTATOR_EXECUTE (load_everything_test)
{
    KAN_UMI_SINGLETON_WRITE (provider, kan_resource_provider_singleton_t)
    if (provider->tags.size == 0u)
    {
        kan_resource_provider_singleton_add_tag (provider, TEST_TRIGGER_TAG_ONE);
        kan_resource_provider_singleton_add_tag (provider, TEST_TRIGGER_TAG_TWO);
    }

    if (provider->tags_dirty || !provider->essential_loading_done ||
        provider->transaction_state != KAN_RESOURCE_TRANSACTION_STATE_NONE)
    {
        // Wait for everything to be processed and loaded.
        return;
    }

    // Everything should be loaded by now, so we can check it.
    kan_interned_string_t id_one = KAN_STATIC_INTERNED_ID_GET (one);
    kan_interned_string_t id_two = KAN_STATIC_INTERNED_ID_GET (two);
    kan_interned_string_t id_three = KAN_STATIC_INTERNED_ID_GET (three);
    kan_interned_string_t id_four = KAN_STATIC_INTERNED_ID_GET (four);
    kan_interned_string_t id_players = KAN_STATIC_INTERNED_ID_GET (players);
    kan_interned_string_t id_characters = KAN_STATIC_INTERNED_ID_GET (characters);
    kan_interned_string_t id_data_txt = kan_string_intern ("data.txt");

    KAN_UMI_RESOURCE_RETRIEVE_LOADED (loaded_one, first_resource_type_t, &id_one)
    KAN_TEST_ASSERT (loaded_one)
    KAN_TEST_CHECK (loaded_one->some_integer == resource_one.some_integer)
    KAN_TEST_CHECK (loaded_one->flag_1 == resource_one.flag_1)
    KAN_TEST_CHECK (loaded_one->flag_2 == resource_one.flag_2)
    KAN_TEST_CHECK (loaded_one->flag_3 == resource_one.flag_3)
    KAN_TEST_CHECK (loaded_one->flag_4 == resource_one.flag_4)

    KAN_UMI_RESOURCE_RETRIEVE_LOADED (loaded_two, first_resource_type_t, &id_two)
    KAN_TEST_ASSERT (loaded_two)
    KAN_TEST_CHECK (loaded_two->some_integer == resource_two.some_integer)
    KAN_TEST_CHECK (loaded_two->flag_1 == resource_two.flag_1)
    KAN_TEST_CHECK (loaded_two->flag_2 == resource_two.flag_2)
    KAN_TEST_CHECK (loaded_two->flag_3 == resource_two.flag_3)
    KAN_TEST_CHECK (loaded_two->flag_4 == resource_two.flag_4)

    KAN_UMI_RESOURCE_RETRIEVE_LOADED (loaded_three, first_resource_type_t, &id_three)
    KAN_TEST_ASSERT (loaded_three)
    KAN_TEST_CHECK (loaded_three->some_integer == resource_three.some_integer)
    KAN_TEST_CHECK (loaded_three->flag_1 == resource_three.flag_1)
    KAN_TEST_CHECK (loaded_three->flag_2 == resource_three.flag_2)
    KAN_TEST_CHECK (loaded_three->flag_3 == resource_three.flag_3)
    KAN_TEST_CHECK (loaded_three->flag_4 == resource_three.flag_4)

    KAN_UMI_RESOURCE_RETRIEVE_LOADED (loaded_four, first_resource_type_t, &id_four)
    KAN_TEST_ASSERT (loaded_four)
    KAN_TEST_CHECK (loaded_four->some_integer == resource_four.some_integer)
    KAN_TEST_CHECK (loaded_four->flag_1 == resource_four.flag_1)
    KAN_TEST_CHECK (loaded_four->flag_2 == resource_four.flag_2)
    KAN_TEST_CHECK (loaded_four->flag_3 == resource_four.flag_3)
    KAN_TEST_CHECK (loaded_four->flag_4 == resource_four.flag_4)

    KAN_UMI_RESOURCE_RETRIEVE_LOADED (loaded_players, second_resource_type_t, &id_players)
    KAN_TEST_ASSERT (loaded_players)
    KAN_TEST_CHECK (loaded_players->first_id == resource_players.first_id)
    KAN_TEST_CHECK (loaded_players->second_id == resource_players.second_id)

    KAN_UMI_RESOURCE_RETRIEVE_LOADED (loaded_characters, second_resource_type_t, &id_characters)
    KAN_TEST_ASSERT (loaded_characters)
    KAN_TEST_CHECK (loaded_characters->first_id == resource_characters.first_id)
    KAN_TEST_CHECK (loaded_characters->second_id == resource_characters.second_id)

    KAN_UMI_RESOURCE_RETRIEVE_LOADED_THIRD_PARTY (loaded_third_party, &id_data_txt)
    KAN_TEST_ASSERT (loaded_third_party)
    KAN_TEST_ASSERT (loaded_third_party_size == (kan_instance_size_t) strlen (third_party_content) + 1u)
    KAN_TEST_CHECK (strcmp (loaded_third_party, third_party_content) == 0)
    global_test_finished = true;
}

KAN_TEST_CASE (load_everything)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_file_system_remove_directory_with_content (WORKSPACE_DIRECTORY);
    kan_file_system_remove_directory_with_content (RAW_DIRECTORY);
    kan_file_system_make_directory (WORKSPACE_DIRECTORY);

    kan_context_t context = setup_context (SETUP_CONTEXT_MOUNT_DEPLOY);
    CUSHION_DEFER { kan_context_destroy (context); }

    kan_context_system_t reflection_system = kan_context_query (context, KAN_CONTEXT_REFLECTION_SYSTEM_NAME);
    KAN_TEST_ASSERT (KAN_HANDLE_IS_VALID (reflection_system))

    kan_reflection_registry_t registry = kan_reflection_system_get_registry (reflection_system);
    initialize_platform_configuration (registry);
    setup_default_raw_resources (registry);
    execute_resource_build (registry, KAN_RESOURCE_BUILD_PACK_MODE_NONE);

    kan_interned_string_t mutators[] = {KAN_STATIC_INTERNED_ID_GET (load_everything_test)};
    run_test_loop (context, sizeof (mutators) / sizeof (mutators[0u]), mutators);
}

KAN_TEST_CASE (load_everything_pack)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_file_system_remove_directory_with_content (WORKSPACE_DIRECTORY);
    kan_file_system_remove_directory_with_content (RAW_DIRECTORY);
    kan_file_system_make_directory (WORKSPACE_DIRECTORY);

    kan_context_t context = setup_context (0u);
    CUSHION_DEFER { kan_context_destroy (context); }

    kan_context_system_t reflection_system = kan_context_query (context, KAN_CONTEXT_REFLECTION_SYSTEM_NAME);
    KAN_TEST_ASSERT (KAN_HANDLE_IS_VALID (reflection_system))

    kan_reflection_registry_t registry = kan_reflection_system_get_registry (reflection_system);
    initialize_platform_configuration (registry);
    setup_default_raw_resources (registry);
    execute_resource_build (registry, KAN_RESOURCE_BUILD_PACK_MODE_INTERNED);

    // And only now we have a pack that we can mount into VFS.
    kan_context_system_t virtual_file_system = kan_context_query (context, KAN_CONTEXT_VIRTUAL_FILE_SYSTEM_NAME);
    KAN_TEST_ASSERT (KAN_HANDLE_IS_VALID (virtual_file_system))

    {
        kan_virtual_file_system_volume_t volume =
            kan_virtual_file_system_get_context_volume_for_write (virtual_file_system);
        CUSHION_DEFER { kan_virtual_file_system_close_context_write_access (virtual_file_system); }
        kan_virtual_file_system_make_directory (volume, RESOURCE_MOUNT_PATH);

        struct kan_file_system_path_container_t path_container;
        kan_file_system_path_container_copy_string (&path_container, WORKSPACE_DIRECTORY);
        kan_resource_build_append_pack_path_in_workspace (&path_container, "essential");
        kan_virtual_file_system_volume_mount_read_only_pack (volume, RESOURCE_MOUNT_PATH "/essential",
                                                             path_container.path);

        kan_file_system_path_container_copy_string (&path_container, WORKSPACE_DIRECTORY);
        kan_resource_build_append_pack_path_in_workspace (&path_container, "required");
        kan_virtual_file_system_volume_mount_read_only_pack (volume, RESOURCE_MOUNT_PATH "/required",
                                                             path_container.path);

        kan_file_system_path_container_copy_string (&path_container, WORKSPACE_DIRECTORY);
        kan_resource_build_append_pack_path_in_workspace (&path_container, "optional_one");
        kan_virtual_file_system_volume_mount_read_only_pack (volume, RESOURCE_MOUNT_PATH "/optional_one",
                                                             path_container.path);

        kan_file_system_path_container_copy_string (&path_container, WORKSPACE_DIRECTORY);
        kan_resource_build_append_pack_path_in_workspace (&path_container, "optional_two");
        kan_virtual_file_system_volume_mount_read_only_pack (volume, RESOURCE_MOUNT_PATH "/optional_two",
                                                             path_container.path);
    }

    kan_interned_string_t mutators[] = {KAN_STATIC_INTERNED_ID_GET (load_everything_test)};
    run_test_loop (context, sizeof (mutators) / sizeof (mutators[0u]), mutators);
}

struct biggest_some_integer_singleton_t
{
    uint64_t value;
};

TEST_UNIVERSE_RESOURCE_PROVIDER_API void biggest_some_integer_singleton_init (
    struct biggest_some_integer_singleton_t *instance)
{
    instance->value = 0u;
}

struct biggest_some_integer_test_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (biggest_some_integer_test_state)
    KAN_UM_BIND_STATE (biggest_some_integer_test_state, state)
};

TEST_UNIVERSE_RESOURCE_PROVIDER_API KAN_UM_MUTATOR_DEPLOY (biggest_some_integer_test)
{
    kan_workflow_graph_node_depend_on (workflow_node, KAN_RESOURCE_PROVIDER_END_CHECKPOINT);
}

TEST_UNIVERSE_RESOURCE_PROVIDER_API KAN_UM_MUTATOR_EXECUTE (biggest_some_integer_test)
{
    bool recalculate = false;
    KAN_UML_EVENT_FETCH (event, kan_resource_transaction_commit_started_event_t) { recalculate = true; }

    if (!recalculate)
    {
        return;
    }

    KAN_UMI_SINGLETON_WRITE (singleton, biggest_some_integer_singleton_t)
    singleton->value = 0u;
    kan_interned_string_t first_type = KAN_STATIC_INTERNED_ID_GET (first_resource_type_t);

    KAN_UML_VALUE_READ (registered, kan_resource_registered_entry_t, type, &first_type)
    {
        KAN_UMI_RESOURCE_RETRIEVE_FRESH_LOADED (loaded, first_resource_type_t, &registered->name)
        if (loaded)
        {
            singleton->value = KAN_MAX (singleton->value, loaded->some_integer);
        }
    }
}

enum load_step_by_step_state_t
{
    LOAD_STEP_BY_STEP_WAITING_ESSENTIAL = 0u,
    LOAD_STEP_BY_STEP_WAITING_REQUIRED,
    LOAD_STEP_BY_STEP_WAITING_OPT_ONE,
    LOAD_STEP_BY_STEP_WAITING_OPT_TWO,
    LOAD_STEP_BY_STEP_DONE,
};

struct load_step_by_step_singleton_t
{
    enum load_step_by_step_state_t state;
};

TEST_UNIVERSE_RESOURCE_PROVIDER_API void load_step_by_step_singleton_init (
    struct load_step_by_step_singleton_t *instance)
{
    instance->state = LOAD_STEP_BY_STEP_WAITING_ESSENTIAL;
}

struct load_step_by_step_test_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (load_step_by_step_test_state)
    KAN_UM_BIND_STATE (load_step_by_step_test_state, state)
};

TEST_UNIVERSE_RESOURCE_PROVIDER_API KAN_UM_MUTATOR_DEPLOY (load_step_by_step_test)
{
    kan_workflow_graph_node_depend_on (workflow_node, "biggest_some_integer_test");
}

TEST_UNIVERSE_RESOURCE_PROVIDER_API KAN_UM_MUTATOR_EXECUTE (load_step_by_step_test)
{
    KAN_UMI_SINGLETON_WRITE (singleton, load_step_by_step_singleton_t)
    KAN_UMI_SINGLETON_WRITE (provider, kan_resource_provider_singleton_t)
    KAN_UMI_SINGLETON_READ (biggest_some_integer, biggest_some_integer_singleton_t)

    switch (singleton->state)
    {
    case LOAD_STEP_BY_STEP_WAITING_ESSENTIAL:
        if (provider->essential_loading_done)
        {
            KAN_TEST_CHECK (biggest_some_integer->value == resource_one.some_integer)
            singleton->state = LOAD_STEP_BY_STEP_WAITING_REQUIRED;
        }

        break;

    case LOAD_STEP_BY_STEP_WAITING_REQUIRED:
        if (provider->transaction_state == KAN_RESOURCE_TRANSACTION_STATE_NONE)
        {
            KAN_TEST_CHECK (biggest_some_integer->value == resource_two.some_integer)
            singleton->state = LOAD_STEP_BY_STEP_WAITING_OPT_ONE;
            kan_resource_provider_singleton_add_tag (provider, TEST_TRIGGER_TAG_ONE);
        }

        break;

    case LOAD_STEP_BY_STEP_WAITING_OPT_ONE:
        if (provider->transaction_state == KAN_RESOURCE_TRANSACTION_STATE_NONE)
        {
            KAN_TEST_CHECK (biggest_some_integer->value == resource_two.some_integer)
            singleton->state = LOAD_STEP_BY_STEP_WAITING_OPT_TWO;
            kan_resource_provider_singleton_add_tag (provider, TEST_TRIGGER_TAG_TWO);
        }

        break;

    case LOAD_STEP_BY_STEP_WAITING_OPT_TWO:
        if (provider->transaction_state == KAN_RESOURCE_TRANSACTION_STATE_NONE)
        {
            KAN_TEST_CHECK (biggest_some_integer->value == resource_four.some_integer)
            singleton->state = LOAD_STEP_BY_STEP_DONE;
            global_test_finished = true;
        }

        break;

    case LOAD_STEP_BY_STEP_DONE:
        break;
    }
}

KAN_TEST_CASE (load_step_by_step)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_file_system_remove_directory_with_content (WORKSPACE_DIRECTORY);
    kan_file_system_remove_directory_with_content (RAW_DIRECTORY);
    kan_file_system_make_directory (WORKSPACE_DIRECTORY);

    kan_context_t context = setup_context (SETUP_CONTEXT_MOUNT_DEPLOY);
    CUSHION_DEFER { kan_context_destroy (context); }

    kan_context_system_t reflection_system = kan_context_query (context, KAN_CONTEXT_REFLECTION_SYSTEM_NAME);
    KAN_TEST_ASSERT (KAN_HANDLE_IS_VALID (reflection_system))

    kan_reflection_registry_t registry = kan_reflection_system_get_registry (reflection_system);
    initialize_platform_configuration (registry);
    setup_default_raw_resources (registry);
    execute_resource_build (registry, KAN_RESOURCE_BUILD_PACK_MODE_NONE);

    kan_interned_string_t mutators[] = {KAN_STATIC_INTERNED_ID_GET (biggest_some_integer_test),
                                        KAN_STATIC_INTERNED_ID_GET (load_step_by_step_test)};
    run_test_loop (context, sizeof (mutators) / sizeof (mutators[0u]), mutators);
}

enum streaming_state_t
{
    STREAMING_STATE_INIT = 0u,
    STREAMING_STATE_WAITING_A,
    STREAMING_STATE_WAITING_B,
    STREAMING_STATE_WAITING_B_UNLOAD,
    STREAMING_STATE_WAITING_B_AGAIN,
    STREAMING_STATE_DONE,
};

struct streaming_singleton_t
{
    enum streaming_state_t state;
    kan_resource_streaming_id_t requst_a_1_id;
    kan_resource_streaming_id_t requst_a_2_id;
    kan_resource_streaming_id_t requst_b_id;
};

TEST_UNIVERSE_RESOURCE_PROVIDER_API void streaming_singleton_init (struct streaming_singleton_t *instance)
{
    instance->state = STREAMING_STATE_INIT;
    instance->requst_a_1_id = KAN_TYPED_ID_32_SET_INVALID (kan_resource_streaming_id_t);
    instance->requst_a_2_id = KAN_TYPED_ID_32_SET_INVALID (kan_resource_streaming_id_t);
    instance->requst_b_id = KAN_TYPED_ID_32_SET_INVALID (kan_resource_streaming_id_t);
}

struct streaming_test_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (streaming_test_state)
    KAN_UM_BIND_STATE (streaming_test_state, state)
};

TEST_UNIVERSE_RESOURCE_PROVIDER_API KAN_UM_MUTATOR_DEPLOY (streaming_test)
{
    kan_workflow_graph_node_depend_on (workflow_node, KAN_RESOURCE_PROVIDER_END_CHECKPOINT);
}

TEST_UNIVERSE_RESOURCE_PROVIDER_API KAN_UM_MUTATOR_EXECUTE (streaming_test)
{
    KAN_UMI_SINGLETON_READ (provider, kan_resource_provider_singleton_t)
    KAN_UMI_SINGLETON_WRITE (singleton, streaming_singleton_t)

    switch (singleton->state)
    {
    case STREAMING_STATE_INIT:
    {
        KAN_UMI_INDEXED_INSERT (request_1, kan_resource_streaming_request_t)
        singleton->requst_a_1_id = kan_next_resource_streaming_id (provider);
        request_1->request_id = singleton->requst_a_1_id;
        request_1->type = KAN_STATIC_INTERNED_ID_GET (streamed_resource_type_t);
        request_1->name = KAN_STATIC_INTERNED_ID_GET (streamed_a);
        request_1->priority = 50u;

        KAN_UMI_INDEXED_INSERT (request_2, kan_resource_streaming_request_t)
        singleton->requst_a_2_id = kan_next_resource_streaming_id (provider);
        request_2->request_id = singleton->requst_a_2_id;
        request_2->type = KAN_STATIC_INTERNED_ID_GET (streamed_resource_type_t);
        request_2->name = KAN_STATIC_INTERNED_ID_GET (streamed_a);
        request_2->priority = 60u;

        singleton->state = STREAMING_STATE_WAITING_A;
        break;
    }

    case STREAMING_STATE_WAITING_A:
    {
        const kan_interned_string_t name = KAN_STATIC_INTERNED_ID_GET (streamed_a);
        KAN_UMI_RESOURCE_RETRIEVE_LOADED (streamed_a, streamed_resource_type_t, &name)

        if (streamed_a)
        {
            KAN_TEST_CHECK (streamed_a->value == resource_streamed_a.value)

            KAN_UMI_INDEXED_INSERT (request, kan_resource_streaming_request_t)
            singleton->requst_b_id = kan_next_resource_streaming_id (provider);
            request->request_id = singleton->requst_b_id;
            request->type = KAN_STATIC_INTERNED_ID_GET (streamed_resource_type_t);
            request->name = KAN_STATIC_INTERNED_ID_GET (streamed_b);
            request->priority = 90u;

            singleton->state = STREAMING_STATE_WAITING_B;
        }

        break;
    }

    case STREAMING_STATE_WAITING_B:
    {
        const kan_interned_string_t name = KAN_STATIC_INTERNED_ID_GET (streamed_b);
        KAN_UMI_RESOURCE_RETRIEVE_LOADED (streamed_b, streamed_resource_type_t, &name)

        if (streamed_b)
        {
            KAN_TEST_CHECK (streamed_b->value == resource_streamed_b.value)
            KAN_UMI_VALUE_DETACH_REQUIRED (request_a, kan_resource_streaming_request_t, request_id,
                                           &singleton->requst_a_1_id)
            KAN_UM_ACCESS_DELETE (request_a);

            KAN_UMI_VALUE_DETACH_REQUIRED (request_b, kan_resource_streaming_request_t, request_id,
                                           &singleton->requst_b_id)
            KAN_UM_ACCESS_DELETE (request_b);
            singleton->state = STREAMING_STATE_WAITING_B_UNLOAD;
        }

        break;
    }

    case STREAMING_STATE_WAITING_B_UNLOAD:
    {
        const kan_interned_string_t name_b = KAN_STATIC_INTERNED_ID_GET (streamed_b);
        KAN_UMI_RESOURCE_RETRIEVE_LOADED (streamed_b, streamed_resource_type_t, &name_b)

        if (!streamed_b)
        {
            const kan_interned_string_t name_a = KAN_STATIC_INTERNED_ID_GET (streamed_a);
            KAN_UMI_RESOURCE_RETRIEVE_LOADED (streamed_a, streamed_resource_type_t, &name_a)
            KAN_TEST_CHECK (streamed_a) // We still should have one request for streamed_a.

            KAN_UMI_INDEXED_INSERT (request, kan_resource_streaming_request_t)
            singleton->requst_b_id = kan_next_resource_streaming_id (provider);
            request->request_id = singleton->requst_b_id;
            request->type = KAN_STATIC_INTERNED_ID_GET (streamed_resource_type_t);
            request->name = KAN_STATIC_INTERNED_ID_GET (streamed_b);
            request->priority = 90u;

            singleton->state = STREAMING_STATE_WAITING_B_AGAIN;
        }

        break;
    }

    case STREAMING_STATE_WAITING_B_AGAIN:
    {
        const kan_interned_string_t name_b = KAN_STATIC_INTERNED_ID_GET (streamed_b);
        KAN_UMI_RESOURCE_RETRIEVE_LOADED (streamed_b, streamed_resource_type_t, &name_b)

        if (streamed_b)
        {
            KAN_TEST_CHECK (streamed_b->value == resource_streamed_b.value)

            const kan_interned_string_t name_a = KAN_STATIC_INTERNED_ID_GET (streamed_a);
            KAN_UMI_RESOURCE_RETRIEVE_LOADED (streamed_a, streamed_resource_type_t, &name_a)
            KAN_TEST_ASSERT (streamed_a) // We still should have one request for streamed_a.
            KAN_TEST_CHECK (streamed_a->value == resource_streamed_a.value)

            singleton->state = STREAMING_STATE_DONE;
            global_test_finished = true;
        }

        break;
    }

    case STREAMING_STATE_DONE:
        break;
    }
}

KAN_TEST_CASE (streaming)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_file_system_remove_directory_with_content (WORKSPACE_DIRECTORY);
    kan_file_system_remove_directory_with_content (RAW_DIRECTORY);
    kan_file_system_make_directory (WORKSPACE_DIRECTORY);

    kan_context_t context = setup_context (SETUP_CONTEXT_MOUNT_DEPLOY);
    CUSHION_DEFER { kan_context_destroy (context); }

    kan_context_system_t reflection_system = kan_context_query (context, KAN_CONTEXT_REFLECTION_SYSTEM_NAME);
    KAN_TEST_ASSERT (KAN_HANDLE_IS_VALID (reflection_system))

    kan_reflection_registry_t registry = kan_reflection_system_get_registry (reflection_system);
    initialize_platform_configuration (registry);
    setup_default_raw_resources (registry);
    execute_resource_build (registry, KAN_RESOURCE_BUILD_PACK_MODE_NONE);

    kan_interned_string_t mutators[] = {KAN_STATIC_INTERNED_ID_GET (streaming_test)};
    run_test_loop (context, sizeof (mutators) / sizeof (mutators[0u]), mutators);
}

struct load_transitive_test_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (load_transitive_test_state)
    KAN_UM_BIND_STATE (load_transitive_test_state, state)
};

TEST_UNIVERSE_RESOURCE_PROVIDER_API KAN_UM_MUTATOR_DEPLOY (load_transitive_test)
{
    kan_workflow_graph_node_depend_on (workflow_node, KAN_RESOURCE_PROVIDER_END_CHECKPOINT);
}

TEST_UNIVERSE_RESOURCE_PROVIDER_API KAN_UM_MUTATOR_EXECUTE (load_transitive_test)
{
    const kan_interned_string_t name = KAN_STATIC_INTERNED_ID_GET (transitive);
    KAN_UML_EVENT_FETCH (commit_event, kan_resource_transaction_commit_started_event_t)
    {
        KAN_UMI_RESOURCE_RETRIEVE_FRESH_LOADED (resource, transitive_resource_type_t, &name)
        KAN_TEST_ASSERT (resource)
        KAN_TEST_CHECK (resource->value == resource_transitive.value);
    }

    KAN_UML_EVENT_FETCH (finished_event, kan_resource_transaction_commit_finished_event_t)
    {
        KAN_UMI_RESOURCE_RETRIEVE_FRESH_LOADED (resource, transitive_resource_type_t, &name)
        KAN_TEST_CHECK (!resource)
        global_test_finished = true;
    }
}

KAN_TEST_CASE (load_transitive)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_file_system_remove_directory_with_content (WORKSPACE_DIRECTORY);
    kan_file_system_remove_directory_with_content (RAW_DIRECTORY);
    kan_file_system_make_directory (WORKSPACE_DIRECTORY);

    kan_context_t context = setup_context (SETUP_CONTEXT_MOUNT_DEPLOY);
    CUSHION_DEFER { kan_context_destroy (context); }

    kan_context_system_t reflection_system = kan_context_query (context, KAN_CONTEXT_REFLECTION_SYSTEM_NAME);
    KAN_TEST_ASSERT (KAN_HANDLE_IS_VALID (reflection_system))

    kan_reflection_registry_t registry = kan_reflection_system_get_registry (reflection_system);
    initialize_platform_configuration (registry);
    setup_default_raw_resources (registry);
    execute_resource_build (registry, KAN_RESOURCE_BUILD_PACK_MODE_NONE);

    kan_interned_string_t mutators[] = {KAN_STATIC_INTERNED_ID_GET (load_transitive_test)};
    run_test_loop (context, sizeof (mutators) / sizeof (mutators[0u]), mutators);
}

enum hot_reload_state_t
{
    HOT_RELOAD_STATE_WAITING_PACKAGED_INITIAL,
    HOT_RELOAD_STATE_WAITING_STREAMED_INITIAL,
    HOT_RELOAD_STATE_WAITING_PACKAGED_RELOAD,
    HOT_RELOAD_STATE_WAITING_STREAMED_RELOAD,
    HOT_RELOAD_STATE_DONE,
};

struct hot_reload_test_singleton_t
{
    enum hot_reload_state_t state;
    bool trying_to_build;
};

TEST_UNIVERSE_RESOURCE_PROVIDER_API void hot_reload_test_singleton_init (struct hot_reload_test_singleton_t *instance)
{
    instance->state = HOT_RELOAD_STATE_WAITING_PACKAGED_INITIAL;
    instance->trying_to_build = false;
}

struct hot_reload_test_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (hot_reload_test_state)
    KAN_UM_BIND_STATE (hot_reload_test_state, state)

    kan_reflection_registry_t registry;
    kan_context_system_t hot_reload_coordination_system;
};

TEST_UNIVERSE_RESOURCE_PROVIDER_API KAN_UM_MUTATOR_DEPLOY (hot_reload_test)
{
    state->registry = kan_universe_get_reflection_registry (universe);
    state->hot_reload_coordination_system =
        kan_context_query (kan_universe_get_context (universe), KAN_CONTEXT_HOT_RELOAD_COORDINATION_SYSTEM_NAME);
    KAN_TEST_ASSERT (KAN_HANDLE_IS_VALID (state->hot_reload_coordination_system))
    kan_workflow_graph_node_depend_on (workflow_node, KAN_RESOURCE_PROVIDER_END_CHECKPOINT);
}

static void modify_test_resources (kan_reflection_registry_t registry)
{
    save_rd (RAW_DIRECTORY "/essential/one.rd", &resource_three, KAN_STATIC_INTERNED_ID_GET (first_resource_type_t),
             registry);
    kan_file_system_remove_file (RAW_DIRECTORY "/required/two.rd");
    save_rd (RAW_DIRECTORY "/required/something.rd", &resource_four, KAN_STATIC_INTERNED_ID_GET (first_resource_type_t),
             registry);
    save_rd (RAW_DIRECTORY "/required/players.rd", &resource_characters,
             KAN_STATIC_INTERNED_ID_GET (second_resource_type_t), registry);
    save_rd (RAW_DIRECTORY "/opt_one/streamed_a.rd", &resource_streamed_b,
             KAN_STATIC_INTERNED_ID_GET (streamed_resource_type_t), registry);
}

TEST_UNIVERSE_RESOURCE_PROVIDER_API KAN_UM_MUTATOR_EXECUTE (hot_reload_test)
{
    KAN_UMI_SINGLETON_READ (provider, kan_resource_provider_singleton_t)
    KAN_UMI_SINGLETON_WRITE (singleton, hot_reload_test_singleton_t)

    if (singleton->trying_to_build)
    {
        if (kan_hot_reload_coordination_system_is_executing (state->hot_reload_coordination_system))
        {
            execute_resource_build (state->registry, KAN_RESOURCE_BUILD_PACK_MODE_NONE);
            kan_hot_reload_coordination_system_finish (state->hot_reload_coordination_system);
            singleton->trying_to_build = false;
        }
        else if (!kan_hot_reload_coordination_system_is_scheduled (state->hot_reload_coordination_system))
        {
            kan_hot_reload_coordination_system_schedule (state->hot_reload_coordination_system);
        }

        return;
    }

    kan_interned_string_t id_one = KAN_STATIC_INTERNED_ID_GET (one);
    kan_interned_string_t id_two = KAN_STATIC_INTERNED_ID_GET (two);
    kan_interned_string_t id_something = KAN_STATIC_INTERNED_ID_GET (something);
    kan_interned_string_t id_players = KAN_STATIC_INTERNED_ID_GET (players);
    kan_interned_string_t id_streamed_a = KAN_STATIC_INTERNED_ID_GET (streamed_a);

    switch (singleton->state)
    {
    case HOT_RELOAD_STATE_WAITING_PACKAGED_INITIAL:
    {
        KAN_UML_EVENT_FETCH (event, kan_resource_transaction_commit_finished_event_t)
        {
            if (!provider->essential_loading_done || !provider->required_loading_done)
            {
                continue;
            }

            KAN_UMI_RESOURCE_RETRIEVE_LOADED (loaded_one, first_resource_type_t, &id_one)
            KAN_TEST_ASSERT (loaded_one)
            KAN_TEST_CHECK (loaded_one->some_integer == resource_one.some_integer)
            KAN_TEST_CHECK (loaded_one->flag_1 == resource_one.flag_1)
            KAN_TEST_CHECK (loaded_one->flag_2 == resource_one.flag_2)
            KAN_TEST_CHECK (loaded_one->flag_3 == resource_one.flag_3)
            KAN_TEST_CHECK (loaded_one->flag_4 == resource_one.flag_4)

            KAN_UMI_RESOURCE_RETRIEVE_LOADED (loaded_two, first_resource_type_t, &id_two)
            KAN_TEST_ASSERT (loaded_two)
            KAN_TEST_CHECK (loaded_two->some_integer == resource_two.some_integer)
            KAN_TEST_CHECK (loaded_two->flag_1 == resource_two.flag_1)
            KAN_TEST_CHECK (loaded_two->flag_2 == resource_two.flag_2)
            KAN_TEST_CHECK (loaded_two->flag_3 == resource_two.flag_3)
            KAN_TEST_CHECK (loaded_two->flag_4 == resource_two.flag_4)

            KAN_UMI_RESOURCE_RETRIEVE_LOADED (loaded_players, second_resource_type_t, &id_players)
            KAN_TEST_ASSERT (loaded_players)
            KAN_TEST_CHECK (loaded_players->first_id == resource_players.first_id)
            KAN_TEST_CHECK (loaded_players->second_id == resource_players.second_id)

            KAN_UMI_INDEXED_INSERT (request, kan_resource_streaming_request_t)
            request->request_id = kan_next_resource_streaming_id (provider);
            request->type = KAN_STATIC_INTERNED_ID_GET (streamed_resource_type_t);
            request->name = id_streamed_a;
            request->priority = 50u;

            singleton->state = HOT_RELOAD_STATE_WAITING_STREAMED_INITIAL;
        }

        break;
    }

    case HOT_RELOAD_STATE_WAITING_STREAMED_INITIAL:
    {
        KAN_UML_RESOURCE_LOADED_EVENT_FETCH (loaded_event, streamed_resource_type_t)
        {
            KAN_UMI_RESOURCE_RETRIEVE_LOADED (loaded, streamed_resource_type_t, &id_streamed_a)
            KAN_TEST_ASSERT (loaded)
            KAN_TEST_CHECK (loaded->value == resource_streamed_a.value)

            modify_test_resources (state->registry);
            singleton->state = HOT_RELOAD_STATE_WAITING_PACKAGED_RELOAD;
            singleton->trying_to_build = true;
        }

        break;
    }

    case HOT_RELOAD_STATE_WAITING_PACKAGED_RELOAD:
    {
        KAN_UML_EVENT_FETCH (event, kan_resource_transaction_commit_finished_event_t)
        {
            if (!provider->essential_loading_done || !provider->required_loading_done)
            {
                continue;
            }

            KAN_UMI_RESOURCE_RETRIEVE_LOADED (loaded_one, first_resource_type_t, &id_one)
            KAN_TEST_ASSERT (loaded_one)
            KAN_TEST_CHECK (loaded_one->some_integer == resource_three.some_integer)
            KAN_TEST_CHECK (loaded_one->flag_1 == resource_three.flag_1)
            KAN_TEST_CHECK (loaded_one->flag_2 == resource_three.flag_2)
            KAN_TEST_CHECK (loaded_one->flag_3 == resource_three.flag_3)
            KAN_TEST_CHECK (loaded_one->flag_4 == resource_three.flag_4)

            KAN_UMI_RESOURCE_RETRIEVE_LOADED (loaded_two, first_resource_type_t, &id_two)
            KAN_TEST_CHECK (!loaded_two)

            KAN_UMI_RESOURCE_RETRIEVE_LOADED (loaded_something, first_resource_type_t, &id_something)
            KAN_TEST_ASSERT (loaded_something)
            KAN_TEST_CHECK (loaded_something->some_integer == resource_four.some_integer)
            KAN_TEST_CHECK (loaded_something->flag_1 == resource_four.flag_1)
            KAN_TEST_CHECK (loaded_something->flag_2 == resource_four.flag_2)
            KAN_TEST_CHECK (loaded_something->flag_3 == resource_four.flag_3)
            KAN_TEST_CHECK (loaded_something->flag_4 == resource_four.flag_4)

            KAN_UMI_RESOURCE_RETRIEVE_LOADED (loaded_players, second_resource_type_t, &id_players)
            KAN_TEST_ASSERT (loaded_players)
            KAN_TEST_CHECK (loaded_players->first_id == resource_characters.first_id)
            KAN_TEST_CHECK (loaded_players->second_id == resource_characters.second_id)

            singleton->state = HOT_RELOAD_STATE_WAITING_STREAMED_RELOAD;
        }

        break;
    }

    case HOT_RELOAD_STATE_WAITING_STREAMED_RELOAD:
    {
        KAN_UML_RESOURCE_LOADED_EVENT_FETCH (loaded_event, streamed_resource_type_t)
        {
            KAN_UMI_RESOURCE_RETRIEVE_LOADED (loaded, streamed_resource_type_t, &id_streamed_a)
            KAN_TEST_ASSERT (loaded)
            KAN_TEST_CHECK (loaded->value == resource_streamed_b.value)
            singleton->state = HOT_RELOAD_STATE_DONE;
            global_test_finished = true;
        }

        break;
    }

    case HOT_RELOAD_STATE_DONE:
        break;
    }
}

KAN_TEST_CASE (hot_reload)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_file_system_remove_directory_with_content (WORKSPACE_DIRECTORY);
    kan_file_system_remove_directory_with_content (RAW_DIRECTORY);
    kan_file_system_make_directory (WORKSPACE_DIRECTORY);

    kan_context_t context = setup_context (SETUP_CONTEXT_WITH_HOT_RELOAD | SETUP_CONTEXT_MOUNT_DEPLOY);
    CUSHION_DEFER { kan_context_destroy (context); }

    kan_context_system_t reflection_system = kan_context_query (context, KAN_CONTEXT_REFLECTION_SYSTEM_NAME);
    KAN_TEST_ASSERT (KAN_HANDLE_IS_VALID (reflection_system))

    kan_reflection_registry_t registry = kan_reflection_system_get_registry (reflection_system);
    initialize_platform_configuration (registry);
    setup_default_raw_resources (registry);
    execute_resource_build (registry, KAN_RESOURCE_BUILD_PACK_MODE_NONE);

    kan_interned_string_t mutators[] = {KAN_STATIC_INTERNED_ID_GET (hot_reload_test)};
    run_test_loop (context, sizeof (mutators) / sizeof (mutators[0u]), mutators);
}
