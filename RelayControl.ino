#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <ESP8266mDNS.h> 
#include <WebSocketsServer.h>
#include "RelayState.h"
#include "LittleFS.h"
#include <string>
#include <vector>

using namespace std;

#define NUM_OF_DATA 10

// Globals
WebSocketsServer websocket = WebSocketsServer(80);
RelayState myRelayState = RelayState();

double startTime;
bool runOnce = false;
bool registrationDone = false;
bool dataArrived = false;

// websocket string
string rawData;
string message;
string command;
string response;
string password;
vector<string> parsecpp(string data, string delim);



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
      break;

    // New client has connected
    case WStype_CONNECTED:
      {
        IPAddress ip = websocket.remoteIP(num);
        Serial.printf("[%u] Connection from ", num);
        Serial.println(ip.toString());
        // stop MDNS
        MDNS.close();
      }
      break;

    // Echo text message back to client
    case WStype_TEXT:
       // Cast payload to string
      rawData = string(reinterpret_cast<char*>(const_cast<uint8_t*>(payload)));
      
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

        vector<string> parsedDataVector{};
        parsedDataVector.reserve(10);
        parsedDataVector = parsecpp(message, ",");

        command = parsedDataVector[0];
        
        if(command.compare("register") == 0){
          MDNS.close();
          response = "registered";
          password = parsedDataVector[1];
          websocket.sendTXT(num, response.c_str());
          response.clear();
        } else if (command.compare("relay") == 0){
          if(parsedDataVector[1] == password){
            // User verifier
            Serial.println("User verified");
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

vector<string> parsecpp(string data, string delim){
  vector<string> myVector{};
  
  myVector.reserve(NUM_OF_DATA);
  int pos = 0;

  while((pos = data.find(delim)) != string::npos){
    myVector.push_back(data.substr(0, pos));
    data.erase(0, pos + delim.length());
  }
  // Push last substring to vector
  myVector.push_back(data.substr(0));
  return myVector;
}

void setup() {
    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP

    // put your setup code here, to run once:
    Serial.begin(115200);
    
    //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wm;

    //reset settings - wipe credentials for testing
    // wm.resetSettings();

    // Set IP Address
    wm.setAPStaticIPConfig(IPAddress(1,1,1,1), IPAddress(1,1,1,1), IPAddress(255,255,255,0));
    bool res;
    res = wm.autoConnect(); // auto generated AP name from chipid

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

        if (!MDNS.begin("esp8266")) {             // Start the mDNS responder for esp8266.local
          Serial.println("Error setting up MDNS responder!");
        }
        Serial.println("mDNS responder started");
        MDNS.addService("esp", "tcp", 80);

        // Start WebSocket server and assign callback
        websocket.begin();
        websocket.onEvent(onWebSocketEvent);

        // LittleFS Stuff
        if(!LittleFS.begin()){
          Serial.println("An Error has occurred while mounting LittleFS");
          return;
        }
        
        File file = LittleFS.open("/test.txt", "r");
        if(!file){
          Serial.println("Failed to open file for reading");
          return;
        }
        
        Serial.println("File Content:");
        while(file.available()){
          Serial.write(file.read());
        }
        file.close();
    }

    startTime = millis();

}

void loop() {
    // put your main code here, to run repeatedly:
    if(millis() - startTime < 60000){
      MDNS.update();
    } else {
      // stop mdns
      if(!runOnce){
        Serial.println("Timeout, Closing MDNS Service");
        MDNS.close();
        runOnce = true;
      }
    }

    if(dataArrived){
      dataArrived = false;
      Serial.printf("Relay state is %d, %d, %d, %d", myRelayState.getStateChannel1(),
        myRelayState.getStateChannel2(), myRelayState.getStateChannel3(),
        myRelayState.getStateChannel4());
    }
    
    websocket.loop();
    
}
