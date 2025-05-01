#include "scene.h"
#include "asciiGameObject.h"
#include "commonVariable.h"
#include <sstream>
#include <iomanip>
using namespace MG;

namespace ColumnsResultScene {
	constexpr const char* TEXTURE_ASCII = "asset\\texture\\columns\\ascii.png";
	constexpr const char* TEXTURE_BG = "asset\\texture\\columns\\result.jpg";

	// =======================================================
	// クラス定義
	// =======================================================
	class ColumnsResultScene : public Scene {
	public:
		void Init() override;
		void Update() override;
		void FixedString(string& str, int value, int padding);
	};


	// =======================================================
	// シーン登録
	// =======================================================
	static SceneName sceneName = REGISTER_SCENE("columns_result", ColumnsResultScene);


	// =======================================================
	// 初期化
	// =======================================================
	void ColumnsResultScene::Init()
	{
		Scene::Init();

		// スクリーンの中心座標を取得
		F2 screenCenter = GetScreenCenter();

		// スクリーンのサイズを取得
		F2 screenSize = GetScreenSize();

		// ゲームオブジェクトを追加
		AddGameObject(
			new GameObjectQuad(
				{ screenSize.x, screenSize.y, 0.0f },		
				{ screenCenter.x, screenCenter.y, 0.0f },	
				LoadTexture(TEXTURE_BG),
				{},
				{ 0.5f, 0.5f, 0.5f, 1.0f }
			)
		);

		AsciiGameObject* label = (AsciiGameObject*)AddGameObject(
			new AsciiGameObject(
				{ 120.0f, 120.0f },
				{ screenCenter.x, screenCenter.y },
				LoadTexture(TEXTURE_ASCII),
				{}, 10, 10,
				"GAME OVER")
		);
		label->originH = HCENTER;
		label->originV = VCENTER;

		AddGameObject(
			new GameObjectText(
				"SCORE",
				{ 64.0f, 64.0f },
				{ screenCenter.x, screenCenter.y + screenSize.y * 0.25f - 64.0f, 0.0f },
				CENTER
			)
		);

		AddGameObject(
			new GameObjectText(
				"PRESS ENTER TO TITLE",
				{ 64.0f, 64.0f },
				{ screenCenter.x, screenCenter.y + screenSize.y * 0.25f + 100.0f, 0.0f },
				CENTER
			)
		);

		string scoreStr;
		FixedString(scoreStr, GetCommonInt("score"), 5);
		AsciiGameObject* scoreLabel = (AsciiGameObject*)AddGameObject(
			new AsciiGameObject(
				{ 64.0f, 64.0f },
				{ screenCenter.x, screenCenter.y + screenSize.y * 0.25f },
				LoadTexture(TEXTURE_ASCII),
				{}, 10, 10,
				scoreStr.data())
		);
		scoreLabel->originH = HCENTER;
		scoreLabel->originV = VCENTER;

		/*AsciiGameObject* enterLabel = (AsciiGameObject*)AddGameObject(
			new AsciiGameObject(
				{ 64.0f, 64.0f },
				{ screenCenter.x, screenCenter.y + screenSize.y * 0.25f + 64.0f },
				LoadTexture(TEXTURE_ASCII),
				{}, 10, 10,
				"PRESS ENTER TO TITLE")
		);
		enterLabel->originH = HCENTER;
		enterLabel->originV = VCENTER;*/
	}


	// =======================================================
	// 更新
	// =======================================================
	void ColumnsResultScene::Update()
	{
		Scene::Update();

		if (INPUT_IS_DOWN_TRIGGER(INPUT_ENTER)) {
			SceneTransit("columns_title", "sample_transition");
		}

	}

	void ColumnsResultScene::FixedString(string& str, int value, int padding)
	{
		std::ostringstream oss;
		oss << std::right << std::setfill('0') << std::setw(padding) << value;
		std::string stringNum = oss.str();

		str = stringNum.data();
	}
}
