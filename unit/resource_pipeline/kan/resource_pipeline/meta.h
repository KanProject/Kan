#pragma once

#include <resource_pipeline_api.h>

#include <kan/api_common/c_header.h>
#include <kan/api_common/core_types.h>
#include <kan/container/interned_string.h>
#include <kan/hash/hash.h>
#include <kan/reflection/markup.h>
#include <kan/reflection/registry.h>
#include <kan/stream/stream.h>

/// \file
/// \brief Contains meta for declaring resource types and for setting up their build routine.
///
/// \par Build rules
/// \parblock
/// Build rules are used to produce resources of new type from resources of old type using user-defined logic. They are
/// primarily used to convert data from edition-friendly format to engine-friendly format. They can also be used to
/// split big resource into sub resources using secondary resource production, for example to separate image mips into
/// different image data resources.
///
/// Build rule always produces resource of built type from resource of primary input type and built resource always
/// has the same name as primary input resource. Resource references from primary input will be considered secondary
/// inputs and loaded if their types are listed in `kan_resource_build_rule_t::secondary_types`. Third party resource
/// references are always considered secondary inputs. Build rule can also produce secondary outputs of any resource
/// type through `kan_resource_build_rule_context_t::produce_native_secondary_output` function. Third party byproducts,
/// for example png's exported from drawing applications, can also be registered as third party secondary resources
/// through `kan_resource_build_rule_context_t::produce_third_party_secondary_output.
///
/// There can be several build rules to build one resource type or resource type that can be used as raw resource can
/// also be built by built rules -- having build rules does not mean that resource cannot be stored in raw format.
/// That is useful for cases like image import -- atlases could be constructed from plain pngs and described directly or
/// could be built from editor-specific files through special adapter resource.
///
/// Special kind of rules -- import-rules -- are used exclusively for parsing third party files as they treat third
/// party resource file as their primary input. It is used as a neat trick to avoid parsing one third party resource
/// several times, which could've happened if it was just passed as secondary input.
/// \endparblock

KAN_C_HEADER_BEGIN

/// \brief Enumerates flags that add special behaviors to resource types.
KAN_REFLECTION_FLAGS
enum kan_resource_type_flags_t
{
    /// \brief Resource should be treated as root while building.
    /// \details Root resources are scanned first and are used as roots for used resource scanning algorithm.
    KAN_RESOURCE_TYPE_ROOT = 1u << 0u,

    /// \brief Resource should be treated as streamed.
    /// \details Streamed resources should never be loaded unless directly requested unlike non-streamed resources that
    ///          are always loaded when their package is marked for loading. Streamed resources should be used to
    ///          separate large chunks of better quality data that is not required all the time, like best mips for very
    ///          heavy textures or cinematic LODs for meshes.
    KAN_RESOURCE_TYPE_STREAMED = 1u << 1u,

    /// \brief Resource should be treated as transitively loaded by resource provider.
    /// \details Transitively loaded resources are always converted to other records in application universe during
    ///          commit step and are never accessed directly except for the process of conversion. It means that
    ///          resource is only loaded for the commit phase and then unloaded after commit phase is done. This
    ///          behavior is important for the resources that cannot be consumed directly in any way, for example for
    ///          materials, textures and other stuff tied to render backend: we need to move their data to the GPU and
    ///          after that we do not want to keep it allocated. In case of hot reload, transitive resources are loaded
    ///          again into the commit phase only if they were actually changed, because if they were not changed then
    ///          using already converted records should suffice.
    KAN_RESOURCE_TYPE_TRANSITIVELY_LOADED = 1u << 2u,
};

/// \brief Defines type used for versioning resource types.
/// \details For versioning rather trivial resources that do not take ages to be built, CUSHION_START_NS_X64 is advised
///          as simple and robust approach for automatically bumping version when file that contains meta is recompiled.
///          And if that file has include with resource type structure, it will always be recompiled on structure change
///          and version will be properly updated.
///
///          For resources where it is crucial to avoid excessive rebuilds due to time constraints,
///          conservative method with version-enum is advised.
typedef kan_stable_size_t kan_resource_version_t;

