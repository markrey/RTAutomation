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

#include "TimerStatus.h"
#include "RTInsteonServer.h"

#include "InsteonTimer.h"
#include "InsteonDriver.h"

TimerStatus::TimerStatus(InsteonDriver *driver) : Dialog(RTINSTEONSERVER_TIMERSTATUS_NAME, RTINSTEONSERVER_TIMERSTATUS_DESC)
{
    m_insteonDriver = driver;
}

void TimerStatus::getDialog(QJsonObject& newDialog)
{
    QList<InsteonTimer> timerList = m_insteonDriver->getTimerList();

    QStringList headers;
    QList<int> widths;
    QStringList data;

    headers << "" << "Name" << "Time" << "DOW" << "Mode" << "Delta" << "Random"
        << "Armed" << "Device Count" << "Next Time";

    widths << 80 << 200 << 60 << 120 << 60 << 60 << 80 << 80 << 100 << 100;

    for (int i = 0; i < timerList.count(); i++) {
        InsteonTimer timer = timerList.at(i);
        data.append("Configure");
        data.append(timer.name);
        if (timer.mode == INSTEON_TIMER_MODE_TOD)
            data.append(timer.time.toString("hh:mm"));
        else
            data.append("---");

        static QString dow[7] = { "S ", "M ", "T ", "W ", "Th ", "F ", "Sa" };
        QString mode;
        QString dowText;

        for (int i = 0; i < 7; i++) {
            if (timer.daysOfWeek & (0x01 << i))
                dowText += dow[i];
        }
        data.append(dowText);

        if (timer.mode == INSTEON_TIMER_MODE_TOD)
            mode = "DOW";
        else if (timer.mode == INSTEON_TIMER_MODE_SUNRISE)
            mode = "Sunrise";
        else if (timer.mode == INSTEON_TIMER_MODE_SUNSET)
            mode = "Sunset";
        data.append(mode);
        data.append(QString::number(timer.deltaTime));
        data.append(timer.randomMode ? "true" : "false");
        data.append(timer.armed ? "true" : "false");
        data.append(QString::number(timer.devices.count()));
        data.append(timer.triggerTime.toString("hh:mm"));
    }

    clearDialog();
    addVar(createInfoTableVar("Timer status", headers, widths, data, RTINSTEONSERVER_TIMERCONFIG_NAME, 0));
    addVar(createInfoButtonVar("add", "Add a new timer", RTINSTEONSERVER_ADDTIMER_NAME));
    return dialog(newDialog, true);
}
