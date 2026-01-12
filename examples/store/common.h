#ifndef COMMON_H
#define COMMON_H

#include <QEloquent/result.h>

QEloquent::Result<int> migrate();
void rollbackMigrations();

QEloquent::Result<int> seed();

#endif // COMMON_H
