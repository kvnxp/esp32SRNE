#include "App.h"

MenuState App::currentState;
bool App::menuActive;
bool App::confirmingExit;
String App::pendingInput;
bool App::waitingForBTName;
WifiManager wifiManager;

void App::showMenu(const vector<String>& menu, const String& title)
{
    if (title != "") {
        ioManager::menuPrintln("\n═══════════════════════════════════");
        ioManager::menuPrintln("   " + title);
        ioManager::menuPrintln("═══════════════════════════════════");
    }
    
    for (size_t i = 0; i < menu.size(); i++)
    {
        String prefix = (i == 0 && menu[i].indexOf("Back") != -1) ? "← " : "  ";
        ioManager::menuPrintln(prefix + String(i) + ". " + menu[i]);
    }
    ioManager::menuPrintln("═══════════════════════════════════");
    ioManager::menuPrintln("Enter your choice: ");
}

void App::exitMenu()
{
    ioManager::menuPrintln("👋 Exiting control panel. Background monitoring continues.");
    menuActive = false;
    ioManager::setMenuActive(false);
    confirmingExit = false;
}

void App::init()
{
    ioManager::statusPrintln("[INIT] Starting application initialization...");
    
    ioManager::init();
    ioManager::statusPrintln("[INIT] I/O Manager initialized");
    
    currentState = MAIN_MENU;
    menuActive = false;  // Start with menu inactive
    ioManager::setMenuActive(false);
    confirmingExit = false;
    waitingForBTName = false;
    
    wifiManager.autoConectWifi();
    ioManager::statusPrintln("[INIT] WiFi auto-connect initiated");
    
    ioManager::statusPrintln("[INIT] Application initialization complete");
    ioManager::statusPrintln("\n🎛️  Type 'menu' to open the control panel");
}

void App::loop()
{
    String input = ioManager::getInput();
    if (input != "")
    {
        input.trim(); // Remove whitespace
        input.toLowerCase(); // Make case insensitive
        
        if (!menuActive && (input == "menu" || input == "m"))
        {
            ioManager::statusPrintln("🎛️  Opening control panel...");
            menuActive = true;
            ioManager::setMenuActive(true);
            MainMenu();
        }
        else if (menuActive)
        {
            pendingInput = input;
            processInput();
        }
        else if (input == "help" || input == "h")
        {
            ioManager::statusPrintln("\n📋 Available commands:");
            ioManager::statusPrintln("  • menu (m) - Open control panel");
            ioManager::statusPrintln("  • help (h) - Show this help");
            ioManager::statusPrintln("  • status   - Show system status");
        }
        else if (input == "status")
        {
            showSystemStatus();
        }
    }
    
    // Check WiFi connection and reconnect if needed
    wifiManager.checkAndReconnect();
}

