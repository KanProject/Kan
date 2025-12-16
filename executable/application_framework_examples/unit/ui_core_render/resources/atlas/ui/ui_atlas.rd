//! kan_resource_atlas_header_t
page_width = 512
page_height = 512

border_size = 1
border_linear_r = 0.0
border_linear_g = 0.0
border_linear_b = 0.0
border_linear_a = 0.0

+entries {
    name = button_negative
    image {
        source = "colorless_button.png"
        type = KAN_RESOURCE_ATLAS_IMAGE_TYPE_NINE_PATCH
        nine_slice {
            tiled_x = 0
            tiled_y = 0
            left = 16
            right = 16
            top = 16
            bottom = 16
        }
        color_table_multiplier_index = 0
    }
}

+entries {
    name = button_positive
    image {
        source = "colorless_button.png"
        type = KAN_RESOURCE_ATLAS_IMAGE_TYPE_NINE_PATCH
        nine_slice {
            tiled_x = 0
            tiled_y = 0
            left = 16
            right = 16
            top = 16
            bottom = 16
        }
        color_table_multiplier_index = 1
    }
}

+entries {
    name = button_neutral
    image {
        source = "colorless_button.png"
        type = KAN_RESOURCE_ATLAS_IMAGE_TYPE_NINE_PATCH
        nine_slice {
            tiled_x = 0
            tiled_y = 0
            left = 16
            right = 16
            top = 16
            bottom = 16
        }
        color_table_multiplier_index = 2
    }
}

+entries {
    name = window
    image {
        source = "colorless_button.png"
        type = KAN_RESOURCE_ATLAS_IMAGE_TYPE_NINE_PATCH
        nine_slice {
            tiled_x = 0
            tiled_y = 0
            left = 16
            right = 16
            top = 16
            bottom = 16
        }
    }
}

+entries {
    name = shield
    image {
        source = "shield.png"
        type = KAN_RESOURCE_ATLAS_IMAGE_TYPE_REGULAR
    }
}

+entries {
    name = tiled_slice
    image {
        source = "tiled_slice.png"
        type = KAN_RESOURCE_ATLAS_IMAGE_TYPE_NINE_PATCH
        nine_slice {
            tiled_x = 1
            tiled_y = 1
            left = 32
            right = 32
            top = 32
            bottom = 32
        }
    }
}
