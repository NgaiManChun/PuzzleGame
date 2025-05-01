#include "scene.h"
#include "progress.h"
#include "asciiGameObject.h"
#include "CSVResource.h"
#include "commonVariable.h"
#include <sstream>
#include <iomanip>
#include <vector>
using namespace MG;

namespace ColumnsScene {
	// =======================================================
	// 定数
	// =======================================================
	constexpr const int COL = 6;
	constexpr const int ROW = 13;
	
	constexpr const char* CONFIG_FILE = "asset\\columns.csv";
	constexpr const char* AUDIO_BGM = "asset\\audio\\columns\\bgm.wav";
	constexpr const char* AUDIO_DISAPPEAR = "asset\\audio\\columns\\disappear.wav";
	
	constexpr const char* TEXTURE_SPADE = "asset\\texture\\columns\\Spade.png";
	constexpr const char* TEXTURE_HEART = "asset\\texture\\columns\\Heart.png";
	constexpr const char* TEXTURE_CLOVER = "asset\\texture\\columns\\Clover.png";
	constexpr const char* TEXTURE_DIAMOND = "asset\\texture\\columns\\Diamond.png";
	constexpr const char* TEXTURE_BG = "asset\\texture\\columns\\BG.jpg";
	constexpr const char* TEXTURE_BG_2 = "asset\\texture\\columns\\BG2.jpg";
	constexpr const char* TEXTURE_BG_3 = "asset\\texture\\columns\\BG3.jpg";
	constexpr const char* TEXTURE_FRAME = "asset\\texture\\columns\\frame.png";
	constexpr const char* TEXTURE_EXPLOSION = "asset\\texture\\columns\\Explosion.png";
	constexpr const char* TEXTURE_WHITE = "asset\\texture\\columns\\white.png";
	constexpr const char* TEXTURE_ASCII = "asset\\texture\\columns\\ascii.png";
	
	constexpr const int TEXTURE_EXPLOSION_ROW = 4;
	constexpr const int TEXTURE_EXPLOSION_COL = 4;
	constexpr const int TEXTURE_EXPLOSION_FRAMES = TEXTURE_EXPLOSION_ROW * TEXTURE_EXPLOSION_COL;
	constexpr const int CURSOR_JEWEL_NUM = 3;

	static float CELL_WIDTH = 0;
	static float CELL_HEIGHT = CELL_WIDTH;
	static int CURSOR_NEW_COL = 3;
	static float NORMAL_SPEED = 700.0f;
	static float SPEED_MAGNIFICATION = 15.0f;
	static float GAMEOVER_HOLD_TIME = 1000.0f;
	static float DISAPPEAR_TIME = 700.0f;
	static float EXPLOSION_TIME = 200.0f;
	static float FALL_TIME = 200.0f;
	

	enum JEWEL_TYPE {
		SPADE,
		HEART,
		CLOVER,
		DIAMOND
	};
	constexpr int JEWEL_TYPE_NUM = 4;

	enum STATE {
		IDLE,
		CHECK_COMBO,
		DISAPPEARING,
		FALLING,
		GAMEOVER
	};

	static Texture* jewelTextures[JEWEL_TYPE_NUM];

	struct JewelCell {
		JEWEL_TYPE type;
		bool use = false;
		bool disappear = false;
	};

	class NextCursor : public GameObject {
	private:
		JEWEL_TYPE jewels[CURSOR_JEWEL_NUM];
	public:
		NextCursor(F3 position) :GameObject({}, position, {}) {
			Rand();
		}

		void Rand()
		{
			bool same = true;
			for (int i = 0; i < CURSOR_JEWEL_NUM; i++) {
				jewels[i] = (JEWEL_TYPE)(rand() % JEWEL_TYPE_NUM);
				if (jewels[i] != jewels[0]) {
					same = false;
				}
			}
			if (same) {
				int index = rand() % CURSOR_JEWEL_NUM;
				jewels[index] = (JEWEL_TYPE)(((int)jewels[index] + (rand() % (JEWEL_TYPE_NUM - 1)) + 1) % JEWEL_TYPE_NUM);
			}
		}

		JEWEL_TYPE GetJewelType(int index) {
			return jewels[index];
		}

