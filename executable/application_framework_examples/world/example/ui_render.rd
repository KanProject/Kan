//! kan_universe_world_definition_t

world_name = "example"
scheduler_name = "trivial"

+configuration {
    name = "ui"
    +layers {
        data {
            __type = kan_ui_configuration_t
            default_bundle_name = "default_ui_bundle"
        }
    }
}

+pipelines {
    name = "update"
    mutator_groups =
        text_shaping,
        ui_controls,
        ui_core,
        ui_example_render
}
