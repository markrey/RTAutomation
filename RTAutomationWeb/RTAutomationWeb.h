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

#ifndef RTMQTTWEBSERVER_H
#define RTMQTTWEBSERVER_H

//  JSON defs

#define RTAUTOMATIONWEB_DEVICESTATUS_NAME   "deviceStatus"
#define RTAUTOMATIONWEB_DEVICESTATUS_DESC   "Device status"

#define RTAUTOMATIONWEB_DEVICECONFIG_NAME   "deviceConfig"
#define RTAUTOMATIONWEB_DEVICECONFIG_DESC   "Configure device"

#define RTAUTOMATIONWEB_ADDDEVICE_NAME      "addDevice"
#define RTAUTOMATIONWEB_ADDDEVICE_DESC      "Add new device"

#define RTAUTOMATIONWEB_ADDTIMER_NAME       "addTimer"
#define RTAUTOMATIONWEB_ADDTIMER_DESC       "Add new timer"

#define RTAUTOMATIONWEB_KAFKACONFIGURE_NAME       "kafkaConfigure"
#define RTAUTOMATIONWEB_KAFKACONFIGURE_DESC       "Configure Kafka consumer"

#include "MainConsole.h"

class WebClient;
class RootWebServer;
class DeviceDatabase;
class KafkaDatabase;

#define PRODUCT_TYPE  "RTAutomationWeb"

class RTAutomationWeb : public MainConsole
{
    Q_OBJECT

public:
    RTAutomationWeb(QObject *parent);

protected:
    void processInput(char c);
    void appExit();

private:
    void showHelp();
    void showStatus();

    WebClient *m_client;
    RootWebServer *m_webServer;
    DeviceDatabase *m_deviceDatabase;
    KafkaDatabase *m_kafkaDatabase;
};

#endif // MOTEWEB_H
