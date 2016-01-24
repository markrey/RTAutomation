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

#include "RTAutomationJSON.h"
#include "Dialog.h"
#include "RTAutomationLog.h"

#define TAG "RTAutomationJSON"

int RTAutomationJSON::staticRecordIndex = 0;
QJsonArray RTAutomationJSON::m_configDialogMenu;
QJsonArray RTAutomationJSON::m_infoDialogMenu;
QList<Dialog *> RTAutomationJSON::m_dialogs;

RTAutomationJSON::RTAutomationJSON()
{
}

void RTAutomationJSON::addArray(const QJsonArray& array, const QString& key)
{
    m_jsonRecord[key] = array;
}

void RTAutomationJSON::addVar(const QJsonObject& var, const QString& key)
{
    m_jsonRecord[key] = var;
}

QJsonObject RTAutomationJSON::completeRequest(const QString& type, const QString& command, const QJsonObject& param)
{
    m_jsonRecord[RTAUTOMATIONJSON_RECORD_TYPE] = type;
    m_jsonRecord[RTAUTOMATIONJSON_RECORD_COMMAND] = command;
    m_jsonRecord[RTAUTOMATIONJSON_RECORD_PARAM] = param;
    m_jsonRecord[RTAUTOMATIONJSON_RECORD_INDEX] = staticRecordIndex++;
    return m_jsonRecord;
}

QJsonObject RTAutomationJSON::completeResponse(const QString& type, const QString& command, const QJsonObject& param, int recordIndex)
{
    m_jsonRecord[RTAUTOMATIONJSON_RECORD_TYPE] = type;
    m_jsonRecord[RTAUTOMATIONJSON_RECORD_COMMAND] = command;
    m_jsonRecord[RTAUTOMATIONJSON_RECORD_PARAM] = param;
    m_jsonRecord[RTAUTOMATIONJSON_RECORD_INDEX] = recordIndex;
    return m_jsonRecord;
}

void RTAutomationJSON::displayJson(QJsonObject json)
{
    RTAutomationLog::logDebug(TAG, QJsonDocument(json).toJson());
}

void RTAutomationJSON::addConfigDialog(Dialog *dialog)
{
    QJsonObject dme;

    m_dialogs.append(dialog);
    dme[RTAUTOMATIONJSON_DIALOGMENU_NAME] = dialog->getName();
    dme[RTAUTOMATIONJSON_DIALOGMENU_DESC] = dialog->getDesc();
    m_configDialogMenu.append(dme);
}

void RTAutomationJSON::addInfoDialog(Dialog *dialog)
{
    QJsonObject dme;

    m_dialogs.append(dialog);
    dme[RTAUTOMATIONJSON_DIALOGMENU_NAME] = dialog->getName();
    dme[RTAUTOMATIONJSON_DIALOGMENU_DESC] = dialog->getDesc();
    m_infoDialogMenu.append(dme);
}

Dialog *RTAutomationJSON::mapNameToDialog(const QString &name)
{
    for (int i = 0; i < m_dialogs.count(); i++) {
        if (name == m_dialogs.at(i)->getName())
            return m_dialogs.at(i);
    }
    return NULL;
}

