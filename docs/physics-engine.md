# PhysicsEngine
In this file I'm going to describe how I deal with physics in the simulation.
## List of contents
1. [Body](#body)
2. [RigidBody](#rigidbody)
3. [PhysicsEngine class](#physicsengine)
## Body
This class is a basic body object. It contains all primary methods and properties any body should have,
such as mass, position and its derivatives, and methods to get or change them.

`~Body()` and `update(float dt)` methods of the class are virtual, so you can't initialize `Body` object and run `update` function.
You can only inheritate this class and override them.

Each body has its own unique `id`. With this it can be easily managed.
## RigidBody
This class inheritates `Body` class and adds some properties and methods for running basic physics processes, such as angle and its derivatives, inertia and some accumulators: force, torque and impulse in case of collision.
Also when you create `RigidBody` you should specify its type with `BodyType` object. There are three types of `RigidBodies`:
- `Static` - immovable objects, has collider. Can be used as surfaces.
- `Kinematic` - Objects, that can move, but is not affected by gravity. 
- `Dynamic` - Regular objects, that can move and are affected by gravity.

## PhysicsEngine
The class that manages the whole simulation.

Bodies are stored as vector of pointers called `bodies`. There is an unordered map called `id2index`, where the key is an `id` of a body, and a value is index of the pointer to this body in the vector `bodies`.

There are some methods to deal with bodies:
- `int add_body(std::unique_ptr<Body> body)` - adds the pointer to the body to the vector `bodies` and returns its `id`
- `Body* get_body(int id)` - returns a pointer to a body by its `id`.
- `void remove_body(int id)` - removes a body with given `id`.
- `void create_body(CreateBodyRequest request)` - creates a body, getting information from the given `CreateBodyRequest` object.

`CreateBodyRequest` contains position of the body, its velocity, mass and type.

### IMPORTANT! RIGHT NOW THIS METHOD ONLY CREATES RIGID BODIES!

Also there are stored static variables for general constants, such as g, linear damping and angular damping.

