#pragma once
#include "Logger.h"
class BVHLogger : public Logger
{
private:
	int fps;
public:
	BVHLogger();
	~BVHLogger();
	void resampleToFrameRate(int fps);
	void out(const char * filename, const std::vector<std::vector<struct pose>> & data, trackingObjects * objs, 
		int numObjects, int numFrames, long duration) override;
};

