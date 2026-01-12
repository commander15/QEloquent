target_headers(QEloquent
    PUBLIC
        driver.h
    PRIVATE
        driver_p.h
        impl/sqlitedriver_p.h
)

target_sources(QEloquent
    PRIVATE
        driver.cpp
        impl/sqlitedriver.cpp
)
