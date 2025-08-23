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
	static std::thread logger_thread;
	static std::queue<std::string> log_queue;
	static std::mutex log_mutex;
	static std::condition_variable log_cv;
	static std::atomic<bool> running;
	
public:
	Logger(std::string name);
	void info(std::string text);
	void warn(std::string text);
	void error(std::string text);
	void raw(std::string message);
	static void start_logging();
	static void stop();

	
	
private:
	
	std::string name;
	std::string get_time();

	void log(std::string text, std::string type);
};


