#include "Window.h"
#include <Windows.h>
#include <glad/glad.h>
#include <time.h>

Window::Window(const char * title, int width, int height) : hWnd(nullptr), hDc(nullptr), hRc(nullptr), errorCode(0) 
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = DefWindowProc;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = "defWindowClass";
	RECT size = { 0, 0, width, height };
	AdjustWindowRect(&size, WS_OVERLAPPED, FALSE);
	RegisterClassEx(&wc);
	hWnd = CreateWindowEx(NULL, wc.lpszClassName, title, WS_OVERLAPPEDWINDOW | wc.style, 0, 0, size.right - size.left, size.bottom - size.top, NULL, NULL, NULL, NULL);
	if (hWnd == nullptr) errorCode = 100;
	else {
		hDc = GetDC(hWnd);

		PIXELFORMATDESCRIPTOR px;
		int ret = 0;
		ZeroMemory(&px, sizeof(px));
		px.nSize = sizeof(px);
		px.nVersion = 1;
		px.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		px.iPixelType = PFD_TYPE_RGBA;
		px.cColorBits = 32;
		//	px.cStencilBits = 8;
		px.cDepthBits = 32;
		px.cAlphaBits = 8;
		px.iLayerType = PFD_MAIN_PLANE;
		ret = ChoosePixelFormat(hDc, &px);
		if (!ret) errorCode = 200;
		else {
			if (SetPixelFormat(hDc, ret, &px) == 0) errorCode = 250;
			else {
				DescribePixelFormat(hDc, ret, sizeof(px), &px);

				hRc = wglCreateContext(hDc);
				if (hRc == NULL) errorCode = 300;
				else {
					if (wglMakeCurrent(hDc, hRc) == FALSE) errorCode = 400;
					else {						
						if (!gladLoadGL()) errorCode = 500;
					}
				}
			}
		}
	}
}

int Window::getLastError()
{
	int e = errorCode;
	errorCode = 0;
	return e;
}


Window::~Window()
{
	if(hDc != nullptr) ReleaseDC(hWnd, hDc);
	if(hRc != nullptr) wglDeleteContext(hRc);
	if(hWnd != nullptr) DestroyWindow(hWnd);
}

bool Window::show(windowVisibility show)
{
	if (hWnd == nullptr) return false;
	ShowWindow(hWnd, (int)show);
	return true;
}

void Window::pollEvents(std::function<void(struct tagMSG &)> f)
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		if (f != nullptr) f(msg);
		if (msg.message == WM_QUIT) quit = true;
		DispatchMessage(&msg);
	}
}

void Window::switchBuffers()
{
	SwapBuffers(hDc);
}

bool Window::shouldQuit()
{
	return quit;
}

bool Window::isKeyPress(keyCode k)
{
	return GetAsyncKeyState((int)k) < 0;
}

