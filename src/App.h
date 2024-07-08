#include "wifi/wifiManager.hpp"
#include "ioManager.hpp"
#include <vector>
#define vector std::vector
#pragma once

class App
{
private:
    /* data */
    static int currentPage;

public:
    static void init();
    void loop();
    void menuSelector(int menu);
    void MainMenu();
    void wifiMenu();
    void miscMenu();

    // void waitForInput();
};
