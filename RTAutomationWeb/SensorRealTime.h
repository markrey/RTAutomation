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

#ifndef _SENSORREALTIME_H_
#define _SENSORREALTIME_H_

#include <Wt/WContainerWidget>
#include <Wt/WLabel>
#include <Wt/WVBoxLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WTime>
#include <Wt/WStandardItemModel>

#include "WebWidget.h"
#include "MQTTSensorDefs.h"

using namespace Wt;
using namespace Wt::Chart;

class VideoWidgetMJPEG;

class SensorRealTime : public WebWidget
{
public:
    SensorRealTime(WWidget *parent, WebServer *webServer);
    virtual ~SensorRealTime();

    void makeInactive();
    void makeActive();
    virtual void setDevice(const QString& deviceName);
    virtual bool processJpegData(const QString &deviceName, const QByteArray &jpeg);
    virtual void updateSensorData();

protected:
    void layoutWidget();
    void setupCharts();
    WCartesianChart *initChart(int sensor, int minY, int maxY, int channels);

    virtual void layoutSizeChanged(int width, int height);
    VideoWidgetMJPEG *m_videoWidgetMJPEG;

    WTime m_lastResizeTime;

    void handleBackClicked();
    void handleHistoryClicked();

    int m_deviceType;

    int m_layoutWidth;

    WCartesianChart *m_charts[MQTTSENSOR_TYPE_COUNT];

    bool m_active;
 };

#endif // _SENSORREALTIME_H_
