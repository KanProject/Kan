//! kan_resource_material_instance_mixin_t

+parameters.f1 { name = cos_anim_period value = 3.0 }
+parameters.f1 { name = glyph_read_time value = 0.05 }

+tail_append {
    tail_name = text_palette
    +parameters.f4 { name = color_main value { x = 1.0 y = 1.0 z = 1.0 w = 1.0 } }
    +parameters.f4 { name = color_outline value { x = 0.0 y = 0.0 z = 0.0 w = 1.0 } }
    +parameters.f4 { name = cos_anim_color value { x = 0.4 y = 0.4 z = 0.4 w = 1.0 } }
    +parameters.f4 { name = color_hovered value { x = 0.7 y = 0.7 z = 0.7 w = 1.0 } }
    +parameters.f4 { name = color_down value { x = 0.9 y = 0.9 z = 0.9 w = 1.0 } }
    +parameters.u1 { name = color_table_multiplier_index value = 0 }
}
