target_headers(QEloquent
    PUBLIC
        serialization.h
        serializable.h
        deserializable.h
    PRIVATE
        serializers/jsonserializer_p.h
        serializers/yamlserializer_p.h
        serializers/csvserializer_p.h
)

target_sources(QEloquent
    PRIVATE
        serializable.cpp
        deserializable.cpp
)
