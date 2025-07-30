#include "Logger.h"

std::thread Logger::logger_thread;
std::queue<std::string> Logger::log_queue;
std::mutex Logger::log_mutex;
std::condition_variable Logger::log_cv;
std::atomic<bool> Logger::running = true;

Logger::Logger(std::string name) : name(name) {}

void Logger::info(std::string text)
{   
    log(text, "INFO");
}

void Logger::warn(std::string text)
{
    log(text, "WARN");
}

void Logger::error(std::string text)
{
    log(text, "ERROR");
}

void Logger::raw(std::string text)
{
    log(text, "RAW");
}

std::string Logger::get_time()
{
    std::time_t now = std::time(nullptr);
    std::tm tm_safe; 
    localtime_s(&tm_safe, &now);

    std::ostringstream oss;
    oss << std::put_time(&tm_safe, "%H:%M:%S");
    return oss.str();
}

void Logger::log(std::string text, std::string type)
{
    std::lock_guard<std::mutex> lock(log_mutex);
    std::string message;
    
    if (type == "RAW")
    {
        message = text;
    }
    else
    {
        if (type == "ERROR")
        {
            message = "\033[31m";
        }

        else if (type == "WARN")
        {
            message = "\033[33m";
        }
        else if (type != "INFO") return;
        message += std::format("[{0}] [{1}/{2}]: {3}\033[0m", get_time(), name, type, text);
    }
    
    log_queue.push(message);
    log_cv.notify_one();
}


void Logger::start_logging()
{
    logger_thread = std::thread([]
        {
            while (running.load())
            {
                std::unique_lock<std::mutex> lock(log_mutex);
                log_cv.wait(lock, [] { return !log_queue.empty() || !running.load(); });

                while (!log_queue.empty())
                {
                    std::string msg = log_queue.front();
                    log_queue.pop();
                    lock.unlock();

                    std::cout << "\33[2K\r" << msg << std::endl;

                    std::cout << ">>> " << std::flush;

                    lock.lock();

                }
            }
        }

    );
}


void Logger::stop()
{
    running.store(false);
    log_cv.notify_one();
    if (logger_thread.joinable())
    {
        logger_thread.join();
    }
}
