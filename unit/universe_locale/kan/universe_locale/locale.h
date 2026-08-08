#pragma once

#include <universe_locale_api.h>

#include <kan/api_common/c_header.h>
#include <kan/api_common/core_types.h>
#include <kan/resource_locale/locale.h>
#include <kan/threading/atomic.h>
#include <kan/universe/universe.h>

/// \brief Provides API for interacting with locale management implementation.
///
/// \par Definition
/// \parblock
/// Locale management automatically manages resource package loading tags from locale selection in
/// `kan_locale_singleton_t`. Locale resources loading is done automatically by resource provider itself.
/// \endparblock

KAN_C_HEADER_BEGIN

/// \brief Group that is used to add all locale management mutators.
#define KAN_LOCALE_MANAGEMENT_MUTATOR_GROUP "locale_management"

/// \brief Checkpoint, after which locale management mutators are executed.
#define KAN_LOCALE_MANAGEMENT_BEGIN_CHECKPOINT "locale_management_begin"

/// \brief Checkpoint, that is hit after all locale management mutators have finished execution.
#define KAN_LOCALE_MANAGEMENT_END_CHECKPOINT "locale_management_end"

/// \brief Singleton for selecting current locale.
struct kan_locale_singleton_t
{
    /// \brief Selected locale name.
    /// \invariant Do not edit it manually! Use `kan_locale_selection_request_t` to ask it to be properly changed!
    kan_interned_string_t selected_locale;
};

UNIVERSE_LOCALE_API void kan_locale_singleton_init (struct kan_locale_singleton_t *instance);

/// \brief Event for asking current selected locale to be changed.
struct kan_locale_selection_request_t
{
    /// \brief New requested locale name. Request is ignored if it is NULL.
    kan_interned_string_t new_locale;
};

/// \brief Event that is sent when either locale selection was changed or locale data was loaded or updated.
/// \details Both locale change and locale hot reload need the same set of actions: reloading dependent data.
///          Therefore it is merged into one event. It also means that `old_selection` will be equal to `new_selection`
///          for the cases when it was related to loading or hot reload.
struct kan_locale_updated_event_t
{
    kan_interned_string_t old_selection;
    kan_interned_string_t new_selection;
};

KAN_C_HEADER_END
