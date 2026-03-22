#include "Logger.h"
#include <windows.h>
#include "Engine.h"
#include "Dispatcher.h"

#include <atomic>

void EnableANSI()
{
    // Enabling using escape sequences in console, so logs can be colored
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

int main()
{   
    EnableANSI();
    // Prepearing shutdown variable
    std::atomic<bool> shutdown = false;

    // Starting logger
    Logger logger = Logger("MAIN");
    Logger::start_logging();

    logger.info("Program has been started.");

    // Starting Engine and Dispatcher
    Engine engine = Engine();
    Dispatcher dispatcher = Dispatcher(&engine, &shutdown);  

    // Main loop
    while (!shutdown.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    engine.stop();
    dispatcher.stop();
    
    return 0;
}

