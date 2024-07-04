#include "WiFi.h"
#include "../ioManager.hpp"
#include "ArduinoJson.h"
#define printl ioManager::println

class WifiManager
{
private:
public:
    WifiManager(){
        // autoConectWifi();
    }
    void inputSSID();
    void inputPassword();
    void connect();
    void getStatus();
    void saveProfile();
    void loadProfile();
    void autoConectWifi();
};
