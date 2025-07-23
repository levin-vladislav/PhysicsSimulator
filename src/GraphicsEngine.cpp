#include "GraphicsEngine.h"
#include <iostream>

GraphicsEngine::GraphicsEngine() : running(true), logger(Logger("GraphicsEngine"))
{
	logger.info("Initialized.");
}

bool GraphicsEngine::is_running() const
{
	return running;
}

void GraphicsEngine::update(float dt)
{

}

