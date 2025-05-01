// =======================================================
// spriteToolDX.cpp
// 
// DirectX環境のスプライトツール
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#include "spriteToolDX.h"
#include "rendererDX.h"

namespace MG {
	SpriteToolDX::SpriteToolDX(RendererDX* renderer, int vertexNum) : renderer(renderer), vertexNum(vertexNum)
	{
		ID3D11Device* pDevice = renderer->GetDevice();

		// 頂点バッファ生成
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX_3D) * vertexNum;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		pDevice->CreateBuffer(&bd, NULL, &m_VertexBuffer);

	}

	SpriteToolDX::~SpriteToolDX()
	{
		if (m_VertexBuffer)
		{
			m_VertexBuffer->Release();
			m_VertexBuffer = NULL;
		}
	}

	void SpriteToolDX::DrawQuad(Texture* texture, F3 position, F3 size, F3 rotate, F4 color, F2 uvOffset, F2 uvRange)
	{
		DrawQuad(((TextureDX*)texture)->resourceView,
			TOXMFLOAT3(position),
			TOXMFLOAT3(size),
			TOXMFLOAT3(rotate),
			TOXMFLOAT4(color),
			TOXMFLOAT2(uvOffset),
			TOXMFLOAT2(uvRange)
		);
	}

	void SpriteToolDX::DrawQuad(ID3D11ShaderResourceView* resourceView, XMFLOAT3 position, XMFLOAT3 size, XMFLOAT3 rotate, XMFLOAT4 color, XMFLOAT2 uvOffset, XMFLOAT2 uvRange)
	{
		static const int SPRITE_VERTEX_NUM = 4;
		static XMFLOAT3 normalizedRect[SPRITE_VERTEX_NUM] =
		{
			XMFLOAT3(-1.0f, -1.0f, 0.0f),
			XMFLOAT3(1.0f, -1.0f, 0.0f),
			XMFLOAT3(-1.0f, 1.0f, 0.0f),
			XMFLOAT3(1.0f, 1.0f, 0.0f)
		};
		static XMFLOAT2 defaultTexCoord[SPRITE_VERTEX_NUM] =
		{
			XMFLOAT2(0.0f, 0.0f),
			XMFLOAT2(1.0f, 0.0f),
			XMFLOAT2(0.0f, 1.0f),
			XMFLOAT2(1.0f, 1.0f),
		};

		ID3D11DeviceContext* context = renderer->GetDeviceContext();

		//頂点の作成
		D3D11_MAPPED_SUBRESOURCE msr;
		context->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		for (int i = 0; i < SPRITE_VERTEX_NUM; i++) {
			vertex[i].Position = normalizedRect[i];
			vertex[i].Diffuse = color;
			vertex[i].TexCoord = XMFLOAT2(defaultTexCoord[i].x * uvRange.x + uvOffset.x, defaultTexCoord[i].y * uvRange.y + uvOffset.y);
		}

		context->Unmap(m_VertexBuffer, 0);

		context->PSSetShaderResources(0, 1, &resourceView);

		renderer->SetWorldMatrix(
			XMMatrixScaling(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f) *
			XMMatrixRotationZ(XMConvertToRadians(rotate.z)) *
			XMMatrixRotationY(XMConvertToRadians(rotate.y)) *
			XMMatrixRotationX(XMConvertToRadians(rotate.x)) *
			XMMatrixTranslation(position.x, position.y, position.z)
		);

		// 頂点バッファ設定
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

		// プリミティブトポロジ設定
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// マテリアル設定
		MATERIAL material;
		ZeroMemory(&material, sizeof(material));
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		renderer->SetMaterial(material);

		// ポリゴン描画
		context->Draw(4, 0);
	}

} // namespace MG