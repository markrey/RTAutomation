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

#include "ManagerClient.h"
#include "RTInsteonManager.h"
#include "RTInsteonJSON.h"
#include "RTInsteonLog.h"

#include <qsettings.h>
#include <qdebug.h>

#define TAG "ManagerClient"

ManagerClient::ManagerClient() : InsteonMQTTClient(0, false)
{

}
void ManagerClient::clientInit()
{
    QSettings settings;

    QString serverID = settings.value(RTINSTEON_PARAMS_SERVERID).toString();

    settings.beginGroup(RTINSTEON_PARAMS_TOPICGROUP);
    m_managementCommandTopic = serverID + "/" + settings.value(RTINSTEON_PARAMS_MANAGEMENTCOMMANDTOPIC).toString();
    m_managementResponseTopic = serverID + "/" + settings.value(RTINSTEON_PARAMS_MANAGEMENTRESPONSETOPIC).toString();
    settings.endGroup();

    addSubTopic(m_managementResponseTopic);
}

void ManagerClient::clientStop()
{
}

void ManagerClient::clientTimer(QTimerEvent *)
{

}

void ManagerClient::clientProcessReceivedMessage(const QString& topic, QJsonObject json)
{
    if (topic == m_managementResponseTopic) {
        emit receiveCommandData(json);
    } else {
        RTInsteonLog::logError(TAG, QString("Received unexpected message on topic %1").arg(topic));
    }
}

void ManagerClient::sendCommandData(QJsonObject command)
{
    publish(m_managementCommandTopic, command);
}
