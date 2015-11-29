Texture2D <uint> index_texture : register(t0);
Texture1D <float4> palette_texture : register(t1);

struct VSOut
{
	float4 pos : SV_POSITION;
};

float4 main(VSOut vs) : SV_TARGET
{
	uint index = index_texture.Load(int3(vs.pos.rg, 0));
	return palette_texture.Load(int2(index, 0));
	//return float4((index % 8) / 8.0f, ((index / 8)%8) / 8.0f, ((index / 64)%4)/4.0f, 1);
}