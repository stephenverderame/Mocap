#include "Window.h"
#include <stdio.h>
#include "VRModel.h"
#include "RenderView.h"
#include "BufferViewModel.h"
#include <time.h>
#include "BVHLogger.h"
#include "RenderCapture.h"
int main() {
	Window display("Mocap", 800, 800);
	display.show(windowVisibility::show);
	RenderView view(800, 800);
	bool playbackMode = false;
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
	while (true) {
//		if (clock() - lastClock < CLOCKS_PER_SEC / 90.f) continue; //90 fps
		display.pollEvents();
		if (display.isKeyPress(keyCode::r) && display.isKeyPress(keyCode::control) && clock() - lastPress > CLOCKS_PER_SEC / 3) {
			printf("Changing modes\n");
			if (playbackMode)
				pvm.startRecording();
			else {
				pvm.finishRecording();
				pvm.serialize("mocapdata.db");
				BVHLogger l;
				l.resampleToFrameRate(24);
				pvm.writeLog("test.bvh", l);
				playback = clock();
				pvm.load("mocapdata.db");				
				cap.saveCapture("testCap.mpeg");
			}
			playbackMode = !playbackMode;
			lastPress = clock();
		}
		if (!playbackMode) {
			for (auto obj : trackedObjs) {
				glm::mat4 m = model.getDeviceTransform(obj);
				view.savePose(obj, m);
				pvm.savePose(obj, m);
			}
		}
		else {
			for (auto obj : trackedObjs) {
				glm::mat4 m = pvm.getDeviceTransform(obj, clock() - playback);
				view.savePose(obj, m);
			}
		}
		view.draw();
		if (!playbackMode) cap.capture();
		display.switchBuffers();
//		lastClock = clock();
	}
	getchar();
}