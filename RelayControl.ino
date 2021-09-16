// #define DEBUG

#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <ESP8266mDNS.h> 
#include <WebSocketsServer.h>
#include "RelayState.h"
#include "FlashRW.h"
#include <string>
#include <vector>

#define NUM_OF_DATA 10
#define NUM_OF_CHANNEL 4

// Pins
int channelPin[NUM_OF_CHANNEL] = {14,12,13,15};
int sizeOfChannelPin = sizeof(channelPin)/sizeof(channelPin[0]);

// Globals
WebSocketsServer websocket = WebSocketsServer(80);
RelayState myRelayState = RelayState();
FlashRW myFlash = FlashRW();
WiFiManager wm;

// Timer
double startTime;
double elapsedTime;
bool runOnce = false;

// Flags
bool registrationDone = false;
bool dataArrived = false;
bool update = false;
bool isRegistered = false;
bool isConnected = false;
std::string buttonState = "";

// websocket string
std::string rawData;
std::string message;
std::string command;
std::string response;
std::string password;
std::vector<std::string> parsecpp(std::string data, std::string delim);

// Called when receiving any WebSocket message
void onWebSocketEvent(uint8_t num,
                      WStype_t type,
                      uint8_t * payload,
                      size_t length) {

  // Figure out the type of WebSocket event
  switch(type) {

    // Client has disconnected
    case WStype_DISCONNECTED:
      if(isConnected){
        #ifdef DEBUG
          Serial.printf("[%u] Disconnected!\n", num);
        #endif
        update = true;
        isConnected = false;
      }
      break;

    // New client has connected
    case WStype_CONNECTED:
      if(!isConnected){
        IPAddress ip = websocket.remoteIP(num);
        #ifdef DEBUG
          Serial.printf("[%u] Connection from ", num);
          Serial.println(ip.toString());
        #endif
        // stop MDNS
        MDNS.close();
        update = false;
        isConnected = true;
      }
      break;

    // Echo text message back to client
    case WStype_TEXT:
       // Cast payload to string
      rawData = std::string(reinterpret_cast<char*>(const_cast<uint8_t*>(payload)));
      
      if(rawData.compare("PING") == 0){
        // Heartbeat routine
        websocket.sendTXT(num, "PONG");
      } else {
        // Call callback function to process message
        // Parse the message
        message = rawData;

        std::vector<std::string> parsedDataVector{};
        parsedDataVector.reserve(10);
        parsedDataVector = parsecpp(message, ",");

        command = parsedDataVector[0];
        
        if(command.compare("register") == 0){
          // Close MDNS immediately
          MDNS.close();

          isRegistered = true;
          response = "registered";
          password = parsedDataVector[1];
          websocket.sendTXT(num, response.c_str());
          response.clear();

          // Store password to flash
          myFlash.writeData(password, "/config.txt");

        } else if (command.compare("relay") == 0){
          if(parsedDataVector[1] == password){
            // User verified
            // Serial.println("User verified");
            isRegistered = true;
            // update relay state here
            for (size_t i = 0; i < NUM_OF_CHANNEL; i++)
            {
              myRelayState.setState(i, parsedDataVector[i+2]);
            }
            dataArrived = true;
          } else {
            // user not verified
            #ifdef DEBUG
              Serial.println("User not verified");
            #endif
          }
        } else if (command.compare("reset") == 0) {
          if(parsedDataVector[1] == password){
            // Reset here
            #ifdef DEBUG
              Serial.println("Reset requested from client");
            #endif
            isRegistered = false;

            // Remove stored data in flash
            myFlash.deleteData("/config.txt");
            myFlash.deleteData("/state.txt");

            // Wipe WiFi setting
            wm.resetSettings();

            delay(2);

            // Restart ESP
            ESP.restart();
          } else {
            // user not verified
            #ifdef DEBUG
              Serial.println("User not verified");
            #endif
          }
        }
      }
      break;

    // For everything else: do nothing
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }
}

