#include "windows.h"

#include "Game.h"
#include "winGraphics.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	winGraphics graphics(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	Game game((Graphics*)&graphics);

	game.run();

	return EXIT_SUCCESS;
}