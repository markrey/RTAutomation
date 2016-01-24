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
#ifndef RC_H
#define RC_H

#include "MQTTSensorDefs.h"

#include <qstring.h>
#include <qcolor.h>
#include <qmutex.h>
#include <qdatetime.h>

class RC
{
public:
    static void init();
    static double minValue(int sensor);
    static double maxValue(int sensor);
    static const QString& name(int sensor);
    static const QString& JSONName(int sensor);
    static const QString& units(int sensor);
    static const QColor& color(int sensor);

    static bool realTime();
    static void setRealTime(bool realTime);
    static const QDateTime& currentTime();
    static void setCurrentTime(const QDateTime& time);
    static int currentDayNumber();

    static bool setSensorRemap(const QList<int>& remap);

private:
    static qreal m_minValue[MQTTSENSOR_TYPE_COUNT];
    static qreal m_maxValue[MQTTSENSOR_TYPE_COUNT];
    static QString m_JSONName[MQTTSENSOR_TYPE_COUNT];
    static QString m_name[MQTTSENSOR_TYPE_COUNT];
    static QString m_units[MQTTSENSOR_TYPE_COUNT];
    static QColor m_color[MQTTSENSOR_TYPE_COUNT];

    static bool m_realTime;
    static QDateTime m_currentTime;
    static QMutex m_lock;

    static int m_remap[MQTTSENSOR_TYPE_COUNT];
};

#endif // RC_H
