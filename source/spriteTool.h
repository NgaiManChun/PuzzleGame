// =======================================================
// spriteTool.h
// 
// 描画ツール、抽象クラス
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#ifndef _SPRITE_TOOL_H
#define _SPRITE_TOOL_H

#include "MGCommon.h"

namespace MG {
	class SpriteTool {
	public:
		virtual void DrawQuad(Texture* texture, F3 position, F3 size,
			F3 rotate = { 0.0f, 0.0f, 0.0f },
			F4 color = { 1.0f, 1.0f, 1.0f, 1.0f },
			F2 uvOffset = { 0.0f, 0.0f },
			F2 uvRange = { 1.0f, 1.0f }
		) = 0;
	};

} // namespace MG

#endif