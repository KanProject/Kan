#pragma once

#include <text_api.h>

#include <kan/api_common/core_types.h>
#include <kan/container/interned_string.h>
#include <kan/context/render_backend_system.h>
#include <kan/inline_math/inline_math.h>
#include <kan/reflection/markup.h>

/// \file
/// \brief Provides API for building text objects and shaping them for rendering.
///
/// \par Text object
/// \parblock
/// Text object is a container of processed text data with proper markup for further processing like shaping for text
/// render. Text objects are built from items: chunks of utf8 character sequences, style-control nodes and icons.
/// Text object construction performs operations like splitting characters into text script runs and merging text script
/// runs if they belonged to consecutive items.
///
/// Beware that we do not implement full bidi algorithm and are using very basic simplified solution for that instead.
/// See docs on `KAN_TEXT_BIDI_CUSTOM_BREAK` for more information about that.
/// \endparblock
///
/// \par Text shaped data
/// \parblock
/// When text is shaped, data for rendering it on GPU is provided through `kan_text_shaped_data_t`. For the sake of
/// implementation simplicity, shaped data coordinates are not guaranteed to be always positive and start from (0, 0).
/// Instead, we aim to start as close to (0, 0) as possible, but provide min-max bounds so the user can tell actual
/// calculated bounds and use them to offset shaped text.
///
/// Glyphs and icons are expected to be rendered in separate draw calls: one draw call for all the glyphs and one draw
/// call for all the icons if text has any. Both glyph and icon data is passed in arrays that can be directly uploaded
/// to the GPU as instanced attributes for the text rendering.
///
/// Glyph instance data contains local min-max coordinates of the glyph rectangle along with rendered glyph uv
/// coordinates and layer on font library atlas. It also has mark 32-bit integer that can be used for various effects
/// and glyph coloring, and read index that can be used for "delayed appearance" effects. Read index is guaranteed to
/// only increment when codepoint cluster increments: it does not increment for glyphs that are drawn on top of the
/// base glyphs, therefore multi-glyph clusters will not be broken.
///
/// Icon instance data provides the same local min-max, mark and read index, and also icon index that should be used by
/// user render logic to choose the actual icon visuals.
///
/// Keep in mind that you need to bind proper font library atlas for rendering. Font library atlas can be changed due
/// to reallocation: we need to reallocate it if it has no space for all the glyphs we need.
/// \endparblock
///
/// \par Font library
/// \parblock
/// Font library is a complete set of font faces for shaping text objects. It is expected that user needs to have only
/// one font library at any time, possibly recreating library when locale changes, but having multiple libraries
/// is supported.
///
/// Font library consists of categories, where every category is a font face for particular unicode script and for
/// particular user-selected style, for example "bold" or "italic". Every category can have its own OpenType font blob
/// data and its own variable axis values.
///
/// Font libraries can shape text objects through `kan_font_library_shape` and precache glyph visuals through
/// `kan_font_library_precache` functions.
/// \endparblock

KAN_C_HEADER_BEGIN

KAN_HANDLE_DEFINE (kan_text_t);
KAN_HANDLE_DEFINE (kan_font_library_t);

/// \brief Unicode codepoint value.
typedef uint32_t kan_unicode_codepoint_t;

/// \brief Returns next unicode codepoint or 0 when iteration ended or text is malformed.
/// \param iterator Pointer to pointer that is used as string iterator and modified by this function.
/// \param boundary Used as text-end mark and this function will return error if boundary interrupts codepoint.
TEXT_API kan_unicode_codepoint_t kan_text_utf8_next (const uint8_t **iterator, const uint8_t *boundary);

/// \brief Returns pointer to the previous codepoint seen from given current codepoint. Returns `NULL` on error.
/// \param from Pointer to the current codepoint for which we're searching previous one.
/// \param boundary Used as text-start mark and this function will return error if boundary interrupts codepoint.
TEXT_API const uint8_t *kan_text_utf8_find_previous (const uint8_t *from, const uint8_t *boundary);

/// \brief Result container for `kan_text_codepoint_to_utf8`.
struct kan_text_utf8_result_t
{
    kan_instance_size_t length;
    char data[4u];
};

/// \brief Converts unicode codepoint to utf8. Result has zero length if error has occurred.
TEXT_API struct kan_text_utf8_result_t kan_text_utf32_to_utf8 (kan_unicode_codepoint_t codepoint);

