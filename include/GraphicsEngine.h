#include "Logger.h"


class GraphicsEngine
{
public:
	GraphicsEngine();
	void update(float dt);
	bool is_running() const;
private:
	bool running;
	Logger logger;
};