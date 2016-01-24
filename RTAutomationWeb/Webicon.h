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

#ifndef _WEBICON_H_
#define _WEBICON_H_

#include <Wt/WSignal>
#include <Wt/WContainerWidget>

#include <qbytearray.h>
#include <qstring.h>

using namespace Wt;

class VideoWidgetMJPEG;
class SensorWidget;
class MQTTSensor;

class Webicon :  public WContainerWidget
{
public:
    Webicon(const QString& deviceName, WContainerWidget *parent = 0);

    bool newFrame(const QByteArray& bFrame, int width, int height);
    void setDisplaySize(int width, int height);
    void setName(const char *name);
    void updateSensorData();

    // Get the signal

    Wt::Signal<const char *>& clickedSignal() { return m_signal;}

protected:
    VideoWidgetMJPEG *m_videoWidget;
    SensorWidget *m_sensorWidget;

    void handleMouseClick();

    Wt::Signal<const char *> m_signal;

    QString m_deviceName;
    int m_deviceType;

    WLabel *m_name;
};

#endif // _WEBICON_H_
