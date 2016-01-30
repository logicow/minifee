#pragma once
#include "engine\Graphics.h"
#include "engine\winSynth.h"
#include "engine\Gamepad.h"

struct slideState
{
	int sprite;
	float timer;
	bool fadingIn;
	bool fadingOut;
	uint32_t palette[256];
};

struct gameState
{
	slideState slide;

	void(*updatePtr)(gameState&);

	Graphics *graphics;
	Gamepad *gamepad;
	winSynth *synth;
};