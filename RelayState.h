#ifndef RELAY_STATE_H
#define RELAY_STATE_H

#include <string>

class RelayState{
  private:
  bool state[4];

  public:
  RelayState();

  // Getter
  bool getState(int whichChannel);

  // Setter
  void setState(int whichChannel, std::string state);
};

#endif