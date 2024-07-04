#include "WiFi.h"
#include "../ioManager.hpp"
#define printl ioManager::println

class wifiManager
{
private:
public:
    void inputSSID();
    void inputPassword();
    void connect();
    void getStatus();
};
