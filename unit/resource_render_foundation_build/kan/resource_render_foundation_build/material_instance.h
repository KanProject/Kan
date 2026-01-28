#pragma once

#include <resource_render_foundation_build_api.h>

#include <kan/api_common/core_types.h>
#include <kan/api_common/mute_warnings.h>
#include <kan/container/dynamic_array.h>
#include <kan/container/interned_string.h>
#include <kan/context/render_backend_system.h>
#include <kan/error/critical.h>
#include <kan/inline_math/inline_math.h>
#include <kan/reflection/markup.h>
#include <kan/render_pipeline_language/compiler.h>
#include <kan/resource_render_foundation/material_instance.h>

/// \file
/// \brief This file stores data structures for defining material instances to be built for runtime usage.
///
/// \par Overview
/// \parblock
/// To define a new material instance to be built for usage, `kan_resource_material_instance_raw_t` resource should be
/// used. It describes all the parameters for material instance: both material set parameters and instanced variants
/// with their attributes as parameters. It also supports inheritance: material instance can inherit data from other
/// material instance and apply its values on top of it. Variants are also inherited and updated by their names.
///
/// Buffers with tail parameters from render pipeline language are supported through `kan_resource_material_tail_set_t`
/// and `kan_resource_material_tail_append_t` structures that are used to set data in particular tail of  particular
/// buffer at some index or append new item to the tail.
///
/// There is also a mixin system that makes it possible to share raw parameters between material instances of different
/// materials. For example, icon render and glyph render for text blocks may have different materials, but still could
/// have similar configuration interface for material instances. Mixins would make it possible share this parameters
/// for both material instances. Keep in mind, that mixins work pretty much like includes in C: material instances just
/// grab everything from them before applying their own parameters, however parameter section application order still
/// stands, for example tail appends from mixin are applied after tail sets from material instance and all mixins.
/// Also, mixins do not support inheritance.
/// \endparblock

KAN_C_HEADER_BEGIN

#define KAN_RESOURCE_MATERIAL_PARAMETER_TYPE(NAME, TYPE)                                                               \
    struct kan_resource_material_parameter_##NAME##_t                                                                  \
    {                                                                                                                  \
        kan_interned_string_t name;                                                                                    \
        TYPE value;                                                                                                    \
    }

KAN_RESOURCE_MATERIAL_PARAMETER_TYPE (f1, float);
KAN_RESOURCE_MATERIAL_PARAMETER_TYPE (f2, struct kan_float_vector_2_t);
KAN_RESOURCE_MATERIAL_PARAMETER_TYPE (f3, struct kan_float_vector_3_t);
KAN_RESOURCE_MATERIAL_PARAMETER_TYPE (f4, struct kan_float_vector_4_t);

KAN_RESOURCE_MATERIAL_PARAMETER_TYPE (u1, uint32_t);
KAN_RESOURCE_MATERIAL_PARAMETER_TYPE (u2, struct kan_uint32_vector_2_t);
KAN_RESOURCE_MATERIAL_PARAMETER_TYPE (u3, struct kan_uint32_vector_3_t);
KAN_RESOURCE_MATERIAL_PARAMETER_TYPE (u4, struct kan_uint32_vector_4_t);

KAN_RESOURCE_MATERIAL_PARAMETER_TYPE (s1, int32_t);
KAN_RESOURCE_MATERIAL_PARAMETER_TYPE (s2, struct kan_int32_vector_2_t);
KAN_RESOURCE_MATERIAL_PARAMETER_TYPE (s3, struct kan_int32_vector_3_t);
KAN_RESOURCE_MATERIAL_PARAMETER_TYPE (s4, struct kan_int32_vector_4_t);

KAN_RESOURCE_MATERIAL_PARAMETER_TYPE (f3x3, struct kan_float_matrix_3x3_t);
KAN_RESOURCE_MATERIAL_PARAMETER_TYPE (f4x4, struct kan_float_matrix_4x4_t);

/// \brief Hex-represented web-compatible color (hex value is SRGB), that is converted to f4 vector with linear values.
KAN_RESOURCE_MATERIAL_PARAMETER_TYPE (color_linear, uint32_t);

/// \brief Hex-represented web-compatible color (hex value is SRGB), that is converted to f4 vector with srgb values.
KAN_RESOURCE_MATERIAL_PARAMETER_TYPE (color_srgb, uint32_t);

