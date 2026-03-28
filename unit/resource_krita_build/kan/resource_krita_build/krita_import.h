#pragma once

#include <resource_krita_build_api.h>

#include <kan/api_common/core_types.h>
#include <kan/container/dynamic_array.h>
#include <kan/container/interned_string.h>
#include <kan/error/critical.h>
#include <kan/reflection/markup.h>

/// \file
/// \brief Contains structure used to describe images imported from Krita file.
///
/// \par Overview
/// \parblock
/// This resource lists images that were imported from single Krita file through import build rule.
/// Only layers marked with "::" in their name are imported as images. Marked layer name format is
/// `<local_name> :: <meta>*`, where `<local_name>` is any sequence of characters prior to `::`,
/// that is stripped to remove unnecessary spaces in the name.
///
/// Produced image has name `<file_name_without_extension>_<local_name>` if `<local_name>` is not empty or just
/// `<file_name_without_extension>` if `<local_name>` is empty.
///
/// Meta strings are character sequences without whitespaces as whitespace is used to separate different meta.
/// Supported meta strings are:
/// - `L<locale_name>` -- image is treated as replacement for the image with the same name for this particular locale.
/// - `F<filter_name>` -- image receives given filter value that can be used for selecting images later.
///
/// Output file has name `<image_name>.png` if locale meta was not found or `<image_name>_<locale_name>.png` if locale
/// was found and image is treated as replacement.
/// \endparblock

KAN_C_HEADER_BEGIN

/// \brief Describes one imported image.
struct kan_resource_krita_header_entry_t
{
    /// \brief Logical name to be used in things like atlases.
    kan_interned_string_t name;

    /// \brief Name of the actual third party resource with image data.
    kan_interned_string_t file;

    /// \brief If image is locale replacement, contains locale name.
    kan_interned_string_t locale;

    /// \brief If image has filter value, contains this value.
    kan_interned_string_t filter;
};

RESOURCE_KRITA_BUILD_API void kan_resource_krita_header_entry_init (struct kan_resource_krita_header_entry_t *instance);

/// \brief Describes all the images imported from particular Krita file.
struct kan_resource_krita_header_t
{
    /// \brief List of PNG images imported from Krita file.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_krita_header_entry_t)
    struct kan_dynamic_array_t entries;
};

RESOURCE_KRITA_BUILD_API void kan_resource_krita_header_init (struct kan_resource_krita_header_t *instance);

RESOURCE_KRITA_BUILD_API void kan_resource_krita_header_shutdown (struct kan_resource_krita_header_t *instance);

KAN_C_HEADER_END
