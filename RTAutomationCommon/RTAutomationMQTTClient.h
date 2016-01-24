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

#ifndef _RTAUTOMATIONMQTTCLIENT_H
#define _RTAUTOMATIONMQTTCLIENT_H

#include <qmutex.h>
#include <qjsonobject.h>
#include <QTimerEvent>
#include <qstringlist.h>

#include "RTAutomationThread.h"

#include "MQTTAsync.h"

class RTAutomationMQTTClient : public RTAutomationThread
{
    Q_OBJECT

public:
    RTAutomationMQTTClient(QObject *parent, bool enabled = true);

    QString getLinkState();

    void setConnected(bool state);
    void processMessage(const char *topic, const unsigned char *message, int messageLength);
    void onConnect();
    void onConnectFailure(MQTTAsync_failureData* response);
    void onConnectionLost();
    void onDisconnect();

public slots:
    void clientRestart();
    void clientEnable(bool state);

signals:
    void clientConnected();
    void clientDisconnected();

protected:
    void initModule();
    void stopModule();
    void timerEvent(QTimerEvent *);

    void dumpClient();

    //  call this to add a topic for subscription when next connected

    void addSubTopic(const QString& topic);

    //  same but can call whem locked

    void lockedAddSubTopic(const QString& topic);

    //  call this to remove a subscription for a topic

    void removeSubTopic(const QString& topic);

    //  same but can call when lockedc

    void lockedRemoveSubTopic(const QString& topic);

    //  call this to publish a message

    void publish(const QString& topic, const QJsonObject json);

    MQTTAsync m_client;

    QMutex m_lock;

    volatile bool m_connected;
    volatile bool m_connectInProgress;
    volatile bool m_disconnected;
    volatile bool m_disconnectInProgress;

    //  Client overrides

    virtual void clientInit() = 0;
    virtual void clientStop() = 0;
    virtual void clientProcessReceivedMessage(const QString& /* topic */, QJsonObject /* json */) {}
    virtual void clientTimer(QTimerEvent *) {}

private:
    int m_timerId;
    bool m_enabled;
    QStringList m_subTopics;

    qint64 m_lastConnectTime;
};

#endif // _RTAUTOMATIONMQTTCLIENT_H
