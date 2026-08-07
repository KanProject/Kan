#define _CRT_SECURE_NO_WARNINGS __CUSHION_PRESERVE__

#include <string.h>

#include <kan/log/logging.h>
#include <kan/resource_text/font.h>
#include <kan/universe/macro.h>
#include <kan/universe_locale/locale.h>
#include <kan/universe_render_foundation/render_graph.h>
#include <kan/universe_resource_provider/provider.h>
#include <kan/universe_text/text.h>

KAN_LOG_DEFINE_CATEGORY (text_management);
KAN_LOG_DEFINE_CATEGORY (text_shaping);
KAN_USE_STATIC_INTERNED_IDS
KAN_USE_STATIC_CPU_SECTIONS

KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (text_management)
UNIVERSE_TEXT_API KAN_UM_MUTATOR_GROUP_META (text_management, KAN_TEXT_MANAGEMENT_MUTATOR_GROUP);

KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (text_shaping)
UNIVERSE_TEXT_API KAN_UM_MUTATOR_GROUP_META (text_shaping, KAN_TEXT_SHAPING_MUTATOR_GROUP);

struct text_management_private_singleton_t
{
    bool pending_full_reload;
};

UNIVERSE_TEXT_API void text_management_private_singleton_init (struct text_management_private_singleton_t *instance)
{
    instance->pending_full_reload = false;
}

struct font_library_t
{
    kan_interned_string_t name;
    kan_font_library_t library;
    kan_interned_string_t usage_class;
    kan_instance_size_t loading_frame_id;
};

KAN_REFLECTION_STRUCT_META (font_library_t)
UNIVERSE_TEXT_API struct kan_repository_meta_automatic_cascade_deletion_t font_library_usage_id_cascade_deletion = {
    .parent_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"name"}},
    .child_type_name = "font_file_link_t",
    .child_key_path = {.reflection_path_length = 1u, .reflection_path = (const char *[]) {"library_name"}},
};

UNIVERSE_TEXT_API void font_library_init (struct font_library_t *instance)
{
    instance->name = NULL;
    instance->library = KAN_HANDLE_SET_INVALID (kan_font_library_t);
    instance->usage_class = NULL;
    instance->loading_frame_id = 0u;
}

UNIVERSE_TEXT_API void font_library_shutdown (struct font_library_t *instance)
{
    if (KAN_HANDLE_IS_VALID (instance->library))
    {
        kan_font_library_destroy (instance->library);
    }
}

struct font_file_link_t
{
    kan_interned_string_t library_name;
    kan_interned_string_t file_name;
};

struct font_library_updated_event_t
{
    kan_interned_string_t name;
};

struct text_management_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (text_management)
    KAN_UM_BIND_STATE (text_management, state)

    kan_allocation_group_t temporary_group;
};

UNIVERSE_TEXT_API void text_management_state_init (struct text_management_state_t *instance)
{
    instance->temporary_group = kan_allocation_group_get_child (kan_allocation_group_stack_get (), "temporary");
}

UNIVERSE_TEXT_API KAN_UM_MUTATOR_DEPLOY (text_management)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_cpu_static_sections_ensure_initialized ();

    kan_workflow_graph_node_depend_on (workflow_node, KAN_RESOURCE_PROVIDER_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_RENDER_FOUNDATION_FRAME_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_TEXT_MANAGEMENT_BEGIN_CHECKPOINT);
    kan_workflow_graph_node_make_dependency_of (workflow_node, KAN_TEXT_MANAGEMENT_END_CHECKPOINT);
}

static void load_font_library (struct text_management_state_t *state,
                               const struct kan_resource_provider_singleton_t *provider,
                               const struct kan_locale_singleton_t *locale_selection,
                               const struct kan_render_context_singleton_t *render_context,
                               struct font_library_t *library)
{
    KAN_CPU_SCOPED_STATIC_SECTION (load_font_library)
    library->loading_frame_id = provider->logic_deduplication_frame_id;
    library->usage_class = NULL;
    KAN_UML_VALUE_DETACH (old_link, font_file_link_t, library_name, &library->name) { KAN_UM_ACCESS_DELETE (old_link); }

    if (KAN_HANDLE_IS_VALID (library->library))
    {
        kan_font_library_destroy (library->library);
        library->library = KAN_HANDLE_SET_INVALID (kan_font_library_t);
    }

