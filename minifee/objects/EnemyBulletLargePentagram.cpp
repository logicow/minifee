#include "EnemyBulletLargePentagram.h"
#include "..\gameState.h"
#include "Player.h"
#include "EnemyLargeSkellinton.h"
#include "EnemyLargeDevil.h"

EnemyBulletLargePentagram::EnemyBulletLargePentagram(float x, float y, int type)
{
	w = 32;
	h = 32;
	this->x = x - w / 2;
	this->y = y - h / 2;
	remove = false;
	timer = 0;
	this->type = type;
	open = false;
	spawned = 0;
	blinkTimer = 0;
}

extern float dist2(float x1, float y1, float x2, float y2);

void EnemyBulletLargePentagram::update(gameState& state)
{
	timer += state.graphics->frameTime;

	if (timer >= 1.0f && !open) {
		if (state.action.player != nullptr && 
			dist2(state.action.player->x + state.action.player->w*0.5, state.action.player->y + state.action.player->h*0.5, x + w*0.5f, y + h*0.5f) < 70.0f &&
			(type == 0 || state.action.bossKills >= 1)) {
			open = true;
			state.synth->playString("2cg+e+c2c#g#+f+c#2da+f#+d2d#a#+g+d#", synthInstrument::SQUARE_WAVE);
			timer = 0.0f;
		}
		else {
			timer = 0.0f;
		}
	}

	int frame = timer * 2.0f;
	if (frame > 2) {
		frame = 2;
	}

	blinkTimer -= state.graphics->frameTime;
	if (timer > 4.0f) {
		if (type == 0) {
			if (spawned < 1) {
				timer -= 0.2f;
				spawned += 1;
				blinkTimer = 0.1f;
				state.synth->playString("zcc#dd#e", synthInstrument::SQUARE_WAVE);
				float rx = ((float)rand() / RAND_MAX - 0.5f)*0.0f;
				float ry = ((float)rand() / RAND_MAX - 0.5f)*0.0f;
				state.action.enemies.push_back(new EnemyLargeSkellinton(x + w / 2 + rx, y + h / 2 + ry));
			}
			else {
				frame = 2 - (timer - 4.0f) * 2.0f;
				if (frame < 0) {
					open = false;
					remove = true;
				}
			}
		}
		else {
			if (spawned < 1) {
				timer -= 1.0f;
				spawned += 1;
				blinkTimer = 0.2f;
				state.synth->playString("cc#dd#e", synthInstrument::SQUARE_WAVE);
				float rx = ((float)rand() / RAND_MAX - 0.5f)*8.0f;
				float ry = ((float)rand() / RAND_MAX - 0.5f)*8.0f;
				state.action.enemies.push_back(new EnemyLargeDevil(x + w / 2 + rx, y + h / 2 + ry));
			}
			else {
				frame = 2 - (timer - 4.0f) * 2.0f;
				if (frame < 0) {
					open = false;
					remove = true;
				}
			}
		}


	}

	if (blinkTimer > 0.0f) {
		frame = 3;
	}

	if (open) {
		int& s = state.graphics->spriteCount;
		state.graphics->spritePtr[s].pos = DirectX::XMFLOAT2(x - state.action.cameraX, y - state.action.cameraY);
		state.graphics->spritePtr[s].sprite_color = 0;
		state.graphics->spritePtr[s].sprite_flags = 0;
		state.graphics->spritePtr[s].sprite_index = state.idx_largePentagram + frame;
		s += 1;
	}

}