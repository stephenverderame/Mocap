#pragma once
#include "View.h"
#include "Model.h"
#include <memory>
struct pvmImpl;
class PlaybackViewModel : public View, public Model
{
private:
	std::unique_ptr<pvmImpl> pimpl;
	bool playbackMode;
public:
	PlaybackViewModel();
	~PlaybackViewModel();
	void startRecording() override;
	void finishRecording() override;
	void addTrackingObj(trackingObjects obj) override;
	void savePose(trackingObjects obj, glm::mat4 & pose) override;
	void savePose(trackingObjects obj, glm::mat4 && pose) override;
	glm::mat4 getDeviceTransform(trackingObjects d) override;
	std::vector<trackingObjects> getAllObjs() override;
};