    KAN_UMI_RESOURCE_RETRIEVE_FRESH_LOADED (locale, kan_resource_locale_t, &locale_selection->selected_locale)
    if (!locale)
    {
        KAN_LOG (text_management, KAN_LOG_ERROR,
                 "Cannot properly create font library \"%s\" as current locale is not accessible!", library->name)
        KAN_UMO_EVENT_INSERT_INIT (font_library_updated_event_t) {.name = library->name};
        return;
    }

    KAN_UMI_RESOURCE_RETRIEVE_FRESH_LOADED (resource, kan_resource_font_library_t, &library->name)
    if (resource_entry_view->unload_planned)
    {
        // Ignore, will be deleted after the transaction.
        KAN_UMO_EVENT_INSERT_INIT (font_library_updated_event_t) {.name = library->name};
        return;
    }

    KAN_ASSERT (resource)
    library->usage_class = resource->usage_class;
    struct kan_font_library_category_t categories_static[KAN_UNIVERSE_TEXT_FONT_CATEGORY_INIT_STACK];
    kan_instance_size_t categories_size = KAN_UNIVERSE_TEXT_FONT_CATEGORY_INIT_STACK;
    struct kan_font_library_category_t *categories = categories_static;
    kan_instance_size_t selected_categories_count = 0u;

    CUSHION_DEFER
    {
        if (categories_static != categories)
        {
            kan_free_general (state->temporary_group, categories,
                              sizeof (struct kan_font_library_category_t) * categories_size);
        }
    }

    // We have to honor order in locale to make sure that font library categories order matches order of languages
    // in locale resource to avoid unexpected behaviors.
    for (kan_instance_size_t locale_language_index = 0u; locale_language_index < locale->font_languages.size;
         ++locale_language_index)
    {
        for (kan_instance_size_t category_index = 0u; category_index < resource->categories.size; ++category_index)
        {
            const struct kan_resource_font_category_t *category =
                &((struct kan_resource_font_category_t *) resource->categories.data)[category_index];
            bool filtered_in = false;

            for (kan_instance_size_t category_language_index = 0u;
                 category_language_index < category->used_for_languages.size; ++category_language_index)
            {
                if (((kan_interned_string_t *) category->used_for_languages.data)[category_language_index] ==
                    ((kan_interned_string_t *) locale->font_languages.data)[locale_language_index])
                {
                    filtered_in = true;
                    break;
                }
            }

            if (!filtered_in)
            {
                continue;
            }

            if (selected_categories_count + category->styles.size > categories_size)
            {
                const kan_instance_size_t new_categories_size =
                    KAN_MAX (categories_size * 2u, categories_size + category->styles.size);

                struct kan_font_library_category_t *new_categories = kan_allocate_general (
                    state->temporary_group, sizeof (struct kan_font_library_category_t) * new_categories_size,
                    alignof (struct kan_font_library_category_t));

                memcpy (new_categories, categories,
                        sizeof (struct kan_font_library_category_t) * selected_categories_count);

                if (categories_static != categories)
                {
                    kan_free_general (state->temporary_group, categories,
                                      sizeof (struct kan_font_library_category_t) * categories_size);
                }

                categories_size = new_categories_size;
                categories = new_categories;
            }

            for (kan_memory_size_t style_index = 0u; style_index < category->styles.size; ++style_index)
            {
                const struct kan_resource_font_style_t *style =
                    &((struct kan_resource_font_style_t *) category->styles.data)[style_index];

                KAN_UMI_RESOURCE_RETRIEVE_FRESH_LOADED_THIRD_PARTY (data, &style->font_data_file)
                if (!data || data_size == 0u)
                {
                    KAN_LOG (text_management, KAN_LOG_ERROR,
                             "Font library \"%s\" is unable to find font data \"%s\" from style \"%s\" from category "
                             "\"%s\"!",
                             library->name, style->font_data_file, style->style, category->used_for_languages)
                    continue;
                }

                if (data_entry_view->unload_planned)
                {
                    KAN_LOG (text_management, KAN_LOG_ERROR,
                             "Font library \"%s\" is unable to tried to use font data \"%s\" from style \"%s\" from "
                             "category \"%s\", but it is marked for unload!",
                             library->name, style->font_data_file, style->style, category->used_for_languages)
                    continue;
                }

                struct kan_font_library_category_t *setup = &categories[selected_categories_count];
                ++selected_categories_count;
                KAN_ASSERT (selected_categories_count <= categories_size)

                setup->script = category->script;
                setup->style = style->style;
                setup->variable_axis_count = style->variable_font_axes.size;
                setup->variable_axis = (kan_floating_t *) style->variable_font_axes.data;
                setup->data_size = data_size;
                setup->data = data;
            }
        }
    }

