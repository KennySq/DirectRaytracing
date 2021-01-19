#pragma once


using namespace Microsoft;
using namespace DirectX;

struct Vertex
{
	XMFLOAT3 mPosition;
	XMFLOAT3 mNormal;
	XMFLOAT2 mUV;

	Vertex(float px, float py, float pz, float nx, float ny, float nz, float u, float v)
		: mPosition(px, py, pz), mNormal(nx, ny, nz), mUV(u, v)
	{}
	Vertex() {}
};

struct Triangle
{
	XMFLOAT4 mPosition[3];
	XMFLOAT4 mNormal[3];
	XMFLOAT2 mUV[3];
};

struct Mesh
{
	WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
	WRL::ComPtr<ID3D11Buffer> mIndexBuffer;
};


void generateSphere(float radius, unsigned int sliceCount, unsigned int stackCount, Mesh& meshData);