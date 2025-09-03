#include "wifi/wifiManager.hpp"
#include "ioManager.hpp"
#include <vector>
#define vector std::vector
#pragma once

enum MenuState { MAIN_MENU, WIFI_MENU, MISC_MENU, BLUETOOTH_MENU };

class App
{
private:
    /* data */
    static MenuState currentState;
    static bool menuActive;
    static bool confirmingExit;
    static String pendingInput;
    static bool waitingForBTName;

public:
    static void init();
    void loop();
    void processInput();
    void handleMainMenu(int selection);
    void handleWifiMenu(int selection);
    void handleMiscMenu(int selection);
    void handleBluetoothMenu(int selection);
    void menuSelector(int menu);
    void MainMenu();
    void wifiMenu();
    void miscMenu();
    void bluetoothMenu();

    // void waitForInput();
};
