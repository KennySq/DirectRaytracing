#include"pch.h"
#include "Engine.h"

using namespace std;

void Engine::generateQuad()
{
	vector<Vertex> vertices;
	vector<UINT> indicies;

	Vertex v[] = {
		Vertex(-1.0f, 1.0f, 0.0f, 0.0f, 0.0f),
		Vertex(1.0f, 1.0f, 0.0f, 1.0f, 0.0f),
		Vertex(-1.0f, -1.0f, 0.0f, 0.0f, 1.0f),
		Vertex(1.0f, -1.0f, 0.0f, 1.0f, 1.0f)
	};

	UINT i[] = { 0,1,2,2,3,1 };

	D3D11_BUFFER_DESC vertexDesc{};
	D3D11_BUFFER_DESC indexDesc{};

	D3D11_SUBRESOURCE_DATA subData{};

	vertexDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(v);
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	subData.pSysMem = v;

	auto Result = mDevice->CreateBuffer(&vertexDesc, &subData, quadVertexBuffer.GetAddressOf());
	assert(Result == S_OK);

	indexDesc.ByteWidth = sizeof(UINT) * ARRAYSIZE(i);
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	subData.pSysMem = i;

	Result = mDevice->CreateBuffer(&indexDesc, &subData, quadIndexBuffer.GetAddressOf());
	assert(Result == S_OK);

}

void Engine::generateBackbuffer()
{
	D3D11_TEXTURE2D_DESC texDesc{};
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	texDesc.Height = mHeight;
	texDesc.Width = mWidth;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	auto Result = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
	assert(Result == S_OK);

	Result = mDevice->CreateRenderTargetView(backBuffer.Get(), &rtvDesc, backRTV.GetAddressOf());
	assert(Result == S_OK);

	mContext->ClearRenderTargetView(backRTV.Get(), Colors::Red);

}

void Engine::generateCamera()
{
	mView = XMMatrixLookAtLH(
		XMVectorSet(5.0f, 5.0f, 0.0f, 1.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	);

	mProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), 1.333f, 0.01f, 1000.0f);

}

void Engine::generateRenderStates()
{
	D3D11_RASTERIZER_DESC rasterDesc{};

	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.DepthClipEnable = true;

	auto Result = mDevice->CreateRasterizerState(&rasterDesc, defaultRS.GetAddressOf());
	assert(Result == S_OK);

	defaultViewport.TopLeftX = 0;
	defaultViewport.TopLeftY = 0;
	defaultViewport.Width = mWidth;
	defaultViewport.Height = mHeight;
	defaultViewport.MaxDepth = 1.0f;
	defaultViewport.MinDepth = 0.0f;

}

void Engine::compileShaders()
{
	wstring screenShaderPath = L"ScreenShader.hlsl";
	wstring computeShaderPath = L"Raytracing.hlsl";

	DWORD compileFlag = 0;
	compileFlag |= D3DCOMPILE_DEBUG;
	ID3DBlob* vBlob, * pBlob, * cBlob, * errBlob;

	D3D11_INPUT_ELEMENT_DESC ilElements[] =
	{
		//LPCSTR SemanticName;
		//UINT SemanticIndex;
		//DXGI_FORMAT Format;
		//UINT InputSlot;
		//UINT AlignedByteOffset;
		//D3D11_INPUT_CLASSIFICATION InputSlotClass;
		//UINT InstanceDataStepRate;

		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};



	auto result = D3DCompileFromFile(screenShaderPath.c_str(), nullptr, nullptr, "ScreenVS", "vs_5_0", 0, compileFlag, &vBlob, &errBlob);
	assert(result == S_OK);

	result = mDevice->CreateInputLayout(ilElements, 2, vBlob->GetBufferPointer(), vBlob->GetBufferSize(), screenIL.GetAddressOf());
	assert(result == S_OK);

	result = mDevice->CreateVertexShader(vBlob->GetBufferPointer(), vBlob->GetBufferSize(), nullptr, screenVS.GetAddressOf());
	assert(result == S_OK);

	result = D3DCompileFromFile(screenShaderPath.c_str(), nullptr, nullptr, "ScreenPS", "ps_5_0", 0, compileFlag, &pBlob, &errBlob);
	assert(result == S_OK);

	std::cout << errBlob << endl;

	result = mDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, screenPS.GetAddressOf());
	assert(result == S_OK);

	std::cout << errBlob << endl;

	

	result = D3DCompileFromFile(computeShaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "rayTraceCS", "cs_5_0", compileFlag,0 , &cBlob, &errBlob);
	
	if(errBlob != nullptr)
		std::cout << (const char*)errBlob->GetBufferPointer() << endl;
	assert(result == S_OK);


	result = mDevice->CreateComputeShader(cBlob->GetBufferPointer(), cBlob->GetBufferSize(), nullptr, rayCS.GetAddressOf());
	assert(result == S_OK);

}