#undef KAN_RESOURCE_MATERIAL_PARAMETER_TYPE

/// \brief Container of typed parameter containers.
/// \details This approach looks weird in code, but makes it possible for the user to add parameters in much more
///          readable format, for example `+parameters.linear_color { name = my_name value = 0xFFFFFFFF }`, which makes
///          working with material instance resources from text editor much more convenient. Also, this format is only
///          used during resource build, therefore we do not need to optimize its memory layout.
struct kan_resource_material_parameter_container_t
{
#define KAN_RESOURCE_MATERIAL_PARAMETER_FIELD(NAME)                                                                    \
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_material_parameter_##NAME##_t)                              \
    struct kan_dynamic_array_t NAME

    KAN_RESOURCE_MATERIAL_PARAMETER_FIELD (f1);
    KAN_RESOURCE_MATERIAL_PARAMETER_FIELD (f2);
    KAN_RESOURCE_MATERIAL_PARAMETER_FIELD (f3);
    KAN_RESOURCE_MATERIAL_PARAMETER_FIELD (f4);

    KAN_RESOURCE_MATERIAL_PARAMETER_FIELD (u1);
    KAN_RESOURCE_MATERIAL_PARAMETER_FIELD (u2);
    KAN_RESOURCE_MATERIAL_PARAMETER_FIELD (u3);
    KAN_RESOURCE_MATERIAL_PARAMETER_FIELD (u4);

    KAN_RESOURCE_MATERIAL_PARAMETER_FIELD (s1);
    KAN_RESOURCE_MATERIAL_PARAMETER_FIELD (s2);
    KAN_RESOURCE_MATERIAL_PARAMETER_FIELD (s3);
    KAN_RESOURCE_MATERIAL_PARAMETER_FIELD (s4);

    KAN_RESOURCE_MATERIAL_PARAMETER_FIELD (f3x3);
    KAN_RESOURCE_MATERIAL_PARAMETER_FIELD (f4x4);

    KAN_RESOURCE_MATERIAL_PARAMETER_FIELD (color_linear);
    KAN_RESOURCE_MATERIAL_PARAMETER_FIELD (color_srgb);

#undef KAN_RESOURCE_MATERIAL_PARAMETER_FIELD
};

RESOURCE_RENDER_FOUNDATION_BUILD_API void kan_resource_material_parameter_container_init (
    struct kan_resource_material_parameter_container_t *instance);

RESOURCE_RENDER_FOUNDATION_BUILD_API void kan_resource_material_parameter_container_shutdown (
    struct kan_resource_material_parameter_container_t *instance);

/// \brief Data structure for setting tail item parameters at particular index.
struct kan_resource_material_tail_set_t
{
    /// \brief Corresponds to `kan_rpl_meta_buffer_t::tail_name`.
    kan_interned_string_t tail_name;

    kan_instance_size_t index;

    struct kan_resource_material_parameter_container_t parameters;
};

RESOURCE_RENDER_FOUNDATION_BUILD_API void kan_resource_material_tail_set_init (
    struct kan_resource_material_tail_set_t *instance);

RESOURCE_RENDER_FOUNDATION_BUILD_API void kan_resource_material_tail_set_shutdown (
    struct kan_resource_material_tail_set_t *instance);

/// \brief Data structure for appending new tail item with given parameters.
struct kan_resource_material_tail_append_t
{
    /// \brief Corresponds to `kan_rpl_meta_buffer_t::tail_name`.
    kan_interned_string_t tail_name;

    struct kan_resource_material_parameter_container_t parameters;
};

RESOURCE_RENDER_FOUNDATION_BUILD_API void kan_resource_material_tail_append_init (
    struct kan_resource_material_tail_append_t *instance);

RESOURCE_RENDER_FOUNDATION_BUILD_API void kan_resource_material_tail_append_shutdown (
    struct kan_resource_material_tail_append_t *instance);

/// \brief Data structure for configuring sampler binding in material.
struct kan_resource_material_sampler_t
{
    /// \brief Corresponds to `kan_rpl_meta_sampler_t::name`.
    kan_interned_string_t name;

    /// \brief Sampling configuration.
    struct kan_render_sampler_t sampler;
};

RESOURCE_RENDER_FOUNDATION_BUILD_API void kan_resource_material_sampler_init (
    struct kan_resource_material_sampler_t *instance);

