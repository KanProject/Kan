#pragma once

#include <context_render_backend_system_api.h>

#include <kan/api_common/min_max.h>
#include <kan/context/render_backend_system.h>
#include <kan/error/critical.h>

/// \file
/// \brief Contains utility batch implementation for batching draws for render backend system.
///
/// \par Description
/// \parblock
/// Batching lots of instanced data and submitting on allocation overflow is a common task for 
/// rendering lots of 2d and UI primitives and for some 3d primitives as well. This file provides
/// inline implementation of common batch management and population logic.
/// \endparblock

KAN_C_HEADER_BEGIN

typedef void (*kan_render_backend_draw_batch_submit_function_t) (kan_memory_size_t user_data,
                                                                 kan_render_buffer_t instance_buffer,
                                                                 kan_instance_size_t instance_buffer_offset,
                                                                 kan_instance_size_t instance_count);

/// \brief Contains render instanced draw call batch data.
struct kan_render_backend_draw_batch_t
{
    struct kan_render_allocated_slice_t allocation;
    uint8_t *next;
    uint8_t *allocation_begin;
    uint8_t *run_begin;
    uint8_t *allocation_end;

    kan_instance_size_t instance_size;
    kan_instance_size_t instance_alignment;
    kan_instance_size_t instance_capacity;

    kan_render_frame_lifetime_buffer_allocator_t allocator;
    kan_render_backend_draw_batch_submit_function_t submit;
    kan_memory_size_t submit_user_data;
};

/// \brief Configures new batch for batching draw call data into instances.
/// \details
/// \param allocator Allocator for frame lifetime data.
/// \param instance_size Size of data structure for one instance.
/// \param instance_alignment Alignment of data structure for one instance.
/// \param instance_limit Maximum count of instance in one allocation. Could be left at
///                       `KAN_INT_MAX (kan_instance_size_t)` if batch should use the whole allocator page.
/// \param submit Submission function. We use separate functor as in some cases user needs to execute complex logic
///               like pipeline binding, which might happen when there are custom submission rules, which is the case
///               for UI, for example. When there is no custom submission rules and pipeline is prepared before
///               batching, using just `kan_render_backend_draw_batch_submit_function_trivial` is advised.
/// \param submit_user_data User data for submit function.
static inline struct kan_render_backend_draw_batch_t kan_render_backend_draw_batch_begin (
    kan_render_frame_lifetime_buffer_allocator_t allocator,
    kan_instance_size_t instance_size,
    kan_instance_size_t instance_alignment,
    kan_instance_size_t instance_limit,
    kan_render_backend_draw_batch_submit_function_t submit,
    kan_memory_size_t submit_user_data)
{
    struct kan_render_backend_draw_batch_t batch = {
        .allocation =
            {
                .buffer = KAN_HANDLE_SET_INVALID (kan_render_buffer_t),
                .slice_offset = 0u,
            },

        .next = NULL,
        .allocation_begin = NULL,
        .run_begin = NULL,
        .allocation_end = NULL,

        .instance_size = instance_size,
        .instance_alignment = instance_alignment,
        .instance_capacity = KAN_MIN (
            instance_limit, kan_render_frame_lifetime_buffer_allocator_get_page_size (allocator) / instance_size),

        .allocator = allocator,
        .submit = submit,
        .submit_user_data = submit_user_data,
    };

    return batch;
}

/// \brief Manually submits batch contents if any.
/// \details Should not be called manually in trivial use cases, but is useful for advanced cases.
///          Submission does not discard allocation unless it is fully used, therefore unused part of the allocation
///          will be preserved after submission and used for new instances if any.
static inline void kan_render_backend_draw_batch_submit (struct kan_render_backend_draw_batch_t *batch)
{
    if (!batch->next)
    {
        return;
    }

    batch->submit (batch->submit_user_data, batch->allocation.buffer,
                   batch->allocation.slice_offset + (batch->run_begin - batch->allocation_begin),
                   (batch->next - batch->run_begin) / batch->instance_size);

    if (batch->next >= batch->allocation_end)
    {
        // Allocation is fully used up, forget it.
        batch->next = NULL;
        batch->allocation_begin = NULL;
        batch->run_begin = NULL;
        batch->allocation_end = NULL;
    }
    else
    {
        // Must've been manual submit, we can continue using the allocation.
        batch->run_begin = batch->next;
    }
}

/// \brief Registers new instance and returns pointer to its data for population.
/// \details Executes submission and allocation under the hood if necessary.
static inline void *kan_render_backend_draw_batch_next (struct kan_render_backend_draw_batch_t *batch)
{
    // If full, submit data first.
    if (batch->next >= batch->allocation_end)
    {
        kan_render_backend_draw_batch_submit (batch);
    }

    // If there is no valid allocation, we need make the new one.
    if (!batch->next)
    {
        // Ensure that batch is in empty state right now.
        KAN_ASSERT (!batch->allocation_begin)
        KAN_ASSERT (!batch->run_begin)
        KAN_ASSERT (!batch->allocation_end)

        batch->allocation = kan_render_frame_lifetime_buffer_allocator_allocate (
            batch->allocator, batch->instance_capacity * batch->instance_size, batch->instance_alignment);
        KAN_ASSERT (KAN_HANDLE_IS_VALID (batch->allocation.buffer))

        batch->next = kan_render_buffer_patch (batch->allocation.buffer, batch->allocation.slice_offset,
                                               batch->instance_capacity * batch->instance_size);

        KAN_ASSERT (batch->next)
        batch->allocation_begin = batch->next;
        batch->run_begin = batch->next;
        batch->allocation_end = batch->next + batch->instance_capacity * batch->instance_size;
    }

    void *allocated = batch->next;
    batch->next += batch->instance_size;
    return allocated;
}

/// \brief Finishes the batch lifetime, submits all instances that were not submitted yet.
static inline void kan_render_backend_draw_batch_end (struct kan_render_backend_draw_batch_t *batch)
{
    kan_render_backend_draw_batch_submit (batch);
}

/// \brief User data structure for `kan_render_backend_draw_batch_submit_function_trivial`.
struct kan_render_backend_draw_batch_submit_trivial_user_data_t
{
    kan_render_pass_instance_t pass_instance;
    kan_instance_size_t instance_binding;
    kan_instance_size_t index_count;
};

/// \brief `kan_render_backend_draw_batch_t` submission functor for trivial use cases.
/// \invariant Expects `kan_render_backend_draw_batch_submit_trivial_user_data_t` user data.
static inline void kan_render_backend_draw_batch_submit_function_trivial (kan_memory_size_t user_data,
                                                                          kan_render_buffer_t instance_buffer,
                                                                          kan_instance_size_t instance_buffer_offset,
                                                                          kan_instance_size_t instance_count)
{
    struct kan_render_backend_draw_batch_submit_trivial_user_data_t *data =
        (struct kan_render_backend_draw_batch_submit_trivial_user_data_t *) user_data;
    kan_render_pass_instance_attributes (data->pass_instance, data->instance_binding, 1u, &instance_buffer,
                                         &instance_buffer_offset);
    kan_render_pass_instance_draw (data->pass_instance, 0u, data->index_count, 0u, 0u, instance_count);
}

KAN_C_HEADER_END
