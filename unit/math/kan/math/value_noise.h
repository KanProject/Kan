#pragma once

#include <math_api.h>

#include <kan/api_common/core_types.h>
#include <kan/container/dynamic_array.h>
#include <kan/math/inline.h>
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

/// \brief Value noise implementation that operates on floating point numbers on an unsigned integer grid.
/// \details Floating point values are mapped into interval that is specified during
///          `kan_floating_grid_value_noise_2d_build`.
struct kan_floating_grid_value_noise_2d_t
{
    kan_instance_size_t cell_width;
    kan_instance_size_t cell_height;
    kan_instance_size_t cells_x;
    kan_instance_size_t cells_y;

    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (kan_floating_t)
    struct kan_dynamic_array_t values;
};

/// \brief Initializes empty value noise instance.
MATH_API void kan_floating_grid_value_noise_2d_init (struct kan_floating_grid_value_noise_2d_t *instance);

/// \brief Shuts down value noise instance.
MATH_API void kan_floating_grid_value_noise_2d_shutdown (struct kan_floating_grid_value_noise_2d_t *instance);

/// \brief Clears any data inside value noise and deallocates memory.
MATH_API void kan_floating_grid_value_noise_2d_reset (struct kan_floating_grid_value_noise_2d_t *instance);

/// \brief Builds value noise lattice for given grid size with given random generator.
MATH_API void kan_floating_grid_value_noise_2d_build (struct kan_floating_grid_value_noise_2d_t *instance,
                                                      struct kan_random_xoshiro_t *generator,
                                                      kan_instance_size_t width,
                                                      kan_instance_size_t height,
                                                      kan_instance_size_t cell_width,
                                                      kan_instance_size_t cell_height,
                                                      kan_floating_t min,
                                                      kan_floating_t max);

/// \brief Samples built value noise lattice at given point.
/// \warning Returns 0.0 if out of bounds or when there is no built lattice.
MATH_API kan_floating_t kan_floating_grid_value_noise_2d_sample (
    const struct kan_floating_grid_value_noise_2d_t *instance, kan_instance_size_t x, kan_instance_size_t y);

KAN_C_HEADER_END
