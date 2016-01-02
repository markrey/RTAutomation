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

#ifndef _RTINSTEONSERVER_H
#define _RTINSTEONSERVER_H

#include "MainConsole.h"

class InsteonServerClient;
class InsteonDriver;

//  RTINSTEONJSON defs

#define RTINSTEONSERVER_SERIAL_NAME         "serialPort"
#define RTINSTEONSERVER_SERIAL_DESC         "PLM serial port"

#define RTINSTEONSERVER_DEVICESTATUS_NAME   "deviceStatus"
#define RTINSTEONSERVER_DEVICESTATUS_DESC   "Device status"

#define RTINSTEONSERVER_TIMERSTATUS_NAME    "timerStatus"
#define RTINSTEONSERVER_TIMERSTATUS_DESC    "Timer status"

#define RTINSTEONSERVER_DEVICECONFIG_NAME   "deviceConfig"
#define RTINSTEONSERVER_DEVICECONFIG_DESC   "Configure device"

#define RTINSTEONSERVER_TIMERCONFIG_NAME    "timerConfig"
#define RTINSTEONSERVER_TIMERCONFIG_DESC    "Configure timer"

#define RTINSTEONSERVER_ADDDEVICE_NAME      "addDevice"
#define RTINSTEONSERVER_ADDDEVICE_DESC      "Add new device"

#define RTINSTEONSERVER_ADDTIMER_NAME       "addTimer"
#define RTINSTEONSERVER_ADDTIMER_DESC       "Add new timer"

#define RTINSTEONSERVER_CONTROLLEDDEVICES_NAME  "controlledDevices"
#define RTINSTEONSERVER_CONTROLLEDDEVICES_DESC  "Controlled devices"

#define RTINSTEONSERVER_EDITCONTROLLED_NAME  "editControlled"
#define RTINSTEONSERVER_EDITCONTROLLED_DESC  "Edit controlled device"

#define RTINSTEONSERVER_TOPICCONFIG_NAME    "topicConfig"
#define RTINSTEONSERVER_TOPICCONFIG_DESC    "Configure MQTT topics"

#define RTINSTEONSERVER_LOCATIONCONFIG_NAME    "locationConfig"
#define RTINSTEONSERVER_LOCATIONCONFIG_DESC    "Configure lattitude and longitude"

class RTInsteonServer : public MainConsole
{
    Q_OBJECT

public:
    RTInsteonServer(QObject *parent, bool daemonMode);

protected:
    void processInput(char c);

private:
    void showHelp();
    void showStatus();

    InsteonServerClient *m_client;
    InsteonDriver *m_insteonDriver;
};

#endif // _RTINSTEONSERVER_H
