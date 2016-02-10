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
#include "RTSRServer.h"

TopicConfig::TopicConfig() : Dialog(RTSRSERVER_TOPICCONFIG_NAME, RTSRSERVER_TOPICCONFIG_DESC)
{
    setConfigDialog(true);
}

bool TopicConfig::setVar(const QString& name, const QJsonObject& var)
{
    bool changed = false;

    if (name == "audioTopic") {
        if (m_audioTopic != var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString()) {
            changed = true;
            m_audioTopic = var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString();
        }
    } else if (name == "decodedSpeechTopic") {
        if (m_decodedSpeechTopic != var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString()) {
            changed = true;
            m_decodedSpeechTopic = var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString();
        }
    } else if (name == "managementCommandTopic") {
        if (m_managementCommandTopic != var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString()) {
            changed = true;
            m_managementCommandTopic = var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString();
        }
    } else if (name == "managementResponseTopic") {
        if (m_managementResponseTopic != var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString()) {
            changed = true;
            m_managementResponseTopic = var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString();
        }
    }
    return changed;
}

void TopicConfig::getDialog(QJsonObject& newDialog)
{
    clearDialog();
    addVar(createConfigStringVar("audioTopic", "Topic for incoming audio stream (sub)", m_audioTopic));
    addVar(createConfigStringVar("decodedSpeechTopic", "Topic for outgoing decoded speech (pub)", m_decodedSpeechTopic));
    addVar(createGraphicsLineVar());
    addVar(createConfigStringVar("managementCommandTopic", "Topic for management commands (sub)", m_managementCommandTopic));
    addVar(createConfigStringVar("managementResponseTopic", "Topic for management responses (pub)", m_managementResponseTopic));
    return dialog(newDialog);
}

void TopicConfig::loadLocalData(const QJsonObject& /* param */)
{
    QSettings settings;

    settings.beginGroup(RTAUTOMATION_PARAMS_TOPICGROUP);
    m_managementCommandTopic = settings.value(RTAUTOMATION_PARAMS_MANAGEMENTCOMMANDTOPIC).toString();
    m_managementResponseTopic = settings.value(RTAUTOMATION_PARAMS_MANAGEMENTRESPONSETOPIC).toString();
    settings.endGroup();

    settings.beginGroup(RTSRSERVER_PARAMS_TOPIC_GROUP);
    m_audioTopic = settings.value(RTSRSERVER_PARAMS_AUDIO_TOPIC).toString();
    m_decodedSpeechTopic = settings.value(RTSRSERVER_PARAMS_DECODEDSPEECH_TOPIC).toString();
    settings.endGroup();
}

void TopicConfig::saveLocalData()
{
    QSettings settings;

    settings.beginGroup(RTAUTOMATION_PARAMS_TOPICGROUP);
    settings.setValue(RTAUTOMATION_PARAMS_MANAGEMENTCOMMANDTOPIC, m_managementCommandTopic);
    settings.setValue(RTAUTOMATION_PARAMS_MANAGEMENTRESPONSETOPIC, m_managementResponseTopic);
    settings.endGroup();

    settings.beginGroup(RTSRSERVER_PARAMS_TOPIC_GROUP);
    settings.setValue(RTSRSERVER_PARAMS_AUDIO_TOPIC, m_audioTopic);
    settings.setValue(RTSRSERVER_PARAMS_DECODEDSPEECH_TOPIC, m_decodedSpeechTopic);
    settings.endGroup();
}

