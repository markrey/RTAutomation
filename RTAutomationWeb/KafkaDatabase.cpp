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

#include "KafkaDatabase.h"
#include "RTAutomationLog.h"
#include "RTMQTTJSON.h"
#include "DeviceDatabase.h"

#include <qdebug.h>
#include <qfile.h>
#include <qsettings.h>
#include <qjsondocument.h>

#define TAG "KafkaDatabase"

KafkaDatabase *g_kafkaDatabase;

KafkaDatabase::KafkaDatabase()
{
    g_kafkaDatabase = this;
}

KafkaDatabase::~KafkaDatabase()
{
    qDebug() << "~KafkaDatabase";
    QMutexLocker lock(&m_lock);

    QMapIterator<QString, KafkaData *> i(m_kafkaData);
    while (i.hasNext()) {
        i.next();
        KafkaData *kd = i.value();
        for (int i = 0; i < kd->m_kafkaList.count(); i++)
            delete kd->m_kafkaList[i];
        delete kd;
    }

    m_kafkaData.clear();
}

KafkaData *KafkaDatabase::getKafkaPtr(const QString& deviceName)
{
    QMutexLocker lock(&m_lock);

    KafkaData *kd = m_kafkaData.value(deviceName);

    if (kd == NULL) {
        kd = new KafkaData();
        kd->m_deviceType = g_deviceDatabase->getDeviceType(deviceName);
        m_kafkaData[deviceName] = kd;
    }
    return kd;
}

void KafkaDatabase::newKafkaMessage(KafkaData *kd, int offset, const QByteArray &message)
{
    QMutexLocker lock(&m_lock);
    KafkaEntry *kafkaEntry;

    if (offset < kd->m_nextOffset)
        return;

    kd->m_nextOffset = offset + 1;

    QJsonDocument var(QJsonDocument::fromJson(message));
    QJsonObject json = var.object();

    if (!json.contains(RTMQTTJSON_TIMESTAMP)) {
        RTAutomationLog::logError(TAG, QString("Kafka message at offset %1 does not contain timestamp").arg(offset));
        return;
    }

    kafkaEntry = new KafkaEntry();
    kafkaEntry->m_timestamp = json.value(RTMQTTJSON_TIMESTAMP).toDouble();
    if (kd->m_kafkaList.count() > 0) {
        if (kd->m_kafkaList.last()->m_timestamp > kafkaEntry->m_timestamp) {
            RTAutomationLog::logError(TAG, QString("New timestamp %1 is older than current %2")
                              .arg(kafkaEntry->m_timestamp, 20, 'g', 20).arg(kd->m_kafkaList.last()->m_timestamp, 20, 'g', 20));
        }

        if ((qint64)kd->m_kafkaList.last()->m_timestamp != (qint64)kafkaEntry->m_timestamp) {
            // into new second - add index entry
            kd->m_kafkaIndex[(qint64)kafkaEntry->m_timestamp] = kd->m_kafkaList.count();
        }
    } else {
        // this is the first entry
        kd->m_kafkaIndex[(qint64)kafkaEntry->m_timestamp] = kd->m_kafkaList.count();
        kd->m_kafkaFirstTime = kafkaEntry->m_timestamp;
        kd->m_frameRate = json[RTMQTTJSON_VIDEO_RATE].toInt();
        kd->m_currentFilename = json.value("filename").toString();
        kd->m_currentFileOffset = 0;
    }

    if (kd->m_currentFilename != json.value("filename").toString()) {
        kd->m_currentFilename = json.value("filename").toString();
        kd->m_currentFileOffset = 0;
    }

    switch (kd->m_deviceType) {
    case DEVICE_TYPE_RTMQTT_VIDEO:
        kafkaEntry->m_filename = kd->m_currentFilename;
        kafkaEntry->m_fileOffset = kd->m_currentFileOffset;
        kafkaEntry->m_frameLength = json.value("length").toInt();
        kd->m_currentFileOffset += kafkaEntry->m_frameLength;
        break;

    case DEVICE_TYPE_RTMQTT_SENSOR:
        kafkaEntry->m_json = json;
        break;

    }
    kd->m_kafkaLastTime = kafkaEntry->m_timestamp;
    kafkaEntry->m_listIndex = kd->m_kafkaList.count();
    kd->m_kafkaList.append(kafkaEntry);
}

void KafkaDatabase::receivedEOF(KafkaData *kd)
{
    QMutexLocker lock(&m_lock);

    kd->m_receivedEOF = true;
}

bool KafkaDatabase::getKafkaTimeInterval(const KafkaData *kd, double &firstTime, double &lastTime)
{
    QMutexLocker lock(&m_lock);

    if (kd->m_kafkaList.count() == 0)
        return false;

    firstTime = kd->m_kafkaFirstTime;
    lastTime = kd->m_kafkaLastTime;
    return true;
}

KafkaEntry *KafkaDatabase::getKafkaEntryByTime(KafkaData *kd, double entryTime)
{
    QMutexLocker lock(&m_lock);

    if ((entryTime < kd->m_kafkaFirstTime) || (entryTime > kd->m_kafkaLastTime)) {
        RTAutomationLog::logError(TAG, QString("Out of range timestamp %1 (%2 - %3) for kafka entry request")
                                  .arg(entryTime).arg(kd->m_kafkaFirstTime).arg(kd->m_kafkaLastTime));
        return NULL;
    }

    int index = kd->m_kafkaIndex.value((qint64)entryTime);

    for (; index < kd->m_kafkaList.count(); index++) {
        if (kd->m_kafkaList.at(index)->m_timestamp == entryTime)
            return kd->m_kafkaList.at(index);
        if (kd->m_kafkaList.at(index)->m_timestamp > entryTime)
            return kd->m_kafkaList.at(index -1);
    }

    return kd->m_kafkaList.at(kd->m_kafkaList.count() - 1);
}

KafkaEntry *KafkaDatabase::getKafkaEntryByIndex(KafkaData *kd, int index)
{
    QMutexLocker lock(&m_lock);

    if ((index < 0) || (index >= kd->m_kafkaList.count())) {
        RTAutomationLog::logError(TAG, QString("Out of range index %1 (%2 - %3) for kafka entry request")
                                  .arg(index).arg(0).arg(kd->m_kafkaList.count()));
        return NULL;
    }

    return kd->m_kafkaList.at(index);
}

int KafkaDatabase::getNextKafkaOffset(const KafkaData *kd)
{
    QMutexLocker lock(&m_lock);

    return kd->m_nextOffset;
}

int KafkaDatabase::getRecordCount(const KafkaData *kd)
{
    QMutexLocker lock(&m_lock);

    return kd->m_kafkaList.count();
}
