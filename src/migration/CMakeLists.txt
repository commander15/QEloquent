target_headers(QEloquent
    PUBLIC
        migrator.h migration.h
        schema.h schemagrammar.h
        tableblueprint.h tabledata.h columndata.h
    PRIVATE
        migrator_p.h migration_p.h
        schemagrammar_p.h
        tableblueprint_p.h
)

target_sources(QEloquent
    PRIVATE
        migrator.cpp migration.cpp
        schema.cpp schemagrammar.cpp
        tableblueprint.cpp tabledata.cpp columndata.cpp
)

target_compile_definitions(QEloquent
    PUBLIC
        QELOQUENT_MIGRATIONS_SUPPORT
)
