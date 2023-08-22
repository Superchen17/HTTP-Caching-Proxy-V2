cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

project(http-proxy)

option(BUILD_TESTS "Build test programs" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE ON)

find_package(Threads REQUIRED)

set(APP_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/client-info.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/datetime.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/logger.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/proxy-server.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/request-parser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/request.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/response-parser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/response.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tcp-server.cpp
)

set(TEST_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/test/client-info-test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test/datetime-test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test/fifo-cache-test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test/parser-test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test/request-parser-test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test/request-test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test/response-parser-test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test/response-test.cpp
)

add_executable(${PROJECT_NAME} 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
  ${APP_SRC}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# ---------------------------------------- gtest ---------------------------------------- #
if(BUILD_TESTS)
  enable_testing()

  include(GoogleTest)

  find_package(GTest REQUIRED)

  add_executable(unit_tests 
    ${CMAKE_CURRENT_SOURCE_DIR}/test/main-test.cpp
    ${APP_SRC} 
    ${TEST_SRC}
  )

  target_link_libraries(unit_tests GTest::GTest)

  target_include_directories(unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

  gtest_discover_tests(unit_tests)
endif(BUILD_TESTS)

# ---------------------------------------- coverage report ---------------------------------------- #
if(BUILD_TESTS)
  add_custom_target(gcov
    COMMAND ctest
    COMMAND lcov --directory . --capture --output-file ./code_coverage.info -rc lcov_branch_coverage=1
    COMMAND genhtml code_coverage.info --branch-coverage --output-directory ./code_coverage_report/
    DEPENDS unit_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build
  )
endif(BUILD_TESTS)