/// \brief Declares signature for move function. Used for secondary production.
/// \details When NULL, `kan_reflection_move_struct` will be used instead.
typedef void (*kan_resource_type_move_functor_t) (void *target, void *source);

/// \brief Declares signature for reset function. Used for secondary production.
/// \details When NULL, `kan_reflection_reset_struct` will be used instead.
typedef void (*kan_resource_type_reset_functor_t) (void *target);

/// \brief Struct meta that marks this struct as resource type.
/// \invariant Cannot be attached several times to one struct.
struct kan_resource_type_meta_t
{
    /// \brief Flags that can alter resource usage behavior including resource build pipeline.
    enum kan_resource_type_flags_t flags;

    /// \brief Resource version is used to detect cases when resource structure has changed while
    ///        raw sources were not changed.
    kan_resource_version_t version;

    kan_resource_type_move_functor_t move;
    kan_resource_type_reset_functor_t reset;
};

/// \brief Enumerates flags that may alter how resource reference is processed.
KAN_REFLECTION_FLAGS
enum kan_resource_reference_meta_flags_t
{
    /// \brief Informs that resource can be absent on some platforms.
    /// \details This flag is needed for the cases when some resources are not needed on some platforms and therefore
    ///          could be skipped on them. It still results in build time check whether resource can be found and
    ///          built, but does not result in error if resource build returns "unsupported" compilation result.
    KAN_RESOURCE_REFERENCE_META_PLATFORM_OPTIONAL = 1u << 0u,

    /// \brief There will be no validation error if reference field contains NULL reference.
    /// \warning Also allows reference arrays to be empty. However, reference arrays in patches cannot be validated
    ///          like that as they're always partial, so we cannot easily check whether array is not empty.
    KAN_RESOURCE_REFERENCE_META_NULLABLE = 1u << 1u,

    /// \brief Informs build validation that loading referenced resource from package context is not required.
    /// \details The most important feature of packages is cascade loading, which means that when loading transaction
    ///          is done we can be sure that everything that was referenced is loaded. To ensure that for optional
    ///          packages, we need additional validation step during resource build. However, there are cases when we
    ///          do not expect resource to be loaded always -- like for cosmetics we would leave a "package enabling
    ///          tag" and reference to cosmetics asset, that should only be loaded if we actually use that. Such
    ///          references should be marked with this flag so validation will not flag them as errors.
    KAN_RESOURCE_REFERENCE_META_LOADING_NOT_REQUIRED = 1u << 2u,
};

/// \brief Struct field meta that informs that this field is either an interned string with resource name or an array
///        of interned strings with resource names that should be taken into account in resource build pipeline.
struct kan_resource_reference_meta_t
{
    /// \brief Name of a resource type to which reference points or NULL for third party resources.
    const char *type_name;

    enum kan_resource_reference_meta_flags_t flags;
};

/// \brief Enumerates result values that can be returned from build rule functor.
enum kan_resource_build_rule_result_t
{
    /// \brief Build rule was successfully executed.
    KAN_RESOURCE_BUILD_RULE_SUCCESS = 0u,

    /// \brief Build rule execution has failed.
    KAN_RESOURCE_BUILD_RULE_FAILURE,

    /// \brief Special result that is returned when given resource is not supported on current platform.
    /// \details Not an error when all references to this resource have flag KAN_RESOURCE_REFERENCE_PLATFORM_OPTIONAL,
    ///          otherwise treated as build error.
    KAN_RESOURCE_BUILD_RULE_UNSUPPORTED,
};

/// \brief Contains information about secondary input for the build rule.
struct kan_resource_build_rule_secondary_node_t
{
    /// \brief Pointer to the next secondary input if any.
    struct kan_resource_build_rule_secondary_node_t *next;

    kan_interned_string_t type;
    kan_interned_string_t name;

    union
    {
        /// \brief Loaded secondary resource data if `type` is not NULL.
        const void *data;