		void Draw() override
		{
			F3 _size{ CELL_WIDTH, CELL_HEIGHT, 0.0f };
			for (int i = 0; i < CURSOR_JEWEL_NUM; i++) {
				F3 _position = {
					position.x + _size.x * 0.5f,
					position.y + _size.y * CURSOR_JEWEL_NUM - _size.y * 0.5f - _size.y * i,
					0.0f
				};
				DrawQuad(
					jewelTextures[jewels[i]],
					_position,
					_size
				);
			}
		}

	};


	// =======================================================
	// 操作してるジェム
	// =======================================================
	class Cursor : public GameObject {
	private:
		JEWEL_TYPE jewels[CURSOR_JEWEL_NUM];
		int col = CURSOR_NEW_COL;
		int row = -1;
		Progress rowOffset{ NORMAL_SPEED, false };
		float magnification = 1.0f;
		bool pause = false;
	public:
		int GetCol() { return col; }
		void SetCol(int col) { this->col = col; }
		int GetRow() { return row; }
		void SetAcceleration(float magnification) { this->magnification = magnification; }
		JEWEL_TYPE GetJewel(int index) { return jewels[index % CURSOR_JEWEL_NUM]; }

		Cursor(F3 size, F3 position, NextCursor* next) : GameObject(size, position, rotate)
		{
			for (int i = 0; i < CURSOR_JEWEL_NUM; i++) {
				jewels[i] = next->GetJewelType(i);
			}
		}
		
		void Swap()
		{
			JEWEL_TYPE last = jewels[CURSOR_JEWEL_NUM - 1];
			for (int i = CURSOR_JEWEL_NUM - 1; i > 0; i--) {
				jewels[i] = jewels[i - 1];
			}
			jewels[0] = last;
		}

		void SetPause(bool pause) {
			this->pause = pause;
		}

		void Update() override
		{
			if (pause) return;

			// rowOffsetが満タンになったら次の行
			rowOffset.IncreaseValue(GetDeltaTime() * magnification);
			if (rowOffset == 1.0f) {
				row++;
				rowOffset.SetValue(0.0f);
			}
		}

		void Draw() override
		{

			F3 _size{ CELL_WIDTH, CELL_HEIGHT, 0.0f };

			// 下から上
			for (int i = 0; i < CURSOR_JEWEL_NUM; i++) {
				F3 _position = {
					position.x - size.x * 0.5f + CELL_WIDTH * 0.5f + CELL_WIDTH * col,
					position.y - size.y * 0.5f + CELL_HEIGHT * 0.5f + CELL_HEIGHT * (row - i) +CELL_HEIGHT * rowOffset,
					0.0f
				};
				DrawQuad(
					jewelTextures[jewels[i]],
					_position,
					_size
				);
			}
		}
	};


	// =======================================================
	// 瓶
	// =======================================================
	class Jar : public GameObject {
	private:
		JewelCell cells[COL][ROW];
		STATE state = IDLE;
		Progress disappearing{ DISAPPEAR_TIME, false };
		Progress explosion{ EXPLOSION_TIME, false };
		Progress falling{ FALL_TIME, false };
		Texture* textureExplosion;
		GameObjectAudio* disappearAudio;
		bool pause = false;
		int score = 0;
		int plusScore = 0;
		int combo = 0;

		// =======================================================
		// コンボになってるかをチェック
		// 
		// 再帰的に使う、countが0になったtrueが戻る
		// =======================================================
		bool CheckCombo(JEWEL_TYPE type, int col, int row, int directionX, int directionY, int count)
		{
			// 無事すべて通過
			if (count == 0) return true;

			// 範囲外に出た
			if (col >= COL || row >= ROW || col < 0 || row < 0) return false;

			// 空のマス
			if (!cells[col][row].use) return false;

			// 違うタイプ
			if (cells[col][row].type != type) return false;

			// 次のマスへ、count - 1
			// count 0 まですべて通過しかtrueは戻ってこない
			bool result = CheckCombo(type, col + directionX, row + directionY, directionX, directionY, count - 1);
			if (result) {
				cells[col][row].disappear = true;
			}
			return result;
		}


