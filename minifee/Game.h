#pragma once

#include "BaseMath.h"
#include "Graphics.h"
#include "Gamepad.h"

class Game
{
public:
	Game(Graphics *aGraphics, Gamepad *aGamepad);

	int run();

private:
	Graphics *graphics;
	Gamepad *gamepad;

	void update();
};