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

#ifndef _SENSORWIDGET_H_
#define _SENSORWIDGET_H_

#include <Wt/WContainerWidget>
#include <Wt/WApplication>
#include <Wt/WText>

#include <qbytearray.h>

#include "RC.h"
#include "MQTTSensor.h"

using namespace Wt;

class SensorWidget : public WContainerWidget
{
public:
    SensorWidget(WContainerWidget *parent=0);
    virtual ~SensorWidget();

    void makeInactive();
    void makeActive();

    bool updateSensorData(MQTTSensor *popSensor);

    void setDisplaySize(int displayWidth, int displayHeight);

protected:
    int m_displayWidth;
    int m_displayHeight;

    WText *m_labels[MQTTSENSOR_TYPE_COUNT];
    WText *m_values[MQTTSENSOR_TYPE_COUNT];

};

#endif // _SENSORWIDGET_H_
