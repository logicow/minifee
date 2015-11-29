#include "Game.h"

Game::Game(Graphics *aGraphics)
{
	graphics = aGraphics;
}

void Game::update()
{
	graphics->startUpdateSprites();
	static double f = 0;
	f += 200.0 * graphics->frameTime;
	if (f >= 320.0) {
		f = 0;
	}
	for (int i = 0; i < 40; i++) {
		graphics->spritePtr[i].pos = DirectX::XMFLOAT2((float)f + i*0.01f, i*0.01f);
		graphics->spritePtr[i].sprite_color = 1;
		graphics->spritePtr[i].sprite_flags = 0;
		graphics->spritePtr[i].sprite_index = 1;
	}
	graphics->spriteCount = 40;

	
}

int Game::run()
{
	graphics->setWindow(320, 200, "Minifee");
	
	graphics->startLoad();
	graphics->loadSprite("small", false);
	graphics->loadSprite("title", false);
	graphics->endLoad();

	graphics->startUpdatePalette();
	for (int i = 0; i < 256; i++) {
		graphics->palettePtr[i] = 0xFFFF0000 | ((i * 0xFF) & 0xFF);
	}

	graphics->startUpdateSpriteLookup();
	for (int i = 0; i < 256; i++) {
		graphics->spriteLookupPtr[i] = i;
	}
	for (int i = 0; i < 256; i++) {
		graphics->spriteLookupPtr[256 + i] = i == 0 ? 0 : 255 - i;
	}

	int frameC = 0;
	while (!graphics->shouldExit())
	{
		update();
		graphics->swap();
		graphics->frameTime;
	}
	return 0;
}