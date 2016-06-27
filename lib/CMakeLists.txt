include(leatherman)
get_commit_string(PROJECT_COMMIT)
set(TEMPLATE_DIR "${CMAKE_CURRENT_LIST_DIR}/templates")
set(INCLUDES_DIR "${CMAKE_CURRENT_LIST_DIR}/inc/${PROJECT_NAME}")

# Generate a file containing the version numbers.
configure_file("${TEMPLATE_DIR}/version.h.in" "${INCLUDES_DIR}/version.h")

# Set the root path macro and expand related template
set(ROOT_PATH ${PROJECT_SOURCE_DIR})
configure_file("${TEMPLATE_DIR}/root_path.h.in" "${INCLUDES_DIR}/root_path.h")

# Set C++ compile flags based on project settings.
set(CMAKE_CXX_FLAGS ${${PROJECT_NAME_UPPER}_CXX_FLAGS})

# Set prefix for logging messages.
leatherman_logging_namespace("puppetlabs.${PROJECT_NAME}")

# Setup compiling the library.
include_directories(inc
    ${Boost_INCLUDE_DIRS}
    ${LEATHERMAN_INCLUDE_DIRS}
    ${cpp-pcp-client_INCLUDE_DIR}
)

set(PROJECT_SOURCES
    src/client.cc
    src/client_configuration.cc
    src/configuration.cc
    src/configuration_parameters.cc
    src/connection_stats.cc
    src/message.cc
    src/pcp-test.cc
    src/schemas.cc
    src/test_connection.cc
    src/test_connection_parameters.cc
    src/test_trivial.cc
    src/util.cc
)

## An object target is generated that can be used by both the library and test
## executable targets.
## Without the intermediate target, unexported symbols can't be tested.
add_library(libprojectsrc OBJECT ${PROJECT_SOURCES})
set_target_properties(libprojectsrc PROPERTIES POSITION_INDEPENDENT_CODE true)

add_library(lib${PROJECT_NAME} $<TARGET_OBJECTS:libprojectsrc>)
set_target_properties(lib${PROJECT_NAME}
    PROPERTIES
    VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
)
target_link_libraries(lib${PROJECT_NAME}
    ${LEATHERMAN_LIBRARIES}
    ${Boost_LIBRARIES}
    ${cpp-pcp-client_LIBRARY}
)

symbol_exports(lib${PROJECT_NAME} "${CMAKE_CURRENT_LIST_DIR}/inc/${PROJECT_NAME}/export.h")

leatherman_install(lib${PROJECT_NAME})
install(DIRECTORY inc/${PROJECT_NAME} DESTINATION include)

add_subdirectory(tests)
