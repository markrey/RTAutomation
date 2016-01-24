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

#include "TimerConfig.h"
#include "InsteonDefs.h"
#include "InsteonDriver.h"
#include "RTInsteonServer.h"
#include "RTAutomationLog.h"

#define TAG "TimerConfig"

TimerConfig::TimerConfig(InsteonDriver *driver) : Dialog(RTINSTEONSERVER_TIMERCONFIG_NAME, RTINSTEONSERVER_TIMERCONFIG_DESC)
{
    setConfigDialog(true);
    m_index = 0;
    m_insteonDriver = driver;

    m_modeList << "Time of Day" << "Sunrise" << "Sunset";
}

void TimerConfig::getDialog(QJsonObject& newDialog)
{
    m_delete = false;

    clearDialog();
    addVar(createInfoStringVar("Timer name", m_name));
    addVar(createConfigRangedIntVar("timeHour", "Time (hour)", m_timeHour, 0, 23));
    addVar(createConfigRangedIntVar("timeMinute", "Time (minute)", m_timeMinute, 0, 59));
    addVar(createConfigDOWVar("dow", "Days of week", m_daysOfWeek));
    addVar(createConfigSelectionFromListVar("mode", "Mode", m_mode, m_modeList));
    addVar(createConfigBoolVar("randomMode", "Randomize delta", m_randomMode));
    addVar(createConfigRangedIntVar("deltaTime", "Delta time (mins)", m_deltaTime, 0, 60));
    addVar(createConfigBoolVar("armed", "Armed", m_armed));
    addVar(createConfigButtonVar("editDevices", "Edit controlled devices", RTINSTEONSERVER_CONTROLLEDDEVICES_NAME));

    addVar(createGraphicsLineVar());
    addVar(createConfigBoolVar("delete", "Delete timer", m_delete));
    return dialog(newDialog);
}

bool TimerConfig::setVar(const QString& name, const QJsonObject& var)
{
    bool changed = false;

    if (name == "timeHour") {
        if (m_timeHour != var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toInt()) {
            changed = true;
            m_timeHour = var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toInt();
        }
    } else if (name == "timeMinute") {
        if (m_timeMinute != var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toInt()) {
            changed = true;
            m_timeMinute = var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toInt();
        }
    } else if (name == "dow") {
        if (m_daysOfWeek != (unsigned int)var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toInt()) {
            changed = true;
            m_daysOfWeek = (unsigned int)var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toInt();
        }
    } else if (name == "mode") {
        if (m_mode != var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString()) {
            changed = true;
            m_mode = var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toString();
        }
    } else if (name == "randomMode") {
        if (m_randomMode != (unsigned int)var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toBool()) {
            changed = true;
            m_randomMode = (unsigned int)var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toBool();
        }
    } else if (name == "deltaTime") {
        if (m_deltaTime != var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toInt()) {
            changed = true;
            m_deltaTime = var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toInt();
        }
    } else if (name == "armed") {
        if (m_armed != (unsigned int)var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toBool()) {
            changed = true;
            m_armed = (unsigned int)var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toBool();
        }
    } else if (name == "delete") {
        if (m_delete != (unsigned int)var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toBool()) {
            changed = true;
            m_delete = (unsigned int)var.value(RTAUTOMATIONJSON_CONFIG_VAR_VALUE).toBool();
        }
    }
    return changed;
}


void TimerConfig::loadLocalData(const QJsonObject& param)
{
    m_index = param.value(RTAUTOMATIONJSON_PARAM_INDEX).toInt();

    QList<InsteonTimer> timerList = m_insteonDriver->getTimerList();

    if (m_index >= timerList.count()) {
        RTAutomationLog::logError(TAG, QString("Timer index %1 bigger than table").arg(m_index));
        return;
    }

    InsteonTimer timer = timerList.at(m_index);

    m_name = timer.name;
    m_timeHour = timer.time.hour();
    m_timeMinute = timer.time.minute();
    m_daysOfWeek = timer.daysOfWeek;
    m_mode = m_modeList.at(timer.mode);
    m_randomMode = timer.randomMode;
    m_deltaTime = timer.deltaTime;
    m_armed = timer.armed;

    m_delete = false;
}

void TimerConfig::saveLocalData()
{
    QList<InsteonTimer> timerList = m_insteonDriver->getTimerList();

    if (m_index >= timerList.count()) {
        RTAutomationLog::logError(TAG, QString("Timer index %1 bigger than table").arg(m_index));
        return;
    }

    InsteonTimer timer = timerList.at(m_index);

    if (!m_delete) {
        timer.name = m_name;
        timer.time.setHMS(m_timeHour, m_timeMinute, 0);
        timer.daysOfWeek = m_daysOfWeek;
        timer.mode = m_modeList.indexOf(m_mode);
        timer.randomMode = m_randomMode;
        timer.deltaTime = m_deltaTime;
        timer.armed = m_armed;
        emit updateTimer(timer);
    } else {
        emit removeTimer(timer.name);
    }
}
