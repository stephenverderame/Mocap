#pragma once
#include <glm.hpp>
#include "trackingObjects.h"
#include <vector>
class Model {
public:
	virtual glm::mat4 getDeviceTransform(trackingObjects o, long frame) = 0;
	virtual glm::mat4 getDeviceTransform(trackingObjects d) = 0;
	virtual std::vector<trackingObjects> getAllObjs() = 0;
};