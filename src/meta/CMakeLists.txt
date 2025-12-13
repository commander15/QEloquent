target_headers(QEloquent
    PUBLIC
        metaobjectregistry.h
        metaobject.h metaproperty.h
        metaobjectgenerator.h
    PRIVATE
        metaobject_p.h metaproperty_p.h
)

target_sources(QEloquent
    PRIVATE
        metaobjectregistry.cpp
        metaobject.cpp metaproperty.cpp
        metaobjectgenerator.cpp
)
