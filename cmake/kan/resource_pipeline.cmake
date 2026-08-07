# Contains functions for registering resource pipeline packages and retrieving registered packages.

# We allow package name to be different than directory name for unit root packages,
# but that forces us to have this separate array. Which should be fine.
define_property (TARGET PROPERTY UNIT_RESOURCE_PACKAGE_NAMES
        BRIEF_DOCS "Names of packages found inside this unit, should be zipped with package directories."
        FULL_DOCS "Names of packages found inside this unit, should be zipped with package directories.")

define_property (TARGET PROPERTY UNIT_RESOURCE_PACKAGE_DIRECTORIES
        BRIEF_DOCS "Directories of packages found inside this unit."
        FULL_DOCS "Should be zipped with package names property.")

# Register all resource packages found inside subdirectory with given name to the current unit.
# Subdirectory must have root package! Name of the root package will always be equal to current unit name,
# not directory name. Other package names will be equal to their directories.
# Arguments:
# - 1st: Name of the subdirectory to start the scanning from it.
function (register_unit_resource_packages ROOT_DIRECTORY_NAME)
    get_target_property (PACKAGE_NAMES "${UNIT_NAME}" UNIT_RESOURCE_PACKAGE_NAMES)
    if (PACKAGE_NAMES)
        message (SEND_ERROR "Unit \"${UNIT_NAME}\" already has registered packages!")
        return ()
    endif ()

    set (ROOT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${ROOT_DIRECTORY_NAME}")
    if (NOT EXISTS "${ROOT_DIRECTORY}/.package.rd")
        message (SEND_ERROR "There is no root package in \"${ROOT_DIRECTORY}\".")
        return ()
    endif ()

    file (GLOB_RECURSE MANIFESTS LIST_DIRECTORIES false "${ROOT_DIRECTORY}/*/.package.rd")
    set (PACKAGE_NAMES)
    set (PACKAGE_DIRECTORIES)

    # Root package.
    list (APPEND PACKAGE_NAMES "${UNIT_NAME}")
    list (APPEND PACKAGE_DIRECTORIES "${ROOT_DIRECTORY}")

    foreach (MANIFEST ${MANIFESTS})
        file (TO_CMAKE_PATH "${MANIFEST}" MANIFEST_PATH)
        cmake_path (ABSOLUTE_PATH MANIFEST_PATH)
        cmake_path (GET MANIFEST_PATH PARENT_PATH MANIFEST_DIRECTORY)
        cmake_path (GET MANIFEST_DIRECTORY FILENAME PACKAGE_NAME)

        list (APPEND PACKAGE_NAMES "${PACKAGE_NAME}")
        list (APPEND PACKAGE_DIRECTORIES "${MANIFEST_DIRECTORY}")
    endforeach ()

    set_target_properties ("${UNIT_NAME}" PROPERTIES
            UNIT_RESOURCE_PACKAGE_NAMES "${PACKAGE_NAMES}"
            UNIT_RESOURCE_PACKAGE_DIRECTORIES "${PACKAGE_DIRECTORIES}")
endfunction ()

# Find all packages with their names in directories that are provided from given shared libraries.
# Arguments:
# - OUTPUT_NAMES: name of the variable to output names of the packages.
# - OUTPUT_DIRECTORIES: name of the variable to output directories of the packages. Optional.
# - ROOT_TARGETS: list of library targets that are used as roots for searching.
function (gather_resource_packages)
    cmake_parse_arguments (ARG "" "OUTPUT_NAMES;OUTPUT_DIRECTORIES" "ROOT_TARGETS" ${ARGV})
    if (DEFINED ARG_UNPARSED_ARGUMENTS OR
            NOT DEFINED ARG_OUTPUT_NAMES OR
            NOT DEFINED ARG_ROOT_TARGETS)
        message (FATAL_ERROR "Incorrect function arguments!")
    endif ()

    set (NAMES)
    set (DIRECTORIES)

    foreach (ROOT_TARGET ${ARG_ROOT_TARGETS})
        find_linked_targets_recursively (TARGET "${ROOT_TARGET}" OUTPUT REFERENCED_TARGETS ARTEFACT_SCOPE)
        foreach (REFERENCED ${REFERENCED_TARGETS})
            get_target_property (THIS_NAMES "${REFERENCED}" UNIT_RESOURCE_PACKAGE_NAMES)
            if (THIS_NAMES)
                list (APPEND NAMES ${THIS_NAMES})
                if (DEFINED ARG_OUTPUT_DIRECTORIES)
                    get_target_property (THIS_DIRECTORIES "${REFERENCED}" UNIT_RESOURCE_PACKAGE_DIRECTORIES)
                    list (APPEND DIRECTORIES ${THIS_DIRECTORIES})
                endif ()
            endif ()
        endforeach ()
    endforeach ()

    set ("${ARG_OUTPUT_NAMES}" "${NAMES}" PARENT_SCOPE)
    if (DEFINED ARG_OUTPUT_DIRECTORIES)
        set ("${ARG_OUTPUT_DIRECTORIES}" "${DIRECTORIES}" PARENT_SCOPE)
    endif ()
endfunction ()

# Generates resource project file using given parameters.
# Arguments:
# - OUTPUT_FILE: path at which resource project file should be saved.
# - WORKSPACE: workspace path for the resource project.
# - PLUGIN_DIRECTORY_NAME: name of the directory used to store plugins.
# - PLATFORM_CONFIGURATION: path to the platform configuration directory.
# - PLATFORM_CONFIGURATION_TAGS: list of enabled platform configuration tags if any. Optional.
# - CORE_ROOT_TARGET: shared library that is used as an always loaded core.
# - CORE_PLUGIN_ROOT_TARGETS: list of shared library targets that are plugins, but are in always loaded core.
# - PLUGIN_ROOT_TARGETS: list of plugin library targets, every target is treated as independent plugin.
function (generate_resource_project)
    cmake_parse_arguments (ARG ""
            "OUTPUT_FILE;WORKSPACE;PLUGIN_DIRECTORY_NAME;PLATFORM_CONFIGURATION;CORE_ROOT_TARGET"
            "PLATFORM_CONFIGURATION_TAGS;CORE_PLUGIN_ROOT_TARGETS;PLUGIN_ROOT_TARGETS" ${ARGV})

    if (DEFINED ARG_UNPARSED_ARGUMENTS OR
            NOT DEFINED ARG_OUTPUT_FILE OR
            NOT DEFINED ARG_WORKSPACE OR
            NOT DEFINED ARG_PLUGIN_DIRECTORY_NAME OR
            NOT DEFINED ARG_PLATFORM_CONFIGURATION OR
            NOT DEFINED ARG_CORE_ROOT_TARGET OR
            NOT DEFINED ARG_CORE_PLUGIN_ROOT_TARGETS OR
            NOT DEFINED ARG_PLUGIN_ROOT_TARGETS)
        message (FATAL_ERROR "Incorrect function arguments!")
    endif ()

    set (CONTENT "//! kan_resource_project_t\n\n")
    gather_resource_packages (OUTPUT_NAMES NAMES OUTPUT_DIRECTORIES DIRECTORIES ROOT_TARGETS
            ${ARG_CORE_ROOT_TARGET} ${ARG_CORE_PLUGIN_ROOT_TARGETS})

    foreach (NAME DIRECTORY IN ZIP_LISTS NAMES DIRECTORIES)
        string (APPEND CONTENT "+packages {\n")
        string (APPEND CONTENT "    name = \"${NAME}\"\n")
        string (APPEND CONTENT "    group = KAN_RESOURCE_PACKAGE_GROUP_CORE\n")
        string (APPEND CONTENT "    directory = \"${DIRECTORY}\"\n")
        string (APPEND CONTENT "}\n\n")
    endforeach ()

    foreach (PLUGIN_TARGET ${ARG_PLUGIN_ROOT_TARGETS})
        gather_resource_packages (OUTPUT_NAMES NAMES OUTPUT_DIRECTORIES DIRECTORIES ROOT_TARGETS "${PLUGIN_TARGET}")
        foreach (NAME DIRECTORY IN ZIP_LISTS NAMES DIRECTORIES)
            string (APPEND CONTENT "+packages {\n")
            string (APPEND CONTENT "    name = \"${NAME}\"\n")
            string (APPEND CONTENT "    group = KAN_RESOURCE_PACKAGE_GROUP_PLUGIN\n")
            string (APPEND CONTENT "    plugin = \"${PLUGIN_TARGET}\"\n")
            string (APPEND CONTENT "    directory = \"${DIRECTORY}\"\n")
            string (APPEND CONTENT "}\n\n")
        endforeach ()
    endforeach ()

    string (APPEND CONTENT "workspace_directory = \"${ARG_WORKSPACE}\"\n")
    string (APPEND CONTENT "platform_configuration_directory = \"${ARG_PLATFORM_CONFIGURATION}\"")

    if (ARG_PLATFORM_CONFIGURATION_TAGS)
        string (APPEND CONTENT "platform_configuration_tags =\n")
        set (COMMA "    ")

        foreach (TAG ${ARG_PLATFORM_CONFIGURATION_TAGS})
            string (APPEND CONTENT "${COMMA}\"${TAG}\"")
            set (COMMA ",\n    ")
        endforeach ()
    endif ()

    string (APPEND CONTENT "\n")
    string (APPEND CONTENT "plugin_directory_name = \"${ARG_PLUGIN_DIRECTORY_NAME}\"\n")
    string (APPEND CONTENT "plugins =\n")
    set (COMMA "    ")

    foreach (PLUGIN_TARGET ${ARG_PLUGIN_ROOT_TARGETS} ${ARG_CORE_PLUGIN_ROOT_TARGETS})
        string (APPEND CONTENT "${COMMA}\"${PLUGIN_TARGET}\"")
        set (COMMA ",\n    ")
    endforeach ()

    file (CONFIGURE OUTPUT "${ARG_OUTPUT_FILE}" CONTENT "${CONTENT}")
endfunction ()
