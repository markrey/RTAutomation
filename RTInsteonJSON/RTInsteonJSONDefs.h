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

#ifndef RTINSTEONJSONDEFS_H
#define RTINSTEONJSONDEFS_H

#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonvalue.h>

//  All variable types are mapped to strings. String are string :-).
//  Integers are sent as decimal numbers in all cases (they may be displayed in hex if the type is set for that).
//  Booleans are sent as "true" or "false"
//  IPAddresses are sent as strings in a dotted address form (xxx.xxx.xxx.xxx)
//  Mac addresses are sent as integers with hex display and processing
//  Insteon address are sent as strings in the dotted form (xx.xx.xx where xx is a hex number)

//  All RTINSTEONJSON records start with the following header:

#define RTINSTEONJSON_RECORD_TYPE              "sjType"        // common to all control messages
#define RTINSTEONJSON_RECORD_COMMAND           "sjCommand"     // the command type
#define RTINSTEONJSON_RECORD_PARAM             "sjParam"       // a parameter
#define RTINSTEONJSON_RECORD_INDEX             "sjIndex"       // a monotonically increasing number set by commander, copied by responder
#define RTINSTEONJSON_RECORD_DATA              "sjData"        // the data in the record

//  Supported record types

#define RTINSTEONJSON_RECORD_TYPE_DIALOGMENU   QString("dialogMenu")  // describes what dialogs are available
#define RTINSTEONJSON_RECORD_TYPE_DIALOG       QString("dialog")   // handles a specific dialog - param has name of dialog
#define RTINSTEONJSON_RECORD_TYPE_DIALOGUPDATE QString("dialogUpdate")  // updates data for a specific dialog - param has name of dialog
#define RTINSTEONJSON_RECORD_TYPE_PING         QString("ping") // ping service for performance measurement

//  Supported commands are:

#define RTINSTEONJSON_COMMAND_REQUEST          QString("request")
#define RTINSTEONJSON_COMMAND_RESPONSE         QString("response")

//  Command Request/Response parameters

#define RTINSTEONJSON_PARAM_DIALOG_NAME        QString("paramDialogName")   // a string representing a dialog name
#define RTINSTEONJSON_PARAM_INDEX              QString("paramIndex")    // an integer value typically used to select a row
#define RTINSTEONJSON_PARAM_COOKIE             QString("paramCookie")   // a string parameter (used as cookie)

//----------------------------------------------------------
//
//  Dialog menu is sent as one object containg two arrays - one for config, one for info

#define RTINSTEONJSON_DIALOGMENU_CONFIG        "dmConfig"      // config menu entries
#define RTINSTEONJSON_DIALOGMENU_INFO          "dmInfo"        // info menu entries

//  Each array (config and info, has these headers:

#define RTINSTEONJSON_DIALOGMENU_NAME          "dmName"        // the internal name of the dialog - returned as param in request
#define RTINSTEONJSON_DIALOGMENU_DESC          "dmDesc"        // human readable description

//  Standard dialog menu entries

#define RTINSTEONJSON_DIALOG_NAME_BASICSETUP   "basicSetup"
#define RTINSTEONJSON_DIALOG_DESC_BASICSETUP   "Basic MQTT client setup"
#define RTINSTEONJSON_DIALOG_NAME_ABOUT        "about"
#define RTINSTEONJSON_DIALOG_DESC_ABOUT        "About this app"
#define RTINSTEONJSON_DIALOG_NAME_RESTARTAPP   "restartApp"
#define RTINSTEONJSON_DIALOG_DESC_RESTARTAPP   "Restart application"
#define RTINSTEONJSON_DIALOG_NAME_PING         "ping"
#define RTINSTEONJSON_DIALOG_DESC_PING         "Ping test service"

//----------------------------------------------------------
//
//  Dialog data is an object that goes in RTINSTEONJSON_RECORD_DATA and has the following headers:

#define RTINSTEONJSON_DIALOG_NAME             "dialogName"        // the internal name of the dialog
#define RTINSTEONJSON_DIALOG_DESC             "dialogDesc"        // this is displayed on the dialog as the title
#define RTINSTEONJSON_DIALOG_UPDATE           "dialogUpdate"      // true if dialog needs updating
#define RTINSTEONJSON_DIALOG_COOKIE           "dialogCookie"      // used to retain state across updates
#define RTINSTEONJSON_DIALOG_DATA             "dialogData"        // an array of dialog entries

// An entry contains:

#define RTINSTEONJSON_DIALOG_VAR_NAME               "varName"       // this is the internal name - usually the class member name
#define RTINSTEONJSON_DIALOG_VAR_DESC               "varDesc"       // the description displayed in the dialog
#define RTINSTEONJSON_DIALOG_VAR_TYPE               "varType"       // one of the supported var types


