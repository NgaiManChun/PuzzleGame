// =======================================================
// MGCommon.h
// 
// MGパッケージの共通関数
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#ifndef _MG_COMMON_H
#define _MG_COMMON_H

#include <string>
#include <list>
#include <map>

namespace MG {
	using std::string;
	using std::list;
	using std::map;

	class Renderer;
	class SpriteTool;
	class AudioTool;
	class ResourceTool;

	class Resource;
	class Texture;
	class Audio;
	class AudioPlayer;

	class Scene;
	class SceneTransition;
	class GameObject;

	struct F2 {
		float x;
		float y;
	};

	struct F3 {
		float x;
		float y;
		float z;
	};

	struct F4 {
		float x;
		float y;
		float z;
		float w;
	};

	void SetRenderer(Renderer* renderer);
	Renderer* GetRenderer();
	void SetSpriteTool(SpriteTool* spriteTool);
	SpriteTool* GetSpriteTool();
	void SetAudioTool(AudioTool* audioTool);
	AudioTool* GetAudioTool();
	void SetResourceTool(ResourceTool* resourceTool);
	ResourceTool* GetResourceTool();

	void SetScreen(float width, float height, float centerX, float centerY);
	F2 GetScreenSize();
	F2 GetScreenCenter();
	float GetScreenWidth();
	float GetScreenHeight();
	float GetScreenCenterX();
	float GetScreenCenterY();
	void SetDeltaTime(int deltaTime);
	int GetDeltaTime();

	Texture* LoadTexture(const std::string& path);
	Texture* LoadTexture(const std::string& path, const std::string& scope);
	void ReleaseTexture(const std::string& path, const std::string& scope);
	void ReleaseTexture(const std::string& scope);

	Audio* LoadAudio(const std::string& path);
	Audio* LoadAudio(const std::string& path, const std::string& scope);
	void ReleaseAudio(const std::string& path, const std::string& scope);
	void ReleaseAudio(const std::string& scope);

	void ReleaseResource(const std::string& scope);

	void DrawQuad(Texture* texture, F3 position, F3 size,
		F3 rotate = { 0.0f, 0.0f, 0.0f },
		F4 color = { 1.0f, 1.0f, 1.0f, 1.0f },
		F2 uvOffset = { 0.0f, 0.0f },
		F2 uvRange = { 1.0f, 1.0f }
	);

} // namespace MG

#endif