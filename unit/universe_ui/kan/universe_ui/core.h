#pragma once

#include <universe_ui_api.h>

#include <kan/api_common/c_header.h>
#include <kan/api_common/core_types.h>
#include <kan/inline_math/inline_math.h>
#include <kan/resource_ui/bundle.h>
#include <kan/threading/atomic.h>
#include <kan/universe/universe.h>
#include <kan/universe_text/text.h>

// TODO: Docs.

KAN_C_HEADER_BEGIN

/// \brief Group that is used to add all ui core mutators: both layout and render ones.
#define KAN_UI_CORE_MUTATOR_GROUP "ui_core"

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

/// \details Should not be accessed outside of left worlds. UI is designed to only be present in leaf worlds.
struct kan_ui_singleton_t
{
    /// \brief Atomic counter for assigning ui node ids. Safe to be modified from different threads.
    struct kan_atomic_int_t node_id_counter;

    float scale;
    kan_instance_offset_t viewport_width;
    kan_instance_offset_t viewport_height;
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
struct kan_ui_bundle_updated_event_t
{
    kan_instance_size_t stub;
};

enum kan_ui_coordinate_type_t
{
    /// \brief Points, just like pixels, but can be scaled by global UI scale.
    KAN_UI_PT = 0u,

    /// \brief Plain old pixels.
    KAN_UI_PX,

    /// \brief Window height ratio, for example 0.5 = half of the window height.
    KAN_UI_WH,

    /// \brief Window width ration, for example 0.5 = half of window width.
    KAN_UI_WW,
};

struct kan_ui_coordinate_t
{
    enum kan_ui_coordinate_type_t type;
    float value;
};

#define KAN_UI_VALUE_BUILD(TYPE, VALUE) ((struct kan_ui_coordinate_t) {.type = TYPE, .value = (VALUE)})
#define KAN_UI_VALUE_PT(VALUE) KAN_UI_VALUE_BUILD (KAN_UI_PT, VALUE)
#define KAN_UI_VALUE_PX(VALUE) KAN_UI_VALUE_BUILD (KAN_UI_PX, VALUE)
#define KAN_UI_VALUE_WH(VALUE) KAN_UI_VALUE_BUILD (KAN_UI_WH, VALUE)
#define KAN_UI_VALUE_WW(VALUE) KAN_UI_VALUE_BUILD (KAN_UI_WW, VALUE)

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
#define KAN_UI_RECT_WH(LEFT, RIGHT, TOP, BOTTOM) KAN_UI_RECT_BUILD (KAN_UI_VALUE_WH, LEFT, RIGHT, TOP, BOTTOM)
#define KAN_UI_RECT_WW(LEFT, RIGHT, TOP, BOTTOM) KAN_UI_RECT_BUILD (KAN_UI_VALUE_WW, LEFT, RIGHT, TOP, BOTTOM)

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

enum kan_ui_horizontal_alignment_t
{
    KAN_UI_HORIZONTAL_ALIGNMENT_LEFT,
    KAN_UI_HORIZONTAL_ALIGNMENT_CENTER,
    KAN_UI_HORIZONTAL_ALIGNMENT_RIGHT,
};

enum kan_ui_vertical_alignment_t
{
    KAN_UI_VERTICAL_ALIGNMENT_TOP,
    KAN_UI_VERTICAL_ALIGNMENT_CENTER,
    KAN_UI_VERTICAL_ALIGNMENT_BOTTOM,
};

struct kan_ui_node_element_setup_t
{
    enum kan_ui_size_flags_t width_flags;
    enum kan_ui_size_flags_t height_flags;

    struct kan_ui_coordinate_t width;
    struct kan_ui_coordinate_t height;
    struct kan_ui_rect_t margin;

    /// \warning Ignored in horizontal containers.
    enum kan_ui_horizontal_alignment_t horizontal_alignment;

    /// \warning Ignored in vertical containers.
    enum kan_ui_vertical_alignment_t vertical_alignment;

    /// \brief If child of frame layout, offset on X-axis from aligned node border to alignment line.
    /// \details For example, for left alignment 10px that means moving element left border 10 pixels to the left from
    ///          parent left border. For right alignment 10px means moving element right border 10 pixels to the right
    ///          from parent right border (therefore you usually need negative values for right alignment). For center
    ///          alignment, 10px means moving element center 10 pixels to the right from parent center line.
    struct kan_ui_coordinate_t frame_offset_x;

    /// \brief The same as `frame_offset_y`, but for horizontal offsets.
    struct kan_ui_coordinate_t frame_offset_y;
};

enum kan_ui_layout_t
{
    KAN_UI_LAYOUT_FRAME = 0u,
    KAN_UI_LAYOUT_VERTICAL_CONTAINER,
    KAN_UI_LAYOUT_HORIZONTAL_CONTAINER,
};

// TODO: Rename nine patch to nine slice (more known name).

struct kan_ui_node_layout_setup_t
{
    enum kan_ui_layout_t layout;
    struct kan_ui_rect_t padding;
};

struct kan_ui_node_render_setup_t
{
    kan_instance_offset_t scroll_x_px;
    kan_instance_offset_t scroll_y_px;
    bool clip_children;
};

struct kan_ui_node_t
{
    kan_ui_node_id_t id;
    kan_ui_node_id_t parent_id;

