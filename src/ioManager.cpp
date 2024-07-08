#include "ioManager.hpp"
#include "SPIFFS.h"

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

    if (!SPIFFS.begin(true))
    {
        println("SPIFFS Mount Failed");
    }
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

void ioManager::writeTextFile(const char *path, const char *text)
{

    File file = SPIFFS.open(path);
    if (!file)
    {
        println("Error to write file ");
        return;
    }
    file.print(text);
    file.close();
}

String ioManager::readTextFile(const char *path)
{
    File file = SPIFFS.open(path);
    if (!file)
    {
        println("Error to read file ");
        return "";
    }
    String data = file.readString();
    file.close();
    return data;
}

void ioManager::sendToHttpClient(String data)
{

    HTTPClient client;

    String url = "";
    String token = "";
    client.begin(url);
    client.addHeader("X-Auth-Token", token);
    client.addHeader("Content-Type", "application/json");

    int result = client.POST(data);
    if (result > 0)
    {
        ioManager::println("HTTP Result " + String(result));
        ioManager::println(client.getString());
    }
    else
    {
        println("error to send ");
        println(String(result));
        println(String(client.getString()));
    }
    client.end();
}