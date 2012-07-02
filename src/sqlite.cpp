#include "sqlite.h"

#include <exception>
#include <iostream>
#include <cassert>

namespace Sqlite
{

const char *Exception::what() const throw()
{
    return msg.c_str();

}

DB::DB(std::string file)
{
    int ret = sqlite3_open(file.c_str(), &db);
    if(ret != SQLITE_OK) {
        sqlite3_close(db);
        throw Exception(db);
    }

    sqlite3_extended_result_codes(db, true);
}

DB::~DB()
{
    sqlite3_close(db);
}

const PreparedStatement::QueryIterator DB::Done;

PreparedStatement DB::prepare(const std::string& query) const
{
    sqlite3_stmt* stmt;
    const char* unused;
    // sqlite expects the null terminator to be included in the length
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &unused);
    if(ret != SQLITE_OK)
        throw Exception(db);

    return std::move(PreparedStatement(db, stmt));
}

PreparedStatement::PreparedStatement()
    : stmt(0)
{
}

PreparedStatement::PreparedStatement(PreparedStatement && other)
    : stmt(other.stmt), index(other.index)
{
    other.stmt = 0;
    other.index = 0;
}

PreparedStatement& PreparedStatement::operator =(PreparedStatement && other)
{
    std::swap(stmt, other.stmt);
    std::swap(index, other.index);
    return *this;
}

PreparedStatement::QueryIterator DB::execute(std::string const& query)
{
    return prepare(query).execute();
}


PreparedStatement::PreparedStatement(sqlite3* db, sqlite3_stmt* stmt) :
    db(db), stmt(stmt), index(0)
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

void PreparedStatement::bind(int index, double value)
{
    assert(index <=  sqlite3_bind_parameter_count(stmt));
    int ret = sqlite3_bind_double(stmt, index, value);
    if(ret != SQLITE_OK)
        throw Exception(db);
}

void PreparedStatement::bind(int index, sqlite3_int64 value)
{
    assert(index <=  sqlite3_bind_parameter_count(stmt));
    int ret = sqlite3_bind_int64(stmt, index, value);
    if(ret != SQLITE_OK)
        throw Exception(db);
}

void PreparedStatement::bind(int index, const std::string& value)
{
    assert(index <=  sqlite3_bind_parameter_count(stmt));
    int ret = sqlite3_bind_text(stmt, index, value.c_str(), value.length(), SQLITE_TRANSIENT);
    if(ret != SQLITE_OK)
        throw Exception(db);
}

void PreparedStatement::bind(int index, const char* value, int n)
{
    assert(index <=  sqlite3_bind_parameter_count(stmt));
    int ret = sqlite3_bind_text(stmt, index, value, n, SQLITE_TRANSIENT);
    if(ret != SQLITE_OK)
        throw Exception(db);
}

void PreparedStatement::reset()
{
    index = 0;
    sqlite3_reset(stmt);
}

PreparedStatement& PreparedStatement::operator<<(std::string const& value)
{
    index++;
    bind(index, value);
    return *this;
}

PreparedStatement& PreparedStatement::operator<<(sqlite3_int64 value)
{
    index++;
    bind(index, value);
    return *this;
}

PreparedStatement& PreparedStatement::operator<<(double value)
{
    index++;
    bind(index, value);
    return *this;
}

PreparedStatement::QueryIterator& PreparedStatement::QueryIterator::operator++()
{
    if(stmt != nullptr) {
        int ret = sqlite3_step(stmt->stmt);
        if(ret == SQLITE_BUSY || ret == SQLITE_ERROR || ret == SQLITE_MISUSE || ret == SQLITE_CONSTRAINT) {
            throw Exception(stmt->db);
        } else if(ret == SQLITE_DONE) {
            stmt = nullptr;
        } else {
            assert(ret==SQLITE_ROW);
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
    : stmt(stmt), index(0)
{
}

Row& Row::operator>>(std::string& value)
{
    assert(index <= sqlite3_data_count(stmt.stmt));
    value = std::string((const char*)sqlite3_column_text(stmt.stmt, index));
    index++;
    return *this;
}

Row& Row::operator>>(double& value)
{
    assert(index <= sqlite3_data_count(stmt.stmt));
    value = sqlite3_column_double(stmt.stmt, index);
    index++;
    return *this;
}

Row& Row::operator>>(sqlite3_int64& value)
{
    assert(index <= sqlite3_data_count(stmt.stmt));
    value = sqlite3_column_int64(stmt.stmt, index);
    index++;
    return *this;
}

int Row::columnCount()
{
    return sqlite3_column_count(stmt.stmt);
}

int Row::getType(int index)
{
    assert(index <= sqlite3_data_count(stmt.stmt));
    return sqlite3_column_type(stmt.stmt, index);
}

int Row::getType()
{
    assert(index <= sqlite3_data_count(stmt.stmt));
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
