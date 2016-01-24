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

#include "VideoResourceMJPEG.h"

#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include "WebClient.h"

#include <qdebug.h>

VideoResourceMJPEG::VideoResourceMJPEG(Wt::WObject *parent) : WResource(parent)
{
}

VideoResourceMJPEG::~VideoResourceMJPEG()
{
    beingDeleted();
}

void VideoResourceMJPEG::makeActive()
{

}

void VideoResourceMJPEG::makeInactive()
{

}

void VideoResourceMJPEG::processJpegData(const QByteArray& jpeg)
{
    m_videoLock.lock();
    m_frame = jpeg;
    m_videoLock.unlock();
}



void VideoResourceMJPEG::handleRequest(const Wt::Http::Request& /*request*/,
                   Wt::Http::Response& response)
{
    response.setMimeType("image/jpeg");
    if (m_frame.length() > 0)
        response.out().write((const char *)m_frame.data(), m_frame.length());
}


void VideoResourceMJPEG::setFrame(const QByteArray& bFrame)
{
    if (bFrame.length() == 0)
        return;

    m_frame = bFrame;
}
