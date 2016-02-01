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

#include "WebClient.h"
#include "MQTTSensor.h"
#include "DeviceDatabase.h"
#include "RTAutomationJSON.h"
#include "RTAutomationLog.h"
#include "RTMQTTJSON.h"

#include <qimage.h>
#include <qbuffer.h>
#include <qdebug.h>
#include <qsettings.h>

#define TAG "WebClient"

WebClient *g_client;

WebClient::WebClient(QObject *) : RTAutomationMQTTClient(0)
{
    g_client = this;
    m_updateTimer = -1;
}

void WebClient::addClientDevice(QString deviceName, int deviceType)
{
    addSubTopic(deviceName + "/" + g_deviceDatabase->getSubTopic(deviceType));
}

void WebClient::removeClientDevice(QString deviceName, int deviceType)
{
    removeSubTopic(deviceName + "/" + g_deviceDatabase->displayType(deviceType));
}

void WebClient::clientInit()
{
    QSettings settings;

    QString deviceID = settings.value(RTAUTOMATION_PARAMS_DEVICEID).toString();

    settings.beginGroup(RTAUTOMATION_PARAMS_TOPICGROUP);
    m_managementCommandTopic = deviceID + "/" + settings.value(RTAUTOMATION_PARAMS_MANAGEMENTCOMMANDTOPIC).toString();
    m_managementResponseTopic = deviceID + "/" + settings.value(RTAUTOMATION_PARAMS_MANAGEMENTRESPONSETOPIC).toString();
    settings.endGroup();

    lockedAddSubTopic(m_managementCommandTopic);

    m_updateTimer = startTimer(WEBCLIENT_SENSOR_UPDATE_INTERVAL);
}

void WebClient::clientStop()
{
    if (m_updateTimer != -1)
        killTimer(m_updateTimer);
    m_updateTimer = -1;
}

void WebClient::clientTimer(QTimerEvent *)
{
    if (!m_connected)
        return;
    emit updateSensorData();
}

void WebClient::clientProcessReceivedMessage(const QString& topic, QJsonObject json)
{
    if (topic == m_managementCommandTopic) {
        emit receiveCommandData(json);
        return;
    }

    QStringList parts = topic.split("/");

    int deviceType = g_deviceDatabase->getTypeFromSubTopic(parts.last());

    if (deviceType == -1) {
        RTAutomationLog::logError(TAG, QString("Message from unsupported topic type ") + topic);
        return;
    }

    parts.removeLast();;
    QString deviceName = parts.join('/');

    QByteArray mediaData;

    switch (deviceType) {
    case DEVICE_TYPE_INSTEON_SERVER:
        // TODO
        break;

    case DEVICE_TYPE_RTMQTT_VIDEO:
        if (!json.contains(RTMQTTJSON_VIDEO_DATA)) {
            RTAutomationLog::logError(TAG, QString("No video data in message from ") + topic);
            return;
        }
        mediaData = QByteArray::fromBase64(json[RTMQTTJSON_VIDEO_DATA].toString().toLatin1());
        emit newJpegData(deviceName, mediaData);
        if (g_deviceDatabase->timeForThumbnail(deviceName))
            sendThumbnail(deviceName, mediaData);
        break;

    case DEVICE_TYPE_RTMQTT_AUDIO:
        if (!json.contains(RTMQTTJSON_AUDIO_DATA)) {
            RTAutomationLog::logError(TAG, QString("No audio data in message from ") + topic);
            return;
        }
        mediaData = QByteArray::fromHex(json[RTMQTTJSON_AUDIO_DATA].toString().toLatin1());
        emit newAudioData(deviceName, mediaData, json[RTMQTTJSON_AUDIO_RATE].toInt(),
                          json[RTMQTTJSON_AUDIO_CHANNELS].toInt(), json[RTMQTTJSON_AUDIO_SAMPTYPE].toString());
        break;

    case DEVICE_TYPE_RTMQTT_SENSOR:
        g_deviceDatabase->newSensorData(deviceName, json);
        break;
    }
}


void WebClient::sendThumbnail(const QString& deviceName, QByteArray jpeg)
{
    QImage image = QImage::fromData(jpeg, "JPG");
    QImage smallImage = image.scaledToWidth(WEB_THUMBNAIL_WIDTH);
    QByteArray smallFrame;
    QBuffer buffer(&smallFrame);
    buffer.open(QIODevice::WriteOnly);
    smallImage.save(&buffer, "JPG");
    emit newThumbnail(deviceName, smallFrame, smallImage.width(), smallImage.height());
}

void WebClient::sendCommandData(QJsonObject json)
{
    publish(m_managementResponseTopic, json);
}


