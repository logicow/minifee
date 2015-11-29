#include "winGamepad.h"
#include "windows.h"

winGamepad::winGamepad()
{
	for (int i = 0; i < gamepadButton::COUNT; i++) {
		pressed[i] = false;
	}
}

void winGamepad::update()
{
	pressed[gamepadButton::UP] = (GetAsyncKeyState(VK_UP) & ~1) != 0;
	pressed[gamepadButton::DOWN] = (GetAsyncKeyState(VK_DOWN) & ~1) != 0;
	pressed[gamepadButton::LEFT] = (GetAsyncKeyState(VK_LEFT) & ~1) != 0;
	pressed[gamepadButton::RIGHT] = (GetAsyncKeyState(VK_RIGHT) & ~1) != 0;
	pressed[gamepadButton::A] = (GetAsyncKeyState(0x43) & ~1) != 0;
	pressed[gamepadButton::B] = (GetAsyncKeyState(0x5A) & ~1) != 0;
	pressed[gamepadButton::X] = (GetAsyncKeyState(VK_SHIFT) & ~1) != 0;
	pressed[gamepadButton::Y] = (GetAsyncKeyState(0x58) & ~1) != 0;
	pressed[gamepadButton::START] = (GetAsyncKeyState(VK_RETURN) & ~1) != 0;
	pressed[gamepadButton::SELECT] = (GetAsyncKeyState(VK_TAB) & ~1) != 0;
}