cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

project(http-proxy)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE ON)

enable_testing()

find_package(Threads REQUIRED)
find_package(GTest REQUIRED)

set(APP_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/client-info.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/logger.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/proxy-server.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/request-parser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/request.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tcp-server.cpp
)

set(TEST_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/test/client-info-test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test/request-parser-test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test/request-test.cpp
)

add_executable(${PROJECT_NAME} 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
  ${APP_SRC}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# ---------------------------------------- gtest ---------------------------------------- #
include(GoogleTest)

add_executable(unit_tests 
  ${CMAKE_CURRENT_SOURCE_DIR}/test/main-test.cpp
  ${APP_SRC} 
  ${TEST_SRC}
)

target_link_libraries(unit_tests GTest::GTest)

target_include_directories(unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

gtest_discover_tests(unit_tests)

# ---------------------------------------- coverage report ---------------------------------------- #
add_custom_target(gcov
  COMMAND ctest
  COMMAND lcov --directory . --capture --output-file ./code_coverage.info -rc lcov_branch_coverage=1
  COMMAND genhtml code_coverage.info --branch-coverage --output-directory ./code_coverage_report/
  DEPENDS unit_tests
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build
)
