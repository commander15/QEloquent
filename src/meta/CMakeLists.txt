target_headers(QEloquent
    PUBLIC
        metaobjectregistry.h
        metaobject.h metaproperty.h metarelation.h
        #metaobjectbuilder.h
    PRIVATE
        metaobject_p.h metaproperty_p.h
)

target_sources(QEloquent
    PRIVATE
        metaobjectregistry.cpp
        metaobject.cpp metaproperty.cpp metarelation.cpp
        #metaobjectbuilder.cpp
)
