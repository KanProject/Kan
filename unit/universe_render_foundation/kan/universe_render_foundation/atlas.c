#define _CRT_SECURE_NO_WARNINGS __CUSHION_PRESERVE__

#include <string.h>

#include <qsort.h>

#include <kan/cpu_profiler/markup.h>
#include <kan/log/logging.h>
#include <kan/resource_render_foundation/atlas.h>
#include <kan/universe/macro.h>
#include <kan/universe_render_foundation/atlas.h>
#include <kan/universe_render_foundation/render_graph.h>
#include <kan/universe_resource_provider/provider.h>

KAN_LOG_DEFINE_CATEGORY (render_foundation_atlas);
KAN_USE_STATIC_INTERNED_IDS
KAN_USE_STATIC_CPU_SECTIONS

KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (render_foundation_atlas_management)
UNIVERSE_RENDER_FOUNDATION_API KAN_UM_MUTATOR_GROUP_META (render_foundation_atlas_management,
                                                          KAN_RENDER_FOUNDATION_ATLAS_MANAGEMENT_MUTATOR_GROUP);

struct render_foundation_atlas_usage_on_insert_event_t
{
    kan_interned_string_t atlas_name;
};

KAN_REFLECTION_STRUCT_META (kan_render_atlas_usage_t)
UNIVERSE_RENDER_FOUNDATION_API struct kan_repository_meta_automatic_on_insert_event_t
    render_foundation_atlas_usage_on_insert_event = {
        .event_type = "render_foundation_atlas_usage_on_insert_event_t",
        .copy_outs_count = 1u,
        .copy_outs =
            (struct kan_repository_copy_out_t[]) {
                {
                    .source_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"name"}},
                    .target_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"atlas_name"}},
                },
            },
};

struct render_foundation_atlas_usage_on_delete_event_t
{
    kan_interned_string_t atlas_name;
};

KAN_REFLECTION_STRUCT_META (kan_render_atlas_usage_t)
UNIVERSE_RENDER_FOUNDATION_API struct kan_repository_meta_automatic_on_delete_event_t
    render_foundation_atlas_usage_on_delete_event = {
        .event_type = "render_foundation_atlas_usage_on_delete_event_t",
        .copy_outs_count = 1u,
        .copy_outs =
            (struct kan_repository_copy_out_t[]) {
                {
                    .source_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"name"}},
                    .target_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"atlas_name"}},
                },
            },
};

enum render_foundation_atlas_state_t
{
    RENDER_FOUNDATION_ATLAS_STATE_INITIAL = 0u,
    RENDER_FOUNDATION_ATLAS_STATE_WAITING,
    RENDER_FOUNDATION_ATLAS_STATE_READY,
};

struct render_foundation_atlas_t
{
    kan_interned_string_t name;
    kan_instance_size_t reference_count;
    kan_resource_usage_id_t usage_id;

    enum render_foundation_atlas_state_t state;
    kan_instance_size_t state_frame_id;
};

KAN_REFLECTION_STRUCT_META (render_foundation_atlas_t)
UNIVERSE_RENDER_FOUNDATION_API struct kan_repository_meta_automatic_cascade_deletion_t
    render_foundation_atlas_usage_id_cascade_deletion = {
        .parent_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"usage_id"}},
        .child_type_name = "kan_resource_usage_t",
        .child_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"usage_id"}},
};

KAN_REFLECTION_STRUCT_META (render_foundation_atlas_t)
UNIVERSE_RENDER_FOUNDATION_API struct kan_repository_meta_automatic_cascade_deletion_t
    render_foundation_atlas_loaded_cascade_deletion = {
        .parent_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"name"}},
        .child_type_name = "kan_render_atlas_loaded_t",
        .child_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"name"}},
};

struct render_foundation_atlas_management_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (render_foundation_atlas_management)
    KAN_UM_BIND_STATE (render_foundation_atlas_management, state)
};

UNIVERSE_RENDER_FOUNDATION_API KAN_UM_MUTATOR_DEPLOY (render_foundation_atlas_management)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_cpu_static_sections_ensure_initialized ();

    kan_workflow_graph_node_depend_on (workflow_node, KAN_RESOURCE_PROVIDER_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_RENDER_FOUNDATION_FRAME_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_RENDER_FOUNDATION_ATLAS_MANAGEMENT_BEGIN_CHECKPOINT);
    kan_workflow_graph_node_make_dependency_of (workflow_node, KAN_RENDER_FOUNDATION_ATLAS_MANAGEMENT_END_CHECKPOINT);
}

