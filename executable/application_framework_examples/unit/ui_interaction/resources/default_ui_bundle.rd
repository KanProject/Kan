//! kan_resource_ui_bundle_t
pass = ui_default
image_atlas = ui_atlas
image_material = ui_image
text_sdf_material = text_sdf
text_icon_material = text_icon

+hit_box_interaction_styles {
    name = default
    regular_image = button
    hovered_image = button_hovered
    down_image = button_down
    disabled_image = button
    down_state_s = 0.1
}

+hit_box_interaction_styles {
    name = line_edit_regular
    regular_image = text_edit_regular
    hovered_image = text_edit_regular_hovered
    down_image = text_edit_regular_hovered
    disabled_image = text_edit_regular
    down_state_s = 0.1
}

+hit_box_interaction_styles {
    name = line_edit_selected
    regular_image = text_edit_selected
    hovered_image = text_edit_selected
    down_image = text_edit_selected
    disabled_image = text_edit_selected
    down_state_s = 0.1
}
