#include "ioManager.hpp"

BluetoothSerial btSerial;
HardwareSerial serial(0);

// #define Serial btSerial

void ioManager::init()
{
    serial.begin(115200);
    btSerial.begin("ESP32BT");
    while (!serial)
    {
        ; // wait for serial port to connect. Needed for native USB port only
    }

    while (!btSerial)
    {
        ;
    }
    serial.println("Serial port initialized");

    // serialOut.println("Serial port initialized");
}

void ioManager::println(String message)
{
    btSerial.println(message);
    serial.println(message);
}

String ioManager::getInput()
{
    String data = "";
    String cache = "";
    bool working = false;

    // int bInput = btSerial.read();

    if (serial.available())
    {
        cache = serial.readString();
    }

    if (btSerial.available())
    {
        cache = btSerial.readString();
    }

    for (size_t i = 0; i < cache.length(); i++)
    {
        int caracter = cache[i];
        if (caracter != 10 && caracter != 13)
        {
            data += (char)caracter;
        }
    }

    return data;
}

String ioManager::waitForInput()
{
    println("Ingresa una opcion:");
    bool wait = true;
    String message = "";
    while (wait)
    {
        String input = ioManager::getInput();
        if (input != "")
        {
            message = input;
            wait = false;
        }
    }
    return message;
}

int ioManager::waitNumberInput()
{
    String data = ioManager::waitForInput();
    int number = data.toInt();
    return number;
}

BluetoothSerial *ioManager::getSerialBT()
{
    return &btSerial;
}