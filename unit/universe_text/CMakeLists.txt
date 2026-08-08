register_concrete (universe_text)
concrete_include (PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
concrete_sources (GLOB "*.c")

concrete_require (
        SCOPE PUBLIC
        ABSTRACT context_render_backend_system text
        CONCRETE_INTERFACE math resource_text universe)

concrete_require (
        SCOPE PRIVATE
        ABSTRACT context_hot_reload_coordination_system cpu_profiler log
        CONCRETE_INTERFACE universe_locale universe_render_foundation universe_resource_provider)
setup_reflected_preprocessing ()

set (KAN_UNIVERSE_TEXT_FONT_CATEGORY_INIT_STACK "32" CACHE STRING
        "Size of stack allocated array for font category initialization.")

concrete_compile_definitions (
        PRIVATE
        KAN_UNIVERSE_TEXT_FONT_CATEGORY_INIT_STACK=${KAN_UNIVERSE_TEXT_FONT_CATEGORY_INIT_STACK})

