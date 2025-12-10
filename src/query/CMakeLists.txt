target_headers(QEloquent
    PUBLIC
        query.h error.h
        querybuilder.h
        queryrunner.h
)

target_sources(QEloquent
    PRIVATE
        query.cpp error.cpp
        querybuilder.cpp
        queryrunner.cpp
)
