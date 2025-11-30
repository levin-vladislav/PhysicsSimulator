#include "Dispatcher.h"
#include "Engine.h"

Engine::Engine() : running(false), logger(Logger("Engine"))
{
	logger.info("Initialized.");
	this->start();
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
	t = std::thread(&Engine::main_loop, this);

}

void Engine::stop()
{
	running.store(false);
	if (t.joinable()) t.join();
}


void Engine::main_loop()
{
	graphicsEngine.init_window();

	using clock = std::chrono::high_resolution_clock;

	auto last_time = clock::now();

	while (running.load() )
	{
		auto now = clock::now();
		std::chrono::duration<float> delta = now - last_time;
		float dt = delta.count();
		last_time = now;
		if (dt > 0.5)
		{
			dt = 0.5;
		}
		else if (dt < 0.02)
		{
			dt = 0.02;
		}

		try
		{
			update(dt);
		}
		catch (char* e)
		{
			logger.error("Unexpected error occured:");
			logger.error(e);
		}
			
		// Runs update function with dt equal to time difference between ticks
	}

	graphicsEngine.stop();
	physicsEngine.stop();

}

void Engine::request(CreateBodyRequest request)
{
	// Manages requesting. Later could be request via JSON
	physicsEngine.create_body(request);
}


