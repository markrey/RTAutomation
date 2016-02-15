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
#include <qlabel.h>
#include <qsignalmapper.h>
#include <qdebug.h>
#include <QCloseEvent>

#include "DialogMenu.h"
#include "GenericDialog.h"
#include "UpdateDialog.h"
#include "RTAutomationLog.h"

#define TAG "DialogMenu"

DialogMenu::DialogMenu(const QString& app, QWidget *parent)
    : QDialog(parent, Qt::WindowCloseButtonHint | Qt::WindowTitleHint)
{
    m_app = app;
    layoutWindow();
    m_timer = startTimer(200);                             // initial delay
    m_tickCount = 0;
    m_menuMode = true;
}

DialogMenu::~DialogMenu()
{
    if (m_timer != -1)
        killTimer(m_timer);
    m_timer = -1;
}

void DialogMenu::closeEvent(QCloseEvent *)
{
    if (m_timer != -1)
        killTimer(m_timer);
    m_timer = -1;
    accept();
}

void DialogMenu::timerEvent(QTimerEvent * /* event */)
{
    if (m_tickCount == 0) {
        killTimer(m_timer);
        m_timer = startTimer(100);                          // this is the correct timer

        RTAutomationJSON sj;
        emit sendAppData(sj.completeRequest(RTAUTOMATIONJSON_RECORD_TYPE_DIALOGMENU, RTAUTOMATIONJSON_COMMAND_REQUEST, QJsonObject()));
    }
    m_bar->setValue(m_tickCount++);
    if (m_tickCount == DIALOGMENU_WAIT_TICKS) {
        killTimer(m_timer);
        m_timer = -1;
        QMessageBox::warning(this, "Get management menu From app",
                         QString("The attempt to get the management menu from ") + m_app + " timed out.",
                                 QMessageBox::Ok);
        reject();
    }
}

void DialogMenu::receiveAppData(QJsonObject json)
{
    if (!m_menuMode) {
        emit sendDialogData(json);
        return;
    }
    if (!json.contains(RTAUTOMATIONJSON_RECORD_TYPE))
        return;
    if (json[RTAUTOMATIONJSON_RECORD_TYPE].toString() != RTAUTOMATIONJSON_RECORD_TYPE_DIALOGMENU)
        return;
    if (!json.contains(RTAUTOMATIONJSON_RECORD_COMMAND))
        return;
    if (json[RTAUTOMATIONJSON_RECORD_COMMAND].toString() != RTAUTOMATIONJSON_COMMAND_RESPONSE)
        return;

    if (!json.contains(RTAUTOMATIONJSON_RECORD_DATA))
        return;
    QJsonObject jo = json.value(RTAUTOMATIONJSON_RECORD_DATA).toObject();
    if (!jo.contains(RTAUTOMATIONJSON_DIALOGMENU_CONFIG) || !jo[RTAUTOMATIONJSON_DIALOGMENU_CONFIG].isArray()) {
        RTAutomationLog::logError(TAG, "Dialog menu did not contain valid config section");
        return;
    }
    if (!jo.contains(RTAUTOMATIONJSON_DIALOGMENU_INFO) || !jo[RTAUTOMATIONJSON_DIALOGMENU_INFO].isArray()) {
        RTAutomationLog::logError(TAG, "Dialog menu did not contain valid info section");
        return;
    }
    m_configMenuArray = jo[RTAUTOMATIONJSON_DIALOGMENU_CONFIG].toArray();
    m_infoMenuArray = jo[RTAUTOMATIONJSON_DIALOGMENU_INFO].toArray();
    killTimer(m_timer);
    m_timer = -1;
    relayoutWindow();
}


void DialogMenu::layoutWindow()
{
    setModal(true);

    setWindowTitle(m_app);
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setSpacing(20);
    vLayout->setContentsMargins(11, 11, 11, 11);
    m_label = new QLabel(QString("Requesting management menu from ") + m_app);
    vLayout->addWidget(m_label, Qt::AlignCenter);

    vLayout->addSpacing(20);

    m_bar = new QProgressBar();
    m_bar->setRange(0, DIALOGMENU_WAIT_TICKS);
    vLayout->addWidget(m_bar, Qt::AlignCenter);

    setLayout(vLayout);
}

