target_headers(QEloquent
    PUBLIC
        migrator.h
        migration.h
        migration_p.h
        schema.h
        tableblueprint.h
    PRIVATE
        migrator_p.h
        migration_p.h
        tableblueprint_p.h
)

target_sources(QEloquent
    PRIVATE
        migrator.cpp
        migration.cpp
        schema.cpp
        tableblueprint.cpp
)

target_compile_definitions(QEloquent
    PUBLIC
        QELOQUENT_MIGRATIONS_SUPPORT
)
