#include "scene.h"
#include "asciiGameObject.h"
#include "progress.h"
using namespace MG;

namespace ColumnsTitleScene {
	constexpr const char* TEXTURE_BG = "asset\\texture\\columns\\title.jpg";
	constexpr const char* TEXTURE_ASCII = "asset\\texture\\columns\\ascii.png";

	// =======================================================
	// クラス定義
	// =======================================================
	class ColumnsTitleScene : public Scene {
	private:
		GameObjectQuad* background;
		Progress backgroundZoom;
	public:
		void Init() override;
		void Uninit() override;
		void Update() override;
	};


	// =======================================================
	// シーン登録
	// =======================================================
	static SceneName sceneName = REGISTER_SCENE("columns_title", ColumnsTitleScene);


	// =======================================================
	// 初期化
	// =======================================================
	void ColumnsTitleScene::Init()
	{
		Scene::Init();

		// スクリーンの中心座標を取得
		F2 screenCenter = GetScreenCenter();

		// スクリーンのサイズを取得
		F2 screenSize = GetScreenSize();

		// ゲームオブジェクトを追加
		background = (GameObjectQuad*)AddGameObject(
			new GameObjectQuad(
				{ screenSize.x, screenSize.y, 0.0f },
				{ screenCenter.x, screenCenter.y, 0.0f },
				LoadTexture(TEXTURE_BG)
			)
		);

		AsciiGameObject* label = (AsciiGameObject*)AddGameObject(
			new AsciiGameObject(
				{ 120.0f, 200.0f },
				{ screenCenter.x + 10.0f, screenCenter.y + 10.0f },
				LoadTexture(TEXTURE_ASCII),
				{}, 10, 10,
				"PUZZLE GAME", 0)
		);
		label->originH = HCENTER;
		label->originV = VCENTER;
		label->color = { 0.0f, 0.0f, 0.0f, 0.5f };

		label = (AsciiGameObject*)AddGameObject(
			new AsciiGameObject(
				{ 120.0f, 200.0f },
				{ screenCenter.x, screenCenter.y },
				LoadTexture(TEXTURE_ASCII),
				{}, 10, 10,
				"PUZZLE GAME", 0)
		);
		label->originH = HCENTER;
		label->originV = VCENTER;
		label->color = { 1.0f, 1.0f, 1.0f, 0.7f };


		AddGameObject(
			new GameObjectText(
				"PRESS ENTER",
				{ 64.0f, 64.0f },
				{ screenCenter.x, screenCenter.y + screenSize.y * 0.25f, 0.0f },
				CENTER,
				0, {}, { 0.0f, 0.0f, 0.0f, 0.5f }
			)
		);

		backgroundZoom = Progress(60000.0f, false);

	}


	// =======================================================
	// 終了処理
	// =======================================================
	void ColumnsTitleScene::Uninit()
	{
		Scene::Uninit();
		background = nullptr;
	}


	// =======================================================
	// 更新
	// =======================================================
	void ColumnsTitleScene::Update()
	{
		Scene::Update();

		if (INPUT_IS_DOWN_TRIGGER(INPUT_ENTER)) {
			SceneTransit("columns", "sample_transition");
		}

		// スクリーンの中心座標を取得
		F2 screenCenter = GetScreenCenter();

		// スクリーンのサイズを取得
		F2 screenSize = GetScreenSize();

		background->size.x = screenSize.x * (1.0f + 0.5f * backgroundZoom);
		background->size.y = screenSize.y * (1.0f + 0.5f * backgroundZoom);
		background->position.y = screenCenter.y + screenSize.y * (0.25f * backgroundZoom);

		backgroundZoom.IncreaseValue(GetDeltaTime());
	}
}
