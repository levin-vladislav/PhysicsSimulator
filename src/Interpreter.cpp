#include "Interpreter.h"
#include "Engine.h"

Interpreter::Interpreter() : logger(Logger("Interpreter"))
{
    init_commands();
	logger.info("Initialized.");
}

void Interpreter::init_commands()
{
    // echo [text] - returns text
    Command echo = Command("echo");
    echo.add_description("echo [text] - returns text");
    echo.set_callback([this](std::string args) {
        if (args.empty())
        {
            logger.error("Command \"echo\" expected arguments but 0 were given.");
            return;
        }
        
        logger.raw(args);
        });
    add_command(echo);


    // time - returns exact time
    Command time = Command("time");
    time.add_description("time - returns exact time");
    time.set_callback([this](std::string args)
        {
            std::time_t now = std::time(nullptr);
            std::tm tm_safe;
            localtime_s(&tm_safe, &now);

            std::ostringstream oss;
            oss << std::put_time(&tm_safe, "%H:%M:%S");
            logger.raw("Current time: " + oss.str());

        });
    add_command(time);


    // delay [time (seconds)] [command] - runs the command with the specified delay.
    Command delay = Command("delay");
    delay.add_description("delay [time (seconds)] [command] - runs the command with the specified delay");
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


    // body_list - returns the list of all bodies
    Command body_list = Command("body_list");
    body_list.add_description("body_list - returns the list of all bodies");
    body_list.set_callback([this](std::string args)
        {
            engine_ptr->physicsEngine.log_bodies();
        });
    add_command(body_list);


    // create [x] [y] [mass] - creates a kinematic rigid body in given coordinates
    Command create = Command("create");
    create.add_description("create [x] [y] [mass] - creates a kinematic rigid body in given coordinates");
    create.set_callback([this](std::string args)
        {
            if (args.empty())
            {
                logger.error("Invalid arguments given");
                return;
            }

            std::istringstream iss(args);
            std::string x_string;
            std::string y_string;
            std::string m_string;

            if (!(iss >> x_string >> y_string >> m_string))
            {
                logger.error("Not enough values were given!");
                return;
            }

            try
            {
                float x = std::stof(x_string);
                float y = std::stof(y_string);
                float m = std::stof(m_string);
                engine_ptr->request(CreateBodyRequest{ glm::vec2(x, y), 
                                                       glm::vec2(0.0f, 0.0f),
                                                       m,
                                                       BodyType::KINEMATIC});
            }
            catch (const std::invalid_argument& e) {
                logger.error("Invalid float was given");
            }
            catch (const std::out_of_range& e) {
                logger.error("Float out of range was given");
            }

        });
    add_command(create);


    // help - show that list
    Command help = Command("help");
    help.add_description("help - show that list");
    help.set_callback([this](std::string args)
        {
            if (args.empty())
            {
                logger.raw("List of the commands:");
                for (auto it = commands.begin(); it != commands.end(); ++it)
                {
                    logger.raw(it->second.get_description());
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

                logger.raw(commands[command].get_description());
                
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
