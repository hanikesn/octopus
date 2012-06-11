#ifndef SQLITE_H
#define SQLITE_H

#include <sqlite3.h>
#include <string>
#include <iterator>

namespace Sqlite
{

class DB;
class PreparedStatement;
class Row;

class Exception : public std::exception
{
public:
    Exception(int code) : code(code) {}

    virtual const char *what() const noexcept;

    const int code;
};

class Row
{
    friend class PreparedStatement;
    Row(PreparedStatement& stmt);
public:
    int columnCount();

    int getType(int index);
    std::string getName(int index);

    std::string getText(int index);

private:
    PreparedStatement& stmt;
};

class PreparedStatement
{
    // no copy
    PreparedStatement(const PreparedStatement&);
    PreparedStatement& operator = (const PreparedStatement&);

    // only Sqlite is allowed to make new prepared statements
    friend class DB;
    friend class Row;
    PreparedStatement(sqlite3_stmt* stmt);
public:
    ~PreparedStatement();

    PreparedStatement(PreparedStatement &&);
    //PreparedStatement& operator=(PreparedStatement&&);

    class QueryIterator : public std::iterator< std::input_iterator_tag, Row >
    {
        friend class PreparedStatement;
        QueryIterator(PreparedStatement* stmt);

    public:
        QueryIterator() : stmt(0) {}

        Row operator*();
        Row operator->();

        bool operator==(const QueryIterator&) const;
        bool operator!=(const QueryIterator&) const;

        QueryIterator& operator++();
        QueryIterator operator++(int);

    private:
        PreparedStatement* stmt;
    };

    void reset();

    void bindText(int index, const std::string& value);
    void bindInt(int index, sqlite3_int64 value);
    void bindDouble(int index, double value);

    QueryIterator execute();
    QueryIterator done();

private:
    sqlite3_stmt* stmt;
};

class DB
{
    // no copy
    DB(const DB&);
    DB& operator = (const DB&);
public:
    DB(std::string name);
    ~DB();

    PreparedStatement::QueryIterator execute(std::string const& query);

    PreparedStatement prepare(const std::string& query);

    static const PreparedStatement::QueryIterator Done;

private:
    sqlite3* db;
};

}

#endif // SQLITE_H
