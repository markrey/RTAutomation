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

#include <qlabel.h>
#include <qboxlayout.h>
#include <qformlayout.h>
#include <qvalidator.h>

#include "InsteonDevice.h"

#include "SetInsteonLevelDlg.h"

SetInsteonLevelDlg::SetInsteonLevelDlg(QWidget *parent, int deviceID, QString name, int level)
    : QDialog(parent, Qt::WindowCloseButtonHint | Qt::WindowTitleHint)
{
    layoutWindow(deviceID, name, level);

    connect(m_okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(m_on, SIGNAL(clicked(bool)), this, SLOT(onClicked(bool)));
    connect(m_off, SIGNAL(clicked(bool)), this, SLOT(offClicked(bool)));

    setWindowTitle("Set Level");
}

int SetInsteonLevelDlg::newLevel()
{
    return m_level->value();
}

void SetInsteonLevelDlg::onClicked(bool)
{
    m_level->setValue(255);
}

void SetInsteonLevelDlg::offClicked(bool)
{
    m_level->setValue(0);
}

void SetInsteonLevelDlg::layoutWindow(int deviceID, QString name, int level)
{
    QVBoxLayout *vLayout = new QVBoxLayout();

    QFormLayout *formLayout = new QFormLayout();

    formLayout->addRow("Device ID", new QLabel(InsteonDevice::deviceIDToString(deviceID)));
    formLayout->addRow("Name", new QLabel(name));

    QHBoxLayout *levelBox = new QHBoxLayout();
    m_on = new QPushButton("On");
    m_off = new QPushButton("Off");
    m_level = new QSlider(Qt::Horizontal);
    m_level->setRange(0, 255);
    m_level->setTickInterval(15);
    m_level->setTickPosition(QSlider::TicksBelow);
    if ((level >= 0) && (level <= 255))
        m_level->setValue(level);
    else
        m_level->setValue(0);
    m_level->setMinimumWidth(200);
    levelBox->addWidget(m_off);
    levelBox->addWidget(m_level);
    levelBox->addWidget(m_on);

    formLayout->addRow("Level", levelBox);

    vLayout->addLayout(formLayout);

    QHBoxLayout *hLayout = new QHBoxLayout();

    m_okButton = new QPushButton("OK");
    m_cancelButton = new QPushButton("Cancel");

    hLayout->addStretch();
    hLayout->addWidget(m_okButton);
    hLayout->addWidget(m_cancelButton);

    vLayout->addSpacing(20);
    vLayout->addLayout(hLayout);
    setLayout(vLayout);
};
