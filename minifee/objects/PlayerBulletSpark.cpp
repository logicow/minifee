#include "PlayerBulletSpark.h"
#include "..\gameState.h"

PlayerBulletSpark::PlayerBulletSpark(float x, float y)
{
	w = 16;
	h = 16;
	this->x = x - w / 2;
	this->y = y - h / 2;
	remove = false;
	timer = 0;
}

void PlayerBulletSpark::update(gameState& state)
{
	timer += state.graphics->frameTime;

	int frame = timer * 20.0f;

	if (frame >= 4) {
		remove = true;
	}
	else {
		int& s = state.graphics->spriteCount;
		state.graphics->spritePtr[s].pos = DirectX::XMFLOAT2(x - state.action.cameraX, y - state.action.cameraY);
		state.graphics->spritePtr[s].sprite_color = 0;
		state.graphics->spritePtr[s].sprite_flags = 0;
		state.graphics->spritePtr[s].sprite_index = state.idx_spark + frame;
		s += 1;
	}
	
}