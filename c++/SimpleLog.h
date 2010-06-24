#ifndef SIMPLELOG_H
#define SIMPLELOG_H

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <unistd.h>

/**
 * @file      SimpleLog.h
 * @copyright Copyright (c) 2004 CyberFOX Software, Inc.
 * @author    Morgan Schweers <cyberfox@gmail.com>
 * @date      Mon Aug 30 17:24:31 2004
 * 
 * @brief     Necessary minimal logging class, to support keeping
 *            track of all kinds of messages, from errors through
 *            trivial details.
 */

enum LogLevels { LOG_TRIVIAL,   /**< Even trivial events get logged.  */
                 LOG_INFO,      /**< Informational messages and worse, all get logged. */
                 LOG_WARNING,   /**< Only warnings and errors get logged. */
                 LOG_ERROR };   /**< Only errors get logged, even warnings are ignored. */

/*** End of Line marker for use with stream outputs. */
//@{
#define EOL "\n"
//@}

/*!@class SimpleLog
 *
 * @brief Almost the simplest form of multi-level log manager,
 * providing just enough functionality to make the program useful to
 * ops.
 */
class SimpleLog {
  private:
    bool m_valid;
    std::ofstream m_log;
    LogLevels m_level;

  public:
    SimpleLog(const char *fname);
    /** 
     * @brief Destroy and close down any logs opened.
     */
    ~SimpleLog() { m_log.close(); }

    void log(LogLevels level, std::string msg);

    /** 
     * @brief Set the logging level to a given value out of LogLevels.
     * 
     * FIXME:mrs: 02-September-2004 16:49 - We should probably return the old logging level.
     *
     * @param newLevel - The new level of logging to switch to.
     * @see LogLevels
     */
    void set_level(LogLevels newLevel) { m_level = newLevel; }

    static LogLevels get_level(std::string levelName);
};

#endif // SIMPLELOG_H
