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

#ifndef INSTEON_DEVICE_H
#define INSTEON_DEVICE_H

#include <qstring.h>
#include <qdatetime.h>
#include <qstringlist.h>
#include <qjsonobject.h>

//  JSON message defs

#define INSTEON_DEVICE_JSON_UPDATELIST      "updateList"
#define INSTEON_DEVICE_JSON_DELTA           "delta"
#define INSTEON_DEVICE_JSON_ALERT           "alert"
#define INSTEON_DEVICE_JSON_SETDEVICELEVEL  "setDeviceLevel"

//  JSON field defs

#define INSTEON_DEVICE_JSON_NAME            "name"
#define INSTEON_DEVICE_JSON_FLAGS           "flags"
#define INSTEON_DEVICE_JSON_GROUP           "group"
#define INSTEON_DEVICE_JSON_DEVICEID        "deviceID"
#define INSTEON_DEVICE_JSON_DEVICECAT       "deviceCat"
#define INSTEON_DEVICE_JSON_DEVICESUBCAT    "deviceSubcat"
#define INSTEON_DEVICE_JSON_CURRENTLEVEL    "currentLevel"
#define INSTEON_DEVICE_JSON_NEWLEVEL        "newLevel"
#define INSTEON_DEVICE_JSON_STATE           "state"


typedef struct
{
    int devCat;                                             // device category
    int devSubcat;                                          // sub category
    QString type;
} DEVICEMAP;

class InsteonDevice
{
public:
    InsteonDevice();
    InsteonDevice(const InsteonDevice &rhs);
    ~InsteonDevice();

    bool read(const QJsonObject& json);
    void write(QJsonObject& json) const;

    bool readNewLevel(const QJsonObject& json);
    void writeNewLevel(QJsonObject& json) const;

    QString name;
    unsigned char flags;
    unsigned char group;
    unsigned int  deviceID;
    unsigned char deviceCat;
    unsigned char deviceSubcat;

    // transient data
    bool poll;
    QDateTime lastTimeSeen;
    int timeouts;
    int currentLevel;
    int newLevel;

    QString state;

    static QStringList getDeviceTypes();
    static const QString& getTypeFromCat(int devCat, int devSubcat);
    static void getCatFromType(const QString& type, unsigned char& cat, unsigned char& subcat);
    static QString deviceIDToString(int deviceID);
};

#endif // INSTEON_DEVICE_H
