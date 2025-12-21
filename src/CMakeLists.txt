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

target_link_libraries(QEloquent PUBLIC Qt::Sql)

target_include_directories(QEloquent PUBLIC $<INSTALL_INTERFACE:include/Qloquent>)

install(TARGETS QEloquent EXPORT QEloquentExport
    ARCHIVE DESTINATION        ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION        ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION        ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION  ${CMAKE_INSTALL_INCLUDEDIR}/QEloquent
    PRIVATE_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/QEloquent/private
)

generate_target_headers(QEloquent FOLDER QEloquent)

include(CMakePackageConfigHelpers)

set(PACKAGE_DIR ${PROJECT_BINARY_DIR}/lib/cmake/QEloquent)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/QEloquentConfig.cmake.in"
    "${PACKAGE_DIR}/QEloquentConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/QEloquent"
)

write_basic_package_version_file(
    "${PACKAGE_DIR}/QEloquentConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

export(EXPORT QEloquentExport
    FILE "${PACKAGE_DIR}/QEloquentTargets.cmake"
    NAMESPACE QEloquent::
)

install(FILES
    "${PACKAGE_DIR}/QEloquentConfig.cmake"
    "${PACKAGE_DIR}/QEloquentConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/QEloquent"
)

install(EXPORT QEloquentExport
    FILE QEloquentTargets.cmake
    NAMESPACE QEloquent::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/QEloquent"
)
