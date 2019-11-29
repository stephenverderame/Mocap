#pragma once
#include "View.h"
#include "Model.h"
#include <memory>
struct bvmImpl;
class BufferViewModel : public View, public Model
{
private:
	std::unique_ptr<bvmImpl> pimpl;
public:
	BufferViewModel();
	~BufferViewModel();
	void startRecording() override;
	void finishRecording() override;
	void addTrackingObj(trackingObjects obj) override;
	void savePose(trackingObjects obj, glm::mat4 & pose, long frame = -1) override;
	void savePose(trackingObjects obj, glm::mat4 && pose, long frame = -1) override;
	glm::mat4 getDeviceTransform(trackingObjects d) override;
	glm::mat4 getDeviceTransform(trackingObjects d, long frame) override;
	std::vector<trackingObjects> getAllObjs() override;

	void serialize(const char * file);
	void load(const char * file);

	void writeLog(const char * file, class Logger & log);

	void clearBuffer();

	long getDuration();
};
