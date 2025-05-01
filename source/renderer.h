// =======================================================
// renderer.h
// 
// レンダラー、抽象クラス
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#ifndef _RENDERER_H
#define _RENDERER_H

#include "MGCommon.h"

namespace MG {
	class Renderer {
	public:
		virtual void SetViewport(float x, float y, float width, float height) = 0;
	};

} // namespace MG

#endif 