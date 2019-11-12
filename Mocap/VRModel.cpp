#include "VRModel.h"
#include <headers\openvr.h>
struct vrImpl {
	vr::IVRSystem * system;
	unsigned int leftId = vr::k_unMaxTrackedDeviceCount, rightId = vr::k_unMaxTrackedDeviceCount;
};
glm::mat4 openVrToGlm(const vr::HmdMatrix34_t & m) {
	glm::mat4 result = glm::mat4(
		m.m[0][0], m.m[1][0], m.m[2][0], 0.0,
		m.m[0][1], m.m[1][1], m.m[2][1], 0.0,
		m.m[0][2], m.m[1][2], m.m[2][2], 0.0,
		m.m[0][3], m.m[1][3], m.m[2][3], 1.0f);
	return result;
}
VRModel::VRModel()
{
	pimpl = std::make_unique<vrImpl>();
	if (vr::VR_IsHmdPresent()) {
		vr::HmdError e;
		pimpl->system = vr::VR_Init(&e, vr::EVRApplicationType::VRApplication_Overlay);
		if (e != vr::VRInitError_None) {
			printf("VR startup error!\n");
		}
	}
	for (unsigned int i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i) {
		vr::ETrackedDeviceClass deviceType = pimpl->system->GetTrackedDeviceClass(i);
		if (deviceType == vr::ETrackedDeviceClass::TrackedDeviceClass_Controller && pimpl->system->IsTrackedDeviceConnected(i)) {
			vr::ETrackedControllerRole controllerRole = pimpl->system->GetControllerRoleForTrackedDeviceIndex(i);
			if (controllerRole == vr::TrackedControllerRole_LeftHand) pimpl->leftId = i;
			else if (controllerRole == vr::TrackedControllerRole_RightHand) pimpl->rightId = i;
		}
	}
}


VRModel::~VRModel()
{
	vr::VR_Shutdown();
}

glm::mat4 VRModel::getDeviceTransform(trackingObjects d) 
{
	vr::TrackedDevicePose_t pose;
	vr::VRControllerState_t state;
	switch (d) {
	case trackingObjects::head:
		pimpl->system->GetDeviceToAbsoluteTrackingPose(vr::ETrackingUniverseOrigin::TrackingUniverseStanding, 0, &pose, 1);
		break;
	case trackingObjects::leftHand:
		if (pimpl->leftId != vr::k_unMaxTrackedDeviceCount) {
			pimpl->system->GetControllerStateWithPose(vr::ETrackingUniverseOrigin::TrackingUniverseStanding, pimpl->leftId, &state, sizeof(state), &pose);
		}
		break;
	case trackingObjects::rightHand:
		if (pimpl->rightId != vr::k_unMaxTrackedDeviceCount) {
			pimpl->system->GetControllerStateWithPose(vr::ETrackingUniverseOrigin::TrackingUniverseStanding, pimpl->rightId, &state, sizeof(state), &pose);
		}
		break;
	}
	return openVrToGlm(pose.mDeviceToAbsoluteTracking);
}
std::vector<trackingObjects> VRModel::getAllObjs()
{
	std::vector<trackingObjects> list;
	if (vr::VR_IsHmdPresent()) {
		list.push_back(trackingObjects::head);
		if (pimpl->leftId != vr::k_unMaxTrackedDeviceCount) list.push_back(trackingObjects::leftHand);
		if (pimpl->rightId != vr::k_unMaxTrackedDeviceCount) list.push_back(trackingObjects::rightHand);
	}
	return list;
}
