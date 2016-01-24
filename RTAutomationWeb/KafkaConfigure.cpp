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

#include "KafkaConfigure.h"
#include "KafkaConsumer.h"
#include "RTAutomationWeb.h"

KafkaConfigure::KafkaConfigure() : Dialog(RTAUTOMATIONWEB_KAFKACONFIGURE_NAME, RTAUTOMATIONWEB_KAFKACONFIGURE_DESC)
{
    setConfigDialog(true);
}

bool KafkaConfigure::setVar(const QString& name, const QJsonObject& var)
{
    bool changed = false;

    if (name == "brokerList") {
        if (m_brokerList != var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString()) {
            changed = true;
            m_brokerList = var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString();
        }
    } else if (name == "videoPath") {
        if (m_videoPath != var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString()) {
            changed = true;
            m_videoPath = var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString();
        }
    }
    return changed;
}

void KafkaConfigure::getDialog(QJsonObject& newDialog)
{
    clearDialog();
    addVar(createConfigStringVar("brokerList", "Kafka broker list", m_brokerList));
    addVar(createConfigStringVar("videoPath", "Path to video data", m_videoPath));
    addVar(createGraphicsLineVar());
    addVar(createGraphicsStringVar("Restart app to implement changes"));
    return dialog(newDialog);
}

void KafkaConfigure::loadLocalData(const QJsonObject& /* param */)
{
    QSettings settings;

    settings.beginGroup(KAFKA_CONSUMER_GROUP);
    m_brokerList = settings.value(KAFKA_CONSUMER_BROKERS).toString();
    m_videoPath = settings.value(KAFKA_VIDEO_PATH).toString();
    settings.endGroup();
}

void KafkaConfigure::saveLocalData()
{
    QSettings settings;

    settings.beginGroup(KAFKA_CONSUMER_GROUP);
    settings.setValue(KAFKA_CONSUMER_BROKERS, m_brokerList);
    settings.setValue(KAFKA_VIDEO_PATH, m_videoPath);
    settings.endGroup();
}

