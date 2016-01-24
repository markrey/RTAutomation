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

// variables used in all RTMQTT JSON records

#define RTMQTTJSON_DEVICEID         "deviceID"              // id of device
#define RTMQTTJSON_TOPIC            "topic"                 // original message topic
#define RTMQTTJSON_TIMESTAMP        "timestamp"             // seconds since epoch

// variables used in JSON sensor records

#define RTMQTTJSON_ACCEL_DATA       "accel"                 // accelerometer x, y, z data in gs
#define RTMQTTJSON_LIGHT_DATA       "light"                 // light data in lux
#define RTMQTTJSON_TEMPERATURE_DATA "temperature"           // temperature data in degrees C
#define RTMQTTJSON_PRESSURE_DATA    "pressure"              // pressure in Pa
#define RTMQTTJSON_HUMIDITY_DATA    "humidity"              // humidity in %RH

// variables used in JSON video records

#define RTMQTTJSON_VIDEO_DATA       "video"                 // video data in hex
#define RTMQTTJSON_VIDEO_WIDTH      "vwidth"                // video frame width
#define RTMQTTJSON_VIDEO_HEIGHT     "vheight"               // video frame height
#define RTMQTTJSON_VIDEO_RATE       "vrate"                 // video frame rate
#define RTMQTTJSON_VIDEO_FORMAT     "vformat"               // video frame format (eg mjpeg)

// variables used in JSON audio records

#define RTMQTTJSON_AUDIO_DATA       "audio"                 // audio data in hex
#define RTMQTTJSON_AUDIO_RATE       "arate"                 // audio sample rate
#define RTMQTTJSON_AUDIO_CHANNELS   "achannels"             // number of audio channels
#define RTMQTTJSON_AUDIO_SAMPTYPE   "asamptype"             // sample type (eg int16)
#define RTMQTTJSON_AUDIO_FORMAT     "aformat"