/// \brief Custom break character for the simplified manual bidi.
/// \details We do not perform full scale bidi for utf8 text fragments. Instead, neutral characters are assigned to
///          the script of the previous non-neutral character or to the next non-neutral character if there is
///          no previous one. It can result in artifacts, therefore we use '\u0091' character as "bidi reset"
///          character which means that characters after "bidi reset" character ignore characters prior to
///          "bidi reset" character when selecting script for neutral characters. '\u0091' character itself is
///          not added to the built text. '\u0091' is marked as private use character in unicode, so it should
///          be fine to use it like that.
#define KAN_TEXT_BIDI_CUSTOM_BREAK "\xc2\x91"

/// \brief Integer value of `KAN_TEXT_BIDI_CUSTOM_BREAK`.
#define KAN_TEXT_BIDI_CUSTOM_BREAK_VALUE 0x91

/// \brief Enumerates known text orientations.
enum kan_text_orientation_t
{
    KAN_TEXT_ORIENTATION_HORIZONTAL = 0u,
    KAN_TEXT_ORIENTATION_VERTICAL,
};

/// \brief Enumerates known horizontal reading directions.
enum kan_text_reading_direction_t
{
    KAN_TEXT_READING_DIRECTION_LEFT_TO_RIGHT = 0u,
    KAN_TEXT_READING_DIRECTION_RIGHT_TO_LEFT,
};

/// \brief Enumerates types of items from which text object is built.
enum kan_text_item_type_t
{
    /// \brief Used for simplicity when empty parameters are skipped during formatting, for example.
    KAN_TEXT_ITEM_EMPTY = 0u,

    /// \brief Used to append utf8 text slice as null terminated string.
    KAN_TEXT_ITEM_UTF8,

    /// \brief Used to insert glyph-sized icon into the text represented by icon index.
    KAN_TEXT_ITEM_ICON,

    /// \brief Used to push styles and mark indices.
    KAN_TEXT_ITEM_STYLE,
};

/// \brief Contains data for the text icon item.
/// \details Icon item y bearing is equal to primary default `ascender * y_scale`.
struct kan_text_item_icon_t
{
    /// \brief Icon indices are passed to the user render code so it can choose the proper icon.
    uint32_t icon_index;

    /// \brief Icon item width is primary default `(ascender - descender) * x_scale`.
    float x_scale;

    /// \brief Icon item height is primary default `(ascender - descender) * y_scale`.
    float y_scale;
};

/// \brief Contains data for the text style item.
struct kan_text_item_style_t
{
    /// \brief Style name or NULL for the default style.
    kan_interned_string_t style;

    /// \brief 32-bit user mark that can be used by user render code for different text render effects.
    uint32_t mark;
};

/// \brief Describes one item of the text object for text object construction.
struct kan_text_item_t
{
    enum kan_text_item_type_t type;
    union
    {
        const char *utf8;
        const kan_unicode_codepoint_t *utf32;
        struct kan_text_item_icon_t icon;
        struct kan_text_item_style_t style;
    };
};

/// \brief Builds text object from provided array of items.
TEXT_API kan_text_t kan_text_create (kan_instance_size_t items_count, struct kan_text_item_t *items);

/// \brief Destroys given text object immediately.
TEXT_API void kan_text_destroy (kan_text_t instance);

/// \brief Describes shaped instance of one glyph from text.
struct kan_text_shaped_glyph_instance_data_t
{
    struct kan_float_vector_2_t min;
    struct kan_float_vector_2_t max;
    struct kan_float_vector_2_t uv_min;
    struct kan_float_vector_2_t uv_max;
    uint32_t layer;
    uint32_t mark;
    uint32_t read_index;
};

/// \brief Describes shaped instance of one icon from text.
struct kan_text_shaped_icon_instance_data_t
{
    struct kan_float_vector_2_t min;
    struct kan_float_vector_2_t max;
    uint32_t icon_index;
    uint32_t mark;
    uint32_t read_index;
};

