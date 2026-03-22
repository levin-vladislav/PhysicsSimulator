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
	for (auto it : physicsEngine.get_ids())
	{
		graphicsEngine.objects[it.first].pos = physicsEngine.getPos(it.first);
		graphicsEngine.objects[it.first].rotation = physicsEngine.getRotation(it.first);
	}
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
	physicsEngine.init();
	graphicsEngine.init_window(dispatcher_ptr);
	graphicsEngine.init_shaders();
	graphicsEngine.init_grid();
	graphicsEngine.objectQueue.push(
		CreateRenderObjectRequest 
		{
			glm::vec2(0.0f, -5.5f),
			std::vector<float> {
				-25.0f, -5.0f,
				25.0f, -5.0f,
				25.0f, 5.0f,
				-25.0f, 5.0f
			}
		}
	);

	using clock = std::chrono::high_resolution_clock;

	auto last_time = clock::now();

	std::ifstream file("simulation.txt");

	if (file.is_open())
	{
		std::string line;
		while (std::getline(file, line)) {
			dispatcher_ptr->interpreter.run_command(line);
		}
		file.close();
	}
	else {
		std::cerr << "Error opening file" << std::endl;
	}

	while (running.load())
	{
		auto now = clock::now();
		std::chrono::duration<float> delta = now - last_time;
		float dt = delta.count() / 1000.0f;

		last_time = now;
		if (dt > 0.5)
		{
			dt = 0.5f;
		}
		else if (dt < 0.02f)
		{
			dt = 0.02f;
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
		std::this_thread::sleep_for(std::chrono::milliseconds(int(1000*dt)));
		// Runs update function with dt equal to time difference between ticks
	}

	graphicsEngine.stop();
	physicsEngine.stop();
}

void Engine::request(CreateBodyRequest physics_request,
					 CreateRenderObjectRequest graphics_request)
{
	// Manages requesting
	physics_request.id = next_id;
	physicsEngine.body_queue.push(physics_request);
	graphics_request.id = next_id;
	graphicsEngine.objectQueue.push(graphics_request);

	next_id++;
}
