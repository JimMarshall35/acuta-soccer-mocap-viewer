#pragma once
#include "MocapFileDefinitions.h"
#include <glm/glm.hpp>

struct MocapFrame {
	glm::vec3 points[PlayerPoints];
};
