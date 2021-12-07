# switch-it!

## Wireless I/O controller for ESP8266

[__switch-it!__](https://play.google.com/store/apps/details?id=com.switchit.app) is available now on Google Play.

## What is switch-it! ?

switch-it! is a simple configurator, monitoring, and controller app for your D.I.Y ESP8266 IoT device. You can use switch-it! for real-time monitoring and controling for example relay and motion sensor through WebSocket and MQTT. All configuration can be done via the app, including Wi-Fi and MQTT setup, so there is no need to hardcode anything, at all.

## Features

- __One Time Binding__ , the app will store connected device locally, so you don't have to manually enter address of the ESP everytime you wan't to access it.
- __Free 3 Device Registration__, bind up to 3 ESP8266 device for free.
- __MQTT__, support control and monitoring through MQTT. (Currently we only support, and tested MQTT implementation with HiveMQ broker).
- __OTA Firmware Update__, update device firmware through webserver.
- __Auto Discovery__, with mDNS service, the app will automatically look for configured ESP within the same local network.
- __No Coding Required__, we have provided a ready to flash firmware binary file. All you need to do is to configure WiFi connection, and bind it with the app.
- __State Persistance__, the board will revert back to the state it is in before a restart, or power out.
- __Pull to Refresh__, for easy reconnection to all bonded ESP board.
- __Board Reset__, swipe card to unbind the ESP board, and reset it to Wi-Fi configuration mode.

## What's new ? - 07 December 2021

- New device available, PIR / Motion sensor
- OTA update support
- MQTT support with HiveMQ
- Redesigned device card to show more device info and state
- Feedback state from device. Example, when sending command from app to turn relay on, the app will wait for confirmation from ESP device. If relay is on, then device will send feedback, and app will update the button state.

## Setup

1. Download .bin firmware from this repository.
2. Flash .bin firmware to ESP8266 board. You can use any flasher you like. We recommend using [NodeMCU PyFlasher](https://github.com/marcelstoer/nodemcu-pyflasher). Flasher settings:
    - BAUD Rate : 115200
    - Flash Mode : Dual I/O (DIO)
    - Erase Flash : Yes
3. After flashing done, reset the board, the built-in LED will light up, this indicate the board is is Wi-Fi setup mode.
4. Connect to the board via Wi-Fi, SSID should be something like ESP_XXXXXX.
5. Most of the time you will be automatically redirected to configuration page, if not, open browser and go to __1.1.1.1__
6. Click on __Configure WiFi__, tap the SSID you want to connect to or manually enter SSID and password. Once you are done, save, then the ESP will restart.
7. Wait until the LED is blinking. This indicates that the board is broadcasting mDNS.
8. Open the app and press __+__ icon to bind the ESP.
9. Give the device a name, and enter any required device parameters.
10. and you are done!

## MQTT Setup Guide

We currently only support, and have tested the MQTT features with broker provided by HiveMQ. HiveMQ offers a free plan with up to 10GB of data traffic, with option to upgrade in case you are using many devices, and running out of data traffic quota.

1. Go to HiveMQ website, and sign up for their free MQTT cloud service. [HiveMQ Cloud](https://www.hivemq.com/mqtt-cloud-broker/)
2. Create authentication for your MQTT broker, username, and password.
3. Go to swith-it! app, and enter:
    - Username
    - Password
    - Hostname (HiveMQ broker server)
    - Port (8883 is the default port)
    - Keep Alive Period (20)
4. MQTT client ID will be auto generated based on your device UUID.
5. Press connect, if connection is successful, globe icon on top right will turn purple.
6. Go back to main screen, the app will automatically contact all devices through websocket, and send MQTT setup command.
7. If setup is successful, MQTT status icon in each device card will turn purple.
8. If not, you can re-attemp the MQTT setup by pull-to-refresh.

## OTA Guide

1. Make sure you are connected to the same network as the Device you are going to update.
2. Access the OTA webserver, address : __http://{ip address}:81__
3. Enter username, and password.
    - Username : IP Address (ex. 192.168.100.100)
    - Password : deviceType_chipID (ex. relay_299RFT or pir_998PTR). Note that chip ID part could be alphanumeric, and are in upper case letter.
4. Locate the bin file, and click update firmware.
5. The device will reset when OTA update is done.

## Wiring Guide

### Relay / GPIO Device

| App Button |   I/O   | Wemos D1 Mini | Lolin NodeMCU v3 |
|:----------:|:--------:|:-------------:|:----------------:|
|      1     | GPIO 14 |       D5      |        D5        |
|      2     | GPIO 12 |       D6      |        D6        |
|      3     | GPIO 13 |       D7      |        D7        |
|      4     | GPIO 15 |       D8      |        D8        |

### PIR/ Motion sensor

This setup is based on the use of HC-SR501 PIR sensor. This sensor uses 3 connection, power (5v), GND, and input. The mapping is as follows,

| HC-SR501 | I/O | Wemos D1 Mini | Lolin NodeMCU v3 |
|----------|------|---------------|------------------|
| 5v       | -    | 5v            | 5v               |
| Signal   | GPIO 5    | D1            | D1               |
| GND      | -    | GND           | GND              |

## Limitation

- Currently, the ESP board can only be controlled by 1 device, the one it is bonded to.
- You might need to have a physical access to the ESP board to reset or update it with a new firmware.
- Future updates might be breaking, you might need to reflash the ESP board, and re-bind the board to the app.

## Disclaimer

Firmware is provided as is, we are not liable for any damage or consequences of using this firmware. Use this software at your own risk.

## Feedback

We value your feedback and input to help us improve and provide you with many more useful apps, libraries, and tools. If you have any question or inquiries, you can reach us via :

- [Invok Lab](mailto:feedback@invoklab.com?subject=Feedback%20-%20switch-it!%20GitHub)
- [Thoby Noorhalim](mailto:thoby.noorhalim@gmail.com?subject=Feedback%20-%20switch-it!%20GitHub)

Thanks!
