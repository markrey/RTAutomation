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
#include <qdebug.h>

#include "RTInsteonManager.h"
#include "ManagerClient.h"
#include "RTInsteonArgs.h"

#include "CommandService.h"
#include "GenericDialog.h"
#include "DialogMenu.h"

RTInsteonManager::RTInsteonManager()
    : QMainWindow()
{
    ui.setupUi(this);

    layoutWindow();

    m_dialog = NULL;

    m_client = new ManagerClient();

    connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));

    connect(m_client, SIGNAL(clientConnected()), this, SLOT(clientConnected()));
    connect(m_client, SIGNAL(clientDisconnected()), this, SLOT(clientDisconnected()));
    connect(this, SIGNAL(clientRestart()), m_client, SLOT(clientRestart()));
    connect(this, SIGNAL(clientEnable(bool)), m_client, SLOT(clientEnable(bool)));

    m_client->resumeThread();

    restoreWindowState();
    initStatusBar();

    setWindowTitle(RTInsteonArgs::getAppName());
}

void RTInsteonManager::closeEvent(QCloseEvent *)
{
    onDisconnect();
    if (m_client) {
        m_client->exitThread();
        m_client = NULL;
    }
    saveWindowState();
}

void RTInsteonManager::clientConnected()
{
    QSettings settings;

    m_brokerStatus->setText("Connected to " + settings.value(RTINSTEON_PARAMS_BROKERADDRESS).toString());
    m_serverName->setText(QString("Using automation server: ") + settings.value(RTINSTEON_PARAMS_SERVERID).toString());
    m_dialog = new DialogMenu(settings.value(RTINSTEON_PARAMS_SERVERID).toString(), this);
    connect(m_dialog, SIGNAL(sendAppData(QJsonObject)), m_client, SLOT(sendCommandData(QJsonObject)));
    connect(m_client, SIGNAL(receiveCommandData(QJsonObject)), m_dialog, SLOT(receiveAppData(QJsonObject)));
    m_dialog->exec();
    onDisconnect();
}

void RTInsteonManager::clientDisconnected()
{
    m_brokerStatus->setText("Disconnected");
    m_serverName->setText("");
}

void RTInsteonManager::layoutWindow()
{
    QSettings settings;

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *vLayout = new QVBoxLayout(centralWidget);

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(16);
    formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    vLayout->addLayout(formLayout);

    m_brokerAddress = new QLineEdit();
    m_brokerAddress->setText(settings.value(RTINSTEON_PARAMS_BROKERADDRESS).toString());
    m_brokerAddress->setToolTip("The MQTT broker address (e.g. tcp://localhost:1883");
    m_brokerAddress->setMinimumWidth(200);
    formLayout->addRow(tr("MQTT broker address:"), m_brokerAddress);

    m_clientID = new QLineEdit();
    m_clientID->setText(settings.value(RTINSTEON_PARAMS_CLIENTID).toString());
    m_clientID->setToolTip("The client ID is used to sign on to the NQTT broker");
    m_clientID->setMinimumWidth(200);
    formLayout->addRow(tr("MQTT client ID:"), m_clientID);

    m_clientSecret = new QLineEdit();
    m_clientSecret->setText(settings.value(RTINSTEON_PARAMS_CLIENTSECRET).toString());
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
    m_automationServerID->setText(settings.value(RTINSTEON_PARAMS_SERVERID).toString());
    m_automationServerID->setToolTip("The device ID of the automation server");
    m_automationServerID->setMinimumWidth(200);
    formLayout->addRow(tr("Automation server ID:"), m_automationServerID);

    settings.beginGroup(RTINSTEON_PARAMS_TOPICGROUP);

    m_managementCommandTopic = new QLineEdit();
    m_managementCommandTopic->setText(settings.value(RTINSTEON_PARAMS_MANAGEMENTCOMMANDTOPIC).toString());
    m_managementCommandTopic->setToolTip("The topic to publish for command messages (e.g. managementCommand)");
    m_managementCommandTopic->setMinimumWidth(200);
    formLayout->addRow(tr("Management command topic (pub):"), m_managementCommandTopic);

    m_managementResponseTopic = new QLineEdit();
    m_managementResponseTopic->setText(settings.value(RTINSTEON_PARAMS_MANAGEMENTRESPONSETOPIC).toString());
    m_managementResponseTopic->setToolTip("The topic to subscribe to for response operations (e.g. managementResponse)");
    m_managementResponseTopic->setMinimumWidth(200);
    formLayout->addRow(tr("Management response topic (sub):"), m_managementResponseTopic);

    settings.endGroup();

    QHBoxLayout *buttonLayout = new QHBoxLayout();

    m_connect = new QPushButton("Connect");
    buttonLayout->addWidget(m_connect);
    connect(m_connect, SIGNAL(clicked(bool)), this, SLOT(onConnect()));
    m_disconnect = new QPushButton("Disconnect");
    buttonLayout->addWidget(m_disconnect);
    m_disconnect->setEnabled(false);
    connect(m_disconnect, SIGNAL(clicked(bool)), this, SLOT(onDisconnect()));

    vLayout->addLayout(buttonLayout);

    vLayout->addStretch();

    setCentralWidget(centralWidget);
}

void RTInsteonManager::onConnect()
{
    QSettings settings;

    if (m_dialog != NULL) {
        delete m_dialog;
        m_dialog = NULL;
    }

    settings.setValue(RTINSTEON_PARAMS_BROKERADDRESS, m_brokerAddress->text());
    settings.setValue(RTINSTEON_PARAMS_CLIENTID, m_clientID->text());
    settings.setValue(RTINSTEON_PARAMS_CLIENTSECRET, m_clientSecret->text());
    settings.setValue(RTINSTEON_PARAMS_SERVERID, m_automationServerID->text());

    settings.beginGroup(RTINSTEON_PARAMS_TOPICGROUP);
    settings.setValue(RTINSTEON_PARAMS_MANAGEMENTCOMMANDTOPIC, m_managementCommandTopic->text());
    settings.setValue(RTINSTEON_PARAMS_MANAGEMENTRESPONSETOPIC, m_managementResponseTopic->text());
    settings.endGroup();

    m_connect->setEnabled(false);
    m_disconnect->setEnabled(true);
    emit clientEnable(true);
}

void RTInsteonManager::onDisconnect()
{
    if (m_dialog != NULL) {
        delete m_dialog;
        m_dialog = NULL;
    }
    m_connect->setEnabled(true);
    m_disconnect->setEnabled(false);
    emit clientEnable(false);
}

void RTInsteonManager::initStatusBar()
{
    m_brokerStatus = new QLabel(this);
    m_brokerStatus->setAlignment(Qt::AlignLeft);
    ui.statusBar->addWidget(m_brokerStatus, 1);
    m_serverName = new QLabel(this);
    m_serverName->setAlignment(Qt::AlignRight);
    ui.statusBar->addWidget(m_serverName, 1);
}

void RTInsteonManager::saveWindowState()
{
    QSettings settings;

    settings.beginGroup("Window");
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("State", saveState());

    settings.endGroup();
}

void RTInsteonManager::restoreWindowState()
{
    QSettings settings;

    settings.beginGroup("Window");
    restoreGeometry(settings.value("Geometry").toByteArray());
    restoreState(settings.value("State").toByteArray());

    settings.endGroup();

}

