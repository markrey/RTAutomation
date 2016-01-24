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

#include "DeviceDatabase.h"
#include "RTAutomationLog.h"
#include "MQTTSensor.h"
#include "WebClient.h"
#include "RTMQTTJSON.h"

#include <qdebug.h>
#include <qfile.h>
#include <qsettings.h>
#include <qjsondocument.h>

#define TAG "DeviceDatabase"

DeviceDatabase *g_deviceDatabase;

DeviceDatabase::DeviceDatabase()
{
    g_deviceDatabase = this;

    m_deviceTypeStrings << "Insteon server" << "RTMQTT video" << "RTMQTT audio" << "RTMQTT sensor";
    m_deviceUnknown = "Unknown";
    m_subTopics << "status" << "video" << "audio" << "sensors";
    m_topicUnknown = "unknown";
}

DeviceDatabase::~DeviceDatabase()
{
    qDebug() << "~DeviceDatabase";
    QMutexLocker lock(&m_lock);

    DeviceData *dev;

    QMapIterator<QString, DeviceData *> i(m_deviceData);
    while (i.hasNext()) {
        i.next();
        dev = i.value();
        deleteSensorData(dev);
        delete dev;
    }

    m_deviceData.clear();
}

void DeviceDatabase::loadDatabase()
{
    QSettings settings;

    settings.beginGroup(DEVICEDATABASE_DEVICE_GROUP);

    QStringList devices = settings.childGroups();

    for (int i = 0; i < devices.count(); i++) {
        QString deviceName = devices.at(i);
        settings.beginGroup(deviceName);
        int deviceType = settings.value(DEVICEDATABASE_DEVICE_TYPE, 0).toInt();
        settings.endGroup();

        addDevice(deviceName, deviceType);
    }

    settings.endGroup();
}

void DeviceDatabase::addDevice(QString deviceName, int deviceType)
{
    QMutexLocker lock(&m_lock);

    if (m_deviceData.contains(deviceName)) {
        RTAutomationLog::logError(TAG, QString("Tried to add device %1 but already in database").arg(deviceName));
        return;
    }

    DeviceData *dev = new DeviceData;

    dev->m_name = deviceName;
    dev->m_deviceType = deviceType;
    dev->m_active = true;
    if (deviceType == DEVICE_TYPE_RTMQTT_SENSOR)
        dev->m_sensor = new MQTTSensor();
    else
        dev->m_sensor = NULL;

    m_deviceData[deviceName] = dev;
    emit addClientDevice(dev->m_name, dev->m_deviceType);

    QSettings settings;

    settings.beginGroup(DEVICEDATABASE_DEVICE_GROUP);

    settings.beginGroup(deviceName);
    settings.setValue(DEVICEDATABASE_DEVICE_TYPE, deviceType);
    settings.endGroup();

    settings.endGroup();
}

QStringList DeviceDatabase::getDeviceList()
{
    QMutexLocker lock(&m_lock);

    QStringList list;

    QMapIterator<QString, DeviceData *> i(m_deviceData);
    while (i.hasNext()) {
        i.next();
        list.append(i.key());
    }
    return list;
}

void DeviceDatabase::removeDevice(QString deviceName)
{
    QMutexLocker lock(&m_lock);

    DeviceData *dev = m_deviceData.value(deviceName);
    if (dev == NULL) {
        RTAutomationLog::logError(TAG, QString("Tried to delete device %1 but device not found").arg(deviceName));
        return;
    }
    m_deviceData.remove(deviceName);
    emit removeClientDevice(dev->m_name, dev->m_deviceType);

    deleteSensorData(dev);
    delete dev;

    QSettings settings;

    settings.beginGroup(DEVICEDATABASE_DEVICE_GROUP);
    settings.remove(deviceName);
    settings.endGroup();
}

MQTTSensor *DeviceDatabase::getSensorData(const QString &deviceName)
{
    QMutexLocker lock(&m_lock);

    DeviceData *dev = m_deviceData.value(deviceName);

    if (dev == NULL) {
        RTAutomationLog::logError(TAG, QString("Tried to get sensor data on non-existent device ") + deviceName);
        return NULL;
    }
    return dev->m_sensor;
}

Thumbnail DeviceDatabase::getThumbnail(const QString &deviceName)
{
    QMutexLocker lock(&m_lock);

    DeviceData *dev = m_deviceData.value(deviceName);

    if (dev == NULL) {
        RTAutomationLog::logError(TAG, QString("Tried to get thumbnail on non-existent device ") + deviceName);
        return Thumbnail();
    }
    return dev->m_thumbnail;
}

void DeviceDatabase::setThumbnailImage(const QString &deviceName, QByteArray jpeg)
{
    QMutexLocker lock(&m_lock);

    DeviceData *dev = m_deviceData.value(deviceName);

    if (dev == NULL) {
        RTAutomationLog::logError(TAG, QString("Tried to set thumbnail image on non-existent device ") + deviceName);
        return;
    }
    dev->m_thumbnail.m_jpeg = jpeg;
    dev->m_thumbnail.m_lastThumbnailTime = QDateTime::currentMSecsSinceEpoch();
}

