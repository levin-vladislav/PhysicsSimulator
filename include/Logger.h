#pragma once
#include <iostream>
#include <format>
#include <chrono>
#include <mutex>
#include <sstream>
#include <atomic>
#include <iomanip>




class Logger
{
	static std::mutex log_mutex;
public:
	Logger(std::string name);
	void info(std::string text);
	void warn(std::string text);
	void error(std::string text);
private:
	
	std::string name;
	std::string get_time();
	void log(std::string text, std::string type);
};

