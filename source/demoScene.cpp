// =======================================================
// demoScene.cpp
// 
// デモシーン
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/03
// =======================================================
#include "scene.h"
#include "progress.h"
#include "CSVResource.h"
using namespace MG;

namespace DemoScene {
	// =======================================================
	// 定数
	// =======================================================
	constexpr const char* TEXTURE_BRICK = "asset\\texture\\Brick.jpg";
	constexpr const char* AUDIO_BGM = "asset\\audio\\bgm.wav";
	constexpr const char* CONFIG_FILE = "asset\\demoScene.csv";

	// =======================================================
	// クラス定義
	// =======================================================
	class DemoScene : public Scene {
	private:
		GameObjectQuad* object1;
		Progress object1Progress;

		F3 objectSize0;
		F3 objectSize1;
		float object1PeriodTime; // 周期時間
	public:
		void Init() override;
		void Uninit() override;
		void Update() override;
	};


	// =======================================================
	// シーン登録
	// =======================================================
	static SceneName sceneName = REGISTER_SCENE("demo", DemoScene);


	// =======================================================
	// 初期化
	// =======================================================
	void DemoScene::Init()
	{
		Scene::Init();

		// CSVファイルから設定を読み込む
		D_TABLE table;
		D_KVTABLE keyValuePair;
		ReadCSVFromPath(CONFIG_FILE, table);

		// TableデータからKey-Value-Pairへ変換
		TableToKeyValuePair("key", table, keyValuePair);

		// 設定を適応
		objectSize0.x = std::stof(keyValuePair["object_0_width"]["value"]);
		objectSize0.y = std::stof(keyValuePair["object_0_height"]["value"]);
		objectSize1.x = std::stof(keyValuePair["object_1_width"]["value"]);
		objectSize1.y = std::stof(keyValuePair["object_1_height"]["value"]);
		object1PeriodTime = std::stof(keyValuePair["object_1_period_time"]["value"]);
		object1Progress = Progress(object1PeriodTime, true);

		// スクリーンの中心座標を取得
		F2 screenCenter = GetScreenCenter();

		// スクリーンのサイズを取得
		F2 screenSize = GetScreenSize();

		// ゲームオブジェクトを追加
		AddGameObject(
			new GameObjectQuad(
				objectSize0,								// サイズ
				{ screenCenter.x, screenCenter.y, 0.0f },	// 中心座標
				LoadTexture(TEXTURE_BRICK)					// テクスチャ
			)
		);

		// 文字オブジェクトを追加
		AddGameObject(
			new GameObjectText(
				"Homeボタンで目次に戻る",			// 表示する文字
				{ 32.0f, 32.0f, 0.0f },				// １文字のサイズ、半角は幅半分
				{
					screenCenter.x - screenSize.x * 0.5f + 20.0f,
					screenCenter.y - screenSize.y * 0.5f + 20.0f,
					0.0f
				},
				LEFT_TOP,	// 始点
				0.0f,
				{},
				{ 1.0f, 0.0f, 0.0f, 1.0f } // 色
			)
		);

		// オーディオを追加
		// 描画するわけじゃないが、
		// プレイヤーとしてのゲームオブジェクト
		GameObjectAudio* audio = (GameObjectAudio*)AddGameObject(
			new GameObjectAudio(
				LoadAudio(AUDIO_BGM),	// 音源
				true					// ループ設定
			)
		);
		audio->Play();
	}


	// =======================================================
	// 終了処理
	// 
	// GameObjectリストに入ってるGameObjectは
	// Scene::Uninit()でdeleteされるので、
	// ここでdeleteする必要はない
	// 
	// ただし、仕様上終了したシーンのインスタンスは
	// 自動的に破棄しないので、
	// ポインタはnullptrにしておいた方がいい
	// 
	// 強制的にシーンのインスタンスを破棄したい場合は
	// ReleaseScene(シーン名)を使ってください
	// =======================================================
	void DemoScene::Uninit()
	{
		Scene::Uninit();
		object1 = nullptr;
	}

	// =======================================================
	// 更新
	// =======================================================
	void DemoScene::Update()
	{
		// GameObjectリストを巡って
		// GameObjectのUpdate関数一個ずつを呼び出す
		Scene::Update();

		// HOMEボタンを押したら目次に戻る
		if (INPUT_IS_DOWN_TRIGGER(INPUT_HOME)) {

			// サンプルの遷移効果
			SceneTransit("index", "sample_transition");

			// デフォルトの遷移効果、パッと切り替えるだけ
			// SceneTransit("index");

			// シーン遷移の最中なのかどうかを調べたい場合は
			// InTransition()を使ってください
		}

		// スクリーンの中心座標を取得
		F2 screenCenter = GetScreenCenter();

		// スクリーンのサイズを取得
		F2 screenSize = GetScreenSize();

		if (!object1) {
			object1 = (GameObjectQuad*)AddGameObject(
				new GameObjectQuad(
					objectSize1,
					{ screenCenter.x - screenSize.x * 0.5f, screenCenter.y, 0.0f },
					LoadTexture(TEXTURE_BRICK)
				)
			);
		}

		object1->position.x += 5.0f;
		object1->rotate.z = 360.0f * object1Progress;

		// object1のX座標が画面外に行ったら削除する
		if (object1->position.x > screenCenter.x + screenSize.x * 0.5f) {
			DeleteGameObject(object1);
			object1 = nullptr;
		}

		// 時間経過処理
		object1Progress.IncreaseValue(GetDeltaTime());
	}

} // namespace DemoScene
