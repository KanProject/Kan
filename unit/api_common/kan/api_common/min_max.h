#pragma once

#include <kan/api_common/core_types.h>

/// \file
/// \brief Provides min-max macros for code readability.
/// \details These macros wrap implementation functions to make it safe to use function calls as macro arguments.
///          Otherwise, function calls would be done twice and this result in error when function returns different
///          value, which is the case for things like random generators.

#define KAN_MIN_MAX_IMPL(TYPE)                                                                                         \
    static inline TYPE kan_min_impl_##TYPE (TYPE left, TYPE right) { return left < right ? left : right; }             \
    static inline TYPE kan_max_impl_##TYPE (TYPE left, TYPE right) { return left > right ? left : right; }             \
    static inline TYPE kan_clamp_impl_##TYPE (TYPE value, TYPE min, TYPE max)                                          \
    {                                                                                                                  \
        return value < min ? min : (value > max ? max : value);                                                        \
    }

KAN_MIN_MAX_IMPL (uint8_t)
KAN_MIN_MAX_IMPL (uint16_t)
KAN_MIN_MAX_IMPL (uint32_t)
KAN_MIN_MAX_IMPL (uint64_t)
KAN_MIN_MAX_IMPL (int8_t)
KAN_MIN_MAX_IMPL (int16_t)
KAN_MIN_MAX_IMPL (int32_t)
KAN_MIN_MAX_IMPL (int64_t)
KAN_MIN_MAX_IMPL (float)
KAN_MIN_MAX_IMPL (double)

#define KAN_MIN(LEFT, RIGHT)                                                                                           \
    __CUSHION_PRESERVE__ _Generic ((LEFT),                                                                             \
        uint8_t: kan_min_impl_uint8_t,                                                                                 \
        uint16_t: kan_min_impl_uint16_t,                                                                               \
        uint32_t: kan_min_impl_uint32_t,                                                                               \
        uint64_t: kan_min_impl_uint64_t,                                                                               \
        int8_t: kan_min_impl_int8_t,                                                                                   \
        int16_t: kan_min_impl_int16_t,                                                                                 \
        int32_t: kan_min_impl_int32_t,                                                                                 \
        int64_t: kan_min_impl_int64_t,                                                                                 \
        float: kan_min_impl_float,                                                                                     \
        double: kan_min_impl_double) ((LEFT), (RIGHT))

#define KAN_MAX(LEFT, RIGHT)                                                                                           \
    __CUSHION_PRESERVE__ _Generic ((LEFT),                                                                             \
        uint8_t: kan_max_impl_uint8_t,                                                                                 \
        uint16_t: kan_max_impl_uint16_t,                                                                               \
        uint32_t: kan_max_impl_uint32_t,                                                                               \
        uint64_t: kan_max_impl_uint64_t,                                                                               \
        int8_t: kan_max_impl_int8_t,                                                                                   \
        int16_t: kan_max_impl_int16_t,                                                                                 \
        int32_t: kan_max_impl_int32_t,                                                                                 \
        int64_t: kan_max_impl_int64_t,                                                                                 \
        float: kan_max_impl_float,                                                                                     \
        double: kan_max_impl_double) ((LEFT), (RIGHT))

#define KAN_CLAMP(VALUE, LEFT, RIGHT)                                                                                  \
    __CUSHION_PRESERVE__ _Generic ((VALUE),                                                                            \
        uint8_t: kan_clamp_impl_uint8_t,                                                                               \
        uint16_t: kan_clamp_impl_uint16_t,                                                                             \
        uint32_t: kan_clamp_impl_uint32_t,                                                                             \
        uint64_t: kan_clamp_impl_uint64_t,                                                                             \
        int8_t: kan_clamp_impl_int8_t,                                                                                 \
        int16_t: kan_clamp_impl_int16_t,                                                                               \
        int32_t: kan_clamp_impl_int32_t,                                                                               \
        int64_t: kan_clamp_impl_int64_t,                                                                               \
        float: kan_clamp_impl_float,                                                                                   \
        double: kan_clamp_impl_double) ((VALUE), (LEFT), (RIGHT))
