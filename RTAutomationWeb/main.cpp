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

#include "RTAutomationWeb.h"
#include "RTAutomationWebWindow.h"
#include "RTAutomationArgs.h"
#include "KafkaConsumer.h"

#include <qapplication.h>
#include <QSettings>

int runGuiApp(int argc, char *argv[]);
int runConsoleApp(int argc, char *argv[]);
void presetDefaultSettings();

int main(int argc, char *argv[])
{
    RTAutomationArgs::setAppName("RTAutomationWeb");
    if (RTAutomationArgs::checkConsoleModeFlag(argc, argv))
        return runConsoleApp(argc, argv);
    else
        return runGuiApp(argc, argv);
}

int runGuiApp(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("richards-tech");
    QCoreApplication::setOrganizationDomain("richards-tech.com");
    QCoreApplication::setApplicationName("RTAutomationWeb");
    presetDefaultSettings();

    RTAutomationWebWindow *w = new RTAutomationWebWindow();
    w->show();
    return a.exec();
}

int runConsoleApp(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setOrganizationName("richards-tech");
    QCoreApplication::setOrganizationDomain("richards-tech.com");
    QCoreApplication::setApplicationName("RTAutomationWeb");
    presetDefaultSettings();

    RTAutomationWeb cc(&a);
    return a.exec();
}

void presetDefaultSettings()
{
    QSettings settings;
    if (!settings.contains(RTAUTOMATION_PARAMS_BROKERADDRESS))
        settings.setValue(RTAUTOMATION_PARAMS_BROKERADDRESS, "tcp://localhost:1883");
    if (!settings.contains(RTAUTOMATION_PARAMS_CLIENTID))
        settings.setValue(RTAUTOMATION_PARAMS_CLIENTID, "rtautomationweb");
    if (!settings.contains(RTAUTOMATION_PARAMS_CLIENTSECRET))
        settings.setValue(RTAUTOMATION_PARAMS_CLIENTSECRET, "rtautomationweb");
    if (!settings.contains(RTAUTOMATION_PARAMS_DEVICEID))
        settings.setValue(RTAUTOMATION_PARAMS_DEVICEID, "rtautomationweb");

    settings.beginGroup(RTAUTOMATION_PARAMS_TOPICGROUP);

    if (!settings.contains(RTAUTOMATION_PARAMS_MANAGEMENTCOMMANDTOPIC))
        settings.setValue(RTAUTOMATION_PARAMS_MANAGEMENTCOMMANDTOPIC, "managementCommand");
    if (!settings.contains(RTAUTOMATION_PARAMS_MANAGEMENTRESPONSETOPIC))
        settings.setValue(RTAUTOMATION_PARAMS_MANAGEMENTRESPONSETOPIC, "managementResponse");

    settings.endGroup();

    settings.beginGroup(KAFKA_CONSUMER_GROUP);

    if (!settings.contains(KAFKA_CONSUMER_BROKERS))
        settings.setValue(KAFKA_CONSUMER_BROKERS, "localhost:9092");
    if (!settings.contains(KAFKA_VIDEO_PATH))
        settings.setValue(KAFKA_VIDEO_PATH, "./video");

    settings.endGroup();

}


