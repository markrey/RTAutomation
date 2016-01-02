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

#include "ControlledDevices.h"
#include "InsteonTimer.h"
#include "InsteonDriver.h"
#include "TimerConfig.h"

#include "RTInsteonServer.h"
#include "RTInsteonLog.h"

#define TAG "ControlledDevices"

ControlledDevices::ControlledDevices(InsteonDriver *driver, TimerConfig *timerConfig)
            : Dialog(RTINSTEONSERVER_CONTROLLEDDEVICES_NAME, RTINSTEONSERVER_CONTROLLEDDEVICES_DESC)
{
    m_insteonDriver = driver;
    m_timerConfig = timerConfig;
}

void ControlledDevices::getDialog(QJsonObject& newDialog)
{
    QStringList headers;
    QList<int> widths;
    QStringList data;
    int deviceIndex;

    m_index = m_timerConfig->getCurrentIndex();

    QList<InsteonDevice> deviceList = m_insteonDriver->getDeviceList();
    QList<InsteonTimer> timerList = m_insteonDriver->getTimerList();

    if (m_index >= timerList.count()) {
        RTInsteonLog::logError(TAG, QString("Timer index %1 bigger than table").arg(m_index));
        return;
    }

    InsteonTimer timer = timerList.at(m_index);

    m_timerName = timer.name;

    headers << "" << "In use" << "ID" << "Name" << "Level";

    widths << 80 << 60 << 80 << 120 << 60;

    for (int i = 0; i < deviceList.count(); i++) {
        InsteonDevice device = deviceList.at(i);
        data.append("Configure");

        //  see if device is in use at the moment

        for (deviceIndex = 0; deviceIndex < timer.devices.count(); deviceIndex++) {
            if (timer.devices.at(deviceIndex).deviceID == device.deviceID)
                break;
        }

        if (deviceIndex == timer.devices.count()) {
            data.append("no");
            data.append(InsteonDevice::deviceIDToString(device.deviceID));
            data.append(device.name);
            data.append("");
        } else {
            data.append("yes");
            data.append(InsteonDevice::deviceIDToString(device.deviceID));
            data.append(device.name);
            data.append(QString::number(timer.devices.at(deviceIndex).level));
        }

    }

    clearDialog();
    addVar(createInfoStringVar("Timer", m_timerName));
    addVar(createInfoTableVar("Controlled devices", headers, widths, data, RTINSTEONSERVER_EDITCONTROLLED_NAME, 0));
    return dialog(newDialog);
}
