#pragma once

#include "Enemy.h"

class EnemyLargeSkellinton : public Enemy
{
public:

	int dir;
	float timer;
	bool walking;
	float walkTimer;
	int previousHp;
	int hitHp;
	float hitTimer;
	float dramaticDeathTimer;
	float deathSparkCooldown;

	EnemyLargeSkellinton(float x, float y);
	virtual void update(gameState& state);
};