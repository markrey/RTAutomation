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

#include <Wt/WApplication>
#include <Wt/WServer>

#include "RootWebServer.h"
#include "WebAuth.h"
#include "WebApplication.h"
#include "WebServer.h"
#include "RTAutomationWeb.h"
#include "WebClient.h"

#include <qprocess.h>
#include <qfileinfo.h>
#include <qdebug.h>

#include "MQTTSensorDefs.h"
#include "RC.h"

#define TAG "RootWebServer"

Wt::WApplication *createApplication(const WEnvironment& env)
{
    WebApplication *app = new WebApplication(env);

    app->setTitle("RTMQTTWeb");

    app->messageResourceBundle().use(app->appRoot() + "templates");

    app->useStyleSheet("RTAutomationWeb.css");

    return app;
}

RootWebServer::RootWebServer() : RTAutomationThread()
{
    QProcess::execute("ln -s /usr/share/Wt/resources resources");
    QProcess::execute("cp ../RTAutomationWeb.css .");
    QProcess::execute("cp ../templates.xml .");
    QProcess::execute("cp ../cacert.pem .");
    QProcess::execute("cp ../dh512.pem .");
    QProcess::execute("cp ../privkey.pem .");
    QProcess::execute("chmod 600 *.pem");

    QList<int> remap;
    remap << MQTTSENSOR_TYPE_LIGHT << MQTTSENSOR_TYPE_TEMPERATURE << MQTTSENSOR_TYPE_HUMIDITY<< MQTTSENSOR_TYPE_PRESSURE
          << MQTTSENSOR_TYPE_ACCELX << MQTTSENSOR_TYPE_ACCELY << MQTTSENSOR_TYPE_ACCELZ;

    RC::setSensorRemap(remap);
}

void RootWebServer::initModule()
{
    QFileInfo sslKey("privkey.pem");

#ifdef WIN32
    int argc = 17;
#else
    int argc = 19;
#endif
    const char *argv[] = {    "./RTAutomationWeb",
#ifndef WIN32
                              "--accesslog",
                              "/dev/null",
#endif
                              "--docroot",
                              ".",
                              "--http-address",
                              "0.0.0.0",
                              "--http-port",
                              "8080",
                              "--https-address",
                              "0.0.0.0",
                              "--https-port",
                              "8081",
                              "--ssl-certificate",
                              "cacert.pem",
                              "--ssl-private-key",
                              "privkey.pem",
                              "--ssl-tmp-dh",
                              "dh512.pem"
                    };

    if (!sslKey.exists()) {
        argc -= 10;                             // take off ssl stuff if no key
        qDebug() << "Using http only";
    } else {
        qDebug() << "Using http and https";
    }

    try {

        m_server = new Wt::WServer(argv[0]);

        ((Wt::WServer *)m_server)->setServerConfiguration(argc,
                                            (char **)argv, WTHTTP_CONFIGURATION);
        ((Wt::WServer *)m_server)->addEntryPoint(Wt::Application, createApplication);

        WebAuth::configureAuth();

        while(1) {
            try {
                ((Wt::WServer *)m_server)->start();
                break;
            } catch (Wt::WServer::Exception& ) {
                sleep(1);
                std::cerr << "Trying to start server again";
            }
        }
    } catch (Wt::WServer::Exception& e) {
        std::cerr << e.what() << std::endl;
    } catch (std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
    }
}

void RootWebServer::stopModule()
{
   ((Wt::WServer *)m_server)->stop();
}
