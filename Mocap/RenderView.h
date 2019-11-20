#pragma once
#include "View.h"
#include <memory>
#include <unordered_map>
struct Pose {
	long time;
	trackingObjects obj;
	glm::mat4 transform;
};
class RenderView : public View
{
private:
	std::unique_ptr<class Renderer> renderer;
	std::unordered_map<trackingObjects, std::unique_ptr<class RenderObject>> objs;
public:
	RenderView(int w, int h);
	void startRecording() override;
	void finishRecording() override;
	void addTrackingObj(trackingObjects obj) override;
	void savePose(trackingObjects obj, glm::mat4 & pose, long frame = -1) override;
	void savePose(trackingObjects obj, glm::mat4 && pose, long frame = -1) override;
	void draw();
	~RenderView();
};

