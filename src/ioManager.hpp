#include <HardwareSerial.h>
#include "BluetoothSerial.h"
#include "HardwareSerial.h"
#include <WString.h>
#include "SPIFFS.h"
#include <HTTPClient.h>
#include "bluetoothmanager.hpp"

#pragma once

class ioManager
{
private:
    static BluetoothManager bluetoothManager;
    static bool menuActive;
public:
    static void init();
    static void println(String message);
    static void menuPrintln(String message);
    static void statusPrintln(String message);
    static void setMenuActive(bool active);
    static bool isMenuActive();
    static String getInput();
    static String waitForInput();
    static int waitNumberInput();
    static BluetoothSerial *getSerialBT();
    static void writeTextFile(const char *path, const char *text);
    static String readTextFile(const char *path);
    static void sendToHttpClient(String data);
    static void setBluetoothName(String name);
    static String getBluetoothName();
};
