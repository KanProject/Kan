#include <application_framework_examples_text_effects_api.h>

#include <stdio.h>
#include <string.h>

#include <kan/context/all_system_names.h>
#include <kan/context/application_framework_system.h>
#include <kan/context/application_system.h>
#include <kan/file_system/path_container.h>
#include <kan/file_system/stream.h>
#include <kan/image/image.h>
#include <kan/inline_math/inline_math.h>
#include <kan/log/logging.h>
#include <kan/precise_time/precise_time.h>
#include <kan/resource_pipeline/meta.h>
#include <kan/test_expectation/test_expectation.h>
#include <kan/test_routine/test_routine.h>
#include <kan/universe/macro.h>
#include <kan/universe_locale/locale.h>
#include <kan/universe_render_foundation/program.h>
#include <kan/universe_render_foundation/render_graph.h>
#include <kan/universe_render_foundation/texture.h>
#include <kan/universe_resource_provider/provider.h>
#include <kan/universe_text/text.h>

KAN_LOG_DEFINE_CATEGORY (application_framework_example_text_effects);
KAN_USE_STATIC_INTERNED_IDS

#define KAN_TEXT_EFFECTS_MUTATOR_GROUP "text_effects"
KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (text_effects_update)
KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (text_effects_render)
APPLICATION_FRAMEWORK_EXAMPLES_TEXT_EFFECTS_API KAN_UM_MUTATOR_GROUP_META (text_effects,
                                                                           KAN_TEXT_EFFECTS_MUTATOR_GROUP);

#define FIXED_WIDTH 1600u
#define FIXED_HEIGHT 800u

#define TEXT_MARK_PALETTE_MASK 0x000000ff

enum text_mark_flag_t
{
    TEXT_MARK_FLAG_OUTLINE = 1u << 8u,
    TEXT_MARK_FLAG_SIN_ANIMATION = 1u << 9u,
    TEXT_MARK_FLAG_READING_ANIMATION = 1u << 10u,
};

#define TEXT_MAKE_MARK(PALETTE, FLAGS) (((PALETTE) & TEXT_MARK_PALETTE_MASK) | (FLAGS))

struct text_effects_config_t
{
    kan_interned_string_t text_material_instance_name;
};

APPLICATION_FRAMEWORK_EXAMPLES_TEXT_EFFECTS_API void text_effects_config_init (struct text_effects_config_t *instance)
{
    instance->text_material_instance_name = NULL;
}

KAN_REFLECTION_STRUCT_META (text_effects_config_t)
APPLICATION_FRAMEWORK_EXAMPLES_TEXT_EFFECTS_API struct kan_resource_type_meta_t text_effects_config_resource_type_meta =
    {
        .flags = KAN_RESOURCE_TYPE_ROOT,
        .version = CUSHION_START_NS_X64,
        .move = NULL,
        .reset = NULL,
};

KAN_REFLECTION_STRUCT_FIELD_META (text_effects_config_t, text_material_instance_name)
APPLICATION_FRAMEWORK_EXAMPLES_TEXT_EFFECTS_API struct kan_resource_reference_meta_t
    text_effects_config_text_material_instance_name_reference_meta = {
        .type_name = "kan_resource_material_instance_t",
        .flags = 0u,
};

struct text_push_constant_t
{
    struct kan_float_matrix_4x4_t projection_view;
    struct kan_float_vector_4_t offset_and_time;
};

enum text_effects_scene_image_t
{
    TEXT_EFFECTS_SCENE_IMAGE_COLOR = 0u,
    TEXT_EFFECTS_SCENE_IMAGE_COUNT,
};

enum text_effects_scene_frame_buffer_t
{
    TEXT_EFFECTS_SCENE_FRAME_BUFFER_COLOR = 0u,
    TEXT_EFFECTS_SCENE_FRAME_BUFFER_COUNT,
};

struct example_text_effects_singleton_t
{
    kan_application_system_window_t window_handle;
    kan_render_surface_t window_surface;
    kan_resource_usage_id_t config_usage_id;
    kan_render_material_instance_usage_id_t text_material_instance_usage_id;

    kan_text_shaping_unit_id_t stable_text_id;
    kan_text_shaping_unit_id_t dirty_text_id;
    kan_text_shaping_unit_id_t unstable_text_id;
    kan_text_shaping_unit_id_t showcase_text_id;
    kan_text_shaping_unit_id_t reading_text_id;

    kan_instance_size_t dirty_text_last_seconds;
    bool frame_checked;

    bool object_buffers_initialized;
    kan_render_buffer_t glyph_vertex_buffer;
    kan_render_buffer_t glyph_index_buffer;
    kan_instance_size_t glyph_index_count;

    kan_render_frame_lifetime_buffer_allocator_t instanced_data_allocator;
    kan_render_pipeline_parameter_set_t text_shared_parameter_set;
    kan_render_image_t text_last_bound_atlas;
};

APPLICATION_FRAMEWORK_EXAMPLES_TEXT_EFFECTS_API void example_text_effects_singleton_init (
    struct example_text_effects_singleton_t *instance)
{
    instance->window_handle = KAN_HANDLE_SET_INVALID (kan_application_system_window_t);
    instance->window_surface = KAN_HANDLE_SET_INVALID (kan_render_surface_t);
    instance->config_usage_id = KAN_TYPED_ID_32_SET_INVALID (kan_resource_usage_id_t);
    instance->text_material_instance_usage_id = KAN_TYPED_ID_32_SET_INVALID (kan_render_material_instance_usage_id_t);

