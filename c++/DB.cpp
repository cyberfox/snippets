#include <vector>
#include <sstream>

/**
 * @file      DB.cpp
 * @copyright Copyright (c) 2004 CyberFOX Software, Inc.
 * @author    Morgan Schweers <cyberfox@gmail.com>
 * @date      Mon Aug 30 17:03:38 2004
 * 
 * @brief     Database abstraction so the code isn't littered with MySQLisms.
 */

#include <mysql/mysql.h>
#include <stdio.h>
#include <string.h>

#include "DB.h"

/** 
 * @brief Construct a db object pointing to the provided database on
 * the given host, as the given user with their password.
 * 
 * @param dbhost - The host holding the database.
 * @param database - The database to connect to.
 * @param user - The user to connect as.
 * @param password - The password of the user we're connecting as.
 */
DB::DB(const string &dbhost, const string &database, const string &user, const string &password) {
  m_successful = true;
  m_dblink = mysql_init(NULL);

  MYSQL *rval = mysql_real_connect(m_dblink, dbhost.c_str(), user.c_str(), password.c_str(), database.c_str(), 0, NULL, 0);

  if(rval == NULL) throw new DBException(__FILE__, __LINE__, "Failed to connect to DB " + database + " on host " + dbhost + ".");

  m_db_opened = true;
}

/** 
 * @brief Destroy this DB object, closing its link to the database.
 * 
 */
DB::~DB() {
  if(m_db_opened) mysql_close(m_dblink);
  m_db_opened = false;
}

static const string makeTag(const string &tagName, const string &content, bool encase=false) {
  if(encase) {
    return "<" + tagName + "><![CDATA[" + content + "]]></" + tagName + ">";
  } else {
    return "<" + tagName + ">" + content + "</" + tagName + ">";
  }
}

/** 
 * @brief Get the name of a specific column, searching for its order index.
 * 
 * @param names - The map of names to column order.
 * @param i - The index of the column to retrieve the name of.
 * 
 * @return - The name, as returned from the database, of the numbered column.
 */
string DBRow::getCol(const field_map &names, int i) {
  for(field_map::const_iterator it=names.begin();
      it != names.end(); it++) {
    if(it->second == i) return it->first;
  }
  return "";
}

/** 
 * @brief Create a new DB row, based on the MySQL row type, with the
 * list of field names (column names) returned from the query that
 * generated this row.
 * 
 * @param newRow - The underlying data from the new row to create.
 * @param fieldnames - The list of field/column names to associate
 * with each piece of data.
 */
DBRow::DBRow(MYSQL_ROW newRow, const field_map &fieldnames) : m_fields(fieldnames) {
  for(int i=0; i<m_fields.size(); i++) {
    string foo = "";

    if(newRow[i] != NULL) {
      foo = newRow[i];
    }

    m_current.push_back(foo);
  }
}

/** 
 * @brief Retrieve a field based on its column name/alias.
 * 
 * @param field - The column name or alias to look up.
 * 
 * @return The text returned from the query, or an empty string if no
 * column found (or the column was empty!).
 */
const string DBRow::getField(const string &field) {
  field_map::iterator exists = m_fields.find(field);

  if(exists == m_fields.end()) return "";

  return m_current[exists->second];
}

/** 
 * @brief Dump all the fields of this row out in XML form, wrapped in
 * the provided tag, and each line prefixed by the provided prefix
 * string.
 * 
 * @param wraptag - The tag to wrap the result in.
 * @param prefix - The characters to prefix every line with.
 * @param indent - The prefix of text to use as an indentation for
 * each of the subsidiary lines of the tag.
 * @param encase - Whether or not to wrap the data in a CDATA section.  (default is false)
 * 
 * @return - An XML-formatted output string, with all columns of the
 * current row as children of the wraptag, and all lines prefixed with
 * the provided prefix string.
 */
const string DBRow::dumpFields(const string &wraptag, const string &prefix, const string &indent, bool encase) {
  ostringstream xml;
  xml << prefix << "<" << wraptag << ">" << EOL;
  for(field_map::iterator it = m_fields.begin(); it != m_fields.end(); it++) {
    xml << prefix << indent << makeTag(it->first, m_current[it->second], encase) << EOL;
  }
  xml << prefix << "</" << wraptag << ">" << EOL;

  return xml.str();
}

class NullPointerException : public std::exception { };

/** 
 * @brief Get a single row from a result set.
 *
 * Complicated by the need to return an auto_ptr pointer, so as not to
 * lose track of memory.  Makes error handling a little more difficult.
 * 
 * @return 
 */
Row DBResultSet::getRow() {
  MYSQL_ROW newRow = mysql_fetch_row(m_result);

  if(newRow == NULL) throw NullPointerException();
  Row rval(new DBRow(newRow, m_fields));

  return rval;
}

/** 
 * @brief Get the next row of results from the result set, and return
 * the previous one to the caller.
 * 
 * @return - A Row of data, as resulted from a DB->query().
 */
Row DBResultSet::next() {
  Row tmpRow = m_curRow;

  m_curRow = getRow();

  return tmpRow;
}

/** 
 * @brief Given a standard MYSQL result set, construct a result set
 * that contains the mapping of column names to column values.
 * 
 * @param rset - A MySQL style result set.
 */
DBResultSet::DBResultSet(MYSQL_RES *rset) : m_result(rset),m_curRow(NULL),is_bad(false) {
  if(rset == NULL) {
    is_bad = true;
  } else {
    for(int i=0; i<m_result->field_count; i++) {
      m_fields[m_result->fields[i].name] = i;
    }

    m_curRow = getRow();
  }
}

DBResultSet::~DBResultSet() {
  mysql_free_result(m_result);
}

/** 
 * @brief Execute an SQL query/statement.
 *
 * This can be a select, update, or anything else.  The result set
 * returned may or may not have value, based on the command provided.
 * If an error occurs, an exception will be thrown, as no errors should
 * be occurring here.  (It is, in other words, an exceptional circumstance.)
 * 
 * @param query - The query/statement to execute.
 * 
 * @return - A result set with the resultant values generated by the
 * provided SQL statement.
 */
ResultSet DB::query(const string &query) {
  int rval = mysql_query(m_dblink, query.c_str());
  if(rval != 0) {
    char buf[1024];
    sprintf(buf, "Serious Error!  We received %d (%s)!", mysql_errno(m_dblink), mysql_error(m_dblink));
    m_log->log(LOG_ERROR, buf);
    m_successful = false;
    throw NullPointerException(); /**< FIXME - throw SQLException(...); */
  }
  MYSQL_RES *results = mysql_use_result(m_dblink);
  if(results == NULL) throw NullPointerException();

  m_successful = true;
  return ResultSet(new DBResultSet(results));
}

/** 
 * @brief Set the logging destination, for any logging that the DB
 * code needs to do.
 * 
 * @param log - A log server, providing a '->log(level, string);' function.
 */
void DB::set_logging(SimpleLog *log) {
  m_log = log;
}
