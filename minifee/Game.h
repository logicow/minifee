#pragma once

#include "Graphics.h"
#include "Gamepad.h"
#include "winSynth.h"

class Game
{
public:
	Game(Graphics *aGraphics, Gamepad *aGamepad, winSynth *aSynth);

	int run();

private:
	Graphics *graphics;
	Gamepad *gamepad;
	winSynth *synth;

	void update();
};