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

class Exception : public virtual std::exception
{
public:
    Exception(sqlite3* db) : code(sqlite3_extended_errcode(db)), msg(sqlite3_errmsg(db))  {}
    Exception(int code, const char * msg) : code(code), msg(msg) {}
    ~Exception() throw () {}

    virtual const char *what() const throw();

    const int code;
    const std::string msg;
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

    int getType();

    Row& operator>>(std::string& value);
    Row& operator>>(double& value);
    Row& operator>>(sqlite3_int64& value);

private:
    PreparedStatement& stmt;
    int index;
};

class PreparedStatement
{
    // no copy
    PreparedStatement(const PreparedStatement&);
    PreparedStatement& operator = (const PreparedStatement&);

    // only Sqlite is allowed to make new prepared statements
    friend class DB;
    friend class Row;
    PreparedStatement(sqlite3* db, sqlite3_stmt* stmt);
public:
    PreparedStatement();
    ~PreparedStatement();

    PreparedStatement(PreparedStatement &&);
    PreparedStatement& operator=(PreparedStatement&&);

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

    void bind(int index, const std::string& value);
    void bind(int index, sqlite3_int64 value);
    void bind(int index, double value);

    PreparedStatement& operator<<(std::string const& value);
    PreparedStatement& operator<<(sqlite3_int64 value);
    PreparedStatement& operator<<(double value);

    QueryIterator execute();
    QueryIterator done();

private:
    sqlite3* db;
    sqlite3_stmt* stmt;
    int index;
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

    PreparedStatement prepare(const std::string& query) const;

    static const PreparedStatement::QueryIterator Done;

private:
    sqlite3* db;
};

}

#endif // SQLITE_H
