# Logger
In this file I will describe how I made logging system.
## List of contents
1. [Overview](#overview)
1. [Non-static methods](#non-static-methods)
1. [Static methods](#static-methods)
1. [Sharing console](#sharing-console)

## Overview
In one class there are two system. One is  a class instance that is created in each module to log something seperately. These ones I call `Loggers`.
Each of them has name, often the same as module. To log something in the module, you will use this.
Another one is runned in a seperated thread - I call it `LOGGER` thread.
It uses static variables because it's a general system for all of the `Loggers`. Important thing is that you should never use the static because all the functionality should be realised via the instances of class.
## Non-static methods
First, you have to create a `Logger` instance. For exapmle, I need a logger for the `Engine`.

	Logger engine_logger = Logger("Engine");

Now we any time we want we can log some information, for example position of a body.
All I have to do is run the following method:

	engine_logger.info(std::format("Body position is: [{}, {}]", body.pos.x, body.pos.y));

And this will print to console the following:

	[HH:MM:SS] [Engine/INFO]: Body position is: 0.0, 0.0

Also you can use three more formatting types.
`warn` will make the message text yellow, and `error` will turn it red.
If you don't want to format your text at all, without module name, current time etc - you can use `raw`.

Also there is one more non-static method, but it's not public - `log`. 
Basically all the previous methods just runs this method with different arguments.
All it do is put message to the queue of messages to be printed.
## Static methods
There are only 2 static methods - `start_logging` and `stop`. First is executed in the `main` file, and the secong is executed in `Dispathcer.stop()` method.

The first one starts `LOGGER` thread. Whenever there is one or more messages in the queue it will print them, otherwise it will sleep.
The second one safely stops the thread.

## Sharing console
The main problem is that console is both output and input source.
That's why it's important to use `Logger` when you need something to be printed.
Each time there is something to be printed, `LOGGER` first remove all the input (including '>>> '), then prints output and adds '>>> ' back to the new strings.
Input cursor is also moved. All of these is made via escape sequences.