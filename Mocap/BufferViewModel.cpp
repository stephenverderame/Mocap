#include "BufferViewModel.h"
#include "pose.h"
#include <vector>
#include <unordered_map>
#include <fstream>
#include "Logger.h"

struct bvmImpl {
	clock_t startTime, endTime, duration;
	std::vector<std::vector<pose>> objPoses;
	std::unordered_map<trackingObjects, size_t> objIds;
	std::unordered_map<trackingObjects, size_t> lastObjPos;
};

BufferViewModel::BufferViewModel()
{
	pimpl = std::make_unique<bvmImpl>();
}


BufferViewModel::~BufferViewModel()
{
}

void BufferViewModel::startRecording()
{
	pimpl->startTime = clock();
}

void BufferViewModel::finishRecording()
{
	pimpl->endTime = clock();
}

void BufferViewModel::addTrackingObj(trackingObjects obj)
{
	pimpl->objIds.emplace(obj, pimpl->objPoses.size());
	pimpl->objPoses.emplace_back();
}

void BufferViewModel::savePose(trackingObjects obj, glm::mat4 & pose, long frame)
{
	frame = frame == -1 ? clock() - pimpl->startTime : frame;
	struct pose p = { frame, obj, pose };
	pimpl->objPoses[pimpl->objIds[obj]].push_back(p);
	pimpl->duration = frame;
}

void BufferViewModel::savePose(trackingObjects obj, glm::mat4 && pose, long frame)
{
	frame = frame == -1 ? clock() - pimpl->startTime : frame;
	struct pose p = { frame, obj, pose };
	pimpl->objPoses[pimpl->objIds[obj]].push_back(p);
	pimpl->duration = frame;
}

glm::mat4 BufferViewModel::getDeviceTransform(trackingObjects d)
{
	return getDeviceTransform(d, clock() - pimpl->endTime);
}

glm::mat4 BufferViewModel::getDeviceTransform(trackingObjects d, long f)
{
	clock_t now = f % pimpl->duration;
	size_t objId = pimpl->objIds[d];
	if (pimpl->lastObjPos[d] >= pimpl->objPoses[objId].size() - 1) pimpl->lastObjPos[d] = 0;
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
	return glm::mat4();
}

std::vector<trackingObjects> BufferViewModel::getAllObjs()
{
	std::vector<trackingObjects> list;
	for (auto it : pimpl->objIds)
		list.push_back(it.first);
	return list;
}

void BufferViewModel::serialize(const char * file)
{
	std::ofstream out(file, std::ios::binary);
	if (out.is_open()) {
		/* File format
		size_t trackingObjNum
			int trackingObj
			size_t trackingObjNumPoses
				pose p
		clock_t startTime, endTime, duration
		*/
		size_t objNum = pimpl->objIds.size();
		out.write((char*)&objNum, sizeof(objNum));
		for (auto it : pimpl->objIds) {
			trackingObjects obj = it.first;
			out.write((char*)&obj, sizeof(obj));
			size_t poseNum = pimpl->objPoses[pimpl->objIds[obj]].size();
			out.write((char*)&poseNum, sizeof(poseNum));
//			out.write((char*)&pimpl->objPoses[pimpl->objIds[obj]], poseNum * sizeof(pose));
			for (size_t i = 0; i < poseNum; ++i) {
				struct pose p = pimpl->objPoses[pimpl->objIds[obj]][i];
				out.write((char*)&(p.time), sizeof(p.time));
				out.write((char*)&(p.obj), sizeof(p.obj));
				out.write((char*)&(p.transform[0][0]), sizeof(p.transform));
			}
		}
		out.write((char*)&pimpl->startTime, sizeof(clock_t));
		out.write((char*)&pimpl->endTime, sizeof(clock_t));
		out.write((char*)&pimpl->duration, sizeof(clock_t));
	}
}

void BufferViewModel::load(const char * file)
{
	std::ifstream in(file, std::ios::binary);
	if (in.is_open()) {
		/* File format
		size_t trackingObjNum
			int trackingObj
			size_t trackingObjNumPoses
				pose p
		clock_t startTime, endTime, duration
		*/
		size_t objNum;
		in.read((char*)&objNum, sizeof(objNum));
		pimpl->objPoses.resize(objNum);
		for (size_t i = 0; i < objNum; ++i) {
			trackingObjects obj;
			in.read((char*)&obj, sizeof(obj));
			size_t poseNum;
			in.read((char*)&poseNum, sizeof(poseNum));
			pimpl->objIds[obj] = i;
			pimpl->objPoses[i].resize(poseNum);
			for (size_t j = 0; j < poseNum; ++j) {
				struct pose & p = pimpl->objPoses[i][j];
				in.read((char*)&(p.time), sizeof(p.time));
				in.read((char*)&(p.obj), sizeof(p.obj));
				in.read((char*)&(p.transform[0][0]), sizeof(p.transform));
			}
//			in.read((char*)pimpl->objPoses[i].data(), poseNum * sizeof(pose));
			pimpl->lastObjPos[obj] = 0;
		}
		clock_t temp;
		in.read((char*)&temp, sizeof(clock_t));
		pimpl->startTime = temp;
		in.read((char*)&temp, sizeof(clock_t));
		pimpl->endTime = temp;
		in.read((char*)&temp, sizeof(clock_t));
		pimpl->duration = temp;

	}
}

void BufferViewModel::writeLog(const char * file, Logger & log)
{
	std::vector<trackingObjects> objs;
	objs.resize(pimpl->objIds.size());
	for (auto it : pimpl->objIds)
		objs[it.second] = it.first;
	log.out(file, &pimpl->objPoses[0][0], &objs[0], objs.size(), pimpl->objPoses[0].size(), pimpl->duration);
}
