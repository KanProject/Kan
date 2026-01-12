#pragma once

#include <universe_ui_api.h>

#include <kan/api_common/c_header.h>
#include <kan/api_common/core_types.h>
#include <kan/inline_math/inline_math.h>
#include <kan/resource_ui/bundle.h>
#include <kan/threading/atomic.h>
#include <kan/universe/universe.h>
#include <kan/universe_text/text.h>

/// \file
/// \brief Provides API for core part of UI implementation: hierarchy, layout, render.
///
/// \par Bundle
/// \parblock
/// Common configuration for rendering UI is provided through bundles: list of resources that reference required
/// resources, including proper render pass, atlas and material instances. Bundle information is available through
/// `kan_ui_bundle_singleton_t` and bundle selection can be also changed there. It is also possible to supply
/// default bundle name through `kan_ui_configuration_t`.
/// \endparblock
///
/// \par Coordinate units
/// \parblock
/// In ideal world, user should not change UI elements when resize happens: only pass proper viewport size. To make it
/// possible, we use different coordinate units that are passed to UI elements instead of just pixels:
///
/// - Points are converted to pixels through multiplication by UI scale. It can be useful if game allows user to scale
///   UI by some factor for better visibility on some displays.
/// - Pixels are just raw direct pixels.
/// - Viewport height ratio is converted to pixels through multiplication by viewport height.
/// - Viewport width ratio is converted to pixels through multiplication by viewport width.
///
/// `KAN_UI_VALUE_*` and `KAN_UI_RECT_*` macros are a syntax sugar for writing coordinate values.
/// \endparblock
///
/// \par Node hierarchy
/// \parblock
/// UI is build from hierarchical nodes that are used to define how element layout should behave. Nodes have various
/// properties that tell the system how to lay them out and how to lay their children. Nodes without parents are treated
/// as children of the viewport root and viewport root always lays out their children according to
/// `KAN_UI_LAYOUT_FRAME`. Most of the node fields are observed for changes, therefore editing `kan_ui_node_t` will
/// automatically trigger appropriate hierarchy sub-tree recalculation. However, it means that opening node records for
/// update or write is quite costly and should be avoided unless necessary.
/// \endparblock
///
/// \par Drawable
/// \parblock
/// `kan_ui_node_drawable_t` nodes have two purposes: to store draw commands for rendering ui node and to store laid out
/// global coordinates of the ui node. This coupling is made because render needs to access laid out coordinates anyway
/// and we do not expect a lot of render-less nodes. If UI node should be rendered, it is advised to manually insert
/// `kan_ui_node_drawable_t` in the same time as `kan_ui_node_t` is inserted and fill draw commands, so the element
/// could be rendered right away. If `kan_ui_node_drawable_t` was not added, it will be added automatically anyway
/// during laying out, but will not append any render commands as they'll be empty.
///
/// For the text rendering, some additional effects are implemented for default pipeline through shaped text marks:
/// palette selection of up to 255 palette entries using `KAN_UI_DEFAULT_TEXT_MARK_PALETTE_MASK` and also SDF outline,
/// cos color animation, reading animation and color table multiplication through `kan_ui_default_text_mark_flag_t`.
/// \endparblock
///
/// \par Layout
/// \parblock
/// Layout is automatically recalculated when nodes are inserted, deleted or modified. Recalculation happens between
/// `KAN_UI_LAYOUT_BEGIN_CHECKPOINT` and `KAN_UI_LAYOUT_END_CHECKPOINT` checkpoints. Algorithm tries to recalculate the
/// smallest possible amount of nodes affected by change, avoiding full scale layout recalculation if possible. Also,
/// changing data under `kan_ui_node_render_setup_t` is much cheaper than other fields as it only updates render data
/// and does not trigger regular layout calculations. But changing `local_element_order` or inserting or deleting nodes
/// has additional cost of recalculating drawable render order.
/// \endparblock
///
/// \par Render
/// \parblock
/// Render execution is split to 2 stages: render graph allocation and render execution. Render graph allocation is
/// separated as in most cases user should allocate their own render graph nodes and make UI node dependant on these
/// nodes. For example, UI contains one or more "game viewport" elements, that are rendered as custom draw commands.
/// User should implement game render as separate node allocations that are dependencies of the UI render and properly
/// update bindings to ensure that allocated images are bound to the right sets.
///
/// That means that UI+user rendering is expected to look like that (in order):
/// 1. UI allocates itself on render graph.
/// 2. User code allocates itself on render graph and makes UI node dependant on its nodes.
/// 3. Both UI and user render command generation mutators run in parallel.
///
/// UI allocation is done between `KAN_UI_RENDER_GRAPH_BEGIN_CHECKPOINT` and `KAN_UI_RENDER_GRAPH_END_CHECKPOINT`
/// checkpoints, while actual UI render command generation is done between `KAN_UI_RENDER_BEGIN_CHECKPOINT` and
/// `KAN_UI_RENDER_END_CHECKPOINT` checkpoints.
/// \endparblock

