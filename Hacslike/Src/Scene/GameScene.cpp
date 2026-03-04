#include "GameScene.h"
#include "../GameObject/Camera/Camera.h"
#include "../GameObject/Character/Enemy/Goblin/EnemyGoblin.h"
#include "../GameObject/Character/Player/Player.h"
#include "../Manager/StageManager.h"
#include"../Manager/ItemDropManager.h"
#include"../GameObject/Coin/Coin.h"
#include"../GameObject/TreasureChest/StartTreasureChest.h"
#include "../GameSystem/GameSystem.h"
#include "../GameObject/Enhancement/EnhancementStone.h"
#include "../GameObject/SaveObject/SaveObject.h"
#include "../UI/DamagePopup.h"
#include"../GameObject/Returner/TitleReturner.h"
#include "../GameObject/ItemShop/ItemShop.h"

GameScene::GameScene() {
	Start();
}

GameScene::~GameScene() {
	// オブジェクトの開放
	for (auto pGameObject : pGameObjectArray) {
		delete pGameObject;
		pGameObject = nullptr;
	}
}

void GameScene::Start() {

	Player* pPlayer = new Player();
	pGameObjectArray.push_back(pPlayer);
	// プレイヤーのUI状態フラグをリセット
	pPlayer->ResetUIStates();
	Camera* pCamera = new Camera(VGet(0, 400.0f, -800.0f));
	pGameObjectArray.push_back(pCamera);

	pCamera->SetTarget(pPlayer);

	//ItemShop* shop = new ItemShop();
	//shop->SetPosition(1000, 0, 400);
	//pGameObjectArray.push_back(shop);

	AudioManager::GetInstance().Load("Res/Audio/BGM/MainGame/NormalFloor.mp3", "NormalFloor", false);
	AudioManager::GetInstance().ChangeVolume(0.1f, "NormalFloor");
	EffectManager::GetInstance().Load("Res/Effect/Item.efkefc", "Item", 10.0f);
	EffectManager::GetInstance().Load("Res/Effect/Explosion.efkefc", "Explosion", 20.0f);

}

void GameScene::Update() {

	
	InputManager* input = &InputManager::GetInstance();

	StageManager::GetInstance().Update();
	EnemyManager::GetInstance().Update();
	CollisionManager::GetInstance().Update();

	for (auto pObj : pGameObjectArray) {
		pObj->Update();
	}

	EffectManager::GetInstance().Update();
	ItemDropManager::GetInstance().Update();

#if _DEBUG アイテムドロップテスト
	// --- アイテムドロップテスト ---
	if (input->IsKeyDown(KEY_INPUT_E) || input->IsButtonDown(XINPUT_GAMEPAD_LEFT_SHOULDER)) {
		VECTOR spawnPos = Player::GetInstance()->GetPosition();
		ItemDropManager::GetInstance().TryDropItem(10, VGet(spawnPos.x, 5.0f, spawnPos.z));
	}
	// --- アイテムドロップテスト ---
	if (input->IsKeyDown(KEY_INPUT_Q)) {
		VECTOR spawnPos = Player::GetInstance()->GetPosition();

		// コインを生成
		Coin::GetInstance()->SpawnCoin(VGet(spawnPos.x + 70, 5.0f, spawnPos.z + 70));

	}
#endif

	Coin::GetInstance()->UpdateAll();
	DamagePopup::UpdateAll();
	TitleReturner::GetInstance()->ShowFloorResetUI();

}


