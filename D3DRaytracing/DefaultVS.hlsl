cbuffer InstanceBuffer : register(b1)
{
    matrix gTransform;
}

cbuffer CameraBuffer : register(b0)
{
    matrix gView;
    matrix gProjection;
}

struct VertexInput
{
    float3 mPosition : POSITION0;
    float3 mNormal : NORMAL0;
    float2 mUV : TEXCOORD0;
};

struct GeometryInput
{
    float4 mPosition : POSITION0;
    float4 mNormal : NORMAL0;
    float2 mUV : TEXCOORD0;
};

struct PixelInput
{
    float4 mPosition : SV_Position;
    float4 mNormal : TEXCOORD0;
    float2 mUV : TEXCOORD1;
};

GeometryInput DefaultVS(VertexInput input)
{
    GeometryInput output = (GeometryInput)0;
    
    output.mPosition = float4(input.mPosition, 1.0f);
    output.mNormal = float4(input.mNormal, 0.0f);
    output.mUV = input.mUV;
    
	return output;
}

[maxvertexcount(12)]
void DefaultGS(triangle GeometryInput input[3], inout TriangleStream<GeometryInput> triStream)
{
    GeometryInput streamOutput[3];
    
    float4 worldPosition[3];
    float4 viewPosition[3];
    float4 projectPosition[3];
    
    float4 worldNormal[3];
    float2 uv[3];
    
    worldPosition[0] = mul(input[0].mPosition, gTransform);
    worldPosition[1] = mul(input[1].mPosition, gTransform);
    worldPosition[2] = mul(input[2].mPosition, gTransform);
    
    viewPosition[0] = mul(worldPosition[0], gView);
    viewPosition[1] = mul(worldPosition[1], gView);
    viewPosition[2] = mul(worldPosition[2], gView);
    
    projectPosition[0] = mul(viewPosition[0], gProjection);
    projectPosition[1] = mul(viewPosition[1], gProjection);
    projectPosition[2] = mul(viewPosition[2], gProjection);
    
    worldNormal[0] = mul(input[0].mNormal, gTransform);
    worldNormal[1] = mul(input[1].mNormal, gTransform);
    worldNormal[2] = mul(input[2].mNormal, gTransform);
    
    uv[0] = input[0].mUV;
    uv[1] = input[1].mUV;
    uv[2] = input[2].mUV;
    
    streamOutput[0].mPosition = projectPosition[0];
    streamOutput[1].mPosition = projectPosition[1];
    streamOutput[2].mPosition = projectPosition[2];
    
    streamOutput[0].mNormal = worldNormal[0];
    streamOutput[1].mNormal = worldNormal[1];
    streamOutput[2].mNormal = worldNormal[2];
    
    streamOutput[0].mUV = uv[0];
    streamOutput[1].mUV = uv[1];
    streamOutput[2].mUV = uv[2];
    
    triStream.Append(streamOutput[0]);
    triStream.Append(streamOutput[1]);
    triStream.Append(streamOutput[2]);
    
    triStream.RestartStrip();
    
    return;
}

