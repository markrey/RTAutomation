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

#ifndef RTAUTOMATIONJSONDEFS_H
#define RTAUTOMATIONJSONDEFS_H

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

//  All RTAUTOMATIONJSON records start with the following header:

#define RTAUTOMATIONJSON_RECORD_TYPE              "sjType"        // common to all control messages
#define RTAUTOMATIONJSON_RECORD_COMMAND           "sjCommand"     // the command type
#define RTAUTOMATIONJSON_RECORD_PARAM             "sjParam"       // a parameter
#define RTAUTOMATIONJSON_RECORD_INDEX             "sjIndex"       // a monotonically increasing number set by commander, copied by responder
#define RTAUTOMATIONJSON_RECORD_DATA              "sjData"        // the data in the record

//  Supported record types

#define RTAUTOMATIONJSON_RECORD_TYPE_DIALOGMENU   QString("dialogMenu")  // describes what dialogs are available
#define RTAUTOMATIONJSON_RECORD_TYPE_DIALOG       QString("dialog")   // handles a specific dialog - param has name of dialog
#define RTAUTOMATIONJSON_RECORD_TYPE_DIALOGUPDATE QString("dialogUpdate")  // updates data for a specific dialog - param has name of dialog
#define RTAUTOMATIONJSON_RECORD_TYPE_PING         QString("ping") // ping service for performance measurement

//  Supported commands are:

#define RTAUTOMATIONJSON_COMMAND_REQUEST          QString("request")
#define RTAUTOMATIONJSON_COMMAND_RESPONSE         QString("response")

//  Command Request/Response parameters

#define RTAUTOMATIONJSON_PARAM_DIALOG_NAME        QString("paramDialogName")   // a string representing a dialog name
#define RTAUTOMATIONJSON_PARAM_INDEX              QString("paramIndex")    // an integer value typically used to select a row
#define RTAUTOMATIONJSON_PARAM_COOKIE             QString("paramCookie")   // a string parameter (used as cookie)

//----------------------------------------------------------
//
//  Dialog menu is sent as one object containg two arrays - one for config, one for info

#define RTAUTOMATIONJSON_DIALOGMENU_CONFIG        "dmConfig"      // config menu entries
#define RTAUTOMATIONJSON_DIALOGMENU_INFO          "dmInfo"        // info menu entries

//  Each array (config and info, has these headers:

#define RTAUTOMATIONJSON_DIALOGMENU_NAME          "dmName"        // the internal name of the dialog - returned as param in request
#define RTAUTOMATIONJSON_DIALOGMENU_DESC          "dmDesc"        // human readable description

//  Standard dialog menu entries

#define RTAUTOMATIONJSON_DIALOG_NAME_BASICSETUP   "basicSetup"
#define RTAUTOMATIONJSON_DIALOG_DESC_BASICSETUP   "Basic MQTT client setup"
#define RTAUTOMATIONJSON_DIALOG_NAME_ABOUT        "about"
#define RTAUTOMATIONJSON_DIALOG_DESC_ABOUT        "About this app"
#define RTAUTOMATIONJSON_DIALOG_NAME_RESTARTAPP   "restartApp"
#define RTAUTOMATIONJSON_DIALOG_DESC_RESTARTAPP   "Restart application"
#define RTAUTOMATIONJSON_DIALOG_NAME_PING         "ping"
#define RTAUTOMATIONJSON_DIALOG_DESC_PING         "Ping test service"

//----------------------------------------------------------
//
//  Dialog data is an object that goes in RTAUTOMATIONJSON_RECORD_DATA and has the following headers:

#define RTAUTOMATIONJSON_DIALOG_NAME             "dialogName"        // the internal name of the dialog
#define RTAUTOMATIONJSON_DIALOG_DESC             "dialogDesc"        // this is displayed on the dialog as the title
#define RTAUTOMATIONJSON_DIALOG_UPDATE           "dialogUpdate"      // true if dialog needs updating
#define RTAUTOMATIONJSON_DIALOG_COOKIE           "dialogCookie"      // used to retain state across updates
#define RTAUTOMATIONJSON_DIALOG_DATA             "dialogData"        // an array of dialog entries

// An entry contains:

#define RTAUTOMATIONJSON_DIALOG_VAR_NAME               "varName"       // this is the internal name - usually the class member name
#define RTAUTOMATIONJSON_DIALOG_VAR_DESC               "varDesc"       // the description displayed in the dialog
#define RTAUTOMATIONJSON_DIALOG_VAR_TYPE               "varType"       // one of the supported var types


//----------------------------------------------------------
//
//  Config defs

//  The rest of the config field entries may be present depending on the var type:

