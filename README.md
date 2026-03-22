# 2D Physics Engine
This is a small 2D engine for running simple rigidbody simulations.
## What can it do?
1. Add the bodies with configurable properties like mass, shape, size, etc.
2. Run pre-made scripts using program's own language.
3. Log information to the console and save it to the file.
# Installation
Download the release
[here](https://github.com/levin-vladislav/PhysicsSimulator/raw/refs/heads/master/build.zip).
Unzip archive to any place on your PC and run it.
# How to use it
After running the program, you will see 3 windows: console, simulation layout and GUI for managing simulations. Add new bodies with button '+' or change the properties of existing bodies. Move the camera by dragging mouse in the window with simulation! Start or stop simulation with button 'toggle'.

Also you can use console for wider functionality. Example of code:
```
create 0 0 1 rectangle 1 // creates a square with side 1 in coordinates 0 0 with mass 1.
create 1 1 1 circle 1 // creates a circle with diameter 1 in coordinates 1 1 with mass 1.
toggle // freezes or unfreezes simulation.
```
