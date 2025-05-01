// =======================================================
// MGCommon.cpp
// 
// MGパッケージの共通関数
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#include "MGCommon.h"
#include "renderer.h"
#include "spriteTool.h"
#include "audioTool.h"
#include "resourceTool.h"

namespace MG {
	static Renderer* g_renderer;
	static SpriteTool* g_spriteTool;
	static AudioTool* g_audioTool;
	static ResourceTool* g_resourceTool;
	static F4 g_screen;
	static int g_deltaTime;

	void SetRenderer(Renderer* renderer)
	{
		g_renderer = renderer;
	}

	Renderer* GetRenderer()
	{
		return g_renderer;
	}

	void SetSpriteTool(SpriteTool* spriteTool)
	{
		g_spriteTool = spriteTool;
	}

	SpriteTool* GetSpriteTool()
	{
		return g_spriteTool;
	}

	void SetAudioTool(AudioTool* audioTool)
	{
		g_audioTool = audioTool;
	}

	AudioTool* GetAudioTool()
	{
		return g_audioTool;
	}

	void SetResourceTool(ResourceTool* resourceTool)
	{
		g_resourceTool = resourceTool;
	}

	ResourceTool* GetResourceTool()
	{
		return g_resourceTool;
	}

	void SetScreen(float width, float height, float centerX, float centerY)
	{
		g_screen = {
			width,
			height,
			centerX,
			centerY
		};
	}

	F2 GetScreenSize()
	{
		return { g_screen.x, g_screen.y };
	}

	F2 GetScreenCenter()
	{
		return { g_screen.z, g_screen.w };
	}

	float GetScreenWidth()
	{
		return g_screen.x;
	}

	float GetScreenHeight()
	{
		return g_screen.y;
	}

	float GetScreenCenterX()
	{
		return g_screen.z;
	}

	float GetScreenCenterY()
	{
		return g_screen.w;
	}

	void SetDeltaTime(int deltaTime)
	{
		g_deltaTime = deltaTime;
	}

	int GetDeltaTime()
	{
		return g_deltaTime;
	}

	Texture* LoadTexture(const std::string& path)
	{
		return LoadTexture(path, RESOURCE_SCOPE_GOBAL);
	}

	Texture* LoadTexture(const std::string& path, const std::string& scope)
	{
		if (g_resourceTool) {
			return g_resourceTool->LoadTexture(path, scope);
		}
		return nullptr;
	}

	void ReleaseTexture(const std::string& path, const std::string& scope)
	{
		g_resourceTool->ReleaseTexture(path, scope);
	}

	void ReleaseTexture(const std::string& scope)
	{
		g_resourceTool->ReleaseTexture(scope);
	}

	Audio* LoadAudio(const std::string& path)
	{
		return LoadAudio(path, RESOURCE_SCOPE_GOBAL);
	}
	Audio* LoadAudio(const std::string& path, const std::string& scope)
	{
		if (g_resourceTool) {
			return g_resourceTool->LoadAudio(path, scope);
		}
		return nullptr;
	}
	void ReleaseAudio(const std::string& path, const std::string& scope)
	{
		g_resourceTool->ReleaseAudio(path, scope);
	}

	void ReleaseAudio(const std::string& scope)
	{
		g_resourceTool->ReleaseAudio(scope);
	}

	void ReleaseResource(const std::string& scope)
	{
		g_resourceTool->ReleaseResource(scope);
	}

	void DrawQuad(Texture* texture, F3 position, F3 size, F3 rotate, F4 color, F2 uvOffset, F2 uvRange
	)
	{
		if (g_spriteTool) {
			g_spriteTool->DrawQuad(texture, position, size, rotate, color, uvOffset, uvRange);
		}
	}
}