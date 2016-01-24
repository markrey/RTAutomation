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

#ifndef MQTTSENSORDEFS_H
#define MQTTSENSORDEFS_H

#include <qstringlist.h>
#include <qqueue.h>
#include <qcolor.h>

//  Display name of sensors

#define MQTTSENSOR_NAME_ACCELX           "Accel (X)"
#define MQTTSENSOR_NAME_ACCELY           "Accel (Y)"
#define MQTTSENSOR_NAME_ACCELZ           "Accel (Z)"
#define MQTTSENSOR_NAME_LIGHT            "Light intensity"
#define MQTTSENSOR_NAME_TEMPERATURE      "Temperature"
#define MQTTSENSOR_NAME_PRESSURE         "Pressure"
#define MQTTSENSOR_NAME_HUMIDITY         "Humidity"

// this symbol defines the advance interval - 1 second

#define MQTTSENSOR_TIME_INTERVAL         1000

// this symbol defines the number of records to keep - enough for 10 minutes

#define MQTTSENSOR_RECORD_LENGTH         600

// this is the length of the record in milliseconds

#define MQTTSENSOR_RECORD_TIMEMS         (MQTTSENSOR_TIME_INTERVAL * MQTTSENSOR_RECORD_LENGTH)

//  this value means don't display

#define MQTTSENSOR_NO_DATA               -1000000

//  min values for each sensor (used for chart)

#define MQTTSENSOR_MIN_ACCELX            -2.5
#define MQTTSENSOR_MIN_ACCELY            -2.5
#define MQTTSENSOR_MIN_ACCELZ            -2.5
#define MQTTSENSOR_MIN_LIGHT             0
#define MQTTSENSOR_MIN_TEMPERATURE       -20
#define MQTTSENSOR_MIN_PRESSURE          800
#define MQTTSENSOR_MIN_HUMIDITY          0

//  max values for each sensor (used for chart)

#define MQTTSENSOR_MAX_ACCELX            2.5
#define MQTTSENSOR_MAX_ACCELY            2.5
#define MQTTSENSOR_MAX_ACCELZ            2.5
#define MQTTSENSOR_MAX_LIGHT             2000
#define MQTTSENSOR_MAX_TEMPERATURE       80
#define MQTTSENSOR_MAX_PRESSURE          1300
#define MQTTSENSOR_MAX_HUMIDITY          100

// units of measurement

#define MQTTSENSOR_UNITS_ACCELX          "g"
#define MQTTSENSOR_UNITS_ACCELY          "g"
#define MQTTSENSOR_UNITS_ACCELZ          "g"
#define MQTTSENSOR_UNITS_LIGHT           "lux"
#define MQTTSENSOR_UNITS_TEMPERATURE     "deg C"
#define MQTTSENSOR_UNITS_PRESSURE        "hPa"
#define MQTTSENSOR_UNITS_HUMIDITY        "%RH"

// plot colors

#define MQTTSENSOR_COLOR_ACCELX          Qt::red
#define MQTTSENSOR_COLOR_ACCELY          Qt::green
#define MQTTSENSOR_COLOR_ACCELZ          Qt::blue
#define MQTTSENSOR_COLOR_LIGHT           Qt::darkBlue
#define MQTTSENSOR_COLOR_TEMPERATURE     Qt::magenta
#define MQTTSENSOR_COLOR_PRESSURE        Qt::cyan
#define MQTTSENSOR_COLOR_HUMIDITY        Qt::darkGreen

//  struct used to hold received sensor data

typedef struct
{
    bool valid;
    qreal value;
    qreal currentData;
    QQueue<qreal> record;
} MQTTSENSOR_DATA;

#define MQTTSENSOR_TYPE_ACCELX           0
#define MQTTSENSOR_TYPE_ACCELY           1
#define MQTTSENSOR_TYPE_ACCELZ           2
#define MQTTSENSOR_TYPE_LIGHT            3
#define MQTTSENSOR_TYPE_TEMPERATURE      4
#define MQTTSENSOR_TYPE_PRESSURE         5
#define MQTTSENSOR_TYPE_HUMIDITY         6

#define MQTTSENSOR_TYPE_COUNT            7

#endif // MQTTSENSORDEFS_H