    library->library = kan_font_library_create (render_context->render_context, selected_categories_count, categories);
    if (KAN_HANDLE_IS_VALID (library->library))
    {
        KAN_CPU_SCOPED_STATIC_SECTION (font_library_precache)
        for (kan_instance_size_t locale_language_index = 0u; locale_language_index < locale->font_languages.size;
             ++locale_language_index)
        {
            for (kan_instance_size_t category_index = 0u; category_index < resource->categories.size; ++category_index)
            {
                const struct kan_resource_font_category_t *category =
                    &((struct kan_resource_font_category_t *) resource->categories.data)[category_index];
                bool filtered_in = false;

                for (kan_instance_size_t category_language_index = 0u;
                     category_language_index < category->used_for_languages.size; ++category_language_index)
                {
                    if (((kan_interned_string_t *) category->used_for_languages.data)[category_language_index] ==
                        ((kan_interned_string_t *) locale->font_languages.data)[locale_language_index])
                    {
                        filtered_in = true;
                        break;
                    }
                }

                if (!filtered_in)
                {
                    continue;
                }

                for (kan_memory_size_t style_index = 0u; style_index < category->styles.size; ++style_index)
                {
                    const struct kan_resource_font_style_t *style =
                        &((struct kan_resource_font_style_t *) category->styles.data)[style_index];

                    struct kan_text_precache_request_t precache_request = {
                        .script = category->script,
                        .style = style->style,
                        .render_format = KAN_FONT_GLYPH_RENDER_FORMAT_SDF,
                        .orientation = KAN_TEXT_ORIENTATION_HORIZONTAL,
                        .utf8 = NULL,
                    };

                    if (category->precache_utf8_horizontal)
                    {
                        precache_request.utf8 = category->precache_utf8_horizontal;
                        if (!kan_font_library_precache (library->library, &precache_request))
                        {
                            KAN_LOG (text_management, KAN_LOG_ERROR,
                                     "Failed to execute precache for library \"%s\" script \"%s\" style \"%s\".",
                                     library->name, category->script, style->style ? style->style : "<default>")
                        }
                    }

                    if (category->precache_utf8_vertical)
                    {
                        precache_request.orientation = KAN_TEXT_ORIENTATION_VERTICAL;
                        precache_request.utf8 = category->precache_utf8_vertical;

                        if (!kan_font_library_precache (library->library, &precache_request))
                        {
                            KAN_LOG (text_management, KAN_LOG_ERROR,
                                     "Failed to execute precache for library \"%s\" script \"%s\" style \"%s\".",
                                     library->name, category->script, style->style ? style->style : "<default>")
                        }
                    }
                }
            }
        }
    }
    else
    {
        KAN_LOG (text_management, KAN_LOG_ERROR, "Failed to create font library \"%s\".", library->name)
    }

    KAN_UMO_EVENT_INSERT_INIT (font_library_updated_event_t) {.name = library->name};
}

