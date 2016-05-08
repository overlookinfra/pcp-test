# Setup compiling the test executable. C++ compile flags are inherited from\
# the parent directory.
include_directories(${LEATHERMAN_CATCH_INCLUDE})

set(TEST_CASES
    pcp-test_test.cc
)

add_executable(${PROJECT_NAME}_test $<TARGET_OBJECTS:libprojectsrc> ${TEST_CASES} main.cc)

target_link_libraries(${PROJECT_NAME}_test
    ${Boost_LIBRARIES}
    ${LEATHERMAN_LIBRARIES}
    ${cpp-pcp-client_LIBRARY}
)

add_test(NAME "unit_tests" COMMAND ${PROJECT_NAME}_test)
