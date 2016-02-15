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

#include "Dialog.h"
#include "RTAutomationLog.h"

#define TAG "Dialog"

Dialog::Dialog(const QString& name, const QString& desc, int minWidth)
{
    m_configDialog = false;
    m_name = name;
    m_desc = desc;
    m_minWidth = minWidth;
}

void Dialog::clearDialog()
{
    m_varArray = QJsonArray();
}

void Dialog::dialog(QJsonObject& newDialog, bool updateFlag, const QString& cookie)
{
    newDialog[RTAUTOMATIONJSON_DIALOG_NAME] = m_name;
    newDialog[RTAUTOMATIONJSON_DIALOG_DESC] = m_desc;
    newDialog[RTAUTOMATIONJSON_DIALOG_MINWIDTH] = m_minWidth;
    newDialog[RTAUTOMATIONJSON_DIALOG_UPDATE] = updateFlag;
    newDialog[RTAUTOMATIONJSON_DIALOG_COOKIE] = cookie;
    newDialog[RTAUTOMATIONJSON_DIALOG_DATA] = m_varArray;
}

void Dialog::addVar(const QJsonObject& var)
{
    m_varArray.append(var);
}

//----------------------------------------------------------
//
//  Config vars

QJsonObject Dialog::createConfigStringVar(const QString& name, const QString& desc, const QString& value)
{
    QJsonObject json;

    json[RTAUTOMATIONJSON_DIALOG_VAR_NAME] = name;
    json[RTAUTOMATIONJSON_DIALOG_VAR_DESC] = desc;
    json[RTAUTOMATIONJSON_DIALOG_VAR_TYPE] = RTAUTOMATIONJSON_CONFIG_VAR_TYPE_STRING;
    json[RTAUTOMATIONJSON_CONFIG_VAR_VALUE] = value;
    return json;
}

QJsonObject Dialog::createConfigPasswordVar(const QString& name, const QString& desc, const QString& value)
{
    QJsonObject json;

    json[RTAUTOMATIONJSON_DIALOG_VAR_NAME] = name;
    json[RTAUTOMATIONJSON_DIALOG_VAR_DESC] = desc;
    json[RTAUTOMATIONJSON_DIALOG_VAR_TYPE] = RTAUTOMATIONJSON_CONFIG_VAR_TYPE_PASSWORD;
    json[RTAUTOMATIONJSON_CONFIG_VAR_VALUE] = value;
    return json;
}

QJsonObject Dialog::createConfigSelectionFromListVar(const QString& name, const QString& desc, const QString& value, const QStringList& list)
{
    QJsonObject json;
    QJsonArray sl;

    json[RTAUTOMATIONJSON_DIALOG_VAR_NAME] = name;
    json[RTAUTOMATIONJSON_DIALOG_VAR_DESC] = desc;
    json[RTAUTOMATIONJSON_DIALOG_VAR_TYPE] = RTAUTOMATIONJSON_CONFIG_VAR_TYPE_SELECTION;
    json[RTAUTOMATIONJSON_CONFIG_VAR_VALUE] = value;

    for (int i = 0; i < list.count(); i++) {
        QJsonObject sle;
        sle[RTAUTOMATIONJSON_CONFIG_VAR_ENTRY] = list.at(i);
        sl.append(sle);
    }
    json[RTAUTOMATIONJSON_CONFIG_STRING_ARRAY] = sl;
    return json;
}

QJsonObject Dialog::createConfigMultiStringFromListVar(const QString& name, const QString& desc, const QStringList& valueList, const QStringList& list)
{
    QJsonObject json;
    QJsonArray vsl;
    QJsonArray sl;

    json[RTAUTOMATIONJSON_DIALOG_VAR_NAME] = name;
    json[RTAUTOMATIONJSON_DIALOG_VAR_DESC] = desc;
    json[RTAUTOMATIONJSON_DIALOG_VAR_TYPE] = RTAUTOMATIONJSON_CONFIG_VAR_TYPE_STRING;
    for (int i = 0; i < valueList.count(); i++) {
        QJsonObject sle;
        sle[RTAUTOMATIONJSON_CONFIG_VAR_ENTRY] = valueList.at(i);
        vsl.append(sle);
    }
    json[RTAUTOMATIONJSON_CONFIG_VAR_VALUE_ARRAY] = vsl;

    for (int i = 0; i < list.count(); i++) {
        QJsonObject sle;
        sle[RTAUTOMATIONJSON_CONFIG_VAR_ENTRY] = list.at(i);
        sl.append(sle);
    }
    json[RTAUTOMATIONJSON_CONFIG_STRING_ARRAY] = sl;
    return json;
}

