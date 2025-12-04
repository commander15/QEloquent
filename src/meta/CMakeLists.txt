target_headers(QEloquent
    PUBLIC
        metatable.h
        namingconvention.h
    PRIVATE
        metatable_p.h
)

target_sources(QEloquent
    PRIVATE
        metatable.cpp
        namingconvention.cpp
)