    struct kan_ui_node_element_setup_t element;

    /// \brief Local integer value used to sort child elements.
    /// \details For frame layout children or for root nodes affects only draw order.
    ///          For container layout children, affects positions on container layout.
    kan_instance_offset_t local_element_order;

    struct kan_ui_node_layout_setup_t layout;
    struct kan_ui_node_render_setup_t render;
};

UNIVERSE_UI_API void kan_ui_node_init (struct kan_ui_node_t *instance);

struct kan_ui_clip_rect_t
{
    kan_instance_offset_t x;
    kan_instance_offset_t y;
    kan_instance_offset_t width;
    kan_instance_offset_t height;
};

enum kan_ui_draw_command_t
{
    /// \brief For disabled commands.
    KAN_UI_DRAW_COMMAND_NONE = 0u,

    /// \brief Image from UI atlas. Receives rect coordinates from instanced data.
    KAN_UI_DRAW_COMMAND_IMAGE,

    /// \brief Shaped text from universe_text unit.
    KAN_UI_DRAW_COMMAND_TEXT,

    /// \brief Element rect will be drawn with given material instance and object parameter set.
    KAN_UI_DRAW_COMMAND_CUSTOM,
};

struct kan_ui_draw_command_image_t
{
    uint32_t image_record_index;

    /// \brief Additional 32-bit mark that will be passed through instanced data and can be used for custom effects if
    ///        user overrides default UI material.
    uint32_t ui_mark;
};

#define KAN_UI_DEFAULT_TEXT_MARK_PALETTE_MASK 0x000000ff

enum kan_ui_default_text_mark_flag_t
{
    KAN_UI_DEFAULT_TEXT_MARK_FLAG_NONE = 0u,
    KAN_UI_DEFAULT_TEXT_MARK_FLAG_OUTLINE = 1u << 8u,
    KAN_UI_DEFAULT_TEXT_MARK_FLAG_COS_ANIMATION = 1u << 9u,
    KAN_UI_DEFAULT_TEXT_MARK_FLAG_READING_ANIMATION = 1u << 10u,
    KAN_UI_DEFAULT_TEXT_MARK_FLAG_COLOR_TABLE = 1u << 11u,
};

#define KAN_UI_DEFAULT_TEXT_MAKE_MARK(PALETTE, FLAGS) (((PALETTE) & KAN_UI_DEFAULT_TEXT_MARK_PALETTE_MASK) | (FLAGS))

struct kan_ui_draw_command_text_t
{
    kan_text_shaping_unit_id_t shaping_unit;

    /// \brief Additional 32-bit mark, that will be passed through push constants for the text as a whole.
    uint32_t ui_mark;

    /// \brief Used to calculate local time for text animation on GPU if any.
    /// \details Relative to `kan_ui_render_graph_singleton_t::animation_global_time_s`.
    float animation_start_time_s;

    // TODO: Would surely require a little bit more fields later for correct spacing.
};

struct kan_ui_draw_command_custom_push_layout_t
{
    struct kan_float_vector_2_t offset;
    struct kan_float_vector_2_t size;
};

struct kan_ui_draw_command_custom_t
{
    /// \details Material instance usage should be managed by the logic, that passed this command to ui.
    kan_interned_string_t material_instance_name;

    /// \details Not owned, must be managed by user that provides it.
    kan_render_pipeline_parameter_set_t object_set;

    /// \details Not owned, must be managed by user that provides it.
    kan_render_pipeline_parameter_set_t shared_set;
};

struct kan_ui_draw_command_data_t
{
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

/// \brief Internal enum for deciding how much we need to recalculate in layout update.
enum kan_ui_layout_dirt_level_t
{
    KAN_UI_LAYOUT_DIRT_LEVEL_NONE = 0u,
    KAN_UI_LAYOUT_DIRT_LEVEL_ONLY_RENDER,
    KAN_UI_LAYOUT_DIRT_LEVEL_FULL,
};

struct kan_ui_node_drawable_t
{
    kan_ui_node_id_t id;
    kan_instance_size_t draw_index;

    bool fully_clipped_out;
    struct kan_ui_clip_rect_t clip_rect;

    struct kan_ui_draw_command_data_t main_draw_command;

    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_ui_draw_command_data_t)
    struct kan_dynamic_array_t additional_draw_commands;

    kan_instance_offset_t local_x;
    kan_instance_offset_t local_y;
    kan_instance_offset_t width;
    kan_instance_offset_t height;
    kan_instance_offset_t global_x;
    kan_instance_offset_t global_y;

    kan_instance_offset_t cached_grow_width;
    kan_instance_offset_t cached_grow_height;

    KAN_REFLECTION_IGNORE
    void *temporary_data;
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

    /// \brief Global time for UI gpu-based animations.
    /// \details Calculated inside render graph singleton by manually appending delta's.
    float animation_global_time_s;

    /// \brief Global time for UI animations loops back to zero when it becomes higher than this value.
    float animation_global_time_loop_s;

    /// \brief Use to calculate deltas for `animation_global_time_s`.
    kan_time_size_t last_time_ns;

    float clear_color_r;
    float clear_color_g;
    float clear_color_b;
    float clear_color_a;
};

UNIVERSE_UI_API void kan_ui_render_graph_singleton_init (struct kan_ui_render_graph_singleton_t *instance);

KAN_C_HEADER_END