#define RTAUTOMATIONJSON_CONFIG_VAR_VALUE              "varValue"      // the current value if only one possible
#define RTAUTOMATIONJSON_CONFIG_VAR_VALUE_ARRAY        "varValueArray" // if the result can be more than one value
#define RTAUTOMATIONJSON_CONFIG_VAR_MINIMUM            "varMinimum"    // minimum value for integer vars
#define RTAUTOMATIONJSON_CONFIG_VAR_MAXIMUM            "varMaximum"    // maximum value for integer vars
#define RTAUTOMATIONJSON_CONFIG_STRING_ARRAY           "varStringArray"// where a string selection is from one of a set

//  var array key

#define RTAUTOMATIONJSON_CONFIG_VAR_ENTRY               QString("varEntry")


//  Supported var types are:

#define RTAUTOMATIONJSON_CONFIG_VAR_TYPE_BOOL            QString("configBool")
#define RTAUTOMATIONJSON_CONFIG_VAR_TYPE_INTEGER         QString("configInt")
#define RTAUTOMATIONJSON_CONFIG_VAR_TYPE_HEXINTEGER      QString("configHexInt")
#define RTAUTOMATIONJSON_CONFIG_VAR_TYPE_STRING          QString("configString")
#define RTAUTOMATIONJSON_CONFIG_VAR_TYPE_PASSWORD        QString("configPassword")
#define RTAUTOMATIONJSON_CONFIG_VAR_TYPE_SELECTION       QString("configSelection")
#define RTAUTOMATIONJSON_CONFIG_VAR_TYPE_SERVICEPATH     QString("configServicePath")
#define RTAUTOMATIONJSON_CONFIG_VAR_TYPE_APPNAME         QString("configAppName")
#define RTAUTOMATIONJSON_CONFIG_VAR_TYPE_UID             QString("configUID")
#define RTAUTOMATIONJSON_CONFIG_VAR_TYPE_IPADDRESS       QString("configIPAddress")
#define RTAUTOMATIONJSON_CONFIG_VAR_TYPE_BUTTON          QString("configButton")
#define RTAUTOMATIONJSON_CONFIG_VAR_TYPE_DOW             QString("configDOW")


//----------------------------------------------------------
//
//  Info defs

#define RTAUTOMATIONJSON_INFO_VAR_VALUE                  "varValue"          // the current value
#define RTAUTOMATIONJSON_INFO_VAR_VALUE_ARRAY            "varValueArray"     // if the result can be more than one value

#define RTAUTOMATIONJSON_INFO_VAR_TYPE_BUTTON            QString("infoButton")
#define RTAUTOMATIONJSON_INFO_VAR_TYPE_STRING            QString("infoString")

//  Tables vars are themselves structured. RTAUTOMATIONJSON_CONFIG_TABLEHEADER_ARRAY is used to contain the column headers.
//  RTAUTOMATIONJSON_CONFIG_TABLECOLUMNWIDTH_ARRAY can optionally be used to control column widths.
//  RTAUTOMATIONJSON_CONFIG_VAR_VALUE_ARRAY is used to hold the entries to be displayed in the table. The array contains the
//  entry for row 0, column 0, then row 0 column 1 etc. The number of columns is determined by the size of the
//  RTAUTOMATIONJSON_CONFIG_TABLEHEADER_ARRAY, the number of rows form the number of elements in the RTAUTOMATIONJSON_CONFIG_VAR_VALUE_ARRAY
//  divided by the number of columns. The way entries are displayed is determined from their JSON var type

#define RTAUTOMATIONJSON_INFO_VAR_TYPE_TABLE             QString("infoTable")

//  var array key

#define RTAUTOMATIONJSON_INFO_VAR_ENTRY                  QString("varEntry")

//  extra table defines

#define RTAUTOMATIONJSON_INFO_TABLEHEADER_ARRAY          QString("tableHeader")
#define RTAUTOMATIONJSON_INFO_TABLECOLUMNWIDTH_ARRAY     QString("tableColumnWidth")
#define RTAUTOMATIONJSON_INFO_TABLE_CONFIG_COLUMN        QString("tableConfigColumn")
#define RTAUTOMATIONJSON_INFO_TABLE_CONFIG_DIALOGNAME    QString("tableConfigDialog")

//  Graphics elements that can be added to a dialog:

#define RTAUTOMATIONJSON_GRAPHICS_VAR_TYPE_STRING        QString("graphicsString")
#define RTAUTOMATIONJSON_GRAPHICS_VAR_TYPE_LINE          QString("graphicsLine")

#define RTAUTOMATIONJSON_GRAPHICS_VAR_VALUE              QString("varValue")

//  Key for stylesheet that can be added to graphics string

#define RTAUTOMATIONJSON_GRAPHICS_VAR_STYLE                  QString("style")

#endif // RTAUTOMATIONJSONDEFS_H
