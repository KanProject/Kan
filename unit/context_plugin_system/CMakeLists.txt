register_concrete (context_plugin_system)
concrete_include (PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
concrete_sources ("*.c")
concrete_require (SCOPE PUBLIC CONCRETE_INTERFACE context)

concrete_require (
        SCOPE PRIVATE
        ABSTRACT
        context_hot_reload_coordination_system error file_system file_system_watcher log memory platform precise_time
        CONCRETE_INTERFACE context_reflection_system context_update_system)

register_context_system (NAME plugin_system_t)
setup_reflected_preprocessing (REFLECTION_GLOB "*.h")

set (KAN_PLUGIN_SYSTEM_PLUGINS_INITIAL_SIZE "8" CACHE STRING "Initial size for configuration plugins array.")
set (KAN_PLUGIN_SYSTEM_HOT_RELOAD_SUFFIX_BUFFER "16" CACHE STRING "Size of a buffer for hot reload suffix generation.")
set (KAN_PLUGIN_SYSTEM_HOT_RELOAD_IO_BUFFER "4096" CACHE STRING "Size of a buffer for hot reload io operations.")
set (KAN_PLUGIN_SYSTEM_HOT_RELOAD_MAX_RETRIES "20" CACHE STRING "Max retries to load one plugin library.")
set (KAN_PLUGIN_SYSTEM_HOT_RELOAD_RETRY_DELAY_NS "100000000" CACHE STRING "Delay between plugin hot reload retries.")

concrete_compile_definitions (
        PRIVATE
        KAN_PLUGIN_SYSTEM_PLUGINS_INITIAL_SIZE=${KAN_PLUGIN_SYSTEM_PLUGINS_INITIAL_SIZE}
        KAN_PLUGIN_SYSTEM_HOT_RELOAD_SUFFIX_BUFFER=${KAN_PLUGIN_SYSTEM_HOT_RELOAD_SUFFIX_BUFFER}
        KAN_PLUGIN_SYSTEM_HOT_RELOAD_IO_BUFFER=${KAN_PLUGIN_SYSTEM_HOT_RELOAD_IO_BUFFER}
        KAN_PLUGIN_SYSTEM_HOT_RELOAD_MAX_RETRIES=${KAN_PLUGIN_SYSTEM_HOT_RELOAD_MAX_RETRIES}
        KAN_PLUGIN_SYSTEM_HOT_RELOAD_RETRY_DELAY_NS=${KAN_PLUGIN_SYSTEM_HOT_RELOAD_RETRY_DELAY_NS})

if (UNIX)
    find_program (PATCH_ELF patchelf)
    if (NOT PATCH_ELF-NOTFOUND)
        # On Unix-like systems, dlopen resolves required shared objects to already linked shared objects if soname
        # matches, which means that if one plugin tries to link to other plugin it would link itself to the old
        # version of that plugin as it is presented earlier in the loaded objects list. We mitigate this issue by
        # patching always-new soname values to hot reload shared object copies using patchelf utility.
        concrete_compile_definitions (PRIVATE KAN_PLUGIN_SYSTEM_USE_PATCH_ELF_FIX)
    else ()
        message (WARNING "Command line utility patchelf not available. It can cause issues during plugin hot reload.")
    endif ()
endif ()
