#pragma once
#include <memory>
#include "event.h"
struct guiImpl;
class Gui : public Observer
{
	std::unique_ptr<guiImpl> pimpl;
public:
	void display();
	Gui(unsigned int width, unsigned int height);
	void add(class Control & ctrl);
	void notify(Event e) override;
	~Gui();
};

