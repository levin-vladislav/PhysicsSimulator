#pragma once
#include "Logger.h"
#include <atomic>
#include <algorithm>
#include <glm/vec2.hpp>
#include "utils.h"
#include <string>
#include <vector>
#include <unordered_map>

class Body
{
	// Basic body object. Contains all primary methods and properties
public:
	int id;
	float mass;

	virtual void update(float dt) = 0;
	virtual ~Body();

	inline glm::vec2 getPos() const { return pos; }
	
	inline glm::vec2 getVelocity() const { return velocity; }
	inline glm::vec2 getAcceleration() const { return acceleration; };

	inline void setPos(glm::vec2 pos) { this->pos = pos; }
	inline void setVelocity(glm::vec2 velocity) { this->velocity = velocity; }
	inline void setAcceleration(glm::vec2 acceleration) { this->acceleration = acceleration; }

	inline void move(glm::vec2 translation) { this->pos += translation; }
	inline void addVelocity(glm::vec2 velocity) { this->velocity += velocity; }
	inline void addAcceleration(glm::vec2 acceleration) { this->acceleration += acceleration; }

	std::string get_info();


protected:
	glm::vec2 pos;
	glm::vec2 velocity;
	glm::vec2 acceleration;
	
};

class RigidBody : public Body
{
	// A sub-class for rigid bodies. 
public:
	RigidBody(BodyType type);

	void update(float dt) override;

	// Transform
	float angle, angularVelocity, angularAcceleration;

	// Mass
	float invMass, inertia, invInertia;

	// Forces
	glm::vec2 forceAccumulator;
	float torqueAccumulator;
	
	// Collision
	glm::vec2 impulseAccumulator;


	bool isTrigger;
	BodyType type;

		
	inline void applyForce(glm::vec2 force) { forceAccumulator += force; }
	inline void applyImpulse(glm::vec2 impulse) { impulseAccumulator += impulse; }

};

class PhysicsEngine
{
	// Object managing the physics of the simulation
public:
	PhysicsEngine();
	void update(float dt);
	bool is_running() const;

	void log_body(int id);
	void log_bodies();

	void create_body(CreateBodyRequest request); // Function that accept a request of creating a body
	int add_body(std::unique_ptr<Body> body);
	Body* get_body(int id);
	void remove_body(int id);

	static float linearDamping;
	static float angularDamping;

	static float g;

private:
	std::vector<std::unique_ptr<Body>> bodies; // Vector of pointers to bodies
	std::unordered_map<int, size_t> id2index; // Map of bodies' ids to index in 'bodies' vector
	Logger logger;
	std::atomic<bool> running;

	int next_id = 0; // Stores id of last body added
};

