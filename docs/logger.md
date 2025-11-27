# Logger
In this file I will describe how I made logging system.
## List of contents
1. [Overview](#overview)
1. [Non-static methods](#non-static-methods)
1. [Static methods](#static-methods)
1. [Sharing console](#sharing-console)

## Overview
The class implements two related systems. One is an instance created in each module to handle logging separately. I refer to these instances as individual `Loggers`.
Each of them has name, often the same as module. To log something in the module, you will use this.
The other runs in a seperate thread - I call it `LOGGER` thread.
It uses static variables because it serves as a shared system for all `Loggers`. It’s important not to use the static members directly — all functionality should be accessed through class instances.
## Non-static methods
First, you have to create a `Logger` instance. For example, suppose I need a logger for the `Engine` module.

	Logger engine_logger = Logger("Engine");

Now we can log information at any time, for example position of a body.
All I have to do is run the following method:

	engine_logger.info(std::format("Body position is: [{}, {}]", body.pos.x, body.pos.y));

And this will print to console the following:

	[HH:MM:SS] [Engine/INFO]: Body position is: 0.0, 0.0

Also you can use three more formatting types.
`warn` will make the message text yellow, and `error` will turn it red.
If you don't want to format your text at all, without module name, current time etc - you can use `raw`.
Raw messages are printed without formatting only to console, but they are printed to log files in the following format:

	[HH:MM:SS] [ModuleName/RAW]: <text>

Each of these functions also accepts argument `show`. It is a bool value, default is true. When true it is printed to both console and log files, otherwise - log files only. 

Also there is one more non-static method, but it's not public - `log`. 
Basically all the previous methods just runs this method with different arguments.
All it does is place message to the queue of pending log entries.
## Static methods
There are only 2 static methods - `start_logging` and `stop`. First is called in `main`, and the second is called in `Dispathcer::stop()` method.

The first one starts `LOGGER` thread. Whenever there are one or more messages in the queue, the logger thread prints them; otherwise, it sleeps.
The second one safely stops the thread.

## Sharing console
The main problem is that console is both output and input source.
That's why it's important to use `Logger` when you need something to be printed.
Each time something needs to be printed, the `LOGGER` temporarily clears the input prompt (including **'>>> '**), then prints output and adds **'>>> '** back to the new strings.
Input cursor is also moved. All of these is made via escape sequences.
