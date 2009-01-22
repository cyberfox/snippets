#ifndef DB_H
#define DB_H

/**
 * @file      DB.h
 * @copyright Copyright (c) 2004 Friendster, Inc.
 * @author    Morgan Schweers <morgan@friendster.com>
 * @date      Mon Aug 30 17:03:05 2004
 * 
 * @brief     Database abstraction, so the code isn't littered with MySQLisms.
 */
#include <iostream>
#include <mysql/mysql.h>
#include <vector>
#include <map>
#include <memory>

#include "SimpleLog.h"

using namespace std;

class DBResultSet;

/*** End of Line marker for use with stream outputs. */
//@{
#define EOL "\n"
//@}

/*!@class DBException
 *
 * @brief A wrapper for the exception handling class, so that we can
 * track where database exceptions occur.
 */
class DBException {
  private:
    string m_file;
    string m_msg;
    int m_line;

  public:
    /** 
     * @brief New exception with the source file & line it was thrown from, and an optional message.
     * 
     * @param file - Generally filled in from __FILE__.
     * @param line - Generally filled in from __LINE__.
     * @param msg - An optional message to associate with the exception, detailing what the program knows about the failure.
     */
    DBException(const string &file, int line, const string &msg) : m_file(file),m_msg(msg.c_str()),m_line(line) { }
    ~DBException();
    /** 
     * @brief Get the custom message provided by the thrower.
     * 
     * @return - Any text the throwing code decided to associate with the exception.
     */
    const string getMessage() { return m_msg; }
    /** 
     * @brief Get the source file that threw this exception.
     * 
     * @return - The source file (from __FILE__) that threw this exception.
     */
    const string getFile() { return m_file; }
    /** 
     * @brief Get the line in the source file that threw this exception.
     * 
     * @return - The line of source (from __LINE__) that threw this exception.
     */
    int getLine() { return m_line; }
};

typedef map<string, int> field_map; /**< A map of column/field names to their column index, for lookups */

/*!@class DBRow
 *
 * @brief A wrapper for a single row of information.
 *
 * DBRow is completely disconnected from the database, and is just a
 * value storage object.  This lets it be passed around to any level,
 * and used directly.
 */
class DBRow {
  private:
    field_map m_fields;
    vector<string> m_current;

  public:
    DBRow(MYSQL_ROW newRow, const field_map &fieldnames);
    string getCol(const field_map &names, int i);
    ~DBRow() { }
    const string getField(const string &field);
    const string dumpFields(const string &wraptag, const string &prefix="", const string &indent="  ", bool encase=false);
};

typedef auto_ptr<DBRow> Row;    /**< Hide the actual DBRow within an auto_ptr, for allocation management. */

/*!@class DBResultSet
 *
 * @brief A wrapper for managing the entire result set of a DB query.
 *
 * This is intended to behave similarly to the Java style of iterator,
 * checking hasNext, then retrieving next.  Since the real exposed object
 * is a ResultSet object (an auto_ptr wrapped DBResultSet), it needs its
 * own 'is valid' check.  This should work fine, as long as it's not
 * virtual.
 */
class DBResultSet {
  private:
    bool is_bad;

    MYSQL_RES *m_result;
    field_map m_fields;
    Row m_curRow;

    Row getRow();
  public:
    DBResultSet(MYSQL_RES *rset);
    virtual ~DBResultSet();

    /** 
     * @brief Determine if this current result set object is valid;
     * only works if isValid is not virtual, otherwise would be a
     * NULL reference just trying to call it.
     * 
     * @return - true if this is not a NULL object, false otherwise.
     */
    bool isValid() { return this != NULL; }
    /** 
     * @brief Check if there is a next row available.
     * 
     * @return - true if there is another row available, false otherwise.
     */
    bool hasNext() { return m_curRow.get() != NULL; }
    Row next();
};

typedef auto_ptr<DBResultSet> ResultSet; /**< Hide the actual DBResultSet within an auto_ptr, for allocation management. */

/*!@class DB
 *
 * @brief A general purpose database object.
 *
 * This provides the minimal interface for interacting with a
 * database, namely 'query', returning a result set.  In theory,
 * this could be an interface, and a MySQLDB class would do the
 * MySQL specific parts, but that's not necessary yet, and an
 * easy refactoring.
 */
class DB {
  private:
    SimpleLog *m_log;
    bool m_db_opened;
    MYSQL *m_dblink;
    bool m_successful;          /**< Allow querying for success when no results returned. */

  public:
    DB(const string &dbhost, const string &db, const string &user, const string &pass);
    virtual ~DB();

    void set_logging(SimpleLog *log);
    ResultSet query(const string &query);
    /** 
     * @brief Commit any current transaction.
     *
     * @return - true if the commit went through, false otherwise.
     * Always true, for now.
     */
    bool commit() { query("COMMIT"); return true; }

    void disconnect() { mysql_close(m_dblink); }

    bool is_successful() { return m_successful; }
};

#endif // DB_H
