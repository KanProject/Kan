//! kan_resource_atlas_header_t
page_width = 512
page_height = 512

border_size = 1
border {
    r = 0.0
    g = 0.0
    b = 0.0
    a = 0.0
}

+entries {
    name = window
    image {
        source = "colorless_button.png"
        type = KAN_RESOURCE_ATLAS_IMAGE_TYPE_NINE_SLICE
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
    name = button
    image {
        source = "colorless_button.png"
        type = KAN_RESOURCE_ATLAS_IMAGE_TYPE_NINE_SLICE
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
    name = button_hovered
    image {
        source = "colorless_button_hovered.png"
        type = KAN_RESOURCE_ATLAS_IMAGE_TYPE_NINE_SLICE
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
    name = button_down
    image {
        source = "colorless_button_down.png"
        type = KAN_RESOURCE_ATLAS_IMAGE_TYPE_NINE_SLICE
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
