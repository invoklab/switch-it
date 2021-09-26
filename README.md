# switch-it!
## Wireless I/O controller for ESP8266

[__switch-it!__](https://play.google.com/store/apps/details?id=com.switchit.app) is available now on Google Play. 
## What is switch-it! ?
switch-it! is a simple controller app that lets you control your ESP8266 I/O pins wirelessly. Need a quick fix for project demonstration? No worries, flash .bin firmware to ESP8266 board, and you can control it in no time. Possible use case includes:
- Turn on/off appliances using relay (smart home)
- Turn on/off LEDs, lights, buzzer
- Control DC Motors, actuators
- and many more!

## Features
- __One Time Binding__ , the app will store connected device locally, so you don't have to manually enter address of the ESP everytime you wan't to access it.
- __3 Device Registration__, bind up to 3 ESP8266 device for free.
- __Auto Discovery__, with mDNS service, the app will automatically look for configured ESP within the same local network.
- __No Coding Required__, we have provided a ready to flash firmware binary file. All you need to do is to configure WiFi connection, and bind it with the app.
- __State Persistance__, the board will revert back to the state it is in before a restart, or power out.
- __Pull to Refresh__, for easy reconnection to all bonded ESP board.
- __Board Reset__, swipe card to unbind the ESP board, and reset it to Wi-Fi configuration mode.

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
9. Give the device a name, and specify how many I/O channel you want to control.
10. and you are done!

## Wiring Guide
| App Button 	|   I/O   	| Wemos D1 Mini 	| Lolin NodeMCU v3 	|
|:----------:	|:--------:	|:-------------:	|:----------------:	|
|      1     	| GPIO 14 	|       D5      	|        D5        	|
|      2     	| GPIO 12 	|       D6      	|        D6        	|
|      3     	| GPIO 13 	|       D7      	|        D7        	|
|      4     	| GPIO 15 	|       D8      	|        D8        	|

## Limitation
- Currently, the ESP board can only be controlled by 1 device, the one it is bonded to. 
- No OTA support yet.
- You might need to have a physical access to the ESP board to reset or update it with a new firmware.
- Future updates might be breaking, you might need to reflash the ESP board, and re-bind the board to the app.

## Disclaimer
Firmware is provided as is, we are not liable for any damage or consequences of using this firmware. Use this software at your own risk.

## Feedback
We value your feedback and input to help us improve and provide you with many more useful apps, libraries, and tools. If you have any question or inquiries, you can reach us via :
- [Invok Lab](mailto:invoklab@gmail.com?subject=Feedback%20-%20switch-it!%20GitHub)
- [Thoby Noorhalim](mailto:thoby.noorhalim@gmail.com?subject=Feedback%20-%20switch-it!%20GitHub)

Thanks!