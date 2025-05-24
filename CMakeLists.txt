cmake_minimum_required(VERSION 3.14)
project(MyProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(MyProject INTERFACE)
target_include_directories(MyProject INTERFACE src/)

option(USE_SFML "Enable SFML support" ON)

if(USE_SFML)
    set(SFML_STATIC_LIBRARIES ON)  
    find_package(SFML 3 REQUIRED COMPONENTS Graphics Window Audio System Network REQUIRED)
    target_link_libraries(MyProject INTERFACE SFML::Graphics SFML::Audio SFML::Network)
endif()

option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

option(BUILD_EXAMPLES "Build examples" ON)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()