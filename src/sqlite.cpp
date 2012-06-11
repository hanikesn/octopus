#include "sqlite.h"

#include <exception>
#include <iostream>

namespace Sqlite
{

const char *Exception::what() const noexcept
{
    switch(code) {
    case 0:
        return "SQLITE_OK";
    case 1:
        return "SQLITE_ERROR";
    case 2:
        return "SQLITE_INTERNAL";
    case 14:
        return "SQLITE_CANTOPEN";
    case 21:
        return "SQLITE_MISUSE";
    case 25:
        return "SQLITE_RANGE";
    default:
        return "Unknown error";
    }

}

DB::DB(std::string file)
{
    int ret = sqlite3_open(file.c_str(), &db);
    if(ret != SQLITE_OK)
        throw Exception(ret);
}

DB::~DB()
{
    sqlite3_close(db);
}

const PreparedStatement::QueryIterator DB::Done;

PreparedStatement DB::prepare(const std::string& query)
{
    sqlite3_stmt* stmt;
    const char* unused;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length(), &stmt, &unused);
    if(ret != SQLITE_OK)
        throw Exception(ret);

    return std::move(PreparedStatement(stmt));
}

PreparedStatement::PreparedStatement(PreparedStatement && other)
    : stmt(other.stmt)
{
    other.stmt = 0;
}

PreparedStatement::QueryIterator DB::execute(std::string const& query)
{
    return prepare(query).execute();
}


PreparedStatement::PreparedStatement(sqlite3_stmt* stmt) :
    stmt(stmt)
{
}

PreparedStatement::~PreparedStatement()
{
    sqlite3_finalize(stmt);
}

PreparedStatement::QueryIterator PreparedStatement::execute()
{
    return QueryIterator(this);
}

PreparedStatement::QueryIterator PreparedStatement::done()
{
    return QueryIterator();
}

void PreparedStatement::bindDouble(int index, double value)
{
    int ret = sqlite3_bind_double(stmt, index, value);
    if(ret != SQLITE_OK)
        throw Exception(ret);
}

void PreparedStatement::bindInt(int index, sqlite3_int64 value)
{
    int ret = sqlite3_bind_int(stmt, index, value);
    if(ret != SQLITE_OK)
        throw Exception(ret);
}

void PreparedStatement::bindText(int index, const std::string& value)
{
    int ret = sqlite3_bind_text(stmt, index, value.c_str(), value.length(), SQLITE_TRANSIENT);
    if(ret != SQLITE_OK)
        throw Exception(ret);
}

void PreparedStatement::reset()
{
    sqlite3_reset(stmt);
}

PreparedStatement::QueryIterator& PreparedStatement::QueryIterator::operator++()
{
    if(stmt != nullptr) {
        int ret = sqlite3_step(stmt->stmt);
        if(ret == SQLITE_BUSY || ret == SQLITE_ERROR || ret == SQLITE_MISUSE) {
            throw Exception(ret);
        } else if(ret == SQLITE_DONE) {
            stmt = nullptr;
        } else {
            // assert(ret==SQLITE_ROW);
        }
    }
    return *this;
}

PreparedStatement::QueryIterator PreparedStatement::QueryIterator::operator ++(int)
{
    auto tmp = *this;
    ++*this;
    return tmp;
}

PreparedStatement::QueryIterator::QueryIterator(PreparedStatement *stmt) :
    stmt(stmt)
{
    ++*this;
}

bool PreparedStatement::QueryIterator::operator==(const QueryIterator& other) const
{
    return stmt == other.stmt;
}
bool PreparedStatement::QueryIterator::operator!=(const QueryIterator& other) const
{
    return !(*this == other);
}

Row PreparedStatement::QueryIterator::operator*()
{
    return Row(*this->stmt);
}
Row PreparedStatement::QueryIterator::operator->()
{
    return Row(*this->stmt);
}

Row::Row(PreparedStatement &stmt)
    : stmt(stmt)
{
}

int Row::columnCount()
{
    return sqlite3_column_count(stmt.stmt);
}

int Row::getType(int index)
{
    return sqlite3_column_type(stmt.stmt, index);
}

std::string Row::getName(int index)
{
    return std::string(sqlite3_column_name(stmt.stmt, index));
}

std::string Row::getText(int index)
{
    return std::string((const char*)sqlite3_column_text(stmt.stmt, index));
}

}
