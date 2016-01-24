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

#include "RTInsteonController.h"
#include "MainTable.h"
#include "SetInsteonLevelDlg.h"
#include "ConfigureDlg.h"
#include "RTAutomationArgs.h"
#include "RTAutomationJSON.h"

#include <qheaderview.h>
#include <qboxlayout.h>
#include <qaction.h>
#include <QMessageBox>
#include <qdir.h>
#include <QStyleFactory>
#include <QApplication>

RTInsteonController::RTInsteonController()
    : QMainWindow()
{
    QApplication::setStyle(QStyleFactory::create("windows"));

    layoutWindow();
    restoreWindowState();
    initStatusBar();

    m_client = new ControllerClient();

    connect(m_client, SIGNAL(clientConnected()), this, SLOT(clientConnected()));
    connect(m_client, SIGNAL(clientDisconnected()), this, SLOT(clientDisconnected()));
    connect(this, SIGNAL(clientRestart()), m_client, SLOT(clientRestart()));
    connect(m_client, SIGNAL(newUpdate(QJsonObject)), this, SLOT(newUpdate(QJsonObject)));
    connect(this, SIGNAL(setDeviceLevel(QJsonObject)), m_client, SLOT(setDeviceLevel(QJsonObject)));
    connect(this, SIGNAL(newSize(QSize)), m_table, SLOT(newSize(QSize)));

    m_client->resumeThread();

    setWindowTitle(RTAutomationArgs::getAppName());

    m_tapSeen = false;
}

void RTInsteonController::closeEvent(QCloseEvent *)
{
    if (m_client) {
        m_client->exitThread();
        m_client->thread()->wait(2000);
        m_client = NULL;
    }

    saveWindowState();
}

void RTInsteonController::resizeEvent(QResizeEvent *)
{
    emit newSize(size());
}


void RTInsteonController::clientConnected()
{
    QSettings settings;

    m_brokerStatus->setText("Connected to " + settings.value(RTAUTOMATION_PARAMS_BROKERADDRESS).toString());
    m_serverName->setText(QString("Using automation server: ") + settings.value(RTAUTOMATION_PARAMS_SERVERID).toString());
}

void RTInsteonController::clientDisconnected()
{
    m_brokerStatus->setText("Disconnected");
    m_serverName->setText("");
}

void RTInsteonController::newUpdate(QJsonObject update)
{
    if (!update.contains(INSTEON_DEVICE_JSON_UPDATELIST))
        return;

    QJsonArray jsa = update[INSTEON_DEVICE_JSON_UPDATELIST].toArray();
    for (int i = 0; i < jsa.count(); i++) {
        QJsonObject params = jsa[i].toObject();

        InsteonDevice idev;
        if (!idev.read(params))
            continue;

        if (!m_insteonMap.contains(idev.deviceID)) {
            int row = m_table->rowCount();
            m_table->insertRow(row);
            m_table->setRowHeight(row, MAINTABLE_ROW_HEIGHT);
            QLabel *label = new QLabel();
            label->setAttribute(Qt::WA_TransparentForMouseEvents, true);
            label->setIndent(20);
            m_table->setCellWidget(row, 0, label);

            QProgressBar *pb = new QProgressBar();
            pb->setMinimum(0);
            pb->setMaximum(255);
            pb->setAttribute(Qt::WA_TransparentForMouseEvents, true);
            m_table->setCellWidget(row, 1, pb);

            m_insteonMap.insert(idev.deviceID, idev);
            m_insteonGridMap.insert(idev.deviceID, row);
        } else {
            m_insteonMap.insert(idev.deviceID, idev);
        }

        int gridSlot = m_insteonGridMap.value(idev.deviceID);
        ((QLabel *)m_table->cellWidget(gridSlot, 0))->setText(idev.name);
        ((QProgressBar *)m_table->cellWidget(gridSlot, 1))->setValue(idev.currentLevel);
        m_table->update();
    }
}

void RTInsteonController::layoutWindow()
{
    QAction *action;

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    centralWidget->setContentsMargins(0, 0, 0, 0);

    m_toolBar = new QToolBar(this);
    addToolBar(Qt::TopToolBarArea, m_toolBar);
    m_toolBar->setObjectName("Toolbar");
    m_toolBar->setMinimumHeight(50);

    m_configure = new QAction("Configure", this);
    connect(m_configure, SIGNAL(triggered()), this, SLOT(onConfigure()));
    m_toolBar->addAction(m_configure);

    m_toolBar->addSeparator();

    action = new QAction("Exit", this);
    connect(action, SIGNAL(triggered()), this, SLOT(onExit()));
    m_toolBar->addAction(action);

    QVBoxLayout *vl = new QVBoxLayout();
    centralWidget->setLayout(vl);

    QHBoxLayout *hl = new QHBoxLayout();
    vl->addLayout(hl);

    m_table = new MainTable(this);
    m_table->setSelectionMode(QAbstractItemView::NoSelection);
    vl->addWidget(m_table);
    m_statusBar = new QStatusBar(this);
    setStatusBar(m_statusBar);

    connect(m_table, SIGNAL(cellClicked(int,int)), this, SLOT(cellClicked(int,int)));
    connect(m_table, SIGNAL(tapAndHold()), this, SLOT(tapAndHold()));
}

void RTInsteonController::tapAndHold()
{
    m_tapSeen = true;
}

void RTInsteonController::cellClicked(int row, int /* col */)
{
 #if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    if (!m_tapSeen)
        return;
#endif
    m_tapSeen = false;
    QString name = ((QLabel *)m_table->cellWidget(row, 0))->text();
    int level = ((QProgressBar *)m_table->cellWidget(row, 1))->value();
    SetInsteonLevelDlg dlg(this, name, level);

    if (dlg.exec() == QDialog::Accepted) {
        foreach(InsteonDevice idev, m_insteonMap) {
            if (idev.name == name) {
                QJsonArray jsa;
                QJsonObject jso;
                idev.newLevel = dlg.newLevel();
                idev.writeNewLevel(jso);
                jsa.append(jso);
                QJsonObject jsonNewLevel;
                jsonNewLevel[INSTEON_DEVICE_JSON_SETDEVICELEVEL] = jsa;
                emit setDeviceLevel(jsonNewLevel);
                return;
            }
        }
    }
}

void RTInsteonController::initStatusBar()
{
    m_brokerStatus = new QLabel(this);
    m_brokerStatus->setAlignment(Qt::AlignLeft);
    m_statusBar->addWidget(m_brokerStatus, 1);
    m_serverName = new QLabel(this);
    m_serverName->setAlignment(Qt::AlignRight);
    m_statusBar->addWidget(m_serverName, 1);
}

void RTInsteonController::saveWindowState()
{
    QSettings settings;

    settings.beginGroup("Window");
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("State", saveState());
    settings.endGroup();

}

void RTInsteonController::restoreWindowState()
{
    QSettings settings;

    settings.beginGroup("Window");
    restoreGeometry(settings.value("Geometry").toByteArray());
    restoreState(settings.value("State").toByteArray());

    settings.endGroup();

}

void RTInsteonController::onExit()
{
    emit close();
}

void RTInsteonController::onConfigure()
{
    ConfigureDlg dlg(this);

    if (dlg.exec() == QDialog::Accepted) {
        emit clientRestart();
        while (m_table->rowCount() > 0)
            m_table->removeRow(0);
        m_insteonGridMap.clear();
        m_insteonMap.clear();
    }
}