//----------------------------------------------------------
//
//  Config defs

//  The rest of the config field entries may be present depending on the var type:

#define RTINSTEONJSON_CONFIG_VAR_VALUE              "varValue"      // the current value if only one possible
#define RTINSTEONJSON_CONFIG_VAR_VALUE_ARRAY        "varValueArray" // if the result can be more than one value
#define RTINSTEONJSON_CONFIG_VAR_MINIMUM            "varMinimum"    // minimum value for integer vars
#define RTINSTEONJSON_CONFIG_VAR_MAXIMUM            "varMaximum"    // maximum value for integer vars
#define RTINSTEONJSON_CONFIG_STRING_ARRAY           "varStringArray"// where a string selection is from one of a set

//  var array key

#define RTINSTEONJSON_CONFIG_VAR_ENTRY               QString("varEntry")


//  Supported var types are:

#define RTINSTEONJSON_CONFIG_VAR_TYPE_BOOL            QString("configBool")
#define RTINSTEONJSON_CONFIG_VAR_TYPE_INTEGER         QString("configInt")
#define RTINSTEONJSON_CONFIG_VAR_TYPE_HEXINTEGER      QString("configHexInt")
#define RTINSTEONJSON_CONFIG_VAR_TYPE_STRING          QString("configString")
#define RTINSTEONJSON_CONFIG_VAR_TYPE_PASSWORD        QString("configPassword")
#define RTINSTEONJSON_CONFIG_VAR_TYPE_SELECTION       QString("configSelection")
#define RTINSTEONJSON_CONFIG_VAR_TYPE_SERVICEPATH     QString("configServicePath")
#define RTINSTEONJSON_CONFIG_VAR_TYPE_APPNAME         QString("configAppName")
#define RTINSTEONJSON_CONFIG_VAR_TYPE_UID             QString("configUID")
#define RTINSTEONJSON_CONFIG_VAR_TYPE_IPADDRESS       QString("configIPAddress")
#define RTINSTEONJSON_CONFIG_VAR_TYPE_BUTTON          QString("configButton")
#define RTINSTEONJSON_CONFIG_VAR_TYPE_DOW             QString("configDOW")


//----------------------------------------------------------
//
//  Info defs

#define RTINSTEONJSON_INFO_VAR_VALUE                  "varValue"          // the current value
#define RTINSTEONJSON_INFO_VAR_VALUE_ARRAY            "varValueArray"     // if the result can be more than one value

#define RTINSTEONJSON_INFO_VAR_TYPE_BUTTON            QString("infoButton")
#define RTINSTEONJSON_INFO_VAR_TYPE_STRING            QString("infoString")

//  Tables vars are themselves structured. RTINSTEONJSON_CONFIG_TABLEHEADER_ARRAY is used to contain the column headers.
//  RTINSTEONJSON_CONFIG_TABLECOLUMNWIDTH_ARRAY can optionally be used to control column widths.
//  RTINSTEONJSON_CONFIG_VAR_VALUE_ARRAY is used to hold the entries to be displayed in the table. The array contains the
//  entry for row 0, column 0, then row 0 column 1 etc. The number of columns is determined by the size of the
//  RTINSTEONJSON_CONFIG_TABLEHEADER_ARRAY, the number of rows form the number of elements in the RTINSTEONJSON_CONFIG_VAR_VALUE_ARRAY
//  divided by the number of columns. The way entries are displayed is determined from their JSON var type

#define RTINSTEONJSON_INFO_VAR_TYPE_TABLE             QString("infoTable")

//  var array key

#define RTINSTEONJSON_INFO_VAR_ENTRY                  QString("varEntry")

//  extra table defines

#define RTINSTEONJSON_INFO_TABLEHEADER_ARRAY          QString("tableHeader")
#define RTINSTEONJSON_INFO_TABLECOLUMNWIDTH_ARRAY     QString("tableColumnWidth")
#define RTINSTEONJSON_INFO_TABLE_CONFIG_COLUMN        QString("tableConfigColumn")
#define RTINSTEONJSON_INFO_TABLE_CONFIG_DIALOGNAME    QString("tableConfigDialog")

//  Graphics elements that can be added to a dialog:

#define RTINSTEONJSON_GRAPHICS_VAR_TYPE_STRING        QString("graphicsString")
#define RTINSTEONJSON_GRAPHICS_VAR_TYPE_LINE          QString("graphicsLine")

#define RTINSTEONJSON_GRAPHICS_VAR_VALUE              QString("varValue")

//  Key for stylesheet that can be added to graphics string

#define RTINSTEONJSON_GRAPHICS_VAR_STYLE                  QString("style")

#endif // RTINSTEONJSONDEFS_H
