#pragma once
#include "trackingObjects.h"
#include <vector>
class Logger
{
public:
	virtual void out(const char * filename, const std::vector<std::vector<struct pose>> & data, trackingObjects * objs, 
		int numObjects, int numFrames, long duration) = 0;

};

