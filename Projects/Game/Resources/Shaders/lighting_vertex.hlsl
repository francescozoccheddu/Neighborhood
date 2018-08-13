struct VSIn
{
    float2 TexCoord : TEXCOORD;
};

struct VSOut
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

VSOut main(in VSIn _sIn)
{
    VSOut sOut;
    sOut.Position = float4(_sIn.TexCoord.x * 2.0 - 1.0, _sIn.TexCoord.y * -2.0 + 1.0, 0.0, 1.0);
    sOut.TexCoord = _sIn.TexCoord;
    return sOut;
}