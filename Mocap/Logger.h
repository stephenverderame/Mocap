#pragma once
#include "trackingObjects.h"
class Logger
{
public:
	virtual void out(const char * filename, struct pose * data, trackingObjects * objs, 
		int numObjects, int numFrames, long duration) = 0;

};

