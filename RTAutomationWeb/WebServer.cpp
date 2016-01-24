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

#include "WebServer.h"
#include "WebClient.h"
#include "HomePageWidget.h"
#include "SensorRealTime.h"
#include "SensorHistory.h"
#include "WebWidget.h"
#include "DeviceDatabase.h"

#include <Wt/Auth/AuthWidget>
#include <qsettings.h>

#include <qdebug.h>

WebServer::WebServer(WApplication *app, WContainerWidget *parent) :
    WContainerWidget(parent), m_app(app)
{
    m_auth.login().changed().connect(this, &WebServer::onAuthEvent);

    Auth::AuthModel *authModel = new Auth::AuthModel(WebAuth::auth(),
                             m_auth.users(), this);
    authModel->addPasswordAuth(&WebAuth::passwordAuth());

    Auth::AuthWidget *authWidget = new Auth::AuthWidget(m_auth.login());
    authWidget->setModel(authModel);
    authWidget->setRegistrationEnabled(false);

    WVBoxLayout *mainLayout = new WVBoxLayout();
    this->setLayout(mainLayout);

    WHBoxLayout *tbox = new WHBoxLayout();

    m_title = new WText("<b>RTMQTTWeb</b>");
    tbox->addWidget(m_title);

    tbox->addWidget(authWidget, 0, Wt::AlignRight);
    mainLayout->addLayout(tbox);

    m_stack = new WStackedWidget();
    mainLayout->addWidget(m_stack);

    m_stack->insertWidget(WIDGET_STACK_MAIN, new HomePageWidget(this));
    m_stack->insertWidget(WIDGET_STACK_SINGLESENSOR, new SensorRealTime(parent, this));
    m_stack->insertWidget(WIDGET_STACK_HISTORYSENSOR, new SensorHistory(parent, this));
    m_stack->setCurrentIndex(WIDGET_STACK_MAIN);

    connect(g_client, SIGNAL(newJpegData(QString, QByteArray)), this,
                            SLOT(newJpegData(QString , QByteArray)), Qt::DirectConnection);
    connect(g_client, SIGNAL(newAudioData(QString, QByteArray, int, int, QString)), this,
                            SLOT(newAudioData(QString , QByteArray, int, int, QString)), Qt::DirectConnection);
    connect(g_client, SIGNAL(updateSensorData()), this, SLOT(updateSensorData()), Qt::DirectConnection);
    connect(g_client, SIGNAL(newThumbnail(QString,QByteArray,int,int)), this, SLOT(newThumbnail(QString,QByteArray,int,int)), Qt::DirectConnection);

    app->internalPathChanged().connect(this, &WebServer::handleInternalPathChange);
    m_lastSingleDeviceName = "";

    m_stack->hide();\
    m_timerID = startTimer(SENSOR_DATA_UPDATE_INTERVAL);

    authWidget->processEnvironment();
}

WebServer::~WebServer()
{
    QMutexLocker lock(&m_lock);

    killTimer(m_timerID);
    disconnect(g_client, SIGNAL(newJpegData(QString, QByteArray)), this, SLOT(newJpegData(QString, QByteArray)));
    disconnect(g_client, SIGNAL(newAudioData(QString, QByteArray, int, int, QString)), this,
                            SLOT(newAudioData(QString , QByteArray, int, int, QString)));
    disconnect(g_client, SIGNAL(updateSensorData()), this, SLOT(updateSensorData()));
    disconnect(g_client, SIGNAL(newThumbnail(QString,QByteArray,int,int)), this, SLOT(newThumbnail(QString,QByteArray,int,int)));
    ((WebWidget *)m_stack->currentWidget())->makeInactive();
}

void WebServer::onAuthEvent()
{
  if (m_auth.login().loggedIn()) {
      m_stack->show();
      ((HomePageWidget *)m_stack->currentWidget())->makeActive();
      m_app->triggerUpdate();
  } else {
      m_stack->hide();
  }
}

void WebServer::handleInternalPathChange(std::string path)
{
    if (path == INTERNAL_PATH_MAIN) {
        setMainPage();
        return;
    }
    if (path == INTERNAL_PATH_SINGLE_DEVICE) {
        setSingleDevice(m_lastSingleDeviceName);
        return;
    }
    if (path == INTERNAL_PATH_HISTORY_DEVICE) {
        setHistoryDevice(m_lastSingleDeviceName);
        return;
    }
    setMainPage();
}


