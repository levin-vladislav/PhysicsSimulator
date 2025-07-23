#include "Interpreter.h"
#include "Logger.h"
class Dispatcher
{
public:
	Dispatcher();
	void update();
	bool is_running() const;
private:
	bool running;
	Interpreter interpreter;
	Logger logger;
};