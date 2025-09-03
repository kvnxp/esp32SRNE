#include "src/App.h"
#include "src/srnemodbus.hpp"
#include "HardwareSerial.h"

#define printl ioManager::statusPrintln

App app;
srnemodbus modbus;

void setup()
{
  printl("[INIT] ===== ESP32 SRNE Controller Starting =====");
  
  App::init();
  printl("[INIT] App initialization complete");
  
  modbus.setup();
  printl("[INIT] Modbus initialization complete");
  
  printl("[INIT] ===== Initialization Complete =====");
}

void loop()
{
  app.loop();
  modbus.loop();

  delay(200);
}
