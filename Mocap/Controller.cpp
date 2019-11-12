#include "Window.h"
#include <stdio.h>
#include "VRModel.h"
#include "RenderView.h"
#include "PlaybackViewModel.h"
#include <time.h>
int main() {
	Window display("Mocap", 800, 800);
	display.show(windowVisibility::show);
	RenderView view(800, 800);
	bool playbackMode = false;
	VRModel model;
	PlaybackViewModel pvm;
	auto trackedObjs = model.getAllObjs();
	for (auto obj : trackedObjs) {
		view.addTrackingObj(obj);
		pvm.addTrackingObj(obj);
	}
	pvm.startRecording();
	clock_t lastPress = 0;
	while (!display.shouldQuit()) {
//		if (clock() - lastClock < CLOCKS_PER_SEC / 90.f) continue; //90 fps
		display.pollEvents();
		if (display.isKeyPress(keyCode::r) && display.isKeyPress(keyCode::control) && clock() - lastPress > CLOCKS_PER_SEC / 3) {
			printf("Changing modes\n");
			if (playbackMode)
				pvm.startRecording();
			else
				pvm.finishRecording();
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
				glm::mat4 m = pvm.getDeviceTransform(obj);
				view.savePose(obj, m);
			}
		}
		view.draw();
		display.switchBuffers();
//		lastClock = clock();
	}
}