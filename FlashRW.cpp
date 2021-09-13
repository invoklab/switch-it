#include "FlashRW.h"

FlashRW::FlashRW(/* args */)
{
  if(LittleFS.begin()){
    this->initSuccessful = true;
  };
}

FlashRW::~FlashRW()
{
}

void FlashRW::writeData(std::string data) {
  //Open the file
  this->file = LittleFS.open("/config.txt", "w");
  //Write to the file
  file.print(data.c_str());
  // delay(2);
  //Close the file
  file.close();
  Serial.println("Write successful");
}

std::string FlashRW::readData(){
  this->file = LittleFS.open("/config.txt", "r");
  if(!file){
    Serial.println("Failed to open file for reading");
  }
  
  Serial.println("File Content:");
  std::string buffer = "";
  char charBuffer;
  while(file.available()){
    // Assign password string here
    // Serial.write(file.read());
    charBuffer = file.read();
    buffer.insert(buffer.end(),charBuffer);
  }
  file.close();
  return buffer;
}

void FlashRW::deleteData(){
  //Open the file
  this->file = LittleFS.open("/config.txt", "w");
  //Write to the file
  file.print("");
  // delay(2);
  //Close the file
  file.close();
  Serial.println("Write successful");
}

bool FlashRW::getInitStatus(){
  return this->initSuccessful;
}
