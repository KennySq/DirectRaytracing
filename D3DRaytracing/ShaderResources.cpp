#include"pch.h"

#include"ShaderResources.h"
#include"Geometry.h"

void TriangleStream::generateBuffer(ID3D11Device* const device, const Mesh* mesh)
{
	D3D11_BUFFER_DESC bufferDesc{};
	D3D11_BUFFER_DESC vertexBufferDesc{};

	mesh->mVertexBuffer.Get()->GetDesc(&vertexBufferDesc);
	int triangleCount = (vertexBufferDesc.ByteWidth / sizeof(Vertex)) / 3;

	bufferDesc.ByteWidth = sizeof(Triangle) * triangleCount;
	bufferDesc.BindFlags = D3D11_BIND_STREAM_OUTPUT;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;

	auto result = device->CreateBuffer(&bufferDesc, nullptr, mBuffer.GetAddressOf());
	assert(result == S_OK);
}
