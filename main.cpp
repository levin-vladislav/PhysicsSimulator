#include "Engine.h"
#include "Dispatcher.h"
#include "Logger.h"
#include <atomic>

int main()
{   
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
    try
    {
        while (!shutdown.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
    catch (const char* e)
    {
        logger.info("Unexpected error occured: ");
        logger.error(e);
        engine.stop();
        dispatcher.stop();
        return -1;
    }

    engine.stop();
    dispatcher.stop();
    
    return 0;

}

