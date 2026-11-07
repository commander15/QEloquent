target_headers(QEloquent
    PUBLIC
        datatype.h id.h
        model.h model_a.h smartmodel.h
        modelhelpers.h modelmaker.h queryhelper.h
        relation.h
    PRIVATE
        model_p.h
        relation_p.h relation_impl.h
)

target_sources(QEloquent
    PRIVATE
        datatype.cpp id.cpp
        model.cpp smartmodel.cpp
        relation.cpp
)
