#include "Engine.h"
#include "Dispatcher.h"
#include "Logger.h"
#include <atomic>

int main()
{   
    std::atomic<bool> shutdown = false;

    Logger logger = Logger("MAIN");
    Logger::start_logging();
    logger.info("Program has been started.");
    Engine engine = Engine();
    Dispatcher dispatcher = Dispatcher(&engine, &shutdown);
    
    engine.start();
    dispatcher.start();
    

    while (!shutdown.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    engine.stop();
    dispatcher.stop();
    
    return 0;

}

