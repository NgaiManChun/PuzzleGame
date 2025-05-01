// =======================================================
// spriteToolDX.h
// 
// DirectX環境のレンダラー
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// 2024/09/11 複数ブレンドステート追加
// =======================================================
#ifndef _RENDERER_DX_H
#define _RENDERER_DX_H

#include "renderer.h"

#pragma warning(push)
#pragma warning(disable:4005)

#define _CRT_SECURE_NO_WARNINGS			// scanf のwarning防止
#include <stdio.h>

#include <d3d11.h>
#include <d3dcompiler.h>

#define DIRECTINPUT_VERSION 0x0800		// 警告対処
#include "dinput.h"
#include "mmsystem.h"

#pragma warning(pop)

#include <DirectXMath.h>
using namespace DirectX;


//テクスチャサポートライブラリ
#include    "DirectXTex.h"
//デバッグビルドかリリースビルドでリンクするライブラリを選択する
#if _DEBUG
#pragma comment(lib, "DirectXTex_Debug.lib")    //デバッグ用
#else
#pragma comment(lib, "DirectXTex_Release.lib")  //リリース用
#endif

#define TOXMFLOAT2(F2) XMFLOAT2(F2.x, F2.y)
#define TOXMFLOAT3(F3) XMFLOAT3(F3.x, F3.y, F3.z)
#define TOXMFLOAT4(F4) XMFLOAT4(F4.x, F4.y, F4.z, F4.w)

namespace MG {

	// ブレンドステート定義
	enum BLENDSTATE
	{
		BLENDSTATE_NONE = 0,	// ブレンドしない
		BLENDSTATE_ALPHA,		// 普通のαブレンド
		BLENDSTATE_ADD,			// 加算合成
		BLENDSTATE_SUB,			// 減算合成
		BLENDSTATE_MAX
	};

	// 頂点構造体
	struct VERTEX_3D
	{
		XMFLOAT3 Position;	//頂点座標　XMFLOAT3 ＝　float x,y,z
		XMFLOAT3 Normal;	//法線ベクトル 
		XMFLOAT4 Diffuse;	//色  XMFLOAT4 = float x,y,z,w
		XMFLOAT2 TexCoord;	//テクスチャ座標 XMFLOAT2 = float x,y
	};

	// マテリアル構造体
	struct MATERIAL
	{
		XMFLOAT4	Ambient;
		XMFLOAT4	Diffuse;
		XMFLOAT4	Specular;
		XMFLOAT4	Emission;
		float		Shininess;
		float		Dummy[3];//16byte境界用（パディング；Padding）
	};

	// カラー構造体
	struct VIEW_COLOR
	{
		XMFLOAT4 Color1;
		XMFLOAT4 Color2;
		XMFLOAT3 ColorPosition1;
		XMFLOAT3 ColorPosition2;
		XMFLOAT2 Dummy2; //16bit境界用
	};

	class RendererDX : public Renderer {
	protected:
		float screenWidth;
		float screenHeight;

		ID3D11Device* m_device = NULL;
		ID3D11DeviceContext* m_context = NULL;
		IDXGISwapChain* m_swapChain = NULL;

		ID3D11RenderTargetView* m_renderTargetView = NULL;
		ID3D11DepthStencilView* m_depthStencilView = NULL;

		ID3D11ShaderResourceView* m_tempResourceView = NULL;
		ID3D11RenderTargetView* m_tempRenderTargetView = NULL;

		ID3D11RasterizerState* m_rasterizerState = NULL;
		ID3D11DepthStencilState* m_depthState = NULL;
		ID3D11SamplerState* m_samplerState = NULL;

		ID3D11InputLayout* m_VertexLayout = NULL;
		ID3D11VertexShader* m_VertexShader = NULL;
		ID3D11PixelShader* m_PixelShader = NULL;

		ID3D11Buffer* m_ConstantBuffer = NULL;
		ID3D11Buffer* m_MaterialBuffer = NULL;
		ID3D11Buffer* m_ColorBuffer = NULL;

		XMMATRIX m_WorldMatrix = {};
		XMMATRIX m_ViewMatrix = {};
		XMMATRIX m_ProjectionMatrix = {};

		float bFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		ID3D11BlendState* bState[BLENDSTATE_MAX];
	public:
		RendererDX(float screenWidth, float screenHeight);
		~RendererDX();
		HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
		void Uninit();
		void UpdateWorldViewProjection();
		void SetWorldMatrix(XMMATRIX WorldMatrix);
		void SetViewMatrix(XMMATRIX ViewMatrix);
		void SetProjectionMatrix(XMMATRIX ProjectionMatrix);
		void SetMaterial(MATERIAL Material);
		void SetViewColor(VIEW_COLOR ViewColor);
		void Present(void);
		void ClearMainView(void);
		void ClearTempView(void);
		void UseMainView(void);
		void UseTempView(void);
		void SetViewport(float x, float y, float width, float height) override;
		void SetBlendState(BLENDSTATE blend);
		ID3D11ShaderResourceView* GetTempResourceView();
		ID3D11Device* GetDevice(void);
		ID3D11DeviceContext* GetDeviceContext(void);
		IDXGISwapChain* GetSwapChain(void);
	};

} // namespace MG

#endif