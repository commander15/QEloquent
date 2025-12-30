target_headers(QEloquent
    PUBLIC
        model.h
        modelhelpers.h
        relation.h
    PRIVATE
        model_p.h
        relation_impl.h
)

target_sources(QEloquent
    PRIVATE
        model.cpp
        relation.cpp
)
