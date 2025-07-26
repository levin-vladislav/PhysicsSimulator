#include "Interpreter.h"

Logger Interpreter::logger = Logger("Interpreter");
std::unordered_map <std::string, Command> Interpreter::commands;

Interpreter::Interpreter()
{
    init_commands();
	logger.info("Initialized.");
}

void Interpreter::init_commands()
{
    Command echo = Command("echo");
    echo.set_callback([](std::vector<std::any> args) {
        std::string text = std::any_cast<std::string>(args[0]);
        std::cout << text << std::endl;
        });
    echo.add_argument("text", ArgType::STRING);
    add_command(echo);


    Command time = Command("time");
    time.set_callback([](std::vector<std::any> args)
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
    delay.set_callback([](std::vector<std::any> args)
    {
            int time = std::any_cast<int>(args[0]);
            std::string command = std::any_cast<std::string>(args[1]);
            std::vector<std::any> args_(args.begin() + 2, args.end());
            std::vector<std::string> new_args;
            for (const auto& item : args_) {
                if (item.type() == typeid(int)) {
                    new_args.push_back(std::to_string(std::any_cast<int>(item)));
                }
                else if (item.type() == typeid(float)) {
                    new_args.push_back(std::to_string(std::any_cast<float>(item)));
                }
                else if (item.type() == typeid(double)) {
                    new_args.push_back(std::to_string(std::any_cast<double>(item)));
                }
                else if (item.type() == typeid(std::string)) {
                    new_args.push_back(std::any_cast<std::string>(item));
                }
            }
            auto f = [time, command, new_args]() {
                std::this_thread::sleep_for(std::chrono::seconds(time));
                Interpreter::execute(command, new_args);
                };
            std::thread t = std::thread(f);
            
    });
    delay.add_argument("time", ArgType::INT);
    delay.add_argument("command", ArgType::STRING);
    delay.add_argument("args", ArgType::STRING);
    add_command(delay);


    Command help = Command("help");
    help.set_callback([](std::vector<std::any> args)
        {
            std::cout << "List of the commands:" << std::endl;
            std::cout << "echo [text] - print everything given next" << std::endl;
            std::cout << "time - print current time" << std::endl;
            std::cout << "exit (or quit) - quit the program" << std::endl;
            std::cout << "help - show that list" << std::endl;
            std::cout << "delay [time (seconds)] [command] - runs a command with given delay" << std::endl;
        });

    add_command(help);
}


void Interpreter::run_command(std::string command)
{
    std::istringstream iss(command);
    std::string cmd_name;
    iss >> cmd_name;

    std::vector<std::string> args;
    std::string arg;
    while (iss >> arg) args.push_back(arg);

    execute(cmd_name, args);
}

void Interpreter::add_command(Command command)
{
    commands[command.get_name()] = command;
}

void Interpreter::execute(std::string command, std::vector<std::string> args)
{
    if (commands.contains(command))
    {
        if (args.size() > commands[command].get_args().size())
        {
            logger.error("Command '" + command + "' expects " + std::to_string(commands[command].get_args().size()) + " arguments, but got " + std::to_string(args.size()));
            return;
        }
        commands[command].execute(args);
    }
    else
    {
        logger.error("Command doesn't exist: " + command);
    }
}

void Command::add_argument(const std::string& arg_name, ArgType type) {
    args.push_back({ arg_name, type });
}

void Command::set_callback(std::function<void(std::vector<std::any>)> cb) {
    callback = cb;
}



const std::string& Command::get_name() const { return name; }
const std::vector<ArgumentSpec>& Command::get_args() const { return args; }

void Command::execute(const std::vector<std::string>& input_args) const {
    using CommandArgs = std::vector<std::any>;
    CommandArgs parsed;
    for (size_t i = 0; i < input_args.size(); ++i) {
        switch (args[i].type) {
        case ArgType::INT:
            parsed.push_back(std::stoi(input_args[i]));
            break;
        case ArgType::FLOAT:
            parsed.push_back(std::stof(input_args[i]));
            break;
        case ArgType::STRING:
            parsed.push_back(input_args[i]);
            break;
        }
    }
    if (callback) {
        callback(parsed);
    }
}

