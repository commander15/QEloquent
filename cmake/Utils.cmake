function(target_headers target)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs PUBLIC PRIVATE)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Adding public headers to the target
    if (ARG_PUBLIC)
        list(TRANSFORM ARG_PUBLIC PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
        set_property(TARGET ${target} APPEND PROPERTY PUBLIC_HEADER ${ARG_PUBLIC})
    endif()

    # Adding private headers to the target
    if (ARG_PRIVATE)
        list(TRANSFORM ARG_PRIVATE PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
        set_property(TARGET ${target} APPEND PROPERTY PRIVATE_HEADER ${ARG_PRIVATE})
    endif()

    target_sources(${target} PRIVATE ${ARG_PUBLIC} ${ARG_PRIVATE})
endfunction()

function(generate_target_headers target)
    set(options)
    set(oneValueArgs DESTINATION FOLDER)
    set(multiValueArgs)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT ARG_DESTINATION)
        set(ARG_DESTINATION ${CMAKE_BINARY_DIR}/include)
    endif()

    if (NOT ARG_FOLDER)
        set(destination ${ARG_DESTINATION})
    else()
        set(destination ${ARG_DESTINATION}/${ARG_FOLDER})
    endif()

    # Generating public headers on destination
    get_target_property(PUBLIC_HEADERS ${target} PUBLIC_HEADER)
    if (PUBLIC_HEADERS)
        generate_nested_headers(${destination} ${PUBLIC_HEADERS})
    endif()

    # Generating private headers on destination
    get_target_property(PRIVATE_HEADERS ${target} PRIVATE_HEADER)
    if (PRIVATE_HEADERS)
        generate_nested_headers(${destination}/private ${PRIVATE_HEADERS})
    endif()

    # Adding destination to target's include path
    target_include_directories(${target} PUBLIC $<BUILD_INTERFACE:${ARG_DESTINATION}>)
endfunction()

function(generate_nested_headers destination)
    # Check if the header file exists and generate nested header file accordingly
    foreach (header ${ARGN})
        if (EXISTS ${header})
            file(RELATIVE_PATH path ${destination} ${header})
        elseif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${header})
            file(RELATIVE_PATH path ${destination} ${CMAKE_CURRENT_SOURCE_DIR}/${header})
        else()
            message(FATAL_ERROR "Can't find header file ${header}")
        endif()

        get_filename_component(header ${header} NAME)
        file(WRITE ${destination}/${header} "#include \"${path}\"")
    endforeach()
endfunction()