std::vector<std::string> parsecpp(std::string data, std::string delim){
  std::vector<std::string> myVector{};
  
  myVector.reserve(NUM_OF_DATA);
  int pos = 0;

  while((pos = data.find(delim)) != std::string::npos){
    myVector.push_back(data.substr(0, pos));
    data.erase(0, pos + delim.length());
  }
  // Push last substring to vector
  myVector.push_back(data.substr(0));
  return myVector;
}

void mdnsBegin(){
  if (!MDNS.begin("esp8266")) {             // Start the mDNS responder for esp8266.local
  #ifdef DEBUG
    Serial.println("Error setting up MDNS responder!");
  #endif
  }
  #ifdef DEBUG
    Serial.println("mDNS responder started");
  #endif
  MDNS.addService("esprel", "tcp", 80);
  isRegistered = false;
  startTime = millis();
}

void setup() {
    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP

    // put your setup code here, to run once:
    Serial.begin(115200);
    
    //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
    // Turn off debug mode
    #ifndef DEBUG
      wm.setDebugOutput(false);
    #endif
    // Set IP Address
    wm.setAPStaticIPConfig(IPAddress(1,1,1,1), IPAddress(1,1,1,1), IPAddress(255,255,255,0));
    bool res;
    res = wm.autoConnect(); // auto generated AP name from chipid

    isRegistered = false;

    if(!res) {
      #ifdef DEBUG
        Serial.println("Failed to connect");
      #endif
        // ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
      #ifdef DEBUG
        Serial.println("Connected to Wi-Fi");
        Serial.println(wm.getDefaultAPName());
        Serial.println(WiFi.SSID());  
        Serial.println(WiFi.localIP());
      #endif

        // Start MDNS Service
        mdnsBegin();

        // Start WebSocket server and assign callback
        websocket.setAuthorization("dGNoLWl0IQ","kdWlubyBma");
        websocket.begin();
        websocket.onEvent(onWebSocketEvent);

        password = myFlash.readData("/config.txt");
        #ifdef DEBUG
        Serial.printf("Password is %s\n", password.c_str());
        Serial.printf("Password length is %d\n", password.length());
        #endif
        
        if(password.length() > 2){
          isRegistered = true;
        } else {
          isRegistered = false;
        }

        // Retrieve last state
        std::string stateBuffer = myFlash.readData("/state.txt");
        for(size_t i = 0; i < stateBuffer.length(); i++){
          myRelayState.setState(i, stateBuffer.substr(i,i+1));
        }
        dataArrived = true;
    }

    // Assign Pin
    for (int i = 0; i < sizeOfChannelPin; i++) {
      pinMode(channelPin[i], OUTPUT);
    }

    startTime = millis();

    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);
    update = true;    
}

void loop() {
    elapsedTime = millis() - startTime;
    bool isTimeout = (elapsedTime < 60000) ? false : true;
    if( !isTimeout && !isRegistered ){
      // If device is not registered, broadcast MDNS
      // Serial.println("Broadcasting MDNSservice");
      MDNS.update();
      runOnce = false;
    } else {
      // If registered, stop broadcasting MDNS
      if(!runOnce){
        if(isRegistered){
          #ifdef DEBUG
            Serial.println("Device registered, closing MDNS service");
          #endif
        } else if (isTimeout){
          #ifdef DEBUG
            Serial.println("Timeout, closing MDNS service");
          #endif
        }
        MDNS.close();
        runOnce = true;
      }
    }

    if(dataArrived){
      dataArrived = false;

      #ifdef DEBUG
      Serial.printf("Relay state is %d, %d, %d, %d\n", myRelayState.getState(0),
        myRelayState.getState(1), myRelayState.getState(2),
        myRelayState.getState(3));
      #endif
      digitalWrite(2, myRelayState.getState(0) ? LOW : HIGH);

      // Update each channel
      for (int i = 0; i < sizeOfChannelPin; i++) {
        // Set pin state
        digitalWrite(channelPin[i], myRelayState.getState(i));
        buttonState = buttonState + std::string(myRelayState.getState(i) ? "1" : "0");
      }

      // Store to flash
      myFlash.writeData(buttonState, "/state.txt");
      buttonState.clear();
    }

    // Websocket Loop
    websocket.loop();
}
