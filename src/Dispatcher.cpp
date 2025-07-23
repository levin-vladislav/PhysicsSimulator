#include "Dispatcher.h"
#include <iostream>

Dispatcher::Dispatcher() : running(false), logger(Logger("Dispatcher"))
{
	logger.info("Initialized.");
}

bool Dispatcher::is_running() const
{
	return running.load();
}

void Dispatcher::main_loop()
{
	while (running.load())
	{

	}
}

void Dispatcher::start()
{
	running.store(true);
	t = std::thread(&Dispatcher::main_loop, this);
}

void Dispatcher::stop()
{
	running.store(false);
	if (t.joinable())
	{
		t.join();
	}
}