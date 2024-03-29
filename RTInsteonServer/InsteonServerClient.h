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

#ifndef _INSTEONSERVERCLIENT_H
#define _INSTEONSERVERCLIENT_H

#include "RTAutomationMQTTClient.h"

class InsteonServerClient : public RTAutomationMQTTClient
{
    Q_OBJECT

public:
    InsteonServerClient(QObject *parent);

public slots:
    void newInsteonUpdate(QJsonObject updateList);
    void sendCommandData(QJsonObject json);

signals:
    void setDeviceLevel(int, int);
    void receiveCommandData(QJsonObject json);

protected:
    void clientInit();
    void clientStop();
    void clientProcessReceivedMessage(const QString& topic, QJsonObject json);
    void clientTimer(QTimerEvent *);

private:
    QString m_statusTopic;
    QString m_controlTopic;
    QString m_managementCommandTopic;
    QString m_managementResponseTopic;
};

#endif // _INSTEONSERVERCLIENT_H