    instance->stable_text_id = KAN_TYPED_ID_32_SET_INVALID (kan_text_shaping_unit_id_t);
    instance->dirty_text_id = KAN_TYPED_ID_32_SET_INVALID (kan_text_shaping_unit_id_t);
    instance->unstable_text_id = KAN_TYPED_ID_32_SET_INVALID (kan_text_shaping_unit_id_t);
    instance->showcase_text_id = KAN_TYPED_ID_32_SET_INVALID (kan_text_shaping_unit_id_t);
    instance->reading_text_id = KAN_TYPED_ID_32_SET_INVALID (kan_text_shaping_unit_id_t);

    instance->dirty_text_last_seconds = 0u;
    instance->frame_checked = false;

    instance->object_buffers_initialized = false;
    instance->glyph_vertex_buffer = KAN_HANDLE_SET_INVALID (kan_render_buffer_t);
    instance->glyph_index_buffer = KAN_HANDLE_SET_INVALID (kan_render_buffer_t);
    instance->glyph_index_count = 0u;

    instance->instanced_data_allocator = KAN_HANDLE_SET_INVALID (kan_render_frame_lifetime_buffer_allocator_t);
    instance->text_shared_parameter_set = KAN_HANDLE_SET_INVALID (kan_render_pipeline_parameter_set_t);
    instance->text_last_bound_atlas = KAN_HANDLE_SET_INVALID (kan_render_image_t);
}

static void example_text_effects_singleton_initialize_object_buffers (struct example_text_effects_singleton_t *instance,
                                                                      kan_render_context_t render_context)
{
    KAN_ASSERT (!instance->object_buffers_initialized)
    struct kan_float_vector_2_t glyph_vertices[] = {
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f},
    };

    instance->glyph_vertex_buffer =
        kan_render_buffer_create (render_context, KAN_RENDER_BUFFER_TYPE_ATTRIBUTE, sizeof (glyph_vertices),
                                  glyph_vertices, kan_string_intern ("glyph"));

    uint16_t glyph_indices[] = {
        0u, 1u, 2u, 2u, 3u, 0u,
    };

    instance->glyph_index_count = sizeof (glyph_indices) / sizeof (glyph_indices[0u]);
    instance->glyph_index_buffer =
        kan_render_buffer_create (render_context, KAN_RENDER_BUFFER_TYPE_INDEX_16, sizeof (glyph_indices),
                                  glyph_indices, kan_string_intern ("glyph"));

    instance->instanced_data_allocator = kan_render_frame_lifetime_buffer_allocator_create (
        render_context, KAN_RENDER_BUFFER_TYPE_ATTRIBUTE, 1048576u, false,
        KAN_STATIC_INTERNED_ID_GET (instanced_data_allocator));
    instance->object_buffers_initialized = true;
}

APPLICATION_FRAMEWORK_EXAMPLES_TEXT_EFFECTS_API void example_text_effects_singleton_shutdown (
    struct example_text_effects_singleton_t *instance)
{
    if (KAN_HANDLE_IS_VALID (instance->instanced_data_allocator))
    {
        kan_render_frame_lifetime_buffer_allocator_destroy (instance->instanced_data_allocator);
    }

    if (KAN_HANDLE_IS_VALID (instance->text_shared_parameter_set))
    {
        kan_render_pipeline_parameter_set_destroy (instance->text_shared_parameter_set);
    }
}

struct text_effects_update_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (text_effects_update)
    KAN_UM_BIND_STATE (text_effects_update, state)

    /// \details Used as hack to force-update showcase text on hot reload.
    bool just_deployed;
};

APPLICATION_FRAMEWORK_EXAMPLES_TEXT_EFFECTS_API KAN_UM_MUTATOR_DEPLOY (text_effects_update)
{
    kan_static_interned_ids_ensure_initialized ();
    state->just_deployed = true;
    kan_workflow_graph_node_make_dependency_of (workflow_node, KAN_TEXT_SHAPING_BEGIN_CHECKPOINT);
}

static void update_dirty_text (struct example_text_effects_singleton_t *singleton, struct kan_text_shaping_unit_t *text)
{
    const kan_instance_size_t seconds =
        (kan_instance_size_t) (kan_precise_time_get_elapsed_nanoseconds () / 1000000000u);

    if (seconds == singleton->dirty_text_last_seconds && KAN_HANDLE_IS_VALID (text->request.text))
    {
        return;
    }

    singleton->dirty_text_last_seconds = seconds;
    if (KAN_HANDLE_IS_VALID (text->request.text))
    {
        kan_text_destroy (text->request.text);
    }

    char buffer[512u];
    snprintf (buffer, sizeof (buffer), "Text that is reshaped once per second: %u.", (unsigned int) seconds);

    struct kan_text_item_t text_items[] = {
        KAN_INIT_TEXT_ITEM_STYLE (NULL, TEXT_MAKE_MARK (0u, TEXT_MARK_FLAG_OUTLINE)),
        KAN_INIT_TEXT_ITEM_UTF8 (buffer),
    };

    struct kan_text_description_t description = {
        .items_count = sizeof (text_items) / sizeof (text_items[0u]),
        .items = text_items,
        .guide_bidi_with_direction = false,
        .direction_to_guide_bidi = KAN_TEXT_READING_DIRECTION_LEFT_TO_RIGHT,
    };

    text->request.text = kan_text_create (&description);
    text->dirty = true;
}

