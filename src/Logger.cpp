#include "Logger.h"

namespace fs = std::filesystem;

std::thread Logger::logger_thread;
std::queue<std::string> Logger::log_queue;
std::mutex Logger::log_mutex;
std::condition_variable Logger::log_cv;
std::atomic<bool> Logger::running = true;

Logger::Logger(std::string name) : name(name) {}

std::string generateLogFileName() {
    // get date
    std::time_t now = std::time(nullptr);
    std::tm tm{};
    localtime_s(&tm, &now);

    std::ostringstream date;
    date << std::put_time(&tm, "%Y-%m-%d");
    std::string prefix = date.str();

    int maxIndex = 0;

    // scan folder for files with this prefix
    std::string logDir = "logs";
    for (const auto& entry : fs::directory_iterator(logDir)) {
        std::string name = entry.path().filename().string();
        if (name.rfind(prefix, 0) == 0) {  // starts with prefix
            // parse "-N.txt"
            size_t dash = name.find_last_of('-');
            size_t dot = name.find_last_of('.');
            if (dash != std::string::npos && dot != std::string::npos) {
                try {
                    int num = std::stoi(name.substr(dash + 1, dot - dash - 1));
                    if (num > maxIndex) maxIndex = num;
                }
                catch (...) {}
            }
        }
    }

    int nextIndex = maxIndex + 1;
    return prefix + "-" + std::to_string(nextIndex) + ".txt";
}


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
        message += std::format("[{0}] [{1}/{2}]: {3}", get_time(), name, type, text);
    }
    
    log_queue.push(message);
    log_cv.notify_one();
}


void Logger::start_logging()
{
    logger_thread = std::thread([]
        {
            std::ofstream latest("logs/latest_log.txt");
            std::ofstream dated("logs/" + generateLogFileName());
            if (!latest.is_open() || !dated.is_open()) {
                // Handle error
                std::cerr << "Failed to open log file!" << std::endl;
                return 1;
            }
            while (running.load())
            {
                std::unique_lock<std::mutex> lock(log_mutex);
                log_cv.wait(lock, [] { return !log_queue.empty() || !running.load(); });

                while (!log_queue.empty())
                {
                    std::string msg = log_queue.front();
                    log_queue.pop();
                    lock.unlock();

                    latest << msg << '\n';
                    dated << msg << '\n';

                    std::cout << "\33[2K\r" << msg << "\033[0m" << std::endl;

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


