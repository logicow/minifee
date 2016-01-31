Texture2D <uint> sprites_texture : register(t0);
Texture2D <uint> lookup_texture : register(t1);

struct VSOut
{
	float4 pos : SV_POSITION;
	float2 tc : TEXCOORD0;
	uint color_row : TEXCOORD1;
};

uint main(VSOut vs) : SV_TARGET
{
	uint sprite_color = sprites_texture.Load(int3(vs.tc.x, vs.tc.y, 0));
	uint result = lookup_texture.Load(int3(sprite_color, vs.color_row, 0));
	if (result == 0) {
		discard;
	}
	return result;
}