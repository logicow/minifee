#include "slide.h"
#include "gameState.h"

void startSlide(int slide, gameState& state)
{
	state.slide.sprite = slide;
	state.slide.timer = 0.0f;
	state.slide.fadingIn = true;
	state.slide.fadingOut = false;
	state.updatePtr = &updateSlide;
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
		
		float brightness = state.slide.timer * 2.0f;
		if (brightness > 1.0f) {
			brightness = 1.0f;
			state.slide.fadingIn = false;
		}
		
		if (brightness <= 0.2f) {
			state.graphics->palettePtr[0] = 0x0;
			state.graphics->palettePtr[1] = 0x0;
			state.graphics->palettePtr[2] = 0x0;
			state.graphics->palettePtr[3] = 0x0;
			state.graphics->palettePtr[4] = 0x0;
		}
		else if (brightness <= 0.4f) {
			state.graphics->palettePtr[0] = 0x0;
			state.graphics->palettePtr[1] = 0x0;
			state.graphics->palettePtr[2] = 0x0;
			state.graphics->palettePtr[3] = 0x0;
			state.graphics->palettePtr[4] = 0xFFFF22FF;
		}
		else if (brightness <= 0.6f) {
			state.graphics->palettePtr[0] = 0x0;
			state.graphics->palettePtr[1] = 0x0;
			state.graphics->palettePtr[2] = 0x0;
			state.graphics->palettePtr[3] = 0xFFFF22FF;
			state.graphics->palettePtr[4] = 0xFFFF22FF;
		}
		else if (brightness <= 0.8f) {
			state.graphics->palettePtr[0] = 0x0;
			state.graphics->palettePtr[1] = 0x0;
			state.graphics->palettePtr[2] = 0x0;
			state.graphics->palettePtr[3] = 0xFFFF22FF;
			state.graphics->palettePtr[4] = 0xFFFFFF22;
		}
		else {
			state.graphics->palettePtr[0] = 0x0;
			state.graphics->palettePtr[1] = 0x0;
			state.graphics->palettePtr[2] = 0xFFFF22FF;
			state.graphics->palettePtr[3] = 0xFFFFFF22;
			state.graphics->palettePtr[4] = 0xFFFFFFFF;
		}
	}

	if (state.slide.fadingOut) {
		state.graphics->startUpdatePalette();

		float brightness = 1.0f - state.slide.timer * 2.0f;
		if (brightness < 0.0f) {
			brightness = 0.0f;
		}

		if (brightness <= 0.2f) {
			state.graphics->palettePtr[0] = 0x0;
			state.graphics->palettePtr[1] = 0x0;
			state.graphics->palettePtr[2] = 0x0;
			state.graphics->palettePtr[3] = 0x0;
			state.graphics->palettePtr[4] = 0x0;
		}
		else if (brightness <= 0.4f) {
			state.graphics->palettePtr[0] = 0x0;
			state.graphics->palettePtr[1] = 0x0;
			state.graphics->palettePtr[2] = 0x0;
			state.graphics->palettePtr[3] = 0x0;
			state.graphics->palettePtr[4] = 0xFFFF22FF;
		}
		else if (brightness <= 0.6f) {
			state.graphics->palettePtr[0] = 0x0;
			state.graphics->palettePtr[1] = 0x0;
			state.graphics->palettePtr[2] = 0x0;
			state.graphics->palettePtr[3] = 0xFFFF22FF;
			state.graphics->palettePtr[4] = 0xFFFF22FF;
		}
		else if (brightness <= 0.8f) {
			state.graphics->palettePtr[0] = 0x0;
			state.graphics->palettePtr[1] = 0x0;
			state.graphics->palettePtr[2] = 0x0;
			state.graphics->palettePtr[3] = 0xFFFF22FF;
			state.graphics->palettePtr[4] = 0xFFFFFF22;
		}
		else {
			state.graphics->palettePtr[0] = 0x0;
			state.graphics->palettePtr[1] = 0x0;
			state.graphics->palettePtr[2] = 0xFFFF22FF;
			state.graphics->palettePtr[3] = 0xFFFFFF22;
			state.graphics->palettePtr[4] = 0xFFFFFFFF;
		}
	}

	if (state.gamepad->pressed[gamepadButton::START] || state.gamepad->pressed[gamepadButton::A] || state.gamepad->pressed[gamepadButton::Y] || state.gamepad->pressed[gamepadButton::B] || state.gamepad->pressed[gamepadButton::X]) {
		if (!state.slide.fadingOut && (state.slide.sprite < 2 || state.slide.timer > 2.0f)) {
			state.slide.fadingOut = true;
			state.slide.timer = 1.0f - state.slide.timer;
			if (state.slide.timer < 0.0f) {
				state.slide.timer = 0.0f;
			}
		}
	}

	if (state.slide.fadingOut && state.slide.timer > 1.0f)
	{
		if (state.slide.sprite == 0) {
			startSlide(1, state);
		}
		else if(state.slide.sprite == 1) {
			startAction(0, state);
		}
		else {
			state.updatePtr = nullptr;
		}
	}
}