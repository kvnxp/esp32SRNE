#include "ioManager.hpp"
#include "SPIFFS.h"

BluetoothManager ioManager::bluetoothManager;
bool ioManager::menuActive = false;
HardwareSerial serial(0);

// #define Serial btSerial

void ioManager::init()
{
    Serial.begin(115200);
    println("[INIT] Initializing I/O Manager...");
    
    // serial.begin(115200);
    println("[INIT] Serial port initialized at 115200 baud");
    
    // Initialize SPIFFS first before Bluetooth Manager
    if (!SPIFFS.begin(true))
    {
        println("SPIFFS Mount Failed");
    } else {
        println("[INIT] SPIFFS file system mounted successfully");
    }
    
    bluetoothManager.init();
    println("[INIT] Bluetooth Manager initialized");
}

void ioManager::println(String message)
{
    // Only print background messages if menu is not active
    if (!menuActive) {
        bluetoothManager.println(message);
        Serial.println(message);
    }
}

void ioManager::statusPrintln(String message)
{
    // Always print status messages (errors, debug, important notifications)
    bluetoothManager.println(message);
    Serial.println(message);
}

void ioManager::menuPrintln(String message)
{
    // Always print menu messages
    bluetoothManager.println(message);
    Serial.println(message);
}

void ioManager::setMenuActive(bool active)
{
    menuActive = active;
}

bool ioManager::isMenuActive()
{
    return menuActive;
}

String ioManager::getInput()
{
    String data = "";
    String cache = "";
    bool working = false;

    // int bInput = btSerial.read();

    if (Serial.available())
    {
        cache = Serial.readString();
    }

    if (bluetoothManager.available())
    {
        cache = bluetoothManager.readString();
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
    return bluetoothManager.getSerial();
}

void ioManager::writeTextFile(const char *path, const char *text)
{
    File file = SPIFFS.open(path, FILE_WRITE);
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
    File file = SPIFFS.open(path, FILE_READ);
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

void ioManager::setBluetoothName(String name)
{
    bluetoothManager.setBluetoothName(name);
}

String ioManager::getBluetoothName()
{
    return bluetoothManager.getBluetoothName();
}