static void update_unstable_text (struct kan_text_shaping_unit_t *text)
{
    if (KAN_HANDLE_IS_VALID (text->request.text))
    {
        kan_text_destroy (text->request.text);
    }

    char buffer[512u];
    snprintf (buffer, sizeof (buffer), "Millisecond part of seconds since startup: %u.",
              (unsigned int) ((kan_precise_time_get_elapsed_nanoseconds () / 1000000u) % 1000u));

    struct kan_text_item_t text_items[] = {
        KAN_INIT_TEXT_ITEM_STYLE (NULL, TEXT_MAKE_MARK (0u, TEXT_MARK_FLAG_OUTLINE)),
        KAN_INIT_TEXT_ITEM_UTF8 (buffer),
    };

    struct kan_text_description_t description = {
        .items_count = sizeof (text_items) / sizeof (text_items[0u]),
        .items = text_items,
        .guide_bidi_with_direction = false,
        .direction_to_guide_bidi = KAN_TEXT_READING_DIRECTION_LEFT_TO_RIGHT,
    };

    text->request.text = kan_text_create (&description);
}

static void update_showcase_text (struct kan_text_shaping_unit_t *text)
{
    text->request.font_size = 32u;
    text->request.render_format = KAN_FONT_GLYPH_RENDER_FORMAT_SDF;
    text->request.orientation = KAN_TEXT_ORIENTATION_HORIZONTAL;
    text->request.reading_direction = KAN_TEXT_READING_DIRECTION_LEFT_TO_RIGHT;
    text->request.alignment = KAN_TEXT_SHAPING_ALIGNMENT_CENTER;
    text->request.primary_axis_limit = FIXED_WIDTH - 100u;
    text->request.allow_breaks = true;
    text->request.generate_edition_markup = false;

    struct kan_text_item_t text_items[] = {
        KAN_INIT_TEXT_ITEM_STYLE (NULL, TEXT_MAKE_MARK (0u, TEXT_MARK_FLAG_OUTLINE)),
        KAN_INIT_TEXT_ITEM_UTF8 ("This text is used to showcase various shader-based effects inside one text unit.\n"),
        KAN_INIT_TEXT_ITEM_STYLE (NULL, TEXT_MAKE_MARK (0u, 0u)),
        KAN_INIT_TEXT_ITEM_UTF8 ("It can be just white without outline. "),
        KAN_INIT_TEXT_ITEM_STYLE (NULL, TEXT_MAKE_MARK (1u, 0u)),
        KAN_INIT_TEXT_ITEM_UTF8 ("It can be red. "),
        KAN_INIT_TEXT_ITEM_STYLE (NULL, TEXT_MAKE_MARK (1u, TEXT_MARK_FLAG_OUTLINE)),
        KAN_INIT_TEXT_ITEM_UTF8 ("With outline! "),
        KAN_INIT_TEXT_ITEM_STYLE (NULL, TEXT_MAKE_MARK (2u, TEXT_MARK_FLAG_OUTLINE)),
        KAN_INIT_TEXT_ITEM_UTF8 ("And green! "),
        KAN_INIT_TEXT_ITEM_STYLE (NULL, TEXT_MAKE_MARK (3u, TEXT_MARK_FLAG_OUTLINE)),
        KAN_INIT_TEXT_ITEM_UTF8 ("And blue!\n"),
        KAN_INIT_TEXT_ITEM_STYLE (kan_string_intern ("bold"), TEXT_MAKE_MARK (0u, 0u)),
        KAN_INIT_TEXT_ITEM_UTF8 ("Let's make this text "),
        KAN_INIT_TEXT_ITEM_STYLE (kan_string_intern ("bold"), TEXT_MAKE_MARK (0u, TEXT_MARK_FLAG_OUTLINE)),
        KAN_INIT_TEXT_ITEM_UTF8 ("BOLD"),
        KAN_INIT_TEXT_ITEM_STYLE (kan_string_intern ("bold"), TEXT_MAKE_MARK (0u, 0u)),
        KAN_INIT_TEXT_ITEM_UTF8 (" again. "),
        KAN_INIT_TEXT_ITEM_STYLE (NULL, TEXT_MAKE_MARK (0u, TEXT_MARK_FLAG_OUTLINE)),
        KAN_INIT_TEXT_ITEM_UTF8 ("And back to regular now.\n"),
        KAN_INIT_TEXT_ITEM_STYLE (NULL, TEXT_MAKE_MARK (0u, TEXT_MARK_FLAG_OUTLINE | TEXT_MARK_FLAG_SIN_ANIMATION)),
        KAN_INIT_TEXT_ITEM_UTF8 ("We can also do some color animations. "),
        KAN_INIT_TEXT_ITEM_STYLE (NULL, TEXT_MAKE_MARK (1u, TEXT_MARK_FLAG_SIN_ANIMATION)),
        KAN_INIT_TEXT_ITEM_UTF8 ("Also. "),
        KAN_INIT_TEXT_ITEM_STYLE (NULL, TEXT_MAKE_MARK (2u, TEXT_MARK_FLAG_SIN_ANIMATION)),
        KAN_INIT_TEXT_ITEM_UTF8 ("Also. "),
        KAN_INIT_TEXT_ITEM_STYLE (NULL, TEXT_MAKE_MARK (3u, TEXT_MARK_FLAG_SIN_ANIMATION)),
        KAN_INIT_TEXT_ITEM_UTF8 ("Also.\n"),
    };

    struct kan_text_description_t description = {
        .items_count = sizeof (text_items) / sizeof (text_items[0u]),
        .items = text_items,
        .guide_bidi_with_direction = false,
        .direction_to_guide_bidi = KAN_TEXT_READING_DIRECTION_LEFT_TO_RIGHT,
    };

    text->request.text = kan_text_create (&description);
    text->dirty = true;
}

