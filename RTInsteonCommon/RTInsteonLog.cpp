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

#include "RTInsteonLog.h"

#include "qdatetime.h"
#include "qdebug.h"

//  log level

#ifdef QT_NO_DEBUG
int RTInsteonLog::m_logDisplayLevel = RTINSTEON_LOG_LEVEL_WARN;
#else
int RTInsteonLog::m_logDisplayLevel = RTINSTEON_LOG_LEVEL_DEBUG;
#endif

void RTInsteonLog::logDebug(const QString& tag, const QString& msg) { RTInsteonLog::addLogMessage(tag, msg, RTINSTEON_LOG_LEVEL_DEBUG); }
void RTInsteonLog::logInfo(const QString& tag, const QString& msg) { RTInsteonLog::addLogMessage(tag, msg,  RTINSTEON_LOG_LEVEL_INFO); }
void RTInsteonLog::logWarn(const QString& tag, const QString& msg) { RTInsteonLog::addLogMessage(tag, msg,  RTINSTEON_LOG_LEVEL_WARN); }
void RTInsteonLog::logError(const QString& tag, const QString& msg) { RTInsteonLog::addLogMessage(tag, msg,  RTINSTEON_LOG_LEVEL_ERROR); }


void RTInsteonLog::addLogMessage(const QString& tag, const QString& msg, int level)
{
    if (level < m_logDisplayLevel)
        return;

    QString logMsg(QDateTime::currentDateTime().toString() + " " + tag + "(" + QString::number(level) + "): " + msg);
    qDebug() << qPrintable(logMsg);
}
