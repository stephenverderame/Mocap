#include "RenderView.h"
#include "Renderer.h"
#include "RenderObject.h"
#include <time.h>


RenderView::RenderView(int w, int h)
{
	renderer = std::make_unique<Renderer>(w, h);
}


RenderView::~RenderView()
{
}

void RenderView::addTrackingObj(trackingObjects obj)
{
	objs.emplace(obj, std::make_unique<Cube>());
}

void RenderView::savePose(trackingObjects obj, glm::mat4 & pose, long frame) 
{
	objs[obj]->setTransform(pose);
	if (obj == trackingObjects::head) objs[obj]->scale(glm::vec3(0.35));
	else objs[obj]->scale(glm::vec3(0.2));
}

void RenderView::savePose(trackingObjects obj, glm::mat4 && pose, long frame)
{
	objs[obj]->setTransform(pose);
	if (obj == trackingObjects::head) objs[obj]->scale(glm::vec3(0.35));
	else objs[obj]->scale(glm::vec3(0.2));
}

void RenderView::draw()
{
	renderer->clear();
	for (auto it = objs.cbegin(); it != objs.cend(); ++it) {
		renderer->draw(it->second);
	}
	renderer->bindTarget();
}

void RenderView::startRecording()
{

}

void RenderView::finishRecording()
{

}
