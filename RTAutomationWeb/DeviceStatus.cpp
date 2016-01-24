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
#include "RTAutomationWeb.h"
#include "DeviceDatabase.h"

DeviceStatus::DeviceStatus() : Dialog(RTAUTOMATIONWEB_DEVICESTATUS_NAME, RTAUTOMATIONWEB_DEVICESTATUS_DESC)
{
}

void DeviceStatus::getDialog(QJsonObject& newDialog)
{
    QStringList deviceList = g_deviceDatabase->getDeviceList();

    QStringList headers;
    QList<int> widths;
    QStringList data;

    headers << "" << "Device name" << "Type";

    widths << 80 << 200 << 200;

    for (int i = 0; i < deviceList.count(); i++) {
        data.append("Configure");
        data.append(deviceList.at(i));
        data.append(g_deviceDatabase->displayType(g_deviceDatabase->getDeviceType(deviceList.at(i))));
    }

    clearDialog();
    addVar(createInfoTableVar("Device status", headers, widths, data, RTAUTOMATIONWEB_DEVICECONFIG_NAME, 0));
    addVar(createInfoButtonVar("add", "Add a new device", RTAUTOMATIONWEB_ADDDEVICE_NAME));
    return dialog(newDialog, true);
}
