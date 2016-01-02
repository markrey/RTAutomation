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

#ifndef INSTEONDEFS_H
#define INSTEONDEFS_H

#include <qdatetime.h>

typedef struct
{
    unsigned int deviceID;
    int category;
    int subCategory;
    int firmwareRev;
} INSTEON_PLM;


//  Command types for the INSTEON_REQUEST structure

//  Timer mode defs

#define INSTEON_TIMER_MODE_TOD              0               // used time of day to trigger
#define INSTEON_TIMER_MODE_SUNRISE          1               // use sunrise to trigger
#define INSTEON_TIMER_MODE_SUNSET           2               // use sunset to trigger


#define INSTEON_REQUEST_TYPE_DIRECT     0               // direct message
#define INSTEON_REQUEST_TYPE_ADD_DEVICE 1               // add a device to the PLM
#define INSTEON_REQUEST_TYPE_REMOVE_DEVICE 2            // remove a device from the PLM

typedef struct
{
    int deviceID;                                       // Insteon device ID of device
    unsigned char requestType;                          // the request type
    unsigned char cmd1;                                 // context dependent device command
    unsigned char cmd2;                                 // context dependent device command

} INSTEON_REQUEST;

#endif // INSTEONDEFS_H
