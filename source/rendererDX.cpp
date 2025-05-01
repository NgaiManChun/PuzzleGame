// =======================================================
// spriteToolDX.cpp
// 
// DirectX環境のレンダラー
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// 2024/09/11 複数ブレンドステート追加
// =======================================================
#include "rendererDX.h"

namespace MG {
	RendererDX::RendererDX(float screenWidth, float screenHeight) : screenWidth(screenWidth), screenHeight(screenHeight) {}
	RendererDX::~RendererDX() {}
	ID3D11ShaderResourceView* RendererDX::GetTempResourceView() { return m_tempResourceView; }
	ID3D11Device* RendererDX::GetDevice(void) { return m_device; }
	ID3D11DeviceContext* RendererDX::GetDeviceContext(void) { return m_context; }
	IDXGISwapChain* RendererDX::GetSwapChain(void) { return m_swapChain; }

	HRESULT RendererDX::Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow) {

		// デバイス、スワップチェーン、コンテキスト生成
		{
			DXGI_SWAP_CHAIN_DESC swapChainDesc;
			ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
			swapChainDesc.BufferCount = 1;
			swapChainDesc.BufferDesc.Width = screenWidth;
			swapChainDesc.BufferDesc.Height = screenHeight;
			swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.OutputWindow = hWnd;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.Windowed = TRUE;

			D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

			HRESULT hr = D3D11CreateDeviceAndSwapChain(
				NULL,
				D3D_DRIVER_TYPE_HARDWARE,
				NULL,
				D3D11_CREATE_DEVICE_BGRA_SUPPORT,
				NULL,
				0,
				D3D11_SDK_VERSION,
				&swapChainDesc,
				&m_swapChain,
				&m_device,
				&featureLevel,
				&m_context
			);
			if (FAILED(hr)) return hr;
		}

		// レンダーターゲットビュー生成
		{
			ID3D11Texture2D* pBackBuffer = NULL;
			m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			m_device->CreateRenderTargetView(pBackBuffer, NULL, &m_renderTargetView);
			pBackBuffer->Release();
		}

		// デプスビュー生成
		{
			ID3D11Texture2D* texture = NULL;
			D3D11_TEXTURE2D_DESC texture2DDesc;
			ZeroMemory(&texture2DDesc, sizeof(texture2DDesc));
			texture2DDesc.Width = screenWidth;
			texture2DDesc.Height = screenHeight;
			texture2DDesc.MipLevels = 1;
			texture2DDesc.ArraySize = 1;
			texture2DDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			texture2DDesc.SampleDesc.Count = 1;
			texture2DDesc.SampleDesc.Quality = 0;
			texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
			texture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			texture2DDesc.CPUAccessFlags = 0;
			texture2DDesc.MiscFlags = 0;
			m_device->CreateTexture2D(&texture2DDesc, NULL, &texture);

			//ステンシルターゲット作成
			D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
			ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
			depthStencilViewDesc.Format = texture2DDesc.Format;
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			depthStencilViewDesc.Flags = 0;
			m_device->CreateDepthStencilView(texture, &depthStencilViewDesc, &m_depthStencilView);
			texture->Release();
		}

		// 一時テクスチャ生成
		{
			ID3D11Texture2D* texture;
			D3D11_TEXTURE2D_DESC texture2DDesc;
			ZeroMemory(&texture2DDesc, sizeof(texture2DDesc));
			texture2DDesc.Width = screenWidth;
			texture2DDesc.Height = screenHeight;
			texture2DDesc.MipLevels = 1;
			texture2DDesc.ArraySize = 1;
			texture2DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			texture2DDesc.SampleDesc.Count = 1;
			texture2DDesc.SampleDesc.Quality = 0;
			texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
			texture2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			texture2DDesc.SampleDesc.Count = 1;
			m_device->CreateTexture2D(&texture2DDesc, NULL, &texture);
			m_device->CreateShaderResourceView(texture, NULL, &m_tempResourceView);
			m_device->CreateRenderTargetView(texture, NULL, &m_tempRenderTargetView);
			texture->Release();
		}

		// ビューポート設定
		{
			D3D11_VIEWPORT viewport;
			viewport.Width = (FLOAT)screenWidth;
			viewport.Height = (FLOAT)screenHeight;
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			viewport.TopLeftX = 0;
			viewport.TopLeftY = 0;
			m_context->RSSetViewports(1, &viewport);
		}

		// ラスタライザステート設定
		{
			D3D11_RASTERIZER_DESC rasterizerDesc;
			ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
			rasterizerDesc.FillMode = D3D11_FILL_SOLID;
			rasterizerDesc.CullMode = D3D11_CULL_BACK;
			rasterizerDesc.DepthClipEnable = TRUE;
			rasterizerDesc.MultisampleEnable = FALSE;
			m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState);
			m_context->RSSetState(m_rasterizerState);
		}

		// ブレンドステート設定
		{
			D3D11_BLEND_DESC belendDesc;
			ZeroMemory(&belendDesc, sizeof(belendDesc));
			belendDesc.AlphaToCoverageEnable = FALSE;
			belendDesc.IndependentBlendEnable = TRUE;
			belendDesc.RenderTarget[0].BlendEnable = TRUE;
			belendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			belendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			belendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			belendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			belendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			belendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			belendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			
			// 無効
			belendDesc.RenderTarget[0].BlendEnable = FALSE;
			m_device->CreateBlendState(&belendDesc, &bState[BLENDSTATE_NONE]);

			// αブレンド
			belendDesc.RenderTarget[0].BlendEnable = TRUE;
			m_device->CreateBlendState(&belendDesc, &bState[BLENDSTATE_ALPHA]);

			// 加算合成
			belendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			belendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			belendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			m_device->CreateBlendState(&belendDesc, &bState[BLENDSTATE_ADD]);

			// 減算合成
			belendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			belendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			belendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
			m_device->CreateBlendState(&belendDesc, &bState[BLENDSTATE_SUB]);

			SetBlendState(BLENDSTATE_ALPHA);

		}

		// 深度ステンシルステート設定
		{
			D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
			ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
			depthStencilDesc.DepthEnable = FALSE;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
			depthStencilDesc.StencilEnable = FALSE;
			m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthState);
			m_context->OMSetDepthStencilState(m_depthState, NULL);
		}

		// サンプラーステート設定
		{
			D3D11_SAMPLER_DESC samplerDesc;
			ZeroMemory(&samplerDesc, sizeof(samplerDesc));
			samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.MipLODBias = 0;
			samplerDesc.MaxAnisotropy = 16;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			m_device->CreateSamplerState(&samplerDesc, &m_samplerState);
			m_context->PSSetSamplers(0, 1, &m_samplerState);
		}

		// 頂点シェーダコンパイル・生成
		{

			ID3DBlob* pErrorBlob;
			ID3DBlob* pVSBlob = NULL;
			HRESULT hr = D3DCompileFromFile(L"shader.hlsl", NULL, NULL, "VertexShaderPolygon", "vs_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pVSBlob, &pErrorBlob);
			if (FAILED(hr))
			{
				MessageBox(NULL, (char*)pErrorBlob->GetBufferPointer(), "VS", MB_OK | MB_ICONERROR);
			}

			m_device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &m_VertexShader);

			// 入力レイアウト生成
			D3D11_INPUT_ELEMENT_DESC layout[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};
			UINT numElements = ARRAYSIZE(layout);

			m_device->CreateInputLayout(layout,
				numElements,
				pVSBlob->GetBufferPointer(),
				pVSBlob->GetBufferSize(),
				&m_VertexLayout);

			pVSBlob->Release();
		}

		// ピクセルシェーダコンパイル・生成
		{
			ID3DBlob* pErrorBlob;
			ID3DBlob* pPSBlob = NULL;
			HRESULT hr = D3DCompileFromFile(L"shader.hlsl", NULL, NULL, "PixelShaderPolygon", "ps_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pPSBlob, &pErrorBlob);
			if (FAILED(hr))
			{
				MessageBox(NULL, (char*)pErrorBlob->GetBufferPointer(), "PS", MB_OK | MB_ICONERROR);
			}

			m_device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &m_PixelShader);

			pPSBlob->Release();
		}

		// 定数バッファ生成
		D3D11_BUFFER_DESC hBufferDesc;
		{
			hBufferDesc.ByteWidth = sizeof(XMMATRIX);
			hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			hBufferDesc.CPUAccessFlags = 0;
			hBufferDesc.MiscFlags = 0;
			hBufferDesc.StructureByteStride = sizeof(float);

			m_device->CreateBuffer(&hBufferDesc, NULL, &m_ConstantBuffer);
			m_context->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);
		}

		{
			hBufferDesc.ByteWidth = sizeof(MATERIAL);
			hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			hBufferDesc.CPUAccessFlags = 0;
			hBufferDesc.MiscFlags = 0;
			hBufferDesc.StructureByteStride = sizeof(float);

			m_device->CreateBuffer(&hBufferDesc, NULL, &m_MaterialBuffer);
			m_context->VSSetConstantBuffers(1, 1, &m_MaterialBuffer);
		}

		{
			hBufferDesc.ByteWidth = sizeof(VIEW_COLOR);
			hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			hBufferDesc.CPUAccessFlags = 0;
			hBufferDesc.MiscFlags = 0;
			hBufferDesc.StructureByteStride = sizeof(float);

			m_device->CreateBuffer(&hBufferDesc, NULL, &m_ColorBuffer);
			m_context->PSSetConstantBuffers(2, 1, &m_ColorBuffer);
		}

		// 入力レイアウト設定
		m_context->IASetInputLayout(m_VertexLayout);

		// シェーダ設定
		m_context->VSSetShader(m_VertexShader, NULL, 0);
		m_context->PSSetShader(m_PixelShader, NULL, 0);

		// 行列の初期化
		m_ProjectionMatrix = XMMatrixOrthographicOffCenterLH(0.0f, screenWidth, screenHeight, 0.0f, 0.0f, 1.0f);
		m_ViewMatrix = XMMatrixIdentity();
		m_WorldMatrix = XMMatrixIdentity();

		return S_OK;

	}

	void RendererDX::Uninit()
	{
		m_rasterizerState->Release();
		for (int i = 0; i < BLENDSTATE_MAX; i++) {
			bState[i]->Release();
		}
		m_depthState->Release();
		m_samplerState->Release();

		m_ConstantBuffer->Release();
		m_MaterialBuffer->Release();
		m_ColorBuffer->Release();

		m_VertexLayout->Release();
		m_VertexShader->Release();
		m_PixelShader->Release();

		m_tempResourceView->Release();
		m_tempRenderTargetView->Release();

		m_renderTargetView->Release();
		m_depthStencilView->Release();

		m_swapChain->Release();
		m_context->Release();
		m_device->Release();

	}

	void RendererDX::UpdateWorldViewProjection() {
		XMMATRIX worldViewProjection = m_WorldMatrix * m_ViewMatrix * m_ProjectionMatrix;
		worldViewProjection = XMMatrixTranspose(worldViewProjection);

		XMFLOAT4X4 matrix;
		XMStoreFloat4x4(&matrix, worldViewProjection);
		m_context->UpdateSubresource(m_ConstantBuffer, 0, NULL, &matrix, 0, 0);
	}

	void RendererDX::SetWorldMatrix(XMMATRIX WorldMatrix)
	{
		m_WorldMatrix = WorldMatrix;
		UpdateWorldViewProjection();
	}

	void RendererDX::SetViewMatrix(XMMATRIX ViewMatrix)
	{
		m_ViewMatrix = ViewMatrix;
		UpdateWorldViewProjection();
	}

	void RendererDX::SetProjectionMatrix(XMMATRIX ProjectionMatrix)
	{
		m_ProjectionMatrix = ProjectionMatrix;
		UpdateWorldViewProjection();
	}

	void RendererDX::SetMaterial(MATERIAL Material)
	{
		m_context->UpdateSubresource(m_MaterialBuffer, 0, NULL, &Material, 0, 0);
	}

	void RendererDX::SetViewColor(VIEW_COLOR ViewColor) {
		m_context->UpdateSubresource(m_ColorBuffer, 0, NULL, &ViewColor, 0, 0);
	}

	void RendererDX::Present()
	{
		m_swapChain->Present(0, 0);
	}

	void RendererDX::ClearMainView(void)
	{
		float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_context->ClearRenderTargetView(m_renderTargetView, ClearColor);
		m_context->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void RendererDX::ClearTempView(void)
	{
		float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_context->ClearRenderTargetView(m_tempRenderTargetView, ClearColor);
		m_context->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void RendererDX::UseMainView(void)
	{
		m_context->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	}

	void RendererDX::UseTempView(void)
	{
		m_context->OMSetRenderTargets(1, &m_tempRenderTargetView, m_depthStencilView);
	}

	void RendererDX::SetViewport(float x, float y, float width, float height)
	{
		D3D11_VIEWPORT vp;
		vp.Width = width;
		vp.Height = height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = x - width * 0.5f;
		vp.TopLeftY = y - height * 0.5f;
		m_context->RSSetViewports(1, &vp);
	}

	void RendererDX::SetBlendState(BLENDSTATE blend)
	{
		m_context->OMSetBlendState(bState[blend], bFactor, 0xffffffff);
	}

} // namespace MG