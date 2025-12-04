target_headers(QEloquent
    PUBLIC
        genericmodel.h
        modelinfo.h modelinfobuilder.h
        modelquery.h
        modelerror.h
    PRIVATE
        genericmodel_p.h
        modelinfo_p.h
)

target_sources(QEloquent
    PRIVATE
        genericmodel.cpp
        modelinfo.cpp modelinfobuilder.cpp
        modelquery.cpp
        modelerror.cpp
)
