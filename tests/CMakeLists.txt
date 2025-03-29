cmake_minimum_required(VERSION 3.14)
project(MyProjectTests)

include(FetchContent)

# Try to find GTest first
find_package(GTest QUIET)

if (NOT GTest_FOUND)
    message(STATUS "GTest not found. Fetching GoogleTest...")
    
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    FetchContent_MakeAvailable(googletest)
endif()

enable_testing()

file(GLOB TEST_SOURCES "*.cpp")

foreach(TEST_FILE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} PRIVATE MyProject GTest::GTest GTest::Main)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()
