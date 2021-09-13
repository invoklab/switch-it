#ifndef FLASHRW_H
#define FLASHRW_H

#include <string>
#include <LittleFS.h>

class FlashRW
{
private:
  File file;
  bool initSuccessful;
public:
  FlashRW();
  ~FlashRW();

  // Setter
  void writeData(std::string data);
  void deleteData();

  // Getter
  std::string readData();
  bool getInitStatus();
};

#endif

