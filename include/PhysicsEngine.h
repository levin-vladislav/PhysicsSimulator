#pragma once
#include "Logger.h"
#include <atomic>
#include <algorithm>
#include "utils.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <box2d/box2d.h>



class PhysicsEngine
{
	// Object managing the physics of the simulation
public:
	PhysicsEngine();
	void update(float dt);
	bool is_running();

	void init();

	void log_body(int id);
	void log_bodies(bool show = true);

	std::queue<CreateBodyRequest> body_queue;

	int create_body(CreateBodyRequest request); // Function that accept a request of creating a body
	
	inline std::vector<b2BodyId> get_bodies() { return bodies; }

	inline std::unordered_map<int, size_t> get_ids() { return id2index; }

	float g = 9.8f;

	inline void setPos(int id, glm::vec2 pos) {
		b2Vec2 Pos = b2Vec2(pos.x, pos.y);
		b2BodyId bodyId = bodies[id2index[id]];

		b2Body_SetTransform(bodyId, Pos, b2Body_GetRotation(bodyId));
		b2Body_SetAwake(bodyId, true);
	}

	inline void setVelocity(int id, glm::vec2 velocity) {
		b2Vec2 vel = b2Vec2(velocity.x, velocity.y);
		b2BodyId bodyId = bodies[id2index[id]];

		b2Body_SetLinearVelocity(bodyId, vel);
	}

	inline glm::vec2 getPos(int id)
	{
		b2BodyId bodyId = bodies[id2index[id]];
		b2Vec2 pos = b2Body_GetPosition(bodyId);
		return glm::vec2(pos.x, pos.y);
	}

	inline float getRotation(int id)
	{
		b2BodyId bodyId = bodies[id2index[id]];
		b2Rot rot = b2Body_GetRotation(bodyId);
		return glm::atan(rot.s / rot.c);
	}

	inline glm::vec2 getVelocity(int id)
	{
		b2BodyId bodyId = bodies[id2index[id]];
		b2Vec2 vel = b2Body_GetLinearVelocity(bodyId);
		return glm::vec2(vel.x, vel.y);
	}

	void setGroundFriction(float friction);

	inline void setG(float g) { this->g = g; }

	void setFriction(int id, float friction);

	std::string get_info(int id);

	inline void pause() { can_update.store(false); }
	inline void toggle() { can_update.store(!can_update.load()); }

	inline void setLogTimeStep(int n) { logTimeStep = n; }

	void stop();

private:
	std::vector<b2BodyId> bodies; // Vector of pointers to bodies
	std::unordered_map<int, size_t> id2index; // Map of bodies' ids to index in 'bodies' vector
	Logger logger;
	std::atomic<bool> running;
	std::atomic<bool> can_update;

	b2WorldDef worldDef;
	b2WorldId worldId;

	b2BodyDef groundBodyDef;
	b2BodyId groundId;
	b2Polygon groundBox;
	b2ShapeDef groundShapeDef;

	int logTimeStep = 15;

	int log_i = 0;

	int subStepCount = 4;

	};

