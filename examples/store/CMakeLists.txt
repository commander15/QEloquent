set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

qt_add_executable(Store
    main.cpp
    models/user.h models/user.cpp
    models/product.h models/product.cpp
    models/sale.h models/sale.cpp
    ui/mainwindow.h ui/mainwindow.cpp
    ui/dashboardview.h ui/dashboardview.cpp
    ui/logindialog.h ui/logindialog.cpp
    ui/session.h
    models/model.h models/model.cpp
    common.h
    migration.cpp
    seeding.cpp
)

target_link_libraries(Store PRIVATE Qt6::Widgets QEloquent)

install(TARGETS Store RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

qt_generate_deploy_app_script(
    TARGET Store
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
