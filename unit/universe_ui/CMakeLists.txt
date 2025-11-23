register_concrete (universe_ui)
concrete_include (PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
concrete_sources (GLOB "*.c")

concrete_require (
        SCOPE PUBLIC
        ABSTRACT context_render_backend_system
        CONCRETE_INTERFACE inline_math universe universe_render_foundation universe_text)

concrete_require (
        SCOPE PRIVATE
        ABSTRACT cpu_profiler log
        CONCRETE_INTERFACE universe_locale universe_resource_provider
        THIRD_PARTY qsort)
setup_reflected_preprocessing ()

set (KAN_UNIVERSE_UI_LAYOUT_TEMPORARY_PAGE "32768" CACHE STRING
        "Size of a page for ui layout calculation temporary allocations.")

concrete_compile_definitions (
        PRIVATE
        KAN_UNIVERSE_UI_LAYOUT_TEMPORARY_PAGE=${KAN_UNIVERSE_UI_LAYOUT_TEMPORARY_PAGE})
