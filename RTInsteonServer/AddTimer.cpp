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

#include "InsteonDriver.h"
#include "AddTimer.h"
#include "RTInsteonServer.h"
#include "RTInsteonJSONDefs.h"

#define TAG "AddTimer"

AddTimer::AddTimer(InsteonDriver *driver) : Dialog(RTINSTEONSERVER_ADDTIMER_NAME, RTINSTEONSERVER_ADDTIMER_DESC)
{
    setConfigDialog(true);
    m_insteonDriver = driver;
}

void AddTimer::getDialog(QJsonObject& newDialog)
{
    clearDialog();
    addVar(createConfigStringVar("name", "Timer name", m_name));
    return dialog(newDialog);
}

bool AddTimer::setVar(const QString& name, const QJsonObject& var)
{
    bool changed = false;

    if (name == "name") {
        if (m_name != var.value(RTINSTEONJSON_CONFIG_VAR_VALUE).toString()) {
            changed = true;
            m_name = var.value(RTINSTEONJSON_CONFIG_VAR_VALUE).toString();
        }
    }
    return changed;
}

void AddTimer::loadLocalData(const QJsonObject& /* param */)
{
    m_name = "Timer";
}

void AddTimer::saveLocalData()
{
    InsteonTimer timer;

    timer.name = m_name;
    timer.time.setHMS(0, 0, 0);
    timer.daysOfWeek = 0x7f;
    timer.mode = 0;
    timer.randomMode = false;
    timer.deltaTime = 0;
    timer.armed = false;

    emit addTimer(timer);
}
