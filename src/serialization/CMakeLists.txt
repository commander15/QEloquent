target_headers(QEloquent
    PUBLIC
        serialization.h
        serializable.h
        deserializable.h
)

target_sources(QEloquent
    PRIVATE
        serializable.cpp
        deserializable.cpp
)
