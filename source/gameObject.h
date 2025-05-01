// =======================================================
// gameObject.h
// 
// ゲームオブジェクト
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include "MGCommon.h"
#include <xaudio2.h>

namespace MG {
	// =======================================================
	// 基底ゲームオブジェクト
	// =======================================================
	class GameObject {
	public:
		bool enable = true;
		F3 size;
		F3 position;
		F3 rotate;
		Scene* scene;

		GameObject(F3 size, F3 position, F3 rotate);
		virtual ~GameObject();
		virtual void Update();
		virtual void Draw() = 0;
		virtual std::string GetType();
	};


	// =======================================================
	// 板ゲームオブジェクト
	// =======================================================
	class GameObjectQuad : public GameObject {
	public:
		Texture* texture;
		F4 color;
		F2 uvOffset;
		F2 uvRange;

		GameObjectQuad(
			F3 size, F3 position,
			Texture* texture,
			F3 rotate = F3{ 0.0f, 0.0f, 0.0f },
			F4 color = F4{ 1.0f, 1.0f, 1.0f, 1.0f },
			F2 uvOffset = F2{ 0.0f, 0.0f },
			F2 uvRange = F2{ 1.0f, 1.0f }
		);
		void Draw() override;
		std::string GetType() override;
	};


	// =======================================================
	// 文字ゲームオブジェクト
	// =======================================================
	enum TEXT_ORIGN {
		LEFT_TOP,
		RIGHT_TOP,
		CENTER
	};

	class GameObjectText : public GameObjectQuad {
	private:
		Texture* tex_ascii;
		Texture* tex_sjis_81_84;
		Texture* tex_sjis_88_8b;
		Texture* tex_sjis_8c_8f;
		Texture* tex_sjis_90_93;
		Texture* tex_sjis_94_97;
		Texture* tex_sjis_98_9b;
		Texture* tex_sjis_9c_9f;
	public:
		std::string value;
		float spacing;
		TEXT_ORIGN orign;

		GameObjectText(
			std::string value, F3 size, F3 position,
			TEXT_ORIGN orign = LEFT_TOP,
			float spacing = 0.0f,
			F3 rotate = F3{ 0.0f, 0.0f, 0.0f },
			F4 color = F4{ 1.0f, 1.0f, 1.0f, 1.0f }
		);
		void SetValue(std::string value);
		void Draw() override;
		std::string GetType() override;
	};


	// =======================================================
	// オーディオゲームオブジェクト
	// =======================================================
	class GameObjectAudio : public GameObject {
	public:
		AudioPlayer* audioPlayer;

		GameObjectAudio(Audio* audio, bool loop = false);
		~GameObjectAudio() override;
		void Draw() override;
		void Update() override;
		void Play();
		void Stop();
		void SetLoop(bool loop);
		void SetVolume(float volume);
		std::string GetType() override;
	};

} // namespace MG

#endif

