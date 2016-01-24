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

#include "Webicon.h"

#include <Wt/WVBoxLayout>
#include <Wt/WLabel>
#include <Wt/WText>

void Webicon::handleMouseClick()
{
    m_signal.emit(qPrintable(m_deviceName));
}

#include "VideoWidgetMJPEG.h"
#include "SensorWidget.h"
#include "MQTTSensor.h"

#include "DeviceDatabase.h"

Webicon::Webicon(const QString& deviceName, WContainerWidget *parent) : WContainerWidget(parent), m_signal(this)
{
    m_deviceName = deviceName;
    m_deviceType = g_deviceDatabase->getDeviceType(deviceName);

    WVBoxLayout *vLayout = new WVBoxLayout(this);
    setLayout(vLayout);
    vLayout->setContentsMargins(5, 5, 5, 5);

    m_name = new WLabel("...");
    vLayout->addWidget(m_name, AlignCenter | AlignMiddle);

    vLayout->addSpacing(10);

    switch (m_deviceType) {
    case DEVICE_TYPE_RTMQTT_SENSOR:
        m_sensorWidget = new SensorWidget(this);
        vLayout->addWidget(m_sensorWidget, AlignTop | AlignMiddle);
        break;

    default:
        m_videoWidget = new VideoWidgetMJPEG();
        vLayout->addWidget(m_videoWidget, AlignTop | AlignMiddle);
        break;
    }

    decorationStyle().setBorder
        (Wt::WBorder(Wt::WBorder::Solid, Wt::WBorder::Thin, Wt::gray));
    clicked().connect(this, &Webicon::handleMouseClick);

}

bool Webicon::newFrame(const QByteArray& bFrame, int width, int height)
{
    if (m_deviceType != DEVICE_TYPE_RTMQTT_VIDEO)
        return false;

    m_videoWidget->newFrame(bFrame, width, height);
    return true;
}

void Webicon::updateSensorData()
{
    if (m_deviceType != DEVICE_TYPE_RTMQTT_SENSOR)
        return;

    m_sensorWidget->updateSensorData(g_deviceDatabase->getSensorData(m_deviceName));
}

void Webicon::setDisplaySize(int width, int height)
{
    switch (m_deviceType) {
    case DEVICE_TYPE_RTMQTT_VIDEO:
        m_videoWidget->setDisplaySize(width, height);
        break;

    case DEVICE_TYPE_RTMQTT_SENSOR:
        m_sensorWidget->setDisplaySize(width, height);
        break;

    default:
        break;
    }
}

void Webicon::setName(const char *name)
{
    m_name->setText(WString("Device: ") + name);
}
