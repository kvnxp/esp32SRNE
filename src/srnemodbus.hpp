#pragma once
#include <ModbusMaster.h>
#include "ArduinoJson.h"
#include "HardwareSerial.h"
#include "ioManager.hpp"

#define printl ioManager::statusPrintln



class srnemodbus
{
private:
    /* data */
public:
    void setup();
    void loop();
};
