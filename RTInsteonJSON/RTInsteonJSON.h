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

#ifndef RTINSTEONJSON_H
#define RTINSTEONJSON_H

#include <qstring.h>
#include <qstringlist.h>
#include "RTInsteonJSONDefs.h"

//  Basic dialog setting keys

#define RTINSTEON_PARAMS_BROKERADDRESS  "MQTTBrokerAddress"
#define RTINSTEON_PARAMS_CLIENTID       "MQTTClientID"
#define RTINSTEON_PARAMS_CLIENTSECRET   "MQTTClientSecret"
#define RTINSTEON_PARAMS_DEVICEID       "MQTTDeviceID"
#define RTINSTEON_PARAMS_SERVERID       "MQTServerID"

//  Topic settings keys

#define RTINSTEON_PARAMS_TOPICGROUP     "topicGroup"
#define RTINSTEON_PARAMS_STATUSTOPIC    "statusTopic"
#define RTINSTEON_PARAMS_CONTROLTOPIC   "controlTopic"
#define RTINSTEON_PARAMS_MANAGEMENTCOMMANDTOPIC   "managementCommandTopic"
#define RTINSTEON_PARAMS_MANAGEMENTRESPONSETOPIC   "managementResponseTopic"

class Dialog;

class RTInsteonJSON
{
public:
    RTInsteonJSON();
    QJsonObject completeRequest(const QString& type, const QString& command, const QJsonObject& param);
    QJsonObject completeResponse(const QString& type, const QString& command, const QJsonObject& param, int recordIndex);

    void addArray(const QJsonArray& array, const QString& key);

    void addVar(const QJsonObject& var, const QString& key);

    static void displayJson(QJsonObject json);
    static const QJsonArray& getConfigDialogMenu() { return m_configDialogMenu; }
    static const QJsonArray& getInfoDialogMenu() { return m_infoDialogMenu; }

    static void addConfigDialog(Dialog *dialog);
    static void addInfoDialog(Dialog *dialog);
    static void addToDialogList(Dialog *dialog) { m_dialogs.append(dialog); }
    static Dialog *mapNameToDialog(const QString& name);

private:
    QJsonObject m_jsonRecord;

    static int staticRecordIndex;
    static QJsonArray m_configDialogMenu;
    static QJsonArray m_infoDialogMenu;

    static QList<Dialog *> m_dialogs;
};

#endif // RTINSTEONJSON_H