static void update_reading_text (struct kan_text_shaping_unit_t *text)
{
    text->request.font_size = 24u;
    text->request.render_format = KAN_FONT_GLYPH_RENDER_FORMAT_SDF;
    text->request.orientation = KAN_TEXT_ORIENTATION_HORIZONTAL;
    text->request.reading_direction = KAN_TEXT_READING_DIRECTION_LEFT_TO_RIGHT;
    text->request.alignment = KAN_TEXT_SHAPING_ALIGNMENT_LEFT;
    text->request.primary_axis_limit = FIXED_WIDTH - 100u;
    text->request.allow_breaks = true;
    text->request.generate_edition_markup = false;

    struct kan_text_item_t text_items[] = {
        KAN_INIT_TEXT_ITEM_STYLE (NULL, TEXT_MAKE_MARK (0u, TEXT_MARK_FLAG_OUTLINE | TEXT_MARK_FLAG_READING_ANIMATION)),
        KAN_INIT_TEXT_ITEM_UTF8 (
            "This text is copied from wikipedia and used to showcase how \"reading appearance\" can be done.\n\n"
            "Robert Guiscard also referred to as Robert de Hauteville, was a Norman adventurer remembered for "
            "his conquest of southern Italy and Sicily in the 11th century.\n"
            "\n"
            "Robert was born into the Hauteville family in Normandy, the sixth son of Tancred de Hauteville "
            "and his wife Fressenda. He inherited the County of Apulia and Calabria from his brother in 1057, "
            "and in 1059 he was made Duke of Apulia and Calabria and Lord of Sicily by Pope Nicholas II. He "
            "was also briefly Prince of Benevento (1078–1081), before returning the title to the papacy.\n"),
    };

    struct kan_text_description_t description = {
        .items_count = sizeof (text_items) / sizeof (text_items[0u]),
        .items = text_items,
        .guide_bidi_with_direction = false,
        .direction_to_guide_bidi = KAN_TEXT_READING_DIRECTION_LEFT_TO_RIGHT,
    };

    text->request.text = kan_text_create (&description);
    text->dirty = true;
}

APPLICATION_FRAMEWORK_EXAMPLES_TEXT_EFFECTS_API KAN_UM_MUTATOR_EXECUTE (text_effects_update)
{
    KAN_UMI_SINGLETON_READ (text_shaping, kan_text_shaping_singleton_t)
    KAN_UMI_SINGLETON_WRITE (singleton, example_text_effects_singleton_t)

    if (!KAN_TYPED_ID_32_IS_VALID (singleton->stable_text_id))
    {
        KAN_NEW_TEXT_SHAPING_UNIT_FROM_LITERAL (stable, "Text that is shaped once and never made dirty.",
                                                TEXT_MAKE_MARK (0u, TEXT_MARK_FLAG_OUTLINE));

        stable_shaping_unit->request.font_size = 32u;
        stable_shaping_unit->request.primary_axis_limit = 425u;
        singleton->stable_text_id = stable_shaping_unit->id;

        // Initialize locale here as well, because this will only be executed once.
        KAN_UMI_SINGLETON_WRITE (locale, kan_locale_singleton_t)
        locale->selected_locale = kan_string_intern ("en");
    }

    if (!KAN_TYPED_ID_32_IS_VALID (singleton->dirty_text_id))
    {
        KAN_NEW_TEXT_SHAPING_UNIT (dirty);
        dirty_shaping_unit->request.font_size = 32u;
        dirty_shaping_unit->request.alignment = KAN_TEXT_SHAPING_ALIGNMENT_CENTER;
        dirty_shaping_unit->request.primary_axis_limit = 425u;
        dirty_shaping_unit->dirty = true;

        singleton->dirty_text_id = dirty_shaping_unit->id;
        update_dirty_text (singleton, dirty_shaping_unit);
    }
    else
    {
        KAN_UMI_VALUE_UPDATE_REQUIRED (unit, kan_text_shaping_unit_t, id, &singleton->dirty_text_id)
        update_dirty_text (singleton, unit);
    }

    if (!KAN_TYPED_ID_32_IS_VALID (singleton->unstable_text_id))
    {
        KAN_NEW_TEXT_SHAPING_UNIT (unstable);
        unstable_shaping_unit->request.font_size = 32u;
        unstable_shaping_unit->request.alignment = KAN_TEXT_SHAPING_ALIGNMENT_RIGHT;
        unstable_shaping_unit->request.primary_axis_limit = 425u;
        unstable_shaping_unit->stable = false;

        singleton->unstable_text_id = unstable_shaping_unit->id;
        update_unstable_text (unstable_shaping_unit);
    }
    else
    {
        KAN_UMI_VALUE_UPDATE_REQUIRED (unit, kan_text_shaping_unit_t, id, &singleton->unstable_text_id)
        update_unstable_text (unit);
    }

    if (!KAN_TYPED_ID_32_IS_VALID (singleton->showcase_text_id))
    {
        KAN_NEW_TEXT_SHAPING_UNIT (showcase);
        singleton->showcase_text_id = showcase_shaping_unit->id;
        update_showcase_text (showcase_shaping_unit);
    }
    else if (state->just_deployed)
    {
        KAN_UMI_VALUE_UPDATE_REQUIRED (unit, kan_text_shaping_unit_t, id, &singleton->showcase_text_id)
        update_showcase_text (unit);
    }

    if (!KAN_TYPED_ID_32_IS_VALID (singleton->reading_text_id))
    {
        KAN_NEW_TEXT_SHAPING_UNIT (reading);
        singleton->reading_text_id = reading_shaping_unit->id;
        update_reading_text (reading_shaping_unit);
    }
    else if (state->just_deployed)
    {
        KAN_UMI_VALUE_UPDATE_REQUIRED (unit, kan_text_shaping_unit_t, id, &singleton->reading_text_id)
        update_reading_text (unit);
    }

    state->just_deployed = false;
}