void GameScene::Render() {

	if (EnhancementStone::GetInstance()->GetIsOpenMenu() == true
		|| SaveObject::GetInstance()->GetIsOpenSaveMenu() == true
		|| ItemShop::GetInstance().GetState() != Invalid) {
		for (auto pObj : pGameObjectArray) {
			pObj->Render();
		}
		StageManager::GetInstance().Render();
		EnemyManager::GetInstance().Render();
	}

	else {
		StageManager::GetInstance().Render();
		EnemyManager::GetInstance().Render();
		for (auto pObj : pGameObjectArray) {
			pObj->Render();
		}

		if (GameSystem::GetInstance()->GetGameStatus() == GameStatus::Playing)
			StageManager::GetInstance().DrawMap();
	}

	Coin::GetInstance()->RenderAll();
	// ★スキル選択画面の描画
	if (isSelectingSkill) {
		skillUI.Render(skillChoices);
	}
	if (isSelectingArtifact) {
		artifactUI.Render(artifactChioces);
	}
	// エフェクトの描画
	EffectManager::GetInstance().Render();

	if(TitleReturner::GetInstance()->GetisShowResetUI() == true){
		TitleReturner::GetInstance()->ShowFloorResetUI();
	}


#if _DEBUG 線

	CollisionManager::GetInstance().Render();

	// オブジェクトの位置関係がわかるように地面にラインを描画する
	{
		VECTOR pos1, pos2;

		// XZ平面 100.0f毎に1本ライン引き
		{
			pos1 = VGet(-5000.0f, 0, -5000.0f);
			pos2 = VGet(-5000.0f, 0, 5000.0f);

			for (int i = 0; i < 100; i++) {
				DrawLine3D(pos1, pos2, gray);

				pos1.x += 100.0f;
				pos2.x += 100.0f;
			}

			pos1 = VGet(-5000.0f, 0, -5000.0f);
			pos2 = VGet(5000.0f, 0, -5000.0f);
			for (int i = 0; i < 100; i++) {
				DrawLine3D(pos1, pos2, gray);

				pos1.z += 100.0f;
				pos2.z += 100.0f;
			}
		}

		// X軸
		{
			pos1 = VZero;
			pos2 = VScale(VRight, 5000);	// VRight * 5000 をしてる
			DrawLine3D(pos1, pos2, red);
		}

		// Y軸
		{
			pos1 = VZero;
			pos2 = VScale(VUp, 5000);	    // VUp * 5000 をしてる
			DrawLine3D(pos1, pos2, green);
		}

		// Z軸
		{
			pos1 = VZero;
			pos2 = VScale(VForward, 5000);	// VRight * 5000 をしてる
			DrawLine3D(pos1, pos2, blue);
		}
	}
#endif

	DamagePopup::RenderAll();
}

void GameScene::Setup() {
	// ロードしてきたセーブがある場合は、StageManager 側が既に LoadFrom() を経て
		// フロア情報・ステージデータを復元しているはずなので、floorCount をリセットしたり
		// 再生成を行ってはいけない。
	if (SaveManager::GetInstance().HasLoadedSave()) {

		// フラグを消費しておく（次回の新規開始では通常の初期化を行うため）
		SaveManager::GetInstance().ClearLoadedFlag();
	}
	else {
		// ニューゲームまたは強くてニューゲーム
		// ※TitleSceneからフラグを渡す必要があるため、以下の方法を推奨：

		// 方法1: グローバルフラグまたはGameSystemに追加
		// 方法2: SceneManagerに情報を持たせる
		// 方法3: Playerクラスに「完全リセット要求フラグ」を追加

		// ここでは方法3を採用（Playerクラスに静的フラグを追加する想定）

		if (Player::GetInstance() && Player::GetInstance()->IsFullResetRequested()) {
			// 完全なるニューゲーム
			Player::GetInstance()->NewPlayerSetUp();
			StatusEnhancement::GetInstance()->StatusSetUp();
			Player::GetInstance()->ClearFullResetFlag();
		}
		else {
			// 強くてニューゲーム（ステータス引き継ぎ）
			Player::GetInstance()->PlayerSetUp();
		}

		StageManager::GetInstance().ResetFloorCount();
		StageManager::GetInstance().NoFadeGenerate();
	}
		
}

void GameScene::Teardown() {
	EnemyManager::GetInstance().UnuseAllEnemy();
	AudioManager::GetInstance().Stop("all");
}
