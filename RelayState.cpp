#include "RelayState.h"

RelayState::RelayState(){};

// Getter
bool RelayState::getState(int whichChannel){
  return this->state[whichChannel];
}


// Setter
void RelayState::setState(int whichChannel, std::string state){
  this->state[whichChannel] = stoi(state);
}