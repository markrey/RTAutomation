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

#ifndef _WEBWIDGET_H_
#define _WEBWIDGET_H_

#include <Wt/WContainerWidget>
#include <Wt/WApplication>
#include <Wt/WImage>
#include <Wt/WString>

#include "WebServer.h"

#include <qbytearray.h>

using namespace Wt;

class MQTTSensor;

class WebWidget : public WContainerWidget
{
public:
    WebWidget(WWidget *parent, WebServer *webServer);

    //  Required overrides

    virtual void makeActive() = 0;
    virtual void makeInactive() = 0;

    //  Optional overrides

    virtual bool processJpegData(const QString& /* deviceName */, const QByteArray& /* jpeg */) { return false;}
    virtual void updateSensorData() {}
    virtual void setDevice(const QString& deviceName) { m_deviceName = deviceName; }
    virtual bool newFrame(const QString& /* deviceName */, const QByteArray& /*bFrame*/, int /*width*/, int /*height*/) { return false; }

protected:
    WebServer *m_webServer;
    WWidget *m_parent;

    WString displayFloat(float val, int precision);
    QString m_deviceName;
};

#endif // _WEBWIDGET_H_
