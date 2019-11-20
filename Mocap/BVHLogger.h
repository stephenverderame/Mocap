#pragma once
#include "Logger.h"
class BVHLogger : public Logger
{
public:
	BVHLogger();
	~BVHLogger();
	void out(const char * filename, struct pose * data, trackingObjects * objs, int numObjects, int numFrames, long duration) override;
};