KAN_C_HEADER_BEGIN

/// \brief Group that is used to add all ui core mutators: both layout and render ones.
#define KAN_UI_CORE_MUTATOR_GROUP "ui_core"

/// \brief Checkpoint, after which ui ui time update mutators are executed.
#define KAN_UI_TIME_BEGIN_CHECKPOINT "ui_time_begin"

/// \brief Checkpoint, that is hit after all ui time update mutators have finished execution.
#define KAN_UI_TIME_END_CHECKPOINT "ui_time_end"

/// \brief Checkpoint, after which ui layout calculation mutators are executed.
#define KAN_UI_LAYOUT_BEGIN_CHECKPOINT "ui_layout_begin"

/// \brief Checkpoint, that is hit after all ui layout calculation mutators have finished execution.
#define KAN_UI_LAYOUT_END_CHECKPOINT "ui_layout_end"

/// \brief Checkpoint, after which ui bundle management mutators are executed.
#define KAN_UI_BUNDLE_MANAGEMENT_BEGIN_CHECKPOINT "ui_bundle_management_begin"

/// \brief Checkpoint, that is hit after all ui bundle management mutators have finished execution.
#define KAN_UI_BUNDLE_MANAGEMENT_END_CHECKPOINT "ui_bundle_management_end"

/// \brief Checkpoint, after which ui is allowed to do render graph allocations for itself.
/// \details Anything that affects allocations, like viewport size, must be edited prior to this checkpoint.
#define KAN_UI_RENDER_GRAPH_BEGIN_CHECKPOINT "ui_render_graph_begin"

/// \brief Checkpoint, after which ui render graph allocations for this frame are done.
#define KAN_UI_RENDER_GRAPH_END_CHECKPOINT "ui_render_graph_end"

/// \brief Checkpoint, after which ui render mutators are executed.
#define KAN_UI_RENDER_BEGIN_CHECKPOINT "ui_render_begin"

/// \brief Checkpoint, that is hit after all ui render mutators have finished execution.
#define KAN_UI_RENDER_END_CHECKPOINT "ui_render_end"

KAN_TYPED_ID_32_DEFINE (kan_ui_node_id_t);

/// \brief Used to generate node ids or edit global ui scale and viewport size for this world and its children if any.
/// \details Should not be accessed outside of left worlds. UI is designed to only be present in leaf worlds.
struct kan_ui_singleton_t
{
    /// \brief Atomic counter for assigning ui node ids. Safe to be modified from different threads.
    struct kan_atomic_int_t node_id_counter;

    /// \brief Affects point-based UI coordinates..
    float scale;

    kan_instance_offset_t viewport_width;
    kan_instance_offset_t viewport_height;

    /// \brief Global time for UI gpu-based animations.
    float animation_global_time_s;

    /// \brief Delta time for UI animations and for delta time bound input.
    float animation_delta_time_s;

    /// \brief Global time for UI animations loops back to zero when it becomes higher than this value.
    float animation_global_time_loop_s;

    /// \brief Use to calculate deltas for `animation_global_time_s`.
    kan_time_size_t last_time_ns;
};

UNIVERSE_UI_API void kan_ui_singleton_init (struct kan_ui_singleton_t *instance);

