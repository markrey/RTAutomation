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

#ifndef WEBCLIENT_H
#define WEBCLIENT_H

#include "RTAutomationMQTTClient.h"
#include <qmutex.h>

#define	WEBCLIENT_BACKGROUND_INTERVAL (1000/100)

#define	WEBCLIENT_SENSOR_UPDATE_INTERVAL (1 * 1000)

#define WEB_THUMBNAIL_WIDTH     320						// scale images to this width to create thumbnail

class MQTTSensor;

class WebClient : public RTAutomationMQTTClient
{
    Q_OBJECT

public:
    WebClient(QObject *parent);

public slots:
    void addClientDevice(QString deviceName, int deviceType);
    void removeClientDevice(QString deviceName, int deviceType);
    void sendCommandData(QJsonObject json);

signals:
    void newJpegData(QString deviceName, QByteArray jpeg);
    void newAudioData(QString deviceName, QByteArray audioData, int rate, int channels, QString sampleType);
    void updateSensorData();
    void newThumbnail(QString, const QByteArray&, int, int);
    void receiveData(int slot, int bytes);
    void receiveCommandData(QJsonObject json);

protected:
    void clientInit();
    void clientStop();
    void clientProcessReceivedMessage(const QString& topic, QJsonObject json);
    void clientTimer(QTimerEvent *);

private:
    void sendThumbnail(const QString& deviceName, QByteArray jpeg);
    void loadSensors(QString group, QString src);

    QString m_managementCommandTopic;
    QString m_managementResponseTopic;

    QStringList m_topics;

    int m_updateTimer;

    QMutex m_lock;
};

extern WebClient *g_client;

#endif // WEBCLIENT_H
