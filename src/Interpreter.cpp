#include "Interpreter.h"

Interpreter::Interpreter() : logger(Logger("Interpreter"))
{
    init_commands();
	logger.info("Initialized.");
}

void Interpreter::init_commands()
{
    Command echo = Command("echo");
    echo.add_description("echo [text] - returns text");
    echo.set_callback([this](std::string args) {
        if (args.empty())
        {
            logger.error("Command \"echo\" expected arguments but 0 were given.");
            return;
        }
        std::cout << "\033[2K\r";
        std::cout << args << std::endl;
        });
    add_command(echo);


    Command time = Command("time");
    time.add_description("time - returns exact time");
    time.set_callback([](std::string args)
        {
            std::time_t now = std::time(nullptr);
            std::tm tm_safe;
            localtime_s(&tm_safe, &now);

            std::ostringstream oss;
            oss << std::put_time(&tm_safe, "%H:%M:%S");
            std::cout << "Current time: " << oss.str() << '\n';
        });
    add_command(time);

    Command delay = Command("delay");
    delay.add_description("delay [time (seconds)] [command] - runs the command with the specified delay.");
    delay.set_callback([this](std::string args)
    {
            std::istringstream iss(args);
            std::string time_str;
            std::string command;
            std::string new_args;
            int time;

            iss >> time_str;
            if (time_str.empty())
            {
                logger.error("Invalid arguments given");
            }
            
            try {
                size_t pos;
                time = std::stoi(time_str, &pos);  
                if (pos != time_str.length()) { 
                    logger.error("Not a number value was given");
                    return;
                }
                
            }
            catch (...) {
                logger.error("Not a number value was given");
                return;
            }

            iss >> command;
            
            std::getline(iss, new_args);
            
            auto f = [this, time, command, new_args]() {
                std::this_thread::sleep_for(std::chrono::seconds(time));
                this->execute(command, new_args);
                };

            std::thread t = std::thread(f);
            t.detach();
    });
    add_command(delay);


    Command help = Command("help");
    help.add_description("help - show that list");
    help.set_callback([this](std::string args)
        {
            if (args.empty())
            {
                std::cout << "List of the commands:" << std::endl;
                for (auto it = commands.begin(); it != commands.end(); ++it)
                {
                    std::cout << it->second.get_description() << std::endl;
                }
            }

            else
            {
                std::istringstream iss(args);
                std::string command;
                iss >> command;

                if (!commands.contains(command)) {
                    logger.error("The given command was not found: " + command);
                    return;
                }

                std::cout << commands[command].get_description() << std::endl;
                
            }
            
        });

    add_command(help);
}


void Interpreter::run_command(std::string command)
{
    std::istringstream iss(command);
    std::string cmd_name;
    std::string args;

    iss >> cmd_name;
    std::getline(iss, args);

    execute(cmd_name, args);
}

void Interpreter::add_command(Command command)
{
    commands[command.get_name()] = command;
}

void Interpreter::execute(std::string command, std::string args)
{
    if (commands.contains(command))
    {
        commands[command].execute(args);
    }
    else
    {
        logger.error("Command doesn't exist: " + command);
    }
}

void Command::set_callback(std::function<void(std::string)> cb) {
    callback = cb;
}

const std::string& Command::get_name() const { return name; }

void Command::execute(const std::string& input_args) const {
    callback(input_args);
}

void Command::add_description(std::string description)
{
    this->description = description;
}

std::string Command::get_description()
{
    return description;
}
