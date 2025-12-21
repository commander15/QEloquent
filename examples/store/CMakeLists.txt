set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

qt_add_executable(Store
    main.cpp
    models/user.h models/user.cpp
    models/product.h models/product.cpp
    models/sale.h models/sale.cpp
    ui/mainwindow.h ui/mainwindow.cpp
    ui/dashboardview.h ui/dashboardview.cpp
    ui/productcatalogview.h ui/productcatalogview.cpp
    ui/saleshistoryview.h ui/saleshistoryview.cpp
)

target_link_libraries(Store PRIVATE Qt6::Widgets QEloquent)
