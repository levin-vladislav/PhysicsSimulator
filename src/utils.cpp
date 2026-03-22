#include "utils.h"

const std::string get_date()
{
    std::time_t now = std::time(nullptr);
    std::tm tm{};
    localtime_s(&tm, &now);
    std::ostringstream date;
    date << std::put_time(&tm, "%Y-%m-%d");
    return date.str();
}

const std::string get_time()
{
    // Don't forget to add check of OS!
    std::time_t now = std::time(nullptr);
    std::tm tm_safe;
    localtime_s(&tm_safe, &now);

    std::ostringstream oss;
    oss << std::put_time(&tm_safe, "%H:%M:%S");
    return oss.str();
}

const std::string readFile(std::string path)
{
    std::string content;
    std::ifstream fileStream(path, std::ios::in);

    if (!fileStream.is_open()) {
        std::cerr << "Could not read file " << path << std::endl;
        return "";
    }

    std::stringstream sstr;
    sstr << fileStream.rdbuf();
    content = sstr.str();
    fileStream.close();
    return content;

}