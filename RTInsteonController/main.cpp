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

#include "RTInsteonController.h"
#include "RTInsteonJSON.h"
#include "RTInsteonArgs.h"

#include <qapplication.h>

void presetDefaultSettings();

int main(int argc, char *argv[])
{
    RTInsteonArgs::setAppName("RTInsteonController");

    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("richards-tech");
    QCoreApplication::setOrganizationDomain("richards-tech.com");
    QCoreApplication::setApplicationName("RTInsteonController");
    presetDefaultSettings();

    a.setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents);

    RTInsteonController *w = new RTInsteonController();

    w->show();

    return a.exec();
}

void presetDefaultSettings()
{
    QSettings settings;
    if (!settings.contains(RTINSTEON_PARAMS_BROKERADDRESS))
        settings.setValue(RTINSTEON_PARAMS_BROKERADDRESS, "tcp://localhost:1883");
    if (!settings.contains(RTINSTEON_PARAMS_CLIENTID))
        settings.setValue(RTINSTEON_PARAMS_CLIENTID, "rtinsteoncontroller");
    if (!settings.contains(RTINSTEON_PARAMS_CLIENTSECRET))
        settings.setValue(RTINSTEON_PARAMS_CLIENTSECRET, "rtinsteoncontroller");
    if (!settings.contains(RTINSTEON_PARAMS_SERVERID))
        settings.setValue(RTINSTEON_PARAMS_SERVERID, "rtinsteonserver");

    settings.beginGroup(RTINSTEON_PARAMS_TOPICGROUP);

    if (!settings.contains(RTINSTEON_PARAMS_STATUSTOPIC))
        settings.setValue(RTINSTEON_PARAMS_STATUSTOPIC, "status");
    if (!settings.contains(RTINSTEON_PARAMS_CONTROLTOPIC))
        settings.setValue(RTINSTEON_PARAMS_CONTROLTOPIC, "control");

    settings.endGroup();
}
