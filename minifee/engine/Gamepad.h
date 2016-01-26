#pragma once

enum gamepadButton {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	A,
	B,
	X,
	Y,
	START,
	SELECT,
	L,
	R,
	COUNT
};

class Gamepad
{
public:
	bool pressed[gamepadButton::COUNT];
	virtual void update() = 0;
};