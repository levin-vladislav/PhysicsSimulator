#include "Dispatcher.h"
#include "Engine.h"
#include <iostream>

Dispatcher::Dispatcher(Engine* engine, std::atomic<bool>* shutdown) : 
	running(false), 
	engine_ptr(engine), 
	logger(Logger("Dispatcher")), 
	shutdown_ptr(shutdown)
{
	logger.info("Initialized.");
}

bool Dispatcher::is_running() const
{
	return running.load();
}

void Dispatcher::main_loop()
{
	while (is_running())
	{
		std::cout << ">>> ";
		std::string input;
		std::getline(std::cin, input);
		if (input == "exit" || input == "quit")
		{
			running.store(false);
			shutdown_ptr->store(true);
		}
		else
		{
			interpreter.run_command(input);
		}
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