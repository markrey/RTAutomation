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


#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WPushButton>
#include <Wt/Chart/WCartesianChart>

#include "WebServer.h"

#include <qdebug.h>
#include "SensorRealTime.h"
#include "VideoWidgetMJPEG.h"
#include "RC.h"
#include "MQTTSensor.h"
#include "DeviceDatabase.h"

#define SENSOR_CHART_X  400
#define SENSOR_CHART_Y  150

SensorRealTime::SensorRealTime(WWidget *parent, WebServer *webServer) : WebWidget(parent, webServer)
{
    layoutWidget();

    setLayoutSizeAware(true);
    m_layoutWidth = 0;
    m_lastResizeTime = WTime::currentServerTime();
    m_active = false;
}

SensorRealTime::~SensorRealTime()
{
    qDebug() << "~SensorRealTime";
}

void SensorRealTime::makeActive()
{
    Thumbnail thumb;

    if (m_deviceType == DEVICE_TYPE_RTMQTT_VIDEO) {
        thumb = g_deviceDatabase->getThumbnail(m_deviceName);
        m_videoWidgetMJPEG->newFrame(thumb.m_jpeg, thumb.m_width, thumb.m_height);
        m_videoWidgetMJPEG->makeActive();
    }
    m_active = true;
}

void SensorRealTime::makeInactive()
{
    m_active = false;
    m_videoWidgetMJPEG->makeInactive();
}

bool SensorRealTime::processJpegData(const QString& deviceName, const QByteArray& jpeg)
{
    if (!m_active)
        return false;
    if (deviceName != m_deviceName)
        return false;

    m_videoWidgetMJPEG->processJpegData(jpeg);
    return true;
}

void SensorRealTime::updateSensorData()
{
    WDateTime dt = WDateTime::currentDateTime().addSecs(-3600 * 4);
    if (m_deviceType != DEVICE_TYPE_RTMQTT_SENSOR)
        return;
    MQTTSensor *popSensor = g_deviceDatabase->getSensorData(m_deviceName);

    for (int sensor = 0; sensor < MQTTSENSOR_TYPE_COUNT; sensor++) {
        const MQTTSENSOR_DATA *sensorData = popSensor->sensorData(sensor);
        if (sensorData->valid) {
            if (!m_charts[sensor]->isVisible()) {
                m_charts[sensor]->show();
            }
            for (int i = 0; i < MQTTSENSOR_RECORD_LENGTH; i++) {
                m_charts[sensor]->model()->setData(i, 0, dt.addSecs(i - MQTTSENSOR_RECORD_LENGTH + 1));
                m_charts[sensor]->model()->setData(i, 1, sensorData->record.at(i));
                m_charts[sensor]->model()->setHeaderData(1,
                            WString(qPrintable(QString(" ") +
                            RC::name(sensor) +
                            " " + QString::number(popSensor->value(sensor), 'f', 2) +
                            " " + RC::units(sensor))));

            }
            m_charts[sensor]->axis(XAxis).setLabelInterval(130);
         } else {
            if (m_charts[sensor]->isVisible()) {
                m_charts[sensor]->hide();
            }
        }
    }
}

void SensorRealTime::setDevice(const QString& deviceName)
{
    WebWidget::setDevice(deviceName);

    for (int i = 0; i < MQTTSENSOR_TYPE_COUNT; i++) {
        m_charts[i]->hide();
    }
    m_deviceType = g_deviceDatabase->getDeviceType(deviceName);
}

void SensorRealTime::handleBackClicked()
{
    m_webServer->setMainPage();
    WApplication::instance()->setInternalPath(INTERNAL_PATH_MAIN,  true);
}

void SensorRealTime::handleHistoryClicked()
{
    m_webServer->setHistoryDevice(m_deviceName);
    WApplication::instance()->setInternalPath(INTERNAL_PATH_HISTORY_DEVICE,  true);
}