/// \brief Inline helper for generation of  ui node ids.
static inline kan_ui_node_id_t kan_next_ui_node_id (const struct kan_ui_singleton_t *ui)
{
    // Intentionally uses const and de-const it to show that it is multithreading-safe function.
    return KAN_TYPED_ID_32_SET (kan_ui_node_id_t,
                                (kan_id_32_t) kan_atomic_int_add ((struct kan_atomic_int_t *) &ui->node_id_counter, 1));
}

/// \brief Name for ui configuration object in universe world.
#define KAN_UI_CONFIGURATION "ui"

/// \brief Structure that contains configuration for the ui in this world.
struct kan_ui_configuration_t
{
    kan_interned_string_t default_bundle_name;
};

/// \brief Singleton that contains loaded ui bundle data for this world and its children.
/// \details Should not be accessed in root world as it would place this singleton in root and force single bundle
///          everywhere, however should be used in as-high-as-possible game worlds and editor worlds in order to avoid
///          unnecessary configuration duplication. Creation in root worlds is considered to be bad as editor may use
///          its separate ui bundle.
struct kan_ui_bundle_singleton_t
{
    /// \brief Bundle name to be selected and loaded for this world.
    /// \details Default value is supplied through `kan_resource_provider_configuration_t` if this field is NULL.
    kan_interned_string_t bundle_name;

    /// \brief If user manually changes `bundle_name`, user should make this field `true`.
    bool selection_dirty;

    /// \brief True if bundle data is loaded and also resources bundle depends on are also loaded.
    bool available;

    /// \brief Loaded bundle data copy if `available`.
    struct kan_resource_ui_bundle_t available_bundle;
};

UNIVERSE_UI_API void kan_ui_bundle_singleton_init (struct kan_ui_bundle_singleton_t *instance);

UNIVERSE_UI_API void kan_ui_bundle_singleton_shutdown (struct kan_ui_bundle_singleton_t *instance);

/// \brief Event that is being sent when `kan_ui_bundle_singleton_t` available data is updated.
struct kan_ui_bundle_updated_t
{
    kan_instance_size_t stub;
};

/// \brief Enumerates supported types of coordinate units.
enum kan_ui_coordinate_type_t
{
    /// \brief Points are just like pixels, but can be scaled by global UI scale.
    KAN_UI_PT = 0u,

    /// \brief Plain old pixels.
    KAN_UI_PX,

    /// \brief Viewport height ratio, for example 0.5 = half of the viewport height.
    KAN_UI_VH,

    /// \brief Viewport width ration, for example 0.5 = half of viewport width.
    KAN_UI_VW,
};

/// \brief Represents one value of UI coordinate.
struct kan_ui_coordinate_t
{
    enum kan_ui_coordinate_type_t type;
    float value;
};

#define KAN_UI_VALUE_BUILD(TYPE, VALUE) ((struct kan_ui_coordinate_t) {.type = TYPE, .value = (VALUE)})
#define KAN_UI_VALUE_PT(VALUE) KAN_UI_VALUE_BUILD (KAN_UI_PT, VALUE)
#define KAN_UI_VALUE_PX(VALUE) KAN_UI_VALUE_BUILD (KAN_UI_PX, VALUE)
#define KAN_UI_VALUE_VH(VALUE) KAN_UI_VALUE_BUILD (KAN_UI_VH, VALUE)
#define KAN_UI_VALUE_VW(VALUE) KAN_UI_VALUE_BUILD (KAN_UI_VW, VALUE)

static inline float kan_ui_calculate_coordinate_floating (const struct kan_ui_singleton_t *ui,
                                                          struct kan_ui_coordinate_t coordinate)
{
    float floating_value = 0.0f;
    switch (coordinate.type)
    {
    case KAN_UI_PT:
        floating_value = coordinate.value * ui->scale;
        break;

    case KAN_UI_PX:
        floating_value = coordinate.value;
        break;

    case KAN_UI_VH:
        floating_value = coordinate.value * (float) ui->viewport_height;
        break;

    case KAN_UI_VW:
        floating_value = coordinate.value * (float) ui->viewport_width;
        break;
    }

