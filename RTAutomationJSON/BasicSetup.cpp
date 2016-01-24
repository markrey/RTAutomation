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

#include <qsettings.h>

#include "BasicSetup.h"

BasicSetup::BasicSetup() : Dialog(RTAUTOMATIONJSON_DIALOG_NAME_BASICSETUP, RTAUTOMATIONJSON_DIALOG_DESC_BASICSETUP)
{
    setConfigDialog(true);
}

bool BasicSetup::setVar(const QString& name, const QJsonObject& var)
{
    bool changed = false;

    if (name == "brokerAddress") {
        if (m_brokerAddress != var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString()) {
            changed = true;
            m_brokerAddress = var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString();
        }
    } else if (name == "clientID") {
        if (m_clientID != var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString()) {
            changed = true;
            m_clientID = var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString();
        }
    } else if (name == "clientSecret") {
        if (m_clientSecret != var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString()) {
            changed = true;
            m_clientSecret = var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString();
        }
    } else if (name == "deviceID") {
        if (m_deviceID != var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString()) {
            changed = true;
            m_deviceID = var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString();
        }
    }
    return changed;
}

void BasicSetup::getDialog(QJsonObject& newDialog)
{
    clearDialog();
    addVar(createConfigStringVar("brokerAddress", "MQTT broker address", m_brokerAddress));
    addVar(createConfigStringVar("clientID", "MQTT client ID", m_clientID));
    addVar(createConfigPasswordVar("clientSecret", "MQTT client secret", m_clientSecret));
    addVar(createConfigStringVar("deviceID", "MQTT device ID", m_deviceID));
    addVar(createGraphicsLineVar());
    addVar(createGraphicsStringVar("Restart app to implement changes"));
    return dialog(newDialog);
}

void BasicSetup::loadLocalData(const QJsonObject& /* param */)
{
    QSettings settings;

    m_brokerAddress = settings.value(RTAUTOMATION_PARAMS_BROKERADDRESS).toString();
    m_clientID = settings.value(RTAUTOMATION_PARAMS_CLIENTID).toString();
    m_clientSecret = settings.value(RTAUTOMATION_PARAMS_CLIENTSECRET).toString();
    m_deviceID = settings.value(RTAUTOMATION_PARAMS_DEVICEID).toString();
}

void BasicSetup::saveLocalData()
{
    QSettings settings;

    settings.setValue(RTAUTOMATION_PARAMS_BROKERADDRESS, m_brokerAddress);
    settings.setValue(RTAUTOMATION_PARAMS_CLIENTID, m_clientID);
    settings.setValue(RTAUTOMATION_PARAMS_CLIENTSECRET, m_clientSecret);
    settings.setValue(RTAUTOMATION_PARAMS_DEVICEID, m_deviceID);
}

