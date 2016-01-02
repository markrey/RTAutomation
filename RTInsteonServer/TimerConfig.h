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

#ifndef TIMERCONFIG_H
#define TIMERCONFIG_H

#include "InsteonTimer.h"

#include "Dialog.h"

class InsteonDriver;

class TimerConfig : public Dialog
{
    Q_OBJECT

public:
    TimerConfig(InsteonDriver *insteonDriver);
    int getCurrentIndex() { return m_index; }

    virtual void loadLocalData(const QJsonObject& param);
    virtual void saveLocalData();

    virtual bool setVar(const QString& name, const QJsonObject& var);
    virtual void getDialog(QJsonObject& newConfig);

signals:
    void updateTimer(InsteonTimer timer);
    void removeTimer(QString name);

private:
    QString m_name;
    int m_timeHour;
    int m_timeMinute;
    unsigned char m_daysOfWeek;
    QString m_mode;
    bool m_randomMode;
    int m_deltaTime;
    bool m_armed;
    bool m_delete;

    QStringList m_modeList;
    InsteonDriver *m_insteonDriver;
    int m_index;
};

#endif // TIMERCONFIG_H