    return floating_value;
}

static inline kan_instance_offset_t kan_ui_calculate_coordinate (const struct kan_ui_singleton_t *ui,
                                                                 struct kan_ui_coordinate_t coordinate)
{
    return (kan_instance_offset_t) roundf (kan_ui_calculate_coordinate_floating (ui, coordinate));
}

static inline struct kan_ui_coordinate_t kan_ui_coordinate_from_pixels (const struct kan_ui_singleton_t *ui,
                                                                        enum kan_ui_coordinate_type_t type,
                                                                        float pixels)
{
    struct kan_ui_coordinate_t result = {
        .type = type,
        .value = 0.0f,
    };

    switch (type)
    {
    case KAN_UI_PT:
        result.value = pixels / ui->scale;
        break;

    case KAN_UI_PX:
        result.value = pixels;
        break;

    case KAN_UI_VH:
        result.value = pixels / (float) ui->viewport_height;
        break;

    case KAN_UI_VW:
        result.value = pixels / (float) ui->viewport_width;
        break;
    }

    return result;
}

/// \brief Represents rect that consists of UI coordinates.
struct kan_ui_rect_t
{
    struct kan_ui_coordinate_t left;
    struct kan_ui_coordinate_t right;
    struct kan_ui_coordinate_t top;
    struct kan_ui_coordinate_t bottom;
};

#define KAN_UI_RECT_BUILD(SET, LEFT, RIGHT, TOP, BOTTOM)                                                               \
    ((struct kan_ui_rect_t) {                                                                                          \
        .left = SET (LEFT),                                                                                            \
        .right = SET (RIGHT),                                                                                          \
        .top = SET (TOP),                                                                                              \
        .bottom = SET (BOTTOM),                                                                                        \
    })

#define KAN_UI_RECT_PT(LEFT, RIGHT, TOP, BOTTOM) KAN_UI_RECT_BUILD (KAN_UI_VALUE_PT, LEFT, RIGHT, TOP, BOTTOM)
#define KAN_UI_RECT_PX(LEFT, RIGHT, TOP, BOTTOM) KAN_UI_RECT_BUILD (KAN_UI_VALUE_PX, LEFT, RIGHT, TOP, BOTTOM)
#define KAN_UI_RECT_VH(LEFT, RIGHT, TOP, BOTTOM) KAN_UI_RECT_BUILD (KAN_UI_VALUE_VH, LEFT, RIGHT, TOP, BOTTOM)
#define KAN_UI_RECT_VW(LEFT, RIGHT, TOP, BOTTOM) KAN_UI_RECT_BUILD (KAN_UI_VALUE_VW, LEFT, RIGHT, TOP, BOTTOM)

/// \brief Flags that alter the behavior of the UI node size calculation.
KAN_REFLECTION_FLAGS
enum kan_ui_size_flags_t
{
    /// \brief When no flags are set, size is treated as fixed.
    KAN_UI_SIZE_FLAG_NONE = 0u,

    /// \brief Size will be treated as min size, layout children may increase size along this axis.
    /// \warning Ignored for frame layouts.
    KAN_UI_SIZE_FLAG_FIT_CHILDREN = 1u << 0u,

    /// \brief If parent layout has remaining space along this axis, this node will attempt to grow along this axis.
    /// \details If several nodes that are children of the same parent are trying to grow, the smallest ones will grow
    ///          first, ideally making all grow nodes same-sized if there is enough remaining space.
    KAN_UI_SIZE_FLAG_GROW = 1u << 1u,
};

/// \brief Specifies how node is aligned horizontally unless its parent layout orders children along X axis.
enum kan_ui_horizontal_alignment_t
{
    KAN_UI_HORIZONTAL_ALIGNMENT_LEFT = 0u,
    KAN_UI_HORIZONTAL_ALIGNMENT_CENTER,
    KAN_UI_HORIZONTAL_ALIGNMENT_RIGHT,
};

