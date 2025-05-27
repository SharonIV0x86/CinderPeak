# Get all example source files in this folder
file(GLOB EXAMPLE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Get all example source files from the extras folder
file(GLOB EXTRA_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/extras/*.cpp")

# Combine both sets of sources
set(ALL_EXAMPLE_SOURCES ${EXAMPLE_SOURCES} ${EXTRA_SOURCES})

foreach(EXAMPLE_FILE ${ALL_EXAMPLE_SOURCES})
    # Get the name of the example without the extension
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_FILE} NAME_WE)

    # Create an executable from the example file
    add_executable(${EXAMPLE_NAME} ${EXAMPLE_FILE})

    # Link the executable with the main interface library (MyProject)
    target_link_libraries(${EXAMPLE_NAME} PRIVATE MyProject)

    # If SFML is enabled, explicitly link it to the example
    if(USE_SFML)
        target_link_libraries(${EXAMPLE_NAME} PRIVATE SFML::Graphics SFML::Audio SFML::Network)
    endif()
endforeach()
