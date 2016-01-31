struct VSIn
{
	float2 pos_in_sprite_unit : POSITION0;
	uint vertex_id : SV_VertexID;
};

struct VSOut
{
	float4 pos : SV_POSITION;
	float2 tc : TEXCOORD0;
};

struct AtlasInfo
{
	uint2 src_pos;
	uint2 size;
};

struct DrawInfo
{
	float2 pos;
	uint index;
};

cbuffer TilemapAtlas
{
	AtlasInfo atlas[1024];
};

cbuffer TilemapInfo
{
	DrawInfo tilemaps[1024];
};

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}

VSOut main(VSIn In)
{
	VSOut Out;
	uint tile_width = 16;
	uint tile_height = 16;

	uint tilemap_id = In.vertex_id >> 2;
	uint atlas_id = tilemaps[tilemap_id].index;

	float2 pos_in_tilemap = atlas[atlas_id].size * In.pos_in_sprite_unit;

	Out.pos = float4(tilemaps[tilemap_id].pos + pos_in_tilemap * float2(tile_width, tile_height), 0.0f, 1.0f);

	Out.tc = atlas[atlas_id].src_pos + pos_in_tilemap + 0.0001f;

	Out.pos = float4(
		(pos_in_tilemap.x * tile_width + tilemaps[tilemap_id].pos.x) * (2.0f / 320.0f) - 1.0f,
		(pos_in_tilemap.y * tile_height + tilemaps[tilemap_id].pos.y) * (-2.0f / 200.0f) + 1.0f, 0.0f, 1.0f);

	return Out;
}
