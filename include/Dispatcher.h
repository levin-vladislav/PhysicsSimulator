#pragma once
#include "Interpreter.h"
#include "Logger.h"
#include <atomic>
#include <thread>


class Engine;

class Dispatcher
{
public:
	Dispatcher(Engine* engine, std::atomic<bool>* shutdown_ptr);
	
	void start();
	void stop();
	bool is_running() const;
private:
	std::thread t;
	std::atomic<bool> running;
	Interpreter interpreter;
	Logger logger;

	Engine* engine_ptr;
	std::atomic<bool>* shutdown_ptr;

	void main_loop();
};