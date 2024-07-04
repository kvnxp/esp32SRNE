#include "App.h"
#define printl ioManager::println

int App::currentPage;
wifiManager wifiManager;

void App::init()
{
    ioManager::init();
    currentPage = -1;
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

    for (size_t i = 0; i < menuItems.size(); i++)
    {
        printl(String(i) + ": " + menuItems[i]);
    }

    int inputNumber = ioManager::waitNumberInput();
    menuSelector(inputNumber);
}

void App::wifiMenu()
{
    currentPage = 1;
    printl("\n Wifi Menu \n");

    vector<String> menuItems = {"Back", "SSID", "password", "connect", "status"};

    int i = 0;
    for (const String &item : menuItems)
    {
        printl(String(i) + ": " + menuItems[i]);
        i++;
    }

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

void App::misc()
{
}