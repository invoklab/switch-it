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
  void writeData(std::string data, std::string path);
  void deleteData(std::string path);

  // Getter
  std::string readData(std::string path);
  bool getInitStatus();
};

#endif

