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
/// Produced image has name `<file_name_without_extension>(_<parent_prefix>)*(_<local_name>)?` where part with 
/// `<local_name>` is absent if this name is empty and parent prefixes are taken from parent layers that have `@` as
/// their first character and stripped string after `@` is treated as one parent prefix. Usage of parent prefixes makes
/// it easy to group lots of images for export without duplicating common name prefixes in every layer name.
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

/// \brief Specifies import size scale factor for Krita files with particular name suffix.
/// \details During content creation, it is usually much more convenient to use big image sizes as it interacts
///          better with how pen-based tools work during drawing. As a result, these images should be scaled down,
///          usually with 0.5 or 0.25 factor. And this is mostly fine from content creation point of view as hand-drawn
///          images are almost never pixel perfect and are expected to be scaled down a little bit.
struct kan_resource_krita_scale_rule_t
{
    /// \brief This scale rule will be applied if third party resource name ends with this string.
    /// \warning Should include ".kra" as Krita files will always end with ".kra" extension and
    ///          this extension will be a part of imported third party resource name.
    kan_interned_string_t suffix;

    /// \brief Imported images will be scaled by this factor.
    float scale_factor;
};

RESOURCE_KRITA_BUILD_API void kan_resource_krita_scale_rule_init (struct kan_resource_krita_scale_rule_t *instance);

/// \brief Contains target platform configuration for importing resource from Krita.
struct kan_resource_krita_platform_configuration_t
{
    /// \brief Default scale factor that is used when nothing in `scale_rules` was matched by suffix.
    float default_scale_factor;

    /// \brief Rules for scaling images during import.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_krita_scale_rule_t)
    struct kan_dynamic_array_t scale_rules;
};

RESOURCE_KRITA_BUILD_API void kan_resource_krita_platform_configuration_init (
    struct kan_resource_krita_platform_configuration_t *instance);

RESOURCE_KRITA_BUILD_API void kan_resource_krita_platform_configuration_shutdown (
    struct kan_resource_krita_platform_configuration_t *instance);

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
    /// \brief Scale factor value that was used during importing images from source Krita file.
    float scale_factor;

    /// \brief List of PNG images imported from Krita file.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_krita_header_entry_t)
    struct kan_dynamic_array_t entries;
};

RESOURCE_KRITA_BUILD_API void kan_resource_krita_header_init (struct kan_resource_krita_header_t *instance);

RESOURCE_KRITA_BUILD_API void kan_resource_krita_header_shutdown (struct kan_resource_krita_header_t *instance);

KAN_C_HEADER_END