struct text_effects_render_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (text_effects_render)
    KAN_UM_BIND_STATE (text_effects_render, state)

    kan_context_system_t application_system_handle;
    kan_context_system_t render_backend_system_handle;

    kan_interned_string_t text_pass_name;
};

APPLICATION_FRAMEWORK_EXAMPLES_TEXT_EFFECTS_API KAN_UM_MUTATOR_DEPLOY (text_effects_render)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_context_t context = kan_universe_get_context (universe);

    state->application_system_handle = kan_context_query (context, KAN_CONTEXT_APPLICATION_SYSTEM_NAME);
    state->render_backend_system_handle = kan_context_query (context, KAN_CONTEXT_RENDER_BACKEND_SYSTEM_NAME);

    // We do not use static strings here as we pass this as variables in different places.
    state->text_pass_name = kan_string_intern ("text");
    kan_workflow_graph_node_depend_on (workflow_node, KAN_TEXT_SHAPING_END_CHECKPOINT);
}

static void try_render_frame (struct text_effects_render_state_t *state,
                              const struct kan_render_context_singleton_t *render_context,
                              const struct kan_render_graph_resource_management_singleton_t *render_resource_management,
                              struct test_routine_singleton_t *test,
                              struct example_text_effects_singleton_t *singleton,
                              const struct text_effects_config_t *config)
{
#define MAX_EXPECTED_SCENE_IMAGE_REQUESTS 16u
    static_assert (MAX_EXPECTED_SCENE_IMAGE_REQUESTS >= TEXT_EFFECTS_SCENE_IMAGE_COUNT,
                   "Static request allocation for frame buffers is big enough.");
    struct kan_render_graph_resource_image_request_t image_requests[MAX_EXPECTED_SCENE_IMAGE_REQUESTS];

#define MAX_EXPECTED_SCENE_FRAME_BUFFER_REQUESTS 16u
    static_assert (MAX_EXPECTED_SCENE_FRAME_BUFFER_REQUESTS >= TEXT_EFFECTS_SCENE_FRAME_BUFFER_COUNT,
                   "Static request allocation for frame buffers is big enough.");
    struct kan_render_graph_resource_frame_buffer_request_t
        frame_buffer_requests[MAX_EXPECTED_SCENE_FRAME_BUFFER_REQUESTS];

    KAN_UMI_VALUE_READ_OPTIONAL (text_pass, kan_render_foundation_pass_loaded_t, name, &state->text_pass_name)
    if (!text_pass || !KAN_HANDLE_IS_VALID (text_pass->pass))
    {
        return;
    }

    // Setup pass.

    if (text_pass->attachments.size != 1u)
    {
        KAN_LOG (application_framework_example_text_effects, KAN_LOG_ERROR,
                 "Text pass has unexpected count of attachments.")
        return;
    }

    struct kan_render_foundation_pass_attachment_t *text_color_attachment =
        &((struct kan_render_foundation_pass_attachment_t *) text_pass->attachments.data)[0u];

    if (text_pass->variants.size != 1u)
    {
        KAN_LOG (application_framework_example_text_effects, KAN_LOG_ERROR,
                 "Text pass has unexpected count of variants.")
        return;
    }

    struct kan_render_foundation_pass_variant_t *text_pass_variant =
        &((struct kan_render_foundation_pass_variant_t *) text_pass->variants.data)[0u];

    if (text_pass_variant->pass_parameter_set_bindings.buffers.size != 0u)
    {
        KAN_LOG (application_framework_example_text_effects, KAN_LOG_ERROR,
                 "Text pass has unexpected parameter set bindings.")
        return;
    }

    image_requests[TEXT_EFFECTS_SCENE_IMAGE_COLOR] = (struct kan_render_graph_resource_image_request_t) {
        .description =
            {
                .format = text_color_attachment->format,
                .width = FIXED_WIDTH,
                .height = FIXED_HEIGHT,
                .depth = 1u,
                .layers = 1u,
                .mips = 1u,
                .render_target = true,
                .supports_sampling = false,
                .always_treat_as_layered = false,
                .tracking_name = NULL,
            },
        .internal = true,
    };

    frame_buffer_requests[TEXT_EFFECTS_SCENE_FRAME_BUFFER_COLOR] =
        (struct kan_render_graph_resource_frame_buffer_request_t) {
            .pass = text_pass->pass,
            .attachments_count = 1u,
            .attachments =
                (struct kan_render_graph_resource_frame_buffer_request_attachment_t[]) {
                    {
                        .image_index = TEXT_EFFECTS_SCENE_IMAGE_COLOR,
                        .image_layer = 0u,
                    },
                },
        };

    struct kan_render_graph_resource_request_t scene_request = {
        .context = render_context->render_context,
        .dependant_count = 0u,
        .dependant = NULL,
        .images_count = TEXT_EFFECTS_SCENE_IMAGE_COUNT,
        .images = image_requests,
        .frame_buffers_count = TEXT_EFFECTS_SCENE_FRAME_BUFFER_COUNT,
        .frame_buffers = frame_buffer_requests,
    };

    const struct kan_render_graph_resource_response_t *response =
        kan_render_graph_resource_management_singleton_request (render_resource_management, &scene_request);

    if (!response)
    {
        KAN_LOG (application_framework_example_text_effects, KAN_LOG_ERROR, "Failed to allocate scene view resources.")
        return;
    }

    KAN_UMI_VALUE_READ_OPTIONAL (text_material_instance, kan_render_material_instance_loaded_t, name,
                                 &config->text_material_instance_name)

    if (!text_material_instance)
    {
        return;
    }

    KAN_UMI_VALUE_READ_OPTIONAL (text_material, kan_render_material_loaded_t, name,
                                 &text_material_instance->material_name)

    if (!text_material)
    {
        return;
    }

    bool everything_rendered = true;

    // Update text shared set if possible.
    if (text_material)
    {
        if (text_material->set_shared_bindings.samplers.size != 1u ||
            text_material->set_shared_bindings.images.size != 1u)
        {
            KAN_LOG (application_framework_example_text_effects, KAN_LOG_ERROR,
                     "Text material has unexpected shared set bindings.")
            return;
        }

        if (text_material->pipelines.size > 1u)
        {
            KAN_LOG (application_framework_example_text_effects, KAN_LOG_ERROR,
                     "Text material has several pipelines, but only one is expected.")
            return;
        }

        if (!KAN_HANDLE_IS_VALID (singleton->text_shared_parameter_set))
        {
            struct kan_rpl_meta_sampler_t *sampler_binding =
                &((struct kan_rpl_meta_sampler_t *) text_material->set_shared_bindings.samplers.data)[0u];

            struct kan_render_parameter_update_description_t bindings[] = {{
                .binding = sampler_binding->binding,
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
            }};

            struct kan_render_pipeline_parameter_set_description_t description = {
                .layout = text_material->set_shared,
                .stable_binding = true,
                .initial_bindings_count = sizeof (bindings) / sizeof (bindings[0u]),
                .initial_bindings = bindings,
                .tracking_name = KAN_STATIC_INTERNED_ID_GET (text_set),
            };

            singleton->text_shared_parameter_set =
                kan_render_pipeline_parameter_set_create (render_context->render_context, &description);
            singleton->text_last_bound_atlas = KAN_HANDLE_SET_INVALID (kan_render_image_t);
        }
    }

    // Scene view passes.

    const kan_time_size_t current_time = kan_precise_time_get_elapsed_nanoseconds ();
    // Circle time every 100s.
    const float time_for_shader =
        test->test_mode_enabled && !singleton->frame_checked ? 0.75f : 1e-9f * (float) (current_time % 100000000000lu);
    const float read_start_time = test->test_mode_enabled && !singleton->frame_checked ? -15.0f : 0.0f;

    // We use reversed depth everywhere in this example.
    struct text_push_constant_t push = {
        .projection_view =
            kan_orthographic_projection (0.0f, (float) FIXED_WIDTH, (float) FIXED_HEIGHT, 0.0f, 0.01f, 1000.0f),
        .offset_and_time = {0.0f, 0.0f, time_for_shader, read_start_time},
    };

    struct kan_render_viewport_bounds_t viewport_bounds = {
        .x = 0.0f,
        .y = 0.0f,
        .width = (float) FIXED_WIDTH,
        .height = (float) FIXED_HEIGHT,
        .depth_min = 0.0f,
        .depth_max = 1.0f,
    };

    struct kan_render_integer_region_2d_t scissor = {
        .x = 0,
        .y = 0,
        .width = FIXED_WIDTH,
        .height = FIXED_HEIGHT,
    };

    struct kan_render_clear_value_t clear_values[] = {
        {
            .color =
                {
                    kan_color_transfer_rgb_to_srgb_approximate (20.0f / 255.0f),
                    kan_color_transfer_rgb_to_srgb_approximate (140.0f / 255.0f),
                    kan_color_transfer_rgb_to_srgb_approximate (190.0f / 255.0f),
                    1.0f,
                },
        },
    };

    kan_render_pass_instance_t pass_instance =
        kan_render_pass_instantiate (text_pass->pass, response->frame_buffers[TEXT_EFFECTS_SCENE_FRAME_BUFFER_COLOR],
                                     &viewport_bounds, &scissor, clear_values);

    kan_render_pass_instance_add_checkpoint_dependency (pass_instance, response->usage_begin_checkpoint);
    kan_render_pass_instance_checkpoint_add_instance_dependency (response->usage_end_checkpoint, pass_instance);

    struct kan_render_material_pipeline_t *pipeline_data =
        &((struct kan_render_material_pipeline_t *) text_material->pipelines.data)[0u];

    struct
    {
        kan_text_shaping_unit_id_t unit_id;
        float anchor_x;
        float anchor_y;
    } requests[] = {
        {
            .unit_id = singleton->stable_text_id,
            .anchor_x = 25.0f,
            .anchor_y = 25.0f,
        },
        {
            .unit_id = singleton->dirty_text_id,
            .anchor_x = FIXED_WIDTH * 0.5f,
            .anchor_y = 25.0f,
        },
        {
            .unit_id = singleton->unstable_text_id,
            .anchor_x = FIXED_WIDTH - 25.0f,
            .anchor_y = 25.0f,
        },
        {
            .unit_id = singleton->showcase_text_id,
            .anchor_x = FIXED_WIDTH * 0.5f,
            .anchor_y = 150.0f,
        },
        {
            .unit_id = singleton->reading_text_id,
            .anchor_x = 25.0f,
            .anchor_y = 400.0f,
        },
    };

    if ((everything_rendered &= kan_render_pass_instance_graphics_pipeline (pass_instance, pipeline_data->pipeline)))
    {
        KAN_UMI_SINGLETON_READ (shaping_singleton, kan_text_shaping_singleton_t)
        if (KAN_HANDLE_IS_VALID (shaping_singleton->font_library_sdf_atlas))
        {
            kan_instance_size_t atlas_layers;
            kan_render_image_get_sizes (shaping_singleton->font_library_sdf_atlas, NULL, NULL, NULL, &atlas_layers);

            if (!KAN_HANDLE_IS_EQUAL (singleton->text_last_bound_atlas, shaping_singleton->font_library_sdf_atlas))
            {
                struct kan_rpl_meta_sampler_t *atlas_binding =
                    &((struct kan_rpl_meta_sampler_t *) text_material->set_shared_bindings.images.data)[0u];

                struct kan_render_parameter_update_description_t bindings[] = {
                    {
                        .binding = atlas_binding->binding,
                        .image_binding =
                            {
                                .image = shaping_singleton->font_library_sdf_atlas,
                                .array_index = 0u,
                                .layer_offset = 0u,
                                .layer_count = atlas_layers,
                            },
                    },
                };

                kan_render_pipeline_parameter_set_update (singleton->text_shared_parameter_set,
                                                          sizeof (bindings) / sizeof (bindings[0u]), bindings);
                singleton->text_last_bound_atlas = shaping_singleton->font_library_sdf_atlas;
            }
        }

        kan_render_pass_instance_pipeline_parameter_sets (pass_instance, KAN_RPL_SET_SHARED, 1u,
                                                          &singleton->text_shared_parameter_set);

        kan_render_pass_instance_pipeline_parameter_sets (pass_instance, KAN_RPL_SET_MATERIAL, 1u,
                                                          &text_material_instance->parameter_set);

        kan_render_pass_instance_indices (pass_instance, singleton->glyph_index_buffer);
        kan_render_pass_instance_attributes (pass_instance, 0u, 1u, &singleton->glyph_vertex_buffer, NULL);

        for (kan_loop_size_t index = 0u; index < sizeof (requests) / sizeof (requests[0u]); ++index)
        {
            KAN_UMI_VALUE_READ_OPTIONAL (unit, kan_text_shaping_unit_t, id, &requests[index].unit_id)
            if (!unit || !unit->shaped)
            {
                everything_rendered = false;
                break;
            }

            push.offset_and_time.x = requests[index].anchor_x;
            push.offset_and_time.y = requests[index].anchor_y;

            const kan_instance_size_t glyphs_count =
                unit->shaped_as_stable ? unit->shaped_stable.glyphs_count : unit->shaped_unstable.glyphs.size;

            switch (unit->request.alignment)
            {
            case KAN_TEXT_SHAPING_ALIGNMENT_LEFT:
                break;

            case KAN_TEXT_SHAPING_ALIGNMENT_CENTER:
                push.offset_and_time.x -= ((float) unit->shaped_primary_size) * 0.5f;
                break;

            case KAN_TEXT_SHAPING_ALIGNMENT_RIGHT:
                push.offset_and_time.x -= (float) unit->shaped_primary_size;
                break;
            }

            kan_render_pass_instance_push_constant (pass_instance, &push);
            if (unit->shaped_as_stable)
            {
                // Otherwise might technically trigger "discards const" error.
                kan_render_buffer_t attributes[] = {unit->shaped_stable.glyphs};
                kan_render_pass_instance_attributes (pass_instance, 1u, 1u, attributes, NULL);
            }
            else
            {
                struct kan_render_allocated_slice_t slice = kan_render_frame_lifetime_buffer_allocator_allocate (
                    singleton->instanced_data_allocator,
                    sizeof (struct kan_text_shaped_glyph_instance_data_t) * glyphs_count,
                    alignof (struct kan_text_shaped_glyph_instance_data_t));

                void *text_instanced_memory =
                    kan_render_buffer_patch (slice.buffer, slice.slice_offset,
                                             sizeof (struct kan_text_shaped_glyph_instance_data_t) * glyphs_count);
                KAN_ASSERT (text_instanced_memory)

                memcpy (text_instanced_memory, unit->shaped_unstable.glyphs.data,
                        sizeof (struct kan_text_shaped_glyph_instance_data_t) * glyphs_count);
                kan_render_pass_instance_attributes (pass_instance, 1u, 1u, &slice.buffer, &slice.slice_offset);
            }

            kan_render_pass_instance_draw (pass_instance, 0u, singleton->glyph_index_count, 0u, 0u, glyphs_count);
        }
    }

    if (everything_rendered)
    {
        struct kan_render_integer_region_2d_t image_region = scissor;
        struct kan_render_integer_region_2d_t surface_region = scissor;

        kan_render_backend_system_present_image_on_surface (singleton->window_surface,
                                                            response->images[TEXT_EFFECTS_SCENE_IMAGE_COLOR], 0u,
                                                            surface_region, image_region, pass_instance);

        if (test->test_mode_enabled && !singleton->frame_checked)
        {
            kan_dynamic_array_set_capacity (&test->expectation_read_back_statuses, 1u);
            *(kan_render_read_back_status_t *) kan_dynamic_array_add_last (&test->expectation_read_back_statuses) =
                kan_render_request_read_back_from_image (response->images[TEXT_EFFECTS_SCENE_IMAGE_COLOR], 0u, 0u,
                                                         test->expectation_read_back_buffer, 0u, pass_instance);
        }

        if (!singleton->frame_checked)
        {
            KAN_LOG (application_framework_example_text_effects, KAN_LOG_INFO, "Queued frame render successfully.")
        }

        singleton->frame_checked = true;
    }
}

