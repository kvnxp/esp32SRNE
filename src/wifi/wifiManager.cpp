#include "wifiManager.hpp"
#include "ArduinoJson.h"
using namespace ArduinoJson;

String ssid = "";
String password = "";
String wifiFileName ="wifiProfile.json";


void WifiManager::autoConectWifi()
{
    loadProfile();
    WiFi.begin(ssid, password);
}

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

void WifiManager::inputSSID()
{
    printl("current SSID: " + ssid);
    printl("ingress SSID Name");
    ssid = ioManager::waitForInput();
}

void WifiManager::inputPassword()
{
    printl("ingress Password");
    password = ioManager::waitForInput();
}

void WifiManager::connect()
{
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
}

void WifiManager::getStatus()
{
    wifiStatus(WiFi.status());
    printl("");
    printl("");
}

void WifiManager::saveProfile()
{
    JsonDocument doc;
    doc["ssid"] = ssid;
    doc["password"] = password;

    File file = SPIFFS.open("/"+wifiFileName, "w");
    if (!file)
    {
        printl("Failed to open file for writing");
        return;
    }

    serializeJson(doc, file);
    file.close();
    printl("Profile saved");
}

void WifiManager::loadProfile()
{
    File file = SPIFFS.open("/"+wifiFileName, "r");
    if (!file)
    {
        printl("Failed to open file for reading");
        return;
    }

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, file);
    if (error)
    {
        printl("Failed to read file, using default configuration");
        return;
    }

    ssid = doc["ssid"].as<String>();
    password = doc["password"].as<String>();
    printl("Profile loaded");
    printl("SSID: " + ssid);
    file.close();
}