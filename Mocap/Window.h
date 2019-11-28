#pragma once
#include <functional>
#include <memory>
struct winImpl;
enum class windowVisibility {
	hide,
	showNormal,
	showMinimized,
	showMaximized,
	showNormalUnactivated,
	show,
	minimize,
	minimizeUnactivated,
	showUnactived,
	restore,
	showDefault,
	forceMinimize
};
enum class keyCode {
	lMouse = 1, rMouse, cancel, mMouse, mb1, mb2,
	back = 0x8, tab, clear = 0xC, enter,
	shift = 0x10, control, alt, pause, capsLock, 
	escape = 0x1B, space = 0x20, pageUp, pageDown, end, home, leftArrow, upArrow, rightArrow, downArrow, select, print, execute,
	printScreen, insert, del, help,
	k_0, k_1, k_2, k_3, k_4, k_5, k_6, k_7, k_8, k_9,
	a = 0x41, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z,
	leftWindows, rightWindows,
	n_0 = 0x60, n_1, n_2, n_3, n_4, n_5, n_6, n_7, n_8, n_9, n_multiply, n_add, n_sep, n_subtract, n_decimal, n_divide,
	f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12
};
enum class cursorType {
	normal, hand
};
struct EventListener
{
	std::function<bool(unsigned long long w, long long l)> callback;
	int eventId;
};
class Window
{
private:
	struct HWND__ * hWnd;
	struct HDC__ * hDc;
	struct HGLRC__ * hRc;
	int errorCode;
	bool quit;
	static Window * activeWindow;
	static long long __stdcall windowProc(struct HWND__ *, unsigned int, unsigned long long, long long);
	std::unique_ptr<winImpl> pimpl;
	static cursorType activeCursor;
public:
	Window(const char * title, int width, int height);
	int getLastError();
	~Window();
	bool show(windowVisibility show);
	void pollEvents(class std::function<void(struct tagMSG &)> f = nullptr);
	void switchBuffers();
	bool shouldQuit();
	bool isKeyPress(keyCode k);
	void attach(class Observer & ob);
	void addCommandListener(EventListener el);
	void addEventListener(EventListener el);
public:
	static void setCursor(cursorType c);
};

