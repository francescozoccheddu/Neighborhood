struct PSIn
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

float4 main(in PSIn _sIn) : SV_TARGET
{
    return float4(0.0f, 1.0f, 1.0f, 1.0f);
}