#include <HardwareSerial.h>
#include "BluetoothSerial.h"
#include <WString.h>

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
};
