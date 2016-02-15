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

#ifndef _RTSRSERVER_H
#define _RTSRSERVER_H

#include "MainConsole.h"

class RTSRServerClient;

//  RTSERSERVER JSON defs

#define RTSRSERVER_TOPICCONFIG_NAME     "topicConfig"
#define RTSRSERVER_TOPICCONFIG_DESC     "Configure MQTT topics"

#define RTSRSERVER_APIAICONFIG_NAME     "apiaiConfig"
#define RTSRSERVER_APIAICONFIG_DESC     "Configure api.ai information"

//  Settings keys

#define RTSRSERVER_PARAMS_TOPIC_GROUP   "RTSRServerTopicGroup"
#define RTSRSERVER_PARAMS_AUDIO_TOPIC   "audio"
#define RTSRSERVER_PARAMS_DECODEDSPEECH_TOPIC   "decodedSpeech"
#define RTSRSERVER_PARAMS_TTSCOMPLETE_TOPIC     "ttsComplete"

class SpeechDecoder;

class RTSRServer : public MainConsole
{
    Q_OBJECT

public:
    RTSRServer(QObject *parent, bool daemonMode);

protected:
    void processInput(char c);

private:
    void showHelp();
    void showStatus();

    RTSRServerClient *m_client;
    SpeechDecoder *m_decoder;
};

#endif // _RTSRSERVER_H