void DialogMenu::relayoutWindow()
{
    QLabel *label;

    delete(layout());
    delete(m_bar);
    m_bar = NULL;
    delete m_label;
    m_label = NULL;

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(11, 11, 11, 11);

    QVBoxLayout *configLayout = new QVBoxLayout();
    configLayout->setSpacing(20);
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(20);

    QSignalMapper *configMapper = new QSignalMapper(this);
    QSignalMapper *infoMapper = new QSignalMapper(this);

    //  Add config entries

    label = new QLabel("Configuration options:");
    configLayout->addWidget(label);

    for (int i = 0; i < m_configMenuArray.count(); i++) {
        QPushButton *button = new QPushButton(m_configMenuArray.at(i).toObject().value(RTAUTOMATIONJSON_DIALOGMENU_DESC).toString());
        connect(button, SIGNAL(clicked()), configMapper, SLOT(map()));
        configMapper->setMapping(button, i);
        configLayout->addWidget(button);
    }
    connect(configMapper, SIGNAL(mapped(int)), this, SLOT(configDialogSelect(int)));

    configLayout->addStretch(1);

    //  Add info entries

    label = new QLabel("Information options:");
    infoLayout->addWidget(label);

    for (int i = 0; i < m_infoMenuArray.count(); i++) {
        QPushButton *button = new QPushButton(m_infoMenuArray.at(i).toObject().value(RTAUTOMATIONJSON_DIALOGMENU_DESC).toString());
        connect(button, SIGNAL(clicked()), infoMapper, SLOT(map()));
        infoMapper->setMapping(button, i);
        infoLayout->addWidget(button);
    }
    connect(infoMapper, SIGNAL(mapped(int)), this, SLOT(infoDialogSelect(int)));
    infoLayout->addStretch(1);

    hLayout->addLayout(configLayout);
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setMinimumWidth(20);
    hLayout->addWidget(line);

    hLayout->addLayout(infoLayout);
    setLayout(hLayout);
}

void DialogMenu::configDialogSelect(int dialogIndex)
{
    int ret;

    if (dialogIndex >= m_configMenuArray.count())
        return;
    QString dialogName = m_configMenuArray[dialogIndex].toObject().value(RTAUTOMATIONJSON_DIALOGMENU_NAME).toString();
    QString dialogDesc = m_configMenuArray[dialogIndex].toObject().value(RTAUTOMATIONJSON_DIALOGMENU_DESC).toString();

    GenericDialog dlg(m_app, dialogName, dialogDesc, 250, QJsonObject(), this);

    m_menuMode = false;
    connect(this, SIGNAL(sendDialogData(QJsonObject)), &dlg, SLOT(receiveDialogData(QJsonObject)));
    connect(&dlg, SIGNAL(sendDialogData(QJsonObject)), this, SLOT(receiveDialogData(QJsonObject)));

    ret = dlg.exec();

    disconnect(this, SIGNAL(sendDialogData(QJsonObject)), &dlg, SLOT(receiveDialogData(QJsonObject)));
    disconnect(&dlg, SIGNAL(sendDialogData(QJsonObject)), this, SLOT(receiveDialogData(QJsonObject)));

    if (ret == QDialog::Accepted) {
        RTAutomationJSON sj;
        QJsonObject param;
        param[RTAUTOMATIONJSON_PARAM_DIALOG_NAME] = dialogName;
        sj.addVar(dlg.getUpdatedDialog(), RTAUTOMATIONJSON_RECORD_DATA);
        UpdateDialog updateDlg(m_app, dialogName, this);
        connect(this, SIGNAL(sendDialogData(QJsonObject)), &updateDlg, SLOT(receiveDialogData(QJsonObject)));
        emit sendAppData(sj.completeRequest(RTAUTOMATIONJSON_RECORD_TYPE_DIALOGUPDATE, RTAUTOMATIONJSON_COMMAND_REQUEST, param));
        updateDlg.exec();
        disconnect(this, SIGNAL(sendDialogData(QJsonObject)), &updateDlg, SLOT(receiveDialogData(QJsonObject)));
    }

    m_menuMode = true;

}


void DialogMenu::infoDialogSelect(int dialogIndex)
{
    if (dialogIndex >= m_infoMenuArray.count())
        return;
    QString dialogName = m_infoMenuArray[dialogIndex].toObject().value(RTAUTOMATIONJSON_DIALOGMENU_NAME).toString();
    QString dialogDesc = m_infoMenuArray[dialogIndex].toObject().value(RTAUTOMATIONJSON_DIALOGMENU_DESC).toString();

    GenericDialog dlg(m_app, dialogName, dialogDesc, 250, QJsonObject(), this);

    m_menuMode = false;
    connect(this, SIGNAL(sendDialogData(QJsonObject)), &dlg, SLOT(receiveDialogData(QJsonObject)));
    connect(&dlg, SIGNAL(sendDialogData(QJsonObject)), this, SLOT(receiveDialogData(QJsonObject)));

    dlg.exec();

    disconnect(this, SIGNAL(sendDialogData(QJsonObject)), &dlg, SLOT(receiveDialogData(QJsonObject)));
    disconnect(&dlg, SIGNAL(sendDialogData(QJsonObject)), this, SLOT(receiveDialogData(QJsonObject)));

    m_menuMode = true;

}

void DialogMenu::receiveDialogData(QJsonObject json)
{
    emit sendAppData(json);
}

