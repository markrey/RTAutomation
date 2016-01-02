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

#include "LocationConfig.h"
#include "RTInsteonServer.h"
#include "InsteonDriver.h"

LocationConfig::LocationConfig() : Dialog(RTINSTEONSERVER_LOCATIONCONFIG_NAME, RTINSTEONSERVER_LOCATIONCONFIG_DESC)
{
    setConfigDialog(true);
}

bool LocationConfig::setVar(const QString& name, const QJsonObject& var)
{
    bool changed = false;

    if (name == "latitude") {
        if (m_latitude != var.value(RTINSTEONJSON_CONFIG_VAR_VALUE).toString()) {
            changed = true;
            m_latitude = var.value(RTINSTEONJSON_CONFIG_VAR_VALUE).toString();
        }
    } else if (name == "longitude") {
        if (m_longitude != var.value(RTINSTEONJSON_CONFIG_VAR_VALUE).toString()) {
            changed = true;
            m_longitude = var.value(RTINSTEONJSON_CONFIG_VAR_VALUE).toString();
        }
    }
    return changed;
}

void LocationConfig::getDialog(QJsonObject& newDialog)
{
    clearDialog();
    addVar(createConfigStringVar("latitude", "Latitude (floating point, north is +ve)", m_latitude));
    addVar(createConfigStringVar("longitude", "Longitude (floating point, west is +ve)", m_longitude));
    return dialog(newDialog);
}

void LocationConfig::loadLocalData(const QJsonObject& /* param */)
{
    QSettings settings;

    settings.beginGroup(INSTEON_SETTINGS_PLM);
    m_latitude = settings.value(INSTEON_SETTINGS_PLM_LATITUDE).toString();
    m_longitude = settings.value(INSTEON_SETTINGS_PLM_LONGITUDE).toString();
    settings.endGroup();
}

void LocationConfig::saveLocalData()
{
    QSettings settings;

    settings.beginGroup(INSTEON_SETTINGS_PLM);
    settings.setValue(INSTEON_SETTINGS_PLM_LATITUDE, m_latitude);
    settings.setValue(INSTEON_SETTINGS_PLM_LONGITUDE, m_longitude);
    settings.endGroup();
}

