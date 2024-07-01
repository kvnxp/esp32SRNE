#include "App.h"
#include <WiFi.h>
#define printl ioManager::println

int App::currentPage;
String ssid = "";
String password = "";

wl_status_t wifiSetup()
{
    return WiFi.begin(ssid, password);
}

void wifiStatus(wl_status_t wstatus)
{
    printl("Status CODE:" + String(wstatus));
    switch (wstatus)
    {
    case WL_STOPPED:
        printl("[WiFi] Stoped");
        break;
    case WL_NO_SSID_AVAIL:
        printl("[WiFi] SSID not found");
        break;
    case WL_CONNECT_FAILED:
        printl("[WiFi] Failed - WiFi not connected! Reason: ");
        break;
    case WL_CONNECTION_LOST:
        printl("[WiFi] Connection was lost");
        break;
    case WL_SCAN_COMPLETED:
        printl("[WiFi] Scan is completed");
        break;
    case WL_DISCONNECTED:
        printl("[WiFi] WiFi is disconnected");
        break;
    case WL_CONNECTED:
        printl("[WiFi] WiFi is connected!");
        printl("SSID: " + ssid);
        printl("[WiFi] IP address: " + WiFi.localIP().toString());
        break;
    default:
        printl("[WiFi] WiFi Status: ");
        printl(String(WiFi.status()));

        break;
    }
}

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
        printl("Input: " + input);
        menuSelector(currentPage);
    }
}

void App::menuSelector(int menu)
{
    printl("current page: " + String(currentPage));

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
        printl("current SSID: " + ssid);
        printl("ingress SSID Name");
        ssid = ioManager::waitForInput();
        menuSelector(currentPage);
        break;

    case 2: // input password
        printl("ingress Password");
        password = ioManager::waitForInput();
        menuSelector(currentPage);
        break;

    case 3: // menu connect
        printl("connect with network:");
        printl("SSID: " + ssid);
        printl("passwd: " + password);
        printl("");
        printl(" yes[1]  no[2]");
        printl("");

        if (ioManager::waitNumberInput() == 1)
        {
            printl("connecting...");
            wifiStatus(wifiSetup());
        }
        wifiMenu();
        break;

    case 4:// wifi Status
        wifiStatus(WiFi.status());
        printl("");
        printl("");
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