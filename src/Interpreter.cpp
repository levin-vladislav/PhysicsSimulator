#include "Interpreter.h"


Interpreter::Interpreter() : logger(Logger("Interpreter"))
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


    Command help = Command("help");
    help.set_callback([](std::vector<std::any> args)
        {
            std::cout << "List of the commands:" << std::endl;
            std::cout << "echo [text] - print everything given next" << std::endl;
            std::cout << "time - print current time" << std::endl;
            std::cout << "exit (or quit) - quit the program" << std::endl;
            std::cout << "help - show that list" << std::endl;
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

    if (commands.contains(cmd_name)) 
    {   
        if (args.size() != commands[cmd_name].get_args().size())
        {
            logger.error("Command '" + cmd_name + "' expects " + std::to_string(commands[cmd_name].get_args().size()) + " arguments, but got " + std::to_string(args.size()));
            return;
        }
        commands[cmd_name].execute(args);
    }
    else
    {
        logger.error("Command doesn't exist: " + cmd_name);
    }
}

void Interpreter::add_command(Command command)
{
    commands[command.get_name()] = command;
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

