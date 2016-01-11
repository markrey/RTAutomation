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
#include <qdatetime.h>
#include <qdebug.h>

#include "InsteonMQTTClient.h"
#include "RTInsteonLog.h"
#include "RTInsteonJSON.h"

#define TAG "InsteonMQTTClient"

#define CLIENT_MIN_CONNECT_INTERVAL ((qint64)5000)

void MQTTClientOnConnect(void* context, MQTTAsync_successData* /* response */)
{
    ((InsteonMQTTClient *)context)->onConnect();
}

void MQTTClientOnConnectFailure(void* context, MQTTAsync_failureData* response)
{
    ((InsteonMQTTClient *)context)->onConnectFailure(response);
}


int MQTTClientMessageArrived(void *context, char *topicName, int /* topicLen */, MQTTAsync_message *message)
{
    ((InsteonMQTTClient *)context)->processMessage((const char *)topicName,
                          (const unsigned char *)message->payload, message->payloadlen);

    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);
    return 1;
}

void MQTTClientConnectionLost(void *context, char *cause)
{
    ((InsteonMQTTClient *)context)->setConnected(false);
    RTInsteonLog::logInfo(TAG, QString("MQTT connection lost %1").arg(cause));
    ((InsteonMQTTClient *)context)->onConnectionLost();
}

void MQTTClientOnDisconnectSuccess(void* context, MQTTAsync_successData* /* response */)
{
    ((InsteonMQTTClient *)context)->onDisconnect();
}

void MQTTClientOnDisconnectFailure(void* context, MQTTAsync_failureData* /* response */)
{
    ((InsteonMQTTClient *)context)->onDisconnect();
}


InsteonMQTTClient::InsteonMQTTClient(QObject *, bool enabled) : RTInsteonThread()
{
    m_enabled = enabled;
}

void InsteonMQTTClient::initModule()
{
    QMutexLocker lock(&m_lock);

    m_connected = false;
    m_connectInProgress = false;
    m_disconnected = false;
    m_disconnectInProgress = false;
    m_lastConnectTime = QDateTime::currentMSecsSinceEpoch() - CLIENT_MIN_CONNECT_INTERVAL;
    if (m_enabled)
        m_timerId = startTimer(100);
    clientInit();
}

void InsteonMQTTClient::stopModule()
{
    QMutexLocker lock(&m_lock);

    if (!m_enabled)
        return;

    killTimer(m_timerId);

    clientStop();

    if (m_connected) {
        dumpClient();

        // wait for disconnect

        qint64 start = QDateTime::currentMSecsSinceEpoch();

        while (!m_disconnected && (QDateTime::currentMSecsSinceEpoch() - start) < 1000)
            m_thread->yieldCurrentThread();
    }
    MQTTAsync_destroy(&m_client);
    m_connected = false;
    m_connectInProgress = false;
    m_subTopics.clear();
    emit clientDisconnected();
}

void InsteonMQTTClient::dumpClient()
{
    MQTTAsync_disconnectOptions dopts = MQTTAsync_disconnectOptions_initializer;

    dopts.context = this;
    dopts.onSuccess = MQTTClientOnDisconnectSuccess;
    dopts.onFailure = MQTTClientOnDisconnectFailure;
    m_connected = false;
    m_connectInProgress = false;
    m_disconnectInProgress = true;
    m_disconnected = false;
    MQTTAsync_disconnect(m_client, &dopts);
}

void InsteonMQTTClient::clientEnable(bool state)
{
    QMutexLocker lock(&m_lock);

    if (m_enabled) {
        killTimer(m_timerId);
        clientStop();
        if (m_connected) {
            dumpClient();
        } else {
            MQTTAsync_destroy(&m_client);
            m_client = NULL;
        }
        m_connected = false;
        m_connectInProgress = false;
        m_subTopics.clear();
        emit clientDisconnected();
    }
    m_enabled = state;

    if (state) {
        m_timerId = startTimer(100);
        clientInit();
    }
}

