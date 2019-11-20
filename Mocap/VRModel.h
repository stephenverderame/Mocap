#pragma once
#include "Model.h"
#include <memory>
struct vrImpl;
class VRModel : public Model
{
private:
	std::unique_ptr<vrImpl> pimpl;
public:
	VRModel();
	glm::mat4 getDeviceTransform(trackingObjects d) override;
	glm::mat4 getDeviceTransform(trackingObjects o, long f) override;
	std::vector<trackingObjects> getAllObjs() override;
	~VRModel();
};

