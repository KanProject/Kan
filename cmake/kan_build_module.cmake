# We use a lot of smaller files to define our build framework, therefore it is easier to put all raw includes into one
# CMake module and include this module where it is needed than including all the small modules everywhere.

include_guard (GLOBAL)

# Include CMake Unit Framework.
include ("${CMAKE_CURRENT_LIST_DIR}/CMakeUnitFramework/CodeGeneration.cmake")
include ("${CMAKE_CURRENT_LIST_DIR}/CMakeUnitFramework/Core.cmake")
include ("${CMAKE_CURRENT_LIST_DIR}/CMakeUnitFramework/Product.cmake")
include ("${CMAKE_CURRENT_LIST_DIR}/CMakeUnitFramework/Unit.cmake")

# Configure CMake Unit Framework.
set (UNIT_FRAMEWORK_API_CASE "mixed_snake_case")

# Include Kan CMake extensions.
include ("${CMAKE_CURRENT_LIST_DIR}/kan/application.cmake")
include ("${CMAKE_CURRENT_LIST_DIR}/kan/binary_directory_flattening.cmake")
include ("${CMAKE_CURRENT_LIST_DIR}/kan/clang_format.cmake")
include ("${CMAKE_CURRENT_LIST_DIR}/kan/context.cmake")
include ("${CMAKE_CURRENT_LIST_DIR}/kan/global_options.cmake")
include ("${CMAKE_CURRENT_LIST_DIR}/kan/preprocessing.cmake")
include ("${CMAKE_CURRENT_LIST_DIR}/kan/reflection_statics.cmake")
include ("${CMAKE_CURRENT_LIST_DIR}/kan/testing.cmake")

# Standard-setting logic is exposed as macro so it could also be easily called by user projects.
macro (enable_kan_c_cxx_standards)
    # We use C23 standard as it is required for Cushion defer feature.
    # However, MSVC/ClangCL do not mark themselves as C23 compliant, 
    # so we ignore that setting there and use clatest with them instead.
    if (NOT MSVC)
        set (CMAKE_C_STANDARD 23)
        set (CMAKE_C_STANDARD_REQUIRED ON)
    else ()
        if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "^.*Clang$")
            add_compile_options ("$<IF:$<COMPILE_LANGUAGE:C>,-Xclang=-std=c23,>")
        else ()
            add_compile_options ("$<IF:$<COMPILE_LANGUAGE:C>,/std:clatest,>")
        endif ()
    endif ()

    set (CMAKE_CXX_STANDARD 17)
endmacro ()

# As per CMake docs:
# https://cmake.org/cmake/help/latest/manual/cmake-generator-expressions.7.html#genex:COMPILE_LANGUAGE:languages
#
# "Note that with Visual Studio Generators and Xcode there is no way to represent target-wide compile definitions or
# include directories separately for C and CXX languages."
#
# Which should've been fine as we're separating C and C++ targets. However, if C target directly depends on C++ target,
# for example on harfbuzz, it will also unwrap `$<COMPILE_LANGUAGE:C>` as `0`, meaning that flags above will not work
# despite the fact of target separation. Fortunately, this is a rare case and we can fix it by using the macro below
# in directories with such unfortunate C targets, for example `text` implementation that uses harfbuzz.
macro (force_fixup_c23_compile_flags)
    if (CMAKE_GENERATOR MATCHES "Visual Studio")
        if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "^.*Clang$")
            add_compile_options ("-Xclang=-std=c23")
        else ()
            add_compile_options ("/std:clatest")
        endif ()
    endif ()
endmacro ()