void App::processInput()
{
    if (waitingForBTName)
    {
        if (pendingInput == "cancel" || pendingInput == "c")
        {
            ioManager::menuPrintln("❌ Cancelled");
            waitingForBTName = false;
            bluetoothMenu();
            return;
        }
        ioManager::setBluetoothName(pendingInput);
        ioManager::menuPrintln("✅ Bluetooth name updated successfully!");
        waitingForBTName = false;
        bluetoothMenu();
        return;
    }

    if (confirmingExit)
    {
        if (pendingInput == "y" || pendingInput == "yes")
        {
            menuActive = false;
            ioManager::setMenuActive(false);
            confirmingExit = false;
            ioManager::menuPrintln("👋 Control panel closed. Type 'menu' to reopen.");
        }
        else if (pendingInput == "n" || pendingInput == "no")
        {
            confirmingExit = false;
            MainMenu();
        }
        else
        {
            ioManager::menuPrintln("❓ Please enter 'y' (yes) or 'n' (no)");
        }
        return;
    }

    // Handle numeric input and shortcuts
    int num = -1;
    if (pendingInput.length() == 1 && isDigit(pendingInput[0])) {
        num = pendingInput.toInt();
    } else if (pendingInput == "back" || pendingInput == "b") {
        num = 0; // Back is always option 0
    } else if (pendingInput == "exit" || pendingInput == "quit" || pendingInput == "q") {
        if (currentState == MAIN_MENU) {
            num = 3; // Exit option in main menu
        } else {
            num = 0; // Back option in submenus
        }
    }
    
    if (num == -1) {
        ioManager::menuPrintln("❓ Invalid input. Use numbers, 'back', 'exit', or 'help'");
        return;
    }

    switch (currentState)
    {
    case MAIN_MENU:
        handleMainMenu(num);
        break;
    case WIFI_MENU:
        handleWifiMenu(num);
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
        confirmingExit = true;
        ioManager::menuPrintln("\n❓ Are you sure you want to exit the control panel?");
        ioManager::menuPrintln("   Type 'y' (yes) or 'n' (no): ");
        break;
    default:
        ioManager::menuPrintln("❌ Invalid option. Please select 0-2.");
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
        ioManager::menuPrintln("📝 Enter WiFi Network Name (SSID):");
        wifiManager.inputSSID();
        wifiMenu();
        break;
    case 2:
        ioManager::menuPrintln("🔐 Enter WiFi Password:");
        wifiManager.inputPassword();
        wifiMenu();
        break;
    case 3:
        ioManager::menuPrintln("🔗 Attempting connection...");
        wifiManager.connect();
        delay(1000);
        wifiMenu();
        break;
    case 4:
        showSystemStatus();
        delay(2000);
        wifiMenu();
        break;
    case 5:
        ioManager::menuPrintln("⚡ Quick Setup Mode:");
        ioManager::menuPrintln("Please provide network name and password...");
        wifiManager.quickSetup();
        wifiMenu();
        break;
    default:
        ioManager::menuPrintln("❌ Invalid selection! Choose 0-5.");
        delay(500);
        wifiMenu();
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
    const vector<String> menuItems = {"📶 WiFi Settings", "📱 Bluetooth Settings", "🚪 Exit Control Panel"};
    showMenu(menuItems, "🎛️ ESP32 SRNE Controller");
}

void App::wifiMenu()
{
    currentState = WIFI_MENU;
    String status = wifiManager.isConnected() ? "✅ Connected" : "❌ Disconnected";
    ioManager::menuPrintln("\n📊 WiFi Status: " + status);
    if (wifiManager.isConnected()) {
        ioManager::menuPrintln("🌐 IP: " + WiFi.localIP().toString());
    }
    const vector<String> menuItems = {"← Back", "📝 Set Network Name", "🔐 Set Password", "🔗 Connect Now", "📊 Show Status", "⚡ Quick Setup"};
    showMenu(menuItems, "📶 WiFi Configuration");
}

void App::bluetoothMenu()
{
    currentState = BLUETOOTH_MENU;
    ioManager::menuPrintln("\n📱 Current Name: " + ioManager::getBluetoothName());
    const vector<String> menuItems = {"← Back", "✏️ Change Name", "ℹ️ Name Info"};
    showMenu(menuItems, "📱 Bluetooth Configuration");
}

void App::handleBluetoothMenu(int selection)
{
    switch (selection)
    {
    case 0:
        MainMenu();
        break;
    case 1:
        ioManager::menuPrintln("✏️ Enter new Bluetooth device name:");
        requestBluetoothName();
        break;
    case 2:
        ioManager::menuPrintln("ℹ️ Bluetooth Device Information:");
        ioManager::menuPrintln("Current Name: " + ioManager::getBluetoothName());
        ioManager::menuPrintln("Status: Available");
        delay(2000);
        bluetoothMenu();
        break;
    default:
        ioManager::menuPrintln("❌ Invalid selection! Choose 0-2.");
        delay(500);
        bluetoothMenu();
        break;
    }
}

void App::showSystemStatus()
{
    ioManager::menuPrintln("\n📊 ═══════ SYSTEM STATUS ═══════");
    
    // WiFi Status
    if (wifiManager.isConnected()) {
        ioManager::menuPrintln("📶 WiFi: ✅ Connected");
        ioManager::menuPrintln("   🌐 IP: " + WiFi.localIP().toString());
        ioManager::menuPrintln("   📶 RSSI: " + String(WiFi.RSSI()) + " dBm");
        ioManager::menuPrintln("   🔗 SSID: " + WiFi.SSID());
    } else {
        ioManager::menuPrintln("📶 WiFi: ❌ Disconnected");
    }
    
    // Bluetooth Status
    ioManager::menuPrintln("📱 Bluetooth: ⚠️ Available");
    ioManager::menuPrintln("   📱 Name: " + ioManager::getBluetoothName());
    
    // System Info
    ioManager::menuPrintln("💾 Free Heap: " + String(ESP.getFreeHeap()) + " bytes");
    ioManager::menuPrintln("⏱️ Uptime: " + String(millis() / 1000) + " seconds");
    ioManager::menuPrintln("🔋 CPU Freq: " + String(ESP.getCpuFreqMHz()) + " MHz");
    
    ioManager::menuPrintln("═══════════════════════════════");
}

void App::requestBluetoothName()
{
    waitingForBTName = true;
    ioManager::menuPrintln("📱 Please enter the new Bluetooth device name:");
}