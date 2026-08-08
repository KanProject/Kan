#define _CRT_SECURE_NO_WARNINGS __CUSHION_PRESERVE__

#include <string.h>

#include <kan/log/logging.h>
#include <kan/universe/macro.h>
#include <kan/universe_locale/locale.h>
#include <kan/universe_resource_provider/provider.h>

KAN_LOG_DEFINE_CATEGORY (locale);
KAN_USE_STATIC_INTERNED_IDS
KAN_USE_STATIC_CPU_SECTIONS

KAN_UM_ADD_MUTATOR_TO_FOLLOWING_GROUP (locale_management)
UNIVERSE_LOCALE_API KAN_UM_MUTATOR_GROUP_META (locale_management, KAN_LOCALE_MANAGEMENT_MUTATOR_GROUP);

struct locale_management_state_t
{
    KAN_UM_GENERATE_STATE_QUERIES (locale_management)
    KAN_UM_BIND_STATE (locale_management, state)
};

UNIVERSE_LOCALE_API KAN_UM_MUTATOR_DEPLOY (locale_management)
{
    kan_static_interned_ids_ensure_initialized ();
    kan_cpu_static_sections_ensure_initialized ();

    kan_workflow_graph_node_depend_on (workflow_node, KAN_LOCALE_MANAGEMENT_BEGIN_CHECKPOINT);
    kan_workflow_graph_node_make_dependency_of (workflow_node, KAN_LOCALE_MANAGEMENT_END_CHECKPOINT);
    // We're mostly post-processing data for tag edition, so we need write access to resource provider singleton,
    // but we do not want to clash with stuff that reads resource provider state after it changes, so we just do
    // our processing on technically the next frame before the real resource provider.
    kan_workflow_graph_node_make_dependency_of (workflow_node, KAN_RESOURCE_PROVIDER_BEGIN_CHECKPOINT);
}

static void remove_tags (struct kan_resource_provider_singleton_t *provider, const struct kan_resource_locale_t *locale)
{
    for (kan_instance_size_t index = 0u; index < locale->package_tags.size; ++index)
    {
        kan_interned_string_t tag = ((kan_interned_string_t *) locale->package_tags.data)[index];
        kan_resource_provider_singleton_remove_tag (provider, tag);
    }
}

static void add_tags (struct kan_resource_provider_singleton_t *provider, const struct kan_resource_locale_t *locale)
{
    for (kan_instance_size_t index = 0u; index < locale->package_tags.size; ++index)
    {
        kan_interned_string_t tag = ((kan_interned_string_t *) locale->package_tags.data)[index];
        kan_resource_provider_singleton_add_tag (provider, tag);
    }
}

UNIVERSE_LOCALE_API KAN_UM_MUTATOR_EXECUTE (locale_management)
{
    KAN_UMI_SINGLETON_WRITE (public, kan_locale_singleton_t)
    KAN_UMI_SINGLETON_WRITE (provider, kan_resource_provider_singleton_t)

    if (provider->transaction_state == KAN_RESOURCE_TRANSACTION_STATE_COMMIT)
    {
        KAN_UML_RESOURCE_LOADED_EVENT_FETCH (loaded_event, kan_resource_locale_t)
        {
            if (loaded_event->name != public->selected_locale)
            {
                continue;
            }

            KAN_UMI_RESOURCE_RETRIEVE_BOTH_LOADED (locale, kan_resource_locale_t, &loaded_event->name)
            // Should never happen if loaded events work correctly.
            KAN_ASSERT (locale_current != locale_fresh)

            if (locale_current)
            {
                remove_tags (provider, locale_current);
            }

            if (locale_fresh)
            {
                add_tags (provider, locale_fresh);
            }

            KAN_UMO_EVENT_INSERT_INIT (kan_locale_updated_event_t) {
                .old_selection = public->selected_locale,
                .new_selection = public->selected_locale,
            };
        }

        KAN_UML_RESOURCE_UNLOAD_PLANNED_EVENT_FETCH (unload_event, kan_resource_locale_t)
        {
            if (unload_event->name != public->selected_locale)
            {
                continue;
            }

            KAN_UMI_RESOURCE_RETRIEVE_LOADED (locale, kan_resource_locale_t, &unload_event->name)
            remove_tags (provider, locale);

            KAN_UMO_EVENT_INSERT_INIT (kan_locale_updated_event_t) {
                .old_selection = public->selected_locale,
                .new_selection = public->selected_locale,
            };
        }
    }

    if (provider->transaction_state == KAN_RESOURCE_TRANSACTION_STATE_NONE ||
        provider->transaction_state == KAN_RESOURCE_TRANSACTION_STATE_COMMIT)
    {
        kan_interned_string_t new_locale_name = NULL;
        KAN_UML_EVENT_FETCH (request, kan_locale_selection_request_t)
        {
            if (request->new_locale)
            {
                new_locale_name = request->new_locale;
            }
        }

        if (new_locale_name)
        {
            // Request fresh as we're supporting request processing during transaction commit.
            KAN_UMI_RESOURCE_RETRIEVE_FRESH_LOADED (old_locale, kan_resource_locale_t, &public->selected_locale)
            KAN_UMI_RESOURCE_RETRIEVE_FRESH_LOADED (new_locale, kan_resource_locale_t, &new_locale_name)

            if (old_locale)
            {
                remove_tags (provider, old_locale);
            }

            if (new_locale)
            {
                add_tags (provider, new_locale);
            }

            KAN_UMO_EVENT_INSERT_INIT (kan_locale_updated_event_t) {
                .old_selection = public->selected_locale,
                .new_selection = new_locale_name,
            };

            public->selected_locale = new_locale_name;
        }
    }
}

void kan_locale_singleton_init (struct kan_locale_singleton_t *instance) { instance->selected_locale = NULL; }
