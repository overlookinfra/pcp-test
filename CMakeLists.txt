# Defines how cmake should behave, and the minimum version necessary to build.
cmake_minimum_required(VERSION 3.2.2)

project(pcp-test VERSION 0.2.0)

string(MAKE_C_IDENTIFIER ${PROJECT_NAME} PROJECT_C_NAME)
string(TOUPPER ${PROJECT_C_NAME} PROJECT_NAME_UPPER)
string(TOLOWER ${PROJECT_C_NAME} PROJECT_NAME_LOWER)

# Common cmake setup
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "Defaulting to a release build.")
    set(CMAKE_BUILD_TYPE "Release"
        CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
endif()

enable_testing()

# Leatherman setup
SET(LEATHERMAN_COMPONENTS locale catch nowide logging)
find_package(Leatherman REQUIRED COMPONENTS ${LEATHERMAN_COMPONENTS})

# Now that we have leatherman, we can pull in its options file, which
# we need for finding all our other libraries.
include(options)
## Pull in common cflags setting from leatherman. Don't override CMAKE_CXX_FLAGS
## at the project root to avoid impacting 3rd party code.
include(cflags)
set(${PROJECT_NAME_UPPER}_CXX_FLAGS "${LEATHERMAN_CXX_FLAGS}")
add_definitions(${LEATHERMAN_DEFINITIONS})
## Pull in helper macros for working with leatherman libraries
include(leatherman)

# Include our .cmake helpers
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Prefer openssl from ports
if ("${CMAKE_SYSTEM_NAME}" MATCHES "Darwin")
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /opt/local/lib)
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} /opt/local/include)
endif()

# Add other dependencies
find_package(Boost 1.54 REQUIRED
             COMPONENTS program_options filesystem chrono system date_time thread log regex random)
find_package(OpenSSL REQUIRED)
find_package(cpp-pcp-client REQUIRED)

# Display a summary of the features
include(FeatureSummary)
feature_summary(WHAT ALL)

add_subdirectory(lib)
add_subdirectory(exe)

# Add cpplint and cppcheck targets
file(GLOB_RECURSE ALL_SOURCES lib/src/*.cc lib/inc/*.hpp exe/*.cc)
add_cpplint_files(${ALL_SOURCES})
enable_cpplint()

add_cppcheck_dirs("${PROJECT_SOURCE_DIR}/lib" "${PROJECT_SOURCE_DIR}/exe")
enable_cppcheck()
