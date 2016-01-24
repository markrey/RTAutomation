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

#ifndef _VIDEORESOURCEMJPEG_H_
#define _VIDEORESOURCEMJPEG_H_

#include <Wt/WContainerWidget>
#include <Wt/WResource>

#include <qbytearray.h>
#include <qmutex.h>

using namespace Wt;

class VideoResourceMJPEG : public WResource
{
public:
    VideoResourceMJPEG(Wt::WObject *parent = 0);
    ~VideoResourceMJPEG();

    void makeInactive();
    void makeActive();

    void setFrame(const QByteArray& bFrame);
    void processJpegData(const QByteArray& jpeg);

    int m_frameWidth;
    int m_frameHeight;

protected:
    void handleRequest(const Wt::Http::Request& request,
                       Wt::Http::Response& response);

private:
    QByteArray m_frame;

    QMutex m_videoLock;
};

#endif // _VIDEORESOURCEMJPEG_H_