/// \brief Data structure for configuring texture-to-image binding in material.
struct kan_resource_material_image_t
{
    /// \brief Corresponds to `kan_rpl_meta_sampler_t::name`.
    kan_interned_string_t name;

    /// \brief Name of the texture resource.
    kan_interned_string_t texture;
};

RESOURCE_RENDER_FOUNDATION_BUILD_API void kan_resource_material_image_init (
    struct kan_resource_material_image_t *instance);

/// \brief Data structure for configuring named material instance variant with instanced data.
struct kan_resource_material_variant_raw_t
{
    kan_interned_string_t name;
    struct kan_resource_material_parameter_container_t parameters;
};

RESOURCE_RENDER_FOUNDATION_BUILD_API void kan_resource_material_variant_raw_init (
    struct kan_resource_material_variant_raw_t *instance);

RESOURCE_RENDER_FOUNDATION_BUILD_API void kan_resource_material_variant_raw_shutdown (
    struct kan_resource_material_variant_raw_t *instance);

/// \brief Describes material instance resource.
struct kan_resource_material_instance_raw_t
{
    /// \brief Material resource name. If has parent, should be equal to parent material name.
    kan_interned_string_t material;

    /// \brief Name of the parent material instance if any.
    kan_interned_string_t parent;

    /// \brief Mixins to be applied on top of a parent data, but before other material instance parameters.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (kan_interned_string_t)
    struct kan_dynamic_array_t use_mixins;

    /// \brief Container for parameters for material set buffers.
    struct kan_resource_material_parameter_container_t parameters;

    /// \brief Array of tail item sets (parameter write at particular index).
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_material_tail_set_t)
    struct kan_dynamic_array_t tail_set;

    /// \brief Array of tail item appends.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_material_tail_append_t)
    struct kan_dynamic_array_t tail_append;

    /// \brief Array of sampler configurations.
    /// \warning Sampler configuration for particular name fully overrides parent sampler configuration.
    ///          Therefore, material must specify full sampler configuration, not only differences from parent sampler.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_material_sampler_t)
    struct kan_dynamic_array_t samplers;

    /// \brief Array of texture selections for image slots.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_material_image_t)
    struct kan_dynamic_array_t images;

    /// \brief Instanced data variants for this material instance if any.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_material_variant_raw_t)
    struct kan_dynamic_array_t variants;
};

RESOURCE_RENDER_FOUNDATION_BUILD_API void kan_resource_material_instance_raw_init (
    struct kan_resource_material_instance_raw_t *instance);

RESOURCE_RENDER_FOUNDATION_BUILD_API void kan_resource_material_instance_raw_shutdown (
    struct kan_resource_material_instance_raw_t *instance);

/// \brief Mixins make it possible to share parameter sets between material instance of different materials.
/// \details Sometimes different materials have similar interfaces and parameters for their interfaces should be
///          identical for several instances. In that case, mixin should be used to avoid duplication.
struct kan_resource_material_instance_mixin_t
{
    /// \brief Container for parameters for material set buffers.
    struct kan_resource_material_parameter_container_t parameters;

    /// \brief Array of tail item sets (parameter write at particular index).
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_material_tail_set_t)
    struct kan_dynamic_array_t tail_set;

    /// \brief Array of tail item appends.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_material_tail_append_t)
    struct kan_dynamic_array_t tail_append;

    /// \brief Array of sampler configurations.
    /// \warning Sampler configuration for particular name fully overrides parent sampler configuration.
    ///          Therefore, material must specify full sampler configuration, not only differences from parent sampler.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_material_sampler_t)
    struct kan_dynamic_array_t samplers;

    /// \brief Array of texture selections for image slots.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_material_image_t)
    struct kan_dynamic_array_t images;

    /// \brief Instanced data variants for this material instance if any.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_resource_material_variant_raw_t)
    struct kan_dynamic_array_t variants;
};

RESOURCE_RENDER_FOUNDATION_BUILD_API void kan_resource_material_instance_mixin_init (
    struct kan_resource_material_instance_mixin_t *instance);

RESOURCE_RENDER_FOUNDATION_BUILD_API void kan_resource_material_instance_mixin_shutdown (
    struct kan_resource_material_instance_mixin_t *instance);

KAN_C_HEADER_END
