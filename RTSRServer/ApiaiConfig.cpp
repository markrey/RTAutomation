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

#include "ApiaiConfig.h"
#include "RTSRServer.h"
#include "SpeechDecoder.h"

ApiaiConfig::ApiaiConfig() : Dialog(RTSRSERVER_APIAICONFIG_NAME, RTSRSERVER_APIAICONFIG_DESC, 500)
{
    setConfigDialog(true);
}

bool ApiaiConfig::setVar(const QString& name, const QJsonObject& var)
{
    bool changed = false;

    if (name == "key") {
        if (m_key != var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString()) {
            changed = true;
            m_key = var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString();
        }
    } else if (name == "token") {
        if (m_token != var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString()) {
            changed = true;
            m_token = var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString();
        }
    }
    return changed;
}

void ApiaiConfig::getDialog(QJsonObject& newDialog)
{
    clearDialog();
    addVar(createConfigStringVar("key", "Subscription key", m_key));
    addVar(createConfigStringVar("token", "Client access token", m_token));
    return dialog(newDialog);
}

void ApiaiConfig::loadLocalData(const QJsonObject& /* param */)
{
    QSettings settings;

    settings.beginGroup(SPEECH_DECODER_GROUP);
    m_key = settings.value(SPEECH_DECODER_KEY).toString();
    m_token = settings.value(SPEECH_DECODER_TOKEN).toString();
    settings.endGroup();
}

void ApiaiConfig::saveLocalData()
{
    QSettings settings;

    settings.beginGroup(SPEECH_DECODER_GROUP);
    settings.setValue(SPEECH_DECODER_KEY, m_key);
    settings.setValue(SPEECH_DECODER_TOKEN, m_token);
    settings.endGroup();
}

