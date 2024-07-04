#include "App.h"
#define printl ioManager::println

int App::currentPage;
WifiManager wifiManager;

void showMenu(vector<String> menu)
{
    for (size_t i = 0; i < menu.size(); i++)
    {
        printl(String(i) + ": " + menu[i]);
    }
}

void App::init()
{
    ioManager::init();
    currentPage = -1;
    wifiManager.autoConectWifi();
}

void App::loop()
{
    String input = ioManager::getInput();
    if (input != "")
    {
        menuSelector(currentPage);
    }
}

void App::menuSelector(int menu)
{
    if (menu == -1)
    {
        MainMenu();
    }
    else
    {
        switch (menu)
        {
        case 0:
            currentPage = -1;
            break;

        case 1:
            wifiMenu();
            break;
        case 2:
            miscMenu();
        default:
            menuSelector(currentPage);
            break;
        }
    }
}

void App::MainMenu()
{
    currentPage = 0;

    printl("\nMainMenu\n");

    vector<String> menuItems = {"exit", "wifi", "config"};

    showMenu(menuItems);

    int inputNumber = ioManager::waitNumberInput();
    menuSelector(inputNumber);
}

void App::wifiMenu()
{
    currentPage = 1;
    printl("\n Wifi Menu \n");

    vector<String> menuItems = {"Back", "SSID", "password", "connect", "status"};

    showMenu(menuItems);

    int input = ioManager::waitNumberInput();

    switch (input)
    {
    case 0: // back
        MainMenu();
        break;

    case 1: // input SSID
        wifiManager.inputSSID();
        wifiMenu();
        break;

    case 2: // input password
        wifiManager.inputPassword();
        wifiMenu();
        break;

    case 3: // menu connect
        wifiManager.connect();
        wifiMenu();
        break;

    case 4: // wifi Status
        wifiManager.getStatus();
        printl("any key to back");
        if (ioManager::waitForInput() != "")
        {
            wifiMenu();
        }
        break;

    default: // any key to reload menu
        menuSelector(currentPage);
        break;
    }
}

void App::miscMenu()
{
    printl("Misc Menu");
    vector<String> menuItems = {"Back", "saveProfile", "loadProfile", "deleteProfile"};

    showMenu(menuItems);

    int select = ioManager::waitNumberInput();

    switch (select)
    {
    case 0:
        MainMenu();
    case 1:
        wifiManager.saveProfile();
        break;
    case 2:
        wifiManager.loadProfile();
        break;
    }
}