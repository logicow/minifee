struct VSIn
{
	float2 pos_in_sprite_unit : POSITION0;
	uint vertex_id : SV_VertexID;
};

struct VSOut
{
	float4 pos : SV_POSITION;
	float2 tc : TEXCOORD0;
	uint color_row : TEXCOORD1;
};

struct AtlasInfo
{
	uint2 src_pos;
	uint2 size;
	float2 offset;
	uint2 padding;
};

struct SpriteInfo
{
	float2 pos;
	uint index;
	uint color_and_flags;
};

cbuffer SpriteAtlas
{
	AtlasInfo atlas[1024];
};

cbuffer SpriteInfo
{
	SpriteInfo sprites[1024];
};

VSOut main(VSIn In)
{
	VSOut Out;
	uint sprite_id = In.vertex_id >> 2;
	uint atlas_id = sprites[sprite_id].index;

	float2 pos_in_sprite = atlas[atlas_id].size * In.pos_in_sprite_unit;

	Out.pos = float4(sprites[sprite_id].pos + atlas[atlas_id].offset + pos_in_sprite, 0.0f, 1.0f);

	Out.tc = atlas[atlas_id].src_pos + pos_in_sprite;
	Out.color_row = sprites[sprite_id].color_and_flags & 0xFF;
	
	Out.pos = float4(
		(pos_in_sprite.x + sprites[sprite_id].pos.x + atlas[atlas_id].offset.x) * ( 2.0f / 320.0f) - 1.0f, 
		(pos_in_sprite.y + sprites[sprite_id].pos.y + atlas[atlas_id].offset.y) * (-2.0f / 200.0f) + 1.0f, 0.0f, 1.0f);

	return Out;
}
