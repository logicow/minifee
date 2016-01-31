#pragma once

#include "Enemy.h"

class EnemyDevil : public Enemy
{
public:

	int dir;
	float timer;
	bool attacking;
	float attackTimer;
	float cooldown;
	int previousHp;
	float hitTimer;

	bool goLeft;
	bool goRight;
	bool goUp;
	bool goDown;

	EnemyDevil(float x, float y);
	virtual void update(gameState& state);
};