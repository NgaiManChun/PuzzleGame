// =======================================================
// spriteToolDX.h
// 
// DirectX環境のスプライトツール
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#ifndef _SPRITE_TOOL_DX_H
#define _SPRITE_TOOL_DX_H

#include "spriteTool.h"
#include "rendererDX.h"
#include "resourceToolDX.h"

namespace MG {
	class SpriteToolDX : public SpriteTool {
	protected:
		RendererDX* renderer;
		ID3D11Buffer* m_VertexBuffer;
		int vertexNum;
	public:
		SpriteToolDX(RendererDX* renderer, int vertexNum);
		~SpriteToolDX();
		void DrawQuad(Texture* texture, F3 position, F3 size, F3 rotate, F4 color, F2 uvOffset, F2 uvRange) override;
		void DrawQuad(ID3D11ShaderResourceView* resourceView, XMFLOAT3 position, XMFLOAT3 size,
			XMFLOAT3 rotate = XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT4 color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
			XMFLOAT2 uvOffset = XMFLOAT2(0.0f, 0.0f),
			XMFLOAT2 uvRange = XMFLOAT2(1.0f, 1.0f)
		);
	};

} // namespace MG

#endif