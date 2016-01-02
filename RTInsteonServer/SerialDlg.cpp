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

#include "SerialDlg.h"
#include "InsteonDriver.h"

#include "RTInsteonServer.h"

#include "qextserialenumerator.h"
#include <qsettings.h>
#include <qstringlist.h>

SerialDlg::SerialDlg() : Dialog(RTINSTEONSERVER_SERIAL_NAME, RTINSTEONSERVER_SERIAL_DESC)
{
    setConfigDialog(true);
}

void SerialDlg::getDialog(QJsonObject& newDialog)
{
    QStringList portList;

    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
    foreach (QextPortInfo info, ports)
        portList.append(info.portName);

    clearDialog();
    addVar(createConfigSelectionFromListVar("serialPort", "Serial port", m_portName, portList));
    return dialog(newDialog);
}

bool SerialDlg::setVar(const QString& name, const QJsonObject& var)
{
    bool changed = false;

    if (name == "serialPort") {
        if (m_portName != var.value(RTINSTEONJSON_CONFIG_VAR_VALUE).toString()) {
            changed = true;
            m_portName = var.value(RTINSTEONJSON_CONFIG_VAR_VALUE).toString();
        }
    }
    return changed;
}

void SerialDlg::loadLocalData(const QJsonObject & /* param */)
{
    QSettings settings;
    settings.beginGroup(INSTEON_SETTINGS_PLM);

    m_portName = settings.value(INSTEON_SETTINGS_PLM_PORT).toString();

    settings.endGroup();
}

void SerialDlg::saveLocalData()
{
    QSettings settings;
    settings.beginGroup(INSTEON_SETTINGS_PLM);

    settings.setValue(INSTEON_SETTINGS_PLM_PORT, m_portName);

    settings.endGroup();
}
