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

#include "RTAutomationWebWindow.h"
#include "WebClient.h"
#include "RootWebServer.h"
#include "RC.h"
#include "DeviceDatabase.h"
#include "DeviceStatus.h"
#include "DeviceConfig.h"
#include "AddDevice.h"
#include "KafkaDatabase.h"
#include "KafkaConfigure.h"

#include "CommandService.h"

RTAutomationWebWindow::RTAutomationWebWindow() : MainDialog()
{
    RC::init();
    m_client = new WebClient(this);
    m_deviceDatabase = new DeviceDatabase();
    m_kafkaDatabase = new KafkaDatabase();
    m_webServer = new RootWebServer();

    connect(m_client, SIGNAL(newThumbnail(QString, const QByteArray&,int,int)),
            m_deviceDatabase, SLOT(newThumbnail(QString, const QByteArray&,int,int)), Qt::DirectConnection);
    connect(m_client, SIGNAL(receiveCommandData(QJsonObject)),
            m_commandService, SLOT(receiveCommandData(QJsonObject)));
    connect(m_commandService, SIGNAL(sendCommandData(QJsonObject)),
            m_client, SLOT(sendCommandData(QJsonObject)));
    connect(m_deviceDatabase, SIGNAL(addClientDevice(QString,int)), m_client, SLOT(addClientDevice(QString,int)));
    connect(m_deviceDatabase, SIGNAL(removeClientDevice(QString,int)), m_client, SLOT(removeClientDevice(QString,int)));

    m_client->resumeThread();
    m_webServer->resumeThread();

    m_deviceDatabase->loadDatabase();

    addStandardDialogs();

    DeviceStatus *deviceStatus = new DeviceStatus();
    RTAutomationJSON::addInfoDialog(deviceStatus);

    KafkaConfigure *kafka = new KafkaConfigure();
    RTAutomationJSON::addConfigDialog(kafka);

    DeviceConfig *deviceConfig = new DeviceConfig();
    RTAutomationJSON::addToDialogList(deviceConfig);

    AddDevice *addDevice = new AddDevice();
    RTAutomationJSON::addToDialogList(addDevice);

    startServices();

}

void RTAutomationWebWindow::appExit()
{
    m_webServer->exitThread();
    m_client->exitThread();
    m_client->thread()->wait(2000);
    delete m_deviceDatabase;
    m_deviceDatabase = NULL;
}