/// \brief Contains information about one visual cluster for the text edition logic.
/// \details Cluster is a one or more glyphs that form one visual letter that is perceived by the user as one letter in
///          most cases. It is important nail down how clusters interact with text editing operations:
///          - Clusters are used to calculate cursor visual and data positions.
///          - Text input is applied at data position and has nothing to do with clusters.
///          - Backspace and Delete keys are applied at data position and operate on codepoint level.
///            They can modify multi-codepoint clusters by removing overlay glyphs from them that way and it is an
///            expected text editing behavior.
///          - Selection operates by converting pointer positions to data positions and then treating codepoints between
///            these data positions as selected. It is expected as any other behavior would break when bidi is present.
///          - Left/Right arrow keys operate purely on cluster level and move cursor between clusters when on horizontal
///            orientation is used.
///          - Top/Down arrow keys operate by changing sequence and then matching old visual cursor positions as pointer
///            position in order to select proper cluster when horizontal orientation is used.
struct kan_text_shaped_edition_cluster_data_t
{
    kan_instance_offset_t visual_min;
    kan_instance_offset_t visual_max;
    kan_instance_offset_t visual_cursor_position;

    /// \brief Used to mark cluster that have inverted (right-to-left or bottom-to-top) behavior for pointer-to-cursor
    ///        calculation logic.
    /// \details When pointer is not inverted, pointing to the left side of the cluster places cursor before this
    ///          cluster (logically, after previous cluster) and pointing to the right side of cluster places cursor
    ///          after this cluster according to this cluster pointer position. When pointer inverted, this logic is
    ///          inverted, however "previous cluster" is then cluster in next array index due to that inversion as well.
    bool invert_pointer;

    /// \brief Index of byte from which codepoints that form this cluster start if all strings passed into text during
    ///        creation are treated as one continuous string.
    /// \details Shaping-level logic has no info about original text items as they were merged or separated during text
    ///          object creation. Therefore, cluster data start index is computed as if all input strings were merged
    ///          into one string, which is usually the case for text edition and therefore is fine.
    kan_instance_size_t start_at_index;
};

/// \brief Contains data about one shaped sequence -- line or column -- needed for editing that text sequence.
struct kan_text_shaped_edition_sequence_data_t
{
    kan_instance_offset_t baseline;
    kan_instance_offset_t ascender;
    kan_instance_offset_t descender;

    /// \brief Index of byte since which content no longer belongs to this sequence.
    /// \details Follows the same indexing rules as `kan_text_shaped_edition_cluster_data_t::start_at_index`.
    ///          Used to simplify pointer-to-content-index conversion.
    kan_instance_size_t end_at_index;

    /// \brief List of clusters in that line/column sequence.
    /// \details Ordered by their appearance: left-to-right for horizontal and top-to-down for vertical.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_text_shaped_edition_cluster_data_t)
    struct kan_dynamic_array_t clusters;
};

TEXT_API void kan_text_shaped_edition_sequence_data_init (struct kan_text_shaped_edition_sequence_data_t *instance);

TEXT_API void kan_text_shaped_edition_sequence_data_shutdown (struct kan_text_shaped_edition_sequence_data_t *instance);

/// \brief Contains shaped data for rendering the whole text object.
struct kan_text_shaped_data_t
{
    /// \brief Size on the primary direction.
    kan_instance_size_t typographic_primary_size;

    /// \brief Size on the secondary direction.
    kan_instance_size_t typographic_secondary_size;

    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_text_shaped_glyph_instance_data_t)
    struct kan_dynamic_array_t glyphs;

    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_text_shaped_icon_instance_data_t)
    struct kan_dynamic_array_t icons;

    /// \brief List of lines/columns for edition implementation, always ordered by baseline value.
    /// \warning Non-empty only if `kan_text_shaping_request_t::generate_edition_markup` is true.
    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (struct kan_text_shaped_edition_sequence_data_t)
    struct kan_dynamic_array_t edition_sequences;
};

TEXT_API void kan_text_shaped_data_init (struct kan_text_shaped_data_t *instance);

TEXT_API void kan_text_shaped_data_shutdown (struct kan_text_shaped_data_t *instance);

/// \brief Describes one font library category.
/// \invariant Category data must be an OpenType font blob.
/// \details Different categories can reuse the same data as it is used as read only memory.
struct kan_font_library_category_t
{
    /// \brief Unicode script id in iso15924 format.
    kan_interned_string_t script;

    /// \brief Name of the provided style or NULL for the default style.
    kan_interned_string_t style;

    kan_instance_size_t variable_axis_count;
    float *variable_axis;

    kan_memory_size_t data_size;
    const void *data;
};

/// \brief Creates new font library with given categories.
/// \details Category order matters as first category with NULL style is treated as primary default category.
TEXT_API kan_font_library_t kan_font_library_create (kan_render_context_t render_context,
                                                     kan_instance_size_t categories_count,
                                                     struct kan_font_library_category_t *categories);

