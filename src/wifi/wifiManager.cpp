#include "wifiManager.hpp"
#include "ArduinoJson.h"
using namespace ArduinoJson;

void WifiManager::autoConectWifi()
{
    ioManager::statusPrintln("[INIT] Starting WiFi auto-connect...");
    
    loadProfile();
    if (ssid != "")
    {
        ioManager::statusPrintln("[INIT] Connecting to saved WiFi network: " + ssid);
        WiFi.begin(ssid.c_str(), password.c_str());
        
        // Wait up to 10 seconds for connection
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 10) {
            delay(1000);
            attempts++;
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            ioManager::statusPrintln("[INIT] WiFi connected successfully!");
            ioManager::statusPrintln("[INIT] IP address: " + WiFi.localIP().toString());
        } else {
            ioManager::statusPrintln("[INIT] WiFi connection failed or timed out");
        }
    } else {
        ioManager::statusPrintln("[INIT] No saved WiFi profile found, skipping auto-connect");
    }
}

void wifiStatus(wl_status_t wstatus)
{
    printl("Status CODE:" + String(wstatus));
    switch (wstatus)
    {
    case WL_STOPPED:
        printl("[WiFi] Stopped");
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
        printl("SSID: " + WiFi.SSID());
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
    printl("Current SSID: " + (ssid == "" ? "Not set" : ssid));
    printl("Enter new SSID (or press enter to keep current):");
    String input = ioManager::waitForInput();
    if (input != "")
    {
        ssid = input;
        printl("SSID set to: " + ssid);
        saveProfile(); // Automatically save when SSID is changed
        printl("Settings saved automatically.");
    }
    else
    {
        printl("SSID unchanged.");
    }
}

void WifiManager::inputPassword()
{
    printl("Enter password:");
    password = ioManager::waitForInput();
    printl("Password set.");
    saveProfile(); // Automatically save when password is changed
    printl("Settings saved automatically.");
}

void WifiManager::connect()
{
    if (ssid == "")
    {
        printl("SSID not set. Please set SSID first.");
        return;
    }
    printl("Connecting to " + ssid + "...");
    wl_status_t status = WiFi.begin(ssid.c_str(), password.c_str());
    wifiStatus(status);
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

    File file = SPIFFS.open("/" + wifiFileName, "w");
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
    if (!SPIFFS.exists("/" + wifiFileName))
    {
        ioManager::statusPrintln("[INIT] WiFi profile file does not exist");
        return;
    }

    File file = SPIFFS.open("/" + wifiFileName, "r");
    if (!file)
    {
        ioManager::statusPrintln("[INIT] Failed to open WiFi profile file for reading");
        return;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error)
    {
        ioManager::statusPrintln("[INIT] Failed to parse WiFi profile JSON");
        return;
    }

    if (doc.containsKey("ssid"))
    {
        ssid = doc["ssid"].as<String>();
    }
    if (doc.containsKey("password"))
    {
        password = doc["password"].as<String>();
    }

    ioManager::statusPrintln("[INIT] WiFi profile loaded successfully");
    if (ssid != "")
    {
        ioManager::statusPrintln("[INIT] Loaded SSID: " + ssid);
    }
}

void WifiManager::reconnect()
{
    if (ssid != "")
    {
        printl("Attempting to reconnect to " + ssid + "...");
        WiFi.disconnect();
        delay(1000);
        WiFi.begin(ssid.c_str(), password.c_str());
    }
}

bool WifiManager::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

void WifiManager::checkAndReconnect()
{
    if (!isConnected() && ssid != "")
    {
        ioManager::println("[WiFi] Connection lost, attempting to reconnect...");
        reconnect();
        
        // Wait a bit for connection attempt
        int attempts = 0;
        while (!isConnected() && attempts < 10)
        {
            delay(1000);
            attempts++;
        }
        
        if (isConnected())
        {
            ioManager::println("[WiFi] Reconnected successfully!");
            wifiStatus(WL_CONNECTED);
        }
        else
        {
            ioManager::println("[WiFi] Failed to reconnect after 10 attempts");
        }
    }
}

void WifiManager::quickSetup()
{
    printl("\n=== WiFi Quick Setup ===");
    printl("Scanning for available networks...");
    
    // Start WiFi scan
    int numNetworks = WiFi.scanNetworks();
    
    if (numNetworks == 0) {
        printl("No networks found. Please try again.");
        return;
    }
    
    printl("Found " + String(numNetworks) + " networks:");
    
    // Display networks with pagination
    const int networksPerPage = 8;
    int currentPage = 0;
    int totalPages = (numNetworks + networksPerPage - 1) / networksPerPage;
    
    while (true) {
        printl("\n--- Page " + String(currentPage + 1) + "/" + String(totalPages) + " ---");
        
        int startIdx = currentPage * networksPerPage;
        int endIdx = min(startIdx + networksPerPage, numNetworks);
        
        for (int i = startIdx; i < endIdx; i++) {
            String security = "";
            switch (WiFi.encryptionType(i)) {
                case WIFI_AUTH_OPEN:
                    security = "(Open)";
                    break;
                case WIFI_AUTH_WEP:
                    security = "(WEP)";
                    break;
                case WIFI_AUTH_WPA_PSK:
                    security = "(WPA)";
                    break;
                case WIFI_AUTH_WPA2_PSK:
                    security = "(WPA2)";
                    break;
                case WIFI_AUTH_WPA_WPA2_PSK:
                    security = "(WPA/WPA2)";
                    break;
                case WIFI_AUTH_WPA2_ENTERPRISE:
                    security = "(WPA2-Enterprise)";
                    break;
                default:
                    security = "(Unknown)";
                    break;
            }
            printl(String(i) + ": " + WiFi.SSID(i) + " " + security + " (RSSI: " + String(WiFi.RSSI(i)) + "dBm)");
        }
        
        printl("\nCommands:");
        if (currentPage > 0) printl("  'p' - Previous page");
        if (currentPage < totalPages - 1) printl("  'n' - Next page");
        printl("  'r' - Rescan networks");
        printl("  'c' - Cancel");
        printl("  Or enter network number (0-" + String(numNetworks - 1) + ") to select:");
        
        String input = ioManager::waitForInput();
        
        if (input == "c" || input == "C") {
            printl("Quick setup cancelled.");
            return;
        } else if (input == "r" || input == "R") {
            printl("Rescanning networks...");
            numNetworks = WiFi.scanNetworks();
            if (numNetworks == 0) {
                printl("No networks found. Please try again.");
                return;
            }
            currentPage = 0;
            totalPages = (numNetworks + networksPerPage - 1) / networksPerPage;
            continue;
        } else if (input == "p" || input == "P") {
            if (currentPage > 0) {
                currentPage--;
            } else {
                printl("Already on first page.");
            }
            continue;
        } else if (input == "n" || input == "N") {
            if (currentPage < totalPages - 1) {
                currentPage++;
            } else {
                printl("Already on last page.");
            }
            continue;
        } else {
            // Try to parse as network index
            int selectedNetwork = input.toInt();
            if (selectedNetwork >= 0 && selectedNetwork < numNetworks) {
                // Network selected, proceed to password input
                String selectedSSID = WiFi.SSID(selectedNetwork);
                printl("\nSelected network: " + selectedSSID);
                
                // Check if network is open (no password needed)
                if (WiFi.encryptionType(selectedNetwork) == WIFI_AUTH_OPEN) {
                    printl("This is an open network (no password required).");
                    password = "";
                } else {
                    printl("Enter password for '" + selectedSSID + "':");
                    password = ioManager::waitForInput();
                }
                
                ssid = selectedSSID;
                
                // Verify information
                printl("\n=== Verification ===");
                printl("SSID: " + ssid);
                if (password != "") {
                    printl("Password: " + String(password.length()) + " characters");
                } else {
                    printl("Password: (Open network)");
                }
                
                printl("\nCommands:");
                printl("  'y' - Connect to this network");
                printl("  'n' - Cancel and return to network list");
                printl("  'c' - Cancel quick setup");
                printl("Confirm connection (y/n/c):");
                
                String confirmInput = ioManager::waitForInput();
                
                if (confirmInput == "y" || confirmInput == "Y") {
                    printl("Connecting to " + ssid + "...");
                    wl_status_t status = WiFi.begin(ssid.c_str(), password.c_str());
                    
                    // Wait for connection
                    int attempts = 0;
                    while (WiFi.status() != WL_CONNECTED && attempts < 15) {
                        delay(1000);
                        attempts++;
                        printl("Attempting connection... (" + String(attempts) + "/15)");
                    }
                    
                    if (WiFi.status() == WL_CONNECTED) {
                        printl("Successfully connected!");
                        wifiStatus(WL_CONNECTED);
                        
                        // Save profile
                        printl("Saving WiFi profile...");
                        saveProfile();
                        printl("Profile saved successfully.");
                    } else {
                        printl("Failed to connect to " + ssid);
                        wifiStatus(WiFi.status());
                    }
                    return;
                } else if (confirmInput == "n" || confirmInput == "N") {
                    printl("Returning to network list...");
                    continue; // Go back to network selection
                } else if (confirmInput == "c" || confirmInput == "C") {
                    printl("Quick setup cancelled.");
                    return;
                } else {
                    printl("Invalid input. Returning to network list...");
                    continue;
                }
            } else {
                printl("Invalid network number. Please enter 0-" + String(numNetworks - 1));
                continue;
            }
        }
    }
}