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

#ifndef _SENSORHISTORY_H_
#define _SENSORHISTORY_H_

#include <Wt/WContainerWidget>
#include <Wt/WLabel>
#include <Wt/WVBoxLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WTime>
#include <Wt/WTimer>
#include <Wt/WStandardItemModel>

#include "WebWidget.h"
#include "MQTTSensorDefs.h"
#include "MQTTSensor.h"

#include <qfile.h>

using namespace Wt;
using namespace Wt::Chart;

class VideoWidgetMJPEG;
class KafkaConsumer;
class KafkaData;
class KafkaEntry;

class SensorHistory : public WebWidget
{
public:
    SensorHistory(WWidget *parent, WebServer *webServer);
    virtual ~SensorHistory();

    void makeInactive();
    void makeActive();
    virtual void setDevice(const QString& deviceName);

protected:
    void TimeTick();
    void layoutWidget();
    void setupCharts();
    void handleBackClicked();
    void handleStopClicked();
    void handlePauseClicked();
    void handleFrameMClicked();
    void handlePlayClicked();
    void handleFramePClicked();

    void handleSliderChanged();

    bool resetToIndex(int index);

    void displayCurrentData();
    void displayCurrentFrame();
    void displayCurrentSensor();

    void populateSensorData();

    WCartesianChart *initChart(int sensor, int minY, int maxY, int channels);

    virtual void layoutSizeChanged(int width, int height);
    VideoWidgetMJPEG *m_videoWidgetMJPEG;

    WText *m_status;
    WText *m_currentTime;
    WSlider *m_slider;

    WTime m_lastResizeTime;

    WTimer *m_timer;

    int m_deviceType;

    int m_layoutWidth;

    WCartesianChart *m_charts[MQTTSENSOR_TYPE_COUNT];

    KafkaConsumer *m_kafkaConsumer;
    KafkaData *m_kd;

    MQTTSensor *m_popSensor;

    QString m_currentFilename;
    QFile m_currentFile;

    QString m_videoPath;

    qint64 m_startTime;
    qint64 m_endTime;
    qint64 m_playTimeOffset;
    qint64 m_playTime;
    bool m_paused;

    WString m_dateTimeFormat;

    KafkaEntry *m_nextKafkaEntry;

    bool m_playMode;

    bool m_active;
 };

#endif // _SENSORHISTORY_H_
