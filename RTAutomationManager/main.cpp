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

#include "RTAutomationManager.h"
#include "RTAutomationArgs.h"
#include "RTAutomationJSON.h"

#include <qapplication.h>
#include <qsettings.h>

void presetDefaultSettings();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("richards-tech");
    QCoreApplication::setOrganizationDomain("richards-tech.com");
    QCoreApplication::setApplicationName("RTAutomationManager");
    presetDefaultSettings();

    RTAutomationManager *w = new RTAutomationManager();

    w->show();

    return a.exec();
}

void presetDefaultSettings()
{
    QSettings settings;
    if (!settings.contains(RTAUTOMATION_PARAMS_BROKERADDRESS))
        settings.setValue(RTAUTOMATION_PARAMS_BROKERADDRESS, "tcp://localhost:1883");
    if (!settings.contains(RTAUTOMATION_PARAMS_CLIENTID))
        settings.setValue(RTAUTOMATION_PARAMS_CLIENTID, "rtinsteonmanager");
    if (!settings.contains(RTAUTOMATION_PARAMS_CLIENTSECRET))
        settings.setValue(RTAUTOMATION_PARAMS_CLIENTSECRET, "rtinsteonmanager");
    if (!settings.contains(RTAUTOMATION_PARAMS_SERVERID))
        settings.setValue(RTAUTOMATION_PARAMS_SERVERID, "rtinsteonserver");

    settings.beginGroup(RTAUTOMATION_PARAMS_TOPICGROUP);

    if (!settings.contains(RTAUTOMATION_PARAMS_MANAGEMENTCOMMANDTOPIC))
        settings.setValue(RTAUTOMATION_PARAMS_MANAGEMENTCOMMANDTOPIC, "managementCommand");
    if (!settings.contains(RTAUTOMATION_PARAMS_MANAGEMENTRESPONSETOPIC))
        settings.setValue(RTAUTOMATION_PARAMS_MANAGEMENTRESPONSETOPIC, "managementResponse");

    settings.endGroup();
}