static void advance_from_initial_state (struct render_foundation_atlas_management_state_t *state,
                                        struct kan_render_atlas_singleton_t *public,
                                        const struct kan_resource_provider_singleton_t *provider,
                                        struct render_foundation_atlas_t *atlas);

static void advance_from_waiting_state (struct render_foundation_atlas_management_state_t *state,
                                        struct kan_render_atlas_singleton_t *public,
                                        const struct kan_resource_provider_singleton_t *provider,
                                        struct render_foundation_atlas_t *atlas);

static void on_atlas_resource_updated (struct render_foundation_atlas_management_state_t *state,
                                       struct kan_render_atlas_singleton_t *public,
                                       const struct kan_resource_provider_singleton_t *provider,
                                       kan_interned_string_t atlas_name)
{
    KAN_UMI_VALUE_UPDATE_OPTIONAL (atlas, render_foundation_atlas_t, name, &atlas_name)
    if (!atlas)
    {
        return;
    }

    atlas->state = RENDER_FOUNDATION_ATLAS_STATE_INITIAL;
    atlas->state_frame_id = provider->logic_deduplication_frame_id;

    ++public->loading_counter;
    // Start advancing, having some data loaded is very likely here.
    advance_from_initial_state (state, public, provider, atlas);
}

static void on_usage_insert (struct render_foundation_atlas_management_state_t *state,
                             struct kan_render_atlas_singleton_t *public,
                             const struct kan_resource_provider_singleton_t *provider,
                             kan_interned_string_t atlas_name)
{
    KAN_UMI_VALUE_UPDATE_OPTIONAL (existent, render_foundation_atlas_t, name, &atlas_name)
    if (existent)
    {
        ++existent->reference_count;
        return;
    }

    KAN_UMO_INDEXED_INSERT (atlas, render_foundation_atlas_t)
    {
        atlas->name = atlas_name;
        atlas->reference_count = 1u;
        atlas->usage_id = KAN_TYPED_ID_32_SET_INVALID (kan_resource_usage_id_t);

        atlas->state = RENDER_FOUNDATION_ATLAS_STATE_INITIAL;
        atlas->state_frame_id = provider->logic_deduplication_frame_id;

        ++public->loading_counter;
        advance_from_initial_state (state, public, provider, atlas);
    }
}

static void on_usage_delete (struct render_foundation_atlas_management_state_t *state,
                             struct kan_render_atlas_singleton_t *public,
                             kan_interned_string_t atlas_name)
{
    KAN_UMI_VALUE_WRITE_OPTIONAL (existent, render_foundation_atlas_t, name, &atlas_name)
    if (existent)
    {
        --existent->reference_count;
        if (existent->reference_count == 0u)
        {
            if (existent->state != RENDER_FOUNDATION_ATLAS_STATE_READY)
            {
                KAN_ASSERT (public->loading_counter > 0u)
                --public->loading_counter;
            }

            // Cascade deletion should handle everything.
            KAN_UM_ACCESS_DELETE (existent);
        }
    }
}

static void advance_from_initial_state (struct render_foundation_atlas_management_state_t *state,
                                        struct kan_render_atlas_singleton_t *public,
                                        const struct kan_resource_provider_singleton_t *provider,
                                        struct render_foundation_atlas_t *atlas)
{
    KAN_LOG (render_foundation_atlas, KAN_LOG_DEBUG,
             "Attempting to advance atlas \"%s\" state from initial to waiting.", atlas->name)

    atlas->state_frame_id = provider->logic_deduplication_frame_id;
    atlas->state = RENDER_FOUNDATION_ATLAS_STATE_WAITING; // We will always advance from initial state.

    // Usage will already be present if we're starting new hot reload while previous one wasn't finished.
    if (!KAN_TYPED_ID_32_IS_VALID (atlas->usage_id))
    {
        atlas->usage_id = kan_next_resource_usage_id (provider);
        KAN_UMO_INDEXED_INSERT (usage, kan_resource_usage_t)
        {
            usage->usage_id = atlas->usage_id;
            usage->type = KAN_STATIC_INTERNED_ID_GET (kan_resource_atlas_t);
            usage->name = atlas->name;
            usage->priority = KAN_UNIVERSE_RENDER_FOUNDATION_ATLAS_PRIORITY;
        }
    }

    advance_from_waiting_state (state, public, provider, atlas);
}

