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
#ifndef _INSTEONDRIVER_H_
#define _INSTEONDRIVER_H_

#include <qmutex.h>
#include <qqueue.h>
#include "qextserialport.h"
#include <qdatetime.h>
#include <qjsonobject.h>

#include "InsteonDevice.h"
#include "InsteonTimer.h"
#include "InsteonDefs.h"

#include "RTInsteonThread.h"

//	Settings file keys

#define INSTEON_SETTINGS_PLM                "PLM"

#define INSTEON_SETTINGS_PLM_PORT           "PLMPort"
#define INSTEON_SETTINGS_PLM_LATITUDE       "PLMLatitude"
#define INSTEON_SETTINGS_PLM_LONGITUDE      "PLMLongitude"
#define	INSTEON_SETTINGS_PLM_ID				"PLMId"
#define	INSTEON_SETTINGS_DEVICE_CATEGORY	"PLMCategory"
#define	INSTEON_SETTINGS_DEVICE_SUBCATEGORY	"PLMSubCategory"
#define	INSTEON_SETTINGS_FIRMWARE_REV		"PLMFirmwareRev"

#define INSTEON_SETTINGS_DEVICE_DATABASE    "DeviceDatabase"
#define INSTEON_SETTINGS_DEVICE_ID          "DeviceID"
#define INSTEON_SETTINGS_DEVICE_NAME        "DeviceName"
#define INSTEON_SETTINGS_DEVICE_FLAGS       "DeviceFlags"
#define INSTEON_SETTINGS_DEVICE_GROUP       "DeviceGroup"
#define INSTEON_SETTINGS_DEVICE_CAT			"DeviceCat"
#define INSTEON_SETTINGS_DEVICE_SUBCAT		"DeviceSubcat"

#define INSTEON_SETTINGS_TIMERS             "Timers"
#define INSTEON_SETTINGS_TIMERS_NAME        "TimerName"
#define INSTEON_SETTINGS_TIMERS_TIMEHOUR    "TimerHour"
#define INSTEON_SETTINGS_TIMERS_TIMEMINUTE  "TimerMinute"
#define INSTEON_SETTINGS_TIMERS_DOW         "TimerDoW"
#define INSTEON_SETTINGS_TIMERS_RANDOM      "TimerRandom"
#define INSTEON_SETTINGS_TIMERS_MODE        "TimerMode"
#define INSTEON_SETTINGS_TIMERS_DELTATIME   "TimerDeltaTime"
#define INSTEON_SETTINGS_TIMERS_DEVICES     "TimerDevices"
#define INSTEON_SETTINGS_TIMERS_DEVICES_ID  "TimerDevicesID"
#define INSTEON_SETTINGS_TIMERS_DEVICES_LEVEL "TimerDevicesLevel"


#define INSTEONDRIVER_INTERVAL          (1000 / 10)

#define INSTEON_CONNECT_RETRY           (1000 * 2)

#define INSTEON_REQUEST_TIMEOUT         (1000 * 3)

#define INSTEON_POLL_INTERVAL           (1000 * 2)

#define INSTEON_TIMER_CHECK_INTERVAL    (1000 * 1)

#define INSTEON_RTINSTEON_UPDATE_INTERVAL     (1000 * 2)

//	state machine states

#define	INSTEON_STATE_IDLE              0                   // port closed
#define	INSTEON_STATE_WFCONNECT         1                   // waiting to connect to PLM
#define INSTEON_STATE_WFPLMINFO         2                   // waiting for PLM info
#define	INSTEON_STATE_WFDEVICES         3                   // waiting for device table
#define	INSTEON_STATE_ACTIVE            4                   // ready for normal operation


class InsteonDriver : public RTInsteonThread
{
    Q_OBJECT

public:
    InsteonDriver();
    const char *getState();
    void setTrace(bool state);
    bool getTrace() { return m_trace; }
    QStringList getDatabase();
    QStringList getTimers();
    const QString& getPortName();

    void addDevice(char *name, char *id, int flags, int group, int cat, int subCat);
    bool setDeviceLevelByIndex(int deviceIndex, unsigned char level);
    bool removeDeviceByIndex(int deviceIndex);

