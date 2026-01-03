find_package(Doxygen)

if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target(doc ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        SOURCES
            Doxyfile.in
            introduction.md
            model_definition.md
            usage.md
        VERBATIM
    )

    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doxygen DESTINATION doc/QEloquent)
else()
    message(STATUS "Doxygen not found, 'doc' target will not be available")
endif()
