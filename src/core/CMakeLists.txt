target_headers(QEloquent
    PUBLIC
        global.h
        connection.h
        entity.h
    PRIVATE
        entity_p.h
)

target_sources(QEloquent
    PRIVATE
        connection.cpp
        entity.cpp
)
