// =======================================================
// spriteToolDX.h
// 
// DirectX環境のリソース管理
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#ifndef _RESOURCE_TOOL_DX_H
#define _RESOURCE_TOOL_DX_H

#include "resourceTool.h"
#include "rendererDX.h"
#include <xaudio2.h>

namespace MG {
	// =======================================================
	// DirectX環境のテクスチャリソース
	// =======================================================
	class TextureDX : public Texture {
	public:
		TextureDX(ID3D11ShaderResourceView* resourceView);
		ID3D11ShaderResourceView* resourceView;
	};


	// =======================================================
	// DirectX環境のオーディオリソース
	// =======================================================
	class AudioDX : public Audio {
	public:
		AudioDX(BYTE* soundData, WAVEFORMATEX waveFormatEX, int length, int playLength);
		BYTE* soundData;
		WAVEFORMATEX waveFormatEX;
		int length;
		int playLength;
	};


	// =======================================================
	// DirectX環境のリソース管理クラス
	// =======================================================
	class ResourceToolDX : public ResourceTool {
	protected:
		RendererDX* renderer;
		void __ReleaseResource(const string& key) override;
	public:
		ResourceToolDX(RendererDX* renderer);
		Texture* LoadTexture(const std::string& path, const std::string& scope = RESOURCE_SCOPE_GOBAL) override;
		Texture* LoadTexture(unsigned int resourceId, const std::string& scope = RESOURCE_SCOPE_GOBAL) override;
		Audio* LoadAudio(const std::string& path, const std::string& scope = RESOURCE_SCOPE_GOBAL) override;
		Audio* LoadAudio(unsigned int resourceId, const std::string& scope = RESOURCE_SCOPE_GOBAL) override;
	};

} // namespace MG

#endif