void Engine::generateRaytraceResources()
{
	D3D11_TEXTURE2D_DESC texDesc{};
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	texDesc.ArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.SampleDesc.Count = 1;

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = texDesc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = texDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	auto result = mDevice->CreateTexture2D(&texDesc, nullptr, rayTexture.GetAddressOf());
	assert(result == S_OK);

	result = mDevice->CreateUnorderedAccessView(rayTexture.Get(), &uavDesc, rayUAV.GetAddressOf());
	assert(result == S_OK);

	result = mDevice->CreateShaderResourceView(rayTexture.Get(), &srvDesc, raySRV.GetAddressOf());
	assert(result == S_OK);

}

void Engine::raytrace()
{
	ID3D11UnorderedAccessView* nullUAV[] = { nullptr };
	mContext->CSSetShader(rayCS.Get(), nullptr, 0);
	mContext->CSSetUnorderedAccessViews(0, 1, rayUAV.GetAddressOf(), nullptr);

	mContext->Dispatch(50, 50, 1);
	mContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);

}

void Engine::drawQuad()
{
	UINT strides[] = { sizeof(Vertex) };
	UINT offsets[] = { 0 };

	ID3D11ShaderResourceView* nullSRV[] = { nullptr };
	ID3D11RenderTargetView* nullRTV[] = { nullptr };

	mContext->OMSetRenderTargets(1, backRTV.GetAddressOf(), nullptr);

	mContext->VSSetShader(screenVS.Get(), nullptr, 0);
	mContext->PSSetShader(screenPS.Get(), nullptr, 0);
	mContext->PSSetShaderResources(0, 1, raySRV.GetAddressOf());

	mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mContext->IASetVertexBuffers(0, 1, quadVertexBuffer.GetAddressOf(), strides, offsets);
	mContext->IASetIndexBuffer(quadIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	mContext->IASetInputLayout(screenIL.Get());
	mContext->ClearRenderTargetView(backRTV.Get(), Colors::Red);

	mContext->DrawIndexed(6, 0, 0);

	mContext->PSSetShaderResources(0, 1, nullSRV);
	mContext->OMSetRenderTargets(1, nullRTV, nullptr);
	mContext->ClearState();

}

void Engine::PostInitialize()
{
	DWORD flag = D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;

	DXGI_SWAP_CHAIN_DESC scDesc{};

	scDesc.BufferCount = 1;
	scDesc.BufferDesc.Width = mWidth;
	scDesc.BufferDesc.Height = mHeight;
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.BufferDesc.RefreshRate.Denominator = 1;
	scDesc.BufferDesc.RefreshRate.Numerator = 144;
	scDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.OutputWindow = hwnd;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scDesc.Windowed = true;

	auto Result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flag, nullptr, 0, D3D11_SDK_VERSION,
		&scDesc, mSwapChain.GetAddressOf(), mDevice.GetAddressOf(), &featureLevel, mContext.GetAddressOf());
	assert(Result == S_OK);


}

void Engine::Init()
{
	PostInitialize();

	generateQuad();
	generateBackbuffer();
	generateCamera();
	generateRenderStates();
	compileShaders();
	generateRaytraceResources();

	mContext->RSSetState(defaultRS.Get());
	mContext->RSSetViewports(1, &defaultViewport);
}

void Engine::Update(float delta)
{
	//raytrace();

	drawQuad();
}

void Engine::Render(float delta)
{
	mSwapChain->Present(0, 0);
}

void Engine::Release()
{
	defaultRS->Release();

	quadVertexBuffer->Release();
	quadIndexBuffer->Release();

	backBuffer->Release();
	backRTV->Release();

}
