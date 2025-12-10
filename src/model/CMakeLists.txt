target_headers(QEloquent
    PUBLIC
        modelinfo.h modelinfobuilder.h
    PRIVATE
        modelinfo_p.h
)

target_sources(QEloquent
    PRIVATE
        modelinfo.cpp modelinfobuilder.cpp
)
