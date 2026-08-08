#pragma once

#include <universe_render_foundation_api.h>

#include <kan/api_common/c_header.h>
#include <kan/api_common/core_types.h>
#include <kan/context/render_backend_system.h>
#include <kan/render_pipeline_language/compiler.h>
#include <kan/threading/atomic.h>
#include <kan/universe/universe.h>

/// \file
/// \brief Provides API for render foundation render pass, material and material instance resource management.
///
/// \par Definition
/// \parblock
/// This file unifies API for interacting with render passes, materials and material instances and their management
/// in runtime. These resources are tightly coupled on render implementation level and therefore need to be managed
/// by unified system to avoid unnecessary complexities. It was decided to call that unified routine
/// "program management" as combination of passes, materials and material instance defines something that can be called
/// render programs and their data.
/// \endparblock
///
/// \par Render passes
/// \parblock
/// Render passes loading and unloading is done automatically through resource package transaction routine.
/// \endparblock
///
/// \par Materials
/// \parblock
/// Materials loading and unloading is done automatically through resource package transaction routine. Also, material
/// pipeline compilation locks transaction commit until all GPU pipelines are compiled, therefore user can be sure
/// that pipelines are ready when transaction is finished.
/// \endparblock
///
/// \par Material instances
/// \parblock
/// Material instance loading and unloading is also done automatically through resource package transaction routine.
/// There are also `kan_render_material_instance_quality_t` records that function the same way as
/// `kan_render_texture_quality_t` and are automatically converted to texture qualities for all textures bound to the
/// material instance.
/// \endparblock

KAN_C_HEADER_BEGIN

/// \brief Group that is used to add all render foundation program management mutators.
#define KAN_RENDER_FOUNDATION_PROGRAM_MANAGEMENT_MUTATOR_GROUP "render_foundation_program_management"

/// \brief Checkpoint, after which render foundation program management mutators are executed.
#define KAN_RENDER_FOUNDATION_PROGRAM_MANAGEMENT_BEGIN_CHECKPOINT "render_foundation_program_management_begin"

/// \brief Checkpoint, that is hit after all render foundation program management mutators have finished execution.
#define KAN_RENDER_FOUNDATION_PROGRAM_MANAGEMENT_END_CHECKPOINT "render_foundation_program_management_end"

KAN_TYPED_ID_32_DEFINE (kan_render_material_instance_quality_id_t);

/// \brief Singleton for publicly accessible data related to program management.
struct kan_render_program_singleton_t
{
    /// \brief Internal counter for generating material quality ids.
    struct kan_atomic_int_t material_instance_quality_id_counter;
};

UNIVERSE_RENDER_FOUNDATION_API void kan_render_program_singleton_init (struct kan_render_program_singleton_t *instance);

/// \brief Inline helper for generation of material quality ids.
static inline kan_render_material_instance_quality_id_t kan_next_material_instance_quality_id (
    const struct kan_render_program_singleton_t *singleton)
{
    // Intentionally request const and de-const it to show that it is multithreading-safe function.
    return KAN_TYPED_ID_32_SET (kan_render_material_instance_quality_id_t,
                                (kan_id_32_t) kan_atomic_int_add (
                                    (struct kan_atomic_int_t *) &singleton->material_instance_quality_id_counter, 1));
}

/// \brief Contains layout and binding information about single variant for pipelines inside render pass.
struct kan_render_foundation_pass_variant_t
{
    kan_interned_string_t name;

    /// \details Can be invalid handle when pipeline has empty parameter set layout.
    kan_render_pipeline_parameter_set_layout_t pass_parameter_set_layout;

    /// \brief Bindings meta for pass pipeline parameter set.
    struct kan_rpl_meta_set_bindings_t pass_parameter_set_bindings;
};

UNIVERSE_RENDER_FOUNDATION_API void kan_render_foundation_pass_variant_init (
    struct kan_render_foundation_pass_variant_t *instance);

UNIVERSE_RENDER_FOUNDATION_API void kan_render_foundation_pass_variant_shutdown (
    struct kan_render_foundation_pass_variant_t *instance);

/// \brief Stores information about pass attachment that could be useful for outer users.
struct kan_render_foundation_pass_attachment_t
{
    enum kan_render_pass_attachment_type_t type;
    enum kan_render_image_format_t format;
};

/// \brief Represents loaded and successfully created render pass.
struct kan_render_foundation_pass_loaded_t
{
    kan_immutable kan_interned_string_t name;
    enum kan_render_pass_type_t type;
    kan_render_pass_t pass;

    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_render_foundation_pass_attachment_t)
    struct kan_dynamic_array_t attachments;

    /// \brief Information about layout and binding for pass pipeline variants in the same order as in resource.
    /// \details Will be empty for passes that do not have any pass customization (no special set layout).
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_render_foundation_pass_variant_t)
    struct kan_dynamic_array_t variants;
};

UNIVERSE_RENDER_FOUNDATION_API void kan_render_foundation_pass_loaded_init (
    struct kan_render_foundation_pass_loaded_t *instance);

UNIVERSE_RENDER_FOUNDATION_API void kan_render_foundation_pass_loaded_shutdown (
    struct kan_render_foundation_pass_loaded_t *instance);

/// \brief Event that is being sent when `kan_render_foundation_pass_t` is updated.
struct kan_render_foundation_pass_updated_event_t
{
    kan_interned_string_t name;
};

/// \brief Stores information about loaded and instanced pipeline for particular pass.
struct kan_render_material_pipeline_t
{
    /// \brief Name of the pass for which pipeline was created.
    kan_interned_string_t pass_name;

    /// \brief Index of this pipeline in pass variants.
    kan_interned_string_t variant_name;

