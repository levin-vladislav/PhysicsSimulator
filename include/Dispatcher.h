#include "Interpreter.h"
#include "Logger.h"
#include <atomic>
#include <thread>

class Dispatcher
{
public:
	Dispatcher();
	
	void start();
	void stop();
	bool is_running() const;
private:
	std::thread t;
	std::atomic<bool> running;
	Interpreter interpreter;
	Logger logger;

	void main_loop();
};