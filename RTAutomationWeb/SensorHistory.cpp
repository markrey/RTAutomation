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
#include <Wt/WSlider>

#include "WebServer.h"

#include "SensorHistory.h"
#include "VideoWidgetMJPEG.h"
#include "RC.h"
#include "KafkaDatabase.h"
#include "KafkaConsumer.h"
#include "DeviceDatabase.h"
#include "RTAutomationLog.h"

#include <qdebug.h>
#include <qsettings.h>

#define SENSOR_CHART_X  400
#define SENSOR_CHART_Y  150

#define TAG "SensorHistory"

//  this is for EST

#define LOCALE_OFFSET (-5 * 60 * 60)

SensorHistory::SensorHistory(WWidget *parent, WebServer *webServer) : WebWidget(parent, webServer)
{
    layoutWidget();

    setLayoutSizeAware(true);
    m_layoutWidth = 0;
    m_lastResizeTime = WTime::currentServerTime();
    m_active = false;
    m_kafkaConsumer = NULL;
    m_popSensor = NULL;
    m_timer = new WTimer();
    m_timer->timeout().connect(this, &SensorHistory::TimeTick);

    m_dateTimeFormat = WString("dd/MM/yyyy hh:mm:ss");
}

SensorHistory::~SensorHistory()
{
    delete m_timer;
    m_timer = NULL;
    qDebug() << "~SensorHistory";
}

void SensorHistory::makeActive()
{
    QString topic;
    QSettings settings;

    m_kd = g_kafkaDatabase->getKafkaPtr(m_deviceName);

    switch (m_deviceType) {
    case DEVICE_TYPE_RTMQTT_VIDEO:
        m_videoWidgetMJPEG->makeActive();
        topic = "videometa";
        settings.beginGroup(KAFKA_CONSUMER_GROUP);
        m_videoPath = settings.value(KAFKA_VIDEO_PATH).toString();
        settings.endGroup();
        break;

    default:
        topic = g_deviceDatabase->getSubTopic(m_deviceType);
        break;

    }
    m_kafkaConsumer = new KafkaConsumer(m_deviceName, m_deviceName + "_" + topic,
                                        g_kafkaDatabase->getNextKafkaOffset(m_kd));

    m_kafkaConsumer->resumeThread();

    m_playMode = false;
    m_paused = true;
    m_timer->setInterval(1000);
    m_timer->start();
    m_active = true;
}

void SensorHistory::makeInactive()
{
    m_timer->stop();
    m_active = false;
    m_playMode = false;
    m_paused = true;
    if (m_deviceType == DEVICE_TYPE_RTMQTT_VIDEO)
        m_videoWidgetMJPEG->makeInactive();
    if (m_kafkaConsumer != NULL) {
        m_kafkaConsumer->exitThread();
        m_kafkaConsumer->thread()->wait(1000);
        m_kafkaConsumer = NULL;
    }
    if (m_popSensor != NULL)
        delete m_popSensor;
    m_popSensor = NULL;
    m_nextKafkaEntry = NULL;
}

void SensorHistory::TimeTick()
{
    double firstTime = 0, lastTime = 0;

    if (!m_playMode) {
        m_status->setText(WString("Waiting for database - got {1} records").arg(g_kafkaDatabase->getRecordCount(m_kd)));
        if (m_kd->m_receivedEOF) {
            // got all the data we need
            m_timer->stop();
            m_kafkaConsumer->exitThread();
            m_kafkaConsumer->thread()->wait(1000);
            m_kafkaConsumer = NULL;
            m_timer->setInterval(10);
            m_timer->start();
            m_playMode = true;
            if (resetToIndex(0)) {
                m_slider->setRange(0, m_kd->m_kafkaList.count());
                g_kafkaDatabase->getKafkaTimeInterval(m_kd, firstTime, lastTime);
                m_startTime = (qint64)(firstTime * 1000.0);
                m_endTime = (qint64)(lastTime * 1000.0);
                WDateTime first = WDateTime::fromTime_t(firstTime + LOCALE_OFFSET);
                WDateTime last = WDateTime::fromTime_t(lastTime + LOCALE_OFFSET);
                if (m_deviceType == DEVICE_TYPE_RTMQTT_SENSOR)
                    populateSensorData();
                m_status->setText(WString("Time interval: {1} - {2}")
                                  .arg(first.toString(m_dateTimeFormat)).arg(last.toString(m_dateTimeFormat)));
                displayCurrentData();
            } else {
                m_status->setText(WString("No data"));
                m_nextKafkaEntry = NULL;
            }
        }
        return;
    }

    // must be in play mode

    if (m_nextKafkaEntry == NULL)
        return;

    if (m_paused)
        return;

    qint64 now = QDateTime::currentMSecsSinceEpoch();

    if ((now - (m_playTime + m_playTimeOffset)) < 0)
        return;

    displayCurrentData();
}

