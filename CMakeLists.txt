cmake_minimum_required (VERSION 3.25)
cmake_policy (VERSION 3.25)

project (
        Kan
        DESCRIPTION "Game development framework for pure C programming."
        # C++ is needed in cases where there are no C bindings for required libraries.
        LANGUAGES C CXX)

list (APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include (kan_build_module)
create_format_targets (kan)

# Must be called from root CMakeLists.txt, otherwise CTest configuration won't be placed into binary root.
enable_testing ()

add_subdirectory (third_party)
enable_kan_c_cxx_standards ()
add_subdirectory (unit)
add_subdirectory (test)
add_subdirectory (executable)
