#include <kan/error/critical.h>
#include <kan/math/value_noise.h>

void kan_unorm16_value_noise_2d_init (struct kan_unorm16_value_noise_2d_t *instance)
{
    instance->cell_width = 0u;
    instance->cell_height = 0u;
    instance->cells_x = 0u;
    instance->cells_y = 0u;

    kan_dynamic_array_init (&instance->values, 0u, sizeof (uint16_t), alignof (uint16_t),
                            kan_allocation_group_stack_get ());
}

void kan_unorm16_value_noise_2d_shutdown (struct kan_unorm16_value_noise_2d_t *instance)
{
    kan_dynamic_array_shutdown (&instance->values);
}

void kan_unorm16_value_noise_2d_reset (struct kan_unorm16_value_noise_2d_t *instance)
{
    instance->cell_width = 0u;
    instance->cell_height = 0u;
    instance->cells_x = 0u;
    instance->cells_y = 0u;
    instance->values.size = 0u;
    kan_dynamic_array_set_capacity (&instance->values, 0u);
}

void kan_unorm16_value_noise_2d_build (struct kan_unorm16_value_noise_2d_t *instance,
                                       struct kan_random_xoshiro_t *generator,
                                       kan_instance_size_t width,
                                       kan_instance_size_t height,
                                       kan_instance_size_t cell_width,
                                       kan_instance_size_t cell_height)
{
    KAN_ASSERT (cell_width > 0u)
    KAN_ASSERT (cell_width <= UINT16_MAX)
    KAN_ASSERT (cell_height > 0u)
    KAN_ASSERT (cell_height <= UINT16_MAX)
    instance->cell_width = cell_width;
    instance->cell_height = cell_height;
    instance->cells_x = width / cell_width + 1u + (width % cell_width > 0u ? 1u : 0u);
    instance->cells_y = height / cell_height + 1u + (height % cell_height > 0u ? 1u : 0u);
    instance->values.size = 0u;

    kan_dynamic_array_set_capacity (&instance->values, instance->cells_x * instance->cells_y);
    instance->values.size = instance->values.capacity;

    for (kan_memory_size_t index = 0u; index < instance->values.size; ++index)
    {
        // We use most significant bits here as they are usually considered better distributed than the least
        // significant bits, and doing things like that is an advised behavior by the author of xoshiro.
        ((uint16_t *) instance->values.data)[index] = (uint16_t) (kan_random_xoshiro_next (generator) >> 16u);
    }
}

static inline kan_instance_size_t unorm16_value_noise_lerp (kan_instance_size_t value_0,
                                                            kan_instance_size_t value_1,
                                                            kan_instance_size_t offset,
                                                            kan_instance_size_t cell)
{
    // As offset < cell and cell <= UINT16_MAX and `kan_instance_size_t` is always at least 32 bits,
    // this should never overflow.
    return value_0 * (cell - offset) / cell + value_1 * offset / cell;
}

kan_instance_size_t kan_unorm16_value_noise_2d_sample (const struct kan_unorm16_value_noise_2d_t *instance,
                                                       kan_instance_size_t x,
                                                       kan_instance_size_t y)
{
    if (instance->cell_width == 0u || instance->cell_height == 0u)
    {
        return 0u;
    }

    const kan_instance_size_t base_x = x / instance->cell_width;
    const kan_instance_size_t base_y = y / instance->cell_height;

    if (base_x + 1u >= instance->cells_x || base_y + 1u >= instance->cells_y)
    {
        return 0u;
    }

    const kan_instance_size_t offset_x = x % instance->cell_width;
    const kan_instance_size_t offset_y = y % instance->cell_height;

    const kan_instance_size_t value_00 = ((uint16_t *) instance->values.data)[instance->cells_x * base_y + base_x];
    const kan_instance_size_t value_10 = ((uint16_t *) instance->values.data)[instance->cells_x * base_y + base_x + 1u];

    const kan_instance_size_t value_01 =
        ((uint16_t *) instance->values.data)[instance->cells_x * (base_y + 1u) + base_x];
    const kan_instance_size_t value_11 =
        ((uint16_t *) instance->values.data)[instance->cells_x * (base_y + 1u) + base_x + 1u];

    const kan_instance_size_t lerp_x_0 = unorm16_value_noise_lerp (value_00, value_10, offset_x, instance->cell_width);
    const kan_instance_size_t lerp_x_1 = unorm16_value_noise_lerp (value_01, value_11, offset_x, instance->cell_width);
    const kan_instance_size_t lerp_y = unorm16_value_noise_lerp (lerp_x_0, lerp_x_1, offset_y, instance->cell_height);

    KAN_ASSERT (lerp_y <= UINT16_MAX)
    return lerp_y;
}
