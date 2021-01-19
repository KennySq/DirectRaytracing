#pragma once
using namespace Microsoft;
using namespace DirectX;

struct HitRecord
{
	XMFLOAT3 mPoint;
	XMFLOAT3 mNormal;
	float mT;
};

class RayObject
{
	WRL::ComPtr<ID3D11Buffer> mObjectBuffer;
public:
	inline bool GenerateBuffer(ID3D11Device* device, ID3D11DeviceContext* context = nullptr)
	{
		D3D11_BUFFER_DESC bufferDesc{};
		
		bufferDesc.ByteWidth = sizeof(HitRecord);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		
		auto result = device->CreateBuffer(&bufferDesc, nullptr, mObjectBuffer.GetAddressOf());
		assert(result == S_OK);

		if (context != nullptr)
		{
			HitRecord data{};
			D3D11_SUBRESOURCE_DATA subData{};

			subData.pSysMem = &data;
			context->UpdateSubresource(mObjectBuffer.Get(), 0, nullptr, &subData, 0, 0);
		}

		return true;
	}

};

