#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

/**
 * @file      Config.h
 * @copyright Copyright (c) 2004 CyberFOX Software, Inc.
 * @author    Morgan Schweers <cyberfox@gmail.com>
 * @date      Mon Aug 30 17:06:26 2004
 * 
 * @brief     Interface to a simple configuration system, allowing
 *            comments, whitespace around keys and values, and
 *            easy simple boolean checks.
 */

using namespace std;

/**
 * @brief A simple class to wrap each line of the configuration file.
 * 
 */
class configLine {
  private:
    string m_line;
    int m_lineno;
    string m_prefix;
    string m_suffix;

  public:
    static string trim(const string &untrimmed);
    /** 
     * @brief Given a line from the configuration file, split it, and save the key and values.
     * 
     * @param line - The line of configuration info to parse.
     * @param lineno - The line number, preserved for potential error use.
     */
    configLine(string line, int lineno) {
      m_line = line;
      m_lineno = lineno;
      m_prefix = trim(line.substr(0, line.find('=')));
      m_suffix = trim(line.substr(line.find('=')+1));
    }
    /** 
     * @brief Get the entire text of the line this configuration data was on.
     * 
     * @return - The original line, including all spaces, etc., from the config file.
     */
    string getLine() { return m_line; }
    /** 
     * @brief Get the line number of the line this configuration data
     * was on, for error handling purposes mainly.
     * 
     * @return - The line number out of the config file, as provided by the initial parser.
     */
    int getLineNo() { return m_lineno; }
    /** 
     * @brief Get the 'key' part of a given configuration line.
     * 
     * @return - The part before the '=' on the current line of the configuration file.
     */
    string getKey() { return m_prefix; }
    /** 
     * @brief Get the 'value' part of a given configuration line.
     * 
     * @return - The part after the '=' on the current line of the configuration file.
     */
    string getValue() { return m_suffix; }
};

typedef vector<configLine> lines_t; /**< Contains all the lines of the configuration file */
typedef map<string,string> cfg_entries; /**< Contains a map of the configuration keys to the configuration values. */

/**
 * @brief A class to manage configuration information loaded from a file.
 *
 * The configuration file has a format roughly like this:
 * <pre>
 *    key = value
 * e.g.
 *    mail.command = sendmail -t
 *    dbhost       = dbserver
 * </pre>
 */
class Config {
  private:
    cfg_entries m_entries;
    lines_t loadLines(const string &fname);
    string m_cfgfile;

  public:
    //  Load the configuration from the provided configuration file.
    Config(const string &fname);
    ~Config() { }

    bool queryBoolean(const string &config, bool defBool=false);
    string query(const string &config, const string &defStr="");
    /** 
     * @brief Return the configuration file originally loaded from.
     *
     * This is especially useful to code that wants to throw out the
     * old configuration, and reload from the same file name.
     * 
     * @return - The original file name that the configuration object
     * was created with.
     */
    string get_config_file() { return m_cfgfile; }
};