QJsonObject Dialog::createConfigAppNameVar(const QString& name, const QString& desc, const QString& value)
{
    QJsonObject json;

    json[RTAUTOMATIONJSON_DIALOG_VAR_NAME] = name;
    json[RTAUTOMATIONJSON_DIALOG_VAR_DESC] = desc;
    json[RTAUTOMATIONJSON_DIALOG_VAR_TYPE] = RTAUTOMATIONJSON_CONFIG_VAR_TYPE_APPNAME;
    json[RTAUTOMATIONJSON_CONFIG_VAR_VALUE] = value;
    return json;
}

QJsonObject Dialog::createConfigUIDVar(const QString& name, const QString& desc, const QString& value)
{
    QJsonObject json;

    json[RTAUTOMATIONJSON_DIALOG_VAR_NAME] = name;
    json[RTAUTOMATIONJSON_DIALOG_VAR_DESC] = desc;
    json[RTAUTOMATIONJSON_DIALOG_VAR_TYPE] = RTAUTOMATIONJSON_CONFIG_VAR_TYPE_UID;
    json[RTAUTOMATIONJSON_CONFIG_VAR_VALUE] = value;
    return json;
}

QJsonObject Dialog::createConfigServicePathVar(const QString& name, const QString& desc, const QString& path)
{
    QJsonObject json;

    json[RTAUTOMATIONJSON_DIALOG_VAR_NAME] = name;
    json[RTAUTOMATIONJSON_DIALOG_VAR_DESC] = desc;
    json[RTAUTOMATIONJSON_DIALOG_VAR_TYPE] = RTAUTOMATIONJSON_CONFIG_VAR_TYPE_SERVICEPATH;
    json[RTAUTOMATIONJSON_CONFIG_VAR_VALUE] = path;
    return json;
}

QJsonObject Dialog::createConfigBoolVar(const QString& name, const QString& desc, bool value)
{
    QJsonObject json;

    json[RTAUTOMATIONJSON_DIALOG_VAR_NAME] = name;
    json[RTAUTOMATIONJSON_DIALOG_VAR_DESC] = desc;
    json[RTAUTOMATIONJSON_DIALOG_VAR_TYPE] = RTAUTOMATIONJSON_CONFIG_VAR_TYPE_BOOL;
    json[RTAUTOMATIONJSON_CONFIG_VAR_VALUE] = value;
    return json;
}

QJsonObject Dialog::createConfigIntVar(const QString& name, const QString& desc, int value)
{
    QJsonObject json;

    json[RTAUTOMATIONJSON_DIALOG_VAR_NAME] = name;
    json[RTAUTOMATIONJSON_DIALOG_VAR_DESC] = desc;
    json[RTAUTOMATIONJSON_DIALOG_VAR_TYPE] = RTAUTOMATIONJSON_CONFIG_VAR_TYPE_INTEGER;
    json[RTAUTOMATIONJSON_CONFIG_VAR_VALUE] = value;
    return json;
}

QJsonObject Dialog::createConfigRangedIntVar(const QString& name, const QString& desc, int value, int minimum, int maxiumum)
{
    QJsonObject json;

    json[RTAUTOMATIONJSON_DIALOG_VAR_NAME] = name;
    json[RTAUTOMATIONJSON_DIALOG_VAR_DESC] = desc;
    json[RTAUTOMATIONJSON_DIALOG_VAR_TYPE] = RTAUTOMATIONJSON_CONFIG_VAR_TYPE_INTEGER;
    json[RTAUTOMATIONJSON_CONFIG_VAR_VALUE] = value;
    json[RTAUTOMATIONJSON_CONFIG_VAR_MINIMUM] = minimum;
    json[RTAUTOMATIONJSON_CONFIG_VAR_MAXIMUM] = maxiumum;
    return json;
}

QJsonObject Dialog::createConfigHexVar(const QString& name, const QString& desc, unsigned int value)
{
    QJsonObject json;

    json[RTAUTOMATIONJSON_DIALOG_VAR_NAME] = name;
    json[RTAUTOMATIONJSON_DIALOG_VAR_DESC] = desc;
    json[RTAUTOMATIONJSON_DIALOG_VAR_TYPE] = RTAUTOMATIONJSON_CONFIG_VAR_TYPE_HEXINTEGER;
    json[RTAUTOMATIONJSON_CONFIG_VAR_VALUE] = (int)value;
    return json;
}