/// \brief Specifies how node is aligned vertically unless its parent layout orders children along Y axis.
enum kan_ui_vertical_alignment_t
{
    KAN_UI_VERTICAL_ALIGNMENT_TOP = 0u,
    KAN_UI_VERTICAL_ALIGNMENT_CENTER,
    KAN_UI_VERTICAL_ALIGNMENT_BOTTOM,
};

/// \brief Contains element-scope configuration for the UI node.
struct kan_ui_node_element_setup_t
{
    enum kan_ui_size_flags_t width_flags;
    enum kan_ui_size_flags_t height_flags;

    struct kan_ui_coordinate_t width;
    struct kan_ui_coordinate_t height;

    /// \brief Margins specify required amount of free space around the element.
    /// \details Margins will be collapsed in a way that preserves free space requirement between elements,
    ///          but minimizes the volume of free space.
    struct kan_ui_rect_t margin;

    /// \brief Element horizontal alignment if supported by the parent.
    /// \warning Ignored in horizontal containers.
    enum kan_ui_horizontal_alignment_t horizontal_alignment;

    /// \brief Element vertical alignment if supported by the parent.
    /// \warning Ignored in vertical containers.
    enum kan_ui_vertical_alignment_t vertical_alignment;

    /// \brief If child of `KAN_UI_LAYOUT_FRAME` layout, offsets coordinate on X axis by this value.
    struct kan_ui_coordinate_t frame_offset_x;

    /// \brief If child of `KAN_UI_LAYOUT_FRAME` layout, offsets coordinate on Y axis by this value.
    struct kan_ui_coordinate_t frame_offset_y;
};

/// \brief Enumerates supported children layouts.
enum kan_ui_layout_t
{
    /// \brief Children order only affects render, elements are positioned using alignments, margins are ignored.
    KAN_UI_LAYOUT_FRAME = 0u,

    /// \brief Children are placed one after another in a vertical list.
    KAN_UI_LAYOUT_VERTICAL_CONTAINER,

    /// \brief Children are placed one after another in a horizontal list.
    KAN_UI_LAYOUT_HORIZONTAL_CONTAINER,
};

/// \brief Contains children-layout-scope configuration for the UI node.
struct kan_ui_node_layout_setup_t
{
    /// \brief Layout for correctly processing children nodes.
    enum kan_ui_layout_t layout;

    /// \brief Minimal distance from this element border to any child border on any layout.
    /// \details Frame offsets from `frame_offset_x` and `frame_offset_y` are applied on top and ignore padding.
    struct kan_ui_rect_t padding;
};

/// \brief Contains render-scope configuration for the UI node.
struct kan_ui_node_render_setup_t
{
    /// \brief Makes it possible to offset child elements by X to simulate scroll.
    struct kan_ui_coordinate_t scroll_x;

    /// \brief Makes it possible to offset child elements by Y to simulate scroll.
    struct kan_ui_coordinate_t scroll_y;

    /// \brief If true, use self bounds to clip self and all children.
    bool clip;
};

/// \brief Node is a building block of UI elements hierarchy and used to define anything that is added to the ui.
struct kan_ui_node_t
{
    kan_immutable kan_ui_node_id_t id;
    kan_immutable kan_ui_node_id_t parent_id;

    /// \brief If true, `kan_ui_node_laid_out_t` will be sent every time this node is laid out due to dirty status.
    bool event_on_laid_out;

    struct kan_ui_node_element_setup_t element;

    /// \brief Local integer value used to sort child elements.
    /// \details For frame layout children or for root nodes affects only draw order.
    ///          For container layout children, affects positions on container layout.
    kan_instance_offset_t local_element_order;

    struct kan_ui_node_layout_setup_t layout;
    struct kan_ui_node_render_setup_t render;
};

UNIVERSE_UI_API void kan_ui_node_init (struct kan_ui_node_t *instance);

/// \brief Event that is sent every time when layout calculations are done for the node with
///        `kan_ui_node_t::event_on_laid_out` flag;
struct kan_ui_node_laid_out_t
{
    kan_ui_node_id_t node_id;
};

/// \brief Contains calculated clip rect for clipping UI nodes.
struct kan_ui_clip_rect_t
{
    kan_instance_offset_t x;
    kan_instance_offset_t y;
    kan_instance_offset_t width;
    kan_instance_offset_t height;
};

