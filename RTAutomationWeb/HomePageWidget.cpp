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

#include <Wt/WContainerWidget>

#include "HomePageWidget.h"
#include "Webicon.h"
#include "WebClient.h"
#include "DeviceDatabase.h"

#include <qdebug.h>

HomePageWidget::HomePageWidget(WebServer *webServer) : WebWidget(NULL, webServer)
{
    layoutWidget();
    setLayoutSizeAware(true);
    m_layoutWidth = 0;
    m_lastResizeTime = WTime::currentServerTime();
    WApplication::instance()->setInternalPath(INTERNAL_PATH_MAIN,  true);
}

HomePageWidget::~HomePageWidget()
{
    m_webicons.clear();
    qDebug() << "~HomePageWidget";
}

void HomePageWidget::makeActive()
{
    QStringList list = g_deviceDatabase->getDeviceList();

    for (int i = 0; i < list.count(); i++) {
        Thumbnail thumb = g_deviceDatabase->getThumbnail(list.at(i));
        if (thumb.m_width != 0) {
            Webicon *icon = m_webicons.value(list.at(i));
            if (icon == NULL)
                continue;
            icon->newFrame(thumb.m_jpeg, thumb.m_width, thumb.m_height);
        }
    }

}

void HomePageWidget::makeInactive()
{
}

bool HomePageWidget::newFrame(const QString& deviceName, const QByteArray& bFrame, int width, int height)
{
    if (width == 0)
        return false;

    Webicon *icon = m_webicons.value(deviceName);

    if (icon == NULL)
        return false;

    icon->newFrame(bFrame, width, height);

    return true;
}

void HomePageWidget::updateSensorData()
{
    QMapIterator<QString, Webicon *> i(m_webicons);
    while (i.hasNext()) {
        i.next();
        i.value()->updateSensorData();
    }
}

void HomePageWidget::handleMouseClicked(const char *deviceName)
{
    m_webServer->setSingleDevice(deviceName);
}
\
void HomePageWidget::layoutWidget()
{
    int slot;
    WHBoxLayout *hLayout;

    setMargin(0, Left | Right);

    WVBoxLayout *m_vLayout = new WVBoxLayout(this);
    setLayout(m_vLayout);
    m_vLayout->setContentsMargins(0, 0, 0, 0);

    QStringList list = g_deviceDatabase->getDeviceList();

    for (slot = 0; slot < list.count(); slot++) {
        if ((slot & 1) == 0) {
            hLayout = new WHBoxLayout();
            hLayout->setContentsMargins(0, 0, 0, 0);
            m_vLayout->addLayout(hLayout);
        }
        Webicon *icon = new Webicon(list.at(slot), this);
        m_webicons[list.at(slot)] = icon;
        icon->clickedSignal().connect(this, &HomePageWidget::handleMouseClicked);
        icon->setDisplaySize(WEBICON_DEFAULT_WIDTH, WEBICON_DEFAULT_HEIGHT);
        icon->setName(qPrintable(list.at(slot)));

        hLayout->addWidget(icon, 0, AlignCenter | AlignTop);
    }
 }

void HomePageWidget::layoutSizeChanged(int width, int /*height*/)
{
    float hScale;
    int motWidth;
    int motHeight;

    if (m_layoutWidth == width)
        return;

    m_layoutWidth = width;

    hScale = (float)(width / 2) / (float)WEBICON_DEFAULT_WIDTH;

    motWidth = (int)((float)WEBICON_DEFAULT_WIDTH * hScale);
    motHeight = (int)((float)WEBICON_DEFAULT_HEIGHT * hScale);

    if (m_lastResizeTime.msecsTo(WTime::currentServerTime()) > 100) {
        m_lastResizeTime = WTime::currentServerTime();

        QMapIterator<QString, Webicon *> i(m_webicons);
        while (i.hasNext()) {
            i.next();
            i.value()->setDisplaySize(motWidth, motHeight);
        }
    }
    refresh();
}
