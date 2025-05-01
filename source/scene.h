// =======================================================
// scene.h
// 
// Sceneクラス及びScene制御
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/07/06
// =======================================================
#ifndef _SCENE_H
#define _SCENE_H

#include "MGCommon.h"
#include "gameObject.h"
#include "input.h"


// =======================================================
// シーン登録用の定型文
// REGISTER_SCENE(シーン名, クラス名)
// =======================================================
#define REGISTER_SCENE(name, className) \
		MG::RegisterScene(name, []() -> MG::Scene* { MG::Scene* scene = (MG::Scene*)new className(); scene->sceneName = name; return scene; })


namespace MG {
	typedef std::string SceneName;

	enum SCENE_COMMAND_TYPE {
		START,
		END,
		LOAD,
		RELEASE,
		UPDATE,
		DRAW
	};

	struct SceneCommand {
		SceneName sceneName;
		SCENE_COMMAND_TYPE type;
	};

	class Scene {
		enum GAME_OBJECT_LIST_COMMAND_TYPE {
			ADD,
			INSERT,
			REMOVE,
			UPDATE,
			DRAW
		};
		struct GameObjectCommand {
			GameObject* gameObject;
			GAME_OBJECT_LIST_COMMAND_TYPE type;
			int index;
		};
	private:
		GameObject* lockedGameObject = nullptr;
		std::list<GameObjectCommand> gameObjectCommands;
		void DoGameObjectCommands();
		void DoGameObjectCommand(GameObjectCommand command);
	protected:
		std::list<GameObject*> gameObjects;
	public:
		SceneName sceneName;

		virtual void Init();

		// =======================================================
		// 終了処理
		// 
		// 稼働リストの中のゲームオブジェクトはすべて破棄
		// このシーンで取得したリソースもすべて解放
		// （他のスコープで使用中のリソースは除外）
		// =======================================================
		virtual void Uninit();

		// =======================================================
		// 更新
		// 
		// 稼働リストの中のゲームオブジェクトのUpdate関数を
		// 一つずつ呼び出す
		// =======================================================
		virtual void Update();


		// =======================================================
		// 描画
		// 
		// 稼働リストの中のゲームオブジェクトのDraw関数を
		// 一つずつ呼び出す
		// =======================================================
		virtual void Draw();

		// ゲームオブジェクトを稼働リストに追加
		GameObject* AddGameObject(GameObject* gameObject);

		// ゲームオブジェクトを稼働リストに挿入
		GameObject* InsertGameObject(GameObject* gameObject, int index);

		// ゲームオブジェクトを稼働リストから除外
		void DeleteGameObject(GameObject* gameObject);

		// テクスチャリソースを取得
		Texture* LoadTexture(const std::string& path);

		// テクスチャリソースを取得、スコープ指定
		Texture* LoadTexture(const std::string& path, const std::string& scope);

		// オーディオリソースを取得
		Audio* LoadAudio(const std::string& path);

		// オーディオリソースを取得、スコープ指定
		Audio* LoadAudio(const std::string& path, const std::string& scope);

		// シーン遷移開始
		void SceneTransit(SceneName dest);

		// シーン遷移開始
		void SceneTransit(SceneName dest, std::string transitionName);

		// シーン遷移開始
		void SceneTransit(SceneName dest, std::string transitionName, SceneName src);

	};


	// =======================================================
	// シーン制御関数
	// =======================================================
	
	void UninitScene();
	SceneName RegisterScene(std::string name, Scene* (*function)());
	void DoSceneCommands();
	void DoSceneCommand(SceneCommand command);
	Scene* LoadScene(const std::string& name);
	void ReleaseScene(const std::string& name);
	void StartScene(const std::string& name);
	void EndScene(const std::string& name);
	void UpdateScene();
	void DrawScene();
	void SceneTransit(SceneName dest, SceneName src);
	void SceneTransit(SceneName dest, SceneName src, std::string transitionName);
	bool InTransition();

} // namespace MG

#endif