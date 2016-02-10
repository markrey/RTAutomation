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

#include "RTAutomationLog.h"
#include "RTMQTTJSON.h"
#include "RTSRServer.h"

#include "SpeechDecoder.h"

#include <qdatetime.h>
#include <qdebug.h>
#include <qsettings.h>
#include <qdir.h>

#include <stdint.h>


#define TAG "SpeedDecoder"

#define SPEECH_SILENCE                  2048                // max value defined as silence
#define SPEECH_GAP                      500                 // this much silence is a gap
#define SPEECH_MAX_OUTSTANDING_SAMPLES  (16000 * 256)       // max samples before we force recognition

SpeechDecoder::SpeechDecoder() : RTAutomationThread()
{
    m_ps = NULL;

    m_commands  << "HELLO ARTY"                             // the wakeup command (must be first entry)
                << "GOODBYE ARTY";                          // cancel current activity (must be second entry)
}

void SpeechDecoder::initModule()
{
    cmd_ln_t *config;

    QString sphinxDir = "/usr/local/share/pocketsphinx/model/en-us/";
    QString phrasesDir = QDir::homePath() + "/Phrases/";

    config = cmd_ln_init(NULL, ps_args(), TRUE,
                 "-hmm",qPrintable(sphinxDir + "en-us"),
                 "-lm", qPrintable(phrasesDir + "0305.lm"),
                 "-dict", qPrintable(phrasesDir + "0305.dic"),
                 NULL);
    if (config == NULL) {
        RTAutomationLog::logError(TAG, "Failed to load speech recognizer config");
        return;
    }
    m_ps = ps_init(config);
    if (m_ps == NULL) {
        RTAutomationLog::logError(TAG, "Failed to generate speech decoder");
        return;
    }

    if (ps_start_utt(m_ps) < 0) {
        RTAutomationLog::logError(TAG, "Speech decoder failed to start");
        ps_free(m_ps);
        m_ps = NULL;
        return;
    }
    m_silenceStart = 1000;
    m_active = false;
    m_commandActive = false;
    m_outstandingSamples = 0;
}

void SpeechDecoder::stopModule()
{
    if (m_ps != NULL) {
        ps_free(m_ps);
        m_ps = NULL;
    }
}

void SpeechDecoder::newAudio(QString topic, QJsonObject audio)
{
    char const *hyp;
    qint32 bestScore;

    if (m_ps == NULL)
        return;

    //  get the actual audio data

    if (!audio.contains(RTMQTTJSON_AUDIO_DATA)) {
        RTAutomationLog::logError(TAG, QString("No audio data in message from ") + topic);
        return;
    }

    if (!audio.contains(RTMQTTJSON_AUDIO_CHANNELS)) {
        RTAutomationLog::logError(TAG, QString("No audio channels in message from ") + topic);
        return;
    }

    if (!audio.contains(RTMQTTJSON_AUDIO_RATE)) {
        RTAutomationLog::logError(TAG, QString("No audio rate in message from ") + topic);
        return;
    }

    int rate = audio.value(RTMQTTJSON_AUDIO_RATE).toInt();
    QByteArray origData = QByteArray::fromBase64(audio[RTMQTTJSON_AUDIO_DATA].toString().toLatin1());
    int channels = audio.value(RTMQTTJSON_AUDIO_CHANNELS).toInt();

    if (rate != 16000) {
        RTAutomationLog::logError(TAG, QString("Unsupported audio rate %1 from ").arg(rate) + topic);
        return;
    }

    QByteArray audioData = reformatAudio(origData, channels, rate);

    int sampleCount = audioData.count() / 2;

    silenceDetector(audioData);
    if (m_active) {
        try {
            if (ps_process_raw(m_ps, (const int16 *)audioData.constData(), sampleCount, FALSE, FALSE) < 0) {
                RTAutomationLog::logError(TAG, "Speech decoder generated an error");
                return;
            }
        } catch (int e) {
            RTAutomationLog::logError(TAG, QString("ps_process_raw failed %1").arg(e));
            ps_free(m_ps);
            m_ps = NULL;
            return;
        }
        m_outstandingSamples += sampleCount;
    }
    if (m_active && (((QDateTime::currentMSecsSinceEpoch() - m_silenceStart) >= SPEECH_GAP) ||
                    (m_outstandingSamples >= SPEECH_MAX_OUTSTANDING_SAMPLES))) {
        qDebug() << "Processing samples " << m_outstandingSamples;
        m_active = false;
        m_outstandingSamples = false;
        try {
            ps_end_utt(m_ps);
        } catch (int e) {
            RTAutomationLog::logError(TAG, QString("ps_end_utt failed %1").arg(e));
            ps_free(m_ps);
            m_ps = NULL;
            return;
        }
        try {
            hyp = ps_get_hyp(m_ps, &bestScore);
        } catch (int e) {
            RTAutomationLog::logError(TAG, QString("ps_get_hyp failed %1").arg(e));
            ps_free(m_ps);
            m_ps = NULL;
            return;
        }
        processInput(QString(hyp));

        try {
            if (ps_start_utt(m_ps) < 0) {
                RTAutomationLog::logError(TAG, "Speech decoder failed to start");
                ps_free(m_ps);
                m_ps = NULL;
                return;
            }
        } catch (int e) {
            RTAutomationLog::logError(TAG, QString("ps_start_utt failed %1").arg(e));
            ps_free(m_ps);
            m_ps = NULL;
            return;
        }
    }
}
QByteArray SpeechDecoder::reformatAudio(const QByteArray origData, int channels, int /* rate */)
{
    if (channels != 2)
        return origData;

    QByteArray newData;
    int val1, val2;
    int res;
    int loopCount = origData.count() / 4;

    for (int i = 0, index = 0; i < loopCount; i++, index += 4) {
        val1 = (int)((int16_t)(((uint16_t)origData[index]) + (((uint16_t)origData[index+1]) << 8)));
        val2 = (int)((int16_t)(((uint16_t)origData[index+2]) + (((uint16_t)origData[index+3]) << 8)));
        res = (val1 + val2) / 2;

        newData.append(res & 0xff);
        newData.append((res >> 8) & 0xff);
    }
    return newData;
}

void SpeechDecoder::processInput(const QString& input)
{
    QString message;

    if (!m_commandActive && (input == m_commands.at(0))) {
        m_commandActive = true;
        sendDecodedSpeech(input, "I am at your command");
        return;
    }
    if (m_commandActive && (input == m_commands.at(1))) {
        m_commandActive = false;
        sendDecodedSpeech(input, "enjoy your day");
        return;
    }
}

bool SpeechDecoder::silenceDetector(const QByteArray& audioData)
{
    unsigned char *ptr = (unsigned char *)audioData.data();
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    bool allSilence = true;

    for (int i = 0; i < audioData.length(); i += 2, ptr += 2) {
        short val = (short)((unsigned short)(*ptr) + (((unsigned short)(*(ptr + 1))) << 8));
        if (abs(val) > SPEECH_SILENCE) {
            m_active = true;
            m_silenceStart = now;
            allSilence = false;
        }
    }
    return allSilence;
}

void SpeechDecoder::sendDecodedSpeech(const QString &text, const QString &say)
{
    if ((text.length() == 0) && (say.length() == 0))
        return;

    QJsonObject jso;
    jso[RTMQTTJSON_DECODEDSPEECH_TEXT] = text;
    jso[RTMQTTJSON_DECODEDSPEECH_SAY] = say;
    jso[RTMQTTJSON_TIMESTAMP] = (double)QDateTime::currentMSecsSinceEpoch() / 1000.0;
    emit decodedSpeech(jso);
}
