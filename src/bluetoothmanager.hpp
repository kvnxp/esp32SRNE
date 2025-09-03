#pragma once
#include "BluetoothSerial.h"
#include <WString.h>
#include "SPIFFS.h"

// Forward declaration
class ioManager;

class BluetoothManager
{
private:
    BluetoothSerial btSerial;
    String currentBTName;

public:
    BluetoothManager();
    void init();
    void println(String message);
    bool available();
    String readString();
    BluetoothSerial *getSerial();
    void setBluetoothName(String name);
    String getBluetoothName();
    void writeTextFile(const char *path, const char *text);
    String readTextFile(const char *path);
};