UNIVERSE_TEXT_API KAN_UM_MUTATOR_EXECUTE (text_management)
{
    KAN_UMI_SINGLETON_READ (render_context, kan_render_context_singleton_t)
    if (!KAN_HANDLE_IS_VALID (render_context->render_context))
    {
        // Won't be able to render glyphs until render context is available.
        return;
    }

    KAN_UMI_SINGLETON_READ (provider, kan_resource_provider_singleton_t)
    KAN_UMI_SINGLETON_WRITE (private, text_management_private_singleton_t)
    KAN_UMI_SINGLETON_READ (locale_selection, kan_locale_singleton_t)

    KAN_UML_EVENT_FETCH (locale_updated_event, kan_locale_updated_event_t)
    {
        if (provider->transaction_state != KAN_RESOURCE_TRANSACTION_STATE_NONE || provider->tags_dirty)
        {
            private->pending_full_reload = true;
            continue;
        }

        KAN_UML_SEQUENCE_UPDATE (library, font_library_t)
        {
            if (library->loading_frame_id != provider->logic_deduplication_frame_id)
            {
                load_font_library (state, provider, locale_selection, render_context, library);
            }
        }
    }

    if (provider->transaction_state == KAN_RESOURCE_TRANSACTION_STATE_COMMIT)
    {
        KAN_UML_RESOURCE_LOADED_EVENT_FETCH (library_loaded_event, kan_resource_font_library_t)
        {
            KAN_UMI_VALUE_UPDATE_OPTIONAL (library, font_library_t, name, &library_loaded_event->name)
            if (library)
            {
                if (library->loading_frame_id != provider->logic_deduplication_frame_id)
                {
                    load_font_library (state, provider, locale_selection, render_context, library);
                }
            }
            else
            {
                KAN_UMI_INDEXED_INSERT (new_library, font_library_t)
                new_library->name = library_loaded_event->name;
                load_font_library (state, provider, locale_selection, render_context, new_library);
            }
        }

        KAN_UML_EVENT_FETCH (third_party_loaded_event, kan_resource_third_party_loaded_event_t)
        {
            KAN_UMI_VALUE_UPDATE_OPTIONAL (link, font_file_link_t, file_name, &third_party_loaded_event->name)
            if (link)
            {
                KAN_UMI_VALUE_UPDATE_OPTIONAL (library, font_library_t, name, &link->library_name)
                if (library && library->loading_frame_id != provider->logic_deduplication_frame_id)
                {
                    KAN_UM_ACCESS_CLOSE_IMMEDIATELY (link);
                    load_font_library (state, provider, locale_selection, render_context, library);
                }
            }
        }

        // We have to process third party unloads right away in order to destroy objects that rely on that data
        // sitting in the memory at the expected address.
        KAN_UML_EVENT_FETCH (third_party_unload_event, kan_resource_third_party_unload_planned_event_t)
        {
            KAN_UMI_VALUE_UPDATE_OPTIONAL (link, font_file_link_t, file_name, &third_party_unload_event->name)
            if (link)
            {
                KAN_UMI_VALUE_UPDATE_OPTIONAL (library, font_library_t, name, &link->library_name)
                if (library && library->loading_frame_id != provider->logic_deduplication_frame_id)
                {
                    KAN_UM_ACCESS_CLOSE_IMMEDIATELY (link);
                    load_font_library (state, provider, locale_selection, render_context, library);
                }
            }
        }

        // Only do pending full reload if there is no dirty tags, as otherwise it might mean that locale changes have
        // not yet reached resource transaction control.
        if (private->pending_full_reload && !provider->tags_dirty)
        {
            private->pending_full_reload = false;
            KAN_UML_SEQUENCE_UPDATE (library, font_library_t)
            {
                if (library->loading_frame_id != provider->logic_deduplication_frame_id)
                {
                    load_font_library (state, provider, locale_selection, render_context, library);
                }
            }
        }
    }

    if (provider->transaction_state == KAN_RESOURCE_TRANSACTION_STATE_NONE)
    {
        KAN_UML_RESOURCE_UNLOAD_PLANNED_EVENT_FETCH (unload_event, kan_resource_font_library_t)
        {
            KAN_UMI_VALUE_DELETE_OPTIONAL (library, font_library_t, name, &unload_event->name)
            if (library)
            {
                KAN_UM_ACCESS_DELETE (library);
            }
        }
    }
}

struct text_shaping_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (text_shaping)
    KAN_UM_BIND_STATE (text_shaping, state)
};

UNIVERSE_TEXT_API KAN_UM_MUTATOR_DEPLOY (text_shaping)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_cpu_static_sections_ensure_initialized ();

    // Should normally be in different worlds, but add dependency just in case.
    kan_workflow_graph_node_depend_on (workflow_node, KAN_TEXT_MANAGEMENT_END_CHECKPOINT);
    kan_workflow_graph_node_depend_on (workflow_node, KAN_TEXT_SHAPING_BEGIN_CHECKPOINT);
    kan_workflow_graph_node_make_dependency_of (workflow_node, KAN_TEXT_SHAPING_END_CHECKPOINT);
}

