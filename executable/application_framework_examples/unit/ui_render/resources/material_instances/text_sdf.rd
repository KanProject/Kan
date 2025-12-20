//! kan_resource_material_instance_raw_t
material = ui_text_glyph_sdf

+parameters {
    name = cos_anim_period
    type = KAN_RPL_META_VARIABLE_TYPE_F1
    value_f1 = 3.0
}

+parameters {
    name = glyph_read_time
    type = KAN_RPL_META_VARIABLE_TYPE_F1
    value_f1 = 0.05
}

+tail_append {
    tail_name = palette
    +parameters {
        name = color_main
        type = KAN_RPL_META_VARIABLE_TYPE_F4
        value_f4 { x = 1.0 y = 1.0 z = 1.0 w = 1.0 }
    }
    +parameters {
        name = color_outline
        type = KAN_RPL_META_VARIABLE_TYPE_F4
        value_f4 { x = 0.0 y = 0.0 z = 0.0 w = 1.0 }
    }
    +parameters {
        name = cos_anim_color
        type = KAN_RPL_META_VARIABLE_TYPE_F4
        value_f4 { x = 0.4 y = 0.4 z = 0.4 w = 1.0 }
    }
    +parameters {
        name = color_table_multiplier_index
        type = KAN_RPL_META_VARIABLE_TYPE_U1
        value_u1 = 0
    }
}
