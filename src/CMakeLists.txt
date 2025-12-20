qt_add_library(QEloquent)

add_subdirectory(core)
add_subdirectory(utils)
add_subdirectory(query)
add_subdirectory(meta)
add_subdirectory(orm)
#add_subdirectory(relation)


target_compile_definitions(QEloquent
    PUBLIC
        QLOQUENT_LIB
    PRIVATE
        QLOQUENT_BUILD
)

target_link_libraries(QEloquent PUBLIC Qt::Sql expected)

target_include_directories(QEloquent PUBLIC $<INSTALL_INTERFACE:include/Qloquent>)

install(TARGETS QEloquent EXPORT QEloquentExport
    ARCHIVE DESTINATION        ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION        ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION        ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION  ${CMAKE_INSTALL_INCLUDEDIR}/QEloquent
    PRIVATE_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/QEloquent/private
)

generate_target_headers(QEloquent FOLDER QEloquent)