static inline void shaping_unit_clean_shaped_data (struct kan_text_shaping_unit_t *unit)
{
    if (unit->shaped)
    {
        if (unit->shaped_as_stable)
        {
            if (KAN_HANDLE_IS_VALID (unit->shaped_stable.glyphs))
            {
                kan_render_buffer_destroy (unit->shaped_stable.glyphs);
            }

            if (KAN_HANDLE_IS_VALID (unit->shaped_stable.icons))
            {
                kan_render_buffer_destroy (unit->shaped_stable.icons);
            }
        }
        else
        {
            kan_dynamic_array_shutdown (&unit->shaped_unstable.glyphs);
            kan_dynamic_array_shutdown (&unit->shaped_unstable.icons);
        }
    }

    for (kan_memory_size_t index = 0u; index < unit->shaped_edition_sequences.size; ++index)
    {
        struct kan_text_shaped_edition_sequence_data_t *data =
            &((struct kan_text_shaped_edition_sequence_data_t *) unit->shaped_edition_sequences.data)[index];
        kan_text_shaped_edition_sequence_data_shutdown (data);
    }

    unit->shaped_edition_sequences.size = 0u;
    kan_dynamic_array_set_capacity (&unit->shaped_edition_sequences, 0u);
}

static void shaping_unit_on_failed (struct kan_text_shaping_unit_t *unit)
{
    shaping_unit_clean_shaped_data (unit);
    unit->shaped = false;
    unit->shaped_as_stable = false;
}

static void shape_unit (struct text_shaping_state_t *state,
                        struct kan_text_shaping_unit_t *unit,
                        const struct kan_resource_locale_t *locale,
                        kan_font_library_t font_library,
                        kan_render_context_t render_context)
{
    unit->dirty = false;
    if (unit->request.primary_axis_limit == 0u || !KAN_HANDLE_IS_VALID (unit->request.text))
    {
        // Silent failure that is actually an expected skip by the docs.
        shaping_unit_on_failed (unit);
        return;
    }

    switch (locale->preferred_direction)
    {
    case KAN_LOCALE_PREFERRED_TEXT_DIRECTION_LEFT_TO_RIGHT:
        unit->request.reading_direction = KAN_TEXT_READING_DIRECTION_LEFT_TO_RIGHT;
        break;

    case KAN_LOCALE_PREFERRED_TEXT_DIRECTION_RIGHT_TO_LEFT:
        unit->request.reading_direction = KAN_TEXT_READING_DIRECTION_RIGHT_TO_LEFT;
        break;
    }

    struct kan_text_shaped_data_t shaped_data;
    kan_allocation_group_stack_push (unit->allocation_group);
    kan_text_shaped_data_init (&shaped_data);
    kan_allocation_group_stack_pop ();

    if (!kan_font_library_shape (font_library, &unit->request, &shaped_data))
    {
        kan_text_shaped_data_shutdown (&shaped_data);
        KAN_LOG (text_shaping, KAN_LOG_ERROR, "Failed to execute text shaping due to errors in backend.")
        shaping_unit_on_failed (unit);
        return;
    }

    unit->shaped_primary_size = shaped_data.typographic_primary_size;
    unit->shaped_secondary_size = shaped_data.typographic_secondary_size;

    if (unit->stable)
    {
        const kan_instance_size_t glyphs_data_size =
            sizeof (struct kan_text_shaped_glyph_instance_data_t) * shaped_data.glyphs.size;

        const kan_instance_size_t icons_data_size =
            sizeof (struct kan_text_shaped_icon_instance_data_t) * shaped_data.icons.size;

        if (!unit->shaped || !unit->shaped_as_stable)
        {
            shaping_unit_clean_shaped_data (unit);
            unit->shaped_stable.glyphs = KAN_HANDLE_SET_INVALID (kan_render_buffer_t);
            unit->shaped_stable.icons = KAN_HANDLE_SET_INVALID (kan_render_buffer_t);
        }

        unit->shaped_stable.glyphs_count = shaped_data.glyphs.size;
        unit->shaped_stable.icons_count = shaped_data.icons.size;

        if (KAN_HANDLE_IS_VALID (unit->shaped_stable.glyphs) && glyphs_data_size > 0u &&
            kan_render_buffer_get_full_size (unit->shaped_stable.glyphs) >= glyphs_data_size)
        {
            void *data = kan_render_buffer_patch (unit->shaped_stable.glyphs, 0u, glyphs_data_size);
            memcpy (data, shaped_data.glyphs.data, glyphs_data_size);
        }
        else
        {
            if (KAN_HANDLE_IS_VALID (unit->shaped_stable.glyphs))
            {
                kan_render_buffer_destroy (unit->shaped_stable.glyphs);
                unit->shaped_stable.glyphs = KAN_HANDLE_SET_INVALID (kan_render_buffer_t);
            }

            if (glyphs_data_size > 0u)
            {
                unit->shaped_stable.glyphs =
                    kan_render_buffer_create (render_context, KAN_RENDER_BUFFER_TYPE_ATTRIBUTE, glyphs_data_size,
                                              shaped_data.glyphs.data, KAN_STATIC_INTERNED_ID_GET (shaped_glyphs));
                KAN_ASSERT (KAN_HANDLE_IS_VALID (unit->shaped_stable.glyphs))
            }
        }

        if (KAN_HANDLE_IS_VALID (unit->shaped_stable.icons) && icons_data_size > 0u &&
            kan_render_buffer_get_full_size (unit->shaped_stable.icons) >= icons_data_size)
        {
            void *data = kan_render_buffer_patch (unit->shaped_stable.icons, 0u, icons_data_size);
            memcpy (data, shaped_data.icons.data, icons_data_size);
        }
        else
        {
            if (KAN_HANDLE_IS_VALID (unit->shaped_stable.icons))
            {
                kan_render_buffer_destroy (unit->shaped_stable.icons);
                unit->shaped_stable.icons = KAN_HANDLE_SET_INVALID (kan_render_buffer_t);
            }

            if (icons_data_size > 0u)
            {
                unit->shaped_stable.icons =
                    kan_render_buffer_create (render_context, KAN_RENDER_BUFFER_TYPE_ATTRIBUTE, icons_data_size,
                                              shaped_data.icons.data, KAN_STATIC_INTERNED_ID_GET (shaped_icons));
                KAN_ASSERT (KAN_HANDLE_IS_VALID (unit->shaped_stable.icons))
            }
        }

        KAN_DYNAMIC_ARRAY_SHUTDOWN_WITH_ITEMS_AUTO (unit->shaped_edition_sequences,
                                                    kan_text_shaped_edition_sequence_data)
        kan_dynamic_array_init_move (&unit->shaped_edition_sequences, &shaped_data.edition_sequences);
        kan_text_shaped_data_shutdown (&shaped_data);
    }
    else
    {
        shaping_unit_clean_shaped_data (unit);
        // On purpose: just copy pointers and go, do not shutdown shaped data on stack.
        unit->shaped_unstable.glyphs = shaped_data.glyphs;
        unit->shaped_unstable.icons = shaped_data.icons;

        KAN_DYNAMIC_ARRAY_SHUTDOWN_WITH_ITEMS_AUTO (unit->shaped_edition_sequences,
                                                    kan_text_shaped_edition_sequence_data)
        unit->shaped_edition_sequences = shaped_data.edition_sequences;
    }

    unit->shaped = true;
    unit->shaped_as_stable = unit->stable;
    KAN_UMO_EVENT_INSERT_INIT (kan_text_shaped_t) {.id = unit->id};
}

