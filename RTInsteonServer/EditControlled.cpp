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

#include "EditControlled.h"
#include "InsteonDefs.h"
#include "InsteonDriver.h"
#include "RTInsteonServer.h"
#include "TimerConfig.h"
#include "RTInsteonLog.h"

#define TAG "EditControlled"

EditControlled::EditControlled(InsteonDriver *driver, TimerConfig *timerConfig)
        : Dialog(RTINSTEONSERVER_EDITCONTROLLED_NAME, RTINSTEONSERVER_EDITCONTROLLED_DESC)
{
    setConfigDialog(true);
    m_deviceIndex = 0;
    m_timerIndex = 0;
    m_insteonDriver = driver;
    m_timerConfig = timerConfig;
}

void EditControlled::getDialog(QJsonObject& newDialog)
{
    clearDialog();
    addVar(createInfoStringVar("Timer name", m_timerName));
    addVar(createInfoStringVar("Device name", m_deviceName));
    addVar(createGraphicsLineVar());

    addVar(createConfigBoolVar("inUse", "In use", m_inUse));
    addVar(createConfigRangedIntVar("level", "Level (0-255)", m_level, 0, 255));

    return dialog(newDialog);
}

bool EditControlled::setVar(const QString& name, const QJsonObject& var)
{
    bool changed = false;

    if (name == "level") {
        if (m_level != var.value(RTINSTEONJSON_CONFIG_VAR_VALUE).toInt()) {
            changed = true;
            m_level = var.value(RTINSTEONJSON_CONFIG_VAR_VALUE).toInt();
        }
    } else if (name == "inUse") {
        if (m_inUse != (unsigned int)var.value(RTINSTEONJSON_CONFIG_VAR_VALUE).toBool()) {
            changed = true;
            m_inUse = (unsigned int)var.value(RTINSTEONJSON_CONFIG_VAR_VALUE).toBool();
        }
    }
    return changed;
}


void EditControlled::loadLocalData(const QJsonObject& param)
{
    int dtIndex;

    m_deviceIndex = param.value(RTINSTEONJSON_PARAM_INDEX).toInt();
    m_timerIndex = m_timerConfig->getCurrentIndex();

    QList<InsteonTimer> timerList = m_insteonDriver->getTimerList();
    QList<InsteonDevice> deviceList = m_insteonDriver->getDeviceList();

    if (m_timerIndex >= timerList.count()) {
        RTInsteonLog::logError(TAG, QString("Timer index %1 bigger than table").arg(m_timerIndex));
        return;
    }

    InsteonTimer timer = timerList.at(m_timerIndex);

    if (m_deviceIndex >= deviceList.count()) {
        RTInsteonLog::logError(TAG, QString("Device index %1 bigger than device table").arg(m_deviceIndex));
        return;
    }

    InsteonDevice device = deviceList.at(m_deviceIndex);

    m_deviceName = device.name;
    m_timerName = timer.name;

    for (dtIndex = 0; dtIndex < timer.devices.count(); dtIndex++) {
        if (timer.devices.at(dtIndex).deviceID == device.deviceID)
            break;
    }

    if (dtIndex < timer.devices.count()) {
        m_level = timer.devices.at(dtIndex).level;
        m_inUse = true;
    } else {
        m_level = 0;
        m_inUse = false;
    }
}

void EditControlled::saveLocalData()
{
    int dtIndex;

    QList<InsteonTimer> timerList = m_insteonDriver->getTimerList();
    QList<InsteonDevice> deviceList = m_insteonDriver->getDeviceList();

    if (m_timerIndex >= timerList.count()) {
        RTInsteonLog::logError(TAG, QString("Timer index %1 bigger than table").arg(m_timerIndex));
        return;
    }

    InsteonTimer timer = timerList.at(m_timerIndex);

    if (m_deviceIndex >= deviceList.count()) {
        RTInsteonLog::logError(TAG, QString("Device index %1 bigger than device table").arg(m_deviceIndex));
        return;
    }

    InsteonDevice device = deviceList.at(m_deviceIndex);

    for (dtIndex = 0; dtIndex < timer.devices.count(); dtIndex++) {
        if (timer.devices.at(dtIndex).deviceID == device.deviceID)
            break;
    }

    if (dtIndex < timer.devices.count()) {
        if (!m_inUse) {
            timer.devices.removeAt(dtIndex);
        } else {
            timer.devices[dtIndex].level = m_level;
        }
    } else {
        if (m_inUse) {
            InsteonTimerDevice itd;
            itd.deviceID = device.deviceID;
            itd.level = m_level;
            timer.devices.append(itd);
        }
    }
    emit updateTimer(timer);
}
