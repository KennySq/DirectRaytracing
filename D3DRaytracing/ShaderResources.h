#pragma once

using namespace Microsoft;


struct Mesh;
struct TriangleStream
{
	WRL::ComPtr<ID3D11Buffer> mBuffer;

	void generateBuffer(ID3D11Device* const device, const Mesh* mesh);

};