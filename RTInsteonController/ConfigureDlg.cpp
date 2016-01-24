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

#include <qboxlayout.h>
#include <qformlayout.h>

#include "ConfigureDlg.h"
#include "RTInsteonController.h"
#include "RTAutomationJSON.h"

ConfigureDlg::ConfigureDlg(QWidget *parent)
    : QDialog(parent, Qt::WindowCloseButtonHint | Qt::WindowTitleHint)
{
    layoutWindow();
    setWindowTitle("Configure RInsteonController");
    connect(m_buttons, SIGNAL(accepted()), this, SLOT(saveData()));
    connect(m_buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

void ConfigureDlg::saveData()
{
    QSettings settings;

    bool changed = false;

    if (m_brokerAddress->text() != settings.value(RTAUTOMATION_PARAMS_BROKERADDRESS).toString()) {
        changed = true;
        settings.setValue(RTAUTOMATION_PARAMS_BROKERADDRESS, m_brokerAddress->text());
    }
    if (m_clientID->text() != settings.value(RTAUTOMATION_PARAMS_CLIENTID).toString()) {
        changed = true;
        settings.setValue(RTAUTOMATION_PARAMS_CLIENTID, m_clientID->text());
    }
    if (m_clientSecret->text() != settings.value(RTAUTOMATION_PARAMS_CLIENTSECRET).toString()) {
        changed = true;
        settings.setValue(RTAUTOMATION_PARAMS_CLIENTSECRET, m_clientSecret->text());
    }
    if (m_automationServerID->text() != settings.value(RTAUTOMATION_PARAMS_SERVERID).toString()) {
        changed = true;
        settings.setValue(RTAUTOMATION_PARAMS_SERVERID, m_automationServerID->text());
    }

    settings.beginGroup(RTAUTOMATION_PARAMS_TOPICGROUP);

    if (m_statusTopic->text() != settings.value(RTAUTOMATION_PARAMS_STATUSTOPIC).toString()) {
        changed = true;
        settings.setValue(RTAUTOMATION_PARAMS_STATUSTOPIC, m_statusTopic->text());
    }
    if (m_controlTopic->text() != settings.value(RTAUTOMATION_PARAMS_CONTROLTOPIC).toString()) {
        changed = true;
        settings.setValue(RTAUTOMATION_PARAMS_CONTROLTOPIC, m_controlTopic->text());
    }

    settings.endGroup();

    if (changed)
        accept();
    else
        reject();
}

void ConfigureDlg::layoutWindow()
{
    QSettings settings;

    setModal(true);

    QVBoxLayout *centralLayout = new QVBoxLayout(this);
    centralLayout->setSpacing(20);
    centralLayout->setContentsMargins(11, 11, 11, 11);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(16);
    formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    m_brokerAddress = new QLineEdit();
    m_brokerAddress->setText(settings.value(RTAUTOMATION_PARAMS_BROKERADDRESS).toString());
    m_brokerAddress->setToolTip("The MQTT broker address (e.g. tcp://localhost:1883");
    m_brokerAddress->setMinimumWidth(200);
    formLayout->addRow(tr("MQTT broker address:"), m_brokerAddress);

    m_clientID = new QLineEdit();
    m_clientID->setText(settings.value(RTAUTOMATION_PARAMS_CLIENTID).toString());
    m_clientID->setToolTip("The client ID is used to sign on to the NQTT broker");
    m_clientID->setMinimumWidth(200);
    formLayout->addRow(tr("MQTT client ID:"), m_clientID);

    m_clientSecret = new QLineEdit();
    m_clientSecret->setText(settings.value(RTAUTOMATION_PARAMS_CLIENTSECRET).toString());
    m_clientSecret->setToolTip("The client secret may be used to sign on to the NQTT broker");
    m_clientSecret->setMinimumWidth(200);
    m_clientSecret->setEchoMode(QLineEdit::Password);
    formLayout->addRow(tr("MQTT client secret:"), m_clientSecret);

    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setMinimumWidth(20);
    formLayout->addRow(line);

    m_automationServerID = new QLineEdit();
    m_automationServerID->setText(settings.value(RTAUTOMATION_PARAMS_SERVERID).toString());
    m_automationServerID->setToolTip("The device ID of the automation server");
    m_automationServerID->setMinimumWidth(200);
    formLayout->addRow(tr("Automation server ID:"), m_automationServerID);

    settings.beginGroup(RTAUTOMATION_PARAMS_TOPICGROUP);

    m_statusTopic = new QLineEdit();
    m_statusTopic->setText(settings.value(RTAUTOMATION_PARAMS_STATUSTOPIC).toString());
    m_statusTopic->setToolTip("The topic to suscribe for status updates (e.g. status)");
    m_statusTopic->setMinimumWidth(200);
    formLayout->addRow(tr("Status topic (sub):"), m_statusTopic);

    m_controlTopic = new QLineEdit();
    m_controlTopic->setText(settings.value(RTAUTOMATION_PARAMS_CONTROLTOPIC).toString());
    m_controlTopic->setToolTip("The topic to publish for control operations (e.g. control)");
    m_controlTopic->setMinimumWidth(200);
    formLayout->addRow(tr("Control topic (pub):"), m_controlTopic);

    settings.endGroup();

    centralLayout->addLayout(formLayout);

    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    m_buttons->setCenterButtons(true);

    centralLayout->addWidget(m_buttons);
}
