#ifndef RELAY_STATE_H
#define RELAY_STATE_H

#include <string>

class RelayState{
  private:
  bool channel1;
  bool channel2;
  bool channel3;
  bool channel4;

  public:
  RelayState();

  // Getter
  bool getStateChannel1();
  bool getStateChannel2();
  bool getStateChannel3();
  bool getStateChannel4();

  // Setter
  void setStateChannel1(std::string state);
  void setStateChannel2(std::string state);
  void setStateChannel3(std::string state);
  void setStateChannel4(std::string state);
};

#endif