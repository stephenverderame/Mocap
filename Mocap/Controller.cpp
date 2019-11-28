#include "Window.h"
#include <stdio.h>
#include "VRModel.h"
#include "RenderView.h"
#include "BufferViewModel.h"
#include <time.h>
#include "BVHLogger.h"
#include "RenderCapture.h"
#include "Gui.h"
#include "Control.h"
#include "res.h"
#include <Windows.h>
bool openSaveFile(char * path);
bool openOpenFile(char * path);
int main() {
	Window display("Mocap", 800, 800);
	display.show(windowVisibility::show);
	RenderView view(800, 800);
	bool playbackMode = true, recorded = false;
	bool screenCap = false;
	VRModel model;
	BufferViewModel pvm;
	auto trackedObjs = model.getAllObjs();
	for (auto obj : trackedObjs) {
		view.addTrackingObj(obj);
		pvm.addTrackingObj(obj);
	}
	pvm.startRecording();
	clock_t lastPress = 0;
	clock_t playback = 0;
	RenderCapture cap(800, 800);
	Gui gui(800, 800);
	Control btn({ "recordOff.png", "record.png" }, 600, 50, 64, 64);
	Control scBtn({ "screenCapOff.png", "screenCapOn.png" }, 600, 120, 64, 64);
	btn.mouseClick = [&btn, &playbackMode, &pvm, &recorded, &model]() {
		if (playbackMode) {
			btn.setActiveTexture(1);
			pvm.clearBuffer();
			auto trackedObjs = model.getAllObjs();
			for (auto obj : trackedObjs) {
				pvm.addTrackingObj(obj);
			}
			pvm.startRecording();
		}
		else {
			btn.setActiveTexture(0);
			pvm.finishRecording();
			recorded = true;
		}
		playbackMode = !playbackMode;
	};
	btn.mouseOver = [&display]() { display.setCursor(cursorType::hand); };
	btn.mouseOut = [&display]() {display.setCursor(cursorType::normal); };
	scBtn.mouseOver = [&display]() { display.setCursor(cursorType::hand); };
	scBtn.mouseOut = [&display]() {display.setCursor(cursorType::normal); };
	scBtn.mouseClick = [&scBtn, &screenCap]() {
		screenCap = !screenCap;
		scBtn.setActiveTexture(screenCap);
	};
	gui.add(btn);
	gui.add(scBtn);
	display.attach(gui);
	display.addCommandListener({[&recorded, &pvm](unsigned long long w, long long l) -> bool {
		if (recorded) {
			char path[MAX_PATH];
			ZeroMemory(path, MAX_PATH);
			BVHLogger log;
			log.resampleToFrameRate(24);
			if(openSaveFile(path)) pvm.writeLog(path, log);
		}
		return true;
	}, IDB_EXPORT_BVH });
	display.addCommandListener({ [&recorded, &pvm](unsigned long long w, long long l) -> bool {
		if (recorded) {
			char path[MAX_PATH];
			ZeroMemory(path, MAX_PATH);
			if(openSaveFile(path)) pvm.serialize(path);
		}
		return true;
	}, IDB_SAVE_DATA });
	display.addCommandListener({ [&recorded, &cap](unsigned long long w, long long l) -> bool {
		if (recorded) {
			char path[MAX_PATH];
			ZeroMemory(path, MAX_PATH);
			if(openSaveFile(path)) cap.saveCapture(path);
		}
		return true;
	}, IDB_SAVE_CAP });
	display.addCommandListener({ [&recorded, &pvm, &playbackMode](unsigned long long w, long long l) -> bool {
		char path[MAX_PATH];
		ZeroMemory(path, MAX_PATH);
		if (openOpenFile(path)) {
			pvm.load(path);
			playbackMode = true;
			recorded = true;
		}
		return true;
	}, IDB_LOAD_DATA });
	while (true) {
//		if (clock() - lastClock < CLOCKS_PER_SEC / 90.f) continue; //90 fps
		display.pollEvents();
		if (!recorded || !playbackMode) {
			for (auto obj : trackedObjs) {
				glm::mat4 m = model.getDeviceTransform(obj);
				view.savePose(obj, m);
				pvm.savePose(obj, m);
			}
		}
		else if(recorded) {
			for (auto obj : trackedObjs) {
				glm::mat4 m = pvm.getDeviceTransform(obj, clock() - playback);
				view.savePose(obj, m);
			}
		}
		view.draw();
		if (screenCap) cap.capture();
		gui.display();
		display.switchBuffers();
//		lastClock = clock();
	}
	getchar();
}

bool openSaveFile(char * path)
{
	OPENFILENAME op;
	ZeroMemory(&op, sizeof(op));
	op.lStructSize = sizeof(op);
	op.lpstrTitle = "Save";
	op.nMaxFile = MAX_PATH;
	op.Flags = 0;// OFN_FILEMUSTEXIST;
	op.lpstrFile = path;
	return GetSaveFileName(&op);
}

bool openOpenFile(char * path)
{
	OPENFILENAME op;
	ZeroMemory(&op, sizeof(op));
	op.lStructSize = sizeof(op);
	op.lpstrTitle = "Open";
	op.nMaxFile = MAX_PATH;
	op.Flags = OFN_FILEMUSTEXIST;
	op.lpstrFile = path;
	return GetOpenFileName(&op);
}