QJsonObject Dialog::createConfigRangedHexVar(const QString& name, const QString& desc,
                                                    unsigned int value, unsigned int minimum, unsigned int maxiumum)
{
    QJsonObject json;

    json[RTAUTOMATIONJSON_DIALOG_VAR_NAME] = name;
    json[RTAUTOMATIONJSON_DIALOG_VAR_DESC] = desc;
    json[RTAUTOMATIONJSON_DIALOG_VAR_TYPE] = RTAUTOMATIONJSON_CONFIG_VAR_TYPE_HEXINTEGER;
    json[RTAUTOMATIONJSON_CONFIG_VAR_VALUE] = (int)value;
    json[RTAUTOMATIONJSON_CONFIG_VAR_MINIMUM] = (int)minimum;
    json[RTAUTOMATIONJSON_CONFIG_VAR_MAXIMUM] = (int)maxiumum;
    return json;
}

QJsonObject Dialog::createConfigButtonVar(const QString& name, const QString& desc, const QString& dialogName)
{
    QJsonObject json;

    json[RTAUTOMATIONJSON_DIALOG_VAR_NAME] = name;
    json[RTAUTOMATIONJSON_DIALOG_VAR_DESC] = desc;
    json[RTAUTOMATIONJSON_DIALOG_VAR_TYPE] = RTAUTOMATIONJSON_CONFIG_VAR_TYPE_BUTTON;
    json[RTAUTOMATIONJSON_CONFIG_VAR_VALUE] = dialogName;
    return json;
}

QJsonObject Dialog::createConfigDOWVar(const QString& name, const QString& desc, unsigned char value)
{
    QJsonObject json;

    json[RTAUTOMATIONJSON_DIALOG_VAR_NAME] = name;
    json[RTAUTOMATIONJSON_DIALOG_VAR_DESC] = desc;
    json[RTAUTOMATIONJSON_DIALOG_VAR_TYPE] = RTAUTOMATIONJSON_CONFIG_VAR_TYPE_DOW;
    json[RTAUTOMATIONJSON_CONFIG_VAR_VALUE] = value;
    return json;
}

//----------------------------------------------------------
//
//  Info vars

QJsonObject Dialog::createInfoStringVar(const QString& desc, const QString& value)
{
    QJsonObject json;

    json[RTAUTOMATIONJSON_DIALOG_VAR_NAME] = QString("");
    json[RTAUTOMATIONJSON_DIALOG_VAR_DESC] = desc;
    json[RTAUTOMATIONJSON_DIALOG_VAR_TYPE] = RTAUTOMATIONJSON_INFO_VAR_TYPE_STRING;
    json[RTAUTOMATIONJSON_INFO_VAR_VALUE] = value;
    return json;
}

QJsonObject Dialog::createInfoButtonVar(const QString& name, const QString& desc, const QString& dialogName)
{
    QJsonObject json;

    json[RTAUTOMATIONJSON_DIALOG_VAR_NAME] = name;
    json[RTAUTOMATIONJSON_DIALOG_VAR_DESC] = desc;
    json[RTAUTOMATIONJSON_DIALOG_VAR_TYPE] = RTAUTOMATIONJSON_INFO_VAR_TYPE_BUTTON;
    json[RTAUTOMATIONJSON_INFO_VAR_VALUE] = dialogName;
    return json;
}

