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

#include "ControllerClient.h"
#include "RTInsteonController.h"
#include "RTAutomationJSON.h"
#include "RTAutomationLog.h"

#define TAG "ControllerClient"

ControllerClient::ControllerClient() : RTAutomationMQTTClient(0)
{

}
void ControllerClient::clientInit()
{
    QSettings settings;

    QString serverID = settings.value(RTAUTOMATION_PARAMS_SERVERID).toString();

    settings.beginGroup(RTAUTOMATION_PARAMS_TOPICGROUP);
    m_statusTopic = serverID + "/" + settings.value(RTAUTOMATION_PARAMS_STATUSTOPIC).toString();
    m_controlTopic = serverID + "/" + settings.value(RTAUTOMATION_PARAMS_CONTROLTOPIC).toString();
    settings.endGroup();

    lockedAddSubTopic(m_statusTopic);
}

void ControllerClient::clientStop()
{
}

void ControllerClient::clientTimer(QTimerEvent *)
{

}

void ControllerClient::clientProcessReceivedMessage(const QString& topic, QJsonObject json)
{
    if (topic == m_statusTopic) {
        emit newUpdate(json);
    } else {
        RTAutomationLog::logError(TAG, QString("Received unexpected message on topic %1").arg(topic));
    }
}

void ControllerClient::setDeviceLevel(QJsonObject newDeviceLevel)
{
    publish(m_controlTopic, newDeviceLevel);
}
