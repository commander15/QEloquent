target_headers(QEloquent
    PUBLIC
        dictionary.h
        result.h
        namingconvention.h
        listproxy.h itemproxy.h
        datamap.h
    PRIVATE
        namingconvention_p.h
)

target_sources(QEloquent
    PRIVATE
        dictionary.cpp
        namingconvention.cpp
        datamap.cpp
)