UNIVERSE_TEXT_API KAN_UM_MUTATOR_EXECUTE (text_shaping)
{
    KAN_UMI_SINGLETON_WRITE (public, kan_text_shaping_singleton_t)
    KAN_UMI_SINGLETON_READ (locale_singleton, kan_locale_singleton_t)
    KAN_UMI_RESOURCE_RETRIEVE_LOADED (locale, kan_resource_locale_t, &locale_singleton->selected_locale)

    if (!locale)
    {
        // Can't shape while locale is not available.
        return;
    }

    public->font_library_sdf_atlas = KAN_HANDLE_SET_INVALID (kan_render_image_t);
    kan_font_library_t selected_font_library = KAN_HANDLE_SET_INVALID (kan_font_library_t);
    kan_interned_string_t selected_font_library_name = NULL;

    KAN_UML_SEQUENCE_READ (font_library, font_library_t)
    {
        if (KAN_HANDLE_IS_VALID (font_library->library) && font_library->usage_class == public->library_usage_class)
        {
            selected_font_library = font_library->library;
            selected_font_library_name = font_library->name;
            break;
        }
    }

    if (!KAN_HANDLE_IS_VALID (selected_font_library))
    {
        KAN_LOG (text_shaping, KAN_LOG_ERROR,
                 "Failed to execute text shaping as font library with usage class \"%s\" is not found.",
                 selected_font_library)
        return;
    }

    KAN_UMI_SINGLETON_READ (render_context, kan_render_context_singleton_t)
    KAN_ASSERT (KAN_HANDLE_IS_VALID (render_context->render_context))

    bool after_loading_reshape = false;
    KAN_UML_EVENT_FETCH (loaded_event, font_library_updated_event_t)
    {
        if (loaded_event->name == selected_font_library_name)
        {
            after_loading_reshape = true;
        }
    }

    // Right now, shaping mutator implementation is intentionally not multithreaded:
    // We do not expect to get that many shaping requests per frame in order to make multithreading justified.
    // Nothing in implementation blocks multithreading and shaping can be safely done from batched tasks.
    // However, no sense to migrate it to batched tasks for now if we are rarely going to reshape more than 10 small
    // unstable units per frame.

    if (after_loading_reshape)
    {
        KAN_CPU_SCOPED_STATIC_SECTION (after_loading_reshape)
        KAN_UML_SEQUENCE_UPDATE (unit, kan_text_shaping_unit_t)
        {
            if (!unit->stable)
            {
                // Will be reshaped below anyway.
                continue;
            }

            shape_unit (state, unit, locale, selected_font_library, render_context->render_context);
        }
    }

    {
        KAN_CPU_SCOPED_STATIC_SECTION (shape_unstable)
        KAN_UML_SIGNAL_UPDATE (unit, kan_text_shaping_unit_t, stable, false)
        {
            shape_unit (state, unit, locale, selected_font_library, render_context->render_context);
        }
    }

    {
        KAN_CPU_SCOPED_STATIC_SECTION (shape_dirty)
        KAN_UML_SIGNAL_UPDATE (unit, kan_text_shaping_unit_t, dirty, true)
        {
            if (!unit->stable)
            {
                // Not stable unit, reset flag and skip it.
                unit->dirty = false;
                continue;
            }

            shape_unit (state, unit, locale, selected_font_library, render_context->render_context);
        }
    }

    public->font_library_sdf_atlas = kan_font_library_get_sdf_atlas (selected_font_library);
}

