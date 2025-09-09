cmake_minimum_required(VERSION 3.14)
project(CinderPeak LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BIN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

# --- SFML Dependency ---
option(USE_SYSTEM_SFML "Use system-installed SFML" OFF)
if(USE_SYSTEM_SFML)
    find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)
else()
    FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG        2.6.1
    )
    # Only build needed modules
    set(SFML_BUILD_AUDIO FALSE CACHE BOOL "" FORCE)
    set(SFML_BUILD_NETWORK FALSE CACHE BOOL "" FORCE)
    set(SFML_BUILD_WINDOW TRUE CACHE BOOL "" FORCE)
    set(SFML_BUILD_GRAPHICS TRUE CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(SFML)
endif()

# --- Core Interface Library ---
add_library(CinderPeak INTERFACE)
target_include_directories(CinderPeak INTERFACE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(CinderPeak INTERFACE
    sfml-graphics
    sfml-window
    sfml-system
)

# --- Tests ---
option(BUILD_TESTS "Build and run unit tests" ON)
if(BUILD_TESTS)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    file(GLOB TEST_SOURCES "${CMAKE_SOURCE_DIR}/tests/*.cpp")
    foreach(test_file ${TEST_SOURCES})
        get_filename_component(test_name ${test_file} NAME_WE)
        add_executable(${test_name}_test ${test_file})
        target_link_libraries(${test_name}_test PRIVATE
            CinderPeak
            GTest::gtest_main
        )
        set_target_properties(${test_name}_test PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}/tests
        )
        add_test(NAME ${test_name} COMMAND ${test_name}_test)
    endforeach()
endif()

# --- Examples ---
option(BUILD_EXAMPLES "Build example programs" ON)
if(BUILD_EXAMPLES)
    file(GLOB_RECURSE EXAMPLE_SOURCES
        "${CMAKE_SOURCE_DIR}/examples/*.cpp"
    )
    foreach(example ${EXAMPLE_SOURCES})
        get_filename_component(example_name ${example} NAME_WE)
        add_executable(${example_name}_example ${example})
        target_link_libraries(${example_name}_example PRIVATE CinderPeak)
        set_target_properties(${example_name}_example PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}/examples
        )
    endforeach()
endif()
