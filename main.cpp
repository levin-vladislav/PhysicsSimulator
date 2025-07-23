#include "Engine.h"
#include "Dispatcher.h"
#include "Logger.h"

int main()
{   
    Logger logger = Logger("MAIN");
    logger.info("Program has been started.");
    Engine engine = Engine();
    Dispatcher dispatcher = Dispatcher();
    
    engine.start();
    dispatcher.start();

    return 1;

}

