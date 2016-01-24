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

#ifndef _HOMEPAGEWIDGET_H_
#define _HOMEPAGEWIDGET_H_

#include <Wt/WContainerWidget>
#include <Wt/WTime>
#include <Wt/WVBoxLayout>
#include <Wt/WHBoxLayout>

#include "WebWidget.h"
#include <qbytearray.h>
#include <qmap.h>

using namespace Wt;

class Webicon;

#define WEBICON_DEFAULT_WIDTH   640
#define WEBICON_DEFAULT_HEIGHT  480

class HomePageWidget : public WebWidget
{
public:
    HomePageWidget(WebServer *webServer);
    virtual ~HomePageWidget();

    bool newFrame(const QString& deviceName, const QByteArray& bFrame, int width, int height);
    void makeActive();
    void makeInactive();
    void updateSensorData();

protected:
    void layoutWidget();
    virtual void layoutSizeChanged(int width, int height);
    QMap <QString, Webicon *> m_webicons;

    void handleMouseClicked(const char *deviceName);

    WTime m_lastResizeTime;
    WVBoxLayout *m_vLayout;
    int m_layoutWidth;                                      // width of the layout as last reported
};

#endif // _HOMEPAGEWIDGET_H_