/// \brief Enumerates draw command types supported by UI renderer.
enum kan_ui_draw_command_t
{
    /// \brief For disabled commands.
    KAN_UI_DRAW_COMMAND_NONE = 0u,

    /// \brief Image from UI atlas. Receives rect coordinates from instanced data. Instanced when possible.
    KAN_UI_DRAW_COMMAND_IMAGE,

    /// \brief Shaped text from universe_text unit.
    KAN_UI_DRAW_COMMAND_TEXT,

    /// \brief Element rect will be drawn with given material instance and object parameter set.
    KAN_UI_DRAW_COMMAND_CUSTOM,
};

/// \brief Contains data for UI image draw command.
struct kan_ui_draw_command_image_t
{
    /// \brief Image entry index in UI atlas.
    uint32_t record_index;

    /// \brief True if image can be overridden by control logic, for example focusable visuals.
    /// \details It is advised to leave it at `false` unless you know that control-specific image replacement should be
    ///          used for this element.
    bool allow_override;

    bool custom_rect;
    kan_instance_offset_t custom_x_offset;
    kan_instance_offset_t custom_y_offset;
    kan_instance_offset_t custom_width;
    kan_instance_offset_t custom_height;
};

/// \brief Syntax sugar drop-in initializer for image draw commands.
#define KAN_UI_IMAGE_COMMAND_DEFAULT(INDEX)                                                                            \
    ((struct kan_ui_draw_command_image_t) {                                                                            \
        .record_index = (INDEX),                                                                                       \
        .allow_override = false,                                                                                       \
        .custom_rect = false,                                                                                          \
        .custom_x_offset = 0,                                                                                          \
        .custom_y_offset = 0,                                                                                          \
        .custom_width = 0,                                                                                             \
        .custom_height = 0,                                                                                            \
    })

/// \brief Syntax sugar drop-in initializer for initializing image commands that will be overridden either way.
#define KAN_UI_IMAGE_COMMAND_NONE                                                                                      \
    ((struct kan_ui_draw_command_image_t) {                                                                            \
        .record_index = KAN_INT_MAX (uint32_t),                                                                        \
        .allow_override = true,                                                                                        \
        .custom_rect = false,                                                                                          \
        .custom_x_offset = 0,                                                                                          \
        .custom_y_offset = 0,                                                                                          \
        .custom_width = 0,                                                                                             \
        .custom_height = 0,                                                                                            \
    })

/// \brief If default text pipeline is used, it is possible select one of up to 255 palette entries for the color when
///        passed as mark in style while creating text object by masking palette index under this mask.
#define KAN_UI_DEFAULT_TEXT_MARK_PALETTE_MASK 0x000000ff

/// \brief If default text pipeline is used, allows to use listed features when passed as mark in style while creating
///        text object.
enum kan_ui_default_text_mark_flag_t
{
    /// \brief No-op zero flag.
    KAN_UI_DEFAULT_TEXT_MARK_FLAG_NONE = 0u,

    /// \brief Enables SDF outline for the parts of text that support it.
    KAN_UI_DEFAULT_TEXT_MARK_FLAG_OUTLINE = 1u << 8u,

    /// \brief Enable cos-based color animation using palette colors.
    KAN_UI_DEFAULT_TEXT_MARK_FLAG_COS_ANIMATION = 1u << 9u,

    /// \brief Enable reading animation that shows glyph clusters one-by-one like typewriter.
    KAN_UI_DEFAULT_TEXT_MARK_FLAG_READING_ANIMATION = 1u << 10u,

    /// \brief Enables multiplication by entry from render color table, entry index is specified in the palette entry.
    KAN_UI_DEFAULT_TEXT_MARK_FLAG_COLOR_TABLE = 1u << 11u,
};

/// \brief Helper for building marks for the default text pipeline for text object styles.
#define KAN_UI_DEFAULT_TEXT_MAKE_MARK(PALETTE, FLAGS) (((PALETTE) & KAN_UI_DEFAULT_TEXT_MARK_PALETTE_MASK) | (FLAGS))

