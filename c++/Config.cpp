#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

#include <string.h>

/**
 * @file      Config.cpp
 * @copyright Copyright (c) 2004 CyberFOX Software, Inc.
 * @author    Morgan Schweers <cyberfox@gmail.com>
 * @date      Mon Aug 30 17:07:41 2004
 * 
 * @brief     Interface to a simple configuration system, allowing
 *            comments, whitespace around keys and values, and
 *            easy simple boolean checks.
 */

#include "Config.h"

/** 
 * @brief Trim whitespace (space and tab) from the end and beginning.
 * 
 * @param untrimmed - The untrimmed string to strip.
 * 
 * @return - A string containing just the non-whitespace-surrounded text.
 */
string configLine::trim(const string &untrimmed) {
  if(untrimmed == "") return untrimmed;
  size_t start = untrimmed.find_first_not_of(" \t");
  size_t end = untrimmed.find_last_not_of(" \t")+1;

  if(start != end && end > 0) {
    if(untrimmed[start] == '"') start++;
    if(untrimmed[end-1] == '"') end--;
  }

  return untrimmed.substr(start, (end-start));
}

#define INCLUDE_CMD "include"

lines_t Config::loadLines(const string &fname) {
  int curline = 0;
  ifstream fin(fname.c_str());
  string step;
  lines_t file_lines;

  while(!fin.eof()) {
    getline(fin, step);
    curline++;
    if(step[0] != '#') {
      if(step.substr(0, strlen(INCLUDE_CMD)) == INCLUDE_CMD) {
        lines_t subfile = loadLines(configLine::trim(step.substr(strlen(INCLUDE_CMD)+1)));
        file_lines.insert(file_lines.end(), subfile.begin(), subfile.end());
      } else {
        if(step != "") {
          configLine thisLine(step, curline);
          file_lines.push_back(thisLine);
        }
      }
    }
  }

  fin.close();
  return file_lines;
}

/** 
 * @brief Config constructor; builds the internal configuration
 * information out of the data in the provided file.
 * 
 * @param fname - The file name to load configuration information from.
 */
Config::Config(const string &fname) : m_cfgfile(fname) {
  lines_t lines = loadLines(fname);
  for(lines_t::iterator it = lines.begin(); it != lines.end(); it++) {
    m_entries[it->getKey()] = it->getValue();
  }
}

/** 
 * @brief Check a configuration value that should be only true or
 * false.
 *
 * It actually allows 'true', 't', 'yes', 'y', and '1' to mean true,
 * and everything else means false.
 * 
 * @param config - The configuration key to search for.
 * @param defBool - The default boolean result, if no matching key found.
 * 
 * @return - true if the provided config key has a value, and it is
 * one of the values recognized as 'true'.  If there is no matching
 * configuration key, and no default has been provided, it returns
 * false.
 */
bool Config::queryBoolean(const string &config, bool defBool) {
  cfg_entries::iterator it = m_entries.find(config);
  if(it == m_entries.end()) return defBool;

  return(it->second == "true" || it->second == "t" ||
         it->second == "yes" || it->second == "y" ||
         it->second == "1");
}

/** 
 * @brief Look up a configuration key, returning the associated value
 * if one exists, or the default provided string if there is no match.
 * 
 * @param config - The configuration key to search for.
 * @param defStr - The default result, if no matching key found.
 * 
 * @return - A string containing the matching configuration value for
 * the provided key, or the default passed string if the key does not
 * exist in the configuration.  If no default string is provided, the
 * empty string is returned.
 */
string Config::query(const string &config, const string &defStr) {
  cfg_entries::iterator it = m_entries.find(config);
  if(it == m_entries.end()) return defStr;

  return it->second;
}
