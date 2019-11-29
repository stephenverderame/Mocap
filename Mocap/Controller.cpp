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
bool openSaveFile(char * path, const char * filter = nullptr);
bool openOpenFile(char * path, const char * filter = nullptr);
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
	clock_t startPlayback = 0;
	clock_t playback = 0;
	RenderCapture cap(800, 800);
	Gui gui(800, 800);
	Control btn({ "recordOff.png", "record.png" }, 600, 50, 64, 64);
	Control scBtn({ "screenCapOff.png", "screenCapOn.png" }, 600, 120, 64, 64);
	Control slider({ "slider.png" }, 100, 700, 25, 50);
	slider.setEnabled(false);
	Control slideBar({ "sliderBar.png" }, 100, 700, 600, 50);
	slideBar.setShow(false);
	slider.setShow(false);
	bool slideBarHold = false;
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
	btn.mouseIn = [&display]() { display.setCursor(cursorType::hand); };
	btn.mouseOut = [&display]() {display.setCursor(cursorType::normal); };
	scBtn.mouseIn = [&display]() { display.setCursor(cursorType::hand); };
	scBtn.mouseOut = [&display]() {display.setCursor(cursorType::normal); };
	scBtn.mouseClick = [&scBtn, &screenCap]() {
		screenCap = !screenCap;
		scBtn.setActiveTexture(screenCap);
	};
	slideBar.mouseClick = [&pvm, &slider, &slideBarHold, &playback, &startPlayback]() {
		int x, y;
		Window::getMousePos(x, y);
		slider.setPos(x);
		slideBarHold = true;
		playback = (x - 100) / 600.0 * pvm.getDuration();
		startPlayback = clock();
	};
	slideBar.mouseRelease = [&slideBarHold, &startPlayback]() {startPlayback = clock();  slideBarHold = false; };
	slideBar.mouseOver = [&pvm, &slider, &slideBarHold, &playback, &startPlayback]() {
		if (slideBarHold) {
			int x, y;
			Window::getMousePos(x, y);
			slider.setPos(x);
			playback = (x - 100) / 600.0 * pvm.getDuration();
			startPlayback = clock();
		}
	};
	slideBar.mouseOut = [&slideBarHold, &display]() {slideBarHold = false; display.setCursor(cursorType::normal); };
	slideBar.mouseIn = [&display]() {display.setCursor(cursorType::hand); };
	gui.add(btn);
	gui.add(scBtn);
	gui.add(slider);
	gui.add(slideBar);
	display.attach(gui);
	display.addCommandListener({[&recorded, &pvm](unsigned long long w, long long l) -> bool {
		if (recorded) {
			char path[MAX_PATH];
			ZeroMemory(path, MAX_PATH);
			BVHLogger log;
			log.resampleToFrameRate(24);
			if(openSaveFile(path, "Motion Capture Files\0*.bvh\0")) pvm.writeLog(path, log);
		}
		return true;
	}, IDB_EXPORT_BVH });
	display.addCommandListener({ [&recorded, &pvm](unsigned long long w, long long l) -> bool {
		if (recorded) {
			char path[MAX_PATH];
			ZeroMemory(path, MAX_PATH);
			if(openSaveFile(path, "Mocap Data Files\0*.db;\0")) pvm.serialize(path);
		}
		return true;
	}, IDB_SAVE_DATA });
	display.addCommandListener({ [&recorded, &cap](unsigned long long w, long long l) -> bool {
		if (recorded) {
			char path[MAX_PATH];
			ZeroMemory(path, MAX_PATH);
			if(openSaveFile(path, "Video Files\0*.mpeg\0")) cap.saveCapture(path);
		}
		return true;
	}, IDB_SAVE_CAP });
	display.addCommandListener({ [&recorded, &pvm, &playbackMode](unsigned long long w, long long l) -> bool {
		char path[MAX_PATH];
		ZeroMemory(path, MAX_PATH);
		if (openOpenFile(path, "Mocap Data Files\0*.db;\0")) {
			pvm.clearBuffer();
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
				glm::mat4 m;
				if(!slideBarHold)
					m = pvm.getDeviceTransform(obj, (clock() - startPlayback) + playback);
				else
					m = pvm.getDeviceTransform(obj, playback);
				view.savePose(obj, m);
			}
			slider.setShow(true);
			slideBar.setShow(true);
			if(!slideBarHold)
			slider.setPos((((clock() - startPlayback) + playback) % pvm.getDuration()) / (double)pvm.getDuration() * 600 + 100);
		}
		view.draw();
		if (screenCap) cap.capture();
		gui.display();
		display.switchBuffers();
//		lastClock = clock();
	}
	getchar();
}

bool openSaveFile(char * path, const char * filter)
{
	OPENFILENAME op;
	ZeroMemory(&op, sizeof(op));
	op.lStructSize = sizeof(op);
	op.lpstrTitle = "Save";
	op.nMaxFile = MAX_PATH;
	op.Flags = 0;// OFN_FILEMUSTEXIST;
	if (filter != nullptr) {
		op.lpstrFilter = filter;
	}
	op.lpstrFile = path;
	return GetSaveFileName(&op);
}

bool openOpenFile(char * path, const char * filter)
{
	OPENFILENAME op;
	ZeroMemory(&op, sizeof(op));
	op.lStructSize = sizeof(op);
	op.lpstrTitle = "Open";
	op.nMaxFile = MAX_PATH;
	op.Flags = OFN_FILEMUSTEXIST;
	if (filter != nullptr) {
		op.lpstrFilter = filter;
	}
	op.lpstrFile = path;
	return GetOpenFileName(&op);
}
