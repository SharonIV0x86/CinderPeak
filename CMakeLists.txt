cmake_minimum_required(VERSION 3.14)
project(CinderPeak LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BIN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin)

add_library(CinderPeak INTERFACE)
target_include_directories(CinderPeak INTERFACE ${CMAKE_SOURCE_DIR}/src)

# === Build Tests ===
option(BUILD_TESTS "Build and run unit tests" ON)
if(BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/tests/*.cpp)

foreach(test_file ${TEST_SOURCES})
    file(RELATIVE_PATH rel_path ${CMAKE_SOURCE_DIR}/tests ${test_file})
    get_filename_component(test_dir ${rel_path} DIRECTORY)
    get_filename_component(test_base ${test_file} NAME_WE)

    string(REPLACE "/" "_" test_dir_flat ${test_dir})
    set(test_name ${test_dir_flat}_${test_base})

    add_executable(${test_name} ${test_file})
    target_link_libraries(${test_name} PRIVATE CinderPeak GTest::gtest_main)

    set_target_properties(${test_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}/tests/${test_dir}
        OUTPUT_NAME ${test_base}
    )

    add_test(
        NAME ${test_name}
        COMMAND $<TARGET_FILE:${test_name}>
    )
endforeach()
endif()

# === Build Examples ===
option(BUILD_EXAMPLES "Build example programs" ON)
if(BUILD_EXAMPLES)
    file(GLOB_RECURSE EXAMPLE_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/examples/*.cpp)

    foreach(example ${EXAMPLE_SOURCES})
        # Get relative path from examples directory
        file(RELATIVE_PATH rel_path ${CMAKE_SOURCE_DIR}/examples ${example})
        get_filename_component(example_dir ${rel_path} DIRECTORY)
        get_filename_component(example_base ${example} NAME_WE)

        # Safely flatten directory name
        if(example_dir)
            string(REPLACE "/" "_" example_dir_flat "${example_dir}")
            set(example_target "${example_dir_flat}_${example_base}")
        else()
            set(example_dir "")
            set(example_target "${example_base}")
        endif()

        add_executable(${example_target}_example ${example})
        target_link_libraries(${example_target}_example PRIVATE CinderPeak)

        # Preserve directory structure in output
        set_target_properties(${example_target}_example PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}/examples/${example_dir}
            OUTPUT_NAME ${example_base}
        )
    endforeach()
endif()
