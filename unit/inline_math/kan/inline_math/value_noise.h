#pragma once

#include <inline_math_api.h>

#include <kan/api_common/core_types.h>
#include <kan/container/dynamic_array.h>
#include <kan/inline_math/inline_math.h>
#include <kan/reflection/markup.h>

/// \file
/// \brief Contains value noise implementations for particular cases.
///
/// \par Description.
/// \parblock
/// Value noise is the simplest noise type: it just randomizes lattice points and then uses interpolation to sample
/// between them. It makes it much easier to control the results comparing to more complex gradient noises.
///
/// For general info see https://en.wikipedia.org/wiki/Value_noise.
/// \endparblock

KAN_C_HEADER_BEGIN

/// \brief Value noise implementation that generates uniformly distributed uint16_t points and
///        simple linear interpolation during sampling.
/// \details Aimed to be used for cases like weight map where smoothness of output does not matter.
///          For example, can be used to generate land outline on map by generating "it is land" weights that can
///          then split into land and sea by using percentiles.
struct kan_unorm16_value_noise_2d_t
{
    kan_instance_size_t cell_width;
    kan_instance_size_t cell_height;
    kan_instance_size_t cells_x;
    kan_instance_size_t cells_y;

    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (uint16_t)
    struct kan_dynamic_array_t values;
};

/// \brief Initializes empty value noise instance.
INLINE_MATH_API void kan_unorm16_value_noise_2d_init (struct kan_unorm16_value_noise_2d_t *instance);

/// \brief Shuts down value noise instance.
INLINE_MATH_API void kan_unorm16_value_noise_2d_shutdown (struct kan_unorm16_value_noise_2d_t *instance);

/// \brief Clears any data inside value noise and deallocates memory.
INLINE_MATH_API void kan_unorm16_value_noise_2d_reset (struct kan_unorm16_value_noise_2d_t *instance);

/// \brief Builds value noise lattice for given grid size with given random generator.
/// \invariant Cell width and cell height are not higher than UINT16_MAX.
INLINE_MATH_API void kan_unorm16_value_noise_2d_build (struct kan_unorm16_value_noise_2d_t *instance,
                                                       struct kan_random_xoshiro_t *generator,
                                                       kan_instance_size_t width,
                                                       kan_instance_size_t height,
                                                       kan_instance_size_t cell_width,
                                                       kan_instance_size_t cell_height);

/// \brief Samples built value noise lattice at given point.
/// \warning Returns 0 if if out of bounds or when there is no built lattice.
/// \details Returns `kan_instance_size_t` instead of `uint16_t` as it is usually simpler for calculations and avoids
///          unnecessary value conversions on user side as well.
INLINE_MATH_API kan_instance_size_t kan_unorm16_value_noise_2d_sample (
    const struct kan_unorm16_value_noise_2d_t *instance, kan_instance_size_t x, kan_instance_size_t y);

KAN_C_HEADER_END
