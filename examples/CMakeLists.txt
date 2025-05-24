# Get all example source files in this folder
file(GLOB EXAMPLE_SOURCES "*.cpp")

foreach(EXAMPLE_FILE ${EXAMPLE_SOURCES})
    # Get the name of the example without the extension
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_FILE} NAME_WE)

    # Create an executable from the example file
    add_executable(${EXAMPLE_NAME} ${EXAMPLE_FILE})

    # Link the executable with the main interface library (MyProject)
    target_link_libraries(${EXAMPLE_NAME} PRIVATE MyProject)

    # If SFML is enabled, explicitly link it to the example (fixes undefined references)
    if(USE_SFML)
        target_link_libraries(${EXAMPLE_NAME} PRIVATE SFML::Graphics SFML::Audio SFML::Network)
    endif()
endforeach()
