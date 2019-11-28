#pragma once
#include <initializer_list>
#include <functional>
struct dims
{
	unsigned int x, y, w, h;
};
class Control
{
	unsigned int textures[10];
	unsigned int activeTexture, numTextures;
	unsigned int x, y, w, h;
public:
	Control(std::initializer_list<const char *> textures, unsigned int x, unsigned int y, unsigned int w, unsigned int h);
	dims getDimensions();
	void bindTarget();
	void setActiveTexture(unsigned int tId) { activeTexture = tId; }
	~Control();
public:
	std::function<void()> mouseClick, mouseOver, mouseOut;
};