void SensorRealTime::layoutWidget()
{
    setupCharts();

    setMargin(0, Left | Right);

    WVBoxLayout *boxLayout = new WVBoxLayout(this);
    setLayout(boxLayout);

    m_videoWidgetMJPEG = new VideoWidgetMJPEG(this);
    boxLayout->addWidget(m_videoWidgetMJPEG, 0, AlignCenter | AlignMiddle);


    for (int sensor = 0; sensor < MQTTSENSOR_TYPE_COUNT; sensor++) {
        boxLayout->addWidget(m_charts[sensor]);
    }

    WHBoxLayout *buttonLayout = new WHBoxLayout();

    WPushButton *backButton = new WPushButton("Back");
    backButton->clicked().connect(this, &SensorRealTime::handleBackClicked);
    buttonLayout->addWidget(backButton, 0, AlignCenter | AlignMiddle);

    WPushButton *historyButton = new WPushButton("History");
    historyButton->clicked().connect(this, &SensorRealTime::handleHistoryClicked);
    buttonLayout->addWidget(historyButton, 0, AlignCenter | AlignMiddle);

    boxLayout->addLayout(buttonLayout);
}

void SensorRealTime::layoutSizeChanged(int width, int /*height*/)
{
    if (m_layoutWidth == width)
        return;

    m_layoutWidth = width;

    if (m_lastResizeTime.msecsTo(WTime::currentServerTime()) > 10) {
        m_lastResizeTime = WTime::currentServerTime();
        m_videoWidgetMJPEG->setDisplaySize(m_layoutWidth, (m_layoutWidth * 3)/4);

        for (int sensor = 0; sensor < MQTTSENSOR_TYPE_COUNT; sensor++)
            m_charts[sensor]->resize((m_layoutWidth * 7) / 8, SENSOR_CHART_Y);
    }
}

void SensorRealTime::setupCharts()
{
    for (unsigned i = 0; i < MQTTSENSOR_TYPE_COUNT; ++i) {
        m_charts[i] = initChart(i, RC::minValue(i), RC::maxValue(i), 1);
        m_charts[i]->resize(SENSOR_CHART_X, SENSOR_CHART_Y); // WPaintedWidget must be given explicit size
    }
}

WCartesianChart *SensorRealTime::initChart(int sensor, int minY, int maxY, int channels)
{
    WCartesianChart *chart;

    WStandardItemModel *model = new WStandardItemModel(MQTTSENSOR_RECORD_LENGTH, channels+1, this);
    model->setHeaderData(0, WString("X"));
    model->setHeaderData(1, WString(qPrintable(RC::name(sensor))));

    chart = new WCartesianChart(this);

    chart->setModel(model);        // set the model
    chart->setXSeriesColumn(0);    // set the column that holds the X data
    chart->setLegendEnabled(true); // enable the legend
    chart->setLegendLocation(Chart::LegendInside, Wt::Left, AlignTop);
    chart->setLegendColumns(1, 170);
    chart->setLegendStyle(WFont(), DotLine, NoBrush);
    chart->setType(ScatterPlot);   // set type to ScatterPlot

    WDateTime dt = WDateTime::currentDateTime();
    for (unsigned i = 0; i < MQTTSENSOR_RECORD_LENGTH; i++) {
        model->setData(i, 0, dt.addSecs(i - MQTTSENSOR_RECORD_LENGTH + 1));
        for (int ch = 0; ch < channels; ch++)
            model->setData(i, ch + 1, 0);
    }

    chart->axis(XAxis).setLocation(MinimumValue);
    chart->axis(YAxis).setLocation(MinimumValue);
    chart->axis(XAxis).setScale(DateTimeScale);
    chart->axis(XAxis).setLabelInterval(120);

    // Provide space for the X and Y axis and title.
    chart->setPlotAreaPadding(50, Left);
    chart->setPlotAreaPadding(25, Top | Bottom);

    // Add the curves

    for (int ch = 0; ch < channels; ch++) {
        WDataSeries s(ch + 1, LineSeries);
        WPen pen;
        pen.setColor(WColor(RC::color(sensor).red(), RC::color(sensor).green(), RC::color(sensor).blue()));
        s.setPen(pen);
        chart->addSeries(s);
    }

    chart->axis(YAxis).setRange(minY, maxY);

    chart->setMargin(1, Top | Bottom);            // add margin vertically
    chart->setMargin(WLength::Auto, Left | Right); // center horizontally
    return chart;
}