void InsteonMQTTClient::timerEvent(QTimerEvent *event)
{
    int rc;

    if (event->timerId() != m_timerId) {
        clientTimer(event);
        return;
    }

    QMutexLocker lock(&m_lock);

    qint64 now = QDateTime::currentMSecsSinceEpoch();

    if (m_disconnectInProgress) {
        if (m_disconnected) {
            m_disconnectInProgress = false;
            MQTTAsync_destroy(&m_client);
            m_client = NULL;
        }
        return;
    }

    if (!m_enabled)
        return;

    if (!m_connected && !m_connectInProgress) {

        if ((now - m_lastConnectTime) < CLIENT_MIN_CONNECT_INTERVAL)
            return;

        m_lastConnectTime = now;

        QSettings settings;

        QString brokerAddress = settings.value(RTINSTEON_PARAMS_BROKERADDRESS).toString();
        QString clientID = settings.value(RTINSTEON_PARAMS_CLIENTID).toString();

        RTInsteonLog::logInfo(TAG, QString("Trying connect to broker ") + brokerAddress);

        MQTTAsync_create(&m_client, qPrintable(brokerAddress), qPrintable(clientID),
               MQTTCLIENT_PERSISTENCE_NONE, NULL);

        MQTTAsync_setCallbacks(m_client, this, MQTTClientConnectionLost, MQTTClientMessageArrived,
                                   NULL);

        MQTTAsync_connectOptions options = MQTTAsync_connectOptions_initializer;
        options.keepAliveInterval = 20;
        options.cleansession = 1;
        options.username = NULL;
        options.password = NULL;
        options.onSuccess = MQTTClientOnConnect;
        options.onFailure = MQTTClientOnConnectFailure;
        options.context = this;
        options.connectTimeout = 3;
        if ((rc = MQTTAsync_connect(m_client, &options)) != MQTTASYNC_SUCCESS) {
            RTInsteonLog::logInfo(TAG, QString("Failed to start connect, return code %1").arg(rc));

            MQTTAsync_destroy(&m_client);
            m_client = NULL;
            return;
        }
        m_connectInProgress = true;
    }
}

void InsteonMQTTClient::onConnect()
{
    QMutexLocker lock(&m_lock);

    MQTTAsync_responseOptions ropts = MQTTAsync_responseOptions_initializer;

    ropts.context = this;

    //  connected so do subscriptions

    for (int i = 0; i < m_subTopics.count(); i++)
        MQTTAsync_subscribe(m_client, qPrintable(m_subTopics.at(i)), 0, &ropts);

    RTInsteonLog::logInfo(TAG, "Connected to MQTT broker");
    emit clientConnected();
    m_connected = true;
    m_connectInProgress = false;

}

void InsteonMQTTClient::onConnectFailure(MQTTAsync_failureData *response)
{
    if (response == 0)
        RTInsteonLog::logInfo(TAG, QString("Connection to broker failed, code -1"));
    else
        RTInsteonLog::logInfo(TAG, QString("Connection to broker failed, code %1").arg(response->code));
    m_connectInProgress = false;
}

void InsteonMQTTClient::onConnectionLost()
{
    QMutexLocker lock(&m_lock);

    dumpClient();
}

void InsteonMQTTClient::onDisconnect()
{
    QMutexLocker lock(&m_lock);
    m_disconnected = true;
    m_disconnectInProgress = false;
    RTInsteonLog::logInfo(TAG, "Disconnect complete");
}

void InsteonMQTTClient::clientRestart()
{
    stopModule();
    initModule();
}

void InsteonMQTTClient::setConnected(bool state)
{
    QMutexLocker lock(&m_lock);
    m_connected = state;
    if (!state)
        emit clientDisconnected();
}

QString InsteonMQTTClient::getLinkState()
{
    QMutexLocker lock(&m_lock);

    if (m_connected)
        return "Connected";
    else
        return "Disconnected";
}

void InsteonMQTTClient::processMessage(const char *topic, const unsigned char *message, int messageLength)
{
    RTInsteonLog::logInfo(TAG, QString("Message from topic %1").arg(topic));

    QByteArray data((char *)(message), messageLength);
    QJsonDocument var(QJsonDocument::fromJson(data));
    QJsonObject json = var.object();
    clientProcessReceivedMessage(QString(topic), json);
}

void InsteonMQTTClient::publish(const QString &topic, const QJsonObject json)
{
    QMutexLocker lock(&m_lock);
    if (!m_connected)
        return;

    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    buffer.write(QJsonDocument(json).toJson(QJsonDocument::Compact));
    MQTTAsync_send(m_client, qPrintable(topic), data.length(), data.data(), 0, 0, 0);
}
