#pragma once
#include "trackingObjects.h"
#include <glm.hpp>
class View {
public:
	virtual void startRecording() = 0;
	virtual void finishRecording() = 0;
	virtual void addTrackingObj(trackingObjects obj) = 0;
	virtual void savePose(trackingObjects obj, glm::mat4 & pose, long frame = -1) = 0;
	virtual void savePose(trackingObjects obj, glm::mat4 && pose, long frame = -1) = 0;
};