		// =======================================================
		// すべてのジェムがしたに詰めてるかをチェック
		// =======================================================
		bool CheckFall() 
		{
			for (int col = 0; col < COL; col++) {
				bool hasEmpty = false;
				// 下から上へ
				for (int row = ROW - 1; row >= 0; row--) {
					if (cells[col][row].use) {

						// 空のマスが巡った後に空じゃないマスがある
						// ということは下に詰める余地があるのでtrue
						if (hasEmpty) return true;
					}
					else {
						// 空のマス
						hasEmpty = true;
					}
				}
			}
			return false;
		}

	public:
		Jar(F3 size, F3 position, F3 rotate) : GameObject(size, position, rotate)
		{
			textureExplosion = LoadTexture(TEXTURE_EXPLOSION, "columns");
			disappearAudio = new GameObjectAudio(LoadAudio(AUDIO_DISAPPEAR));
		}

		~Jar() {
			delete disappearAudio;
			disappearAudio = nullptr;
		}

		STATE GetState() { return state; }
		void SetState(STATE state){ this->state = state; }


		// =======================================================
		// そのマスの下は底なのかをチェック
		// =======================================================
		bool CHeckReach(int col, int row)
		{
			if (row + 1 < 0) return false;
			if (row == ROW - 1)return true;
			
			return cells[col][row + 1].use;
		}

		void SetJewel(int col, int row, JEWEL_TYPE type)
		{
			cells[col][row].type = type;
			cells[col][row].use = true;
			cells[col][row].disappear = false;
		}

		void SetPause(bool pause) {
			this->pause = pause;
		}

		int GetScore() {
			return score;
		}

		int GetPlusScore() {
			return plusScore;
		}

		void FillScore() {
			score += plusScore;
			plusScore -= plusScore;
		}

		void Draw() override 
		{

			F3 _size{ CELL_WIDTH, CELL_HEIGHT, 0.0f };
			F3 rotate{ 0.0f, 0.0f, 0.0f };
			F4 color{ 1.0f, 1.0f, 1.0f, 1.0f };

			for (int col = 0; col < COL; col++) {
				int emptyNum = 0;
				// 下から上へ
				for (int row = ROW - 1; row >= 0; row--) {
					if (cells[col][row].use) {

						// 空じゃないマス
						F3 _position{
							position.x - size.x * 0.5f + CELL_WIDTH * 0.5f + CELL_WIDTH * col,
							position.y - size.y * 0.5f + CELL_HEIGHT * 0.5f + CELL_HEIGHT * row,
							0.0f
						};

						if (cells[col][row].disappear) {

							// 空じゃないが、消える途中のジェム
							if (disappearing != 1.0f) {

								// 点滅中
								DrawQuad(
									jewelTextures[cells[col][row].type],
									_position, _size, rotate,
									{ 1.0f, 1.0f, 1.0f, (cosf(disappearing * 3.14f * 2.0f * 7.0f) + 1.0f) * 0.5f }
								);
							}
							else {

								// 爆発中
								int index = round(explosion * TEXTURE_EXPLOSION_FRAMES);
								F2 uvRange{
									1.0f / TEXTURE_EXPLOSION_COL,
									1.0f / TEXTURE_EXPLOSION_ROW
								};
								F2 uvOffset{
									(index % TEXTURE_EXPLOSION_COL) * uvRange.x,
									(index / TEXTURE_EXPLOSION_COL) * uvRange.y,
								};
								DrawQuad(
									textureExplosion,
									_position, _size, rotate, color,
									uvOffset,
									uvRange
								);
							}
						}
						else {
							if (state == FALLING) {

								// 消した後詰める途中の場合
								// 下から上へ数えて、ここまで空のマス分だけy座標を追加
								_position.y += CELL_HEIGHT * emptyNum * falling;
							}
							DrawQuad(
								jewelTextures[cells[col][row].type],
								_position,
								_size
							);
						}
					}
					else {
						emptyNum++;
					}
				}
			}
		}