void WebServer::newJpegData(QString deviceName, QByteArray jpeg)
{
    QMutexLocker lock(&m_lock);

    bool needUpdate = false;

    Wt::WApplication::UpdateLock wlock(m_app);
    if (wlock) {

        switch (m_stack->currentIndex()) {
            case WIDGET_STACK_SINGLESENSOR:
                needUpdate = ((SensorRealTime *)m_stack->currentWidget())->processJpegData(deviceName, jpeg);
                break;
        }
        if (needUpdate) {
            m_app->triggerUpdate();
        }
    }
}

void WebServer::newAudioData(QString /* deviceName */, QByteArray /* audioData */,
                             int /* rate */, int /* channels */, QString /* sampleType */)
{
    QMutexLocker lock(&m_lock);

 /*   bool needUpdate = false;

    Wt::WApplication::UpdateLock wlock(m_app);
    if (wlock) {

        switch (m_stack->currentIndex()) {
            case WIDGET_STACK_SINGLESENSOR:
                needUpdate = ((SensorRealTime *)m_stack->currentWidget())->processJpegData(deviceName, jpeg);
                break;
        }
        if (needUpdate) {
            m_app->triggerUpdate();
        }
    }*/
}


void WebServer::updateSensorData()
{
    QMutexLocker lock(&m_lock);

    Wt::WApplication::UpdateLock wlock(m_app);
    if (wlock) {

        switch (m_stack->currentIndex()) {
        case WIDGET_STACK_SINGLESENSOR:
            ((SensorRealTime *)m_stack->currentWidget())->updateSensorData();
            break;
        case WIDGET_STACK_MAIN:
            ((HomePageWidget *)m_stack->currentWidget())->updateSensorData();
            break;
        }
        m_app->triggerUpdate();
    }
}

void WebServer::newThumbnail(QString deviceName, QByteArray jpeg, int width, int height)
{
    bool needUpdate = false;

    // now update appropriate widgets

    Wt::WApplication::UpdateLock wlock(m_app);
    if (wlock) {

        switch (m_stack->currentIndex()) {
            case WIDGET_STACK_MAIN:
                needUpdate = ((HomePageWidget *)m_stack->currentWidget())->newFrame(deviceName, jpeg, width, height);
                break;
        }
        if (needUpdate)
            m_app->triggerUpdate();

    }

}

void WebServer::setMainPage()
{
    // only called in response to a user action so no lock

    ((WebWidget *)m_stack->currentWidget())->makeInactive();
    m_stack->setCurrentIndex(WIDGET_STACK_MAIN);
    ((HomePageWidget *)m_stack->currentWidget())->makeActive();
    m_title->setText("<h3>RTMQTTWeb</h3>");
    m_app->triggerUpdate();
}

void WebServer::setSingleDevice(const QString& deviceName)
{
    // only called in response to a user action so no lock

    m_lastSingleDeviceName = deviceName;
    ((WebWidget *)m_stack->currentWidget())->makeInactive();
    m_stack->setCurrentIndex(WIDGET_STACK_SINGLESENSOR);
    ((SensorRealTime *)m_stack->currentWidget())->setDevice(deviceName);
    ((SensorRealTime *)m_stack->currentWidget())->makeActive();
    m_title->setText(WString("<h3>") + WString("Device: ") + WString(qPrintable(deviceName)) + WString("</h3>"));
    m_app->triggerUpdate();
    WApplication::instance()->setInternalPath(INTERNAL_PATH_SINGLE_DEVICE,  true);
}

void WebServer::setHistoryDevice(const QString& deviceName)
{
    // only called in response to a user action so no lock

    m_lastSingleDeviceName = deviceName;
    ((WebWidget *)m_stack->currentWidget())->makeInactive();
    m_stack->setCurrentIndex(WIDGET_STACK_HISTORYSENSOR);
    ((SensorHistory *)m_stack->currentWidget())->setDevice(deviceName);
    ((SensorHistory *)m_stack->currentWidget())->makeActive();
    m_title->setText(WString("<h3>") + WString("Device history: ") + WString(qPrintable(deviceName)) + WString("</h3>"));
    m_app->triggerUpdate();
    WApplication::instance()->setInternalPath(INTERNAL_PATH_HISTORY_DEVICE,  true);
}