bool DeviceDatabase::timeForThumbnail(const QString& deviceName)
{
    QMutexLocker lock(&m_lock);

    DeviceData *dev = m_deviceData.value(deviceName);

    if (dev == NULL) {
        RTAutomationLog::logError(TAG, QString("Tried to check thumbnail time on non-existent device ") + deviceName);
        return false;
    }

    return (QDateTime::currentMSecsSinceEpoch() - dev->m_thumbnail.m_lastThumbnailTime) >= DEVICE_THUMBNAIL_INTERVAL;
 }

void DeviceDatabase::newThumbnail(QString deviceName, const QByteArray& bFrame, int width, int height)
{
    QMutexLocker lock(&m_lock);

    if (width == 0)
        return;

    DeviceData *dev = m_deviceData.value(deviceName);

    if (dev == NULL) {
        RTAutomationLog::logError(TAG, QString("Tried to set new thumbnail image on non-existent device ") + deviceName);
        return;
    }
    dev->m_thumbnail.m_jpeg = bFrame;
    dev->m_thumbnail.m_width = width;
    dev->m_thumbnail.m_height = height;
    dev->m_thumbnail.m_lastThumbnailTime = QDateTime::currentMSecsSinceEpoch();
}

void DeviceDatabase::newSensorData(const QString &deviceName, const QJsonObject &json)
{
    QMutexLocker lock(&m_lock);

    DeviceData *dev = m_deviceData.value(deviceName);

    if (dev == NULL) {
        RTAutomationLog::logError(TAG, QString("Tried to set new sensor data on non-existent device ") + deviceName);
        return;
    }
    if (dev->m_sensor == NULL) {
        RTAutomationLog::logError(TAG, QString("Tried to set new sensor data on wrong type of device ") + deviceName);
        return;
    }
    dev->m_sensor->setRealTimeData(json);
}

int DeviceDatabase::getDeviceType(const QString &deviceName)
{
    QMutexLocker lock(&m_lock);

    DeviceData *dev = m_deviceData.value(deviceName);

    if (dev == NULL) {
        RTAutomationLog::logError(TAG, QString("Tried to get device type on non-existent device ") + deviceName);
        return -1;
    }
    return dev->m_deviceType;
}


void DeviceDatabase::setDeviceType(const QString &deviceName, int deviceType)
{
    QMutexLocker lock(&m_lock);

    DeviceData *dev = m_deviceData.value(deviceName);

    if (dev == NULL) {
        RTAutomationLog::logError(TAG, QString("Tried to set device type on non-existent device ") + deviceName);
        return;
    }

    if (dev->m_deviceType == deviceType)
        return;

    if (deviceType == DEVICE_TYPE_RTMQTT_SENSOR) {
        deleteSensorData(dev);
        dev->m_sensor = new MQTTSensor();
    } else {
        deleteSensorData(dev);
    }

    emit removeClientDevice(dev->m_name, dev->m_deviceType);

    dev->m_deviceType = deviceType;

    emit addClientDevice(dev->m_name, dev->m_deviceType);

    QSettings settings;

    settings.beginGroup(DEVICEDATABASE_DEVICE_GROUP);

    settings.beginGroup(deviceName);
    settings.setValue(DEVICEDATABASE_DEVICE_TYPE, deviceType);
    settings.endGroup();

    settings.endGroup();
}

const QString& DeviceDatabase::displayType(int deviceType)
{
    if ((deviceType >= 0) && (deviceType < m_deviceTypeStrings.count()))
        return m_deviceTypeStrings.at(deviceType);
    return m_deviceUnknown;
}

const QString& DeviceDatabase::getSubTopic(int deviceType)
{
    if ((deviceType < 0) || (deviceType >= m_subTopics.count()))
        return m_topicUnknown;
    return m_subTopics.at(deviceType);
}

int DeviceDatabase::getTypeFromSubTopic(const QString &subTopic)
{
    return m_subTopics.indexOf(subTopic);
}

const QStringList& DeviceDatabase::listDeviceTypes()
{
    return m_deviceTypeStrings;
}

int DeviceDatabase::mapStringTypeToType(const QString &stringType)
{
    for (int i = 0; i < m_deviceTypeStrings.count(); i++) {
        if (stringType == m_deviceTypeStrings.at(i))
            return i;
    }
    return -1;
}

Thumbnail::Thumbnail()
{
    m_lastThumbnailTime = QDateTime::currentMSecsSinceEpoch() - DEVICE_THUMBNAIL_INTERVAL;
              QFile defaultFrame(":/Images/DefaultFrame.jpg");

    defaultFrame.open(QIODevice::ReadOnly);
    m_jpeg = defaultFrame.read(1000000);
    m_width = 640;
    m_height = 480;
    defaultFrame.close();
}

//  Note - private functions below assume already locked

void DeviceDatabase::deleteSensorData(DeviceData *dev)
{
    if (dev->m_sensor == NULL)
        return;

    m_discardQueue.append(dev->m_sensor);
    dev->m_sensor = NULL;
}

