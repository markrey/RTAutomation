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
//
//  Based on example from:

/*
 * librdkafka - Apache Kafka C library
 *
 * Copyright (c) 2014, Magnus Edenhill
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * Apache Kafka consumer & producer example programs
 * using the Kafka driver from librdkafka
 * (https://github.com/edenhill/librdkafka)
 */

#include "KafkaConsumer.h"
#include "RTAutomationLog.h"
#include "DeviceDatabase.h"
#include "KafkaDatabase.h"

#include <ctype.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/time.h>
#include <errno.h>

#include <qsettings.h>
#include <qdebug.h>

#define TAG "KafkaConsumer"

#define BLOCK_SIZE  10000

KafkaConsumer::KafkaConsumer(QString deviceName, QString topic, int64_t startOffset)
{
    m_deviceName = deviceName;
    m_topic = topic;
    m_startOffset = startOffset;
    m_timerId = -1;
    m_messageBuffer = NULL;
    m_kd = g_kafkaDatabase->getKafkaPtr(deviceName);
}

void KafkaConsumer::initModule()
{
    consumerStart();
}

void KafkaConsumer::stopModule()
{
    consumerStop();
}

void KafkaConsumer::consumerStart()
{
    const char *debug = NULL;
    char errstr[512];
    rd_kafka_conf_t *conf;
    rd_kafka_topic_conf_t *topic_conf;
    char tmp[128];

    QSettings settings;

    settings.beginGroup(KAFKA_CONSUMER_GROUP);

    QString brokers = settings.value(KAFKA_CONSUMER_BROKERS).toString();

    settings.endGroup();

    /* Kafka configuration */
    conf = rd_kafka_conf_new();

    /* Quick termination */
    snprintf(tmp, sizeof(tmp), "%i", SIGIO);
    rd_kafka_conf_set(conf, "internal.termination.signal", tmp, NULL, 0);

    rd_kafka_conf_set(conf, "queued.min.messages", "1000000", NULL, 0);


    /* Kafka topic configuration */
    topic_conf = rd_kafka_topic_conf_new();

    /* Create Kafka handle */
    if (!(m_consumer = rd_kafka_new(RD_KAFKA_CONSUMER, conf,
                errstr, sizeof(errstr)))) {
        fprintf(stderr,
            "%% Failed to create Kafka consumer: %s\n",
            errstr);
        return;
    }

    if (debug)
        rd_kafka_set_log_level(m_consumer, 7);

    /* Add broker(s) */
    if (rd_kafka_brokers_add(m_consumer, qPrintable(brokers)) < 1) {
        fprintf(stderr, "%% No valid brokers specified\n");
        return;
    }

    /* Create topic to consume from */
    m_topicStruct = rd_kafka_topic_new(m_consumer, qPrintable(m_topic), topic_conf);

    m_messageBuffer = (rd_kafka_message_t **)malloc(sizeof(*m_messageBuffer) * BLOCK_SIZE);

    /* Start consuming */
    if (rd_kafka_consume_start(m_topicStruct, 0, m_startOffset) == -1){
        fprintf(stderr, "%% Failed to start consuming: %s\n",
            rd_kafka_err2str(rd_kafka_errno2err(errno)));
        return;
    }

    m_timerId = startTimer(100);
}

void KafkaConsumer::consumerStop()
{
    killTimer(m_timerId);

    rd_kafka_consume_stop(m_topicStruct, 0);

    rd_kafka_topic_destroy(m_topicStruct);

    free(m_messageBuffer);

    /* Destroy the handle */
    rd_kafka_destroy(m_consumer);

    m_consumer = NULL;
}

void KafkaConsumer::timerEvent(QTimerEvent *)
{
    int r = rd_kafka_consume_batch(m_topicStruct, 0,
                   500,
                   m_messageBuffer,
                   BLOCK_SIZE);
    if (r != -1) {
        for (int i = 0 ; i < r ; i++) {
            msgConsume(m_messageBuffer[i],NULL);
            rd_kafka_message_destroy(m_messageBuffer[i]);
        }
    }
}

/* Returns wall clock time in microseconds */
uint64_t wall_clock (void) {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return ((uint64_t)tv.tv_sec * 1000000LLU) +
        ((uint64_t)tv.tv_usec);
}

void KafkaConsumer::msgConsume(rd_kafka_message_t *rkmessage, void * /* opaque */)
{
    if (rkmessage->err) {
        if (rkmessage->err == RD_KAFKA_RESP_ERR__PARTITION_EOF) {
            g_kafkaDatabase->receivedEOF(m_kd);
            return;
        }

        printf("%% Consume error for topic \"%s\" [%"PRId32"] "
               "offset %"PRId64": %s\n",
               rd_kafka_topic_name(rkmessage->rkt),
               rkmessage->partition,
               rkmessage->offset,
               rd_kafka_message_errstr(rkmessage));
        fflush(stdout);

                if (rkmessage->err == RD_KAFKA_RESP_ERR__UNKNOWN_PARTITION ||
                    rkmessage->err == RD_KAFKA_RESP_ERR__UNKNOWN_TOPIC)

        return;
    }

    g_kafkaDatabase->newKafkaMessage(m_kd, rkmessage->offset,
                         QByteArray((char *)rkmessage->payload, rkmessage->len));

    m_messageOffset = rkmessage->offset;
    m_messageCount++;
    m_messageBytes += rkmessage->len;
}



