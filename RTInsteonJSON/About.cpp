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

#include "About.h"
#include "RTInsteonArgs.h"

About::About() : Dialog(RTINSTEONJSON_DIALOG_NAME_ABOUT, RTINSTEONJSON_DIALOG_DESC_ABOUT)
{
}

void About::getDialog(QJsonObject& newDialog)
{
    clearDialog();
    addVar(createGraphicsStringVar(RTInsteonArgs::getAppName(), "font-size: 20px; qproperty-alignment: AlignCenter"));
    addVar(createGraphicsStringVar("An RTAutomation Application", "qproperty-alignment: AlignCenter"));
    addVar(createGraphicsStringVar("Copyright (c) 2015-2016, richards-tech, LLC", "qproperty-alignment: AlignCenter"));
    addVar(createGraphicsLineVar());
    addVar(createInfoStringVar("Name:", RTInsteonArgs::getAppName()));
    addVar(createInfoStringVar("Build date:", QString("%1 %2").arg(__DATE__).arg(__TIME__)));
    addVar(createInfoStringVar("Qt version:", qVersion()));

    return dialog(newDialog);
}

