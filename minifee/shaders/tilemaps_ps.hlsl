
Texture2D <uint> tiles_texture : register(t0);
Texture2D <uint> tilemap_texture : register(t1);

struct VSOut
{
	float4 pos : SV_POSITION;
	float2 tc : TEXCOORD0;
};

uint main(VSOut vs) : SV_TARGET
{
	uint tile_width = 16;
	uint tile_height = 16;

	uint tile = tilemap_texture.Load(int3(vs.tc, 0));

	int2 tiles_tc = int2((frac(vs.tc.x) + (tile % 256)) * tile_width, (frac(vs.tc.y) + (tile / 256)) * tile_height);

	uint tile_color = tiles_texture.Load(int3(tiles_tc, 0));

	if (tile_color == 0) {
		discard;
	}
	return tile_color;
}
