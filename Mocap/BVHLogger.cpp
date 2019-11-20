#include "BVHLogger.h"
#include <fstream>
#include "BufferViewModel.h"
#include "pose.h"
#include <gtx\matrix_decompose.hpp>
#include <iomanip>

struct bvhPose {
	glm::vec3 position, rotation;
};

std::ostream& operator<<(std::ostream & stream, const bvhPose & pose)
{
	stream << pose.position.x << " " << pose.position.y << " " << pose.position.z << " "
		<< glm::degrees(pose.rotation.z) << " " << glm::degrees(pose.rotation.x) << " " << glm::degrees(pose.rotation.y);
	return stream;
}

std::ostream& operator<<(std::ostream & stream, const bvhPose && pose)
{
	stream << pose.position.x << " " << pose.position.y << " " << pose.position.z << " "
		<< glm::degrees(pose.rotation.z) << " " << glm::degrees(pose.rotation.x) << " " << glm::degrees(pose.rotation.y);
	return stream;
}

std::ostream& operator<<(std::ostream & stream, const glm::vec3 & vec)
{
	stream << vec.x << " " << vec.y << " " << vec.z;
	return stream;
}

bvhPose toBvh(glm::mat4 & mat) {
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 pos;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(mat, scale, rotation, pos, skew, perspective);
	rotation = glm::conjugate(rotation);
	bvhPose p = { pos, glm::vec3(rotation.x, rotation.y, rotation.z) };
	return p;
}

BVHLogger::BVHLogger()
{
}


BVHLogger::~BVHLogger()
{
}

void BVHLogger::out(const char * filename, pose * data, trackingObjects * objs, int numObjects, int numFrames, clock_t duration)
{
	std::ofstream f(filename);
	if (!f.is_open()) return;
	f << "HIERARCHY\nROOT Hips\n{\n";
	f << "\tOFFSET 0.00 0.00 0.00\n";
	f << "\tCHANNELS 3 Xposition Yposition Zposition\n";
	int lHand = -1, rHand = -1, head = -1;
	glm::vec3 hips = toBvh(data[head * numFrames].transform).position;
	hips.y = 0;
	for (int i = 0; i < numObjects; ++i) {
		if (objs[i] == trackingObjects::leftHand) lHand = i;
		else if (objs[i] == trackingObjects::rightHand) rHand = i;
		else if (objs[i] == trackingObjects::head) head = i;
	}
	if (head != -1) {
		glm::vec3 p = toBvh(data[head * numFrames].transform).position;
		f << "\tJOINT Head\n\t{\n";
		f << "\t\tOFFSET " << "0.0 " << std::setprecision(6) << p.y << " 0.0\n";
		f << "\t\tCHANNELS 6 Xposition Yposition Zposition Zrotation Xrotation Yrotation\n";
		f << "\t\tEnd Site\n\t\t{\n";
		f << "\t\t\tOFFSET 0.0 0.0 0.0\n\t\t}\n";
		f << "\t}\n";
	}
	if (lHand != -1) {
		f << "\tJOINT LeftHand\n\t{\n";
		glm::vec3 offset = toBvh(data[lHand * numFrames].transform).position - hips;
		f << "\t\tOFFSET " << offset.x << " " << offset.y << " " << offset.z << "\n";
		f << "\t\tCHANNELS 6 Xposition Yposition Zposition Zrotation Xrotation Yrotation\n";
		f << "\t\tEnd Site\n\t\t{\n";
		f << "\t\t\tOFFSET 0.0 0.0 0.0\n\t\t}\n";
		f << "\t}\n";
	}
	if (rHand != -1) {
		f << "\tJOINT RightHand\n\t{\n";
		glm::vec3 offset = toBvh(data[rHand * numFrames].transform).position - hips;
		f << "\t\tOFFSET " << offset.x << " " << offset.y << " " << offset.z << "\n";
		f << "\t\tCHANNELS 6 Xposition Yposition Zposition Zrotation Xrotation Yrotation\n";
		f << "\t\tEnd Site\n\t\t{\n";
		f << "\t\t\tOFFSET 0.0 0.0 0.0\n\t\t}\n";
		f << "\t}\n";
	}
	f << "}\n";
	f << "MOTION\nFrames: " << numFrames << "\n";
	f << "Frame Time: " << (double)duration / CLOCKS_PER_SEC / (double)numFrames << "\n";
	for (int i = 0; i < numFrames; ++i) {
		f << hips;
		if (head != -1) f << " " << toBvh(data[head * numFrames + i].transform);
		if (lHand != -1) f << " " << toBvh(data[lHand * numFrames + i].transform);
		if (rHand != -1) f << " " << toBvh(data[lHand * numFrames + i].transform);
		f << "\n";
	}
}
