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

#include "RC.h"
#include "MQTTSensor.h"
#include "RTMQTTJSON.h"

#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qdebug.h>

//#define AWAREDB_DEBUG

MQTTSensor::MQTTSensor(qint64 startTime, qint64 endTime)
{
    m_startTime = startTime;
    m_endTime = endTime;

    if ((m_startTime == -1) || (m_endTime == -1)) {
        m_entryCount = MQTTSENSOR_RECORD_LENGTH;
    } else {
        m_entryCount = (endTime / MQTTSENSOR_TIME_INTERVAL) - (startTime / MQTTSENSOR_TIME_INTERVAL);
    }
    initStore(&m_realTime);
}

void MQTTSensor::initStore(MQTTSENSOR_STORE *info)
{
    for (int i = 0; i < MQTTSENSOR_TYPE_COUNT; i++) {
        info->sensorList.append(MQTTSENSOR_DATA());
        info->sensorList[i].currentData = 0;
        info->sensorList[i].valid = false;
    }


    info->currentDataCount = 0;
    info->currentTime = 0;

    for (int sensor = 0; sensor < MQTTSENSOR_TYPE_COUNT; sensor++) {
        int minValue = RC::minValue(sensor);
        info->sensorList[sensor].value = minValue;
        for (int index = 0; index < m_entryCount; index++)
            info->sensorList[sensor].record.enqueue(minValue);
    }

}


void MQTTSensor::sensorsToDefaults(MQTTSENSOR_STORE *info)
{
    for (int sensor = 0; sensor < MQTTSENSOR_TYPE_COUNT; sensor++) {
        info->sensorList[sensor].value = MQTTSENSOR_NO_DATA;
        for (int index = 0; index < m_entryCount; index++)
            info->sensorList[sensor].record[index] = MQTTSENSOR_NO_DATA;
    }
}

QStringList MQTTSensor::sensors() const
{
    QStringList result;

    for (int sensor = 0; sensor < MQTTSENSOR_TYPE_COUNT; sensor++) {
        if (m_realTime.sensorList.at(sensor).valid)
            result << RC::name(sensor);
    }
    return result;
}


QStringList MQTTSensor::values() const
{
    QStringList result;

    for (int sensor = 0; sensor < MQTTSENSOR_TYPE_COUNT; sensor++) {
        if (m_realTime.sensorList.at(sensor).valid) {
            qreal val = m_realTime.sensorList.at(sensor).value;
            if (val == MQTTSENSOR_NO_DATA)
                result << "";
            else
                result << QString::number(val, 'f', 2) + " " + RC::units(sensor);
        }
    }
    return result;
}


double MQTTSensor::value(int sensor) const
{
    if ((sensor < 0) || (sensor >= MQTTSENSOR_TYPE_COUNT))
        return 0;
    return m_realTime.sensorList[sensor].value;
}

bool MQTTSensor::valid(int sensor) const
{
    if ((sensor < 0) || (sensor >= MQTTSENSOR_TYPE_COUNT))
        return false;
    return m_realTime.sensorList[sensor].valid;
}

const MQTTSENSOR_DATA *MQTTSensor::sensorData(int sensor) const
{
    if ((sensor < 0) || (sensor >= MQTTSENSOR_TYPE_COUNT))
        return NULL;
    return &(m_realTime.sensorList[sensor]);
}

qint64 MQTTSensor::currentTime()
{
    return m_realTime.currentTime;
}

bool MQTTSensor::setRealTimeData(const QJsonObject& json)
{
    qreal tempData[MQTTSENSOR_TYPE_COUNT];

    //  process JSON for each sensor type

    for (int sensor = 0; sensor < MQTTSENSOR_TYPE_COUNT; sensor++) {
        tempData[sensor] = 0;
        if (!json.contains(RC::JSONName(sensor))) {
             continue;                                       // this one not there
        }
        if (RC::JSONName(sensor) == RTMQTTJSON_ACCEL_DATA) {
            // special case for accel - need to convert array to three values
            QJsonArray ja = json[RTMQTTJSON_ACCEL_DATA].toArray();
            tempData[sensor] = ja[0].toDouble();          // X
            m_realTime.sensorList[sensor++].valid = true;
            tempData[sensor] = ja[1].toDouble();          // Y
            m_realTime.sensorList[sensor++].valid = true;
            tempData[sensor] = ja[2].toDouble();            // Z
            m_realTime.sensorList[sensor].valid = true;
        } else {
            m_realTime.sensorList[sensor].valid = true;
            tempData[sensor] = json.value(RC::JSONName(sensor)).toDouble();
        }
    }

    double timestamp = json[RTMQTTJSON_TIMESTAMP].toDouble() * 1000;    // change to mS units

    //  now decide what to do with the new reading

    if (m_realTime.currentDataCount == 0) {
        // first time
        m_realTime.currentTime = timestamp;
        for (int i = 0; i < MQTTSENSOR_TYPE_COUNT; i++)
            m_realTime.sensorList[i].currentData = tempData[i];
        m_realTime.currentDataCount = 1;
    } else {
        // not first time - calculate time since last result added to record
        int timeUnits = (timestamp - m_realTime.currentTime) / MQTTSENSOR_TIME_INTERVAL;
        if (timeUnits > 0) {
            // compute average over time
            for (int i = 0; i < MQTTSENSOR_TYPE_COUNT; i++)
                m_realTime.sensorList[i].value = m_realTime.sensorList[i].currentData / m_realTime.currentDataCount;

            // need to add some data to record
            m_realTime.currentTime += timeUnits * MQTTSENSOR_TIME_INTERVAL;

            // shift in required number of copies of current data
            for (int time = 0; time < timeUnits; time++) {
                for (int sensor = 0; sensor < MQTTSENSOR_TYPE_COUNT; sensor++) {
                    m_realTime.sensorList[sensor].record.dequeue();
                    m_realTime.sensorList[sensor].record.enqueue(m_realTime.sensorList[sensor].value);
                }
            }
            // reset for next time
            for (int sensor = 0; sensor < MQTTSENSOR_TYPE_COUNT; sensor++)
               m_realTime.sensorList[sensor].currentData = 0;
            m_realTime.currentDataCount = 0;
        }
        // now just accumulate new sample
        for (int sensor = 0; sensor < MQTTSENSOR_TYPE_COUNT; sensor++)
            m_realTime.sensorList[sensor].currentData += tempData[sensor];
        m_realTime.currentDataCount++;
    }
    return true;
}
