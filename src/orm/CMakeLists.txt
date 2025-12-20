target_headers(QEloquent
    PUBLIC
        model.h modelhelpers.h
        relation.h relation_impl.h
    PRIVATE
        model_p.h
)

target_sources(QEloquent
    PRIVATE
        model.cpp
        relation.cpp
)
