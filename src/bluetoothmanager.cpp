#include "bluetoothmanager.hpp"
#include "ioManager.hpp"

BluetoothManager::BluetoothManager() : currentBTName("ESP32BT")
{
}

void BluetoothManager::init()
{
    ioManager::statusPrintln("[INIT] Initializing Bluetooth Manager...");
    
    // Load Bluetooth name from file if exists
    String savedName = readTextFile("/btname.txt");
    if (savedName != "")
    {
        currentBTName = savedName;
        ioManager::statusPrintln("[INIT] Bluetooth name loaded from file: " + currentBTName);
    } else {
        ioManager::statusPrintln("[INIT] Using default Bluetooth name: " + currentBTName);
    }
    
    if (btSerial.begin(currentBTName))
    {
        ioManager::statusPrintln("[INIT] Bluetooth serial started with name: " + currentBTName);
        
        // Wait for Bluetooth to be ready with timeout
        unsigned long startTime = millis();
        while (!btSerial && (millis() - startTime) < 5000) // 5 second timeout
        {
            delay(100);
        }
        
        if (btSerial)
        {
            ioManager::statusPrintln("[INIT] Bluetooth ready");
        } else {
            ioManager::statusPrintln("[ERROR] Bluetooth failed to initialize within timeout");
        }
    } else {
        ioManager::statusPrintln("[ERROR] Failed to start Bluetooth serial");
    }
}

void BluetoothManager::println(String message)
{
    btSerial.println(message);
}

bool BluetoothManager::available()
{
    return btSerial.available();
}

String BluetoothManager::readString()
{
    return btSerial.readString();
}

BluetoothSerial *BluetoothManager::getSerial()
{
    return &btSerial;
}

void BluetoothManager::setBluetoothName(String name)
{
    currentBTName = name;
    writeTextFile("/btname.txt", name.c_str());
    ioManager::statusPrintln("[INIT] Bluetooth name changed to: " + name + ". Restart required for changes to take effect.");
}

String BluetoothManager::getBluetoothName()
{
    return currentBTName;
}

void BluetoothManager::writeTextFile(const char *path, const char *text)
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

String BluetoothManager::readTextFile(const char *path)
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
