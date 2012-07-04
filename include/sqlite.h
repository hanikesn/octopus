#ifndef SQLITE_H
#define SQLITE_H

#include <sqlite3.h>
#include <string>
#include <iterator>

/**
 * A thin wrapper for sqlite
 */
namespace Sqlite
{

class DB;
class PreparedStatement;
class Row;

/**
 * @brief This exception is thrown whenever a problem with sqlite occurs
 */
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

/**
 * @brief Use this class to access the rows in the result of a query
 *
 */
class Row
{
    friend class PreparedStatement;
    Row(PreparedStatement& stmt);
public:
    int columnCount();

    int getType(int index);
    std::string getName(int index);

    std::string getText(int index);

    /**
     * @brief Get the sqlite type of the current column
     */
    int getType();

    /**
     * @brief Use these method to query the columns one after another
     */
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

    /**
     * Use this class for access to the result rows.
     *
     * A QueryIterator is invalid as soon as the PreparedStatement that created it is destroyed.
     */
    class QueryIterator : public std::iterator< std::input_iterator_tag, Row >
    {
        friend class PreparedStatement;
        QueryIterator(PreparedStatement* stmt);

    public:
        QueryIterator() : stmt(0) {}

        /**
         * Warning: Rows will be invalid once the query is incremented or is destroyed
         */
        Row operator*();
        Row operator->();

        bool operator==(const QueryIterator&) const;
        bool operator!=(const QueryIterator&) const;

        QueryIterator& operator++();
        QueryIterator operator++(int);

    private:
        PreparedStatement* stmt;
    };

    /**
     * @brief reset the statement so that it can be used again.
     */
    void reset();

    /**
     * @brief Use these methods to bind the values manually to the statement.
     */
    void bind(int index, const std::string& value);
    void bind(int index, const char*, int n);
    void bind(int index, sqlite3_int64 value);
    void bind(int index, double value);

    /**
     * @brief "feed" new to data into the query. Use these methods to bind values one after another to the statement.
     */
    PreparedStatement& operator<<(std::string const& value);
    PreparedStatement& operator<<(sqlite3_int64 value);
    PreparedStatement& operator<<(double value);

    /**
     * @brief execute a query and get a iterator to the result.
     * The Iterator should be incremented until it equals done()
     */
    QueryIterator execute();
    /**
     * @brief Check against this iterator to know wether the query was successful
     */
    QueryIterator done();

private:
    sqlite3* db;
    sqlite3_stmt* stmt;
    int index;
};

/**
 * @brief Open a SQlite database and execute queries on it
 *
 */
class DB
{
    // no copy
    DB(const DB&);
    DB& operator = (const DB&);
public:
    DB(std::string name);
    ~DB();

    /**
     * Execute a simple query without results.
     * @return true if successful
     */
    bool execute(std::string const& query);

    /**
     * @brief Prepare a new query
     */
    PreparedStatement prepare(const std::string& query) const;

private:
    sqlite3* db;
};

}

#endif // SQLITE_H
