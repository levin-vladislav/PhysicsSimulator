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



enum class ArgType { INT, FLOAT, STRING };

struct ArgumentSpec
{
    std::string name;
    ArgType type;
};

class Command {
    std::string name;
    std::vector<ArgumentSpec> args;
    std::function<void(std::vector<std::any>)> callback;

public:
    Command() {}
    Command(const std::string& name) : name(name) {}

    void add_argument(const std::string& arg_name, ArgType type);

    void set_callback(std::function<void(std::vector<std::any>)> cb);

    const std::string& get_name() const;
    const std::vector<ArgumentSpec>& get_args() const;

    void execute(const std::vector<std::string>& input_args) const;
};


class Interpreter
{
public:
	Interpreter();

	void run_command(std::string command);
    void add_command(Command command);
    void init_commands();
	

private:
	Logger logger;

    std::unordered_map <std::string, Command> commands;

};





