cmake_minimum_required(VERSION 3.26)

project(
    god-of-war-menu
    VERSION 0.22
    LANGUAGES CXX
)

set(PROJECT_RESOURCES_PATH "${CMAKE_CURRENT_LIST_DIR}/resources")
set(PROJECT_SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}/source")

#
# Store the current git commit hash for later use
#
execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
    OUTPUT_VARIABLE BUILD_GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

#
# Set up the actual library
#
add_subdirectory("${PROJECT_SOURCE_PATH}")

#
# And finally produce build artifacts
#
install(
    DIRECTORY
        "${PROJECT_RESOURCES_PATH}/mods/"
    DESTINATION
        "mods/"
)

install(
    TARGETS
        main_output_dll
    RUNTIME
        DESTINATION "/"
)

set(CPACK_PACKAGE_FILE_NAME "GoWScriptLoader-${CMAKE_PROJECT_VERSION_MAJOR}_${CMAKE_PROJECT_VERSION_MINOR}")
set(CPACK_GENERATOR "ZIP")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
include(CPack)
