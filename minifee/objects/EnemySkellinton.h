#pragma once

#include "Enemy.h"

class EnemySkellinton : public Enemy
{
public:

	int dir;
	float timer;
	bool walking;
	float walkTimer;

	EnemySkellinton(float x, float y);
	virtual void update(gameState& state);
};