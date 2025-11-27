#pragma once
#include <iostream>
#include <format>
#include <chrono>
#include <mutex>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <ctime>
#include <string>
#include <atomic>
#include <iomanip>
#include <queue>
#include <functional>
#include <condition_variable>
#include <typeinfo>
#include <type_traits>

enum class MessageType {
	RAW, 
	INFO, 
	WARN,
	ERROR
};

class Message {
public:
	Message(std::string text, MessageType type, std::string name, bool show = true);
	std::string text;
	MessageType type;
	std::string name;
	bool show;
};

class Logger
{
	// Object that safely logs information to console

	// static variables to manage all of the loggers
	// Each of the different loggers use one thread
	static std::thread logger_thread;
	static std::queue<Message> log_queue;
	static std::mutex log_mutex;
	static std::condition_variable log_cv;
	static std::atomic<bool> running;
	
public:
	Logger(std::string name);

	// Methods to output data
	void info(std::string text, bool show = true);    //  [<current_time>][<name>/INFO] <text> 
	void warn(std::string text, bool show = true);    //  [<current_time>][<name>/WARN] <text> (color - yellow)
	void error(std::string text, bool show = true);   //  [<current_time>][<name>/INFO] <text> (color - red)
	void raw(std::string message, bool show = true);  //  <text>


    // Static methods to manage all loggers at once
	static void start_logging();
	static void stop();

	
private:
	
	std::string name;
	static std::string get_time(); // i gotta remove it 

	void log(std::string text, MessageType type, std::string name, bool show);
};






