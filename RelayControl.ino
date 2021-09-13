#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <ESP8266mDNS.h> 
#include <WebSocketsServer.h>
#include "RelayState.h"
#include "FlashRW.h"
#include <string>
#include <vector>

#define NUM_OF_DATA 10

// Globals
WebSocketsServer websocket = WebSocketsServer(80);
RelayState myRelayState = RelayState();
FlashRW myFlash = FlashRW();
WiFiManager wm;

double startTime;
double elapsedTime;
bool runOnce = false;
bool registrationDone = false;
bool dataArrived = false;
bool update = false;
bool isRegistered = false;
bool isConnected = false;

// websocket string
std::string rawData;
std::string message;
std::string command;
std::string response;
std::string password;
std::vector<std::string> parsecpp(std::string data, std::string delim);

// LittleFS

// Called when receiving any WebSocket message
void onWebSocketEvent(uint8_t num,
                      WStype_t type,
                      uint8_t * payload,
                      size_t length) {

  // Figure out the type of WebSocket event
  switch(type) {

    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      update = true;
      isConnected = false;
      mdnsBegin();
      break;

    // New client has connected
    case WStype_CONNECTED:
      {
        if(!isConnected){
          IPAddress ip = websocket.remoteIP(num);
          Serial.printf("[%u] Connection from ", num);
          Serial.println(ip.toString());
          // stop MDNS
          MDNS.close();
          update = false;
          isConnected = true;
        }
        
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

        #ifdef DEBUG
          Serial.printf("Message data is %s \n", this->message.c_str());
        #endif

        std::vector<std::string> parsedDataVector{};
        parsedDataVector.reserve(10);
        parsedDataVector = parsecpp(message, ",");

        command = parsedDataVector[0];
        
        if(command.compare("register") == 0){

          isRegistered = true;
          response = "registered";
          password = parsedDataVector[1];
          websocket.sendTXT(num, response.c_str());
          response.clear();

          // Store password to flash
          myFlash.writeData(password);

        } else if (command.compare("relay") == 0){
          if(parsedDataVector[1] == password){
            // User verifier
            // Serial.println("User verified");
            // update relay state here
            myRelayState.setStateChannel1(parsedDataVector[2]);
            myRelayState.setStateChannel2(parsedDataVector[3]);
            myRelayState.setStateChannel3(parsedDataVector[4]);
            myRelayState.setStateChannel4(parsedDataVector[5]);
            dataArrived = true;
          } else {
            // user not verified
            Serial.println("User not verified");
          }
        } else if (command.compare("reset") == 0) {
          if(parsedDataVector[1] == password){
            // Reset here
            Serial.println("Reset reqeusted from client");
            isRegistered = false;

            // Restart mdns, make it discoverable again
            mdnsBegin();

            // Remove stored data in flash
            myFlash.writeData("");

            // wm.resetSettings();

            // delay(2);

            // Restart ESP
            // ESP.restart();
          } else {
            // user not verified
            Serial.println("User not verified");
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
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started");
  MDNS.addService("esprel", "tcp", 80);
}

void setup() {
    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP

    // put your setup code here, to run once:
    Serial.begin(115200);
    
    //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
    

    //reset settings - wipe credentials for testing
    // wm.resetSettings();

    // Set IP Address
    wm.setAPStaticIPConfig(IPAddress(1,1,1,1), IPAddress(1,1,1,1), IPAddress(255,255,255,0));
    bool res;
    res = wm.autoConnect(); // auto generated AP name from chipid

    isRegistered = false;

    if(!res) {
        Serial.println("Failed to connect");
        // ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("Connected to Wi-Fi");
        Serial.println(wm.getDefaultAPName());
        Serial.println(WiFi.SSID());  
        Serial.println(WiFi.localIP());

        mdnsBegin();

        // Start WebSocket server and assign callback
        websocket.begin();
        websocket.onEvent(onWebSocketEvent);

        password = myFlash.readData();
        Serial.printf("Password is %s\n", password.c_str());

        isRegistered = true;
    }

    startTime = millis();

    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);
    update = true;

}

void loop() {

    elapsedTime = millis() - startTime;
    if(( elapsedTime < 60000 ) && !isRegistered ){
      // If device is not registered, broadcast MDNS
      MDNS.update();
    } else {
      // If registered, stop broadcasting MDNS
      if(!runOnce){
        if(isRegistered){
          Serial.println("Device registered, closing MDNS service");
        } else if (elapsedTime < 60000){
          Serial.println("Timeout, closing MDNS service");
        }
        MDNS.close();
        runOnce = true;
      }
    }

    if(dataArrived){
      dataArrived = false;
      Serial.printf("Relay state is %d, %d, %d, %d\n", myRelayState.getStateChannel1(),
        myRelayState.getStateChannel2(), myRelayState.getStateChannel3(),
        myRelayState.getStateChannel4());
      digitalWrite(2, myRelayState.getStateChannel1() ? LOW : HIGH);
    }

    // Websocket Loop
    websocket.loop();
}
