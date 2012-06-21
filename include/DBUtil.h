#ifndef DBUTIL_H
#define DBUTIL_H

#include "sqlite.h"
#include "common.h"
#include "value.h"

inline Sqlite::Row& operator >>(Sqlite::Row& row, QString& value)
{
    std::string str;
    row >> str;
    value = fromStdString(str);
    return row;
}

inline Sqlite::Row& operator >>(Sqlite::Row& row, Value& value)
{
    switch(row.getType())
    {
    case SQLITE_TEXT:
    {
        std::string str;
        row >> str;
        value = Value(fromStdString(str));
    }
        break;
    case SQLITE_FLOAT:
    {
        double val;
        row >> val;
        value = Value(val);
    }
        break;
    default:
        throw std::exception();
    }

    return row;
}

inline Sqlite::PreparedStatement& operator <<(Sqlite::PreparedStatement& stmt, Value const& value)
{
    switch(value.getType())
    {
    case Value::DOUBLE:
        stmt << value.asDouble();
        break;
    case Value::STRING:
        stmt << toStdString(value.asString());
        break;
    case Value::EMPTY:
        break;
    }

    return stmt;
}

#endif // DBUTIL_H
