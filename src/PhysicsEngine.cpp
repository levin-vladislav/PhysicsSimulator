#include "PhysicsEngine.h"
#include <iostream>


// PhysicsEngine
float PhysicsEngine::linearDamping = 1.0;
float PhysicsEngine::angularDamping = 1.0;
float PhysicsEngine::g = 9.8f;

PhysicsEngine::PhysicsEngine() : running(true), logger(Logger("PhysicsEngine"))
{
	logger.info("Initialized.");
}

bool PhysicsEngine::is_running() const
{
	return running.load();
}

void PhysicsEngine::update(float dt)
{

	for (auto& body : bodies)
	{
		if (!body) continue;
		body->update(dt);
		if (RigidBody* rigid = dynamic_cast<RigidBody*>(body.get()))
		{
			rigid->applyForce(glm::vec2(0.0f, -rigid->mass * g));
		}
		
	}
}

int PhysicsEngine::add_body(std::unique_ptr<Body> body)
{
	int id = next_id++;
	id2index[id] = bodies.size();
	body->id = id;
	bodies.push_back(std::move(body));
	return id;
}

void PhysicsEngine::remove_body(int id)
{
	auto it = id2index.find(id);
	if (it == id2index.end()) return;

	size_t index = it->second;
	size_t last = bodies.size() - 1;

	if (index != last) {
		std::swap(bodies[index], bodies[last]);
		int moved_id = bodies[index]->id;
		id2index[moved_id] = index;
	}

	bodies.pop_back();
	id2index.erase(id);
}

void PhysicsEngine::create_body(CreateBodyRequest request)
{
	auto body = std::make_unique<RigidBody>(request.type);
	body->setPos(request.pos);
	body->setVelocity(request.velocity);
	body->mass = request.mass;
	add_body(std::move(body));
	logger.info(std::format("pos [{}, {}]; velocity [{}, {}]", request.pos.x, request.pos.y,
		request.velocity.x, request.velocity.y));
}

void PhysicsEngine::log_body(int id)
{
	logger.info(bodies[id2index[id]]->get_info());
}

void PhysicsEngine::log_bodies()
{
	for (auto& body : bodies)
	{
		logger.info(body->get_info());
	}
}



// Body

Body* PhysicsEngine::get_body(int id)
{
	auto it = id2index.find(id);
	if (it == id2index.end()) return nullptr;
	return bodies[it->second].get();
}

std::string Body::get_info()
{
	return std::format("Body {}: pos [{}, {}]; velocity [{}, {}], acceleration [{}, {}]", 
		id, pos.x, pos.y,
		velocity.x, velocity.y, 
		acceleration.x, acceleration.y);
}

Body::~Body() {}



// RigidBody

RigidBody::RigidBody(BodyType type) :
	angle(0), angularVelocity(0), angularAcceleration(0),
	invMass(1), inertia(1), invInertia(1),
	forceAccumulator(glm::vec2(0,0)),
	torqueAccumulator(0),
	impulseAccumulator(0),
	type(type),
	isTrigger(false)
{
	pos = glm::vec2(0, 0);
	acceleration = glm::vec2(0, 0);
	velocity = glm::vec2(0, 0);
}

void RigidBody::update(float dt)
{

	// Computing velocity and acceleration from the accumulators
	velocity += impulseAccumulator * invMass;
	acceleration += forceAccumulator * invMass;
	
	// Computing new velocity
	velocity += acceleration * dt;

	// Computing position
	pos += velocity * dt;

	// Computing new angles and angular velocities etc
	angularAcceleration += torqueAccumulator * invInertia;
	angularVelocity += angularAcceleration * dt;
	angle += angularVelocity * dt;

	// Damping
	velocity *= PhysicsEngine::linearDamping;
	angularVelocity *= PhysicsEngine::angularDamping;

	// Resetting accumulators
	impulseAccumulator = glm::vec2(0.0f);
	forceAccumulator = glm::vec2(0.0f);
	torqueAccumulator = 0.0f;
	acceleration = glm::vec2(0.0f);
	angularAcceleration = 0.0f;

}

