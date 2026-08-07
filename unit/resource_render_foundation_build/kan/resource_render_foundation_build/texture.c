#define _CRT_SECURE_NO_WARNINGS __CUSHION_PRESERVE__

#include <string.h>

#include <kan/api_common/min_max.h>
#include <kan/file_system/stream.h>
#include <kan/image/image.h>
#include <kan/log/logging.h>
#include <kan/math/inline.h>
#include <kan/memory/allocation.h>
#include <kan/resource_pipeline/meta.h>
#include <kan/resource_render_foundation_build/texture.h>
#include <kan/stream/random_access_stream_buffer.h>

KAN_LOG_DEFINE_CATEGORY (resource_render_foundation_texture);
KAN_USE_STATIC_INTERNED_IDS

void kan_resource_texture_platform_configuration_init (struct kan_resource_texture_platform_configuration_t *instance)
{
    kan_dynamic_array_init (&instance->supported_formats, 0u, sizeof (enum kan_resource_texture_format_t),
                            alignof (enum kan_resource_texture_format_t), kan_allocation_group_stack_get ());
}

void kan_resource_texture_platform_configuration_shutdown (
    struct kan_resource_texture_platform_configuration_t *instance)
{
    kan_dynamic_array_shutdown (&instance->supported_formats);
}

KAN_REFLECTION_STRUCT_META (kan_resource_texture_build_preset_t)
RESOURCE_RENDER_FOUNDATION_BUILD_API struct kan_resource_type_meta_t kan_resource_texture_build_preset_resource_type = {
    .flags = 0u,
    .version = CUSHION_START_NS_X64,
    .move = NULL,
    .reset = NULL,
};

void kan_resource_texture_build_preset_init (struct kan_resource_texture_build_preset_t *instance)
{
    instance->mip_generation = KAN_RESOURCE_TEXTURE_MIP_GENERATION_AVERAGE;
    instance->streamed_mips = 0u;
    instance->inlined_mips = 1u;
    kan_dynamic_array_init (&instance->supported_target_formats, 0u, sizeof (enum kan_resource_texture_format_t),
                            alignof (enum kan_resource_texture_format_t), kan_allocation_group_stack_get ());
}

void kan_resource_texture_build_preset_shutdown (struct kan_resource_texture_build_preset_t *instance)
{
    kan_dynamic_array_shutdown (&instance->supported_target_formats);
}

KAN_REFLECTION_STRUCT_META (kan_resource_texture_header_t)
RESOURCE_RENDER_FOUNDATION_BUILD_API struct kan_resource_type_meta_t kan_resource_texture_header_resource_type = {
    .flags = 0u,
    .version = CUSHION_START_NS_X64,
    .move = NULL,
    .reset = NULL,
};

KAN_REFLECTION_STRUCT_FIELD_META (kan_resource_texture_header_t, preset)
RESOURCE_RENDER_FOUNDATION_BUILD_API struct kan_resource_reference_meta_t kan_resource_texture_header_reference_preset =
    {
        .type_name = "kan_resource_texture_build_preset_t",
        .flags = 0u,
};

KAN_REFLECTION_STRUCT_FIELD_META (kan_resource_texture_header_t, image)
RESOURCE_RENDER_FOUNDATION_BUILD_API struct kan_resource_reference_meta_t kan_resource_texture_header_reference_image =
    {
        .type_name = NULL,
        .flags = 0u,
};

void kan_resource_texture_header_init (struct kan_resource_texture_header_t *instance)
{
    instance->preset = NULL;
    instance->image_class = KAN_RESOURCE_TEXTURE_IMAGE_CLASS_COLOR_SRGB;
    instance->image = NULL;
}

static enum kan_resource_build_rule_result_t texture_build (struct kan_resource_build_rule_context_t *context);

