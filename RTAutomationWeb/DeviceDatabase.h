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

#ifndef _DEVICEDATABASE_H
#define _DEVICEDATABASE_H

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qmutex.h>
#include <qmap.h>
#include <qqueue.h>
#include <qdatetime.h>

//  Device types

#define DEVICE_TYPE_INSTEON_SERVER      0
#define DEVICE_TYPE_RTMQTT_VIDEO        1
#define DEVICE_TYPE_RTMQTT_AUDIO        2
#define DEVICE_TYPE_RTMQTT_SENSOR       3

#define DEVICE_THUMBNAIL_INTERVAL       1000                // interval betwen thumbnail generation

//  Settings entries

#define DEVICEDATABASE_DEVICE_GROUP     "deviceDatabaseGroup"
#define DEVICEDATABASE_DEVICE_TYPE      "deviceType"

class MQTTSensor;

class Thumbnail
{
public:
    Thumbnail();
    QByteArray m_jpeg;                                  // the jpeg image
    int m_width;                                        // the width
    int m_height;                                       // the height
    qint64 m_lastThumbnailTime;                         // last time a thumbnail was generated
};

class DeviceData
{
public:
    DeviceData() { m_active = false; }

    int m_deviceType;                                   // the type of the device
    QString m_name;                                     // forms the root of the MQTT topic
    bool m_active;                                      // if entry is meaningful and in use
    Thumbnail m_thumbnail;
    MQTTSensor *m_sensor;                               // the sensor data
};

class DeviceDatabase : public QObject
{
    Q_OBJECT

public:
    DeviceDatabase();
    virtual ~DeviceDatabase();

    void loadDatabase();                                    // get info from settings file
    QStringList getDeviceList();

    int getDeviceType(const QString& deviceName);
    void setDeviceType(const QString& deviceName, int deviceType);

    Thumbnail getThumbnail(const QString& deviceName);
    void setThumbnailImage(const QString& deviceName, QByteArray jpeg);
    bool timeForThumbnail(const QString& deviceName);

    MQTTSensor* getSensorData(const QString& deviceName);

    const QString& displayType(int deviceType);
    const QStringList& listDeviceTypes();
    int mapStringTypeToType(const QString& stringType);

    const QString& getSubTopic(int deviceType);
    int getTypeFromSubTopic(const QString& subTopic);

    void newSensorData(const QString& deviceName, const QJsonObject& json);

public slots:
    void newThumbnail(QString, const QByteArray&, int, int);
    void addDevice(QString deviceName, int deviceType);
    void removeDevice(QString deviceName);

signals:
    void addClientDevice(QString deviceName, int deviceType);
    void removeClientDevice(QString deviceName, int deviceType);

private:
    void deleteSensorData(DeviceData *dev);

    QMutex m_lock;

    QStringList m_deviceTypeStrings;
    QStringList m_subTopics;
    QString m_deviceUnknown;
    QString m_topicUnknown;

    QMap <QString, DeviceData *> m_deviceData;
    QQueue<MQTTSensor *> m_discardQueue;

};

extern DeviceDatabase *g_deviceDatabase;

#endif // _DEVICEDATABASE_H
