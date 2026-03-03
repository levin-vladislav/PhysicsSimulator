#include "PhysicsEngine.h"
#include <iostream>


// PhysicsEngine
float PhysicsEngine::linearDamping = 1.0;
float PhysicsEngine::angularDamping = 1.0;
float PhysicsEngine::g = 9.8f;

PhysicsEngine::PhysicsEngine() : running(true), can_update(true), logger(Logger("PhysicsEngine")) {}

void PhysicsEngine::init()
{
	worldDef = b2DefaultWorldDef();
	worldDef.gravity = b2Vec2(0.0f, -g);
	worldId = b2CreateWorld(&worldDef);

	groundBodyDef = b2DefaultBodyDef();
	groundBodyDef.position = b2Vec2(0.0f, -5.5f);
	groundBodyDef.type = b2_staticBody;
	
	groundId = b2CreateBody(worldId, &groundBodyDef);
	groundBox = b2MakeBox(25.0f, 5.0f);
	groundShapeDef = b2DefaultShapeDef();
	b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

	logger.info("Initialized.");
	logger.info(std::format("Ground pos: {} {}", 0.0f, -5.0f));
}

bool PhysicsEngine::is_running()
{
	return running.load();
}

void PhysicsEngine::update(float dt)
{
	if (!body_queue.empty())
	{
		create_body(body_queue.front());
		body_queue.pop();
	}
	if (can_update.load())
	{
		b2World_Step(worldId, 1.0f/60.0f, subStepCount);
		log_bodies(false);
	}
}


int PhysicsEngine::create_body(CreateBodyRequest request)
{
	logger.info(std::format("world index: {}", worldId.index1));
	b2BodyDef bodyDef = b2DefaultBodyDef()
	
	bodyDef.type = b2_dynamicBody;
	bodyDef.position = b2Vec2(request.pos.x, request.pos.y);

	b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.density = 1.0f;
	shapeDef.material.friction = 0.3f;

	b2Polygon box = b2MakeBox(0.5f, 0.5f);

	b2CreatePolygonShape(bodyId, &shapeDef, &box);

	b2Body_SetLinearVelocity(bodyId, b2Vec2(0.0f, 0.0f));

	id2index[request.id] = bodies.size();
	bodies.push_back(bodyId);
	

	logger.info(std::format("id {} pos [{}, {}]; velocity [{}, {}]",
		request.id,
		request.pos.x,
		request.pos.y,
		request.velocity.x,
		request.velocity.y));

	return request.id;
}


void PhysicsEngine::log_body(int id)
{
	logger.info(get_info(id));
}

void PhysicsEngine::log_bodies(bool show)
{
	for (auto it : id2index)
	{
		logger.info(get_info(it.first), show);
	}
}



std::string PhysicsEngine::get_info(int id)
{
	glm::vec2 pos = getPos(id);
	glm::vec2 velocity = getVelocity(id);
	return std::format("Body {}: pos [{}, {}]; velocity [{}, {}]", 
		id, pos.x, pos.y,
		velocity.x, velocity.y);
}

void PhysicsEngine::stop()
{
	b2DestroyWorld(worldId);
	running.store(false);
}

