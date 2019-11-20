#pragma once
#include <time.h>
#include "trackingObjects.h"
#include <glm.hpp>
struct pose {
	clock_t time;
	trackingObjects obj;
	glm::mat4 transform;
};