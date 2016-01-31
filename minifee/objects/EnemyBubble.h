#pragma once

#include "Enemy.h"

class EnemyBubble : public Enemy
{
public:
	float timer;

	EnemyBubble(float x, float y);
	virtual void update(gameState& state);
};