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
#include "RTMQTTJSON.h"

qreal RC::m_minValue[MQTTSENSOR_TYPE_COUNT];
qreal RC::m_maxValue[MQTTSENSOR_TYPE_COUNT];
QString RC::m_JSONName[MQTTSENSOR_TYPE_COUNT];
QString RC::m_name[MQTTSENSOR_TYPE_COUNT];
QString RC::m_units[MQTTSENSOR_TYPE_COUNT];
QColor RC::m_color[MQTTSENSOR_TYPE_COUNT];
int RC::m_remap[MQTTSENSOR_TYPE_COUNT];

bool RC::m_realTime;
QDateTime RC::m_currentTime;

QMutex RC::m_lock;

void RC::init()
{
    m_JSONName[MQTTSENSOR_TYPE_ACCELX] = RTMQTTJSON_ACCEL_DATA;
    m_JSONName[MQTTSENSOR_TYPE_ACCELY] = RTMQTTJSON_ACCEL_DATA;
    m_JSONName[MQTTSENSOR_TYPE_ACCELZ] = RTMQTTJSON_ACCEL_DATA;
    m_JSONName[MQTTSENSOR_TYPE_LIGHT] = RTMQTTJSON_LIGHT_DATA;
    m_JSONName[MQTTSENSOR_TYPE_TEMPERATURE] = RTMQTTJSON_TEMPERATURE_DATA;
    m_JSONName[MQTTSENSOR_TYPE_PRESSURE] = RTMQTTJSON_PRESSURE_DATA;
    m_JSONName[MQTTSENSOR_TYPE_HUMIDITY] = RTMQTTJSON_HUMIDITY_DATA;

    m_name[MQTTSENSOR_TYPE_ACCELX] = MQTTSENSOR_NAME_ACCELX;
    m_name[MQTTSENSOR_TYPE_ACCELY] = MQTTSENSOR_NAME_ACCELY;
    m_name[MQTTSENSOR_TYPE_ACCELZ] = MQTTSENSOR_NAME_ACCELZ;
    m_name[MQTTSENSOR_TYPE_LIGHT] = MQTTSENSOR_NAME_LIGHT;
    m_name[MQTTSENSOR_TYPE_TEMPERATURE] = MQTTSENSOR_NAME_TEMPERATURE;
    m_name[MQTTSENSOR_TYPE_PRESSURE] = MQTTSENSOR_NAME_PRESSURE;
    m_name[MQTTSENSOR_TYPE_HUMIDITY] = MQTTSENSOR_NAME_HUMIDITY;

    m_minValue[MQTTSENSOR_TYPE_ACCELX] = MQTTSENSOR_MIN_ACCELX;
    m_minValue[MQTTSENSOR_TYPE_ACCELY] = MQTTSENSOR_MIN_ACCELY;
    m_minValue[MQTTSENSOR_TYPE_ACCELZ] = MQTTSENSOR_MIN_ACCELZ;
    m_minValue[MQTTSENSOR_TYPE_LIGHT] = MQTTSENSOR_MIN_LIGHT;
    m_minValue[MQTTSENSOR_TYPE_TEMPERATURE] = MQTTSENSOR_MIN_TEMPERATURE;
    m_minValue[MQTTSENSOR_TYPE_PRESSURE] = MQTTSENSOR_MIN_PRESSURE;
    m_minValue[MQTTSENSOR_TYPE_HUMIDITY] = MQTTSENSOR_MIN_HUMIDITY;

    m_maxValue[MQTTSENSOR_TYPE_ACCELX] = MQTTSENSOR_MAX_ACCELX;
    m_maxValue[MQTTSENSOR_TYPE_ACCELY] = MQTTSENSOR_MAX_ACCELY;
    m_maxValue[MQTTSENSOR_TYPE_ACCELZ] = MQTTSENSOR_MAX_ACCELZ;
    m_maxValue[MQTTSENSOR_TYPE_LIGHT] = MQTTSENSOR_MAX_LIGHT;
    m_maxValue[MQTTSENSOR_TYPE_TEMPERATURE] = MQTTSENSOR_MAX_TEMPERATURE;
    m_maxValue[MQTTSENSOR_TYPE_PRESSURE] = MQTTSENSOR_MAX_PRESSURE;
    m_maxValue[MQTTSENSOR_TYPE_HUMIDITY] = MQTTSENSOR_MAX_HUMIDITY;

    m_units[MQTTSENSOR_TYPE_ACCELX] = MQTTSENSOR_UNITS_ACCELX;
    m_units[MQTTSENSOR_TYPE_ACCELY] = MQTTSENSOR_UNITS_ACCELY;
    m_units[MQTTSENSOR_TYPE_ACCELZ] = MQTTSENSOR_UNITS_ACCELZ;
    m_units[MQTTSENSOR_TYPE_LIGHT] = MQTTSENSOR_UNITS_LIGHT;
    m_units[MQTTSENSOR_TYPE_TEMPERATURE] = MQTTSENSOR_UNITS_TEMPERATURE;
    m_units[MQTTSENSOR_TYPE_PRESSURE] = MQTTSENSOR_UNITS_PRESSURE;
    m_units[MQTTSENSOR_TYPE_HUMIDITY] = MQTTSENSOR_UNITS_HUMIDITY;

    m_color[MQTTSENSOR_TYPE_ACCELX] = MQTTSENSOR_COLOR_ACCELX;
    m_color[MQTTSENSOR_TYPE_ACCELY] = MQTTSENSOR_COLOR_ACCELY;
    m_color[MQTTSENSOR_TYPE_ACCELZ] = MQTTSENSOR_COLOR_ACCELZ;
    m_color[MQTTSENSOR_TYPE_LIGHT] = MQTTSENSOR_COLOR_LIGHT;
    m_color[MQTTSENSOR_TYPE_TEMPERATURE] = MQTTSENSOR_COLOR_TEMPERATURE;
    m_color[MQTTSENSOR_TYPE_PRESSURE] = MQTTSENSOR_COLOR_PRESSURE;
    m_color[MQTTSENSOR_TYPE_HUMIDITY] = MQTTSENSOR_COLOR_HUMIDITY;

    m_realTime = true;
    m_currentTime = QDateTime::currentDateTime();

    for (int sensor = 0; sensor < MQTTSENSOR_TYPE_COUNT; sensor++)
        m_remap[sensor] = sensor;
}