void kan_text_shaping_singleton_init (struct kan_text_shaping_singleton_t *instance)
{
    instance->unit_id_counter = kan_atomic_int_init (1);
    instance->library_usage_class = NULL;
    instance->font_library_sdf_atlas = KAN_HANDLE_SET_INVALID (kan_render_image_t);
}

void kan_text_shaping_unit_init (struct kan_text_shaping_unit_t *instance)
{
    instance->id = KAN_TYPED_ID_32_SET_INVALID (kan_text_shaping_unit_id_t);
    instance->request.font_size = 24u;
    instance->request.render_format = KAN_FONT_GLYPH_RENDER_FORMAT_SDF;
    instance->request.orientation = KAN_TEXT_ORIENTATION_HORIZONTAL;
    instance->request.reading_direction = KAN_TEXT_READING_DIRECTION_LEFT_TO_RIGHT;
    instance->request.alignment = KAN_TEXT_SHAPING_ALIGNMENT_LEFT;
    instance->request.primary_axis_limit = 0u;
    instance->request.allow_breaks = true;
    instance->request.generate_edition_markup = false;
    instance->request.text = KAN_HANDLE_SET_INVALID (kan_text_t);

    instance->stable = true;
    instance->dirty = true;
    instance->shaped = false;
    instance->shaped_as_stable = true;

    instance->shaped_primary_size = 0u;
    instance->shaped_secondary_size = 0u;

    instance->shaped_stable.glyphs_count = 0u;
    instance->shaped_stable.icons_count = 0u;
    instance->shaped_stable.glyphs = KAN_HANDLE_SET_INVALID (kan_render_buffer_t);
    instance->shaped_stable.icons = KAN_HANDLE_SET_INVALID (kan_render_buffer_t);

    kan_dynamic_array_init (
        &instance->shaped_edition_sequences, 0u, sizeof (struct kan_text_shaped_edition_sequence_data_t),
        alignof (struct kan_text_shaped_edition_sequence_data_t), kan_allocation_group_stack_get ());
    instance->allocation_group = kan_allocation_group_stack_get ();
}

void kan_text_shaping_unit_shutdown (struct kan_text_shaping_unit_t *instance)
{
    if (KAN_HANDLE_IS_VALID (instance->request.text))
    {
        kan_text_destroy (instance->request.text);
    }

    shaping_unit_clean_shaped_data (instance);
    KAN_DYNAMIC_ARRAY_SHUTDOWN_WITH_ITEMS_AUTO (instance->shaped_edition_sequences,
                                                kan_text_shaped_edition_sequence_data)
}