    /// \brief Handle to the actual pipeline. Might still be in compilation stage.
    kan_render_graphics_pipeline_t pipeline;
};

UNIVERSE_RENDER_FOUNDATION_API void kan_render_material_pipeline_init (struct kan_render_material_pipeline_t *instance);

UNIVERSE_RENDER_FOUNDATION_API void kan_render_material_pipeline_shutdown (
    struct kan_render_material_pipeline_t *instance);

///\brief Contains material loaded data: its parameter set layouts, pipelines and family meta.
struct kan_render_material_loaded_t
{
    /// \brief Material resource name.
    kan_immutable kan_interned_string_t name;

    /// \brief Array with currently instanced pipelines for existing passes.
    /// \details It is advised to cache pipeline handles in separate render cache record in the order that is
    ///          convenient for the render implementation.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_render_material_pipeline_t)
    struct kan_dynamic_array_t pipelines;

    /// \brief Layout for material set of parameters for pipeline if any.
    kan_render_pipeline_parameter_set_layout_t set_material;

    /// \brief Layout for object set of parameters for pipeline if any.
    kan_render_pipeline_parameter_set_layout_t set_object;

    /// \brief Layout for shared set of parameters for pipeline if any.
    kan_render_pipeline_parameter_set_layout_t set_shared;

    /// \brief Information about vertex attribute sources used by this material.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_rpl_meta_attribute_source_t)
    struct kan_dynamic_array_t vertex_attribute_sources;

    /// \brief Size of push constant for pipelines of this material or zero if push constants are not used.
    kan_instance_size_t push_constant_size;

    /// \brief Whether this material has instanced attribute source.
    bool has_instanced_attribute_source;

    /// \brief Information about instanced attribute source for this material if it exists.
    KAN_REFLECTION_VISIBILITY_CONDITION_FIELD (has_instanced_attribute_source)
    KAN_REFLECTION_VISIBILITY_CONDITION_VALUE (true)
    struct kan_rpl_meta_attribute_source_t instanced_attribute_source;

    /// \brief Information about bindings for material set of this material.
    struct kan_rpl_meta_set_bindings_t set_material_bindings;

    /// \brief Information about bindings for object set of this material.
    struct kan_rpl_meta_set_bindings_t set_object_bindings;

    /// \brief Information about bindings for unstable set of this material.
    struct kan_rpl_meta_set_bindings_t set_shared_bindings;
};

UNIVERSE_RENDER_FOUNDATION_API void kan_render_material_loaded_init (struct kan_render_material_loaded_t *instance);

UNIVERSE_RENDER_FOUNDATION_API void kan_render_material_loaded_shutdown (struct kan_render_material_loaded_t *instance);

/// \brief Sent when loaded material is inserted or updated, including pipeline update due to pass-related operations.
struct kan_render_material_updated_event_t
{
    kan_interned_string_t name;
};

/// \brief Sent when loaded material instance is inserted or updated,
///        including pipeline update due to pass-related operations.
struct kan_render_material_instance_updated_event_t
{
    kan_interned_string_t name;
};

/// \brief Used to inform program management that material instance needs to be loaded.
struct kan_render_material_instance_quality_t
{
    /// \brief This quality unique id, must be generated from `kan_next_material_instance_quality_id`.
    kan_immutable kan_render_material_instance_quality_id_t quality_id;

    /// \brief Name of the material instance resource.
    kan_immutable kan_interned_string_t name;

    /// \brief Advised mip value to be loaded for bound textures.
    kan_immutable kan_instance_size_t best_advised_mip;
};

UNIVERSE_RENDER_FOUNDATION_API void kan_render_material_instance_quality_init (
    struct kan_render_material_instance_quality_t *instance);

/// \brief Describes loaded material instance variant.
struct kan_render_material_instance_variant_t
{
    kan_interned_string_t name;

    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (uint8_t)
    struct kan_dynamic_array_t instanced_data;
};

UNIVERSE_RENDER_FOUNDATION_API void kan_render_material_instance_variant_init (
    struct kan_render_material_instance_variant_t *instance);

UNIVERSE_RENDER_FOUNDATION_API void kan_render_material_instance_variant_shutdown (
    struct kan_render_material_instance_variant_t *instance);

/// \brief Contains buffer used by material instance, mostly needed for internal purposes.
struct kan_render_material_instance_bound_buffer_t
{
    kan_instance_size_t binding;
    kan_render_buffer_t buffer;
};

UNIVERSE_RENDER_FOUNDATION_API void kan_render_material_instance_bound_buffer_init (
    struct kan_render_material_instance_bound_buffer_t *instance);

UNIVERSE_RENDER_FOUNDATION_API void kan_render_material_instance_bound_buffer_shutdown (
    struct kan_render_material_instance_bound_buffer_t *instance);

///\brief Contains material instance loaded data: its material parameter set and variants.
struct kan_render_material_instance_loaded_t
{
    kan_immutable kan_interned_string_t name;
    kan_interned_string_t material_name;

    /// \brief Built parameter set for material set with data from this material instance.
    kan_render_pipeline_parameter_set_t parameter_set;

    kan_instance_size_t requested_best_mip;
    kan_instance_size_t requested_best_mip_frame_id;

    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_render_material_instance_variant_t)
    struct kan_dynamic_array_t variants;

    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_render_material_instance_bound_buffer_t)
    struct kan_dynamic_array_t bound_buffers;
};

UNIVERSE_RENDER_FOUNDATION_API void kan_render_material_instance_loaded_init (
    struct kan_render_material_instance_loaded_t *instance);

UNIVERSE_RENDER_FOUNDATION_API void kan_render_material_instance_loaded_shutdown (
    struct kan_render_material_instance_loaded_t *instance);

KAN_C_HEADER_END