/// \brief Returns image that is used as SDF glyphs atlas right now.
TEXT_API kan_render_image_t kan_font_library_get_sdf_atlas (kan_font_library_t instance);

/// \brief Enumerates supported glyph render formats.
enum kan_font_glyph_render_format_t
{
    /// \brief Glyphs are rendered as signed distance fields.
    /// \details Looks good for the most fonts for latin scripts, but may have issues for scripts with more complex
    ///          glyph visuals like arabic ones.
    KAN_FONT_GLYPH_RENDER_FORMAT_SDF = 0u,
};

/// \brief Enumerates support line alignments for text shaping.
enum kan_text_shaping_alignment_t
{
    /// \brief Shaped text lines are aligned to the left border.
    /// \details For vertical orientation, treated as top alignment.
    KAN_TEXT_SHAPING_ALIGNMENT_LEFT = 0u,

    /// \brief Shaped text lines are aligned to be in the center using line length limit.
    KAN_TEXT_SHAPING_ALIGNMENT_CENTER,

    /// \brief Shaped text lines are aligned to the right border.
    /// \details For vertical orientation, treated as bottom alignment.
    KAN_TEXT_SHAPING_ALIGNMENT_RIGHT,
};

/// \brief Describes how text object should be shaped for render.
struct kan_text_shaping_request_t
{
    /// \brief Font size for calculating glyph sizes for shaping.
    kan_instance_size_t font_size;

    /// \brief Glyph render format for all glyphs in the text.
    enum kan_font_glyph_render_format_t render_format;

    /// \brief Desired text orientation.
    enum kan_text_orientation_t orientation;

    /// \brief Reading direction that should be based on locale.
    /// \details Reading direction decides where lines begin and whether text runs can be broken while splitting text
    ///          into lines. Only text runs of the script that has the same reading direction as requested can be broken
    ///          into lines, text runs of non-compatible scripts should not be broken in order to keep text readable and
    ///          easy to understand.
    enum kan_text_reading_direction_t reading_direction;

    /// \brief Alignment for text lines.
    enum kan_text_shaping_alignment_t alignment;

    /// \brief Line/column length limit in pixels.
    /// \details Shaping algorithm tries as hard as possible to fit text data into this limit,
    ///          introducing line breaks whenever possible.
    kan_instance_size_t primary_axis_limit;

    /// \brief True if line/column breaks are allowed.
    /// \details When breaks are not allowed, primary axis limit is still used for alignment
    ///          if shaped text primary size is less than limit.
    bool allow_breaks;

    /// \brief If true, then additional markup needed for text edition implementation will be generated as well.
    bool generate_edition_markup;

    /// \brief Text object to be shaped into data for rendering.
    kan_text_t text;
};

/// \brief Performs text shaping for the given request.
/// \details Should be quite fast and okay for every-frame execution (if it is really needed) as long as all glyphs
///          are already cached. Glyphs are cached the first time they are encountered or using precache request.
///          When glyphs are not cached, can result in noticeable hitch, sometimes up to 100ms when there are no cached
///          glyphs at all.
TEXT_API bool kan_font_library_shape (kan_font_library_t instance,
                                      struct kan_text_shaping_request_t *request,
                                      struct kan_text_shaped_data_t *output);

/// \brief Describes which glyphs need to be precached.
struct kan_text_precache_request_t
{
    /// \brief Script for category selection.
    /// \details Script can be inferred from utf8 sequence, but user always knows for which script precaching is
    ///          executed, therefore we can skip script detection for every character and simplify implementation.
    kan_interned_string_t script;

    /// \brief Style for the category selection.
    kan_interned_string_t style;

    /// \brief Glyph render format for all glyphs in the text.
    enum kan_font_glyph_render_format_t render_format;

    /// \brief Desired text orientation.
    enum kan_text_orientation_t orientation;

    /// \brief Sequence of utf8 codepoints, nominal glyphs for which are precached.
    const char *utf8;
};

/// \brief Precaches glyph data on atlas for nominal glyphs specified by codepoints from given utf8 string.
TEXT_API bool kan_font_library_precache (kan_font_library_t instance, struct kan_text_precache_request_t *request);

/// \brief Destroys given text library.
TEXT_API void kan_font_library_destroy (kan_font_library_t instance);

KAN_C_HEADER_END
