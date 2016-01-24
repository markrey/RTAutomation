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

#ifndef MQTTSENSOR_H
#define MQTTSENSOR_H

#include <qobject.h>
#include <qstringlist.h>
#include <qqueue.h>
#include <qcolor.h>
#include <qdatetime.h>

#include "MQTTSensorDefs.h"

#define MQTTSENSOR_CHUNKS_PER_RECORD 2

#define MQTTSENSOR_HISTORY_CHUNK (MQTTSENSOR_RECORD_LENGTH / MQTTSENSOR_CHUNKS_PER_RECORD)

#define MQTTSENSOR_HISTORY_INTERVAL (MQTTSENSOR_RECORD_TIMEMS / MQTTSENSOR_CHUNKS_PER_RECORD)

typedef struct
{
    QList <MQTTSENSOR_DATA> sensorList;
    int currentDataCount;

    // currentTime means different things for real time and historic displays unfortunately.
    // For real time, currentTime is actually the time of the last entry in the store.
    // For historic, currentTime is actually the time of the start of the store.
    // The access function currentTime() takes care of this automatically and always returns
    // the time of the end of the store.

    qint64 currentTime;
} MQTTSENSOR_STORE;


class MQTTSensor : public QObject
{
    Q_OBJECT

public:
    MQTTSensor(qint64 startTime = -1, qint64 endTime = -1);

    QStringList sensors() const;
    QStringList values() const;
    QStringList values(int index) const;
    bool valid(int sensor) const;
    bool valid(int sensor, int index) const;
    double value(int sensor) const;
    double value(int sensor, int index) const;
    qint64 currentTime();
    const MQTTSENSOR_DATA *sensorData(int sensor) const;

    bool setRealTimeData(const QJsonObject& json);

    void newCurrentTime();

    QString m_name;


private:
    void initStore(MQTTSENSOR_STORE *info);
    void sensorsToDefaults(MQTTSENSOR_STORE *info);

    qint64 m_startTime;
    qint64 m_endTime;

    int m_entryCount;

    MQTTSENSOR_STORE m_realTime;
};

#endif // MQTTSENSOR_H