/// \brief Contains data for UI text draw command.
struct kan_ui_draw_command_text_t
{
    /// \brief Shaping unit that should be drawn when ready.
    kan_text_shaping_unit_id_t shaping_unit;
};

/// \brief Describes push constant layout data for custom draw command.
struct kan_ui_draw_command_custom_push_layout_t
{
    struct kan_float_vector_2_t offset;
    struct kan_float_vector_2_t size;

    uint32_t ui_mark;
    uint32_t unused_padding[3u];

    float local_time;
    float unused_padding_2[3u];
};

/// \brief Contains data for UI custom draw command.
struct kan_ui_draw_command_custom_t
{
    /// \details Material instance usage should be managed by the logic, that passed this command to ui.
    kan_interned_string_t material_instance_name;

    /// \details Not owned, must be managed by user that provides it.
    kan_render_pipeline_parameter_set_t object_set;

    /// \details Not owned, must be managed by user that provides it.
    kan_render_pipeline_parameter_set_t shared_set;
};

/// \brief If default draw pipelines are used, it is possible select one of up to 255 palette entries for configuring
///        how rendered primitive should interact with different mark flags.
#define KAN_UI_DEFAULT_COMMAND_MARK_PALETTE_MASK 0x000000ff

/// \brief UI mark flags that are supported in default pipelines and are used to pass information from controls.
KAN_REFLECTION_FLAGS
enum kan_ui_default_command_mark_flag_t
{
    KAN_UI_DEFAULT_MARK_FLAG_NONE = 0u,
    KAN_UI_DEFAULT_MARK_FLAG_HOVERED = 1u << 8u,
    KAN_UI_DEFAULT_MARK_FLAG_DOWN = 1u << 9u,

    /// \brief Used for blinking primitives like text cursors.
    KAN_UI_DEFAULT_MARK_FLAG_BLINK = 1u << 10u,
};

/// \brief Helper for building marks for the ui draw pipelines for draw commands.
#define KAN_UI_DEFAULT_COMMAND_MAKE_MARK(PALETTE, FLAGS)                                                               \
    (((PALETTE) & KAN_UI_DEFAULT_COMMAND_MARK_PALETTE_MASK) | (FLAGS))

/// \brief Describes one draw command for the UI render.
struct kan_ui_draw_command_data_t
{
    /// \brief Additional 32-bit mark that will be passed through instanced data and is used to tell more info about
    ///        the element and its state.
    /// \warning Not the same mark as text marks in text command, this one is intended for passing additional state info
    ///          like hover or mouse down.
    uint32_t ui_mark;

    /// \brief Used to calculate local time for primitive animation on GPU if any animation is used.
    /// \details Relative to `kan_ui_render_graph_singleton_t::animation_global_time_s`.
    float animation_start_time_s;

    /// \brief If inside `kan_ui_node_drawable_t::additional_draw_commands`, setting this to true results in this
    ///        command being executed prior to `kan_ui_node_drawable_t::main_draw_command`.
    /// \details Additional commands are rarely used and therefore we'd like to avoid overhead of using two separate
    ///          arrays for early commands and regular commands as even unused array field still takes 32 bytes of
    ///          original structure.
    bool early;

    enum kan_ui_draw_command_t type;
    union
    {
        KAN_REFLECTION_VISIBILITY_CONDITION_FIELD (type)
        KAN_REFLECTION_VISIBILITY_CONDITION_VALUE (KAN_UI_DRAW_COMMAND_IMAGE)
        struct kan_ui_draw_command_image_t image;

        KAN_REFLECTION_VISIBILITY_CONDITION_FIELD (type)
        KAN_REFLECTION_VISIBILITY_CONDITION_VALUE (KAN_UI_DRAW_COMMAND_TEXT)
        struct kan_ui_draw_command_text_t text;

        KAN_REFLECTION_VISIBILITY_CONDITION_FIELD (type)
        KAN_REFLECTION_VISIBILITY_CONDITION_VALUE (KAN_UI_DRAW_COMMAND_CUSTOM)
        struct kan_ui_draw_command_custom_t custom;
    };
};