void SensorHistory::handleStopClicked()
{
    if (!m_playMode)
        return;

    resetToIndex(0);
    displayCurrentData();
}

void SensorHistory::handlePauseClicked()
{
    if (!m_playMode)
        return;

    m_paused = true;
}

void SensorHistory::handleFrameMClicked()
{
    if (!m_playMode)
        return;

    m_paused = true;

    if (m_nextKafkaEntry == NULL) {
        resetToIndex(m_kd->m_kafkaList.count() - 2);
    } else {
        if (m_nextKafkaEntry->m_listIndex > 1)
            resetToIndex(m_nextKafkaEntry->m_listIndex - 2);
    }
    displayCurrentData();
}

void SensorHistory::handlePlayClicked()
{
    if (!m_playMode)
        return;

    if (m_nextKafkaEntry == NULL)
        return;

    resetToIndex(m_nextKafkaEntry->m_listIndex);
    displayCurrentData();
    m_paused = false;
}

void SensorHistory::handleFramePClicked()
{
    if (!m_playMode)
        return;

    if (m_nextKafkaEntry == NULL)
        return;

    m_paused = true;
    resetToIndex(m_nextKafkaEntry->m_listIndex);
    displayCurrentData();
}

void SensorHistory::handleSliderChanged()
{
    if (!m_playMode)
        return;

    m_paused = true;
    resetToIndex(m_slider->value());
    displayCurrentData();
}

bool SensorHistory::resetToIndex(int index)
{
    m_currentFilename = "";
    m_nextKafkaEntry = g_kafkaDatabase->getKafkaEntryByIndex(m_kd, index);
    if (m_nextKafkaEntry == NULL) {
        RTAutomationLog::logError(TAG, QString("Failed to get kafka entry for index %1").arg(index));
        return false;
    }
    m_playTime = (qint64)(m_nextKafkaEntry->m_timestamp * 1000.0);
    m_playTimeOffset = QDateTime::currentMSecsSinceEpoch() - m_playTime;
    m_paused = true;
    return true;
}

void SensorHistory::displayCurrentData()
{
    if (m_nextKafkaEntry == NULL)
        return;

    WDateTime current = WDateTime::fromTime_t(m_nextKafkaEntry->m_timestamp + LOCALE_OFFSET);
    m_currentTime->setText(WString("Time code: ") + current.toString(m_dateTimeFormat) + WString(":{1}")
                                   .arg(((int64_t)(m_nextKafkaEntry->m_timestamp * 1000.0)) % 1000));

    switch (m_deviceType) {
    case DEVICE_TYPE_RTMQTT_VIDEO:
        displayCurrentFrame();
        break;

    case DEVICE_TYPE_RTMQTT_SENSOR:
        displayCurrentSensor();
        break;
    }
    m_slider->setValue(m_nextKafkaEntry->m_listIndex);

    // advance to next entry

    m_nextKafkaEntry = g_kafkaDatabase->getKafkaEntryByIndex(m_kd, m_nextKafkaEntry->m_listIndex + 1);
    if (m_nextKafkaEntry == NULL) {
        return;
    }
    m_playTime = (qint64)(m_nextKafkaEntry->m_timestamp * 1000);
}

void SensorHistory::displayCurrentFrame()
{
    if (m_currentFilename != m_nextKafkaEntry->m_filename) {
        m_currentFile.close();
        m_currentFile.setFileName(m_videoPath + "/" + m_nextKafkaEntry->m_filename);
        m_currentFile.open(QIODevice::ReadOnly);
        m_currentFilename = m_nextKafkaEntry->m_filename;
    }
    m_currentFile.seek(m_nextKafkaEntry->m_fileOffset);
    QByteArray jpeg = m_currentFile.read(m_nextKafkaEntry->m_frameLength);

    m_videoWidgetMJPEG->processJpegData(jpeg);
}

void SensorHistory::populateSensorData()
{
    m_popSensor = new MQTTSensor(m_startTime, m_endTime);

    for (int i = 0; i < m_kd->m_kafkaList.count(); i++)
        m_popSensor->setRealTimeData(m_kd->m_kafkaList.at(i)->m_json);
}

