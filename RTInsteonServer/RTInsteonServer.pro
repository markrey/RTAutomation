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
TARGET = RTInsteonServer

QT += core gui network widgets

CONFIG += debug_and_release

unix:QMAKE_CXXFLAGS_RELEASE -= -g

target.path = /usr/bin
INSTALLS += target

DEFINES += QT_NETWORK_LIB

INCLUDEPATH += GeneratedFiles \
        GeneratedFiles/release

LIBS += -lpaho-mqtt3a

MOC_DIR += GeneratedFiles/release

OBJECTS_DIR += release

DESTDIR = Output

UI_DIR += GeneratedFiles

RCC_DIR += GeneratedFiles

include(RTInsteonServer.pri)
include(../RTAutomationJSON/RTAutomationJSON.pri)
include(../RTAutomationCommon/RTAutomationCommon.pri)
include(../qextserialport/src/qextserialport.pri)