bool RC::setSensorRemap(const QList<int> &remap)
{
    if (remap.count() != MQTTSENSOR_TYPE_COUNT)
        return false;

    for (int sensor = 0; sensor < MQTTSENSOR_TYPE_COUNT; sensor++) {
        if ((remap.at(sensor) < 0) || (remap.at(sensor) >= MQTTSENSOR_TYPE_COUNT))
            return false;
    }

    for (int sensor = 0; sensor < MQTTSENSOR_TYPE_COUNT; sensor++) {
        m_remap[sensor] = remap.at(sensor);
    }
    return true;
}

double RC::minValue(int sensor)
{
    QMutexLocker lock(&m_lock);

    if ((sensor < 0) || (sensor >= MQTTSENSOR_TYPE_COUNT))
        return 0;
    return m_minValue[m_remap[sensor]];
}

double RC::maxValue(int sensor)
{
    QMutexLocker lock(&m_lock);

    if ((sensor < 0) || (sensor >= MQTTSENSOR_TYPE_COUNT))
        return 0;
    return m_maxValue[m_remap[sensor]];
}

const QString& RC::JSONName(int sensor)
{
    QMutexLocker lock(&m_lock);

    if ((sensor < 0) || (sensor >= MQTTSENSOR_TYPE_COUNT))
        return m_JSONName[0];
    return m_JSONName[m_remap[sensor]];
}

const QString& RC::name(int sensor)
{
    QMutexLocker lock(&m_lock);

    if ((sensor < 0) || (sensor >= MQTTSENSOR_TYPE_COUNT))
        return m_name[0];
    return m_name[m_remap[sensor]];
}

const QString& RC::units(int sensor)
{
    QMutexLocker lock(&m_lock);

    if ((sensor < 0) || (sensor >= MQTTSENSOR_TYPE_COUNT))
        return m_units[0];
    return m_units[m_remap[sensor]];
}

const QColor& RC::color(int sensor)
{
    QMutexLocker lock(&m_lock);

    if ((sensor < 0) || (sensor >= MQTTSENSOR_TYPE_COUNT))
        return m_color[0];
    return m_color[m_remap[sensor]];
}

bool RC::realTime()
{
    QMutexLocker lock(&m_lock);

    return m_realTime;
}

void RC::setRealTime(bool realTime)
{
    QMutexLocker lock(&m_lock);

    m_realTime = realTime;
}

const QDateTime& RC::currentTime()
{
    QMutexLocker lock(&m_lock);

    return m_currentTime;
}

void RC::setCurrentTime(const QDateTime& time)
{
    QMutexLocker lock(&m_lock);

    if (time.toMSecsSinceEpoch() > QDateTime::currentMSecsSinceEpoch())
        m_currentTime = QDateTime::currentDateTime();
    else
        m_currentTime = time;
}

int RC::currentDayNumber()
{
   return -m_currentTime.daysTo(QDateTime(QDate(1970, 1, 1)));
}
