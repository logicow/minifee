#include "EnemyBand.h"
#include "..\gameState.h"
#include "PlayerBulletSpark.h"

EnemyBand::EnemyBand(float x, float y, int type)
{
	w = 16;
	h = 16;
	this->x = x - w / 2;
	this->y = y - h / 2;
	remove = false;
	hp = -42;
	timer = 0;
	this->type = type;
}

extern bool move(float& x, float& y, float w, float h, float dx, float dy, gameState& state);

void EnemyBand::update(gameState& state)
{
	if (state.action.bossKills >= 2 && hp == -42) {
		hp = 1;
	}

	if (hp <= 0 && state.action.bossKills >= 2) {
		state.synth->playString("1a+a+a", synthInstrument::SQUARE_WAVE);
		state.action.playerBullets.push_back(new PlayerBulletSpark(x + h / 2, y + h / 2));
		remove = true;
		state.action.bandLeft -= 1;
		return;
	}

	timer += state.graphics->frameTime;



	int& s = state.graphics->spriteCount;
	state.graphics->spritePtr[s].pos = DirectX::XMFLOAT2(x - state.action.cameraX, y - state.action.cameraY);
	state.graphics->spritePtr[s].sprite_color = 0;
	state.graphics->spritePtr[s].sprite_flags = 0;
	
	if (type == 0) {
		state.graphics->spritePtr[s].sprite_index = state.idx_guitar;
	}
	if (type == 1) {
		state.graphics->spritePtr[s].sprite_index = state.idx_guitar + 1 + ((int)(timer * 10.0f)%2);
	}
	if (type == 2) {
		state.graphics->spritePtr[s].sprite_index = state.idx_drum;
	}
	if (type == 3) {
		state.graphics->spritePtr[s].sprite_index = state.idx_drum + 1 + ((int)(timer * 30.0f) % 2);
	}
	if (type == 4) {
		state.graphics->spritePtr[s].sprite_index = state.idx_microphone;
	}
	if (type == 5) {
		state.graphics->spritePtr[s].sprite_index = state.idx_microphone + 1 + ((int)(timer * 5.0f) % 2);
	}
	
	s += 1;
}