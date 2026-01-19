//! kan_application_framework_program_configuration_t

${ENABLED_SYSTEMS}

+enabled_systems {
    name = render_backend_system_t
    configuration {
        __type = kan_render_backend_system_config_t
        application_info_name = "Kan example"
        version_major = 1
        version_minor = 0
        version_patch = 0
    }
}

log_name = example_ui_interaction
program_world = "example/ui_interaction"
${AUTO_BUILD_SUFFIX}
