#pragma once
#include <string>
#include "graphicsTypes.h"

class Graphics
{
public:
	SpriteVertex *spritePtr;
	uint32_t *palettePtr;
	int spriteCount;
	uint8_t *spriteLookupPtr;
	double frameTime;

	virtual void setWindow(int width, int height, std::string name) = 0;
	virtual bool shouldExit() = 0;
	virtual void swap() = 0;
	virtual void startLoad() = 0;
	virtual void endLoad() = 0;

	virtual int loadSprite(std::string name, bool centered) = 0;

	virtual void startUpdateSprites() = 0;
	virtual void startUpdateSpriteLookup() = 0;
	virtual void startUpdatePalette() = 0;
};