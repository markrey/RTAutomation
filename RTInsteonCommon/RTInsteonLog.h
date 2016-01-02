////////////////////////////////////////////////////////////////////////////
//
//  This file is part of RTAutomation
//
//  Copyright (c) 2015-2016, richards-tech, LLC
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of
//  this software and associated documentation files (the "Software"), to deal in
//  the Software without restriction, including without limitation the rights to use,
//  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
//  Software, and to permit persons to whom the Software is furnished to do so,
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef _RTINSTEONLOG_H_
#define _RTINSTEONLOG_H_

#include "qstring.h"

//  Log defines

#define RTINSTEON_LOG_LEVEL_DEBUG             0                   // debug level
#define RTINSTEON_LOG_LEVEL_INFO              1                   // info level
#define RTINSTEON_LOG_LEVEL_WARN              2                   // warn level
#define RTINSTEON_LOG_LEVEL_ERROR             3                   // error level

#define RTINSTEON_LOG_LEVEL_NONE              4                   // used to display no log messages

class RTInsteonLog
{
public:
    static void logDebug(const QString& tag, const QString& msg);   // log message at debug level
    static void logInfo(const QString& tag, const QString& msg);    // log message at info level
    static void logWarn(const QString& tag, const QString& msg);    // log message at warn level
    static void logError(const QString& tag, const QString& msg);   // log message at error level
    static void setLogDisplayLevel(int level) { m_logDisplayLevel = level;} // sets the log level to be displayed

private:
    static void addLogMessage(const QString& tag, const QString& msg, int level);
    static int m_logDisplayLevel;
};

#endif //_RTINSTEONLOG_H_
