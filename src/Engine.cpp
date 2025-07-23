#include "Engine.h"

Engine::Engine() : running(false), logger(Logger("Engine"))
{
	logger.info("Initialized.");
}

bool Engine::is_running() const
{
	return running.load();
}

void Engine::update()
{
	physicsEngine.update();
	graphicsEngine.update();
}

void Engine::start()
{
	running.store(true);
}

void Engine::stop()
{
	running.store(false);
}