APPLICATION_FRAMEWORK_EXAMPLES_TEXT_EFFECTS_API KAN_UM_MUTATOR_EXECUTE (text_effects_render)
{
    KAN_UMI_SINGLETON_READ (render_context, kan_render_context_singleton_t)
    KAN_UMI_SINGLETON_READ (render_graph, kan_render_graph_resource_management_singleton_t)
    KAN_UMI_SINGLETON_READ (program_singleton, kan_render_program_singleton_t)
    KAN_UMI_SINGLETON_READ (resource_provider, kan_resource_provider_singleton_t)
    KAN_UMI_SINGLETON_WRITE (test, test_routine_singleton_t)
    KAN_UMI_SINGLETON_WRITE (singleton, example_text_effects_singleton_t)

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

        singleton->window_handle = kan_application_system_window_create (state->application_system_handle,
                                                                         "application_framework_example_text_effects",
                                                                         FIXED_WIDTH, FIXED_HEIGHT, flags);

        enum kan_render_surface_present_mode_t present_modes[] = {
            KAN_RENDER_SURFACE_PRESENT_MODE_MAILBOX,
            KAN_RENDER_SURFACE_PRESENT_MODE_IMMEDIATE,
            KAN_RENDER_SURFACE_PRESENT_MODE_INVALID,
        };

        singleton->window_surface =
            kan_render_backend_system_create_surface (state->render_backend_system_handle, singleton->window_handle,
                                                      present_modes, KAN_STATIC_INTERNED_ID_GET (window_surface));

        KAN_ASSERT (KAN_HANDLE_IS_VALID (singleton->window_surface))
        kan_application_system_window_raise (state->application_system_handle, singleton->window_handle);
    }

    if (test->test_mode_enabled && !KAN_HANDLE_IS_VALID (test->expectation_read_back_buffer))
    {
        test->expectation_read_back_buffer = kan_render_buffer_create (
            render_context->render_context, KAN_RENDER_BUFFER_TYPE_READ_BACK_STORAGE, FIXED_WIDTH * FIXED_HEIGHT * 4u,
            NULL, KAN_STATIC_INTERNED_ID_GET (test_read_back_buffer));
    }

    const kan_interned_string_t root_config_name = KAN_STATIC_INTERNED_ID_GET (root_config);
    if (!KAN_TYPED_ID_32_IS_VALID (singleton->config_usage_id))
    {
        KAN_UMO_INDEXED_INSERT (request, kan_resource_usage_t)
        {
            request->usage_id = kan_next_resource_usage_id (resource_provider);
            singleton->config_usage_id = request->usage_id;
            request->type = KAN_STATIC_INTERNED_ID_GET (text_effects_config_t);
            request->name = root_config_name;
        }
    }

    if (!singleton->object_buffers_initialized && KAN_HANDLE_IS_VALID (render_context->render_context))
    {
        example_text_effects_singleton_initialize_object_buffers (singleton, render_context->render_context);
    }

    KAN_UMI_RESOURCE_RETRIEVE_IF_LOADED_AND_FRESH (root_config, text_effects_config_t, &root_config_name)
    if (root_config)
    {
        if (!KAN_TYPED_ID_32_IS_VALID (singleton->text_material_instance_usage_id))
        {
            KAN_UMO_INDEXED_INSERT (usage, kan_render_material_instance_usage_t)
            {
                usage->usage_id = kan_next_material_instance_usage_id (program_singleton);
                singleton->text_material_instance_usage_id = usage->usage_id;
                usage->name = root_config->text_material_instance_name;
            }
        }

        KAN_UML_EVENT_FETCH (material_instance_updated, kan_render_material_instance_updated_event_t)
        {
            // Destroy parameter sets on hot reload in order to create new ones during next render.
            if (material_instance_updated->name == root_config->text_material_instance_name)
            {
                if (KAN_HANDLE_IS_VALID (singleton->text_shared_parameter_set))
                {
                    kan_render_pipeline_parameter_set_destroy (singleton->text_shared_parameter_set);
                    singleton->text_shared_parameter_set = KAN_HANDLE_SET_INVALID (kan_render_pipeline_parameter_set_t);
                }
            }
        }

        if (KAN_HANDLE_IS_VALID (render_context->render_context) && render_context->frame_scheduled)
        {
            try_render_frame (state, render_context, render_graph, test, singleton, root_config);
        }
    }
}