KAN_REFLECTION_STRUCT_META (kan_resource_texture_t)
RESOURCE_RENDER_FOUNDATION_BUILD_API struct kan_resource_build_rule_t kan_resource_texture_build_rule = {
    .primary_input_type = "kan_resource_texture_header_t",
    .platform_configuration_type = "kan_resource_texture_platform_configuration_t",
    .secondary_types_count = 1u,
    .secondary_types = (const char *[]) {"kan_resource_texture_build_preset_t"},
    .functor = texture_build,
    .version = CUSHION_START_NS_X64,
};

static void free_transitive_mip_data (kan_floating_t **image_mips,
                                      kan_instance_size_t channels,
                                      struct kan_resource_texture_t *output,
                                      kan_allocation_group_t mips_allocation_group)
{
    for (kan_instance_size_t mip = 0u; mip < output->streamed_mips + output->inlined_mips; ++mip)
    {
        const kan_instance_size_t width = output->width >> mip;
        const kan_instance_size_t height = output->height >> mip;
        kan_free_general (mips_allocation_group, image_mips[mip], width * height * channels * sizeof (kan_floating_t));
    }

    kan_free_general (mips_allocation_group, image_mips,
                      sizeof (kan_floating_t *) * (output->streamed_mips + output->inlined_mips));
}

static enum kan_resource_build_rule_result_t texture_build (struct kan_resource_build_rule_context_t *context)
{
    const struct kan_resource_texture_header_t *input = context->primary_input;
    struct kan_resource_texture_t *output = context->primary_output;
    const struct kan_resource_texture_platform_configuration_t *configuration = context->platform_configuration;

    if (!input->preset)
    {
        KAN_LOG (resource_render_foundation_texture, KAN_LOG_ERROR,
                 "Texture header \"%s\" has no specified preset and therefore cannot be built.", context->primary_name)
        return KAN_RESOURCE_BUILD_RULE_FAILURE;
    }

    if (!input->image)
    {
        KAN_LOG (resource_render_foundation_texture, KAN_LOG_ERROR,
                 "Texture header \"%s\" has no specified image and therefore cannot be built.", context->primary_name)
        return KAN_RESOURCE_BUILD_RULE_FAILURE;
    }

    kan_static_interned_ids_ensure_initialized ();
    const struct kan_resource_texture_build_preset_t *preset = NULL;
    const char *image_path = NULL;

    struct kan_resource_build_rule_secondary_node_t *secondary_node = context->secondary_input_first;
    while (secondary_node)
    {
        if (secondary_node->type == KAN_STATIC_INTERNED_ID_GET (kan_resource_texture_build_preset_t) &&
            secondary_node->name == input->preset)
        {
            preset = secondary_node->data;
        }
        else if (!secondary_node->type && secondary_node->name == input->image)
        {
            image_path = secondary_node->third_party_path;
        }

        secondary_node = secondary_node->next;
    }

    // Should never happen.
    KAN_ASSERT (preset)
    KAN_ASSERT (image_path)

    // We use floating point numbers as we would still need to use them in lots of cases, and it is better to avoid
    // compressing transitive data back-and-forth from float and to float. For example, to properly generate averaged
    // mips for srgb-encoded data we need to transfer it to rgb first and then to srgb back if target format is srgb,
    // which is usually the case.
    kan_floating_t **image_mips = NULL;
    kan_instance_size_t image_channels = 0u;

    kan_allocation_group_t main_allocation_group =
        kan_allocation_group_get_child (kan_allocation_group_root (), "resource_render_foundation_texture_build");
    kan_allocation_group_t mips_allocation_group = kan_allocation_group_get_child (main_allocation_group, "mips");

    // Pre-initialize mip count.
    output->streamed_mips = preset->streamed_mips;
    output->inlined_mips = preset->inlined_mips;
    kan_instance_size_t total_mips = output->streamed_mips + output->inlined_mips;

