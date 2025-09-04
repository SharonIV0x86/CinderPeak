cmake_minimum_required(VERSION 3.10)
project(VisualizationModule CXX)

# Find SFML package
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Create an INTERFACE library for header-only code
add_library(Visualization INTERFACE)

# Include directories
target_include_directories(Visualization
    INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Link SFML to the interface library
target_link_libraries(Visualization
    INTERFACE
        sfml-graphics
        sfml-window
        sfml-system
)

# For IDE support - list the header files
target_sources(Visualization
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/VisualizationEngine.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/InteractiveNode.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/InteractiveEdge.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/VisualizationTypes.hpp
)

# Add the example executable
add_executable(VisualizationDemo ../../examples/VisualizationDemo.cpp)

# Link ONLY the Visualization library (it will bring SFML automatically)
target_link_libraries(VisualizationDemo PRIVATE Visualization)
