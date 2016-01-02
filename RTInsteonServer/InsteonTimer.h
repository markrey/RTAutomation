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

#ifndef INSTEON_TIMER_H
#define INSTEON_TIMER_H

#include <qstring.h>
#include <qdatetime.h>

#include "InsteonTimerDevice.h"

class InsteonTimer
{
public:
    InsteonTimer();
    InsteonTimer(const InsteonTimer &rhs);
    ~InsteonTimer();

    QString name;                                       // name of timer
    QTime time;                                         // trigger time
    unsigned char daysOfWeek;                           // flags for active on which days of week
    int mode;                                           // if using TOD, sunset or sunrise
    bool randomMode;                                    // if random mode enabled
    int deltaTime;                                      // delta time in minutes
    QList<InsteonTimerDevice> devices;					// devices being controlled
    bool armed;                                         // it has armed (ie not fired so far today)
    QTime triggerTime;                                  // today's calculated time
};

#endif // INSTEON_TIMER_H
