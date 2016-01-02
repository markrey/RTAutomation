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

#include "RTInsteonServer.h"
#include "InsteonServerClient.h"
#include "InsteonDriver.h"
#include "SerialDlg.h"
#include "DeviceStatus.h"
#include "TimerStatus.h"
#include "DeviceConfig.h"
#include "AddDevice.h"
#include "TimerConfig.h"
#include "AddTimer.h"
#include "ControlledDevices.h"
#include "EditControlled.h"
#include "TopicConfig.h"
#include "LocationConfig.h"

#include "CommandService.h"

#include <stdio.h>

RTInsteonServer::RTInsteonServer(QObject *parent, bool daemonMode) : MainConsole(parent, daemonMode)
{
    qRegisterMetaType<InsteonDevice>("InsteonDevice");
    qRegisterMetaType<InsteonTimer>("InsteonTimer");
    qRegisterMetaType<InsteonTimerDevice>("InsteonTimerDevice");

    m_client = new InsteonServerClient(this);
    m_insteonDriver = new InsteonDriver();

    connect(m_insteonDriver, SIGNAL(newInsteonUpdate(QJsonObject)), m_client, SLOT(newInsteonUpdate(QJsonObject)));
    connect(m_client, SIGNAL(setDeviceLevel(int, int)), m_insteonDriver, SLOT(setDeviceLevel(int, int)));

    connect(m_client, SIGNAL(receiveCommandData(QJsonObject)),
            m_commandService, SLOT(receiveCommandData(QJsonObject)));
    connect(m_commandService, SIGNAL(sendCommandData(QJsonObject)),
            m_client, SLOT(sendCommandData(QJsonObject)));

    addStandardDialogs();

    TopicConfig *topicConfig = new TopicConfig();
    RTInsteonJSON::addConfigDialog(topicConfig);

    LocationConfig *locationConfig = new LocationConfig();
    RTInsteonJSON::addConfigDialog(locationConfig);

    SerialDlg *serialDlg = new SerialDlg();
    connect(serialDlg, SIGNAL(newPortName(QString)), m_insteonDriver, SLOT(newPortName(QString)));
    RTInsteonJSON::addConfigDialog(serialDlg);

    DeviceStatus *deviceStatus = new DeviceStatus(m_insteonDriver);
    RTInsteonJSON::addInfoDialog(deviceStatus);

    DeviceConfig *deviceConfig = new DeviceConfig(m_insteonDriver);
    connect(deviceConfig, SIGNAL(addDevice(InsteonDevice)), m_insteonDriver, SLOT(addDevice(InsteonDevice)));
    connect(deviceConfig, SIGNAL(removeDevice(int)), m_insteonDriver, SLOT(removeDevice(int)));
    RTInsteonJSON::addToDialogList(deviceConfig);

    TimerConfig *timerConfig = new TimerConfig(m_insteonDriver);
    connect(timerConfig, SIGNAL(updateTimer(InsteonTimer)), m_insteonDriver, SLOT(updateTimer(InsteonTimer)));
    connect(timerConfig, SIGNAL(removeTimer(QString)), m_insteonDriver, SLOT(removeTimer(QString)));
    RTInsteonJSON::addToDialogList(timerConfig);

    AddDevice *addDevice = new AddDevice(m_insteonDriver);
    connect(addDevice, SIGNAL(addDevice(InsteonDevice)), m_insteonDriver, SLOT(addDevice(InsteonDevice)));
    RTInsteonJSON::addToDialogList(addDevice);

    AddTimer *addTimer = new AddTimer(m_insteonDriver);
    connect(addTimer, SIGNAL(addTimer(InsteonTimer)), m_insteonDriver, SLOT(addTimer(InsteonTimer)));
    RTInsteonJSON::addToDialogList(addTimer);

    TimerStatus *timerStatus = new TimerStatus(m_insteonDriver);
    RTInsteonJSON::addInfoDialog(timerStatus);

    ControlledDevices *controlledDevices = new ControlledDevices(m_insteonDriver, timerConfig);
    RTInsteonJSON::addToDialogList(controlledDevices);

    EditControlled *editControlled = new EditControlled(m_insteonDriver, timerConfig);
    connect(editControlled, SIGNAL(updateTimer(InsteonTimer)), m_insteonDriver, SLOT(updateTimer(InsteonTimer)));
    RTInsteonJSON::addToDialogList(editControlled);

    m_client->resumeThread();
    m_insteonDriver->resumeThread();

    startServices();
}

void RTInsteonServer::showHelp()
{
    printf("\nOptions are:\n\n");
    printf("  H - Show help\n");
    printf("  S - show status\n");
    printf("  T - toggle Insteon trace\n");
    printf("  X - Exit\n");
}

void RTInsteonServer::showStatus()
{
    printf("\nInsteon state: %s\n", m_insteonDriver->getState());
    printf("Insteon trace is %s\n", m_insteonDriver->getTrace() ? "on" : "off");
}

void RTInsteonServer::processInput(char c)
{
    switch (c) {
    case 'H':
        showHelp();
        break;

    case 'S':
        showStatus();
        break;

    case 'T':
        m_insteonDriver->setTrace(!m_insteonDriver->getTrace());
        break;
   }
    printf("\n");
}
