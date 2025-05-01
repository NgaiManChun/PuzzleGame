// =======================================================
// gameObject.cpp
// 
// ゲームオブジェクト
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#include "gameObject.h"
#include "audioTool.h"

namespace MG {

	// =======================================================
	// 基底ゲームオブジェクト
	// =======================================================
	GameObject::GameObject(F3 size, F3 position, F3 rotate)
		: size(size), position(position), rotate(rotate), scene(nullptr) {}
	GameObject::~GameObject() {}
	void GameObject::Update() {}
	std::string GameObject::GetType() { return "GameObject"; }

	
	// =======================================================
	// 板ゲームオブジェクト
	// =======================================================
	GameObjectQuad::GameObjectQuad(
		F3 size, F3 position,
		Texture* texture,
		F3 rotate,
		F4 color,
		F2 uvOffset,
		F2 uvRange
	) : GameObject(size, position, rotate), texture(texture), color(color), uvOffset(uvOffset), uvRange(uvRange) {}
	std::string GameObjectQuad::GetType() { return "GameObjectQuad"; }

	void GameObjectQuad::Draw()
	{
		DrawQuad(texture, position, size, rotate, color, uvOffset, uvRange);
	}


	// =======================================================
	// 文字ゲームオブジェクト
	// =======================================================
	GameObjectText::GameObjectText(
		std::string value, F3 size, F3 position,
		TEXT_ORIGN orign,
		float spacing,
		F3 rotate,
		F4 color
	) :
		GameObjectQuad(size, position, texture, rotate, color), value(value.data()), spacing(spacing), orign(orign)
	{
		tex_ascii = LoadTexture("asset\\texture\\font\\ascii.png");
		tex_sjis_81_84 = LoadTexture("asset\\texture\\font\\sjis81_84.png");
		tex_sjis_88_8b = LoadTexture("asset\\texture\\font\\sjis88_8b.png");
		tex_sjis_8c_8f = LoadTexture("asset\\texture\\font\\sjis8c_8f.png");
		tex_sjis_90_93 = LoadTexture("asset\\texture\\font\\sjis90_93.png");
		tex_sjis_94_97 = LoadTexture("asset\\texture\\font\\sjis94_97.png");
		tex_sjis_98_9b = LoadTexture("asset\\texture\\font\\sjis98_9b.png");
		tex_sjis_9c_9f = LoadTexture("asset\\texture\\font\\sjis9c_9f.png");
	}

	std::string GameObjectText::GetType() { return "GameObjectText"; }

	void GameObjectText::SetValue(std::string value) {
		this->value = value.data();
	}

	void GameObjectText::Draw() {
		const char* chars = value.c_str();
		int length = value.length();

		float width = 0.0f;
		int count = 0;
		for (int i = 0; i < length; i++) {
			unsigned char c = chars[i];
			if (c >= 0x81 && c <= 0x84) {
				i++;
			}
			count++;
		}
		width = length * size.x * 0.5f;
		width += spacing * (count - 1);
		float currX = position.x;
		float currY = position.y;
		if (orign == LEFT_TOP || orign == RIGHT_TOP) {
			currY += size.y * 0.5f;
		}
		if (orign == RIGHT_TOP) {
			currX -= width + size.x;
		}
		if (orign == CENTER) {
			currX -= width * 0.5f;
		}
		for (int i = 0; i < length; i++) {
			unsigned char c = chars[i];

			Texture* tex;
			float c_width;
			int col = 0;
			int row = 0;
			int index = 0;


			if (c >= 32 && c <= 126) {
				col = 20;
				row = 9;
				index = c - 32;
				tex = tex_ascii;
				c_width = size.x * 0.5f;
			}
			else if (c >= 161 && c <= 223) {
				col = 20;
				row = 9;
				index = 20 * 5 + c - 161;
				tex = tex_ascii;
				c_width = size.x * 0.5f;
			}
			else {
				unsigned char c1 = 0;
				if (c >= 0x81 && c <= 0x9f) {
					if (i + 1 < length) {
						c1 = chars[i + 1];
						if (c1 < 0x40 || c1 > 0xfc) {
							c1 = 0;
						}
					}
				}

				if (c1 == 0) continue;

				c_width = size.x;
				col = 28;
				row = 27;

				if (c >= 0x81 && c <= 0x84) {
					tex = tex_sjis_81_84;
					index = (c - 0x81) * (0xfd - 0x40) + c1 - 0x40;
				}
				else if (c >= 0x88 && c <= 0x8b) {
					tex = tex_sjis_88_8b;
					index = (c - 0x88) * (0xfd - 0x40) + c1 - 0x40;
				}
				else if (c >= 0x8c && c <= 0x8f) {
					tex = tex_sjis_8c_8f;
					index = (c - 0x8c) * (0xfd - 0x40) + c1 - 0x40;
				}
				else if (c >= 0x90 && c <= 0x93) {
					tex = tex_sjis_90_93;
					index = (c - 0x90) * (0xfd - 0x40) + c1 - 0x40;
				}
				else if (c >= 0x94 && c <= 0x97) {
					tex = tex_sjis_94_97;
					index = (c - 0x94) * (0xfd - 0x40) + c1 - 0x40;
				}
				else if (c >= 0x98 && c <= 0x9b) {
					tex = tex_sjis_98_9b;
					index = (c - 0x98) * (0xfd - 0x40) + c1 - 0x40;
				}
				else if (c >= 0x9c && c <= 0x9f) {
					tex = tex_sjis_9c_9f;
					index = (c - 0x9c) * (0xfd - 0x40) + c1 - 0x40;
				}
				else {
					continue;
				}
				i++;
			}

			F2 uvRange = F2{ 1.0f / col, 1.0f / row };
			F2 uvOffset = F2{ uvRange.x * (index % col), uvRange.y * (index / col) };
			currX += c_width * 0.5f;
			DrawQuad(tex, F3{ currX, currY, position.z }, F3{ c_width, size.y, size.z }, rotate, color, uvOffset, uvRange);
			currX += c_width * 0.5f;
			currX += spacing;
		}
	}


	// =======================================================
	// オーディオゲームオブジェクト
	// =======================================================
	GameObjectAudio::GameObjectAudio(Audio* audio, bool loop) : GameObject({}, {}, {})
	{
		audioPlayer = GetAudioTool()->CreateAudioPlayer(audio, loop);
	}

	void GameObjectAudio::Draw() {}
	void GameObjectAudio::Update() {}
	std::string GameObjectAudio::GetType() { return "GameObjectAudio"; }

	GameObjectAudio::~GameObjectAudio()
	{
		delete audioPlayer;
		audioPlayer = nullptr;
	}

	void GameObjectAudio::Play()
	{
		audioPlayer->Start();
	}

	void GameObjectAudio::Stop()
	{
		audioPlayer->Stop();
	}

	void GameObjectAudio::SetLoop(bool loop)
	{
		audioPlayer->SetLoop(loop);
	}

	void GameObjectAudio::SetVolume(float volume)
	{
		audioPlayer->SetVolume(volume);
	}

} // namespace MG