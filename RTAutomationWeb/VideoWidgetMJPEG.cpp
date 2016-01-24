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

#include "VideoWidgetMJPEG.h"
#include "VideoResourceMJPEG.h"
#include <qdebug.h>

VideoWidgetMJPEG::VideoWidgetMJPEG(WContainerWidget *parent) : WImage(parent)
{
    m_videoResource = new VideoResourceMJPEG(this);
    setImageLink(m_videoResource);
    m_displayWidth = 0;
    m_displayHeight = 0;
    m_frameWidth = 0;
    m_frameHeight = 0;
 }

VideoWidgetMJPEG::~VideoWidgetMJPEG()
{
    qDebug() << "~VideoWidgetMJPEG";
}

void VideoWidgetMJPEG::makeActive()
{
    this->show();
    m_videoResource->makeActive();
}

void VideoWidgetMJPEG::makeInactive()
{
    this->hide();
    m_videoResource->makeInactive();
}

void VideoWidgetMJPEG::newFrame(const QByteArray& bFrame, int frameWidth, int frameHeight)
{
    if ((frameWidth != m_frameWidth) || (frameHeight != m_frameHeight)) {

        m_frameWidth = frameWidth;
        m_frameHeight = frameHeight;

        doResize();
    }
    m_videoResource->setFrame(bFrame);
    m_videoResource->setChanged();
}

void VideoWidgetMJPEG::processJpegData(const QByteArray& jpeg)
{
    m_videoResource->processJpegData(jpeg);
/*
    if ((m_frameWidth != m_videoResource->m_frameWidth) || (m_frameHeight != m_videoResource->m_frameHeight)) {

        m_frameWidth = m_videoResource->m_frameWidth;
        m_frameHeight = m_videoResource->m_frameHeight;

        doResize();
    }*/
    m_videoResource->setChanged();
}


void VideoWidgetMJPEG::setDisplaySize(int displayWidth, int displayHeight)
{
    m_displayWidth = displayWidth;
    m_displayHeight = displayHeight;
    doResize();
}

void VideoWidgetMJPEG::doResize()
{
    float hScale, vScale;

    hScale = (float)m_displayWidth / m_frameWidth;
    vScale = (float)m_displayHeight / m_frameHeight;

    if (vScale < hScale) {
        resize((int)((float)m_frameWidth * vScale), (int)((float)m_frameHeight * vScale));
    } else {
        resize((int)((float)m_frameWidth * hScale), (int)((float)m_frameHeight) * hScale);
    }
}
