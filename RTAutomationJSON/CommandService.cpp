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

#include "CommandService.h"
#include "About.h"
#include "BasicSetup.h"
#include "RTAutomationLog.h"

#define TAG "CommandService"

CommandService::CommandService() : RTAutomationThread()
{
}

void CommandService::initModule()
{
}

void CommandService::stopModule()
{

}


void CommandService::receiveCommandData(QJsonObject json)
{
    if (!json.contains(RTAUTOMATIONJSON_RECORD_TYPE))
        return;

    QString type = json.value(RTAUTOMATIONJSON_RECORD_TYPE).toString();

    if (type == RTAUTOMATIONJSON_RECORD_TYPE_PING) {
        if (!json.contains(RTAUTOMATIONJSON_RECORD_COMMAND))
            return;
        QString command = json.value(RTAUTOMATIONJSON_RECORD_COMMAND).toString();

        if (command == RTAUTOMATIONJSON_COMMAND_REQUEST) {
            json[RTAUTOMATIONJSON_RECORD_COMMAND] = RTAUTOMATIONJSON_COMMAND_RESPONSE;
            emit sendCommandData(json);   // just send it back
        }
    } else if (type == RTAUTOMATIONJSON_RECORD_TYPE_DIALOGMENU) {
        if (!json.contains(RTAUTOMATIONJSON_RECORD_COMMAND))
            return;
        QString command = json.value(RTAUTOMATIONJSON_RECORD_COMMAND).toString();

        if (command == RTAUTOMATIONJSON_COMMAND_REQUEST) {
            processDialogMenuRequest(json);
        }
    } else if (type == RTAUTOMATIONJSON_RECORD_TYPE_DIALOG) {
        if (!json.contains(RTAUTOMATIONJSON_RECORD_COMMAND))
            return;
        QString command = json.value(RTAUTOMATIONJSON_RECORD_COMMAND).toString();

        if (command == RTAUTOMATIONJSON_COMMAND_REQUEST) {
            processDialogRequest(json);
        }
    } else if (type == RTAUTOMATIONJSON_RECORD_TYPE_DIALOGUPDATE) {
        if (!json.contains(RTAUTOMATIONJSON_RECORD_COMMAND))
            return;
        QString command = json.value(RTAUTOMATIONJSON_RECORD_COMMAND).toString();

        if (command == RTAUTOMATIONJSON_COMMAND_REQUEST) {
            processDialogUpdateRequest(json);
        }
    }
}

void CommandService::processDialogMenuRequest(const QJsonObject& request)
{
    RTAutomationJSON js;
    QJsonObject jo;

    jo[RTAUTOMATIONJSON_DIALOGMENU_CONFIG] = RTAutomationJSON::getConfigDialogMenu();
    jo[RTAUTOMATIONJSON_DIALOGMENU_INFO] = RTAutomationJSON::getInfoDialogMenu();
    js.addVar(jo, RTAUTOMATIONJSON_RECORD_DATA);

    emit sendCommandData(js.completeResponse(RTAUTOMATIONJSON_RECORD_TYPE_DIALOGMENU,
                                             RTAUTOMATIONJSON_COMMAND_RESPONSE,
                                             request[RTAUTOMATIONJSON_RECORD_PARAM].toObject(),
                                             (int)request[RTAUTOMATIONJSON_RECORD_INDEX].toDouble()));
}

void CommandService::processDialogRequest(const QJsonObject& request)
{
    RTAutomationJSON js;
    Dialog *dialog;

    QJsonObject param = request[RTAUTOMATIONJSON_RECORD_PARAM].toObject();
    QString dialogName = param[RTAUTOMATIONJSON_PARAM_DIALOG_NAME].toString();

    dialog = RTAutomationJSON::mapNameToDialog(dialogName);

    if (dialog == NULL) {
        RTAutomationLog::logError(TAG, QString("No match for dialog name ") + dialogName);
        return;
    }

    if (dialog->isConfigDialog()) {
        dialog->loadLocalData(param);
        QJsonObject json;
        dialog->getDialog(json);
        js.addVar(json, RTAUTOMATIONJSON_RECORD_DATA);
        emit sendCommandData(js.completeResponse(RTAUTOMATIONJSON_RECORD_TYPE_DIALOG,
                                                 RTAUTOMATIONJSON_COMMAND_RESPONSE,
                                                 param,
                                                 (int)request[RTAUTOMATIONJSON_RECORD_INDEX].toDouble()));

    } else {
        dialog->loadLocalData(param);
        QJsonObject json;
        dialog->getDialog(json);
        js.addVar(json, RTAUTOMATIONJSON_RECORD_DATA);
        emit sendCommandData(js.completeResponse(RTAUTOMATIONJSON_RECORD_TYPE_DIALOG,
                                                 RTAUTOMATIONJSON_COMMAND_RESPONSE,
                                                 param,
                                                 (int)request[RTAUTOMATIONJSON_RECORD_INDEX].toDouble()));
    }
}

void CommandService::processDialogUpdateRequest(const QJsonObject& request)
{
    RTAutomationJSON js;
    Dialog *config;

    QJsonObject param = request[RTAUTOMATIONJSON_RECORD_PARAM].toObject();
    QString dialogName = param[RTAUTOMATIONJSON_PARAM_DIALOG_NAME].toString();

    config = RTAutomationJSON::mapNameToDialog(dialogName);

    if (config == NULL) {
        RTAutomationLog::logError(TAG, QString("No match for dialog name ") + dialogName);
        return;
    }

    if (!config->isConfigDialog()) {
        RTAutomationLog::logError(TAG, QString("Got request to update info dialog ") + dialogName);
        return;
    }
    config->loadLocalData(param);

    if (!request.contains(RTAUTOMATIONJSON_RECORD_DATA)) {
        return;
    }

    QJsonObject dialog = request[RTAUTOMATIONJSON_RECORD_DATA].toObject();
    if (dialog[RTAUTOMATIONJSON_DIALOG_NAME].toString() != dialogName) {
        return;
    }

    if (config->setDialog(dialog))
        config->saveLocalData();

    emit sendCommandData(js.completeResponse(RTAUTOMATIONJSON_RECORD_TYPE_DIALOGUPDATE,
                                             RTAUTOMATIONJSON_COMMAND_RESPONSE,
                                             param,
                                             (int)request[RTAUTOMATIONJSON_RECORD_INDEX].toDouble()));
}


