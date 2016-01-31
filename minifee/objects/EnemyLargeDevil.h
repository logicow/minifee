#pragma once

#include "Enemy.h"

class EnemyLargeDevil : public Enemy
{
public:

	int dir;
	float timer;
	bool attacking;
	float attackTimer;
	float cooldown;
	int previousHp;
	float hitTimer;

	int hitHp;
	float dramaticDeathTimer;
	float deathSparkCooldown;

	bool goLeft;
	bool goRight;
	bool goUp;
	bool goDown;

	EnemyLargeDevil(float x, float y);
	virtual void update(gameState& state);
};