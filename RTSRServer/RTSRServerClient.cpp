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

#include <qbytearray.h>
#include <qjsondocument.h>
#include <qbuffer.h>
#include <qjsonarray.h>
#include <qsettings.h>
#include <qdebug.h>
#include <qdatetime.h>

#include "RTSRServerClient.h"
#include "RTAutomationLog.h"
#include "RTSRServer.h"
#include "RTMQTTJSON.h"

#define TAG "RTSRServerClient"

RTSRServerClient::RTSRServerClient(QObject *parent) : RTAutomationMQTTClient(parent)
{

}
void RTSRServerClient::clientInit()
{
    QSettings settings;

    m_deviceID = settings.value(RTAUTOMATION_PARAMS_DEVICEID).toString();

    settings.beginGroup(RTAUTOMATION_PARAMS_TOPICGROUP);
    m_managementCommandTopic = m_deviceID + "/" + settings.value(RTAUTOMATION_PARAMS_MANAGEMENTCOMMANDTOPIC).toString();
    m_managementResponseTopic = m_deviceID + "/" + settings.value(RTAUTOMATION_PARAMS_MANAGEMENTRESPONSETOPIC).toString();
    settings.endGroup();

    settings.beginGroup(RTSRSERVER_PARAMS_TOPIC_GROUP);
    m_audioTopic = settings.value(RTSRSERVER_PARAMS_AUDIO_TOPIC).toString();
    m_decodedSpeechTopic = m_deviceID + "/" + settings.value(RTSRSERVER_PARAMS_DECODEDSPEECH_TOPIC).toString();
    m_ttsCompleteTopic = settings.value(RTSRSERVER_PARAMS_TTSCOMPLETE_TOPIC).toString();
    settings.endGroup();

    lockedAddSubTopic(m_managementCommandTopic);
    lockedAddSubTopic(m_audioTopic);
    lockedAddSubTopic(m_ttsCompleteTopic);
}

void RTSRServerClient::clientStop()
{
}

void RTSRServerClient::clientTimer(QTimerEvent *)
{

}

void RTSRServerClient::clientProcessReceivedMessage(const QString& topic, QJsonObject json)
{
    if (topic == m_audioTopic)
        emit newAudio(topic, json);
    else if (topic == m_ttsCompleteTopic) {
        emit ttsComplete(topic, json);
    } else if (topic == m_managementCommandTopic) {
        emit receiveCommandData(json);
    }
}

void RTSRServerClient::sendCommandData(QJsonObject json)
{
    publish(m_managementResponseTopic, json);
}

void RTSRServerClient::decodedSpeech(QJsonObject decodedSpeech)
{
    decodedSpeech[RTMQTTJSON_TIMESTAMP] = (double)QDateTime::currentMSecsSinceEpoch() / 1000.0;
    decodedSpeech[RTMQTTJSON_TOPIC] = m_decodedSpeechTopic;
    decodedSpeech[RTMQTTJSON_DEVICEID] = m_deviceID;
    publish(m_decodedSpeechTopic, decodedSpeech);
}
