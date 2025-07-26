#include "Logger.h"

std::mutex Logger::log_mutex;

Logger::Logger(std::string name) : name(name) {}

void Logger::info(std::string text)
{   
    std::thread t(&Logger::log, this, text, "INFO");
    t.join();
}

void Logger::warn(std::string text)
{
    std::thread t(&Logger::log, this, text, "WARN");
    t.join();
}

void Logger::error(std::string text)
{
    std::thread t(&Logger::log, this, text, "ERROR");
    t.join();
}

std::string Logger::get_time()
{
    std::time_t now = std::time(nullptr);
    std::tm tm_safe; 
    localtime_s(&tm_safe, &now);

    std::ostringstream oss;
    oss << std::put_time(&tm_safe, "%H:%M:%S");
    return "[" + oss.str() + "]";
}

void Logger::log(std::string text, std::string type)
{
    std::lock_guard<std::mutex> lock(Logger::log_mutex);
    std::cout << "\033[2K";
    if (type == "ERROR")
    {
        std::cout << "\033[31m";
    }
    else if (type == "WARN")
    {
        std::cout << "\033[33m";
    }
    std::cout << get_time() << " [" << name << "/" << type << "]: " << text << "\n";
    std::cout << "\033[0m";
}
