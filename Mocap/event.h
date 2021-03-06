#pragma once
enum class controlEvent
{
	null, click, hover, release
};
struct Event
{
	controlEvent ev;
	unsigned int x, y;
	void * data;
};

class Observer
{
public:
	virtual void notify(Event e) = 0;
};