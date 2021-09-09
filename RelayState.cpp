#include "RelayState.h"

RelayState::RelayState(){};

// Getter
bool RelayState::getStateChannel1(){
  return this->channel1;
}

bool RelayState::getStateChannel2(){
  return this->channel2;
}

bool RelayState::getStateChannel3(){
  return this->channel3;
}

bool RelayState::getStateChannel4(){
  return this->channel4;
}


// Setter
void RelayState::setStateChannel1(std::string state){
  this->channel1 = stoi(state);
}

void RelayState::setStateChannel2(std::string state){
  this->channel2 = stoi(state);
}

void RelayState::setStateChannel3(std::string state){
  this->channel3 = stod(state);
}

void RelayState::setStateChannel4(std::string state){
  this->channel4 = stoi(state);
}