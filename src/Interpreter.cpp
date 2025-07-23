#include "Interpreter.h"
#include <iostream>

Interpreter::Interpreter() : logger(Logger("Interpreter"))
{
	logger.info("Initialized.");
}