#include "EnemyBubble.h"
#include "..\gameState.h"

EnemyBubble::EnemyBubble(float x, float y)
{
	w = 32;
	h = 32;
	this->x = x - w / 2;
	this->y = y - h / 2;
	remove = false;
	hp = -1;
	timer = 0;
}

extern bool move(float& x, float& y, float w, float h, float dx, float dy, gameState& state);

void EnemyBubble::update(gameState& state)
{
	if (state.action.bossKills >= 2) {
		remove = true;
		return;
	}

	timer += state.graphics->frameTime;
	if (timer < 10.0f) {
		return;
	}

	int& s = state.graphics->spriteCount;
	state.graphics->spritePtr[s].pos = DirectX::XMFLOAT2(x - state.action.cameraX, y - state.action.cameraY);
	state.graphics->spritePtr[s].sprite_color = 0;
	state.graphics->spritePtr[s].sprite_flags = 0;
	state.graphics->spritePtr[s].sprite_index = state.idx_bubble;
	s += 1;
}