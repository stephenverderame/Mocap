#include "Renderer.h"
#include "Shader.h"
#include "RenderObject.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>



Renderer::Renderer(int width, int height) : w(width), h(height)
{
	shader = std::make_unique<Shader>("renderVert.glsl", "renderFrag.glsl");
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.f);
	glm::mat4 view = glm::lookAt(glm::vec3(0, 1, -3), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
	glm::mat4 projView = proj * view;
	shader->use();
	shader->setMat4("projView", projView);
	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);
	glGenFramebuffers(2, fbos);
	glGenTextures(2, texs);
	glGenRenderbuffers(2, rbos);

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texs[0]);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA, width, height, GL_TRUE);
//	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindRenderbuffer(GL_RENDERBUFFER, rbos[0]);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, fbos[0]);
	glEnable(GL_MULTISAMPLE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texs[0], 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbos[0]);
	int s = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(s != GL_FRAMEBUFFER_COMPLETE) printf("%x\n", s);
	glViewport(0, 0, width, height);
	
/*	glBindFramebuffer(GL_FRAMEBUFFER, fbos[1]);

	glBindTexture(GL_TEXTURE_2D, texs[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindRenderbuffer(GL_RENDERBUFFER, rbos[1]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texs[1], 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbos[1]);
	int s = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(s != GL_FRAMEBUFFER_COMPLETE) printf("%x\n", s);
	glViewport(0, 0, width, height);*/
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


Renderer::~Renderer()
{
	glDeleteRenderbuffers(2, rbos);
	glDeleteTextures(2, texs);
	glDeleteFramebuffers(2, fbos);
}

void Renderer::draw(const class RenderObject & obj) 
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbos[0]);
	shader->use();
	shader->setMat4("model", obj.getTransform());
	obj.draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::draw(const std::unique_ptr<class RenderObject> & obj)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbos[0]);
	shader->use();
	shader->setMat4("model", obj->getTransform());
	obj->draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::clear() 
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbos[0]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.5, 0.5, 0.5, 1.0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.5, 0.5, 0.5, 1.0);
}

void Renderer::bindTarget()
{
//	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbos[1]);
//	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbos[0]);
//	glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texs[0]);
	
}
