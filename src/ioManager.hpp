#include <HardwareSerial.h>
#include "BluetoothSerial.h"
#include "HardwareSerial.h"
#include <WString.h>
#include "SPIFFS.h"
#include <HTTPClient.h>

#pragma once

class ioManager
{
private:
public:
    static void init();
    static void println(String message);
    static String getInput();
    static String waitForInput();
    static int waitNumberInput();
    static BluetoothSerial *getSerialBT();
    static void writeTextFile(const char *path, const char *text);
    static String readTextFile(const char *path);
    static void sendToHttpClient(String data);
};
