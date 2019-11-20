#include "PlaybackViewModel.h"
#include <time.h>
#include <vector>
#include <unordered_map>

struct pose {
	clock_t time;
	trackingObjects obj;
	glm::mat4 transform;

};

struct pvmImpl {
	clock_t startTime, duration;
	std::vector<std::vector<pose>> objPoses;
	std::unordered_map<trackingObjects, size_t> objIds;
	std::unordered_map<trackingObjects, size_t> lastObjPos;
};

PlaybackViewModel::PlaybackViewModel() : playbackMode(false)
{
	pimpl = std::make_unique<pvmImpl>();
}


PlaybackViewModel::~PlaybackViewModel()
{
}

void PlaybackViewModel::startRecording()
{
	pimpl->startTime = clock();
	playbackMode = false;
}

void PlaybackViewModel::finishRecording()
{
	playbackMode = true;
	pimpl->startTime = clock();
}

void PlaybackViewModel::addTrackingObj(trackingObjects obj)
{
	if (!playbackMode) {
		pimpl->objIds.emplace(obj, pimpl->objPoses.size());
		pimpl->objPoses.emplace_back();
	}
}

void PlaybackViewModel::savePose(trackingObjects obj, glm::mat4 & pose)
{
	if (!playbackMode) {
		struct pose p = { clock() - pimpl->startTime, obj, pose };
		pimpl->objPoses[pimpl->objIds[obj]].push_back(p);
		pimpl->duration = clock() - pimpl->startTime;
	}
}

void PlaybackViewModel::savePose(trackingObjects obj, glm::mat4 && pose)
{
	if (!playbackMode) {
		struct pose p = { clock() - pimpl->startTime, obj, pose };
		pimpl->objPoses[pimpl->objIds[obj]].push_back(p);
		pimpl->duration = clock() - pimpl->startTime;
	}
}

glm::mat4 PlaybackViewModel::getDeviceTransform(trackingObjects d)
{
	if (playbackMode) {
		clock_t now = (clock() - pimpl->startTime) % pimpl->duration;
		size_t objId = pimpl->objIds[d];
		if (pimpl->lastObjPos[d] >= pimpl->objPoses[objId].size()) pimpl->lastObjPos[d] = 0;
		glm::mat4 * transform = nullptr;
		for (size_t i = pimpl->lastObjPos[d]; i < pimpl->objPoses[objId].size(); ++i) {
			struct pose & p = pimpl->objPoses[objId][i];
			if (now < p.time) {
				pimpl->lastObjPos[d] = i;
				break;
			}
			transform = &p.transform;
		}
		if (transform != nullptr) return *transform;
	}
	return glm::mat4();
}

std::vector<trackingObjects> PlaybackViewModel::getAllObjs()
{
	std::vector<trackingObjects> list;
	for (auto it : pimpl->objIds)
		list.push_back(it.first);
	return list;
}