#include "ioManager.hpp"
#include <vector>
#define vector std::vector

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
    // void waitForInput();
};
