#ifndef DATABASEADAPTER_H
#define DATABASEADAPTER_H

#include "sqlite.h"

#include <QString>

class DatabaseAdapter
{
public:
    DatabaseAdapter(QString const& file);

private:
    Sqlite::DB db;
};

#endif // DATABASEADAPTER_H
