if(NOT BUILD_TESTING)
  return()
endif()

find_package(GTest CONFIG REQUIRED)

enable_testing()
include(GoogleTest)

file(GLOB UNIT_TEST_SOURCES "unit/*.cpp")

add_executable(gqlxy_core_unit_tests ${UNIT_TEST_SOURCES})
target_link_libraries(gqlxy_core_unit_tests PRIVATE gqlxy_core GTest::gtest_main)
target_include_directories(gqlxy_core_unit_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
gtest_discover_tests(gqlxy_core_unit_tests)
