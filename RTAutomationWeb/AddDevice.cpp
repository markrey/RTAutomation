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

#include "AddDevice.h"
#include "RTAutomationWeb.h"
#include "RTAutomationJSONDefs.h"
#include "DeviceDatabase.h"

#define TAG "AddDevice"

AddDevice::AddDevice() : Dialog(RTAUTOMATIONWEB_ADDDEVICE_NAME, RTAUTOMATIONWEB_ADDDEVICE_DESC)
{
    setConfigDialog(true);
}

void AddDevice::getDialog(QJsonObject& newDialog)
{
    clearDialog();
    addVar(createConfigStringVar("name", "Device name", m_deviceName));
    addVar(createConfigSelectionFromListVar("type", "Device type",
                    g_deviceDatabase->displayType(m_deviceType), g_deviceDatabase->listDeviceTypes()));
    return dialog(newDialog);
}

bool AddDevice::setVar(const QString& name, const QJsonObject& var)
{
    bool changed = false;

    if (name == "type") {
        if (g_deviceDatabase->displayType(m_deviceType) != var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString()) {
            changed = true;
            m_deviceType = g_deviceDatabase->mapStringTypeToType(var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString());
        }
    } else if (name == "name") {
        if (m_deviceName != var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString()) {
            changed = true;
            m_deviceName = var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString();
        }
    }
    return changed;
}

void AddDevice::loadLocalData(const QJsonObject& /* param */)
{
    m_deviceName = "device";
    m_deviceType = 0;
}

void AddDevice::saveLocalData()
{
    g_deviceDatabase->addDevice(m_deviceName, m_deviceType);
}
