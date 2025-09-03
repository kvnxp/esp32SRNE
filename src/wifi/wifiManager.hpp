#include "WiFi.h"
#include "../ioManager.hpp"
#include "ArduinoJson.h"

#define printl ioManager::menuPrintln

class WifiManager
{
private:
    String ssid;
    String password;
    String wifiFileName = "wifiProfile.json";

public:
    WifiManager()
    {
        ssid = "";
        password = "";
    }
    void inputSSID();
    void inputPassword();
    void connect();
    void getStatus();
    void saveProfile();
    void loadProfile();
    void autoConectWifi();
    bool isConnected();
    void checkAndReconnect();
    void reconnect();
    void quickSetup();
};
