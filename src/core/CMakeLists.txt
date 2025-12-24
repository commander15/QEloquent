target_headers(QEloquent
    PUBLIC
        global.h
        entity.h
        connection.h
        driver.h
)

target_sources(QEloquent
    PRIVATE
        entity.cpp
        connection.cpp
        driver.cpp
)