    // Scope for defers.
    {
        struct kan_stream_t *image_load_stream = kan_direct_file_stream_open_for_read (image_path, true);
        if (!image_load_stream)
        {
            KAN_LOG (resource_render_foundation_texture, KAN_LOG_ERROR,
                     "Failed to open image at \"%s\" for texture header \"%s\".", context->primary_third_party_path,
                     context->primary_name)
            return KAN_RESOURCE_BUILD_RULE_FAILURE;
        }

        image_load_stream =
            kan_random_access_stream_buffer_open_for_read (image_load_stream, KAN_RESOURCE_RF_TEXTURE_LOAD_BUFFER);
        CUSHION_DEFER { image_load_stream->operations->close (image_load_stream); }

        struct kan_image_raw_data_t image_data;
        kan_image_raw_data_init (&image_data);
        CUSHION_DEFER { kan_image_raw_data_shutdown (&image_data); }

        if (!kan_image_load (image_load_stream, &image_data))
        {
            KAN_LOG (resource_render_foundation_texture, KAN_LOG_ERROR,
                     "Failed to load image at \"%s\" for texture header \"%s\".", context->primary_third_party_path,
                     context->primary_name)
            return KAN_RESOURCE_BUILD_RULE_FAILURE;
        }

        output->width = image_data.width;
        output->height = image_data.height;

        // Adjust mip count to image size.
        kan_instance_size_t found_mips = 0u;
        kan_instance_size_t check_width = output->width;
        kan_instance_size_t check_height = output->height;

        while (found_mips < total_mips)
        {
            check_width >>= 1u;
            check_height >>= 1u;

            if (check_width == 0u || check_height == 0u)
            {
                // Cannot use more mips. Need to remove excessive mips.
                total_mips = found_mips;

                if (output->streamed_mips >= total_mips)
                {
                    output->streamed_mips = total_mips - 1u;
                    output->inlined_mips = 1u;
                }
                else
                {
                    output->inlined_mips = total_mips - output->streamed_mips;
                }

                break;
            }

            ++found_mips;
        }

        // Calculate decoded channel count.
        switch (input->image_class)
        {
        case KAN_RESOURCE_TEXTURE_IMAGE_CLASS_COLOR_SRGB:
        case KAN_RESOURCE_TEXTURE_IMAGE_CLASS_COLOR_RGB:
            image_channels = 4u;
            break;

        case KAN_RESOURCE_TEXTURE_IMAGE_CLASS_DEPTH_GRAYSCALE:
        case KAN_RESOURCE_TEXTURE_IMAGE_CLASS_DEPTH_FLOAT_32:
            image_channels = 1u;
            break;
        }

        // Allocate data arrays per mip.
        image_mips = kan_allocate_general (mips_allocation_group, sizeof (kan_floating_t *) * total_mips,
                                           alignof (kan_floating_t *));

        for (kan_memory_size_t mip = 0u; mip < (kan_memory_size_t) total_mips; ++mip)
        {
            const kan_instance_size_t width = output->width >> mip;
            KAN_ASSERT (width > 0u)
            const kan_instance_size_t height = output->height >> mip;
            KAN_ASSERT (height > 0u)
            image_mips[mip] =
                kan_allocate_general (mips_allocation_group, width * height * image_channels * sizeof (kan_floating_t),
                                      alignof (kan_floating_t));
        }

        // Properly decode first mip from image data.
        const kan_memory_size_t source_pixel_count = image_data.width * image_data.height;
        const uint8_t *source_pixel = image_data.data;
        kan_floating_t *target_pixel = image_mips[0u];

        switch (input->image_class)
        {
        case KAN_RESOURCE_TEXTURE_IMAGE_CLASS_COLOR_SRGB:
            for (kan_memory_size_t pixel_index = 0u; pixel_index < source_pixel_count;
                 ++pixel_index, source_pixel += sizeof (uint32_t), target_pixel += 4u)
            {
                target_pixel[0u] = kan_color_transfer_srgb_to_rgb ((kan_floating_t) source_pixel[0u] / 255.0f);
                target_pixel[1u] = kan_color_transfer_srgb_to_rgb ((kan_floating_t) source_pixel[1u] / 255.0f);
                target_pixel[2u] = kan_color_transfer_srgb_to_rgb ((kan_floating_t) source_pixel[2u] / 255.0f);
                // Alpha is not in sRGB color space.
                target_pixel[3u] = (kan_floating_t) source_pixel[3u] / 255.0f;
            }

            break;

        case KAN_RESOURCE_TEXTURE_IMAGE_CLASS_COLOR_RGB:
            for (kan_memory_size_t pixel_index = 0u; pixel_index < source_pixel_count;
                 ++pixel_index, source_pixel += sizeof (uint32_t), target_pixel += 4u)
            {
                target_pixel[0u] = (kan_floating_t) source_pixel[0u] / 255.0f;
                target_pixel[1u] = (kan_floating_t) source_pixel[1u] / 255.0f;
                target_pixel[2u] = (kan_floating_t) source_pixel[2u] / 255.0f;
                target_pixel[3u] = (kan_floating_t) source_pixel[3u] / 255.0f;
            }

            break;

        case KAN_RESOURCE_TEXTURE_IMAGE_CLASS_DEPTH_GRAYSCALE:
            for (kan_memory_size_t pixel_index = 0u; pixel_index < source_pixel_count;
                 ++pixel_index, source_pixel += sizeof (uint32_t), ++target_pixel)
            {
                *target_pixel = (kan_floating_t) source_pixel[0u] / 255.0f;
            }

            break;

        case KAN_RESOURCE_TEXTURE_IMAGE_CLASS_DEPTH_FLOAT_32:
            for (kan_memory_size_t pixel_index = 0u; pixel_index < source_pixel_count;
                 ++pixel_index, source_pixel += sizeof (uint32_t), ++target_pixel)
            {
                *target_pixel = *(kan_floating_t *) source_pixel;
            }

            break;
        }
    }

