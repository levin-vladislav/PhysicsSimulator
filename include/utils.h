#pragma once
#include <glm/vec2.hpp>
#include <glm/mat2x2.hpp>
#include <unordered_map>
#include <string>
#include <filesystem>
#include <ctime>
#include <iostream>
#include <sstream>
#include <fstream>
#include <chrono>
#include <queue>


enum class BodyType { STATIC, DYNAMIC, KINEMATIC };

struct CreateBodyRequest
{
	glm::vec2 pos;
	glm::vec2 velocity;
	float mass;
	BodyType type;
};

struct CreateRenderObjectRequest
{
	glm::vec2 pos;
	std::vector<float> vertices;
	const char* vertexShaderPath;
	const char* fragmentShaderPath;
};

std::string get_date();
std::string get_time();

std::string readFile(std::string path);