        /// \brief Path to third party binary data if `type` is NULL.
        const char *third_party_path;
    };
};

KAN_HANDLE_DEFINE (kan_resource_build_rule_interface_t);

/// \brief Declares signature for native secondary output production and registration.
/// \details Returns given whether production was successfully registered. `data` is allowed to point to stack as move
///          and reset functors from `kan_resource_type_meta_t` are used according to that meta docs.
typedef bool (*kan_resource_build_rule_produce_native_secondary_output_functor_t) (
    kan_resource_build_rule_interface_t interface, kan_interned_string_t type, kan_interned_string_t name, void *data);

/// \brief Declares signature for third party secondary output production and registration.
/// \details Returns given whether production was successfully registered.
///          Returns name of the resource through output variable.
typedef bool (*kan_resource_build_rule_produce_third_party_secondary_output_functor_t) (
    kan_resource_build_rule_interface_t interface, const char *path, kan_interned_string_t *name_output);

/// \brief Context that is provided to build rule execution functor.
struct kan_resource_build_rule_context_t
{
    /// \brief Name of the primary input and output resources.
    kan_interned_string_t primary_name;

    union
    {
        /// \brief Pointer to primary input resource data if rule is not an import-rule.
        const void *primary_input;

        /// \brief Path to third party resource file if rule is an import-rule.
        const char *primary_third_party_path;
    };

    /// \brief First node of secondary input list if any.
    /// \invariant When references are produced from an array, their order is preserved in this list.
    struct kan_resource_build_rule_secondary_node_t *secondary_input_first;

    /// \brief Pointer to primary output resource data.
    void *primary_output;

    /// \brief Pointer to platform configuration if build rule requested any.
    const void *platform_configuration;

    /// \brief Path to a temporary directory that could be used for temporary outputs from third party tools.
    /// \details Deleted after build execution. To preserve outputs as third party resources, produce them as secondary
    ///          resources using `produce_third_party_secondary_output`.
    const char *temporary_workspace;

    /// \brief Opaque interface data that is used to pass information to additional capability functors.
    kan_resource_build_rule_interface_t interface;

    /// \brief Functor that is used to produce native secondary outputs.
    kan_resource_build_rule_produce_native_secondary_output_functor_t produce_native_secondary_output;

    /// \brief Functor that is used to produce third party secondary outputs.
    kan_resource_build_rule_produce_third_party_secondary_output_functor_t produce_third_party_secondary_output;

    /// \brief Reflection registry that is being used by build tool.
    /// \details In some cases, import build rules need to run third party tool with separate import scripts, that will
    ///          produce some kind of an output readable data file that describes imported data. Then this file needs to
    ///          be loaded and post processed by build rule logic, and registry is needed for loading.
    kan_reflection_registry_t reflection_registry;
};

/// \brief Functor for build rule implementation logic.
typedef enum kan_resource_build_rule_result_t (*kan_resource_build_rule_functor_t) (
    struct kan_resource_build_rule_context_t *context);

/// \brief Declares a build rule for producing built resources.
/// \details Should be attached to struct with primary output resource type, where primary output resource type is a
///          resource that is always produced from this rule on successful execution.
struct kan_resource_build_rule_t
{
    /// \brief Type of the resource from which new primary output resource is being built.
    /// \details If this field is NULL, then build rule is considered import-rule: it searches for the raw third party
    ///          resource with required name and passes its path as primary input instead.
    const char *primary_input_type;

    /// \brief Name of platform configuration type for build rule or NULL if platform configuration is not used.
    const char *platform_configuration_type;

    /// \brief Count of entries in `secondary_types`.
    kan_instance_size_t secondary_types_count;

    /// \brief References in primary input resource with types from this array will be used as
    ///        secondary inputs for that build rule.
    const char **secondary_types;

    /// \brief Functor that implements build rule.
    kan_resource_build_rule_functor_t functor;

    /// \brief Build rule version for resource build routine results versioning.
    kan_resource_version_t version;
};

KAN_C_HEADER_END