    bool addTimer(char *name);
    bool removeTimerByIndex(int timerIndex);
    bool addDeviceToTimerByIndex(int timerIndex, int deviceIndex, int level);
    const QString& getDeviceName(int deviceID);

    QList<InsteonDevice> getDeviceList();
    QList<InsteonTimer> getTimerList();

public slots:
    void addDevice(InsteonDevice device);
    void removeDevice(int deviceID);
    void setDeviceLevel(int deviceID, int level);

    void addTimer(InsteonTimer timer);
    void updateTimer(InsteonTimer timer);
    void removeTimer(QString name);

    void newPortName(QString portName);

signals:
    void PLMPortOpen();
    void PLMPortClosed();
    void newInsteonUpdate(QJsonObject updateList);

protected:
    void initModule();
    void timerEvent(QTimerEvent *event);
    void stopModule();

private slots:
    void onReadyRead();

private:
    void background();
    void reconnect();
    void disconnect();
    void processTimers();
    void calculateSunriseSunset();
    QTime convertJDToQTime(double JD);
    void displayJD(double JD);

    void sendQueuedRequest();
    void processResponses();
    void processStandardMessage(const QByteArray& message);
    void processALLLinkingCompleted(const QByteArray& message);
    bool decodePLMInfo(const QByteArray& message);
    InsteonDevice *decodeALLLinkRecord(const QByteArray& message);
    bool deviceChanged(InsteonDevice *a, InsteonDevice *b);

    unsigned int getDeviceID(const unsigned char *id);
    void setDeviceID(unsigned int intId, unsigned char *id);
    int findDeviceIndexInDatabase(const unsigned char *id);
    int findDeviceIndexInDatabase(unsigned int intId);
    bool getPollState(int deviceCat, int deviceSubcat);
    bool isBroadcastMessage(const QByteArray& message) {return (message.at(5) == 0) && (message.at(6) == 0);}

    int findTimerIndex(QString name);

    void queueLevelRequest(unsigned deviceID);
    void queueAddDevice(unsigned int deviceID);
    void queueRemoveDevice(unsigned int deviceID);

    bool sendSimpleCommand(char command);
    bool sendDirectMessage(unsigned int deviceID, unsigned char cmd1, unsigned char cmd2);
    bool sendAddDeviceMessage(unsigned int deviceID, bool add);

    void sendAlert(const InsteonDevice& device);
    void sendDelta(const InsteonDevice& device);

    QTime getTriggerTime(InsteonTimer timer);

    void displayMessage(const QByteArray& message, const char*prefix);

    void loadDatabase();
    void upsertDevice(InsteonDevice rec);
    void saveDatabase();
    void loadTimers();
    void saveTimers();

    QMutex m_lock;
    int m_timer;

    QString m_portName;                                     // the port name
    QextSerialPort *m_port;                                 // serial port driver
    bool m_portSwitch;                                      // tries alternate ports

    QQueue<QByteArray> m_responseQueue;                     // the responses from the PLM

    QByteArray m_partialResponse;                           // where a response is built
    int m_partialLeft;                                      // number of bytes left to complete message

    INSTEON_PLM	m_plm;                                      // PLM data
    QList<InsteonDevice> m_devices;                         // the device list
    QList<InsteonTimer> m_timers;                           // timer records

    QQueue<INSTEON_REQUEST> m_requestQueue;                 // queue of outstanding requests

    int m_state;
    qint64 m_lastConnect;                                   // time of last connect
    qint64 m_lastRequestTime;                               // time of last request
    bool m_requestOutstanding;                              // true if request outstanding
    bool m_expectingStandardMessage;                        // if a request should also generate a standard message
    unsigned int m_messageDeviceID;                         // and the device from which it should come

    int m_deviceToPoll;
    qint64 m_lastDevicePollTime;

    int m_lastTimerCheckMinute;

    bool m_trace;

    QTime m_sunset;
    QTime m_sunrise;
    double m_latitude;
    double m_longitude;

    qint64 m_lastStatusUpdate;                              // controls sending status updates
};

#endif // _INSTEONDRIVER_H_