static inline bool is_image_compatible_with_atlas (kan_render_image_t image,
                                                   const struct kan_resource_atlas_t *resource)
{
    kan_instance_size_t width;
    kan_instance_size_t height;
    kan_instance_size_t depth;
    kan_instance_size_t layers;
    kan_render_image_get_sizes (image, &width, &height, &depth, &layers);

    return width == resource->page_width && height == resource->page_height && depth == 1u &&
           layers == resource->page_count;
}

KAN_REFLECTION_IGNORE
enum atlas_entry_gpu_flags_t
{
    ATLAS_ENTRY_GPU_FLAG_NINE_SLICE = 1u << 0u,
    ATLAS_ENTRY_GPU_FLAG_COLOR_MULTIPLIER = 1u << 1u,
};

KAN_REFLECTION_IGNORE
enum atlas_entry_nine_slice_gpu_flags_t
{
    ATLAS_ENTRY_NINE_SLICE_GPU_FLAG_TILED_X = 1u << 0u,
    ATLAS_ENTRY_NINE_SLICE_GPU_FLAG_TILED_Y = 1u << 1u,
};

/// \details Must be in sync with `atlas_entry` in `atlas_entry.rpl`.
KAN_REFLECTION_IGNORE
struct atlas_entry_gpu_data_t
{
    uint32_t flags;
    uint32_t page;

    struct kan_float_vector_2_t uv_min;
    struct kan_float_vector_2_t uv_max;

    /// \details Needed for nine slice.
    struct kan_float_vector_2_t pixel_size;

    uint32_t nine_slice_flags;
    float nine_slice_left;
    float nine_slice_right;
    float nine_slice_top;
    float nine_slice_bottom;

    uint32_t color_table_multiplier_index;
};

static void load_atlas (struct render_foundation_atlas_management_state_t *state,
                        const struct kan_resource_atlas_t *resource,
                        struct kan_render_atlas_loaded_t *loaded)
{
    KAN_CPU_SCOPED_STATIC_SECTION (load_atlas)
    KAN_UMI_SINGLETON_READ (render_context, kan_render_context_singleton_t)

    if (!KAN_HANDLE_IS_VALID (loaded->image) || !is_image_compatible_with_atlas (loaded->image, resource))
    {
        if (KAN_HANDLE_IS_VALID (loaded->image))
        {
            kan_render_image_destroy (loaded->image);
        }

        struct kan_render_image_description_t description = {
            .format = KAN_RENDER_IMAGE_FORMAT_RGBA32_SRGB,
            .width = resource->page_width,
            .height = resource->page_height,
            .depth = 1u,
            .layers = resource->page_count,
            .mips = 1u,

            .render_target = false,
            .supports_sampling = true,
            .always_treat_as_layered = true,
            .tracking_name = loaded->name,
        };

        loaded->image = kan_render_image_create (render_context->render_context, &description);
    }

    if (!KAN_HANDLE_IS_VALID (loaded->entry_buffer) ||
        kan_render_buffer_get_full_size (loaded->entry_buffer) !=
            sizeof (struct atlas_entry_gpu_data_t) * resource->total_entries)
    {
        if (KAN_HANDLE_IS_VALID (loaded->entry_buffer))
        {
            kan_render_buffer_destroy (loaded->entry_buffer);
        }

        // Currently, we do not fill initial data as we'd like to fill all entry gpu data into the patch address instead
        // of just using memcpy.
        loaded->entry_buffer = kan_render_buffer_create (
            render_context->render_context, KAN_RENDER_BUFFER_TYPE_STORAGE,
            sizeof (struct atlas_entry_gpu_data_t) * resource->total_entries, NULL, loaded->name);
    }

    if (!KAN_HANDLE_IS_VALID (loaded->image))
    {
        KAN_LOG (render_foundation_atlas, KAN_LOG_ERROR,
                 "Failed to finish loading of atlas \"%s\" as GPU image creation has failed.", loaded->name)
        return;
    }

    if (!KAN_HANDLE_IS_VALID (loaded->entry_buffer))
    {
        KAN_LOG (render_foundation_atlas, KAN_LOG_ERROR,
                 "Failed to finish loading of atlas \"%s\" as GPU entry buffer creation has failed.", loaded->name)
        return;
    }

    const kan_loop_size_t page_size = resource->page_width * resource->page_height * 4u;
    for (kan_loop_size_t page_index = 0u; page_index < resource->page_count; ++page_index)
    {
        kan_render_image_upload_data (loaded->image, (kan_instance_size_t) page_index, 0u, page_size,
                                      resource->data.data + page_size * page_index);
    }

