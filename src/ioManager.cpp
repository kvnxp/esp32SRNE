#include "ioManager.hpp"

BluetoothSerial btSerial;

// #define Serial btSerial

void ioManager::init()
{
    Serial0.begin(115200);
    btSerial.begin("ESP32BT");
    while (!Serial)
    {
        ; // wait for serial port to connect. Needed for native USB port only
    }

    while (!btSerial)
    {
        ;
    }
    Serial0.println("Serial port initialized");

    // serialOut.println("Serial port initialized");
}

void ioManager::println(String message)
{
    btSerial.println(message);
    Serial0.println(message);
}

String ioManager::getInput()
{
    String data = "";
    String cache = "";
    bool working = false;

    // int bInput = btSerial.read();

    if (Serial0.available())
    {
        cache = Serial0.readString();
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

    // while (sInput > 0 && Serial0.available())
    // {
    //     if ((sInput == '\n' || sInput == '\r') && !full)
    //     {
    //         break;
    //     }

    //     data += (char)sInput;
    //     char a = (char)sInput;
    //     println(String(a));
    //     sInput = Serial.read();
    // }

    // if (bInput > 0 && btSerial.available())
    // {
    //     working = true;
    //     while (bInput > 0 && btSerial.available())
    //     {
    //         if ((bInput == '\n' || bInput == '\r') && !full)
    //         {
    //             break;
    //         }

    //         data += (char)bInput;
    //         bInput = Serial.read();
    //     }
    //     working = false;
    // }

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