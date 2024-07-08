#include "src/App.h"
#include "src/srnemodbus.hpp"
#include "HardwareSerial.h"

#define printl ioManager::println

App app;
srnemodbus modbus;

void setup()
{
  App::init();
  modbus.setup();
}

void loop()
{
  app.loop();
  modbus.loop();

  delay(200);
}