/// \brief Internal data that is cached for simpler layout partial recalculation.
/// \details Some data like grow size and compound margins can only be calculated by checking the whole hierarchy.
///          However, when we know that upper-level hierarchy didn't change, it is much easier and much more logical
///          to use cached values during partial recalculation.
struct kan_ui_layout_cached_t
{
    kan_instance_offset_t grow_width;
    kan_instance_offset_t grow_height;
    kan_instance_offset_t compound_margin_left;
    kan_instance_offset_t compound_margin_right;
    kan_instance_offset_t compound_margin_top;
    kan_instance_offset_t compound_margin_bottom;
};

/// \brief Internal enum for deciding how much we need to recalculate in layout update.
enum kan_ui_layout_dirt_level_t
{
    KAN_UI_LAYOUT_DIRT_LEVEL_NONE = 0u,
    KAN_UI_LAYOUT_DIRT_LEVEL_ONLY_RENDER,
    KAN_UI_LAYOUT_DIRT_LEVEL_FULL,
};

/// \brief Accompanies UI node and stores draw commands, laid out coordinates and layout transient data.
struct kan_ui_node_drawable_t
{
    kan_immutable kan_ui_node_id_t id;

    /// \brief Internal index that is used to sort draw commands.
    kan_instance_size_t draw_index;

    /// \brief True if fully clipped out and can never be visible.
    bool fully_clipped_out;

    /// \brief If true, drawable will not be rendered.
    /// \details Does not affect children. Mostly intended to be used by controls for temporary hide/show logic.
    ///          For the high level ui node management logic, it is advised to just delete nodes that should not
    ///          be visible, for example HUD elements that are only shown when user clicks on some button.
    bool hidden;

    /// \brief Clip rect that should be used to render this element.
    struct kan_ui_clip_rect_t clip_rect;

    /// \brief Primary draw command that should be filled by the user.
    /// \details Separate from `additional_draw_commands` array as we'd like to avoid allocating array for one command
    ///          while one command per node is a primary case.
    struct kan_ui_draw_command_data_t main_draw_command;

    /// \brief Additional draw commands to be executed after `main_draw_command` if any.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_ui_draw_command_data_t)
    struct kan_dynamic_array_t additional_draw_commands;

    kan_instance_offset_t local_x;
    kan_instance_offset_t local_y;
    kan_instance_offset_t width;
    kan_instance_offset_t height;
    kan_instance_offset_t global_x;
    kan_instance_offset_t global_y;
    kan_instance_offset_t draw_offset_x;
    kan_instance_offset_t draw_offset_y;

    /// \brief Internal data that is cached for simpler layout partial recalculation.
    struct kan_ui_layout_cached_t cached;

    /// \brief Internal data pointer during layout recalculation.
    KAN_REFLECTION_IGNORE
    void *temporary_data;

    /// \brief Internal dirt level that is only used during layout recalculation.
    enum kan_ui_layout_dirt_level_t layout_dirt_level;
};

UNIVERSE_UI_API void kan_ui_node_drawable_init (struct kan_ui_node_drawable_t *instance);

UNIVERSE_UI_API void kan_ui_node_drawable_shutdown (struct kan_ui_node_drawable_t *instance);

/// \brief Contains ui render graph data, which is only valid for the outside user after
///        `KAN_UI_RENDER_GRAPH_END_CHECKPOINT`.
struct kan_ui_render_graph_singleton_t
{
    /// \brief Allocation with all ui render graph resources.
    const struct kan_render_graph_resource_response_t *allocation;

    /// \brief Pass instance results of which can be presented unto the surface.
    kan_render_pass_instance_t final_pass_instance;

    /// \brief Render image which can be presented on surface after `final_pass_instance`.
    kan_render_image_t final_image;

    /// \brief Clear color for UI viewport render target image.
    struct kan_color_srgb_t clear_color;
};

UNIVERSE_UI_API void kan_ui_render_graph_singleton_init (struct kan_ui_render_graph_singleton_t *instance);

KAN_C_HEADER_END
