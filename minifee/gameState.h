#pragma once
#include "engine\Graphics.h"
#include "engine\winSynth.h"
#include "engine\Gamepad.h"
#include "slide.h"
#include "action.h"
#include "objects\EnemyBullet.h"

struct gameState
{
	slideState slide;
	actionState action;

	void(*updatePtr)(gameState&);

	Graphics *graphics;
	Gamepad *gamepad;
	winSynth *synth;


	int idx_player;
	int idx_playerBulletFocus;
	int idx_spark;
	int idx_enemySkellinton;
	int idx_enemyLargeSkellinton;
	int idx_pentagram;
	int idx_largePentagram;
	int idx_drum;
	int idx_guitar;
	int idx_microphone;
	int idx_bubble;
	int idx_enemyDemon;
	int idx_enemyLargeDemon;
	int idx_enemyBulletShot;
	int idx_enemyBulletLargeShot;

};