QJsonObject Dialog::createInfoTableVar(const QString& desc, const QStringList& columnHeaders,
                                   const QList<int>& columnWidths, const QStringList& data, const QString& dialogName, int configColumn)
{
    QJsonObject json;
    QJsonArray ch;
    QJsonArray cw;

    json[RTAUTOMATIONJSON_DIALOG_VAR_NAME] = QString("");
    json[RTAUTOMATIONJSON_DIALOG_VAR_DESC] = desc;
    json[RTAUTOMATIONJSON_DIALOG_VAR_TYPE] = RTAUTOMATIONJSON_INFO_VAR_TYPE_TABLE;

    if (columnHeaders.count() != columnWidths.count()) {
        RTAutomationLog::logError(TAG, QString("Column header (%1)/width(%2) count mismatch")
                           .arg(columnHeaders.count()).arg(columnWidths.count()));
        return json;
    }

    for (int i = 0; i < columnHeaders.count(); i++) {
        QJsonObject columnHeader;
        columnHeader[RTAUTOMATIONJSON_CONFIG_VAR_ENTRY] = columnHeaders.at(i);
        ch.append(columnHeader);
        QJsonObject columnWidth;
        columnWidth[RTAUTOMATIONJSON_CONFIG_VAR_ENTRY] = columnWidths.at(i);
        cw.append(columnWidth);
    }
    json[RTAUTOMATIONJSON_INFO_TABLEHEADER_ARRAY] = ch;
    json[RTAUTOMATIONJSON_INFO_TABLECOLUMNWIDTH_ARRAY] = cw;

    if (configColumn != -1) {
        if (configColumn >= columnHeaders.count()) {
            RTAutomationLog::logError(TAG, QString("Table config column %1 exceeds table width %2")
                               .arg(configColumn).arg(columnHeaders.count()));
        } else {
            json[RTAUTOMATIONJSON_INFO_TABLE_CONFIG_COLUMN] = configColumn;
            json[RTAUTOMATIONJSON_INFO_TABLE_CONFIG_DIALOGNAME] = dialogName;
        }
    }

    QJsonArray dataArray;
    for (int i = 0; i < data.count(); i++) {
        QJsonObject jo;
        jo[RTAUTOMATIONJSON_INFO_VAR_ENTRY] = data.at(i);
        dataArray.append(jo);
    }
    json[RTAUTOMATIONJSON_CONFIG_VAR_VALUE_ARRAY] = dataArray;
    return json;
}

//----------------------------------------------------------
//
//  Graphics items

QJsonObject Dialog::createGraphicsStringVar(const QString& value, const QString& style)
{
    QJsonObject json;

    json[RTAUTOMATIONJSON_DIALOG_VAR_NAME] = QString("");
    json[RTAUTOMATIONJSON_DIALOG_VAR_DESC] = QString("");
    json[RTAUTOMATIONJSON_DIALOG_VAR_TYPE] = RTAUTOMATIONJSON_GRAPHICS_VAR_TYPE_STRING;
    json[RTAUTOMATIONJSON_GRAPHICS_VAR_VALUE] = value;
    json[RTAUTOMATIONJSON_GRAPHICS_VAR_STYLE] = style;
    return json;
}

QJsonObject Dialog::createGraphicsLineVar()
{
    QJsonObject json;

    json[RTAUTOMATIONJSON_DIALOG_VAR_NAME] = QString("");
    json[RTAUTOMATIONJSON_DIALOG_VAR_DESC] = QString("");
    json[RTAUTOMATIONJSON_DIALOG_VAR_TYPE] = RTAUTOMATIONJSON_GRAPHICS_VAR_TYPE_LINE;
    return json;
}

//----------------------------------------------------------
//
//  setDialog

bool Dialog::setDialog(const QJsonObject& json)
{
    bool changed = false;
    if (!json.contains(RTAUTOMATIONJSON_DIALOG_NAME)) {
        RTAutomationLog::logError(TAG, "No dialog name in setDialog");
        return false;
    }
    if (!json.contains(RTAUTOMATIONJSON_DIALOG_DESC)) {
        RTAutomationLog::logError(TAG, QString("No dialog desc in setDialog for ") + json.value(RTAUTOMATIONJSON_DIALOG_NAME).toString());
        return false;
    }

    if (!json.contains(RTAUTOMATIONJSON_DIALOG_DATA)) {
        RTAutomationLog::logError(TAG, QString("No dialog data in setDialog for ") + json.value(RTAUTOMATIONJSON_DIALOG_NAME).toString());
        return false;
    }

    QJsonArray varArray = json[RTAUTOMATIONJSON_DIALOG_DATA].toArray();

    for (int i = 0; i < varArray.count(); i++) {
        QJsonObject var = varArray.at(i).toObject();
        if (!var.contains(RTAUTOMATIONJSON_DIALOG_VAR_NAME)) {
            RTAutomationLog::logError(TAG, QString("Var with no name in ") + json.value(RTAUTOMATIONJSON_DIALOG_NAME).toString());
            continue;
        }
        QString name = var.value(RTAUTOMATIONJSON_DIALOG_VAR_NAME).toString();
        changed |= setVar(name, var);
    }
    return changed;
}
