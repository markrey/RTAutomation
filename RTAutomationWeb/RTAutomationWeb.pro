#////////////////////////////////////////////////////////////////////////////
#//
#//  This file is part of RTAutomation
#//
#//  Copyright (c) 2015-2016, richards-tech, LLC
#//
#//  Permission is hereby granted, free of charge, to any person obtaining a copy of
#//  this software and associated documentation files (the "Software"), to deal in
#//  the Software without restriction, including without limitation the rights to use,
#//  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
#//  Software, and to permit persons to whom the Software is furnished to do so,
#//  subject to the following conditions:
#//
#//  The above copyright notice and this permission notice shall be included in all
#//  copies or substantial portions of the Software.
#//
#//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
#//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
#//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
#//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
#//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

TEMPLATE = app
TARGET = RTAutomationWeb
DESTDIR = Output

QT += core gui network widgets

DESTDIR = Output

QMAKE_CXXFLAGS_RELEASE -= -g

target.path = /usr/bin
LIBS += -lpaho-mqtt3a
INSTALLS += target

CONFIG += debug_and_release
QMAKE_CXXFLAGS += $$(QT_CXXFLAGS)

DEFINES += QT_NETWORK_LIB  BOOST_SIGNALS_NO_DEPRECATION_WARNING

INCLUDEPATH += GeneratedFiles
DEPENDPATH +=
MOC_DIR += GeneratedFiles/release

LIBS += -lwthttp -lwt -lwtdbo -lwtdbosqlite3 -lboost_regex \
  -lboost_signals -lboost_system -lboost_thread -lboost_filesystem \
  -lboost_program_options -lboost_date_time \
  -lrdkafka -lsasl2 -lssl -lcrypto -lz -lpthread -lrt

OBJECTS_DIR += release
UI_DIR += GeneratedFiles
RCC_DIR += GeneratedFiles
include(RTAutomationWeb.pri)
include(../RTAutomationJSON/RTAutomationJSON.pri)
include(../RTAutomationCommon/RTAutomationCommon.pri)
include(MQTTSensor/MQTTSensor.pri)
