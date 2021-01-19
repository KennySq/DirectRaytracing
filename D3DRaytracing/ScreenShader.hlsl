SamplerState defaultSampler : register(s0);

cbuffer cameraBuffer : register(b0)
{
    matrix gView;
    matrix gProjection;
}

Texture2D rayBuffer : register(t0);

struct Vertex
{
    float4 mPosition : POSITION0;
    float2 mUV : TEXCOORD0;
};

struct Pixel
{
    float4 mSysPosition : SV_Position;
    float2 mUV : TEXCOORD0;
};

Pixel ScreenVS(Vertex input)
{
    Pixel output = (Pixel) 0;
    
    output.mSysPosition = input.mPosition;
    output.mUV = input.mUV;
    
    return output;
}

float4 ScreenPS(Pixel input) : SV_Target0
{
    return rayBuffer.Sample(defaultSampler, input.mUV);
}