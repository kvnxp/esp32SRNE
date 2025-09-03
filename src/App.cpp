#include "App.h"
#define printl ioManager::menuPrintln

MenuState App::currentState;
bool App::menuActive;
bool App::confirmingExit;
String App::pendingInput;
bool App::waitingForBTName;
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
    printl("[INIT] Starting application initialization...");
    
    ioManager::init();
    printl("[INIT] I/O Manager initialized");
    
    currentState = MAIN_MENU;
    menuActive = false;  // Start with menu inactive
    ioManager::setMenuActive(false);
    confirmingExit = false;
    waitingForBTName = false;
    
    wifiManager.autoConectWifi();
    printl("[INIT] WiFi auto-connect initiated");
    
    printl("[INIT] Application initialization complete");
    printl("Type 'menu' to enter the main menu");
}

void App::loop()
{
    String input = ioManager::getInput();
    if (input != "")
    {
        ioManager::statusPrintln("[DEBUG] Received input: '" + input + "'");
        if (!menuActive && input == "menu")
        {
            ioManager::statusPrintln("[DEBUG] Entering menu mode");
            menuActive = true;
            ioManager::setMenuActive(true);
            MainMenu();
        }
        else if (menuActive)
        {
            pendingInput = input;
            processInput();
        }
    }
    
    // Check WiFi connection and reconnect if needed
    wifiManager.checkAndReconnect();
}

void App::processInput()
{
    if (waitingForBTName)
    {
        ioManager::setBluetoothName(pendingInput);
        waitingForBTName = false;
        bluetoothMenu();
        return;
    }

    if (confirmingExit)
    {
        if (pendingInput == "y" || pendingInput == "Y")
        {
            menuActive = false;
            ioManager::setMenuActive(false);
            confirmingExit = false;
            printl("Menu exited. Type 'menu' to re-enter.");
        }
        else if (pendingInput == "n" || pendingInput == "N")
        {
            confirmingExit = false;
            MainMenu();
        }
        else
        {
            printl("Please enter y or n.");
        }
        return;
    }

    int num = pendingInput.toInt();
    switch (currentState)
    {
    case MAIN_MENU:
        handleMainMenu(num);
        break;
    case WIFI_MENU:
        handleWifiMenu(num);
        break;
    case MISC_MENU:
        handleMiscMenu(num);
        break;
    case BLUETOOTH_MENU:
        handleBluetoothMenu(num);
        break;
    }
}

void App::handleMainMenu(int selection)
{
    switch (selection)
    {
    case 0:
        wifiMenu();
        break;
    case 1:
        bluetoothMenu();
        break;
    case 2:
        miscMenu();
        break;
    case 3:
        confirmingExit = true;
        printl("Are you sure you want to exit the menu? (y/n)");
        break;
    default:
        printl("Invalid option. Please select 0-3.");
        MainMenu();
        break;
    }
}

void App::handleWifiMenu(int selection)
{
    switch (selection)
    {
    case 0:
        MainMenu();
        break;
    case 1:
        wifiManager.inputSSID();
        wifiMenu();
        break;
    case 2:
        wifiManager.inputPassword();
        wifiMenu();
        break;
    case 3:
        wifiManager.connect();
        wifiMenu();
        break;
    case 4:
        wifiManager.getStatus();
        wifiMenu();
        break;
    case 5:
        wifiManager.quickSetup();
        wifiMenu();
        break;
    default:
        printl("Invalid option. Please select 0-5.");
        wifiMenu();
        break;
    }
}

void App::handleMiscMenu(int selection)
{
    switch (selection)
    {
    case 0:
        MainMenu();
        break;
    case 1:
        wifiManager.saveProfile();
        miscMenu();
        break;
    case 2:
        wifiManager.loadProfile();
        miscMenu();
        break;
    default:
        printl("Invalid option. Please select 0-2.");
        miscMenu();
        break;
    }
}

void App::menuSelector(int menu)
{
    // Removed: Old function, no longer needed
}

void App::MainMenu()
{
    currentState = MAIN_MENU;
    printl("\nMain Menu\n");
    const vector<String> menuItems = {"WiFi Settings", "Bluetooth Settings", "Misc Settings", "Exit Menu"};
    showMenu(menuItems);
    printl("Select an option (0-3):");
}

void App::wifiMenu()
{
    currentState = WIFI_MENU;
    printl("\nWiFi Menu\n");
    const vector<String> menuItems = {"Back", "Set SSID", "Set Password", "Connect", "Status", "Quick Setup"};
    showMenu(menuItems);
    printl("Select an option (0-5):");
}

void App::miscMenu()
{
    currentState = MISC_MENU;
    printl("\nMisc Menu\n");
    const vector<String> menuItems = {"Back", "Save Profile", "Load Profile"};
    showMenu(menuItems);
    printl("Select an option (0-2):");
}

void App::bluetoothMenu()
{
    currentState = BLUETOOTH_MENU;
    printl("\nBluetooth Menu\n");
    printl("Current Bluetooth Name: " + ioManager::getBluetoothName());
    const vector<String> menuItems = {"Back", "Set Bluetooth Name", "Save Name to File"};
    showMenu(menuItems);
    printl("Select an option (0-2):");
}

void App::handleBluetoothMenu(int selection)
{
    switch (selection)
    {
    case 0:
        MainMenu();
        break;
    case 1:
        printl("Enter new Bluetooth name:");
        waitingForBTName = true;
        break;
    case 2:
        printl("Name is automatically saved when set.");
        bluetoothMenu();
        break;
    default:
        printl("Invalid option. Please select 0-2.");
        bluetoothMenu();
        break;
    }
}