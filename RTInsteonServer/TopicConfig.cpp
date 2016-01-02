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

#include "TopicConfig.h"
#include "RTInsteonServer.h"

TopicConfig::TopicConfig() : Dialog(RTINSTEONSERVER_TOPICCONFIG_NAME, RTINSTEONSERVER_TOPICCONFIG_DESC)
{
    setConfigDialog(true);
}

bool TopicConfig::setVar(const QString& name, const QJsonObject& var)
{
    bool changed = false;

    if (name == "statusTopic") {
        if (m_statusTopic != var.value(RTINSTEONJSON_CONFIG_VAR_VALUE).toString()) {
            changed = true;
            m_statusTopic = var.value(RTINSTEONJSON_CONFIG_VAR_VALUE).toString();
        }
    } else if (name == "controlTopic") {
        if (m_controlTopic != var.value(RTINSTEONJSON_CONFIG_VAR_VALUE).toString()) {
            changed = true;
            m_controlTopic = var.value(RTINSTEONJSON_CONFIG_VAR_VALUE).toString();
        }
    } else if (name == "managementCommandTopic") {
        if (m_managementCommandTopic != var.value(RTINSTEONJSON_CONFIG_VAR_VALUE).toString()) {
            changed = true;
            m_managementCommandTopic = var.value(RTINSTEONJSON_CONFIG_VAR_VALUE).toString();
        }
    } else if (name == "managementResponseTopic") {
        if (m_managementResponseTopic != var.value(RTINSTEONJSON_CONFIG_VAR_VALUE).toString()) {
            changed = true;
            m_managementResponseTopic = var.value(RTINSTEONJSON_CONFIG_VAR_VALUE).toString();
        }
    }
    return changed;
}

void TopicConfig::getDialog(QJsonObject& newDialog)
{
    clearDialog();
    addVar(createConfigStringVar("statusTopic", "Topic for device status (pub)", m_statusTopic));
    addVar(createConfigStringVar("controlTopic", "Topic for device control (sub)", m_controlTopic));
    addVar(createConfigStringVar("managementCommandTopic", "Topic for management commands (sub)", m_managementCommandTopic));
    addVar(createConfigStringVar("managementResponseTopic", "Topic for management responses (pub)", m_managementResponseTopic));
    return dialog(newDialog);
}

void TopicConfig::loadLocalData(const QJsonObject& /* param */)
{
    QSettings settings;

    settings.beginGroup(RTINSTEON_PARAMS_TOPICGROUP);
    m_statusTopic = settings.value(RTINSTEON_PARAMS_STATUSTOPIC).toString();
    m_controlTopic = settings.value(RTINSTEON_PARAMS_CONTROLTOPIC).toString();
    m_managementCommandTopic = settings.value(RTINSTEON_PARAMS_MANAGEMENTCOMMANDTOPIC).toString();
    m_managementResponseTopic = settings.value(RTINSTEON_PARAMS_MANAGEMENTRESPONSETOPIC).toString();
    settings.endGroup();
}

void TopicConfig::saveLocalData()
{
    QSettings settings;

    settings.beginGroup(RTINSTEON_PARAMS_TOPICGROUP);
    settings.setValue(RTINSTEON_PARAMS_STATUSTOPIC, m_statusTopic);
    settings.setValue(RTINSTEON_PARAMS_CONTROLTOPIC, m_controlTopic);
    settings.setValue(RTINSTEON_PARAMS_MANAGEMENTCOMMANDTOPIC, m_managementCommandTopic);
    settings.setValue(RTINSTEON_PARAMS_MANAGEMENTRESPONSETOPIC, m_managementResponseTopic);
    settings.endGroup();
}

