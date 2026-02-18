cmake_minimum_required(VERSION 3.14)
project(CinderPeak LANGUAGES CXX)

option(SANITIZE "Enable sanitizers (ASAN/UBSAN)" OFF)
option(BUILD_BENCHMARKS "Build benchmarks" OFF)
option(BUILD_COVERAGE "Build with coverage flags" OFF)

if(SANITIZE)
    if(MSVC)
        message(WARNING "Sanitizers not supported on MSVC; ignoring SANITIZE")
    else()
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address,undefined)
    endif()
endif()

if(BUILD_COVERAGE)
    if(NOT MSVC)
        add_compile_options(--coverage -fprofile-arcs -ftest-coverage)
        add_link_options(--coverage)
    else()
        message(WARNING "Coverage not supported on MSVC; ignoring BUILD_COVERAGE")
    endif()
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(BIN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin)

# === Comprehensive Warning Flags (applied per-target) ===
option(PEDANTIC_WARNINGS "Enable comprehensive compiler warnings" ON)

# Function to apply warnings to a target
function(apply_warning_flags target_name)
    if(NOT PEDANTIC_WARNINGS)
        return()
    endif()
    
    if(MSVC)
        # MSVC: Show all useful warnings
        target_compile_options(${target_name} PRIVATE
            /W4                 # Enable level 4 warnings (most warnings)
            /permissive-        # Enforce standards conformance
            # Disable specific noisy warnings that are not actionable
            /wd4100             # Unreferenced formal parameter (common in interfaces/templates)
            /wd4201             # Nonstandard extension used: nameless struct/union
            /wd4458             # Declaration hides class member
            /wd4459             # Declaration hides global declaration
        )
    else()
        # GCC/Clang: Enable comprehensive warnings
        target_compile_options(${target_name} PRIVATE
            -Wall               # Enable most warning flags
            -Wextra             # Enable extra warning flags
            -Wpedantic          # Warn on language extensions
        )
        
        # Additional safe warnings that work across all versions
        target_compile_options(${target_name} PRIVATE
            -Wshadow            # Warn when a local variable shadows another
            -Wformat=2          # Extra format string checks
            -Wunused            # Warn on unused entities
            -Wnon-virtual-dtor  # Warn on non-virtual destructor
        )
        
        # More aggressive warnings (may not be supported on all compilers)
        # Only add if compiler supports them
        include(CheckCXXCompilerFlag)
        
        check_cxx_compiler_flag("-Wconversion" HAS_WCONVERSION)
        if(HAS_WCONVERSION)
            target_compile_options(${target_name} PRIVATE -Wconversion)
        endif()
        
        check_cxx_compiler_flag("-Wold-style-cast" HAS_WOLD_STYLE_CAST)
        if(HAS_WOLD_STYLE_CAST)
            target_compile_options(${target_name} PRIVATE -Wold-style-cast)
        endif()
        
        check_cxx_compiler_flag("-Wcast-align" HAS_WCAST_ALIGN)
        if(HAS_WCAST_ALIGN)
            target_compile_options(${target_name} PRIVATE -Wcast-align)
        endif()
        
        check_cxx_compiler_flag("-Woverloaded-virtual" HAS_WOVERLOADED_VIRTUAL)
        if(HAS_WOVERLOADED_VIRTUAL)
            target_compile_options(${target_name} PRIVATE -Woverloaded-virtual)
        endif()
        
        check_cxx_compiler_flag("-Wdouble-promotion" HAS_WDOUBLE_PROMOTION)
        if(HAS_WDOUBLE_PROMOTION)
            target_compile_options(${target_name} PRIVATE -Wdouble-promotion)
        endif()
        
        # IMPORTANT: Do NOT treat warnings as errors - only show them
        target_compile_options(${target_name} PRIVATE -Wno-error)
        
        # GCC-specific warnings
        if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            check_cxx_compiler_flag("-Wlogical-op" HAS_WLOGICAL_OP)
            if(HAS_WLOGICAL_OP)
                target_compile_options(${target_name} PRIVATE -Wlogical-op)
            endif()
            
            # GCC 7+ warnings
            if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 7.0)
                check_cxx_compiler_flag("-Wnull-dereference" HAS_WNULL_DEREF)
                if(HAS_WNULL_DEREF)
                    target_compile_options(${target_name} PRIVATE -Wnull-dereference)
                endif()
                
                check_cxx_compiler_flag("-Wduplicated-branches" HAS_WDUP_BRANCHES)
                if(HAS_WDUP_BRANCHES)
                    target_compile_options(${target_name} PRIVATE -Wduplicated-branches)
                endif()
                
                check_cxx_compiler_flag("-Wimplicit-fallthrough" HAS_WIMPLICIT_FALLTHROUGH)
                if(HAS_WIMPLICIT_FALLTHROUGH)
                    target_compile_options(${target_name} PRIVATE -Wimplicit-fallthrough)
                endif()
            endif()
            
            # GCC 6+ warnings
            if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 6.0)
                check_cxx_compiler_flag("-Wduplicated-cond" HAS_WDUP_COND)
                if(HAS_WDUP_COND)
                    target_compile_options(${target_name} PRIVATE -Wduplicated-cond)
                endif()
                
                check_cxx_compiler_flag("-Wmisleading-indentation" HAS_WMISLEADING_INDENT)
                if(HAS_WMISLEADING_INDENT)
                    target_compile_options(${target_name} PRIVATE -Wmisleading-indentation)
                endif()
                
                check_cxx_compiler_flag("-Wuseless-cast" HAS_WUSELESS_CAST)
                if(HAS_WUSELESS_CAST)
                    target_compile_options(${target_name} PRIVATE -Wuseless-cast)
                endif()
            endif()
        endif()
        
        # Clang-specific warnings
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            check_cxx_compiler_flag("-Wrange-loop-analysis" HAS_WRANGE_LOOP)
            if(HAS_WRANGE_LOOP)
                target_compile_options(${target_name} PRIVATE -Wrange-loop-analysis)
            endif()
            
            # Disable C++98 compatibility warnings for Clang
            target_compile_options(${target_name} PRIVATE
                -Wno-c++98-compat
                -Wno-c++98-compat-pedantic
            )
        endif()
    endif()
endfunction()

add_library(CinderPeak INTERFACE)
target_include_directories(CinderPeak INTERFACE 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/tests/integration/CinderGraph/common
)

# === Build Tests ===
option(BUILD_TESTS "Build and run unit tests" ON)
if(BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
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
    
    # Apply warning flags to this test target
    apply_warning_flags(${test_name})

    set_target_properties(${test_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}/tests/${test_dir}
        OUTPUT_NAME ${test_base}
    )

    add_test(
        NAME ${test_name}
        COMMAND $<TARGET_FILE:${test_name}>
        
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
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
        
        # Apply warning flags to this example target
        apply_warning_flags(${example_target}_example)

        # Preserve directory structure in output
        set_target_properties(${example_target}_example PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}/examples/${example_dir}
            OUTPUT_NAME ${example_base}
        )
    endforeach()
endif()
