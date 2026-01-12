#ifndef QELOQUENT_SCHEMAGRAMMAR_P_H
#define QELOQUENT_SCHEMAGRAMMAR_P_H

#include "schemagrammar.h"

namespace QEloquent {

class DefaultSchemaGrammar final : public SchemaGrammar
{
public:
    DefaultSchemaGrammar(Driver *driver)
        : SchemaGrammar(driver) {}

    bool hasFeature(GrammarFeature) const override {
        return false;
    }

    QPair<ColumnType, NumberSign> primaryKeyType() const override {
        return { ColumnData::Integer, ColumnData::Signed };
    }

    QString autoIncrementKeyword() const override {
        return QString();
    }
};

}

#endif // QELOQUENT_SCHEMAGRAMMAR_P_H
