#include "Gui.h"
#include "Shader.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "Control.h"
#include <vector>

struct guiImpl
{
	std::unique_ptr<class Shader> shader;
	unsigned int rectVbo, rectVao;
	unsigned int w, h;
	std::vector<Control *> controls;
	size_t lastMouseIn = ~0;
};
const float vertices[] = {
	0.0, 0.0,
	1.0, 1.0,
	0.0, 1.0,

	0.0, 0.0,
	1.0, 0.0,
	1.0, 1.0
};

Gui::Gui(unsigned int w, unsigned int h)
{
	pimpl = std::make_unique<guiImpl>();
	pimpl->shader = std::make_unique<Shader>("postprocessVert.glsl", "postprocessFrag.glsl");
	glm::mat4 proj = glm::ortho(0.0f, (float)w, 0.0f, (float)h, -1.f, 1.f);
	auto & shader = pimpl->shader;
	shader->use();
	shader->setMat4("proj", proj);
	shader->setInt("aaSamples", 4);
	shader->setInt("tex", 0);
	shader->setInt("bg", 1);
	pimpl->h = h;
	pimpl->w = w;
	glGenVertexArrays(1, &pimpl->rectVao);
	glBindVertexArray(pimpl->rectVao);
	glGenBuffers(1, &pimpl->rectVbo);
	glBindBuffer(GL_ARRAY_BUFFER, pimpl->rectVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}


Gui::~Gui()
{
	glDeleteBuffers(1, &pimpl->rectVbo);
	glDeleteVertexArrays(1, &pimpl->rectVao);
}

void Gui::display()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glClearColor(0.0, 0.0, 0.0, 1.0);
	pimpl->shader->use();
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0));
	model = glm::scale(model, glm::vec3(pimpl->w, pimpl->h, 1.0));
	pimpl->shader->setMat4("model", model);
	pimpl->shader->setBool("ms", true);
	glBindVertexArray(pimpl->rectVao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	pimpl->shader->setBool("ms", false);
	for (auto control : pimpl->controls) {
		if (!control->isShown()) continue;
		control->bindTarget();
		auto d = control->getDimensions();
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(d.x, pimpl->h - d.y - d.h - 30, 0.0));
		model = glm::scale(model, glm::vec3(d.w, d.h, 1.0));
		pimpl->shader->setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	glEnable(GL_DEPTH_TEST);

}
void Gui::add(Control & c)
{
	pimpl->controls.push_back(&c);
}

void Gui::notify(Event e)
{
	bool mouseInControl = false;
	for (size_t i = 0; i < pimpl->controls.size(); ++i) {
		if (!pimpl->controls[i]->isShown() || !pimpl->controls[i]->isEnabled()) continue;
		auto c = pimpl->controls[i];
		auto dim = c->getDimensions();
		if (e.x >= dim.x && e.x <= dim.x + dim.w && e.y >= dim.y && e.y <= dim.y + dim.h) {
			if (e.ev == controlEvent::click)
				if (c->mouseClick != nullptr) c->mouseClick();
			if (e.ev == controlEvent::hover) {
				if (c->mouseIn != nullptr && pimpl->lastMouseIn != i) c->mouseIn();
				if (c->mouseOver != nullptr) c->mouseOver();
				if (pimpl->lastMouseIn != ~0 && i != pimpl->lastMouseIn && pimpl->controls[pimpl->lastMouseIn]->mouseOut != nullptr) 
					pimpl->controls[pimpl->lastMouseIn]->mouseOut();
				pimpl->lastMouseIn = i;
				mouseInControl = true;
			}
			else if (e.ev == controlEvent::release) {
				if (c->mouseRelease != nullptr) c->mouseRelease();
			}
		}
	}
	if (!mouseInControl && pimpl->lastMouseIn != ~0 && e.ev == controlEvent::hover) {
		if (pimpl->controls[pimpl->lastMouseIn]->mouseOut != nullptr) pimpl->controls[pimpl->lastMouseIn]->mouseOut();
		pimpl->lastMouseIn = ~0;
	}
}