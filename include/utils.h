#pragma once
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/mat2x2.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>
#include <string>
#include <filesystem>
#include <ctime>
#include <iostream>
#include <sstream>
#include <fstream>
#include <chrono>
#include <queue>


struct CreateBodyRequest
{
	glm::vec2 pos;
	glm::vec2 velocity;
	float mass;
	int id = 0;
};

struct CreateRenderObjectRequest
{
	glm::vec2 pos;
	std::vector<float> vertices;
	int id = 0;
};

std::string get_date();
std::string get_time();

std::string readFile(std::string path);