    loaded->mapping.size = 0u;
    kan_dynamic_array_set_capacity (&loaded->mapping, resource->total_entries);

    loaded->locale_requirements.size = 0u;
    kan_dynamic_array_set_capacity (&loaded->locale_requirements, resource->total_entries);

    kan_loop_size_t data_index = 0u;
    struct atlas_entry_gpu_data_t *entry_data = kan_render_buffer_patch (
        loaded->entry_buffer, 0u, sizeof (struct atlas_entry_gpu_data_t) * resource->total_entries);

    for (kan_loop_size_t entry_index = 0u; entry_index < resource->entries.size; ++entry_index, ++data_index)
    {
        const struct kan_resource_atlas_entry_t *entry =
            &((struct kan_resource_atlas_entry_t *) resource->entries.data)[entry_index];
        const kan_loop_size_t data_index_start = data_index;

#define FILL_ENTRY_GPU_DATA                                                                                            \
    {                                                                                                                  \
        uint32_t entry_flags = 0u;                                                                                     \
        switch (input->type)                                                                                           \
        {                                                                                                              \
        case KAN_RESOURCE_ATLAS_IMAGE_TYPE_REGULAR:                                                                    \
            break;                                                                                                     \
                                                                                                                       \
        case KAN_RESOURCE_ATLAS_IMAGE_TYPE_NINE_SLICE:                                                                 \
            entry_flags |= ATLAS_ENTRY_GPU_FLAG_NINE_SLICE;                                                            \
            break;                                                                                                     \
        }                                                                                                              \
                                                                                                                       \
        if (input->color_table_multiplier_index != KAN_INT_MAX (kan_instance_size_t))                                  \
        {                                                                                                              \
            entry_flags |= ATLAS_ENTRY_GPU_FLAG_COLOR_MULTIPLIER;                                                      \
        }                                                                                                              \
                                                                                                                       \
        output->flags = entry_flags;                                                                                   \
        output->page = (uint32_t) input->page;                                                                         \
                                                                                                                       \
        output->uv_min.x = (float) input->x / (float) resource->page_width;                                            \
        output->uv_min.y = (float) input->y / (float) resource->page_height;                                           \
        output->uv_max.x = (float) (input->x + input->width) / (float) resource->page_width;                           \
        output->uv_max.y = (float) (input->y + input->height) / (float) resource->page_height;                         \
                                                                                                                       \
        output->pixel_size.x = (float) input->width;                                                                   \
        output->pixel_size.y = (float) input->height;                                                                  \
                                                                                                                       \
        if (input->type == KAN_RESOURCE_ATLAS_IMAGE_TYPE_NINE_SLICE)                                                   \
        {                                                                                                              \
            uint32_t nine_slice_flags = 0u;                                                                            \
            if (input->nine_slice.tiled_x)                                                                             \
            {                                                                                                          \
                nine_slice_flags |= ATLAS_ENTRY_NINE_SLICE_GPU_FLAG_TILED_X;                                           \
            }                                                                                                          \
                                                                                                                       \
            if (input->nine_slice.tiled_y)                                                                             \
            {                                                                                                          \
                nine_slice_flags |= ATLAS_ENTRY_NINE_SLICE_GPU_FLAG_TILED_Y;                                           \
            }                                                                                                          \
                                                                                                                       \
            output->nine_slice_flags = nine_slice_flags;                                                               \
            output->nine_slice_left = (float) input->nine_slice.left;                                                  \
            output->nine_slice_right = (float) input->nine_slice.right;                                                \
            output->nine_slice_top = (float) input->nine_slice.top;                                                    \
            output->nine_slice_bottom = (float) input->nine_slice.bottom;                                              \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            output->nine_slice_left = 0.0f;                                                                            \
            output->nine_slice_right = 0.0f;                                                                           \
            output->nine_slice_top = 0.0f;                                                                             \
            output->nine_slice_bottom = 0.0f;                                                                          \
        }                                                                                                              \
                                                                                                                       \
        output->color_table_multiplier_index = (uint32_t) input->color_table_multiplier_index;                         \
    }

        for (kan_loop_size_t replacement_index = 0u; replacement_index < entry->replacements.size;
             ++replacement_index, ++data_index)
        {
            const struct kan_resource_atlas_entry_replacement_t *replacement =
                &((struct kan_resource_atlas_entry_replacement_t *) entry->replacements.data)[replacement_index];

            *(kan_interned_string_t *) kan_dynamic_array_add_last (&loaded->locale_requirements) =
                replacement->for_locale;

            const struct kan_resource_atlas_image_t *input = &replacement->image;
            struct atlas_entry_gpu_data_t *output = &entry_data[data_index];
            FILL_ENTRY_GPU_DATA
        }

        *(kan_interned_string_t *) kan_dynamic_array_add_last (&loaded->locale_requirements) = NULL;
        const struct kan_resource_atlas_image_t *input = &entry->image;
        struct atlas_entry_gpu_data_t *output = &entry_data[data_index];
        FILL_ENTRY_GPU_DATA

        struct kan_render_atlas_loaded_entry_mapping_t *mapping = kan_dynamic_array_add_last (&loaded->mapping);
        mapping->entry_name = entry->name;
        mapping->match_start = data_index_start;
#undef FILL_ENTRY_GPU_DATA
    }

