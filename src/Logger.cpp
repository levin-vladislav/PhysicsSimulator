#include "Logger.h"

namespace fs = std::filesystem;


// Declaring static variables
std::thread Logger::logger_thread;
std::queue<Message> Logger::log_queue;
std::mutex Logger::log_mutex;
std::condition_variable Logger::log_cv;
std::atomic<bool> Logger::running = true;

std::unordered_map<MessageType, std::string> MessageTypeToStr =
{
    {MessageType::RAW, "RAW"},
    {MessageType::INFO, "INFO" },
    {MessageType::WARN, "WARN" },
    {MessageType::ERROR, "ERROR"}
};

Logger::Logger(std::string name) : name(name) {}


std::string generateLogFileName() {
    // Function generating log file name in format:
    // YY-MM-DD-N.txt      N - number of logs this day

    // Move this function to utils.cpp - std::string get_date()
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
 
void Logger::info(std::string text, bool show)
{   
    log(text, MessageType::INFO, name, show);
}

void Logger::warn(std::string text, bool show)
{
    log(text, MessageType::WARN, name, show);
}

void Logger::error(std::string text, bool show)
{
    log(text, MessageType::ERROR, name, show);
}

void Logger::raw(std::string text, bool show)
{
    log(text, MessageType::RAW, name, show);
}

// Move this function to utils.cpp
std::string Logger::get_time()
{
    // Don't forget to add check of OS!
    std::time_t now = std::time(nullptr);
    std::tm tm_safe;
    localtime_s(&tm_safe, &now);

    std::ostringstream oss;
    oss << std::put_time(&tm_safe, "%H:%M:%S");
    return oss.str();
}

void Logger::log(std::string text, MessageType type, std::string name, bool show)
{
    std::lock_guard<std::mutex> lock(log_mutex);
    std::string message;
    
    
    //message += std::format("[{0}] [{1}/{2}]: {3}", get_time(), name, "", text);

    
    Message msg = Message(text, type, name, show);
    
    log_queue.push(msg);
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
                    Message msg = log_queue.front();
                    log_queue.pop();
                    lock.unlock();

                    std::string msg_formatted = std::format("[{0}] [{1}/{2}]: {3}", get_time(), msg.name, MessageTypeToStr[msg.type], msg.text);

                    if (msg.show)
                    {
                        std::cout << "\33[2K\r";
                        if (msg.type == MessageType::RAW)
                        {
                            std::cout << msg.text;
                        }
                        else if (msg.type == MessageType::INFO)
                        {
                            std::cout << msg_formatted;
                        }
                        else if (msg.type == MessageType::WARN)
                        {
                            std::cout << "\033[33m" << msg_formatted;
                        }
                        else if (msg.type == MessageType::ERROR)
                        {
                            std::cout << "\033[31m" << msg_formatted;
                        }
                        std::cout << "\033[0m" << std::endl;
                    }

                    latest << msg_formatted << '\n' << std::flush;
                    dated << msg_formatted << '\n' << std::flush;

                   
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

Message::Message(std::string text, MessageType type, std::string name, bool show) : text(text), type(type), show(show), name(name) {}




