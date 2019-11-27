#pragma once
#include <memory>
struct capImpl;
class RenderCapture
{
	std::unique_ptr<capImpl> pimpl;
public:
	RenderCapture(unsigned int width, unsigned int height, unsigned int outWidth = ~0, unsigned int outHeight = ~0, unsigned int fps = 24);
	~RenderCapture();
	void capture();
	void setFrameRate(unsigned int fps);
	void saveCapture(const char * filename);
};

