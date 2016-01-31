#pragma once

struct gameState;

struct slideState
{
	int sprite;
	float timer;
	bool fadingIn;
	bool fadingOut;
};

void startSlide(int slide, gameState& state);

void updateSlide(gameState& state);