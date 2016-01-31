#pragma once

#include "EnemyBullet.h"

class EnemyBulletPentagram : public EnemyBullet
{
public:
	float x;
	float y;
	float w;
	float h;

	float timer;
	float blinkTimer;
	int type;
	bool open;
	int spawned;
	bool onExit;
	bool onExitTriggered;

	EnemyBulletPentagram(float x, float y, int type, bool onExit);
	virtual void update(gameState& state);
};