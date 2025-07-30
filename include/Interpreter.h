#pragma once
#include "Logger.h"
#include <vector>
#include <functional>
#include <unordered_map>
#include <any>
#include <iostream>
#include <format>
#include <chrono>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <string>
#include "Engine.h"

enum class ArgType { INT, FLOAT, STRING };

struct ArgumentSpec
{
    std::string name;
    ArgType type;
};

class Command {
    std::string name;
    std::function<void(std::string)> callback;
    std::string description;

public:
    Command() {}
    Command(const std::string& name) : name(name) {}


    void set_callback(std::function<void(std::string cb)>);

    const std::string& get_name() const;

    void execute(const std::string& input_args) const;

    void add_description(std::string description);
    std::string get_description();
};


class Interpreter
{
public:
	Interpreter();

	void run_command(std::string command);
    void add_command(Command command);
    void init_commands();
    void execute(std::string command, std::string args);
	
    Engine* engine_ptr;

private:
	Logger logger;

    std::unordered_map <std::string, Command> commands;

};





