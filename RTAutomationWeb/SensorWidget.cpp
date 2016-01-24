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

#include <Wt/WVBoxLayout>
#include <Wt/WHBoxLayout>

#include "SensorWidget.h"
#include <qdebug.h>

#include "DeviceDatabase.h"

SensorWidget::SensorWidget(WContainerWidget *parent) : WContainerWidget(parent)
{
    m_displayWidth = 0;
    m_displayHeight = 0;

    WHBoxLayout *mainLayout = new WHBoxLayout();
    WVBoxLayout *labels = new WVBoxLayout();
    WVBoxLayout *values = new WVBoxLayout();
    mainLayout->addLayout(labels);
    mainLayout->addLayout(values);

    setLayout(mainLayout);

    for (int i = 0; i < MQTTSENSOR_TYPE_COUNT; i++) {
        WText *text = new WText("...", this);
        text->hide();
        values->addWidget(text);
        m_values[i] = text;

        text = new WText(this);
        text->hide();
        labels->addWidget(text);
        m_labels[i] = text;
        m_labels[i]->setText(WString(qPrintable(RC::name(i) + ":")));
    }

    labels->addStretch(1);
    values->addStretch(1);
    mainLayout->addStretch(1);
}

SensorWidget::~SensorWidget()
{
    qDebug() << "~SensorWidget";
}

void SensorWidget::makeActive()
{
}

void SensorWidget::makeInactive()
{
}

bool SensorWidget::updateSensorData(MQTTSensor *popSensor)
{
    if (popSensor == NULL)
        return false;
    for (int sensor = 0; sensor < MQTTSENSOR_TYPE_COUNT; sensor++) {
        const MQTTSENSOR_DATA *sensorData = popSensor->sensorData(sensor);
        if (sensorData->valid) {
            if (!m_values[sensor]->isVisible()) {
                m_labels[sensor]->show();
                m_values[sensor]->show();
            }
            m_values[sensor]->setText(WString(qPrintable(QString::number(popSensor->value(sensor), 'f', 2) +
                                      " " + RC::units(sensor))));
        }
    }
    return true;
}

void SensorWidget::setDisplaySize(int displayWidth, int displayHeight)
{
    resize(displayWidth, (2 * displayHeight / 3));
}
