#pragma once

#include "BaseMath.h"
#include "Graphics.h"

class Game
{
public:
	Game(Graphics *aGraphics);

	int run();

private:
	Graphics *graphics;

	void update();
};