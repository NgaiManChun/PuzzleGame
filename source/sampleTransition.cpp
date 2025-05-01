// =======================================================
// sampleTransitaion.cpp
// 
// シーン遷移のサンプル
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/03
// =======================================================
#include "sceneTransitaion.h"
#include "progress.h"
#include "rendererDX.h"
#include "resourceToolDX.h"
using namespace MG;

namespace SampleTransition {
	// =======================================================
	// クラス定義
	// 
	// SceneTransitionクラスに継承
	// =======================================================
	class SampleTransition : public SceneTransition {
	private:
		Progress progress = Progress(1000.0f, false);
	public:
		void Update() override;
		void Draw() override;
	};

	// =======================================================
	// 遷移効果登録
	// 
	// REGISTER_TRANSITION( 遷移効果名, クラス名 )
	// 「static TransitionName name = 」の部分は必須
	// =======================================================
	static TransitionName name = REGISTER_TRANSITION("sample_transition", SampleTransition);


	// =======================================================
	// 更新
	// 
	// 各シーンのUpdateが呼び出される前にこっちの処理を行う
	// 遷移処理が終わったら、inTransitionをFALSEにしてください
	// このオブジェクトは自動的に消滅される
	// =======================================================
	void SampleTransition::Update()
	{
		inTransition = true;
		if (progress == 0.0f) {
			// 目的シーンを開始する
			StartScene(m_dest);
		}
		else if (progress == 1.0f) {
			// 遷移元のシーンを終了する
			EndScene(m_src);
			inTransition = false;
		}

		// 時間経過処理
		progress.IncreaseValue(GetDeltaTime());
	}


	// =======================================================
	// 描画
	// =======================================================
	void SampleTransition::Draw()
	{
		// コピーを取る、リスト変動防止
		std::list<Scene*> runningScenes(*m_runningScenes);

		// この時、稼働中のシーンは二つ（遷移先と遷移元）あるのでループ
		for (auto itr = runningScenes.begin(); itr != runningScenes.end(); itr++) {
			Scene* scene = *itr;

			// ここからはDirecX専用なので囲んでる
#ifdef _RENDERER_DX_H

		// スクリーンの中心座標を取得
			F2 screenCenter = GetScreenCenter();

			// スクリーンのサイズを取得
			F2 screenSize = GetScreenSize();

			RendererDX* renderer = (RendererDX*)GetRenderer();

			// 描画ターゲットを臨時テクスチャに設定する
			renderer->UseTempView();
			renderer->SetViewport(screenCenter.x, screenCenter.y, screenSize.x, screenSize.y);

			// 臨時テクスチャをクリア
			renderer->ClearTempView();

			// シーンの描画処理
			DoSceneCommand({ scene->sceneName, DRAW });

			// 臨時テクスチャを取得
			TextureDX tex = TextureDX(renderer->GetTempResourceView());

			// メイン描画ターゲットに切り替える
			renderer->UseMainView();
			renderer->SetViewport(screenCenter.x, screenCenter.y, screenSize.x, screenSize.y);

			// 臨時テクスチャを貼り付ける
			if (scene->sceneName == m_dest) {
				// 遷移先
				DrawQuad(
					&tex,
					{ screenCenter.x, screenCenter.y, 0 },
					{ screenSize.x, screenSize.y, 0 },
					{ 0.0f, 0.0f, 0.0f },
					{ 1.0f, 1.0f, 1.0f, progress }
				);
			}
			else if (scene->sceneName == m_src) {
				// 遷移元
				DrawQuad(
					&tex,
					{ screenCenter.x, screenCenter.y, 0 },
					{ screenSize.x, screenSize.y, 0 },
					{ 0.0f, 0.0f, 0.0f },
					{ 1.0f, 1.0f, 1.0f, 1.0f - progress }
				);
			}

#else
			scene->Draw();
#endif	

		}
	}

} // namespace SampleTransition