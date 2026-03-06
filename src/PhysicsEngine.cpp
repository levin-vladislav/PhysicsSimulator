#include "PhysicsEngine.h"
#include <iostream>


// PhysicsEngine

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
		if (log_i++ == logTimeStep)
		{
			log_bodies(false);
			log_i = 0;
		}
	}
}

int PhysicsEngine::create_body(CreateBodyRequest request)
{
	logger.info(std::format("world index: {}", worldId.index1));
	b2BodyDef bodyDef = b2DefaultBodyDef();
	
	bodyDef.type = b2_dynamicBody;
	if (!request.isStatic)
	{
		bodyDef.type = b2_dynamicBody;
	}
	else
	{ 
		bodyDef.type = b2_staticBody;
	}

	bodyDef.position = b2Vec2(request.pos.x, request.pos.y);

	b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.density = request.mass / (request.radius * request.radius);
	shapeDef.material.friction = 0.3f;
	shapeDef.material.restitution = 1.0f;

	if (request.shape == 0)
	{
		b2Polygon box = b2MakeBox(request.radius, request.radius);
		b2CreatePolygonShape(bodyId, &shapeDef, &box);
	}
	else if (request.shape == 1)
	{
		b2Circle circle = b2Circle();
		circle.radius = request.radius;
		circle.center = b2Vec2(0.0f, 0.0f);
		b2CreateCircleShape(bodyId, &shapeDef, &circle);
	}
	

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

void PhysicsEngine::setFriction(int id, float friction)
{
	b2ShapeId shapes[8];
	b2BodyId bodyId = bodies[id2index[id]];
	int shapeCount = b2Body_GetShapes(bodyId, shapes, 8);

	for (int i = 0; i < shapeCount; i++)
	{
		b2Shape_SetFriction(shapes[i], friction);
	}
}

void PhysicsEngine::setGroundFriction(float friction)
{
	b2ShapeId shapes[8];
	int shapeCount = b2Body_GetShapes(groundId, shapes, 8);

	for (int i = 0; i < shapeCount; i++)
	{
		b2Shape_SetFriction(shapes[i], friction);
	}
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

