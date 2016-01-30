#include "slide.h"

void startSlide(int slide, gameState& state)
{
	state.slide.sprite = slide;
	state.updatePtr = &updateSlide;
	state.slide.timer = 0.0f;
	state.slide.fadingIn = true;
	state.slide.fadingOut = false;

	state.slide.palette;
}

void updateSlide(gameState& state)
{
	//int& t = state.graphics->tilemapDrawCount;
	//state.graphics->tilemapDrawPtr[t].pos = DirectX::XMFLOAT2((float)0, (float)0);
	//state.graphics->tilemapDrawPtr[t].tilemap_index = state.slide;
	//t += 1;

	int& s = state.graphics->spriteCount;
	state.graphics->spritePtr[s].pos = DirectX::XMFLOAT2((float)0, (float)0);
	state.graphics->spritePtr[s].sprite_color = 0;
	state.graphics->spritePtr[s].sprite_flags = 0;
	state.graphics->spritePtr[s].sprite_index = state.slide.sprite;
	s += 1;

	state.slide.timer += state.graphics->frameTime;

	if (state.slide.fadingIn) {
		state.graphics->startUpdatePalette();
		for (int i = 0; i < 256; i++) {
			int a = (state.graphics->palettes[0][i] >> 24) & 0xFF;
			int r = (state.graphics->palettes[0][i] >> 16) & 0xFF;
			int g = (state.graphics->palettes[0][i] >> 8 ) & 0xFF;
			int b = (state.graphics->palettes[0][i] >> 0 ) & 0xFF;

			float brightness = state.slide.timer;
			if (brightness > 1.0f) {
				brightness = 1.0f;
				state.slide.fadingIn = false;
			}

			r *= brightness;
			g *= brightness;
			b *= brightness;

			state.graphics->palettePtr[i] = (a << 24) + (r << 16) + (g << 8) + b;
		}
	}
}