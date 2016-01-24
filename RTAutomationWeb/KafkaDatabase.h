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

#ifndef _KAFKADATABASE_H
#define _KAFKADATABASE_H

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qmutex.h>
#include <qmap.h>
#include <qdatetime.h>
#include <qjsonobject.h>

class KafkaEntry
{
public:
    double m_timestamp;
    int m_listIndex;                                    // where it is in list

    // video stuff

    QString m_filename;
    qint64 m_fileOffset;                                // where it is in file
    int m_frameLength;

    // sensor stuff

    QJsonObject m_json;                                 // need the raw entry
};

class KafkaData
{
public:
    KafkaData() { m_nextOffset = 0; m_deviceType = 0; m_receivedEOF = false; m_frameRate = 10;}
    int m_deviceType;                                   // the type of the device
    QList<KafkaEntry *> m_kafkaList;                    // historic data from kafka
    QMap<qint64, int> m_kafkaIndex;                     // index into list at 1 second intervals
    int m_frameRate;                                    // video frame rate
    double m_kafkaFirstTime;                            // timestamp of first entry
    double m_kafkaLastTime;                             // timestamp of last entry
    int m_nextOffset;                                   // last received offset
    bool m_receivedEOF;                                 // if EOF has been seen (i.e. up to date)

    QString m_currentFilename;                          // curren filename it is saved into
    qint64 m_currentFileOffset;                         // current offset into that file
};

class KafkaDatabase : public QObject
{
public:
    KafkaDatabase();
    virtual ~KafkaDatabase();

    KafkaData *getKafkaPtr(const QString& deviceName);

    void newKafkaMessage(KafkaData *kd, int offset, const QByteArray& message);
    void receivedEOF(KafkaData *kd);

    bool getKafkaTimeInterval(const KafkaData *kd, double& firstTime, double& lastTime);
    KafkaEntry *getKafkaEntryByTime(KafkaData *kd, double entryTime);
    KafkaEntry *getKafkaEntryByIndex(KafkaData *kd, int index);
    int getNextKafkaOffset(const KafkaData *kd);
    int getRecordCount(const KafkaData *kd);

private:
    QMutex m_lock;

    QMap <QString, KafkaData *> m_kafkaData;
};

extern KafkaDatabase *g_kafkaDatabase;

#endif // _KAFKADATABASE_H
