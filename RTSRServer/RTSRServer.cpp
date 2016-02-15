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

#include "RTSRServer.h"
#include "RTSRServerClient.h"
#include "TopicConfig.h"
#include "ApiaiConfig.h"
#include "SpeechDecoder.h"

#include "CommandService.h"

#include <stdio.h>

RTSRServer::RTSRServer(QObject *parent, bool daemonMode) : MainConsole(parent, daemonMode)
{
    m_client = new RTSRServerClient(this);
    m_decoder = new SpeechDecoder();

    connect(m_client, SIGNAL(receiveCommandData(QJsonObject)),
            m_commandService, SLOT(receiveCommandData(QJsonObject)));
    connect(m_commandService, SIGNAL(sendCommandData(QJsonObject)),
            m_client, SLOT(sendCommandData(QJsonObject)));

    connect(m_decoder, SIGNAL(decodedSpeech(QJsonObject)), m_client, SLOT(decodedSpeech(QJsonObject)));
    connect(m_client, SIGNAL(newAudio(QString,QJsonObject)), m_decoder, SLOT(newAudio(QString, QJsonObject)));
    connect(m_client, SIGNAL(ttsComplete(QString,QJsonObject)), m_decoder, SLOT(ttsComplete(QString, QJsonObject)));

    addStandardDialogs();

    TopicConfig *topicConfig = new TopicConfig();
    RTAutomationJSON::addConfigDialog(topicConfig);

    ApiaiConfig *apiaiConfig = new ApiaiConfig();
    RTAutomationJSON::addConfigDialog(apiaiConfig);

    m_client->resumeThread();
    m_decoder->resumeThread();

    startServices();
}

void RTSRServer::showHelp()
{
    printf("\nOptions are:\n\n");
    printf("  H - Show help\n");
    printf("  X - Exit\n");
}

void RTSRServer::processInput(char c)
{
    switch (c) {
    case 'H':
        showHelp();
        break;
    }

    printf("\n");
}