		void Update() override 
		{
			if (pause) return;
			if (state == CHECK_COMBO) {
				int _combo = 0;
				for (int row = 0; row < ROW; row++) {
					for (int col = 0; col < COL; col++) {
						// →
						_combo += CheckCombo(cells[col][row].type, col, row, 1, 0, 3);

						// ↓
						_combo += CheckCombo(cells[col][row].type, col, row, 0, 1, 3);

						// 右下
						_combo += CheckCombo(cells[col][row].type, col, row, 1, 1, 3);

						// 左下
						_combo += CheckCombo(cells[col][row].type, col, row, -1, 1, 3);

						// どの道すべてのマス巡るので
						// 逆方向は不要
					}
				}
				if (_combo > 0) {
					if (combo == 0) {
						FillScore();
					}
					combo += _combo;
					plusScore = 0;
					for (int i = 1; i <= combo; i++) {
						plusScore += 100 * i;
					}
					disappearAudio->Play();
					state = DISAPPEARING;
					disappearing.SetValue(0.0f);
					explosion.SetValue(0.0f);
				}
				else {
					// コンボなかったらすぐ操作できる状態に戻る
					state = IDLE;
				}
			}
			else if (state == DISAPPEARING) {

				if (explosion == 1.0f) {

					// 爆発エフェクト完了した後
					for (int row = 0; row < ROW; row++) {
						for (int col = 0; col < COL; col++) {
							if (cells[col][row].disappear) {
								cells[col][row].use = false;
								cells[col][row].disappear = false;
							}
						}
					}

					// 下に詰められるかをチェック
					if (CheckFall()) {
						state = FALLING;
						falling.SetValue(0.0f);
					}
					else {
						state = IDLE;
					}
				}
				
				explosion.IncreaseValue(GetDeltaTime() * (disappearing == 1.0f));
				disappearing.IncreaseValue(GetDeltaTime());
			}
			else if (state == FALLING) {
				if (falling == 1.0f) {
					for (int col = 0; col < COL; col++) {
						int emptyNum = 0;
						for (int row = ROW - 1; row >= 0; row--) {
							if (cells[col][row].use) {
								if (emptyNum > 0) {
									cells[col][row + emptyNum] = cells[col][row];
									cells[col][row].use = false;
								}
							}
							else {
								emptyNum++;
							}
						}
					}

					// 詰めた後新しいコンボないかチェック
					state = CHECK_COMBO;
				}
				falling.IncreaseValue(GetDeltaTime());
			}
			else if (state == IDLE) {
				if (plusScore > 0) {
					if (plusScore > 3) {
						score += 3;
						plusScore -= 3;
					}
					else {
						FillScore();
					}
				}
				combo = 0;
			}
			
		}
	};

	class BG : public GameObjectQuad {
	private:
		Texture* nextTexture = nullptr;
		Progress progress{ 3000.0f, false };
		int row = 9;
		int col = 16;
	public:
		BG(F3 size, F3 position,
			Texture* texture) :GameObjectQuad(size, position, texture) {}
		~BG() {
			nextTexture = nullptr;
		}

		void SetNext(Texture* nextTexture) {
			if (this->nextTexture) {
				texture = this->nextTexture;
			}
			this->nextTexture = nextTexture;
			progress.SetValue(0);
		}

		void Update() override {
			if (nextTexture) {
				progress.IncreaseValue(GetDeltaTime());
			}
			if (nextTexture && progress >= 1.0f) {
				progress.SetValue(0);
				texture = nextTexture;
				nextTexture = nullptr;
			}
		}

		void Draw() override {
			GameObjectQuad::Draw();
			if (nextTexture) {
				F4 _color = color;
				
				
				float nodeWidth = size.x / col;
				float nodeHeight = size.y / row;
				F2 uvRange = { 1.0f / col, 1.0f / row };
				// スクリーンの中心座標を取得
				F2 screenCenter = GetScreenCenter();

				// スクリーンのサイズを取得
				F2 screenSize = GetScreenSize();

				float maxLength = sqrt(col * col + row * row) * 1.5f;

				int index = 0;
				for (int y = 0; y < row; y++) {
					for (int x = 0; x < col; x++) {

						_color.w = (maxLength * progress - sqrt(x * x + y * y)) * 0.2f;

						F3 _position = { 
							screenCenter.x - screenSize.x * 0.5f + nodeWidth * 0.5f + nodeWidth * x, 
							screenCenter.y - screenSize.y * 0.5f + nodeHeight * 0.5f + nodeHeight * y
						};
						F2 uvOffset = { uvRange.x * x, uvRange.y * y };
						DrawQuad(nextTexture, _position, { nodeWidth, nodeHeight }, rotate, _color, uvOffset, uvRange);
						index++;
					}
				}
			}
		}
	};

