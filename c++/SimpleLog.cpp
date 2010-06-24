#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <unistd.h>

#include <stdio.h>
#include <string.h>

#include "SimpleLog.h"

/**
 * @file      SimpleLog.cpp
 * @copyright Copyright (c) 2004 CyberFOX Software, Inc.
 * @author    Morgan Schweers <cyberfox@gmail.com>
 * @date      Thu Sep  2 16:45:51 2004
 * 
 * @brief     Implementation of the minimal logging class,
 *            keeping track of all kinds of messages and
 *            logging them to a file.
 */

/** 
 * @brief Construct a new logging object, outputting to the provided file name.
 * 
 * @param fname - The file name to open and write the log to.
 */
SimpleLog::SimpleLog(const char *fname) :
  m_valid(true),
  m_level(LOG_INFO)
{
  char *pidptr = strstr(fname, "%pid%");
  if(pidptr == NULL) {
    m_log.open(fname, std::ios::out | std::ios::app);
  } else {
    if(strlen(fname) > 256) {
      std::cerr << "Error creating log.  Filename is too long." << EOL;
      m_valid = false;
    } else {
      char buf[512];
      *pidptr = 0;
      sprintf(buf, "%s%u%s", fname, getpid(), pidptr+5);
      m_log.open(buf);
    }
  }
}

/** 
 * @brief Log a message at a given importance level to the log file;
 * if the importance level is lower than what is currently set as the
 * 'cutoff', don't write it to the log.
 * 
 * @param level - The importance level of the message.
 * @param msg - The actual message to store in the log.
 */
void SimpleLog::log(LogLevels level, std::string msg) {
  if(level >= m_level) {
    m_log << time(NULL) << ": " << msg << EOL;
    m_log.flush();
  }
}

/** 
 * @brief Translate a log level in plain text to an entry from the enum 'LogLevels'.
 *
 * This is mainly used by configuration code, so it can translate from
 * the text in a config file to a usable value.
 * 
 * @param levelName - The string name of the log level to translate.
 * 
 * @return - A value out of the enum LogLevels that corresponds with the text provided.
 */
LogLevels SimpleLog::get_level(std::string levelName) {
  transform(levelName.begin(), levelName.end(), levelName.begin(), tolower);

  if(levelName == "trivial") return LOG_TRIVIAL;
  if(levelName == "info") return LOG_INFO;
  if(levelName == "warning") return LOG_WARNING;
  if(levelName == "error") return LOG_ERROR;

  //  Default to trivial log.
  return(LOG_TRIVIAL);
}
