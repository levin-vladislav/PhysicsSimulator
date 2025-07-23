#include "GraphicsEngine.h"
#include "PhysicsEngine.h"
#include <Logger.h>
#include <atomic>
#include <thread>
#include <chrono>

class Dispatcher;

class Engine
{	
public:
	Engine();
	void update(float dt);
	void start();
	void stop();
	bool is_running() const;

private:
	std::thread t;
	PhysicsEngine physicsEngine;
	GraphicsEngine graphicsEngine;
	std::atomic<bool> running;
	Logger logger;

	void main_loop();
};