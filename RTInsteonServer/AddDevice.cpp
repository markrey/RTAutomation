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

#include "InsteonDriver.h"
#include "AddDevice.h"
#include "RTInsteonServer.h"
#include "RTAutomationJSONDefs.h"

#define TAG "AddDevice"

AddDevice::AddDevice(InsteonDriver *driver) : Dialog(RTINSTEONSERVER_ADDDEVICE_NAME, RTINSTEONSERVER_ADDDEVICE_DESC)
{
    setConfigDialog(true);
    m_insteonDriver = driver;
}

void AddDevice::getDialog(QJsonObject& newDialog)
{
    QStringList types = InsteonDevice::getDeviceTypes();

    clearDialog();
    addVar(createConfigRangedHexVar("id", "Insteon ID", m_id, 0, 0xffffff));
    addVar(createConfigSelectionFromListVar("type", "Insteon device type", m_type, types));
    addVar(createConfigStringVar("name", "Device name", m_name));
    addVar(createConfigRangedHexVar("group", "Insteon group", m_group, 0, 0xff));
    addVar(createConfigRangedHexVar("flags", "Insteon flags", m_flags, 0, 0xff));
    return dialog(newDialog);
}

bool AddDevice::setVar(const QString& name, const QJsonObject& var)
{
    bool changed = false;

    if (name == "type") {
        if (m_type != var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString()) {
            changed = true;
            m_type = var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString();
        }
    } else if (name == "name") {
        if (m_name != var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString()) {
            changed = true;
            m_name = var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString();
        }
    } else if (name == "group") {
        if (m_group != (unsigned int)var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toInt()) {
            changed = true;
            m_group = (unsigned int)var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toInt();
        }
    } else if (name == "flags") {
        if (m_flags != (unsigned int)var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toInt()) {
            changed = true;
            m_flags = (unsigned int)var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toInt();
        }
    } else if (name == "id") {
        if (m_id != (unsigned int)var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toInt()) {
            changed = true;
            m_id = (unsigned int)var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toInt();
        }
    }
    return changed;
}

void AddDevice::loadLocalData(const QJsonObject& /* param */)
{
    InsteonDevice device;
    m_type = device.getTypeFromCat(1, 1);
    m_name = "Device";
    m_group = 1;
    m_flags = 226;
    m_id = 0x111111;
}

void AddDevice::saveLocalData()
{
    InsteonDevice device;

    device.deviceID = m_id;
    device.name = m_name;
    device.group = m_group;
    device.flags = m_flags;
    device.getCatFromType(m_type, device.deviceCat, device.deviceSubcat);
    emit addDevice(device);
}
