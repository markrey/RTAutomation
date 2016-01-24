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

#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_

#include <Wt/WContainerWidget>
#include <Wt/WImage>
#include <Wt/WApplication>
#include <Wt/WStackedWidget>
#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WSlider>
#include <Wt/WText>

#include "WebAuth.h"

// This is the only part of the web session that joins the Qt world with the Wt world

#include <QObject>
#include <qmutex.h>

using namespace Wt;

#define WEBSERVER_MAX_WIDGETS 3

//  Widget stack indices

#define WIDGET_STACK_MAIN      0
#define WIDGET_STACK_SINGLESENSOR 1
#define WIDGET_STACK_HISTORYSENSOR 2

//  Internal path string defs

#define INTERNAL_PATH_MAIN  "/main"
#define INTERNAL_PATH_SINGLE_DEVICE "/SingleDevice"
#define INTERNAL_PATH_HISTORY_DEVICE "/HistoryDevice"

//  Sensor data update interval

#define SENSOR_DATA_UPDATE_INTERVAL 1000

class WebServer : public QObject, public WContainerWidget
{
    Q_OBJECT

public:
    WebServer(WApplication *app, WContainerWidget *parent);
    virtual ~WebServer();

    // This function activates the single device page for the specified device

    void setSingleDevice(const QString& deviceName);

    // This function activates the history device page for the specified device

    void setHistoryDevice(const QString& deviceName);

    // This function sets the stack to the main page

    void setMainPage();

public slots:
    void newJpegData(QString deviceName, QByteArray jpeg);
    void newAudioData(QString deviceName, QByteArray audioData, int rate, int channels, QString sampleType);
    void updateSensorData();
    void newThumbnail(QString deviceName, QByteArray jpeg, int width, int height);

protected:
    WStackedWidget *m_stack;
    WApplication *m_app;
    void handleInternalPathChange(std::string path);

    QString m_lastSingleDeviceName;

    WebAuth m_auth;

    void onAuthEvent();

    QMutex m_lock;

    int m_timerID;

private:
    WText *m_title;

};

#endif // _WEBSERVER_H_
