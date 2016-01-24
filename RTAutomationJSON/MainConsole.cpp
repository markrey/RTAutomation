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

#include <qprocess.h>
#include <qapplication.h>

#include "MainConsole.h"
#include "CommandService.h"
#include "About.h"
#include "BasicSetup.h"
#include "RestartApp.h"
#include "RTAutomationLog.h"
#include "RTAutomationArgs.h"

#ifndef _WINDOWS
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#else
#include <conio.h>
#endif
#include <stdio.h>

volatile bool MainConsole::sigIntReceived = false;

MainConsole::MainConsole(QObject *parent, bool daemonMode)
    : QThread(parent)
{
    m_restart = false;
    m_mustExit = false;
    m_daemonMode = daemonMode;

    connect((QCoreApplication *)parent, SIGNAL(aboutToQuit()), this, SLOT(aboutToQuit()));

    m_commandService = new CommandService();

}

void MainConsole::addStandardDialogs()
{
    RTAutomationJSON::addConfigDialog(new BasicSetup());

    RestartApp *restartApp = new RestartApp();
    connect(restartApp, SIGNAL(restartApp()), this, SLOT(restartApp()), Qt::QueuedConnection);
    RTAutomationJSON::addConfigDialog(restartApp);

    RTAutomationJSON::addInfoDialog(new About());
}

void MainConsole::startServices()
{
    m_configMenuArray = RTAutomationJSON::getConfigDialogMenu();
    m_infoMenuArray = RTAutomationJSON::getInfoDialogMenu();

    m_commandService->resumeThread();

#ifndef WIN32
    registerSigHandler();
    if (m_daemonMode)
        RTAutomationLog::setLogDisplayLevel(RTAUTOMATION_LOG_LEVEL_NONE);
#endif
    start();
}

void MainConsole::stopServices()
{
    if (m_commandService) {
        m_commandService->exitThread();
        m_commandService = NULL;
    }
}

void MainConsole::aboutToQuit()
{

    for (int i = 0; i < 5; i++) {
        if (wait(1000))
            break;

        printf("Waiting for console thread to exit...\n");
    }
    if (m_restart) {
        thread()->msleep(1000);
        QStringList args = QApplication::arguments();
        args.removeFirst();
        QProcess::startDetached(QApplication::applicationFilePath(), args);
    }
    exit(0);
}

void MainConsole::run()
{
    char c;

#ifndef WIN32
    if (m_daemonMode) {
        runDaemon();
        return;
    }
#endif

#ifndef _WINDOWS
        struct termios	ctty;

        tcgetattr(fileno(stdout), &ctty);
        ctty.c_lflag &= ~(ICANON);
        ctty.c_cc[VMIN] = 1;
        ctty.c_cc[VTIME] = 1;
        tcsetattr(fileno(stdout), TCSANOW, &ctty);
#endif

    while (!MainConsole::sigIntReceived && !m_mustExit) {

        printf("\nEnter option: ");

#ifdef _WINDOWS
        c = toupper(_getch());
#else
        fflush(stdout);
        while (!checkForChar()) {
            if (m_mustExit || MainConsole::sigIntReceived)
                break;
        }
        if (m_mustExit || MainConsole::sigIntReceived)
            break;

        c = toupper(getchar());
#endif
        switch (c) {
        case 'X':
            m_mustExit = true;
            break;

        default:
            processInput(c);
        }
    }
    m_mustExit = true;
    stopServices();
    appExit();
    ((QCoreApplication *)parent())->exit();
}

void MainConsole::restartApp()
{
    m_restart = true;
    m_mustExit = true;
}


#ifndef _WINDOWS
void MainConsole::runDaemon()
{
    while (!MainConsole::sigIntReceived && !m_mustExit)
        msleep(100);

    stopServices();
    appExit();
    ((QCoreApplication *)parent())->exit();
}

void MainConsole::registerSigHandler()
{
    struct sigaction sia;

    bzero(&sia, sizeof sia);
    sia.sa_handler = MainConsole::sigHandler;

    if (sigaction(SIGINT, &sia, NULL) < 0)
        perror("sigaction(SIGINT)");
    if (sigaction(SIGQUIT, &sia, NULL) < 0)
        perror("sigaction(SIGQUIT)");
}

void MainConsole::sigHandler(int)
{
    MainConsole::sigIntReceived = true;
}

bool MainConsole::checkForChar()
{
    struct timeval tv = {0, 100000};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv) == 1;
}

#endif

