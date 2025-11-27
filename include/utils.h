#pragma once
#include <glm/vec2.hpp>

enum class BodyType { STATIC, DYNAMIC, KINEMATIC };

struct CreateBodyRequest
{
	glm::vec2 pos;
	glm::vec2 velocity;
	float mass;
	BodyType type;
};




// std::string get_date() {}
// std::string get_time() {}


