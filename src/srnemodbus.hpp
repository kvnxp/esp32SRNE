#pragma once
#include <ModbusMaster.h>
#include "ArduinoJson.h"
#include "HardwareSerial.h"
#include "ioManager.hpp"

#define printl ioManager::println



class srnemodbus
{
private:
    /* data */
public:
    void setup();
    void loop();
};
