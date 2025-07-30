#pragma once
#include "GraphicsEngine.h"
#include "PhysicsEngine.h"
#include "Logger.h"
#include <atomic>
#include <thread>
#include <chrono>
#include "utils.h"

class Dispatcher;

class Engine
{	
public:
	Engine();
	void update(float dt);
	void start();
	void stop();
	bool is_running() const;
	void request(CreateBodyRequest request);

	PhysicsEngine physicsEngine;
	GraphicsEngine graphicsEngine;

private:
	std::thread t;
	std::atomic<bool> running;
	Logger logger;

	void main_loop();
};