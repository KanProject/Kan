register_interface (api_common)
interface_include ("${CMAKE_CURRENT_SOURCE_DIR}")

set (KAN_CORE_TYPES_PRESET "default" CACHE STRING "Preset for core types. Supported presets: default.")
if (KAN_CORE_TYPES_PRESET STREQUAL "default")
    interface_compile_definitions (KAN_CORE_TYPES_PRESET_DEFAULT)
else ()
    message (SEND_ERROR "Unknown core types preset value \"${KAN_CORE_TYPES_PRESET}\".")
endif ()
