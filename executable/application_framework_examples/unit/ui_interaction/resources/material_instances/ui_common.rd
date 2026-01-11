//! kan_resource_material_instance_mixin_t

+tail_append {
    tail_name = ui_mark_palette
    +parameters {
        name = color_modifier_regular
        type = KAN_RPL_META_VARIABLE_TYPE_F4
        value_f4 { x = 1.0 y = 1.0 z = 1.0 w = 1.0 }
    }
    +parameters {
        name = color_modifier_hovered
        type = KAN_RPL_META_VARIABLE_TYPE_F4
        value_f4 { x = 1.0 y = 1.0 z = 1.0 w = 1.0 }
    }
    +parameters {
        name = color_modifier_down
        type = KAN_RPL_META_VARIABLE_TYPE_F4
        value_f4 { x = 1.0 y = 1.0 z = 1.0 w = 1.0 }
    }
    +parameters {
        name = color_modifier_regular
        type = KAN_RPL_META_VARIABLE_TYPE_F4
        value_f4 { x = 1.0 y = 1.0 z = 1.0 w = 1.0 }
    }
    +parameters {
        name = no_blink_interval_s
        type = KAN_RPL_META_VARIABLE_TYPE_F1
        value_f1 = 1.0
    }
    +parameters {
        name = blink_invisible_s
        type = KAN_RPL_META_VARIABLE_TYPE_F1
        value_f1 = 0.1
    }
    +parameters {
        name = blink_visible_s
        type = KAN_RPL_META_VARIABLE_TYPE_F1
        value_f1 = 0.2
    }
}
