struct VSIn
{
	float2 pos : POSITION0;
};

struct VSOut
{
	float4 pos : SV_POSITION;
};

VSOut main(VSIn In)
{
	VSOut Out;
	Out.pos = float4(In.pos, 0.0f, 1.0f);
	return Out;
}
