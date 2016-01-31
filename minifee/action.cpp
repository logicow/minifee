#include "action.h"
#include "slide.h"
#include "gameState.h"
#include "objects\Player.h"
#include "objects\Enemy.h"
#include "objects\EnemyBullet.h"
#include "objects\PlayerBullet.h"
#include "objects\EnemyBulletPentagram.h"
#include "objects\EnemyBubble.h"
#include "objects\EnemyBand.h"
#include "objects\EnemyBulletLargePentagram.h"
//#include "objects\EnemyLargeSkellinton.h"
//#include "objects\EnemyLargeDevil.h"

void startAction(int level, gameState& state)
{
	state.action.level = level;
	state.action.introTimer = 0.0f;
	state.action.fadeTimer = 0.0f;
	state.action.winTimer = 0.0f;
	state.action.fadingIn = true;
	state.action.fadingOut = false;
	state.updatePtr = &updateAction;
	state.action.bandLeft = 6;

	state.action.player = nullptr;// new Player(32.0f, 32.0f);
	
	state.action.cameraX = 40;
	state.action.cameraY = 4;

	state.action.enemyBullets.push_back(new EnemyBulletPentagram(17 * 16, 51 * 16, 0, false));
	state.action.enemyBullets.push_back(new EnemyBulletPentagram(17 * 16, 43 * 16, 1, false));
	state.action.enemyBullets.push_back(new EnemyBulletPentagram(18 * 16, 37 * 16, 0, true));
	state.action.enemyBullets.push_back(new EnemyBulletPentagram(26 * 16, 32 * 16, 0, false));
	state.action.enemyBullets.push_back(new EnemyBulletPentagram(19 * 16, 22 * 16, 1, true));
	state.action.enemyBullets.push_back(new EnemyBulletPentagram(15 * 16, 21 * 16, 1, true));
	state.action.enemyBullets.push_back(new EnemyBulletPentagram(12.51 * 16, 11 * 16, 2, false));
	state.action.enemyBullets.push_back(new EnemyBulletPentagram(7  * 16, 17 * 16, 0, false));

	//state.action.enemies.push_back(new EnemyLargeDevil(17 * 16, 51 * 16));
	
	state.action.enemyBullets.push_back(new EnemyBulletLargePentagram(12.5 * 16, 4 * 16, 0));
	state.action.enemyBullets.push_back(new EnemyBulletLargePentagram(12.5 * 16, 3 * 16, 1));

	state.action.enemies.push_back(new EnemyBubble(14.5 * 16, 5 * 16));
	state.action.enemies.push_back(new EnemyBubble(12.5 * 16, 7 * 16));
	state.action.enemies.push_back(new EnemyBubble(9.5 * 16, 6 * 16));

	state.action.enemies.push_back(new EnemyBand(9 * 16, 6 * 16, 0));
	state.action.enemies.push_back(new EnemyBand(10 * 16, 6 * 16, 1));
	state.action.enemies.push_back(new EnemyBand(12 * 16, 7 * 16, 4));
	state.action.enemies.push_back(new EnemyBand(13 * 16, 7 * 16, 5));
	state.action.enemies.push_back(new EnemyBand(14 * 16, 5 * 16, 2));
	state.action.enemies.push_back(new EnemyBand(15 * 16, 5 * 16, 3));

	state.action.bossKills = 0;
}

void updateAction(gameState& state)
{
	//update player
	state.action.introTimer += state.graphics->frameTime;

	if (state.action.bandLeft <= 0) {
		state.action.winTimer += state.graphics->frameTime;
		if (state.action.winTimer >= 3.0f && state.action.fadingOut == false) {
			state.action.fadingOut = true;
			state.action.fadeTimer = 0.0f;
			state.synth->playXM(-1);
		}
	}

	static float respawnTimer = 0.0f;
	respawnTimer -= state.graphics->frameTime;
	if (state.action.player) {
		respawnTimer = 3.0f;
		state.action.player->update(state);
		if (state.action.player->remove) {
			delete(state.action.player);
			state.action.player = nullptr;
		}
	}
	else {
		if (state.action.introTimer >= 5.0f && respawnTimer <= 0.0f) {
			state.action.player = new Player(400.0f, 1000.0f);
		}
	}

	for (int i = 0; i < (int)state.action.playerBullets.size(); i++) {
		state.action.playerBullets[i]->update(state);
		if (state.action.playerBullets[i]->remove) {
			delete(state.action.playerBullets[i]);
			state.action.playerBullets[i] = state.action.playerBullets[state.action.playerBullets.size() - 1];
			state.action.playerBullets.pop_back();
			i -= 1;
		}
	}

	//to draw pentagrams behind lols
	for (int i = 0; i < (int)state.action.enemyBullets.size(); i++) {
		state.action.enemyBullets[i]->update(state);
		if (state.action.enemyBullets[i]->remove) {
			delete(state.action.enemyBullets[i]);
			state.action.enemyBullets[i] = state.action.enemyBullets[state.action.enemyBullets.size() - 1];
			state.action.enemyBullets.pop_back();
			i -= 1;
		}
	}

	for (int i = 0; i < (int)state.action.enemies.size(); i++) {
		state.action.enemies[i]->update(state);
		if (state.action.enemies[i]->remove) {
			delete(state.action.enemies[i]);
			state.action.enemies[i] = state.action.enemies[state.action.enemies.size() - 1];
			state.action.enemies.pop_back();
			i -= 1;
		}
	}

	int& t = state.graphics->tilemapDrawCount;
	state.graphics->tilemapDrawPtr[t].pos = DirectX::XMFLOAT2(-state.action.cameraX, -state.action.cameraY);
	state.graphics->tilemapDrawPtr[t].tilemap_index = state.action.level;
	t += 1;
	
	state.action.fadeTimer += state.graphics->frameTime;

	if (state.action.fadingIn) {
		state.graphics->startUpdatePalette();

		float brightness = state.action.fadeTimer;
		if (brightness > 1.0f) {
			brightness = 1.0f;
			state.action.fadingIn = false;
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

	if (state.action.fadingOut) {
		state.graphics->startUpdatePalette();

		float brightness = 1.0f - state.action.fadeTimer;
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

	if (state.action.fadingOut && state.action.fadeTimer > 1.0f)
	{
		if (state.action.level == 0) {
			startSlide(2, state);
		}
	}
}