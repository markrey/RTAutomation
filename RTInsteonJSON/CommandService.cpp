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
#include "RTInsteonLog.h"

#define TAG "CommandService"

CommandService::CommandService() : RTInsteonThread()
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
    if (!json.contains(RTINSTEONJSON_RECORD_TYPE))
        return;

    QString type = json.value(RTINSTEONJSON_RECORD_TYPE).toString();

    if (type == RTINSTEONJSON_RECORD_TYPE_PING) {
        if (!json.contains(RTINSTEONJSON_RECORD_COMMAND))
            return;
        QString command = json.value(RTINSTEONJSON_RECORD_COMMAND).toString();

        if (command == RTINSTEONJSON_COMMAND_REQUEST) {
            json[RTINSTEONJSON_RECORD_COMMAND] = RTINSTEONJSON_COMMAND_RESPONSE;
            emit sendCommandData(json);   // just send it back
        }
    } else if (type == RTINSTEONJSON_RECORD_TYPE_DIALOGMENU) {
        if (!json.contains(RTINSTEONJSON_RECORD_COMMAND))
            return;
        QString command = json.value(RTINSTEONJSON_RECORD_COMMAND).toString();

        if (command == RTINSTEONJSON_COMMAND_REQUEST) {
            processDialogMenuRequest(json);
        }
    } else if (type == RTINSTEONJSON_RECORD_TYPE_DIALOG) {
        if (!json.contains(RTINSTEONJSON_RECORD_COMMAND))
            return;
        QString command = json.value(RTINSTEONJSON_RECORD_COMMAND).toString();

        if (command == RTINSTEONJSON_COMMAND_REQUEST) {
            processDialogRequest(json);
        }
    } else if (type == RTINSTEONJSON_RECORD_TYPE_DIALOGUPDATE) {
        if (!json.contains(RTINSTEONJSON_RECORD_COMMAND))
            return;
        QString command = json.value(RTINSTEONJSON_RECORD_COMMAND).toString();

        if (command == RTINSTEONJSON_COMMAND_REQUEST) {
            processDialogUpdateRequest(json);
        }
    }
}

void CommandService::processDialogMenuRequest(const QJsonObject& request)
{
    RTInsteonJSON js;
    QJsonObject jo;

    jo[RTINSTEONJSON_DIALOGMENU_CONFIG] = RTInsteonJSON::getConfigDialogMenu();
    jo[RTINSTEONJSON_DIALOGMENU_INFO] = RTInsteonJSON::getInfoDialogMenu();
    js.addVar(jo, RTINSTEONJSON_RECORD_DATA);

    emit sendCommandData(js.completeResponse(RTINSTEONJSON_RECORD_TYPE_DIALOGMENU,
                                             RTINSTEONJSON_COMMAND_RESPONSE,
                                             request[RTINSTEONJSON_RECORD_PARAM].toObject(),
                                             (int)request[RTINSTEONJSON_RECORD_INDEX].toDouble()));
}

void CommandService::processDialogRequest(const QJsonObject& request)
{
    RTInsteonJSON js;
    Dialog *dialog;

    QJsonObject param = request[RTINSTEONJSON_RECORD_PARAM].toObject();
    QString dialogName = param[RTINSTEONJSON_PARAM_DIALOG_NAME].toString();

    dialog = RTInsteonJSON::mapNameToDialog(dialogName);

    if (dialog == NULL) {
        RTInsteonLog::logError(TAG, QString("No match for dialog name ") + dialogName);
        return;
    }

    if (dialog->isConfigDialog()) {
        dialog->loadLocalData(param);
        QJsonObject json;
        dialog->getDialog(json);
        js.addVar(json, RTINSTEONJSON_RECORD_DATA);
        emit sendCommandData(js.completeResponse(RTINSTEONJSON_RECORD_TYPE_DIALOG,
                                                 RTINSTEONJSON_COMMAND_RESPONSE,
                                                 param,
                                                 (int)request[RTINSTEONJSON_RECORD_INDEX].toDouble()));

    } else {
        dialog->loadLocalData(param);
        QJsonObject json;
        dialog->getDialog(json);
        js.addVar(json, RTINSTEONJSON_RECORD_DATA);
        emit sendCommandData(js.completeResponse(RTINSTEONJSON_RECORD_TYPE_DIALOG,
                                                 RTINSTEONJSON_COMMAND_RESPONSE,
                                                 param,
                                                 (int)request[RTINSTEONJSON_RECORD_INDEX].toDouble()));
    }
}

void CommandService::processDialogUpdateRequest(const QJsonObject& request)
{
    RTInsteonJSON js;
    Dialog *config;

    QJsonObject param = request[RTINSTEONJSON_RECORD_PARAM].toObject();
    QString dialogName = param[RTINSTEONJSON_PARAM_DIALOG_NAME].toString();

    config = RTInsteonJSON::mapNameToDialog(dialogName);

    if (config == NULL) {
        RTInsteonLog::logError(TAG, QString("No match for dialog name ") + dialogName);
        return;
    }

    if (!config->isConfigDialog()) {
        RTInsteonLog::logError(TAG, QString("Got request to update info dialog ") + dialogName);
        return;
    }
    config->loadLocalData(param);

    if (!request.contains(RTINSTEONJSON_RECORD_DATA)) {
        return;
    }

    QJsonObject dialog = request[RTINSTEONJSON_RECORD_DATA].toObject();
    if (dialog[RTINSTEONJSON_DIALOG_NAME].toString() != dialogName) {
        return;
    }

    if (config->setDialog(dialog))
        config->saveLocalData();

    emit sendCommandData(js.completeResponse(RTINSTEONJSON_RECORD_TYPE_DIALOGUPDATE,
                                             RTINSTEONJSON_COMMAND_RESPONSE,
                                             param,
                                             (int)request[RTINSTEONJSON_RECORD_INDEX].toDouble()));
}