    {
        struct kan_render_atlas_loaded_entry_mapping_t temporary;
#define AT_INDEX(INDEX) (((struct kan_render_atlas_loaded_entry_mapping_t *) loaded->mapping.data)[INDEX])
#define LESS(first_index, second_index)                                                                                \
    __CUSHION_PRESERVE__ AT_INDEX (first_index).entry_name < AT_INDEX (second_index).entry_name
#define SWAP(first_index, second_index)                                                                                \
    __CUSHION_PRESERVE__                                                                                               \
    temporary = AT_INDEX (first_index), AT_INDEX (first_index) = AT_INDEX (second_index),                              \
    AT_INDEX (second_index) = temporary

        QSORT (loaded->mapping.size, LESS, SWAP);
#undef LESS
#undef SWAP
#undef AT_INDEX
    }

    KAN_UMO_EVENT_INSERT_INIT (kan_render_atlas_updated_event_t) {.name = loaded->name};
}

static void advance_from_waiting_state (struct render_foundation_atlas_management_state_t *state,
                                        struct kan_render_atlas_singleton_t *public,
                                        const struct kan_resource_provider_singleton_t *provider,
                                        struct render_foundation_atlas_t *atlas)
{
    KAN_LOG (render_foundation_atlas, KAN_LOG_DEBUG, "Attempting to advance atlas \"%s\" state from waiting to ready.",
             atlas->name)
    atlas->state_frame_id = provider->logic_deduplication_frame_id;
    KAN_UMI_RESOURCE_RETRIEVE_IF_LOADED_AND_FRESH (resource, kan_resource_atlas_t, &atlas->name)

    if (!resource)
    {
        // Still waiting.
        return;
    }

    atlas->state = RENDER_FOUNDATION_ATLAS_STATE_READY;
    KAN_UMI_VALUE_UPDATE_OPTIONAL (existing_loaded, kan_render_atlas_loaded_t, name, &atlas->name)

    if (existing_loaded)
    {
        load_atlas (state, resource, existing_loaded);
    }
    else
    {
        KAN_UMO_INDEXED_INSERT (new_loaded, kan_render_atlas_loaded_t)
        {
            new_loaded->name = atlas->name;
            load_atlas (state, resource, new_loaded);
        }
    }

    KAN_UMI_VALUE_DETACH_REQUIRED (usage, kan_resource_usage_t, usage_id, &atlas->usage_id)
    KAN_UM_ACCESS_DELETE (usage);
    atlas->usage_id = KAN_TYPED_ID_32_SET_INVALID (kan_resource_usage_id_t);

    KAN_ASSERT (public->loading_counter > 0u)
    --public->loading_counter;
    KAN_LOG (render_foundation_atlas, KAN_LOG_DEBUG, "Advanced atlas \"%s\" state to ready.", atlas->name)
}

