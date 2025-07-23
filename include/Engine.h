#include "GraphicsEngine.h"
#include "PhysicsEngine.h"
#include <Logger.h>
#include <atomic>
#include <thread>

class Engine
{	
public:
	Engine();
	void update();
	void start();
	void stop();
	bool is_running() const;

private:
	PhysicsEngine physicsEngine;
	GraphicsEngine graphicsEngine;
	std::atomic<bool> running;
	Logger logger;
};