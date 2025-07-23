#include "Dispatcher.h"
#include <iostream>

Dispatcher::Dispatcher() : running(false), logger(Logger("Dispatcher"))
{
	logger.info("Initialized.");
}

bool Dispatcher::is_running() const
{
	return running;
}

void Dispatcher::update()
{
	
}