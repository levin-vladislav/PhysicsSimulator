#include "Engine.h"

Engine::Engine() : running(false), logger(Logger("Engine"))
{
	logger.info("Initialized.");
}

bool Engine::is_running() const
{
	return running.load();
}

void Engine::update(float dt)
{
	physicsEngine.update(dt);
	graphicsEngine.update(dt);
}

void Engine::start()
{
	running.store(true);

}

void Engine::stop()
{
	running.store(false);
}


void Engine::main_loop()
{
	using clock = std::chrono::high_resolution_clock;

	auto last_time = clock::now();

	while (running.load())
	{
		auto now = clock::now();
		std::chrono::duration<float> delta = now - last_time;
		float dt = delta.count();
		last_time = now;

		update(dt);

	}
}

