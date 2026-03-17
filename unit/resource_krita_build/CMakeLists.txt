find_program (KRITARUNNER kritarunner)
if (KRITARUNNER-NOTFOUND)
    message (WARNING "Unable to find \"kritarunner\" executable, krita resource integration is disabled.")
    return ()
endif ()

register_concrete (resource_krita_build)
concrete_include (PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
concrete_sources ("*.c")

concrete_require (SCOPE PUBLIC CONCRETE_INTERFACE resource_render_foundation)
concrete_require (
        SCOPE PRIVATE 
        ABSTRACT error file_system log platform
        CONCRETE_INTERFACE math resource_pipeline resource_render_foundation_build serialization
        THIRD_PARTY qsort)
setup_reflected_preprocessing ()

set (KAN_RESOURCE_KRITA_IO_BUFFER "16384" CACHE STRING "Size of an IO buffer for reading generated imported headers.")
set (KAN_RESOURCE_KRITA_ATLAS_CAPACITY "64" CACHE STRING
        "Initial capacity for the atlas entries that are built from Krita files.")

concrete_compile_definitions (
        PRIVATE
        KAN_RESOURCE_KRITA_IO_BUFFER=${KAN_RESOURCE_KRITA_IO_BUFFER}
        KAN_RESOURCE_KRITA_ATLAS_CAPACITY=${KAN_RESOURCE_KRITA_ATLAS_CAPACITY})
