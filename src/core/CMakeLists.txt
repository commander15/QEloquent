target_headers(QEloquent
    PUBLIC
        global.h config.h
        entity.h
        connection.h
        transactionguard.h
)

target_sources(QEloquent
    PRIVATE
        entity.cpp
        connection.cpp
        transactionguard.cpp
)

configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h @ONLY)