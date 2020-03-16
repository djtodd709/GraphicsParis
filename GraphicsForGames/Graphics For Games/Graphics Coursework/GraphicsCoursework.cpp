#include "../../nclgl/window.h"
#include "Renderer.h"

#include <Windows.h>

#pragma comment(lib, "nclgl.lib")

int main() {
	Window w("Paris", 1920, 1200, true);
	if (!w.HasInitialised()) {
		return -1;
	}


	Renderer renderer(w);
	if (!renderer.HasInitialised()) {
		return -1;
	}

	PlaySound("LaVieEnRose.wav", NULL, SND_ASYNC);

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_P)) {
			renderer.RestartEiffel();
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_O)) {
			renderer.RestartLouvre(true);
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_M)) {
			renderer.SetCameraFree(false);
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_N)) {
			renderer.SetCameraFree(true);
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_1)) {
			renderer.GoToScene(1);
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_2)) {
			renderer.GoToScene(2);
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_3)) {
			renderer.GoToScene(3);
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_4)) {
			renderer.GoToScene(4);
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_5)) {
			renderer.GoToScene(5);
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_6)) {
			renderer.GoToScene(6);
		}
	}

	return 0;
}