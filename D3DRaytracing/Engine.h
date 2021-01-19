#pragma once

using namespace Microsoft;
using namespace DirectX;

struct Vertex
{
	XMFLOAT4 mPosition;
	XMFLOAT2 mUV;

	Vertex(float px, float py, float pz, float u, float v) : mPosition(px, py, pz, 1.0f), mUV(u, v) {}
	~Vertex() {}
};

class Engine
{
	HWND hwnd;

	WRL::ComPtr<ID3D11Device> mDevice;
	WRL::ComPtr<IDXGISwapChain> mSwapChain;
	WRL::ComPtr<ID3D11DeviceContext> mContext;

	UINT mWidth = 800, mHeight = 600;

	WRL::ComPtr<ID3D11ComputeShader> rayCS;
	WRL::ComPtr<ID3D11VertexShader> screenVS;
	WRL::ComPtr<ID3D11PixelShader> screenPS;
	WRL::ComPtr<ID3D11InputLayout> screenIL;

	WRL::ComPtr<ID3D11Texture2D> backBuffer;
	WRL::ComPtr<ID3D11RenderTargetView> backRTV;

	WRL::ComPtr<ID3D11Texture2D> rayTexture;
	WRL::ComPtr<ID3D11UnorderedAccessView> rayUAV;
	WRL::ComPtr<ID3D11ShaderResourceView> raySRV;

	WRL::ComPtr<ID3D11Buffer> quadVertexBuffer;
	WRL::ComPtr<ID3D11Buffer> quadIndexBuffer;

	WRL::ComPtr<ID3D11Buffer> cameraBuffer;

	WRL::ComPtr<ID3D11RasterizerState> defaultRS;
	D3D11_VIEWPORT defaultViewport;

	XMMATRIX mView;
	XMMATRIX mProjection;

private:
	void generateQuad();
	void generateBackbuffer();
	void generateCamera();

	void generateRenderStates();

	void compileShaders();

	void generateRaytraceResources();
	void raytrace();

	void drawQuad();
public:

	Engine(HWND windowHandle) : hwnd(windowHandle) {}

	

	void PostInitialize();
	void Init();
	void Update(float delta);
	void Render(float delta);
	void Release();

};

