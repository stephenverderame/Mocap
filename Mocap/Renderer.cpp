#include "Renderer.h"
#include "Shader.h"
#include "RenderObject.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>



Renderer::Renderer(int width, int height)
{
	shader = std::make_unique<Shader>("renderVert.glsl", "renderFrag.glsl");
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.f);
	glm::mat4 view = glm::lookAt(glm::vec3(0, 1, -3), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
	glm::mat4 projView = proj * view;
	shader->use();
	shader->setMat4("projView", projView);
	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);
}


Renderer::~Renderer()
{
}

void Renderer::draw(const class RenderObject & obj) 
{
	shader->use();
	shader->setMat4("model", obj.getTransform());
	obj.draw();
}

void Renderer::draw(const std::unique_ptr<class RenderObject> & obj)
{
	shader->use();
	shader->setMat4("model", obj->getTransform());
	obj->draw();
}

void Renderer::clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 0.0, 0.0, 1.0);
}