	// =======================================================
	// クラス定義
	// =======================================================
	class ColumnsScene : public Scene {
	private:
		Jar* jar;
		Cursor* cursor;
		NextCursor* next;
		BG* background;
		GameObjectQuad* pauseBG;
		AsciiGameObject* pauseLabel;
		AsciiGameObject* scoreLabel;
		AsciiGameObject* plusScoreLabel;
		Progress plusScoreShineProgress{ 1500.0f, true };
		Progress gameoverHold{ GAMEOVER_HOLD_TIME, false };

		std::vector<const char*> backgroundImages;
		int bgChangeScore = 1500;
		int backgroundIndex = 0;

		bool pause = false;
	public:
		void Init() override;
		void Uninit() override;
		void Update() override;
		void FixedString(string& str, int value, int padding);
		F4 hsv2rgb(float h, float s, float v);
	};


	// =======================================================
	// シーン登録
	// =======================================================
	static SceneName sceneName = REGISTER_SCENE("columns", ColumnsScene);


	// =======================================================
	// 初期化
	// =======================================================
	void ColumnsScene::Init()
	{
		Scene::Init();

		// CSVファイルから設定を読み込む
		D_TABLE table;
		D_KVTABLE keyValuePair;
		ReadCSVFromPath(CONFIG_FILE, table);

		// TableデータからKey-Value-Pairへ変換
		TableToKeyValuePair("key", table, keyValuePair);

		CURSOR_NEW_COL = std::stoi(keyValuePair["CURSOR_NEW_COL"]["value"]);
		NORMAL_SPEED = std::stof(keyValuePair["NORMAL_SPEED"]["value"]);
		SPEED_MAGNIFICATION = std::stof(keyValuePair["SPEED_MAGNIFICATION"]["value"]);
		GAMEOVER_HOLD_TIME = std::stof(keyValuePair["GAMEOVER_HOLD_TIME"]["value"]);
		DISAPPEAR_TIME = std::stof(keyValuePair["DISAPPEAR_TIME"]["value"]);
		EXPLOSION_TIME = std::stof(keyValuePair["EXPLOSION_TIME"]["value"]);
		FALL_TIME = std::stof(keyValuePair["FALL_TIME"]["value"]);

		backgroundImages.push_back(TEXTURE_BG);
		backgroundImages.push_back(TEXTURE_BG_2);
		backgroundImages.push_back(TEXTURE_BG_3);

		bgChangeScore = 1500;
		backgroundIndex = 0;

		srand(timeGetTime());

		CELL_WIDTH = 50.0f / 1280.0f * GetScreenWidth();
		CELL_HEIGHT = CELL_WIDTH;

		// テクスチャを取得しておく
		jewelTextures[SPADE] = LoadTexture(TEXTURE_SPADE);
		jewelTextures[HEART] = LoadTexture(TEXTURE_HEART);
		jewelTextures[CLOVER] = LoadTexture(TEXTURE_CLOVER);
		jewelTextures[DIAMOND] = LoadTexture(TEXTURE_DIAMOND);

		// スクリーンの中心座標を取得
		F2 screenCenter = GetScreenCenter();

		// スクリーンのサイズを取得
		F2 screenSize = GetScreenSize();

		// 背景
		background = (BG*)AddGameObject(
			new BG(
				{ screenSize.x, screenSize.y, 0.0f },
				{ screenCenter.x, screenCenter.y, 0.0f },
				LoadTexture(backgroundImages[backgroundIndex])
			)
		);

		// 枠
		AddGameObject(
			new GameObjectQuad(
				{ screenSize.x, screenSize.y, 0.0f },
				{ screenCenter.x, screenCenter.y, 0.0f },
				LoadTexture(TEXTURE_FRAME)
			)
		);

		// 瓶
		jar = (Jar*)AddGameObject(
			new Jar(
				{ CELL_WIDTH * COL, CELL_HEIGHT * ROW, 0.0f }, 
				{ screenCenter.x, screenCenter.y, 0.0f },
				{ 0.0f, 0.0f, 0.0f }
			)
		);

		AddGameObject(
			new GameObjectText(
				"NEXT",
				{ 64.0f, 64.0f },
				{ screenCenter.x + CELL_WIDTH * COL * 0.5f + 100.0f, screenCenter.y - CELL_HEIGHT * ROW * 0.5f, 0.0f },
				LEFT_TOP
			)
		);
		string scoreStr;
		FixedString(scoreStr, jar->GetScore(), 5);
		scoreLabel = (AsciiGameObject*)AddGameObject(
			new AsciiGameObject(
				{64.0f, 64.0f}, 
				{ screenCenter.x - CELL_WIDTH * COL * 0.5f - 100.0f, screenCenter.y - CELL_HEIGHT * ROW * 0.5f }, 
				LoadTexture(TEXTURE_ASCII), 
				{}, 10, 10, 
				scoreStr.data())
		);
		scoreLabel->originH = RIGHT;
		scoreLabel->originV = TOP;

		plusScoreLabel = (AsciiGameObject*)AddGameObject(
			new AsciiGameObject(
				{ 32.0f, 32.0f },
				{ screenCenter.x - CELL_WIDTH * COL * 0.5f - 100.0f, screenCenter.y - CELL_HEIGHT * ROW * 0.5f + 64.0f },
				LoadTexture(TEXTURE_ASCII),
				{}, 10, 10,
				"0")
		);
		plusScoreLabel->originH = RIGHT;
		plusScoreLabel->originV = TOP;

		next = (NextCursor*)AddGameObject(
			new NextCursor(
				{ screenCenter.x + CELL_WIDTH * COL * 0.5f + 100.0f, screenCenter.y - CELL_HEIGHT * ROW * 0.5f + 64.0f, 0.0f }
			)
		);

		pauseBG = (GameObjectQuad*)AddGameObject(
			new GameObjectQuad(
				{ screenSize.x, screenSize.y, 0.0f },
				{ screenCenter.x, screenCenter.y, 0.0f },
				LoadTexture(TEXTURE_WHITE),
				{},
				{ 0.0f, 0.0f, 0.0f, 0.5f }
			)
		);

		pauseLabel = (AsciiGameObject*)AddGameObject(
			new AsciiGameObject(
				{ 200.0f, 200.0f },
				{ screenCenter.x, screenCenter.y },
				LoadTexture(TEXTURE_ASCII),
				{}, 10, 10,
				"PAUSE")
		);
		pauseLabel->originH = HCENTER;
		pauseLabel->originV = VCENTER;
		pauseBG->enable = false;
		pauseLabel->enable = false;

		// BGM
		GameObjectAudio* audio = (GameObjectAudio*)AddGameObject(
			new GameObjectAudio(
				LoadAudio(AUDIO_BGM),	// 音源
				true					// ループ設定
			)
		);
		audio->Play();
		audio->SetVolume(0.05f);

		gameoverHold = Progress( GAMEOVER_HOLD_TIME, false );
		pause = false;
		
	}


