#include "BVHLogger.h"
#include <fstream>
#include "BufferViewModel.h"
#include "pose.h"
#include <gtx\matrix_decompose.hpp>
#include <gtx\euler_angles.hpp>
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

bvhPose toBvh(const glm::mat4 & mat) {
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 pos;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(mat, scale, rotation, pos, skew, perspective);
//	rotation = glm::conjugate(rotation);
	glm::vec3 rot;
	glm::extractEulerAngleXYZ(mat, rot.x, rot.y, rot.z);
	bvhPose p = { pos, rot };
/*	glm::mat4 test;
	test = glm::rotate(test, rot.x, glm::vec3(1, 0, 0));
	test = glm::rotate(test, rot.y, glm::vec3(0, 1, 0));
	test = glm::rotate(test, rot.z, glm::vec3(0, 0, 1));
	test = glm::translate(test, pos);
	mat = test;*/
	return p;
}
bvhPose toRelative(bvhPose && pose, const glm::vec3 & parent)
{
	pose.position -= parent;
	return pose;
}

BVHLogger::BVHLogger() : fps(-1)
{
}


BVHLogger::~BVHLogger()
{
}

void BVHLogger::resampleToFrameRate(int fps)
{
	this->fps = fps;
}

void BVHLogger::out(const char * filename, const std::vector<std::vector<pose>> & data, trackingObjects * objs, int numObjects, int numFrames, long duration)
{
	std::ofstream f(filename);
	if (!f.is_open()) return;
	f << "HIERARCHY\n";// ROOT Hips\n{ \n";
//	f << "\tOFFSET 0.00 0.00 0.00\n";
//	f << "\tCHANNELS 3 Xposition Yposition Zposition\n";
	int lHand = -1, rHand = -1, head = -1;
	for (int i = 0; i < numObjects; ++i) {
		if (objs[i] == trackingObjects::leftHand) lHand = i;
		else if (objs[i] == trackingObjects::rightHand) rHand = i;
		else if (objs[i] == trackingObjects::head) head = i;
	}
	glm::vec3 hips = toBvh(data[head][0].transform).position;
	hips.y = 0;
	if (head != -1) {
		glm::vec3 p = toBvh(data[head][0].transform).position;
		f << "ROOT Head\n{\n";
		f << "\tOFFSET " << p << "\n";
		f << "\tCHANNELS 6 Xposition Yposition Zposition Zrotation Xrotation Yrotation\n";
		f << "\tEnd Site\n\t{\n";
		f << "\t\tOFFSET 0.0 0.0 0.0\n\t}\n";
		f << "}\n";
	}
	if (lHand != -1) {
		f << "ROOT LeftHand\n{\n";
		glm::vec3 offset = toBvh(data[lHand][0].transform).position;
		f << "\tOFFSET " << offset << "\n";
		f << "\tCHANNELS 6 Xposition Yposition Zposition Zrotation Xrotation Yrotation\n";
		f << "\tEnd Site\n\t{\n";
		f << "\t\tOFFSET 0.0 0.0 0.0\n\t}\n";
		f << "}\n";
	}
	if (rHand != -1) {
		f << "ROOT RightHand\n\t{\n";
		glm::vec3 offset = toBvh(data[rHand][0].transform).position;
		f << "\tOFFSET " << offset << "\n";
		f << "\tCHANNELS 6 Xposition Yposition Zposition Zrotation Xrotation Yrotation\n";
		f << "\tEnd Site\n\t\t{\n";
		f << "\t\tOFFSET 0.0 0.0 0.0\n\t}\n";
		f << "}\n";
	}
//	f << "}\n";
	int skip = 1;
	int originalFrames = numFrames;
	if (fps != -1) {
		numFrames = (double)numFrames / ((double)numFrames / (duration / CLOCKS_PER_SEC)) * fps;
		skip = round((double)originalFrames / numFrames);
	}
	f << "MOTION\nFrames: " << numFrames << "\n";
	f << "Frame Time: " << (double)duration / CLOCKS_PER_SEC / (double)numFrames << "\n";
	for (int i = 0; i < originalFrames; i += skip) {
//		f << " 0.0 0.0 0.0";
		if (head != -1) f << " " << toBvh(data[head][i].transform);
		if (lHand != -1) f << " " << toBvh(data[lHand][i].transform);
		if (rHand != -1) f << " " << toBvh(data[rHand][i].transform);
		f << "\n";
	}
}
