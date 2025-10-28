# PhysicsEngine
This file explains how physics is handled in the simulation.
## List of contents
1. [Body](#body)
2. [RigidBody](#rigidbody)
3. [PhysicsEngine class](#physicsengine)
## Body
This class represents a basic physical body. It defines the fundamental properties and methods that every body should have, such as mass, position, velocity, and acceleration, along with getters and setters.

`~Body()` and `update(float dt)` methods of the class are virtual, so you can't initialize `Body` object and run `update` function.
You can only inherit from this class and override those methods.

Each body has its own unique `id`. With this it can be easily managed.
## RigidBody
This class inherites from `Body` and adds additional properties and methods for running basic physics processes, such as angle and its derivatives, inertia and some accumulators: force, torque and impulse in case of collision.
When creating `RigidBody` you must specify its type using a `BodyType` value. There are three types of `RigidBodies`:
- `Static` - immovable objects, has collider. Can be used as surfaces.
- `Kinematic` - Objects, that can move, but is not affected by gravity. 
- `Dynamic` - Regular objects, that can move and are affected by gravity.

## PhysicsEngine
The class that manages the whole simulation.

Bodies are stored in a `std::vector` of smart pointers called `bodies`. An unordered_map named `id2index` maps each body’s `id` to its index in the `bodies` vector.

There are some methods to deal with bodies:
- `int add_body(std::unique_ptr<Body> body)` - adds the pointer to the body to the vector `bodies` and returns its `id`
- `Body* get_body(int id)` - returns a pointer to a body by its `id`.
- `void remove_body(int id)` - removes a body with given `id`.
- `void create_body(CreateBodyRequest request)` - creates a body, getting information from the given `CreateBodyRequest` object.

`CreateBodyRequest` contains position of the body, its velocity, mass and type.

### IMPORTANT! RIGHT NOW THIS METHOD ONLY CREATES RIGID BODIES!

The class also stores static variables for general constants such as gravity (`g`), linear damping, and angular damping.

