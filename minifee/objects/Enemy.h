#pragma once

struct gameState;

class Enemy
{
public:
	float x;
	float y;
	float w;
	float h;

	int hp;

	bool remove;

	virtual void update(gameState& state) = 0;
};