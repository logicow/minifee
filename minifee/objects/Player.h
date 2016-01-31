#pragma once

struct gameState;

class Player
{
public:
	float x;
	float y;
	float w;
	float h;
	bool remove;
	int dir;
	float cooldown;
	bool dead;
	float deadTimer;
	float invinco;

	Player(float x, float y);
	void update(gameState& state);
};