//! kan_resource_material_header_t
sources = "ui_text_icon.rpl"

+passes {
    name = ui_default

    +entry_points {
        stage = KAN_RPL_PIPELINE_STAGE_GRAPHICS_CLASSIC_VERTEX
        function_name = ui_vertex_main
    }

    +entry_points {
        stage = KAN_RPL_PIPELINE_STAGE_GRAPHICS_CLASSIC_FRAGMENT
        function_name = ui_fragment_main
    }
}
