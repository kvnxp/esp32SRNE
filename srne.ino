#include "src/App.h"
#include "src/srnemodbus.hpp"
#include "HardwareSerial.h"
#include <SPIFFS.h>

#define printl ioManager::println

App app;
// srnemodbus modbus;



void setup()
{
  App::init();
  // modbus.setup();

  if (!SPIFFS.begin(true)){
    printl("Fail to format fs");
  }

 File fil =  SPIFFS.open("/","test.txt",true);
 if (!fil){
  printl("error to open file");
 }else 
 {
  if (fil.print("hola")){
    printl("message writed");
  }else {
    printl("error to write");
  }
  
 }
 

}

void loop()
{
  app.loop();
  // modbus.loop();

  delay(200);
}
