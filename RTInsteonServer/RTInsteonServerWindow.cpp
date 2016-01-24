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

#include <qboxlayout.h>

#include "RTInsteonServerWindow.h"
#include "RTInsteonServer.h"
#include "InsteonServerClient.h"
#include "InsteonDriver.h"
#include "InsteonDevice.h"

#include "DeviceConfig.h"
#include "SetInsteonLevelDlg.h"
#include "TimerConfig.h"
#include "SerialDlg.h"
#include "DeviceStatus.h"
#include "TimerStatus.h"
#include "AddDevice.h"
#include "AddTimer.h"
#include "ControlledDevices.h"
#include "EditControlled.h"
#include "TopicConfig.h"
#include "LocationConfig.h"

#include "CommandService.h"

RTInsteonServerWindow::RTInsteonServerWindow() : MainDialog()
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

    m_client->resumeThread();
    m_insteonDriver->resumeThread();

    addStandardDialogs();

    TopicConfig *topicConfig = new TopicConfig();
    RTAutomationJSON::addConfigDialog(topicConfig);

    LocationConfig *locationConfig = new LocationConfig();
    RTAutomationJSON::addConfigDialog(locationConfig);

    SerialDlg *serialDlg = new SerialDlg();
    connect(serialDlg, SIGNAL(newPortName(QString)), m_insteonDriver, SLOT(newPortName(QString)));
    RTAutomationJSON::addConfigDialog(serialDlg);

    DeviceConfig *deviceConfig = new DeviceConfig(m_insteonDriver);
    connect(deviceConfig, SIGNAL(addDevice(InsteonDevice)), m_insteonDriver, SLOT(addDevice(InsteonDevice)));
    connect(deviceConfig, SIGNAL(removeDevice(int)), m_insteonDriver, SLOT(removeDevice(int)));
    RTAutomationJSON::addToDialogList(deviceConfig);

    TimerConfig *timerConfig = new TimerConfig(m_insteonDriver);
    connect(timerConfig, SIGNAL(updateTimer(InsteonTimer)), m_insteonDriver, SLOT(updateTimer(InsteonTimer)));
    connect(timerConfig, SIGNAL(removeTimer(QString)), m_insteonDriver, SLOT(removeTimer(QString)));
    RTAutomationJSON::addToDialogList(timerConfig);

    AddDevice *addDevice = new AddDevice(m_insteonDriver);
    connect(addDevice, SIGNAL(addDevice(InsteonDevice)), m_insteonDriver, SLOT(addDevice(InsteonDevice)));
    RTAutomationJSON::addToDialogList(addDevice);

    AddTimer *addTimer = new AddTimer(m_insteonDriver);
    connect(addTimer, SIGNAL(addTimer(InsteonTimer)), m_insteonDriver, SLOT(addTimer(InsteonTimer)));
    RTAutomationJSON::addToDialogList(addTimer);

    DeviceStatus *deviceStatus = new DeviceStatus(m_insteonDriver);
    RTAutomationJSON::addInfoDialog(deviceStatus);

    TimerStatus *timerStatus = new TimerStatus(m_insteonDriver);
    RTAutomationJSON::addInfoDialog(timerStatus);

    ControlledDevices *controlledDevices = new ControlledDevices(m_insteonDriver, timerConfig);
    RTAutomationJSON::addToDialogList(controlledDevices);

    EditControlled *editControlled = new EditControlled(m_insteonDriver, timerConfig);
    connect(editControlled, SIGNAL(updateTimer(InsteonTimer)), m_insteonDriver, SLOT(updateTimer(InsteonTimer)));
    RTAutomationJSON::addToDialogList(editControlled);

    startServices();
}
