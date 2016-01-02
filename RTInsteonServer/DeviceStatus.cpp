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

#include "DeviceStatus.h"
#include "RTInsteonServer.h"

#include "InsteonDevice.h"
#include "InsteonDriver.h"

DeviceStatus::DeviceStatus(InsteonDriver *driver) : Dialog(RTINSTEONSERVER_DEVICESTATUS_NAME, RTINSTEONSERVER_DEVICESTATUS_DESC)
{
    m_insteonDriver = driver;
}

void DeviceStatus::getDialog(QJsonObject& newDialog)
{
    QList<InsteonDevice> deviceList = m_insteonDriver->getDeviceList();

    QStringList headers;
    QList<int> widths;
    QStringList data;

    headers << "" << "Device ID" << "Name" << "Type" << "Group"
                 << "Flags" << "Current Level" << "State" << "Timeouts" << "Last Seen";

    widths << 80 << 100 << 250 << 250 << 60 << 60 << 120 << 120 << 80 << 80;

    for (int i = 0; i < deviceList.count(); i++) {
        InsteonDevice device = deviceList.at(i);
        data.append("Configure");
        data.append(InsteonDevice::deviceIDToString(device.deviceID));
        data.append(device.name);
        data.append(InsteonDevice::getTypeFromCat(device.deviceCat, device.deviceSubcat));
        data.append(QString::number(device.group));
        data.append(QString::number(device.flags));
        data.append(QString::number(device.currentLevel));
        data.append(device.state);
        data.append(QString::number(device.timeouts));
        data.append(device.lastTimeSeen.time().toString("HH:mm:ss"));
    }

    clearDialog();
    addVar(createInfoTableVar("Device status", headers, widths, data, RTINSTEONSERVER_DEVICECONFIG_NAME, 0));
    addVar(createInfoButtonVar("add", "Add a new device", RTINSTEONSERVER_ADDDEVICE_NAME));
    return dialog(newDialog, true);
}
