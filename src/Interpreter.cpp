#include "Interpreter.h"
#include "Engine.h"

Interpreter::Interpreter() : logger(Logger("Interpreter"))
{
    init_commands();
	logger.info("Initialized.");
}

void Interpreter::init_commands()
{
    // Registering commands
    // Command [command_name] = Command(“[command_name]”);
    // [command_name].add_description(“This is displayed when /“help/” is called”);
    // [command_name].set_callback([this](std::string args) {
    //    // Body of function. Here can be added parsing of 'args' string and 
    //    // what function does
    // };)
    //


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
            
            logger.raw("Current time: " + get_time());

        });
    add_command(time);


    // delay [time (seconds)] [command] - runs the command with the specified delay.
    Command delay = Command("delay");
    delay.add_description("delay [time (seconds)] [command] - runs the command with the specified delay");
    delay.set_callback([this](std::string args){
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
                time = int(std::stof(time_str, &pos) * 1000.0f);  
                                
            }
            catch (...) {
                logger.error("Not a number value was given");
                return;
            }

            iss >> command;
            
            std::getline(iss, new_args);
            
            auto f = [this, time, command, new_args]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(time));
                this->execute(command, new_args);
                };

            std::thread t = std::thread(f);
            t.detach();});
    add_command(delay);


    // body_list - returns the list of all bodies
    Command body_list = Command("body_list");
    body_list.add_description("body_list - returns the list of all bodies");
    body_list.set_callback([this](std::string args)
        {
            engine_ptr->physicsEngine.log_bodies();
        });
    add_command(body_list);


    // create [x] [y] [mass] - creates a dynamic rigid body in given coordinates
    Command create = Command("create");
    create.add_description("create [x] [y] [mass] [shape] [size] - creates a dynamic rigid body in given coordinates");
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
            std::string s_string;
            std::string r_string;

            if (!(iss >> x_string >> y_string >> m_string >> s_string >> r_string))
            {
                logger.error("Not enough values were given!");
                return;
            }
            try
            {
                float x = std::stof(x_string);
                float y = std::stof(y_string);
                float m = std::stof(m_string);
                float r = std::stof(r_string);

                int shape;
                if (s_string == "circle")
                {
                    shape = 1;
                }
                else if (s_string == "rectangle" || s_string == "")
                {
                    shape = 0;
                }
                else
                {
                    logger.error("Unknown shape type was given! Drawing a rectangle");
                    shape = 0;
                }
                engine_ptr->request(CreateBodyRequest{ glm::vec2(x, y), 
                                                       glm::vec2(0.0f, 0.0f),
                                                       m,
                    shape,
                    r/2},
                    CreateRenderObjectRequest
                    {
                    glm::vec2(x, y),
                    std::vector<float> {-r/2, -r/2, 
                                        r/2, -r/2,
                                         r/2, r/2, 
                                         -r/2, r/2},
                    shape,
                    r/2
                    });
            }
            catch (const std::invalid_argument& e) {
                logger.error("Invalid float was given");
            }
            catch (const std::out_of_range& e) {
                logger.error("Float out of range was given");
            }

        });
    add_command(create);

    // createStatic [x] [y] [shape] [size] - creates a static rigid body in given coordinates
    Command createStatic = Command("createStatic");
    createStatic.add_description("createStatic [x] [y] [shape] [size] - creates a static rigid body in given coordinates");
    createStatic.set_callback([this](std::string args)
        {
            if (args.empty())
            {
                logger.error("Invalid arguments given");
                return;
            }

            std::istringstream iss(args);
            std::string x_string;
            std::string y_string;
            std::string s_string;
            std::string r_string;

            if (!(iss >> x_string >> y_string >> s_string >> r_string))
            {
                logger.error("Not enough values were given!");
                return;
            }

            try
            {
                float x = std::stof(x_string);
                float y = std::stof(y_string);
                float r = std::stof(r_string);

                int shape;
                if (s_string == "circle")
                {
                    shape = 1;
                }
                else if (s_string == "rectangle" || s_string == "")
                {
                    shape = 0;
                }
                else
                {
                    logger.error("Unknown shape type was given! Drawing a rectangle");
                    shape = 0;
                }
                engine_ptr->request(CreateBodyRequest{ glm::vec2(x, y),
                                                       glm::vec2(0.0f, 0.0f),
                                                       0.0f,
                    shape,
                    r / 2, true },
                    CreateRenderObjectRequest
                    {
                    glm::vec2(x, y),
                    std::vector<float> {-r / 2, -r / 2,
                                        r / 2, -r / 2,
                                         r / 2, r / 2,
                                         -r / 2, r / 2},
                    shape,
                    r / 2
                    });
            }
            catch (const std::invalid_argument& e) {
                logger.error("Invalid float was given");
            }
            catch (const std::out_of_range& e) {
                logger.error("Float out of range was given");
            }

        });
    add_command(createStatic);

    // setCameraPos [x] [y] - sets camera pos to given coordinates
    Command setCameraPos = Command("setCameraPos");
    setCameraPos.add_description("setCameraPos [x] [y] - sets camera pos to given coordinates");
    setCameraPos.set_callback([this](std::string args)
        {
            if (args.empty())
            {
                logger.error("Invalid arguments given");
                return;
            }

            std::istringstream iss(args);
            std::string x_string;
            std::string y_string;

            if (!(iss >> x_string >> y_string))
            {
                logger.error("Not enough values were given!");
                return;
            }
            try
            {
                float x = std::stof(x_string);
                float y = std::stof(y_string);
                this->engine_ptr->graphicsEngine.setCameraPos(glm::vec2(x, y));
            }
            catch (const std::invalid_argument& e) {
                logger.error("Invalid float was given");
            }
            catch (const std::out_of_range& e) {
                logger.error("Float out of range was given");
            }

        });
    add_command(setCameraPos);

    // setBodyPos [id] [x] [y] - sets body pos to given coordinates
    Command setBodyPos = Command("setBodyPos");
    setBodyPos.add_description("setBodyPos [id] [x] [y] - sets body pos to given coordinates");
    setBodyPos.set_callback([this](std::string args)
        {
            if (args.empty())
            {
                logger.error("Invalid arguments given");
                return;
            }

            std::istringstream iss(args);
            std::string i_string;
            std::string x_string;
            std::string y_string;

            if (!(iss >> i_string >> x_string >> y_string))
            {
                logger.error("Not enough values were given!");
                return;
            }
            try
            {
                int id = std::stoi(i_string);
                float x = std::stof(x_string);
                float y = std::stof(y_string);
                this->engine_ptr->physicsEngine.setPos(id, glm::vec2(x, y));
            }
            catch (const std::invalid_argument& e) {
                logger.error("Invalid float was given");
            }
            catch (const std::out_of_range& e) {
                logger.error("Float out of range was given");
            }

        });
    add_command(setBodyPos);

    // setBodyVelocity [id] [x] [y] - sets body velocity to given values
    Command setBodyVelocity = Command("setBodyVelocity");
    setBodyVelocity.add_description("setBodyVelocity [id] [x] [y] - sets body velocity to given values");
    setBodyVelocity.set_callback([this](std::string args)
        {
            if (args.empty())
            {
                logger.error("Invalid arguments given");
                return;
            }

            std::istringstream iss(args);
            std::string i_string;
            std::string x_string;
            std::string y_string;

            if (!(iss >> i_string >> x_string >> y_string))
            {
                logger.error("Not enough values were given!");
                return;
            }
            try
            {
                int id = std::stoi(i_string);
                float x = std::stof(x_string);
                float y = std::stof(y_string);
                this->engine_ptr->physicsEngine.setVelocity(id, glm::vec2(x, y));
            }
            catch (const std::invalid_argument& e) {
                logger.error("Invalid float was given");
            }
            catch (const std::out_of_range& e) {
                logger.error("Float out of range was given");
            }

        });
    add_command(setBodyVelocity);

    // throwBody [id] [velocity] [angle (degrees)] - throws body with given velocity and with given angle to the earth
    Command throwBody = Command("throwBody");
    throwBody.add_description("throwBody [id] [velocity] [angle (degrees)] - throws body with given velocity and with given angle to the earth");
    throwBody.set_callback([this](std::string args)
        {
            if (args.empty())
            {
                logger.error("Invalid arguments given");
                return;
            }

            std::istringstream iss(args);
            std::string i_string;
            std::string v_string;
            std::string a_string;

            if (!(iss >> i_string >> v_string >> a_string))
            {
                logger.error("Not enough values were given!");
                return;
            }
            try
            {
                int id = std::stoi(i_string);
                float v = std::stof(v_string);
                float a = std::stof(a_string);
                float vx = glm::cos(a * 3.141592653589793f / 180.0f)*v;
                float vy = glm::sin(a * 3.141592653589793f / 180.0f)*v;
                this->engine_ptr->physicsEngine.setVelocity(id, glm::vec2(vx, vy));
            }
            catch (const std::invalid_argument& e) {
                logger.error("Invalid float was given");
            }
            catch (const std::out_of_range& e) {
                logger.error("Float out of range was given");
            }

        });
    add_command(throwBody);

    // moveCamera [x] [y] - moves camera on the given step
    Command moveCamera = Command("moveCamera");
    moveCamera.add_description("moveCamera [x] [y] - moves camera on the given step");
    moveCamera.set_callback([this](std::string args)
        {
            if (args.empty())
            {
                logger.error("Invalid arguments given");
                return;
            }

            std::istringstream iss(args);
            std::string x_string;
            std::string y_string;

            if (!(iss >> x_string >> y_string))
            {
                logger.error("Not enough values were given!");
                return;
            }
            try
            {
                float x = std::stof(x_string);
                float y = std::stof(y_string);
                this->engine_ptr->graphicsEngine.moveCamera(glm::vec2(x, y));
            }
            catch (const std::invalid_argument& e) {
                logger.error("Invalid float was given");
            }
            catch (const std::out_of_range& e) {
                logger.error("Float out of range was given");
            }

        });
    add_command(moveCamera);

    // setZoom [zoom] - sets zoom
    Command setZoom = Command("setZoom");
    setZoom.add_description("setZoom [zoom] - sets zoom");
    setZoom.set_callback([this](std::string args)
        {
            if (args.empty())
            {
                logger.error("Invalid arguments given");
                return;
            }

            std::istringstream iss(args);
            std::string z_string;

            if (!(iss >> z_string))
            {
                logger.error("Not enough values were given!");
                return;
            }
            try
            {
                float z = std::stof(z_string);
                this->engine_ptr->graphicsEngine.setZoom(z);
            }
            catch (const std::invalid_argument& e) {
                logger.error("Invalid float was given");
            }
            catch (const std::out_of_range& e) {
                logger.error("Float out of range was given");
            }

        });
    add_command(setZoom);

    // setG [g] - sets g (default = 9.8)
    Command setG = Command("setG");
    setG.add_description("setG [g] - sets g (default = 9.8)");
    setG.set_callback([this](std::string args)
        {
            if (args.empty())
            {
                logger.error("Invalid arguments given");
                return;
            }

            std::istringstream iss(args);
            std::string g_string;

            if (!(iss >> g_string))
            {
                logger.error("Not enough values were given!");
                return;
            }
            try
            {
                float g = std::stof(g_string);
                this->engine_ptr->physicsEngine.setG(g);
            }
            catch (const std::invalid_argument& e) {
                logger.error("Invalid float was given");
            }
            catch (const std::out_of_range& e) {
                logger.error("Float out of range was given");
            }

        });
    add_command(setG);

    // "zoom [zoom] - multiplies current zoom on giving value
    Command Zoom = Command("zoom");
    Zoom.add_description("zoom [zoom] - multiplies current zoom on giving value");
    Zoom.set_callback([this](std::string args)
        {
            if (args.empty())
            {
                logger.error("Invalid arguments given");
                return;
            }

            std::istringstream iss(args);
            std::string z_string;

            if (!(iss >> z_string))
            {
                logger.error("Not enough values were given!");
                return;
            }
            try
            {
                float z = std::stof(z_string);
                this->engine_ptr->graphicsEngine.Zoom(z);
            }
            catch (const std::invalid_argument& e) {
                logger.error("Invalid float was given");
            }
            catch (const std::out_of_range& e) {
                logger.error("Float out of range was given");
            }

        });
    add_command(Zoom);

    // setGrid [size] [thickness] - sets grid
    Command setGrid = Command("setGrid");
    setGrid.add_description("setGrid [size] [thickness] - sets grid");
    setGrid.set_callback([this](std::string args)
        {
            if (args.empty())
            {
                logger.error("Invalid arguments given");
                return;
            }

            std::istringstream iss(args);
            std::string s_string;
            std::string t_string;

            if (!(iss >> s_string >> t_string))
            {
                logger.error("Not enough values were given!");
                return;
            }
            try
            {
                float s = std::stof(s_string);
                float t = std::stof(t_string);
                this->engine_ptr->graphicsEngine.setGrid(s, t);
            }
            catch (const std::invalid_argument& e) {
                logger.error("Invalid float was given");
            }
            catch (const std::out_of_range& e) {
                logger.error("Float out of range was given");
            }

        });
    add_command(setGrid);

    // setFriction [id] [friction] - sets friction for a body
    Command setFriction = Command("setFriction");
    setFriction.add_description("setFriction [id] [friction] - sets friction for a body");
    setFriction.set_callback([this](std::string args)
        {
            if (args.empty())
            {
                logger.error("Invalid arguments given");
                return;
            }

            std::istringstream iss(args);
            std::string i_string;
            std::string f_string;

            if (!(iss >> i_string >> f_string))
            {
                logger.error("Not enough values were given!");
                return;
            }
            try
            {
                int i = std::stoi(i_string);
                float f = std::stof(f_string);
                this->engine_ptr->physicsEngine.setFriction(i, f);
            }
            catch (const std::invalid_argument& e) {
                logger.error("Invalid float or int was given");
            }
            catch (const std::out_of_range& e) {
                logger.error("Float out of range was given");
            }

        });
    add_command(setFriction);

    // setLogFrequency [n] - logs body information to a file each n ticks
    Command setLogFrequency = Command("setLogFrequency");
    setLogFrequency.add_description("setLogFrequency [n] - logs body information to a file each n ticks");
    setLogFrequency.set_callback([this](std::string args)
        {
            if (args.empty())
            {
                logger.error("Invalid arguments given");
                return;
            }

            std::istringstream iss(args);
            std::string n_string;

            if (!(iss >> n_string))
            {
                logger.error("Not enough values were given!");
                return;
            }
            try
            {
                int n = std::stoi(n_string);
                engine_ptr->physicsEngine.setLogTimeStep(n);
            }
            catch (const std::invalid_argument& e) {
                logger.error("Invalid float was given");
            }
            catch (const std::out_of_range& e) {
                logger.error("Float out of range was given");
            }

        });
    add_command(setLogFrequency);

    // pause - pauses simulation
    Command pause = Command("pause");
    pause.add_description("pause - pauses simulation");
    pause.set_callback([this](std::string args)
        {
            engine_ptr->physicsEngine.pause();
            logger.info("Simulation paused.");
        });
    add_command(pause);

    // toggle - toggle running state of simulation
    Command toggle = Command("toggle");
    toggle.add_description("toggle - toggle running state of simulation");
    toggle.set_callback([this](std::string args)
        {
            engine_ptr->physicsEngine.toggle();
            if (engine_ptr->physicsEngine.is_running())
            {
                logger.info("Simulation is running.");
            }
            else logger.info("Simulation paused.");
        });
    add_command(toggle);

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
    // Seperates command name and args
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


