#include "PhysicsEngine.h"
#include <iostream>

PhysicsEngine::PhysicsEngine() : running(true), logger(Logger("PhysicsEngine"))
{
	logger.info("Initialized.");
}

bool PhysicsEngine::is_running() const
{
	return running;
}

void PhysicsEngine::update()
{

}

