#pragma once

#include <kan/api_common/c_header.h>

/// \file
/// \brief Contains quick select algorithm implementation.
///
/// \par Description.
/// \parblock
/// Quick select is an algorithm that makes it possible to find K-th smallest value in average linear time.
/// This is very useful when working with percentiles as fully sorting values would be much slower.
/// \endparblock 

KAN_C_HEADER_BEGIN

/// \brief Quick select algorithm implemented through one macro.
/// \details Implementation is based upon https://rosettacode.org/wiki/Quickselect_algorithm#C.
/// \warning User must be aware that algorithm modifies input sequence using swaps!
/// \param SIZE Size of an input sequence.
/// \param K Index of smallest element to find as we are searching for K-th smallest element.
/// \param OUTPUT Where resulting index that points to resulting value should be stored. If you're using copy of an
///               array to run quick select, you must take value from the copy on which algorithm was executed!
/// \param QLESS Macro for comparing values at two indices. Receives two indices and should evaluate to true if left
///              index value is smaller than right index value.
/// \param QSWAP Macro for swapping value at two indices. Receives two indices and should swap values at them.
#define QSELECT(SIZE, K, OUTPUT, QLESS, QSWAP)                                                                         \
    {                                                                                                                  \
        unsigned long offset = 0u;                                                                                     \
        unsigned long length = (SIZE);                                                                                 \
        unsigned long k = (K);                                                                                         \
                                                                                                                       \
        while (true)                                                                                                   \
        {                                                                                                              \
            unsigned long i;                                                                                           \
            unsigned long st;                                                                                          \
                                                                                                                       \
            if (length <= 1u)                                                                                          \
            {                                                                                                          \
                OUTPUT = (typeof (OUTPUT)) offset;                                                                     \
                break;                                                                                                 \
            }                                                                                                          \
                                                                                                                       \
            for (i = st = 0u; i < length - 1u; ++i)                                                                    \
            {                                                                                                          \
                if (QLESS (offset + length - 1u, offset + i))                                                          \
                {                                                                                                      \
                    continue;                                                                                          \
                }                                                                                                      \
                                                                                                                       \
                QSWAP (offset + i, offset + st);                                                                       \
                ++st;                                                                                                  \
            }                                                                                                          \
                                                                                                                       \
            QSWAP (offset + length - 1u, offset + st);                                                                 \
            if (st == k)                                                                                               \
            {                                                                                                          \
                OUTPUT = (typeof (OUTPUT)) (offset + st);                                                              \
                break;                                                                                                 \
            }                                                                                                          \
            else if (st > k)                                                                                           \
            {                                                                                                          \
                length = st;                                                                                           \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                offset += st;                                                                                          \
                length -= st;                                                                                          \
                k -= st;                                                                                               \
            }                                                                                                          \
        }                                                                                                              \
    }

KAN_C_HEADER_END
