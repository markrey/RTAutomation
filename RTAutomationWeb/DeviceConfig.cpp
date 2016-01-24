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

#include "DeviceConfig.h"
#include "RTAutomationWeb.h"
#include "RTAutomationLog.h"

#define TAG "DeviceConfig"

DeviceConfig::DeviceConfig() : Dialog(RTAUTOMATIONWEB_DEVICECONFIG_NAME, RTAUTOMATIONWEB_DEVICECONFIG_DESC)
{
    setConfigDialog(true);
    m_index = 0;
}

void DeviceConfig::getDialog(QJsonObject& newDialog)
{
    m_delete = false;

    clearDialog();
    addVar(createInfoStringVar("Device name", m_deviceName));
    addVar(createConfigSelectionFromListVar("type", "Device type",
                    g_deviceDatabase->displayType(m_deviceType), g_deviceDatabase->listDeviceTypes()));
    addVar(createGraphicsLineVar());
    addVar(createConfigBoolVar("delete", "Delete device", m_delete));
    return dialog(newDialog);
}

bool DeviceConfig::setVar(const QString& name, const QJsonObject& var)
{
    bool changed = false;

    if (name == "type") {
        if (g_deviceDatabase->displayType(m_deviceType) != var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString()) {
            changed = true;
            m_deviceType = g_deviceDatabase->mapStringTypeToType(var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString());
        }
    } else if (name == "delete") {
        if (m_delete != var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toBool()) {
            changed = true;
            m_delete = var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toBool();
        }
    }
    return changed;
}

void DeviceConfig::loadLocalData(const QJsonObject& param)
{
    m_index = param.value(RTAUTOMATIONJSON_PARAM_INDEX).toInt();

    QStringList deviceList = g_deviceDatabase->getDeviceList();

    if (m_index >= deviceList.count()) {
        RTAutomationLog::logError(TAG, QString("Device index %1 bigger than table").arg(m_index));
        return;
    }
    m_deviceName = deviceList[m_index];

    m_deviceType = g_deviceDatabase->getDeviceType(m_deviceName);
}

void DeviceConfig::saveLocalData()
{
    if (!m_delete) {
        g_deviceDatabase->setDeviceType(m_deviceName, m_deviceType);
    } else {
        g_deviceDatabase->removeDevice(m_deviceName);
    }
}
