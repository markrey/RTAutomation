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

#ifndef _RTSRSERVERCLIENT_H
#define _RTSRSERVERCLIENT_H

#include "RTAutomationMQTTClient.h"

class RTSRServerClient : public RTAutomationMQTTClient
{
    Q_OBJECT

public:
    RTSRServerClient(QObject *parent);

public slots:
    void sendCommandData(QJsonObject json);
    void decodedSpeech(QJsonObject decodedSpeech);

signals:
    void receiveCommandData(QJsonObject json);
    void newAudio(QString topic, QJsonObject json);
    void ttsComplete(QString topic, QJsonObject json);

protected:
    void clientInit();
    void clientStop();
    void clientProcessReceivedMessage(const QString& topic, QJsonObject json);
    void clientTimer(QTimerEvent *);

private:
    QString m_audioTopic;
    QString m_decodedSpeechTopic;
    QString m_ttsCompleteTopic;
    QString m_managementCommandTopic;
    QString m_managementResponseTopic;
    QString m_deviceID;
};

#endif // _RTSRSERVERCLIENT_H