void SensorHistory::displayCurrentSensor()
{
    int recordIndex;

    for (int sensor = 0; sensor < MQTTSENSOR_TYPE_COUNT; sensor++) {
        const MQTTSENSOR_DATA *sensorData = m_popSensor->sensorData(sensor);
        if (sensorData->valid) {
            if (!m_charts[sensor]->isVisible()) {
                m_charts[sensor]->show();
            }
            recordIndex = (qint64)(m_nextKafkaEntry->m_timestamp) - (m_startTime / 1000) - MQTTSENSOR_RECORD_LENGTH;
            if (recordIndex < 0)
                recordIndex = 0;

            if (recordIndex >= ((m_endTime / 1000) - MQTTSENSOR_RECORD_LENGTH))
                recordIndex = (m_endTime / 1000) - MQTTSENSOR_RECORD_LENGTH - 1;

            WDateTime dt = WDateTime::fromTime_t((qint64)(m_nextKafkaEntry->m_timestamp) + LOCALE_OFFSET);
            for (int i = 0; i < MQTTSENSOR_RECORD_LENGTH; i++) {
                m_charts[sensor]->model()->setData(i, 0, dt.addSecs(i - MQTTSENSOR_RECORD_LENGTH + 1));
                m_charts[sensor]->model()->setData(i, 1, sensorData->record.at(recordIndex + i));
                m_charts[sensor]->model()->setHeaderData(1,
                            WString(qPrintable(QString(" ") +
                            RC::name(sensor) +
                            " " + QString::number(sensorData->record.at(recordIndex + MQTTSENSOR_RECORD_LENGTH - 1), 'f', 2) +
                            " " + RC::units(sensor))));

            }
            m_charts[sensor]->axis(XAxis).setLabelInterval(120);
            m_charts[sensor]->axis(XAxis).setLabelFormat("mm:ss");
          } else {
            if (m_charts[sensor]->isVisible()) {
                m_charts[sensor]->hide();
            }
        }
    }
}

void SensorHistory::setDevice(const QString& deviceName)
{
    WebWidget::setDevice(deviceName);

    for (int i = 0; i < MQTTSENSOR_TYPE_COUNT; i++) {
        m_charts[i]->hide();
    }
    m_deviceType = g_deviceDatabase->getDeviceType(deviceName);
}

void SensorHistory::handleBackClicked()
{
    m_webServer->setMainPage();
    WApplication::instance()->setInternalPath(INTERNAL_PATH_MAIN,  true);
}

void SensorHistory::layoutWidget()
{
    WPushButton *button;

    setupCharts();

    setMargin(0, Left | Right);

    WVBoxLayout *boxLayout = new WVBoxLayout(this);
    setLayout(boxLayout);

    m_status = new WText();
    boxLayout->addWidget(m_status);

    m_currentTime = new WText();
    boxLayout->addWidget(m_currentTime);

    WHBoxLayout *controlLayout = new WHBoxLayout();

    controlLayout->addStretch(1);

    button = new WPushButton("Stop", this);
    button->clicked().connect(this, &SensorHistory::handleStopClicked);
    controlLayout->addWidget(button, 0, AlignCenter | AlignMiddle);

    button = new WPushButton("Pause", this);
    button->clicked().connect(this, &SensorHistory::handlePauseClicked);
    controlLayout->addWidget(button, 0, AlignCenter | AlignMiddle);

    button = new WPushButton("Frame -", this);
    button->clicked().connect(this, &SensorHistory::handleFrameMClicked);
    controlLayout->addWidget(button, 0, AlignCenter | AlignMiddle);

    button = new WPushButton("Play", this);
    button->clicked().connect(this, &SensorHistory::handlePlayClicked);
    controlLayout->addWidget(button, 0, AlignCenter | AlignMiddle);

    button = new WPushButton("Frame +", this);
    button->clicked().connect(this, &SensorHistory::handleFramePClicked);
    controlLayout->addWidget(button, 0, AlignCenter | AlignMiddle);

    controlLayout->addStretch(1);

    boxLayout->addLayout(controlLayout);

    m_slider = new WSlider();
    m_slider->valueChanged().connect(this, &SensorHistory::handleSliderChanged);
    boxLayout->addWidget(m_slider, 1);

    m_videoWidgetMJPEG = new VideoWidgetMJPEG(this);
    m_videoWidgetMJPEG->hide();
    boxLayout->addWidget(m_videoWidgetMJPEG, 0, AlignCenter | AlignMiddle);

    for (int sensor = 0; sensor < MQTTSENSOR_TYPE_COUNT; sensor++) {
        boxLayout->addWidget(m_charts[sensor]);
    }

    button = new WPushButton("Back", this);
    button->clicked().connect(this, &SensorHistory::handleBackClicked);
    boxLayout->addWidget(button, 0, AlignCenter | AlignMiddle);
}

void SensorHistory::layoutSizeChanged(int width, int /*height*/)
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

void SensorHistory::setupCharts()
{
    for (unsigned i = 0; i < MQTTSENSOR_TYPE_COUNT; ++i) {
        m_charts[i] = initChart(i, RC::minValue(i), RC::maxValue(i), 1);
        m_charts[i]->resize(SENSOR_CHART_X, SENSOR_CHART_Y); // WPaintedWidget must be given explicit size
    }
}

WCartesianChart *SensorHistory::initChart(int sensor, int minY, int maxY, int channels)
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



