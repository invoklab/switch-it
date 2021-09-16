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

void FlashRW::writeData(std::string data, std::string path) {
  //Open the file
  this->file = LittleFS.open(path.c_str(), "w");
  //Write to the file
  file.print(data.c_str());
  //Close the file
  file.close();
  #ifdef DEBUG
    Serial.println("Write successful");
  #endif
}

std::string FlashRW::readData(std::string path){
  this->file = LittleFS.open(path.c_str(), "r");
  if(!file){
    #ifdef DEBUG
      Serial.println("Failed to open file for reading");
    #endif
  }
  
  #ifdef DEBUG
    Serial.println("File Content:");
  #endif
  std::string buffer = "";
  char charBuffer;
  while(file.available()){
    // Assign password string here
    charBuffer = file.read();
    buffer.insert(buffer.end(),charBuffer);
  }
  file.close();
  return buffer;
}

void FlashRW::deleteData(std::string path){
  //Open the file
  this->file = LittleFS.open(path.c_str(), "w");
  //Write to the file
  file.print("");
  //Close the file
  file.close();
  #ifdef DEBUG
    Serial.println("Write successful");
  #endif
}

bool FlashRW::getInitStatus(){
  return this->initSuccessful;
}
