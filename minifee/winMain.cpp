#include "windows.h"

#include "Game.h"
#include "winGraphics.h"
#include "winGamepad.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	winGraphics graphics(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	winGamepad gamepad;
	Game game((Graphics*)&graphics, (Gamepad*)&gamepad);

	game.run();

	return EXIT_SUCCESS;
}