	// =======================================================
	// 終了処理
	// =======================================================
	void ColumnsScene::Uninit()
	{
		Scene::Uninit();
		backgroundImages.clear();
		background = nullptr;
		jar = nullptr;
		cursor = nullptr;
		next = nullptr;
		pauseBG = nullptr;
		pauseLabel = nullptr;
		scoreLabel = nullptr;
		plusScoreLabel = nullptr;
	}


	// =======================================================
	// 更新
	// =======================================================
	void ColumnsScene::Update()
	{
		Scene::Update();

		if (INPUT_IS_DOWN_TRIGGER(INPUT_SPACE) && !InTransition()) {
			pause = !pause;
			jar->SetPause(pause);
			if (cursor) {
				cursor->SetPause(pause);
			}
			pauseBG->enable = pause;
			pauseLabel->enable = pause;
			jar->enable = !pause;
			next->enable = !pause;
			if (cursor) {
				cursor->enable = !pause;
			}
		}

		if (pause) return;

		// スコア更新
		string scoreStr;
		FixedString(scoreStr, jar->GetScore(), 5);
		scoreLabel->value = scoreStr.data();

		if (jar->GetPlusScore() > 0) {
			scoreStr = "+" + std::to_string(jar->GetPlusScore());
			plusScoreLabel->value = scoreStr;
			plusScoreLabel->color = hsv2rgb(plusScoreShineProgress, 0.5f, 1.0f);
			plusScoreLabel->enable = true;
			plusScoreShineProgress.IncreaseValue(GetDeltaTime());
		}
		else {
			plusScoreLabel->enable = false;
		}

		if (jar->GetState() == IDLE && !InTransition()) {
			// 操作できる状態

			// 操作できるジェムがなかったら生成
			if (!cursor) {
				cursor = (Cursor*)InsertGameObject(
					new Cursor(
						jar->size,
						jar->position,
						next
					), 2
				);
				next->Rand();
			}

			// 背景チェンジ
			bool change = false;
			while (backgroundIndex < backgroundImages.size() - 1 && jar->GetScore() + jar->GetPlusScore() >= (backgroundIndex + 1) * bgChangeScore) {
				backgroundIndex++;
				change = true;
			}
			if (change) {
				background->SetNext(LoadTexture(backgroundImages[backgroundIndex]));
			}

			// 上ボタン
			if (INPUT_IS_DOWN_TRIGGER(INPUT_UP)) {
				cursor->Swap();
			}

			// 左ボタン
			if (INPUT_IS_DOWN_TRIGGER(INPUT_LEFT)) {
				int col = cursor->GetCol();
				if ((col - 1 >= 0) && !jar->CHeckReach(col - 1, cursor->GetRow())) {
					cursor->SetCol(col - 1);
				}
			}

			// 右ボタン
			if (INPUT_IS_DOWN_TRIGGER(INPUT_RIGHT)) {
				int col = cursor->GetCol();
				if ((col + 1 < COL) && !jar->CHeckReach(col + 1, cursor->GetRow())) {
					cursor->SetCol(col + 1);
				}
			}

			// 下ボタン
			cursor->SetAcceleration((INPUT_IS_DOWN(INPUT_DOWN))? SPEED_MAGNIFICATION :1.0f + ((float)(jar->GetScore() + jar->GetPlusScore()) / 99900.0f) * SPEED_MAGNIFICATION);

			// 底に触れたら
			if (jar->CHeckReach(cursor->GetCol(), cursor->GetRow())) {
				int row = cursor->GetRow();
				if (row - 2 < 0) {
					cursor->SetAcceleration(0.0f);
					jar->SetState(GAMEOVER);
					jar->FillScore();
					SetCommonInt("score", jar->GetScore());
				}
				else {
					for (int i = 0; i < CURSOR_JEWEL_NUM; i++) {
						jar->SetJewel(cursor->GetCol(), row - i, cursor->GetJewel(i));
					}
					jar->SetState(CHECK_COMBO);

					DeleteGameObject(cursor);
					cursor = nullptr;
				}
				
				
			}
		}
		else if (jar->GetState() == GAMEOVER) {
			if (gameoverHold == 1.0f) {
				SceneTransit("columns_result", "sample_transition");
			}
			gameoverHold.IncreaseValue(GetDeltaTime());
		}
	}

	void ColumnsScene::FixedString(string& str, int value, int padding)
	{
		std::ostringstream oss;
		oss << std::right << std::setfill('0') << std::setw(padding) << value;
		std::string stringNum = oss.str();

		str = stringNum.data();
	}

	F4 ColumnsScene::hsv2rgb(float h, float s, float v) {
		float r = v;
		float g = v;
		float b = v;
		if (s > 0.0f) {
			h *= 6.0f;
			int i = (int)h;
			float f = h - (float)i;
			switch (i) {
			default:
			case 0:
				g *= 1 - s * (1 - f);
				b *= 1 - s;
				break;
			case 1:
				r *= 1 - s * f;
				b *= 1 - s;
				break;
			case 2:
				r *= 1 - s;
				b *= 1 - s * (1 - f);
				break;
			case 3:
				r *= 1 - s;
				g *= 1 - s * f;
				break;
			case 4:
				r *= 1 - s * (1 - f);
				g *= 1 - s;
				break;
			case 5:
				g *= 1 - s;
				b *= 1 - s * f;
				break;
			}
		}
		return { r, g, b, 1.0f };
	}
}
