#include <kan/error/critical.h>
#include <kan/math/value_noise.h>

void kan_floating_grid_value_noise_2d_init (struct kan_floating_grid_value_noise_2d_t *instance)
{
    instance->cell_width = 0u;
    instance->cell_height = 0u;
    instance->cells_x = 0u;
    instance->cells_y = 0u;

    kan_dynamic_array_init (&instance->values, 0u, sizeof (kan_floating_t), alignof (kan_floating_t),
                            kan_allocation_group_stack_get ());
}

void kan_floating_grid_value_noise_2d_shutdown (struct kan_floating_grid_value_noise_2d_t *instance)
{
    kan_dynamic_array_shutdown (&instance->values);
}

void kan_floating_grid_value_noise_2d_reset (struct kan_floating_grid_value_noise_2d_t *instance)
{
    instance->cell_width = 0u;
    instance->cell_height = 0u;
    instance->cells_x = 0u;
    instance->cells_y = 0u;
    instance->values.size = 0u;
    kan_dynamic_array_set_capacity (&instance->values, 0u);
}

void kan_floating_grid_value_noise_2d_build (struct kan_floating_grid_value_noise_2d_t *instance,
                                             struct kan_random_xoshiro_t *generator,
                                             kan_instance_size_t width,
                                             kan_instance_size_t height,
                                             kan_instance_size_t cell_width,
                                             kan_instance_size_t cell_height,
                                             kan_floating_t min,
                                             kan_floating_t max)
{
    instance->cell_width = cell_width;
    instance->cell_height = cell_height;
    instance->cells_x = width / cell_width + 1u + (width % cell_width > 0u ? 1u : 0u);
    instance->cells_y = height / cell_height + 1u + (height % cell_height > 0u ? 1u : 0u);
    instance->values.size = 0u;

    kan_dynamic_array_set_capacity (&instance->values, instance->cells_x * instance->cells_y);
    instance->values.size = instance->values.capacity;

    for (kan_memory_size_t index = 0u; index < instance->values.size; ++index)
    {
        ((kan_floating_t *) instance->values.data)[index] =
            min + kan_random_xoshiro_next_floating (generator) * (max - min);
    }
}

kan_floating_t kan_floating_grid_value_noise_2d_sample (const struct kan_floating_grid_value_noise_2d_t *instance,
                                                        kan_instance_size_t x,
                                                        kan_instance_size_t y)
{
    if (instance->cell_width == 0u || instance->cell_height == 0u)
    {
        return 0.0f;
    }

    const kan_instance_size_t base_x = x / instance->cell_width;
    const kan_instance_size_t base_y = y / instance->cell_height;

    if (base_x + 1u >= instance->cells_x || base_y + 1u >= instance->cells_y)
    {
        return 0.0f;
    }

    const kan_instance_size_t offset_x = x % instance->cell_width;
    const kan_instance_size_t offset_y = y % instance->cell_height;

    const kan_floating_t value_00 = ((kan_floating_t *) instance->values.data)[instance->cells_x * base_y + base_x];
    const kan_floating_t value_10 =
        ((kan_floating_t *) instance->values.data)[instance->cells_x * base_y + base_x + 1u];

    const kan_floating_t value_01 =
        ((kan_floating_t *) instance->values.data)[instance->cells_x * (base_y + 1u) + base_x];
    const kan_floating_t value_11 =
        ((kan_floating_t *) instance->values.data)[instance->cells_x * (base_y + 1u) + base_x + 1u];

    const kan_floating_t lerp_x_0 =
        kan_float_lerp (value_00, value_10, (float) offset_x / (float) instance->cell_width);
    const kan_floating_t lerp_x_1 =
        kan_float_lerp (value_01, value_11, (float) offset_x / (float) instance->cell_width);
    const kan_floating_t lerp_y = kan_float_lerp (lerp_x_0, lerp_x_1, (float) offset_y / (float) instance->cell_height);
    return lerp_y;
}
