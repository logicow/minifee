#include "EnemyBulletPentagram.h"
#include "..\gameState.h"
#include "Player.h"
#include "EnemySkellinton.h"
#include "EnemyDevil.h"

EnemyBulletPentagram::EnemyBulletPentagram(float x, float y, int type, bool onExit)
{
	w = 16;
	h = 16;
	this->x = x - w / 2;
	this->y = y - h / 2;
	remove = false;
	timer = 0;
	this->type = type;
	open = false;
	spawned = 0;
	blinkTimer = 0;
	this->onExit = onExit;
	onExitTriggered = false;
}

float dist(float x1, float y1, float x2, float y2)
{
	return sqrt((x1-x2) * (x1-x2) + (y1-y2)*(y1-y2));
}

float dist2(float x1, float y1, float x2, float y2)
{
	return sqrt((x1 - x2) * (x1 - x2) * 0.33f + (y1 - y2)*(y1 - y2));
}

void EnemyBulletPentagram::update(gameState& state)
{
	timer += state.graphics->frameTime;

	if (timer >= 0.3f && !open) {
		float d = state.action.player != nullptr ? dist2(state.action.player->x + state.action.player->w*0.5, state.action.player->y + state.action.player->h*0.5, x + w*0.5f, y + h*0.5f) : 1000.0f;
		if (onExit ? d > 40.0f && onExitTriggered : d < 60.0f) {
			open = true;
			state.synth->playString("2cg+e+c2c#g#+f+c#2da+f#+d2d#a#+g+d#", synthInstrument::SQUARE_WAVE);
			timer = 0.0f;
		}
		else {
			if (d < 36.0f) {
				onExitTriggered = true;
			}
			timer = 0.0f;
		}
	}
	
	int frame = timer * 5.0f;
	if (frame > 2) {
		frame = 2;
	}

	blinkTimer -= state.graphics->frameTime;
	if (timer > 2.0f) {
		if (type == 0) {
			if (spawned < 14) {
				timer -= 0.12f;
				spawned += 1;
				blinkTimer = 0.1f;
				state.synth->playString("zcc#dd#e", synthInstrument::SQUARE_WAVE);
				float rx = ((float)rand() / RAND_MAX - 0.5f)*8.0f;
				float ry = ((float)rand() / RAND_MAX - 0.5f)*8.0f;
				state.action.enemies.push_back(new EnemySkellinton(x + w / 2 + rx, y + h / 2 + ry));
			}
			else {
				frame = 2 - (timer - 2.0f) * 6.0f;
				if (frame < 0) {
					open = false;
					remove = true;
				}
			}
		}
		else if(type == 1) {
			if (spawned < 3) {
				timer -= 1.0f;
				spawned += 1;
				blinkTimer = 0.2f;
				state.synth->playString("cc#dd#e", synthInstrument::SQUARE_WAVE);
				float rx = ((float)rand() / RAND_MAX - 0.5f)*8.0f;
				float ry = ((float)rand() / RAND_MAX - 0.5f)*8.0f;
				state.action.enemies.push_back(new EnemyDevil(x + w / 2 + rx, y + h / 2 + ry));
			}
			else {
				frame = 2 - (timer - 2.0f) * 6.0f;
				if (frame < 0) {
					open = false;
					remove = true;
				}
			}
		}
		else  {
			if (spawned < 20) {
				timer -= 0.08f;
				spawned += 1;
				blinkTimer = 0.1f;
				state.synth->playString("zcc#dd#e", synthInstrument::SQUARE_WAVE);
				float rx = ((float)rand() / RAND_MAX - 0.5f)*8.0f;
				float ry = ((float)rand() / RAND_MAX - 0.5f)*8.0f;
				state.action.enemies.push_back(new EnemySkellinton(x + w / 2 + rx, y + h / 2 + ry));
			}
			else {
				frame = 2 - (timer - 2.0f) * 6.0f;
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
		state.graphics->spritePtr[s].sprite_index = state.idx_pentagram + frame;
		s += 1;
	}

}