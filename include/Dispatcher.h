#pragma once
#include "Interpreter.h"
#include "Logger.h"
#include <atomic>
#include <thread>

class Engine;

class Dispatcher
{
	// Class listening managing input from console
public:
	Dispatcher(Engine* engine, std::atomic<bool>* shutdown_ptr);
	
	void start();
	void stop();
	bool is_running() const;

	Engine* engine_ptr;
	Interpreter interpreter;
private:
	std::thread t;
	std::atomic<bool> running;
	Logger logger;

	std::atomic<bool>* shutdown_ptr;

	void main_loop();
};