    CUSHION_DEFER { free_transitive_mip_data (image_mips, image_channels, output, mips_allocation_group); };
    for (kan_memory_size_t next_mip = 1u; next_mip < total_mips; ++next_mip)
    {
        const kan_instance_size_t source_width = output->width >> (next_mip - 1u);
        KAN_ASSERT (source_width > 0u)
        const kan_instance_size_t source_height = output->height >> (next_mip - 1u);
        KAN_ASSERT (source_height > 0u)

        const kan_instance_size_t mip_width = output->width >> next_mip;
        KAN_ASSERT (mip_width > 0u)
        const kan_instance_size_t mip_height = output->height >> next_mip;
        KAN_ASSERT (mip_height > 0u)

        const kan_floating_t *source_data = image_mips[next_mip - 1u];
        kan_floating_t *target_data = image_mips[next_mip];

        for (kan_memory_size_t y = 0u; y < (kan_memory_size_t) mip_height; ++y)
        {
            for (kan_memory_size_t x = 0u; x < (kan_memory_size_t) mip_width; ++x)
            {
                kan_floating_t *mip_pixel = target_data + image_channels * (y * mip_width + x);
                for (kan_memory_size_t channel = 0u; channel < image_channels; ++channel)
                {
                    switch (preset->mip_generation)
                    {
                    case KAN_RESOURCE_TEXTURE_MIP_GENERATION_AVERAGE:
                        mip_pixel[channel] = 0.0f;
                        break;

                    case KAN_RESOURCE_TEXTURE_MIP_GENERATION_MIN:
                        mip_pixel[channel] = FLT_MAX;
                        break;

                    case KAN_RESOURCE_TEXTURE_MIP_GENERATION_MAX:
                        mip_pixel[channel] = FLT_MIN;
                        break;
                    }
                }

                kan_memory_size_t average_samples = 0u;
                for (kan_memory_size_t offset_y = 0u; offset_y < 2u; ++offset_y)
                {
                    for (kan_memory_size_t offset_x = 0u; offset_x < 2u; ++offset_x)
                    {
                        const kan_memory_size_t sample_x = x * 2u + offset_x;
                        const kan_memory_size_t sample_y = y * 2u + offset_y;

                        if (sample_x < source_width && sample_y < source_height)
                        {
                            const kan_floating_t *sample_pixel =
                                source_data + image_channels * (sample_y * source_width + sample_x);
                            ++average_samples;

                            for (kan_memory_size_t channel = 0u; channel < image_channels; ++channel)
                            {
                                switch (preset->mip_generation)
                                {
                                case KAN_RESOURCE_TEXTURE_MIP_GENERATION_AVERAGE:
                                    mip_pixel[channel] += sample_pixel[channel];
                                    break;

                                case KAN_RESOURCE_TEXTURE_MIP_GENERATION_MIN:
                                    mip_pixel[channel] = KAN_MIN (mip_pixel[channel], sample_pixel[channel]);
                                    break;

                                case KAN_RESOURCE_TEXTURE_MIP_GENERATION_MAX:
                                    mip_pixel[channel] = KAN_MAX (mip_pixel[channel], sample_pixel[channel]);
                                    break;
                                }
                            }
                        }
                    }
                }

                switch (preset->mip_generation)
                {
                case KAN_RESOURCE_TEXTURE_MIP_GENERATION_AVERAGE:
                {
                    KAN_ASSERT (average_samples > 0u)
                    const kan_floating_t average_modifier = 1.0f / (kan_floating_t) average_samples;
                    for (kan_memory_size_t channel = 0u; channel < image_channels; ++channel)
                    {
                        mip_pixel[channel] *= average_modifier;
                    }

                    break;
                }

                case KAN_RESOURCE_TEXTURE_MIP_GENERATION_MIN:
                case KAN_RESOURCE_TEXTURE_MIP_GENERATION_MAX:
                    break;
                }
            }
        }
    }

