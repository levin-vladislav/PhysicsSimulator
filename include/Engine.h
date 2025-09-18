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
	// Object managing the simulation
public:
	Engine();

	void update(float dt); // main function, runs every tick
	void start(); // starts Engine main loop
	void stop();  // stops Engine main loop

	bool is_running() const;

	void request(CreateBodyRequest request); // Method for proper Dispatcher <=> Body interaction

	PhysicsEngine physicsEngine; // Manages physics in the project
	GraphicsEngine graphicsEngine; // Manages graphics in the project

private:
	std::thread t; // Engine thread
	std::atomic<bool> running;
	Logger logger; // Engine logger

	void main_loop();
};