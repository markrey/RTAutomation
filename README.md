# RTAutomation - apps to control an smart home installation using MQTT

### Introduction

RTAutomation consists of a set of apps that can be used to control a smart home automation system. The apps are:

* RTInsteonServer. This app drives an Insteon PowerLinc modem and is able to obtain the current level of pollable devices and also run timers to auutomatically control device states. It uses MQTT to transfer the status of pollable devices and also alerts for devices such as open/close sensors such as the Insteon 2843-222. It runs on Ubuntu systems.

* RTInsteonController. This is a GUI controller app that displays device status and allows device levels to be controlled. It runs on Ubuntu and Mac OS X.

* RTAutomationWeb. This provides a web interface to real time RTMQTT data and also historic data via NiFi with RTNiFiStreamProcessors and Kafka. RTAutomationWeb runs on Ubuntu 14.04.

* RTAutomationManager. RTInsteonServer and RTAutomationWeb can be run in console mode instead of GUI mode when run on headless servers. To manage RTInsteonServer and RTAutomationWeb from another desktop, RTAutomationManager can be used to duplicate the local GUI management interface. RTAutomationManager runs on Ubuntu and Mac OS X.

The status and alert messages generated by RTInsteonServer can be captured in Apache NiFi by the GetMQTTSensorProcessor that is part of RTNiFiStreamProcessors at https://github.com/richards-tech/RTNiFiStreamProcessors.

### Pre-requisites

#### General

It is assumed that an MQTT broker is installed and running and available to the apps.

#### Ubuntu

The apps require Qt5 and the Paho MQTT client:

    cd ~/
    sudo apt-get install qt5-default git build-essential
    git clone http://git.eclipse.org/gitroot/paho/org.eclipse.paho.mqtt.c.git
    cd org.eclipse.paho.mqtt.c
    make
    sudo make install
    
To build RTAutomationWeb, Wt is required:

    sudo apt-get install witty witty-dev
    
The librdkafka client is also required:

    cd ~/
    git clone https://github.com/edenhill/librdkafka
    cd librdkafka
    ./configure
    make
    sudo make install
    
#### Mac OS X

Qt5, Xcode and the Paho MQTT client are required. Xcode can be installed from the App Store. Qt5 can be installed using the QT5 installers here - http://doc.qt.io/qt-5/osx.html.

Palo MQTT prebuilt libraries can be used. Download the Mac version from this page - https://projects.eclipse.org/projects/technology.paho/downloads.

### Build

Once the pre-requisites have been installed, the repo can be cloned and built:

    cd ~/
    git clone git://github.com/richards-tech/RTAutomation.git
    cd RTAutomation
    qmake
    make -j4
    sudo make install
    
On Ubuntu this will build RTInsteonServer, RTInsteonController and RTInsteonManager. On Mac OS X, this will omit RTInsteonServer as it will not work on the Mac OS X.

### Run

The easiest way to start is to run RTInsteonServer in GUI mode - basically just enter RTInsteonServer to start it running. Then, the serial port for the Insteon modem can be set (defaults to /dev/ttyUSB0) and the broker address can be configured (defaults to tcp://localhost:1883). On a headless server, RTInsteonServer must be run in console mode:

    RTInsteonServer -x
    
To set up the basic configuration, run RTInsteonServer and exit. It will create a default configuration file in ~/.config/richards-tech/RTInsteonServer.conf that looks like this:

    [General]
    MQTTBrokerAddress=tcp://localhost:1883
    MQTTClientID=rtinsteonserver
    MQTTClientSecret=rtinsteonserver
    MQTTDeviceID=rtinsteonserver
    
    [PLM]
    PLMLatitude=42.0
    PLMLongitude=75.0
    PLMPort=ttyUSB0
    
    [topicGroup]
    controlTopic=control
    managementCommandTopic=managementCommand
    managementResponseTopic=managementResponse
    statusTopic=status
    
Edit this file to set the broker address and other MQTT client information as required. Once RTInsteonServer is able to connect to the MQTT broker, the remaining configuration can be performed using RTInsteonManager running elsewhere.

There are a couple of entries for latitude and longitude. Timers can be set to trigger at sunset or sunrise and the latitude and longitude of the site is required for this to be accurate.

RTInsteonManager can then be run. It's GUI allows the MQTT broker information to be configured and also to set the deviceid of the app to be managed. This is set up so that the defaults work correctly to manage RTInsteonServer (default deviceid is rtinsteonserver). Make any changes to broker information that may be required and press Connect. After a second or two RTInsteonManager should be connected to the broker and displaying the main management menu from RTInsteonServer.

RTInsteonManager allows Insteon devices to be added to RTInsteonServer's database and also configure timers to set device levels at specfic times of day. There are two ways to get Insteon devices into the database. The easiest is probably to manually enter the Insteon device address. Use the Device status button on the main management menu and then press the Add a new device button. There is no need to link the Insteon devices in this situation. The other method is to link the Insteon devices in the normal way - with the PowerLinc modem as the controller and the device should appear in the device status display. A unique name can then be added to the device manually.

For non-pollable devices (typically open/close sensors, motion sensors etc) that generate state changes without polling, it is necessary to use Insteon linking with the sensor as the controller. The new device will appear on the device status display without a name and with a default Insteon device type. The Configure button must be used to select the correct device type and give the device a unique name.

RTInsteonController can be used to display and control the current levels of Insteon devices in InsteonServer's database. Once started, the Configure button can be used to set up the broker address and ID of the RTInsteonServer instance to use. The Insteon devices and current state will be displayed in a list. Clicking on one of the entries allows its level to be set.

### RTInsteonServer and RTInsteonManager display updates

By default, displays such as Device status do not auto-update. There is a manual update button that can be used for this. Alternatively, an automatic update can be set and te rate controlled with the dropdown at the top of the display.

### MQTT Topics

The topics in the set of apps are pre-configured to operate correctly. While they can be changed if necessary, it is then important that all apps are updated together. In general, there is no need to change the topics as the MQTT topics are qualified by the device ID (of the form <deviceID>/<topic> when sent to the MQTT broker) so choosing unique device IDs ensures that MQTT topics won't clash.

### MAC OS X app deployment

To build an bundle that can be moved to other Macs without having to install any other pre-requisites, the macdeployqt tool (included with Qt5 on the Mac) can be used. After building the app (RTInsteonController in this case) run:

    cd ~/
    macdeployqt RTAutomation/RTInsteonController/Output/RTInsteonController.app
    
This will add the require Qt dependencies but not the Paho MQTT client libraries. These can be added manually by copying the contents of the lib directory of the Paho client download into Contents/Frameworks directory of the app bundle. Then the RTInsteonController.app can be copied onto other Macs as required.

### Running RTAutomationWeb

By default, RTAutomationWeb provides a web server on port 8080. The MQTT broker address and Kafka broker list need to be configured. Devices to be accessed must be added manually via the management interface (either locally or via RTAutomationManager).

The default user name and password are both "richards-tech". SSL is potentially supported if the correct certificates are available in the local directory. The secure interface will appear on port 8081.

Both real-time and historic data can be displayed. Real-time data comes directly from MQTT streams. Historic data comes from NiFi/Kafka and assumes that the correct NiFi processors and Kafka topics have been configured.


