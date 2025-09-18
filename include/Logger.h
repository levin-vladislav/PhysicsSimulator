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




class Logger
{
	// Object that safely logs to console


	// static variables to manage all of the loggers
	// Each og the different loggers use one thread
	static std::thread logger_thread;
	static std::queue<std::string> log_queue;
	static std::mutex log_mutex;
	static std::condition_variable log_cv;
	static std::atomic<bool> running;
	
public:
	Logger(std::string name);

	// Methods to output data
	void info(std::string text);    //  [<current_time>][<name>/INFO] <text> 
	void warn(std::string text);    //  [<current_time>][<name>/WARN] <text> (color - yellow)
	void error(std::string text);   //  [<current_time>][<name>/INFO] <text> (color - red)
	void raw(std::string message);  //  <text>


    // Static methods to manage all loggers at once
	static void start_logging();
	static void stop();

	
private:
	
	// 
	std::string name;
	std::string get_time();

	void log(std::string text, std::string type);
};