UNIVERSE_RENDER_FOUNDATION_API KAN_UM_MUTATOR_EXECUTE (render_foundation_atlas_management)
{
    KAN_UMI_SINGLETON_READ (render_context, kan_render_context_singleton_t)
    if (!KAN_HANDLE_IS_VALID (render_context->render_context))
    {
        return;
    }

    KAN_UMI_SINGLETON_READ (resource_provider, kan_resource_provider_singleton_t)
    if (!resource_provider->scan_done)
    {
        return;
    }

    KAN_UMI_SINGLETON_WRITE (public, kan_render_atlas_singleton_t)
    KAN_UML_RESOURCE_UPDATED_EVENT_FETCH (updated_event, kan_resource_atlas_t)
    {
        on_atlas_resource_updated (state, public, resource_provider, updated_event->name);
    }

    KAN_UML_EVENT_FETCH (on_insert_event, render_foundation_atlas_usage_on_insert_event_t)
    {
        on_usage_insert (state, public, resource_provider, on_insert_event->atlas_name);
    }

    KAN_UML_EVENT_FETCH (on_delete_event, render_foundation_atlas_usage_on_delete_event_t)
    {
        on_usage_delete (state, public, on_delete_event->atlas_name);
    }

    KAN_UML_RESOURCE_LOADED_EVENT_FETCH (loaded_event, kan_resource_atlas_t)
    {
        KAN_UMI_VALUE_UPDATE_OPTIONAL (atlas, render_foundation_atlas_t, name, &loaded_event->name)
        if (atlas && atlas->state_frame_id != resource_provider->logic_deduplication_frame_id)
        {
            switch (atlas->state)
            {
            case RENDER_FOUNDATION_ATLAS_STATE_INITIAL:
            case RENDER_FOUNDATION_ATLAS_STATE_READY:
                KAN_ASSERT_FORMATTED (false,
                                      "Atlas \"%s\" in state %u received resource loaded event, which is totally "
                                      "unexpected in this state.",
                                      atlas->name, (unsigned int) atlas->state)
                break;

            case RENDER_FOUNDATION_ATLAS_STATE_WAITING:
                advance_from_waiting_state (state, public, resource_provider, atlas);
                break;
            }
        }
    }
}

void kan_render_atlas_singleton_init (struct kan_render_atlas_singleton_t *instance)
{
    instance->usage_id_counter = kan_atomic_int_init (1);
    instance->loading_counter = 0u;
}

void kan_render_atlas_loaded_init (struct kan_render_atlas_loaded_t *instance)
{
    instance->name = NULL;
    instance->image = KAN_HANDLE_SET_INVALID (kan_render_image_t);
    instance->entry_buffer = KAN_HANDLE_SET_INVALID (kan_render_buffer_t);

    kan_dynamic_array_init (&instance->mapping, 0u, sizeof (struct kan_render_atlas_loaded_entry_mapping_t),
                            alignof (struct kan_render_atlas_loaded_entry_mapping_t),
                            kan_allocation_group_stack_get ());

    kan_dynamic_array_init (&instance->locale_requirements, 0u, sizeof (kan_interned_string_t),
                            alignof (kan_interned_string_t), kan_allocation_group_stack_get ());
}

kan_instance_size_t kan_render_atlas_loaded_query (const struct kan_render_atlas_loaded_t *instance,
                                                   kan_interned_string_t entry_name,
                                                   kan_interned_string_t locale_name)
{
    kan_loop_size_t left = 0u;
    kan_loop_size_t right = instance->mapping.size;
    const struct kan_render_atlas_loaded_entry_mapping_t *mappings = instance->mapping.data;
    const kan_interned_string_t *locale_requirements = instance->locale_requirements.data;

    while (left < right)
    {
        kan_loop_size_t middle = (left + right) / 2u;
        if (entry_name < mappings[middle].entry_name)
        {
            right = middle;
        }
        else if (entry_name > mappings[middle].entry_name)
        {
            left = middle + 1u;
        }
        else
        {
            // Found the proper mapping. Now need to match locales.
            kan_loop_size_t match = mappings[middle].match_start;

            while (locale_requirements[match])
            {
                if (locale_requirements[match] == locale_name)
                {
                    // Locale-specific replacement discovered.
                    return match;
                }

                ++match;
                KAN_ASSERT (match < instance->locale_requirements.size)
            }

            // Encountered null-locale entry, match has ended with no replacements.
            return match;
        }
    }

    return KAN_INT_MAX (kan_instance_size_t);
}

void kan_render_atlas_loaded_shutdown (struct kan_render_atlas_loaded_t *instance)
{
    if (KAN_HANDLE_IS_VALID (instance->image))
    {
        kan_render_image_destroy (instance->image);
    }

    if (KAN_HANDLE_IS_VALID (instance->entry_buffer))
    {
        kan_render_buffer_destroy (instance->entry_buffer);
    }

    kan_dynamic_array_shutdown (&instance->mapping);
    kan_dynamic_array_shutdown (&instance->locale_requirements);
}