    struct kan_resource_streamed_texture_data_t streamed_texture_data;
    kan_allocation_group_stack_push (main_allocation_group);
    kan_resource_streamed_texture_data_init (&streamed_texture_data);
    kan_allocation_group_stack_pop ();

    CUSHION_DEFER { kan_resource_streamed_texture_data_shutdown (&streamed_texture_data); }
    bool conversion_successful = true;
    kan_dynamic_array_set_capacity (&output->formats, preset->supported_target_formats.size);

    for (kan_memory_size_t format_index = 0u; format_index < preset->supported_target_formats.size; ++format_index)
    {
        enum kan_resource_texture_format_t format =
            ((enum kan_resource_texture_format_t *) preset->supported_target_formats.data)[format_index];
        bool supported_by_platform = false;

        for (kan_memory_size_t configuration_index = 0u;
             configuration_index < (kan_memory_size_t) configuration->supported_formats.size; ++configuration_index)
        {
            if (format ==
                ((enum kan_resource_texture_format_t *) configuration->supported_formats.data)[configuration_index])
            {
                supported_by_platform = true;
                break;
            }
        }

        if (!supported_by_platform)
        {
            continue;
        }

        // Ensure that target format can be achieved from input image class at all.
        bool format_can_be_targeted = true;

        switch (format)
        {
        case KAN_RESOURCE_TEXTURE_FORMAT_UNCOMPRESSED_R8_SRGB:
        case KAN_RESOURCE_TEXTURE_FORMAT_UNCOMPRESSED_RG16_SRGB:
        case KAN_RESOURCE_TEXTURE_FORMAT_UNCOMPRESSED_RGBA32_SRGB:
        case KAN_RESOURCE_TEXTURE_FORMAT_UNCOMPRESSED_R8_UNORM:
        case KAN_RESOURCE_TEXTURE_FORMAT_UNCOMPRESSED_RG16_UNORM:
        case KAN_RESOURCE_TEXTURE_FORMAT_UNCOMPRESSED_RGBA32_UNORM:
            switch (input->image_class)
            {
            case KAN_RESOURCE_TEXTURE_IMAGE_CLASS_COLOR_SRGB:
            case KAN_RESOURCE_TEXTURE_IMAGE_CLASS_COLOR_RGB:
                KAN_ASSERT (image_channels == 4u)
                break;

            case KAN_RESOURCE_TEXTURE_IMAGE_CLASS_DEPTH_GRAYSCALE:
            case KAN_RESOURCE_TEXTURE_IMAGE_CLASS_DEPTH_FLOAT_32:
                KAN_LOG (resource_render_foundation_texture, KAN_LOG_ERROR,
                         "Texture header \"%s\" declares depth texture, but preset \"%s\" lists color format among "
                         "target formats.",
                         context->primary_name, input->preset)
                format_can_be_targeted = false;
                break;
            }

            break;

        case KAN_RESOURCE_TEXTURE_FORMAT_UNCOMPRESSED_D16:
        case KAN_RESOURCE_TEXTURE_FORMAT_UNCOMPRESSED_D32:
            switch (input->image_class)
            {
            case KAN_RESOURCE_TEXTURE_IMAGE_CLASS_COLOR_SRGB:
            case KAN_RESOURCE_TEXTURE_IMAGE_CLASS_COLOR_RGB:
                KAN_LOG (resource_render_foundation_texture, KAN_LOG_ERROR,
                         "Texture header \"%s\" declares color texture, but preset \"%s\" lists depth format among "
                         "target formats.",
                         context->primary_name, input->preset)
                format_can_be_targeted = false;
                break;

            case KAN_RESOURCE_TEXTURE_IMAGE_CLASS_DEPTH_GRAYSCALE:
            case KAN_RESOURCE_TEXTURE_IMAGE_CLASS_DEPTH_FLOAT_32:
                KAN_ASSERT (image_channels == 1u)
                break;
            }

            break;
        }

        if (!format_can_be_targeted)
        {
            conversion_successful = false;
            continue;
        }

        struct kan_resource_texture_format_item_t *item = kan_dynamic_array_add_last (&output->formats);
        KAN_ASSERT (item)

        kan_allocation_group_stack_push (output->formats.allocation_group);
        kan_resource_texture_format_item_init (item);
        kan_allocation_group_stack_pop ();

        item->format = format;
        kan_dynamic_array_set_capacity (&item->streamed_mips, output->streamed_mips);
        kan_dynamic_array_set_capacity (&item->inlined_mips, output->inlined_mips);

        for (kan_memory_size_t mip = 0u; mip < (kan_memory_size_t) total_mips; ++mip)
        {
            const kan_instance_size_t width = output->width >> mip;
            const kan_instance_size_t height = output->height >> mip;
            const kan_memory_size_t source_pixel_count = width * height;
            const char *target_format_name = "unknown";
            struct kan_dynamic_array_t *target_output_array;

            if (mip < output->streamed_mips)
            {
                target_output_array = &streamed_texture_data.data;
            }
            else
            {
                struct kan_resource_inlined_texture_data_t *inlined = kan_dynamic_array_add_last (&item->inlined_mips);
                kan_allocation_group_stack_push (item->inlined_mips.allocation_group);
                kan_resource_inlined_texture_data_init (inlined);
                kan_allocation_group_stack_pop ();
                target_output_array = &inlined->data;
            }

#define CLAMPED_UINT_COLOR(VALUE) (uint8_t) (255.0f * KAN_CLAMP (VALUE, 0.0f, 1.0f))
            switch (format)
            {
            case KAN_RESOURCE_TEXTURE_FORMAT_UNCOMPRESSED_R8_SRGB:
            {
                target_format_name = "r_srgb";
                kan_dynamic_array_set_capacity (target_output_array, sizeof (uint8_t) * width * height);
                target_output_array->size = target_output_array->capacity;
                const kan_floating_t *source_pixel = image_mips[mip];
                uint8_t *target_pixel = target_output_array->data;

                for (kan_memory_size_t pixel_index = 0u; pixel_index < source_pixel_count;
                     ++pixel_index, source_pixel += 4u, ++target_pixel)
                {
                    target_pixel[0u] = CLAMPED_UINT_COLOR (kan_color_transfer_rgb_to_srgb (source_pixel[0u]));
                }

                break;
            }

            case KAN_RESOURCE_TEXTURE_FORMAT_UNCOMPRESSED_RG16_SRGB:
            {
                target_format_name = "rg_srgb";
                kan_dynamic_array_set_capacity (target_output_array, sizeof (uint16_t) * width * height);
                target_output_array->size = target_output_array->capacity;
                const kan_floating_t *source_pixel = image_mips[mip];
                uint8_t *target_pixel = target_output_array->data;

                for (kan_memory_size_t pixel_index = 0u; pixel_index < source_pixel_count;
                     ++pixel_index, source_pixel += 4u, target_pixel += 2u)
                {
                    target_pixel[0u] = CLAMPED_UINT_COLOR (kan_color_transfer_rgb_to_srgb (source_pixel[0u]));
                    target_pixel[1u] = CLAMPED_UINT_COLOR (kan_color_transfer_rgb_to_srgb (source_pixel[1u]));
                }

                break;
            }

            case KAN_RESOURCE_TEXTURE_FORMAT_UNCOMPRESSED_RGBA32_SRGB:
            {
                target_format_name = "rgba_srgb";
                kan_dynamic_array_set_capacity (target_output_array, sizeof (uint32_t) * width * height);
                target_output_array->size = target_output_array->capacity;
                const kan_floating_t *source_pixel = image_mips[mip];
                uint8_t *target_pixel = target_output_array->data;

                for (kan_memory_size_t pixel_index = 0u; pixel_index < source_pixel_count;
                     ++pixel_index, source_pixel += 4u, target_pixel += 4u)
                {
                    target_pixel[0u] = CLAMPED_UINT_COLOR (kan_color_transfer_rgb_to_srgb (source_pixel[0u]));
                    target_pixel[1u] = CLAMPED_UINT_COLOR (kan_color_transfer_rgb_to_srgb (source_pixel[1u]));
                    target_pixel[2u] = CLAMPED_UINT_COLOR (kan_color_transfer_rgb_to_srgb (source_pixel[2u]));
                    // Alpha is not in sRGB color space.
                    target_pixel[3u] = CLAMPED_UINT_COLOR (source_pixel[3u]);
                }

                break;
            }

            case KAN_RESOURCE_TEXTURE_FORMAT_UNCOMPRESSED_R8_UNORM:
            {
                target_format_name = "r_unorm";
                kan_dynamic_array_set_capacity (target_output_array, sizeof (uint8_t) * width * height);
                target_output_array->size = target_output_array->capacity;
                const kan_floating_t *source_pixel = image_mips[mip];
                uint8_t *target_pixel = target_output_array->data;

                for (kan_memory_size_t pixel_index = 0u; pixel_index < source_pixel_count;
                     ++pixel_index, source_pixel += 4u, ++target_pixel)
                {
                    target_pixel[0u] = CLAMPED_UINT_COLOR (source_pixel[0u]);
                }

                break;
            }

            case KAN_RESOURCE_TEXTURE_FORMAT_UNCOMPRESSED_RG16_UNORM:
            {
                target_format_name = "rg_unorm";
                kan_dynamic_array_set_capacity (target_output_array, sizeof (uint16_t) * width * height);
                target_output_array->size = target_output_array->capacity;
                const kan_floating_t *source_pixel = image_mips[mip];
                uint8_t *target_pixel = target_output_array->data;

                for (kan_memory_size_t pixel_index = 0u; pixel_index < source_pixel_count;
                     ++pixel_index, source_pixel += 4u, target_pixel += 2u)
                {
                    target_pixel[0u] = CLAMPED_UINT_COLOR (source_pixel[0u]);
                    target_pixel[1u] = CLAMPED_UINT_COLOR (source_pixel[1u]);
                }

                break;
            }

            case KAN_RESOURCE_TEXTURE_FORMAT_UNCOMPRESSED_RGBA32_UNORM:
            {
                target_format_name = "rgba_unorm";
                kan_dynamic_array_set_capacity (target_output_array, sizeof (uint32_t) * width * height);
                target_output_array->size = target_output_array->capacity;
                const kan_floating_t *source_pixel = image_mips[mip];
                uint8_t *target_pixel = target_output_array->data;

                for (kan_memory_size_t pixel_index = 0u; pixel_index < source_pixel_count;
                     ++pixel_index, source_pixel += 4u, target_pixel += 4u)
                {
                    target_pixel[0u] = CLAMPED_UINT_COLOR (source_pixel[0u]);
                    target_pixel[1u] = CLAMPED_UINT_COLOR (source_pixel[1u]);
                    target_pixel[2u] = CLAMPED_UINT_COLOR (source_pixel[2u]);
                    target_pixel[3u] = CLAMPED_UINT_COLOR (source_pixel[3u]);
                }

                break;
            }

            case KAN_RESOURCE_TEXTURE_FORMAT_UNCOMPRESSED_D16:
            {
                target_format_name = "d16";
                kan_dynamic_array_set_capacity (target_output_array, sizeof (uint16_t) * width * height);
                target_output_array->size = target_output_array->capacity;
                const kan_floating_t *source_pixel = image_mips[mip];
                uint16_t *target_pixel = (uint16_t *) target_output_array->data;

                for (kan_memory_size_t pixel_index = 0u; pixel_index < source_pixel_count;
                     ++pixel_index, ++source_pixel, ++target_pixel)
                {
                    *target_pixel =
                        (uint16_t) (KAN_CLAMP (*source_pixel, 0.0f, 1.0f) * (kan_floating_t) KAN_INT_MAX (uint16_t));
                }

                break;
            }

            case KAN_RESOURCE_TEXTURE_FORMAT_UNCOMPRESSED_D32:
            {
                target_format_name = "d32";
                kan_dynamic_array_set_capacity (target_output_array, sizeof (kan_floating_t) * width * height);
                target_output_array->size = target_output_array->capacity;
                memcpy (target_output_array->data, image_mips[mip], sizeof (kan_floating_t) * width * height);
                break;
            }
            }
#undef CLAMPED_COLOR

            if (mip < output->streamed_mips)
            {
                char name_buffer[KAN_RESOURCE_RF_TEXTURE_DATA_MAX_NAME_LENGTH];
                snprintf (name_buffer, sizeof (name_buffer), "%s_%s_mip_%u", context->primary_name, target_format_name,
                          (unsigned int) mip);

                kan_interned_string_t *spot = kan_dynamic_array_add_last (&item->streamed_mips);
                *spot = kan_string_intern (name_buffer);

                if (!context->produce_native_secondary_output (
                        context->interface, KAN_STATIC_INTERNED_ID_GET (kan_resource_streamed_texture_data_t), *spot,
                        &streamed_texture_data))
                {
                    KAN_LOG (resource_render_foundation_texture, KAN_LOG_ERROR,
                             "Failed to produce data resource \"%s\" for texture \"%s\".", *spot, context->primary_name)
                    conversion_successful = false;
                }
            }
        }
    }

    kan_dynamic_array_set_capacity (&output->formats, output->formats.size);
    if (output->formats.size == 0u)
    {
        KAN_LOG (resource_render_foundation_texture, KAN_LOG_ERROR,
                 "Failed to build texture \"%s\" with preset \"%s\": no supported formats found.",
                 context->primary_name, input->preset)
        conversion_successful = false;
    }

    return conversion_successful ? KAN_RESOURCE_BUILD_RULE_SUCCESS : KAN_RESOURCE_BUILD_RULE_FAILURE;
}
