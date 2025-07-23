#include "Logger.h"

class PhysicsEngine
{
public:
	PhysicsEngine();
	void update();
	bool is_running() const;
private:
	Logger logger;
	bool running;
};