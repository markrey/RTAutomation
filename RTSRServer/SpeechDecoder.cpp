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
#define SPEECH_TTS_MIN                  5000                // 5 seconds wait TTS start
#define SPEECH_TTS_MAX                  60000               // 60 seconds wait maximum for TTS if started

//  speech decode states

#define WAITING_FOR_HELLO               0                   // waiting for wakeup phrase
#define WAITING_FOR_INPUT               1                   // waiting for some speech to be decoded
#define WAITING_FOR_DECODE              2                   // waiting for decode from api.ai
#define WAITING_FOR_TTS                 3                   // waiting for tts to finish

SpeechDecoder::SpeechDecoder() : RTAutomationThread()
{
    m_ps = NULL;

    m_helloCommand = "HELLO ARTY";                          // the hello phrase
    m_goodbyeCommand = "GOODBYE ARTY";                      // the goodbye [hrase

    m_timerID = -1;
    m_curlProcess = NULL;
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

    QSettings settings;

    settings.beginGroup(SPEECH_DECODER_GROUP);
    m_decoderKey = settings.value(SPEECH_DECODER_KEY).toString();
    m_decoderToken = settings.value(SPEECH_DECODER_TOKEN).toString();
    settings.endGroup();

    m_state = WAITING_FOR_HELLO;

    m_curlProcess = new QProcess(this);
    connect(m_curlProcess, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(decoderError(QProcess::ProcessError)));
    connect(m_curlProcess, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(decoderFinished(int,QProcess::ExitStatus)));
    connect(m_curlProcess, SIGNAL(readyReadStandardError()),
            this, SLOT(decoderReadyReadStandardError()));
    connect(m_curlProcess, SIGNAL(readyReadStandardOutput()),
            this, SLOT(decoderReadyReadStandardOutput()));

    m_silenceStart = 1000;
    m_active = false;
    m_outstandingSamples = 0;

    m_timerID = startTimer(100);
}

void SpeechDecoder::stopModule()
{
    if (m_timerID != -1) {
        killTimer(m_timerID);
        m_timerID = -1;
    }

    if (m_ps != NULL) {
        ps_free(m_ps);
        m_ps = NULL;
    }

    if (m_curlProcess != NULL) {
        m_curlProcess->deleteLater();
        m_curlProcess = NULL;
    }
}

void SpeechDecoder::timerEvent(QTimerEvent *)
{
    switch (m_state) {
        case WAITING_FOR_HELLO:
            break;

        case WAITING_FOR_INPUT:
            break;

        case WAITING_FOR_DECODE:
            break;

        case WAITING_FOR_TTS:
            if ((QDateTime::currentMSecsSinceEpoch() > m_ttsTimer)) {
                m_state = m_nextState;
                qDebug() << "timeout not tts";
            }
            break;

    }
}

void SpeechDecoder::ttsComplete(QString /* topic */, QJsonObject json)
{
    if (m_state != WAITING_FOR_TTS)
        return;

    if (json.value("complete").toBool()) {
        qDebug() << "tts complete";
        m_state = m_nextState;
        return;
    }

    // started but not complete - extend timer

    m_ttsTimer = QDateTime::currentMSecsSinceEpoch() + SPEECH_TTS_MAX;
    qDebug() << "tts starting";
}

void SpeechDecoder::newAudio(QString topic, QJsonObject audio)
{
    char const *hyp;
    qint32 bestScore;

    if ((m_state == WAITING_FOR_DECODE) || (m_state == WAITING_FOR_TTS))
        return;                                             // don't process while waiting for response

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
        m_accumulatedAudio += audioData;
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

        if ((m_decoderToken != "") && (m_decoderKey != "") &&
                (m_state == WAITING_FOR_INPUT) && (m_accumulatedAudio.length() > 0)) {
            QStringList args;
            args << "-k"
                 << "-F"
                 << "request={'timezone':'America/New_York', 'lang':'en'};type=application/json"
                 << "-F"
                 << "voiceData=@audio.wav;type=audio/wav"
                 << "-H"
                 << "Authorization: Bearer " + m_decoderToken
                 << "-H"
                 << "ocp-apim-subscription-key: " + m_decoderKey
                 << "https://api.api.ai/v1/query?v=20150910";

            QFile file("audio.wav");
            file.open(QIODevice::WriteOnly);
            file.write(m_accumulatedAudio);
            file.close();
            m_accumulatedAudio.clear();

            m_curlProcess->start("curl", args);
            m_state = WAITING_FOR_DECODE;
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
    switch (m_state) {
        case WAITING_FOR_HELLO:
            if (input == m_helloCommand) {
                m_state = WAITING_FOR_INPUT;
                sendDecodedSpeech(input, "I am at your command", WAITING_FOR_INPUT);
                m_accumulatedAudio.clear();
            }
            break;

        case WAITING_FOR_INPUT:
            if (input == m_goodbyeCommand) {
                m_state = WAITING_FOR_HELLO;
                sendDecodedSpeech(input, "enjoy your day", WAITING_FOR_HELLO);
            }
            break;
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

void SpeechDecoder::sendDecodedSpeech(const QString &text, const QString &say, int nextState)
{
    if ((text.length() == 0) && (say.length() == 0))
        return;

    QJsonObject jso;
    jso[RTMQTTJSON_DECODEDSPEECH_TEXT] = text;
    jso[RTMQTTJSON_DECODEDSPEECH_SAY] = say;
    jso[RTMQTTJSON_TIMESTAMP] = (double)QDateTime::currentMSecsSinceEpoch() / 1000.0;
    emit decodedSpeech(jso);
    m_state = WAITING_FOR_TTS;
    m_nextState = nextState;

    m_ttsTimer = QDateTime::currentMSecsSinceEpoch() + SPEECH_TTS_MIN;
}

//  decode process signal handlers

void SpeechDecoder::decoderError(QProcess::ProcessError error)
{
    qDebug() << "process error " << error;
    if (m_state == WAITING_FOR_DECODE)
        m_state = WAITING_FOR_INPUT;
}

void SpeechDecoder::decoderFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "process finished " << exitCode << ", " << exitStatus;
    if (m_state == WAITING_FOR_DECODE)
        m_state = WAITING_FOR_INPUT;
}

void SpeechDecoder::decoderReadyReadStandardError()
{
//    qDebug() << "stderr: " << m_curlProcess->readAllStandardError();
}

void SpeechDecoder::decoderReadyReadStandardOutput()
{
    QString output = m_curlProcess->readAllStandardOutput();

    // find JSON part
    int index = output.indexOf('{');
    if (index == -1)
        return;
    output.remove(0, index);

    QJsonDocument var(QJsonDocument::fromJson(output.toLatin1()));
    QJsonObject json = var.object();

    if(!json.contains("result"))
        return;

    QJsonObject result = json.value("result").toObject();

    if(!result.contains("fulfillment"))
        return;

    QJsonObject fulfillment = result.value("fulfillment").toObject();

    if (fulfillment.contains("speech")) {
        QString tts = fulfillment.value("speech").toString();
        sendDecodedSpeech(tts, tts, WAITING_FOR_INPUT);
    }

    qDebug() << var.toJson();
}
