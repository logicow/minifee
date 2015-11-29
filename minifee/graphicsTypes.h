#pragma once
#include <DirectXMath.h>

#define MAX_SPRITES 4096

struct SpriteConstant
{
	uint32_t tc_start[2];
	uint32_t tc_size[2];
	DirectX::XMFLOAT2 tc_offset;
	uint32_t padding[2];
};

struct SpriteVertex
{
	DirectX::XMFLOAT2 pos;
	uint32_t sprite_index;
	uint8_t sprite_color;
	uint8_t sprite_flags;
	uint16_t sprite_padding;
};