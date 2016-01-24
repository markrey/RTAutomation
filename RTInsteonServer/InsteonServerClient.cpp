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

#include "InsteonServerClient.h"
#include "InsteonDevice.h"
#include "RTAutomationLog.h"
#include "RTInsteonServer.h"

#define TAG "InsteonServerClient"

InsteonServerClient::InsteonServerClient(QObject *parent) : RTAutomationMQTTClient(parent)
{
}

void InsteonServerClient::clientInit()
{
    QSettings settings;

    QString deviceID = settings.value(RTAUTOMATION_PARAMS_DEVICEID).toString();

    settings.beginGroup(RTAUTOMATION_PARAMS_TOPICGROUP);
    m_statusTopic = deviceID + "/" + settings.value(RTAUTOMATION_PARAMS_STATUSTOPIC).toString();
    m_controlTopic = deviceID + "/" + settings.value(RTAUTOMATION_PARAMS_CONTROLTOPIC).toString();
    m_managementCommandTopic = deviceID + "/" + settings.value(RTAUTOMATION_PARAMS_MANAGEMENTCOMMANDTOPIC).toString();
    m_managementResponseTopic = deviceID + "/" + settings.value(RTAUTOMATION_PARAMS_MANAGEMENTRESPONSETOPIC).toString();
    settings.endGroup();

    addSubTopic(m_controlTopic);
    addSubTopic(m_managementCommandTopic);
}

void InsteonServerClient::clientStop()
{
}

void InsteonServerClient::clientTimer(QTimerEvent *)
{

}

void InsteonServerClient::clientProcessReceivedMessage(const QString& topic, QJsonObject json)
{
     if (topic == m_controlTopic) {
        if (!json.contains(INSTEON_DEVICE_JSON_SETDEVICELEVEL))
            return;

        QJsonArray jsa = json[INSTEON_DEVICE_JSON_SETDEVICELEVEL].toArray();
        for (int i = 0; i < jsa.count(); i++) {
            QJsonObject params = jsa[i].toObject();

            InsteonDevice idev;
            if (!idev.readNewLevel(params))
                continue;
            emit setDeviceLevel(idev.deviceID, idev.newLevel);

        }
    } else if (topic == m_managementCommandTopic) {
        emit receiveCommandData(json);
    } else {
        RTAutomationLog::logError(TAG, QString("Received unexpected message on topic %1").arg(topic));
    }

}

void InsteonServerClient::sendCommandData(QJsonObject json)
{
    publish(m_managementResponseTopic, json);
}


void InsteonServerClient::newInsteonUpdate(QJsonObject updateList)
{
    publish(m_statusTopic, updateList);
}
