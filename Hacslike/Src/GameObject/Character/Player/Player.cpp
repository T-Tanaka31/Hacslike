#include "Player.h"
#include "../../../Definition.h"
#include "../../Camera/Camera.h"
#include "../../../Component/Collider/Collider.h"
#include "../../Item/ItemFactory.h"
#include "../../Item/ItemEquip/ItemEquip.h"
#include "../../TreasureChest/StartTreasureChest.h"
#include "../../../GameSystem/GameSystem.h"
#include "../../../Manager/FadeManager.h"
#include "../../../Manager/SkillManager.h"
#include "../../../Manager/CollisionManager.h"
#include "../../../Manager/ArtifactManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/TimeManager.h"
#include "../../../GameObject/Weapon/Weapon.h"
#include "../../../Save/SaveFormat.h"
#include"../../../Save/SaveIO.h"
#include"../../../Manager/SaveManager.h"
#include "../../../UI/GameEndUI.h"
#include "../../../Enhancement/StatusEnhancement.h"
#include <math.h>
#include <cmath>


// シングルトンインスタンスの初期化
Player* Player::instance = nullptr;
bool Player::fullResetRequested = false; // 追加

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="_pos"></param>
Player::Player(VECTOR _pos)
	: Character(_pos, "Player", Lv, exp, speed)
	, pWeapon(nullptr)
	, input(&InputManager::GetInstance())
	, currentWeaponId()
	, changeWeaponButtonPressed(false)
	, isItemUI(false)
	, coinValue(0)
	, expValue()
	, weaponData(nullptr)
	, hpRate()
	, maxExp()
	, remainExp()
	, deadTime() {

	// コンストラクタでシングルトンの重複生成を防ぐ
	if (instance != nullptr) {
#if _DEBUG
		printfDx("[Error] Player instance already exists!\n");
#endif
	}
	else {
		instance = this;
	}
	Start();

}

/// <summary>
/// デストラクタ
/// </summary>
Player::~Player() {
	delete pWeapon;
	pWeapon = nullptr;

	if (instance == this) {
		instance = nullptr;
	}

	MV1DeleteModel(PLAYER_MODEL_HANDLE);
	delete hpBar;
	delete expBar;
}

#pragma region シングルトン関連
Player* Player::CreateInstance(VECTOR _pos) {
	if (!instance) {
		//instance = new Player(_pos);
	}
	return instance;
}

Player* Player::GetInstance() {
	return instance;
}

void Player::DestroyInstance() {
	delete instance;
	instance = nullptr;
}
#pragma endregion

/// <summary>
/// 死亡処理
/// </summary>
void Player::DeadExecute() {
	if (hp > 0 || isDead) return;
	isDead = true;
	GameEndUI::GetInstance()->Start();
	pAnimator->Play("Down1", 0.9f);
	AudioManager::GetInstance().Stop("all");

	AudioManager::GetInstance().PlayOneShot("PlayerDeath");
}

#pragma region セーブとロード
void Player::SaveTo(BinaryWriter& w) {
	// 基本の数値
	w.WritePOD(Lv);
	w.WritePOD(exp);
	w.WritePOD(maxExp);
	w.WritePOD(hp);
	w.WritePOD(maxHp);
	//w.WritePOD(atk);
	w.WritePOD(def);
	w.WritePOD(criticalHitRate);
	w.WritePOD(criticalDamage);
	w.WritePOD(coinValue);
	w.WritePOD(isSelectArtifact);
	w.WritePOD(isDead);
	// position
	float px = GetPosition().x;
	float py = GetPosition().y;
	float pz = GetPosition().z;
	w.WritePOD(px);
	w.WritePOD(py);
	w.WritePOD(pz);

}

void Player::LoadFrom(BinaryReader& r, uint32_t saveVersion) {
	static int loadCallCount = 0;
	loadCallCount++;
	/*printfDx("[Player::LoadFrom] called times=%d\n", loadCallCount);*/
	r.ReadPOD(Lv);
	r.ReadPOD(exp);
	r.ReadPOD(maxExp);
	r.ReadPOD(hp);
	r.ReadPOD(maxHp);
	//r.ReadPOD(atk);
	r.ReadPOD(def);
	r.ReadPOD(criticalHitRate);
	r.ReadPOD(criticalDamage);
	r.ReadPOD(coinValue);
	r.ReadPOD(isSelectArtifact);
	r.ReadPOD(isDead);
	float px, py, pz;
	r.ReadPOD(px);
	r.ReadPOD(py);
	r.ReadPOD(pz);
	SetPosition(VECTOR{ px, py,pz });
	
	CollisionManager::GetInstance().CheckRegister(pCollider);


	
}
#pragma endregion

uint32_t Player::GetFloorForSave() const {
	return 0;
}


/// <summary>
/// 初期化処理
/// </summary>
void Player::Start() {
	//	非表示だったら初期化しない
	if (!isVisible)
		return;
	SetCollider(new CapsuleCollider(this, VZero, VScale(VUp, 200), 50.0f));
	CollisionManager::GetInstance().UnRegister(pCollider);

	modelHandle = PLAYER_MODEL_HANDLE;

	pAnimator->SetModelHandle(modelHandle);

	maxHp = 100;
	hp = maxHp;
	baseAttack = 5;
	def = 2;
	exp = 0;
	criticalHitRate = 10;
	criticalDamage = 100;

	maxExp = 100;

	deadTime = 0;

	SetFontSize(20);

	prevHP = GetHp();

	isMenuUI = false;

	// hpゲージの生成
	if (hpBar == nullptr)
		hpBar = new Gauge(hp, maxHp, hpBarPosX, hpBarPosY, hpBarWidth, hpBarHeight);
	if (expBar == nullptr) {
		expBar = new CircleGauge(exp, maxExp, expGaugePosX, expGaugePosY, expGaugeRadius, expGaugeThickness, expGaugeStartDeg, expGaugeEndDeg);
		expBar->ChangeColor(yellow, GetColor(200, 200, 150), black, GetColor(240, 240, 100));
	}

#if _DEBUG
	SetCoinValue(1000000000);
#endif

	//	アニメーションの読み込み
#pragma region Animation

	GetAnimator()->Load("Res/PlayerModel/Neutral.mv1", "Idle", true, true);
	GetAnimator()->Load("Res/PlayerModel/Walking.mv1", "Walk", true, true);
	GetAnimator()->Load("Res/PlayerModel/Attack1.mv1", "Atk1", true);
	GetAnimator()->Load("Res/PlayerModel/Attack2.mv1", "Atk2", true);
	GetAnimator()->Load("Res/PlayerModel/Attack3.mv1", "Atk3", true);
	GetAnimator()->Load("Res/PlayerModel/Run.mv1", "Run", true);
	GetAnimator()->Load("Res/PlayerModel/AxeAttack1.mv1", "AxeAtk1", true);
	GetAnimator()->Load("Res/PlayerModel/AxeAttack3.mv1", "AxeAtk2", true);
	GetAnimator()->Load("Res/PlayerModel/AxeAttack2.mv1", "AxeAtk3", true);
	GetAnimator()->Load("Res/PlayerModel/AxeAttack4.mv1", "AxeAtk4", true);
	GetAnimator()->Load("Res/PlayerModel/GreatAttack1.mv1", "GreatAtk1", true);
	GetAnimator()->Load("Res/PlayerModel/GreatAttack2.mv1", "GreatAtk2", true);
	GetAnimator()->Load("Res/PlayerModel/GreatAttack3.mv1", "GreatAtk3", true);
	GetAnimator()->Load("Res/PlayerModel/GreatAttack4.mv1", "GreatAtk4", true);
	GetAnimator()->Load("Res/PlayerModel/GreatCharge1.mv1", "GreatCharge1", true);
	GetAnimator()->Load("Res/PlayerModel/GreatCharge2.mv1", "GreatCharge2", true, true);
	GetAnimator()->Load("Res/PlayerModel/GreatCharge3.mv1", "GreatCharge3", true);
	GetAnimator()->Load("Res/PlayerModel/Down1.mv1", "Down1", false);
	GetAnimator()->Load("Res/PlayerModel/Down2.mv1", "Down2", true, true);
	GetAnimator()->Load("Res/PlayerModel/LanceDA.mv1", "LanceAtk1", true);

#pragma endregion

	pAnimator->Play(0);

	WeaponManager::GetInstance().LoadWeapons();
	maxWeaponId = WeaponManager::GetInstance().GetMaxWeaponId();
	changeWeaponButtonPressed = false;
	currentWeaponId = 10;
	weaponData = WeaponManager::GetInstance().GetWeapon(currentWeaponId);
	if (weaponData) {
		pWeapon = new Weapon(weaponData->name, weaponData->modelHandle);

		// std::array<float, N> 型に変換して渡す
		pWeapon->SetColLength(std::array<float, 3>{
			weaponData->colLength[0], weaponData->colLength[1], weaponData->colLength[2]
		});
		pWeapon->SetColRadius(std::array<float, 3>{
			weaponData->colRadius[0], weaponData->colRadius[1], weaponData->colRadius[2]
		});
		pWeapon->SetType((WeaponType)weaponData->type);
		pWeapon->SetAnimationSpeed(std::array<float, 4>{
			weaponData->attackSpeed[0], weaponData->attackSpeed[1],
				weaponData->attackSpeed[2], weaponData->attackSpeed[3]
		});

		pWeapon->attach(modelHandle, pWeapon->GetModelHandle(), "wp", this);
	}

	playerMovement = new PlayerMovement(this);
	playerAttack = new PlayerAttack(this, pWeapon, playerMovement);
	// 木の棒アイテムを生成（ItemFactory を使う場合は CreateItem でも可）
	std::unique_ptr<ItemBase> stick = std::make_unique<ItemStick>(
		VZero, "木の棒", "そこら辺に落ちてる木の棒\n世界で一つの木の棒", 0, 5, "Res/ItemIcon/stick.png"
	);

	// インベントリに追加
	GetInventory()->AddItem(std::move(stick));

	Inventory::InventoryItem* lastItem = GetInventory()->GetLastItem();

	// 装備（GetLastItem が nullptr でないことを確認してから）
	if (lastItem != nullptr && lastItem->item) {
		GetInventory()->EquipItem(lastItem->item.get());
		lastItem->item->Use();
		// 装備処理後に確実に攻撃力を再計算して反映する
		UpdateAtkFromEquipment();
	}

	AudioManager::GetInstance().Load("Res/SE/決定ボタンを押す2.mp3", "Select", false);
	AudioManager::GetInstance().Load("Res/SE/決定ボタンを押す38.mp3", "Decision", false);


	pAnimator->GetAnimation("Down1")->SetEvent([this]() {pAnimator->Play("Down2"); }, pAnimator->GetTotalTime("Down1"));
	pAnimator->GetAnimation("Down2")->SetEvent([this]() {
		SceneManager::GetInstance().ChangeScene(SceneType::Title);
		EnemyManager::GetInstance().DeleteAllEnemy();
		}, 0);
	pAnimator->GetAnimation("Down2")->SetEvent([this]() {CollisionManager::GetInstance().UnRegister(pCollider); }, 0);

	StringCenterPos("レベル", MainFont, &lX, &lY);

	SetSpeed(1);
}

/// <summary>
/// 更新処理
/// </summary>
void Player::Update() {
	//	非表示だったら更新しない
	if (!isVisible)
		return;

	if (isDead) {
		GameEndUI::GetInstance()->Update();
	}

	hpRate = (float)hp / (float)maxHp;

	if (Lv == MAX_LV) {
		exp = 0;
	}

	//ゲームシステムクラスで動きを制限
	if (GameSystem::GetInstance()->IsPlayable()) {
		playerMovement->Update();
		playerAttack->Update();
		pAnimator->Update();
		if (!isOpenMenu) {
			inventory.UseItemShortcutUpdate();
		}
	}

	MV1SetMatrix(modelHandle, matrix);

#if _DEBUG
	WeaponInput();
#endif

	//アイテムの取得
	AddItem();
	//アイテムのインベントリ表示非表示
	//OpenInventory();

	ArtifactManager::GetInstance().Update(this);


	GetBossArtifact();
	

	if (hitChest && !isSelectBossArtifact) {  // ボス報酬選択中は宝箱処理をスキップ
		GetArtifact();
	}

#pragma region スキル選択
	if (exp >= maxExp && !isSelectingSkill && !isSelectBossArtifact) {
		remainExp = exp - maxExp;
		exp = remainExp;
		RuneCost(Lv);
		LvUp(1);
		skillChoices = SkillManager::GetInstance().GenerateSkillChoices();
		skillUI.StartSelection();
		isSelectingSkill = true;
		GameSystem::GetInstance()->SetGameStatus(GameStatus::Stop);
	}
	if (isSelectingSkill && !isSelectBossArtifact) {
		// ★スキル選択中の処理
		SetMouseDispFlag(TRUE);
		int selected = skillUI.UpdateSelection(skillChoices);
		if (selected != -1) {

			if (GetInstance() && selected >= 0 && selected < (int)skillChoices.size()) {
				SkillManager::GetInstance().ApplySelectedSkill(this, skillChoices[selected]);
			}
			isSelectingSkill = false;
			GameSystem::GetInstance()->SetGameStatus(GameStatus::Playing);

		}
	}
#pragma endregion

	selectMenu();

	OpenMenu();

	inventory.Update(this);

	int currentHP = GetHp();

	// HPが変化したか？
	if (currentHP != prevHP) {
		GetInventory()->RefreshHealShortcut();
	}

	prevHP = currentHP;

	if (isArtifactUI) {
		artifactUI.Update();
	}

	if(isTitleUI){
		titleUI.Update();
	}


#if _DEBUG
	if (input->IsKeyDown(KEY_INPUT_3)) {
		AddExp(maxExp);
	}
	else if (input->IsKeyDown(KEY_INPUT_2)) {
		Damage(this, 10 + def);
	}
	if (input->IsButtonDown(XINPUT_GAMEPAD_Y) || input->IsKeyDown(KEY_INPUT_1)) {
		AddHp(maxHp / 10);
	}


#endif





	GameObject::Update();

	//	武器の更新
	if (pCollider != nullptr && pWeapon != nullptr) {
		pWeapon->Update();
	}

	if (pCollider != nullptr) {
		pCollider->Update();
	}

}


/// <summary>
/// 描画処理
/// </summary>
void Player::Render() {
	//	非表示だったら描画しない
	if (!isVisible)
		return;

	/*ArtifactManager::GetInstance().Render();*/

#pragma region プレイヤーのHPやEXP(没)
	//int cx = 0, cy = 800;   //	中心座標
	//int r_outer = 200;		//	外側半径
	//int r_inner = 160;		//	内側半径
	//int steps = 50;			//	分割数

	//double percent = (double)hp / maxHp * 100;		//	HP割合
	//double max_angle = 90.0;						//	四分円の角度
	//double angle_end = max_angle * percent / 100.0;	//	HP割合に応じて何度まで描画するか

	//for (int i = 0; i < steps; i++) {
	//	double angle1 = Deg2Rad(angle_end * i / steps);			//	ラジアン角への変換
	//	double angle2 = Deg2Rad(angle_end * (i + 1) / steps);	//	ラジアン角への変換

	//	// 外側頂点
	//	int x1o = cx + (int)(r_outer * cos(angle1));
	//	int y1o = cy - (int)(r_outer * sin(angle1));
	//	int x2o = cx + (int)(r_outer * cos(angle2));
	//	int y2o = cy - (int)(r_outer * sin(angle2));

	//	// 内側頂点
	//	int x1i = cx + (int)(r_inner * cos(angle1));
	//	int y1i = cy - (int)(r_inner * sin(angle1));
	//	int x2i = cx + (int)(r_inner * cos(angle2));
	//	int y2i = cy - (int)(r_inner * sin(angle2));

	//	// 三角形2つで四角形を描画
	//	DrawTriangle(x1o, y1o, x2o, y2o, x1i, y1i, GetColor(30, 200, 30), TRUE);
	//	DrawTriangle(x2o, y2o, x2i, y2i, x1i, y1i, GetColor(30, 200, 30), TRUE);
	//}

	//int expR_outer = 160;  //	外側半径
	//int expR_inner = 140;  //	内側半径
	//int expSteps = 50;	   //	分割数

	//double expPercent = (double)exp / maxExp * 100;
	//double Exp_max_angle = 90.0;
	//double Exp_angle_end = Exp_max_angle * expPercent / 100.0;

	//for (int i = 0; i < expSteps; i++) {
	//	double expAngle1 = Deg2Rad(Exp_angle_end * i / expSteps);		//	ラジアン角への変換
	//	double expAngle2 = Deg2Rad(Exp_angle_end * (i + 1) / expSteps);	//	ラジアン角への変換

	//	//	外側頂点
	//	int expX1o = cx + (int)(expR_outer * cos(expAngle1));
	//	int expY1o = cy - (int)(expR_outer * sin(expAngle1));
	//	int expX2o = cx + (int)(expR_outer * cos(expAngle2));
	//	int expY2o = cy - (int)(expR_outer * sin(expAngle2));

	//	//	内側頂点
	//	int expX1i = cx + (int)(expR_inner * cos(expAngle1));
	//	int expY1i = cy - (int)(expR_inner * sin(expAngle1));
	//	int expX2i = cx + (int)(expR_inner * cos(expAngle2));
	//	int expY2i = cy - (int)(expR_inner * sin(expAngle2));

	//	//	三角形2つで四角形を描画
	//	DrawTriangle(expX1o, expY1o, expX2o, expY2o, expX1i, expY1i, yellow, TRUE);
	//	DrawTriangle(expX2o, expY2o, expX2i, expY2i, expX1i, expY1i, yellow, TRUE);
	//}
	//DrawCircle(cx, cy, r_outer, white, FALSE);
	//DrawCircle(cx, cy, r_inner, white, FALSE);
	//DrawCircle(cx, cy, expR_inner, white, FALSE);

#pragma endregion		

	playerMovement->Render();

#if _DEBUG
	DrawFormatStringToHandle(200, 200, red, MainFont, "x : %d | z : %d", (int)position.x, (int)position.z);
#endif

#pragma region プレイヤーの描画
	// --- 通常モデル描画 ---
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	MV1SetMatrix(modelHandle, matrix);
	MV1DrawModel(modelHandle);
#pragma endregion

#pragma region 武器の描画
	//	武器の描画
	if (pWeapon != nullptr) {
		pWeapon->Render();
	}
#pragma endregion

	DrawBox(hpBarPosX - 3, hpBarPosY - 3, hpBarPosX + hpBarWidth + 3, hpBarPosY + hpBarHeight + 3, black, true);
	hpBar->Render();
	DrawCircle(expGaugePosX - 1, expGaugePosY - 1, expGaugeRadius + 3, black);
	expBar->Render();
	DrawStringToHandle(lX, lY, "レベル", white, MainFont);
	VECTOR uPos = StringCenterPos(std::to_string(Lv).c_str(), MainFont_Bold, uX, uY);
	DrawFormatStringToHandle(uPos.x, uPos.y, white, MainFont_Bold, "%d", Lv);
	std::string hpString = MergeString(std::to_string(hp), " / ", std::to_string(maxHp));
	int hpValueUIPos = StringRightPos(hpString.c_str(),MainFont, hpBarPosX + hpBarWidth + 3);
	DrawStringToHandle(hpValueUIPos, hpBarPosY - 3 - 20, hpString.c_str(), white,MainFont);
#pragma region アイテムのインベントリ表示
	inventory.ItemDropRender();
	if (isMenuUI) {
		DrawMenu();
	}
	if (isItemUI) {
		inventory.Render();
	}
	if (isArtifactUI) {
		artifactUI.Render();
	}
	if(isTitleUI){
		titleUI.Render();
	}

	inventory.AddItemRender();

	inventory.UseItemShortcutRender();

	if (isSelectArtifact) {
		artifactSelectUI.Render(artifactChioces);
	}

	artifactSelectUI.Render(bossArtifactChioces);



	if (isSelectingSkill) {
		if (!isSelectBossArtifact) {
			skillUI.Render(skillChoices);

		}
	}


	AddItemRender();
	if (hitChest) {
		GetArtifactRender();
	}

#pragma endregion

	if (isDead) {
		GameEndUI::GetInstance()->Draw();
	}

}

/// <summary>
/// 武器切り替え
/// </summary>
/// <param name="weaponId"></param>
void Player::ChangeWeapon(int weaponId) {
	weaponData = WeaponManager::GetInstance().GetWeapon(weaponId);
	if (!weaponData) return;

	pWeapon->ChangeModel(weaponData->modelHandle);
	pWeapon->SetType(static_cast<WeaponType>(weaponData->type));

	// 同様に std::array で渡す
	pWeapon->SetColLength(std::array<float, 3>{
		weaponData->colLength[0], weaponData->colLength[1], weaponData->colLength[2]
	});
	pWeapon->SetColRadius(std::array<float, 3>{
		weaponData->colRadius[0], weaponData->colRadius[1], weaponData->colRadius[2]
	});
	pWeapon->SetAnimationSpeed(std::array<float, 4>{
		weaponData->attackSpeed[0], weaponData->attackSpeed[1],
			weaponData->attackSpeed[2], weaponData->attackSpeed[3]
	});

	playerAttack->SetWeapon(pWeapon);
}

/// <summary>
/// 武器切り替え入力
/// </summary>
void Player::WeaponInput() {
	if ((input->IsKeyDown(KEY_INPUT_C) && !changeWeaponButtonPressed ||
		input->IsButtonDown(XINPUT_GAMEPAD_BACK) && !changeWeaponButtonPressed) && !playerAttack->IsAttacking()) {
		changeWeaponButtonPressed = true;

		currentWeaponId++;
		if (currentWeaponId > maxWeaponId)
			currentWeaponId = 1;

		ChangeWeapon(currentWeaponId);
	}
	else if (!input->IsKeyDown(KEY_INPUT_C)) {
		changeWeaponButtonPressed = false;
	}
}

/// <summary>
/// アイテムの取得
/// </summary>
void Player::AddItem() {
	if (!hitItem) return;

	//if (input->IsKeyDown(KEY_INPUT_F) || input->IsButtonDown(XINPUT_GAMEPAD_B)) {

		hitItem->SetVisible(false);

		// インベントリへ追加
		GetInventory()->AddItem(std::move(hitItem->TakeItem()));

		// DropManager から削除
		ItemDropManager::GetInstance().RemoveItem(hitItem);

		hitItem = nullptr; // 念のため解除
	//}
}

void Player::OpenMenu() {
	// TAB または START でメニュー開閉
	if (!isOpenMenu) {
		if (((input->IsKeyDown(KEY_INPUT_TAB)) || input->IsButtonDown(XINPUT_GAMEPAD_START)) && !isMenuUI) {
			isMenuUI = true;
			AudioManager::GetInstance().PlayOneShot("Decision");
			GameSystem::GetInstance()->SetGameStatus(GameStatus::Stop);
		}
		else if (((input->IsKeyDown(KEY_INPUT_TAB)) || input->IsButtonDown(XINPUT_GAMEPAD_START)) && isMenuUI) {
			isMenuUI = false;
			isItemUI = false;
			isArtifactUI = false;
			isSaveUI = false;
			isTitleUI = false;
			isMenuSelected = false; // 閉じたときにリセット
			titleUI.ResetConfirmation();
			AudioManager::GetInstance().PlayOneShot("Decision");
			GameSystem::GetInstance()->SetGameStatus(GameStatus::Playing);

		}
	}

	if (isMenuUI) {
		// メニュー操作
		if (!isMenuSelected) // 選択中はカーソル操作を無効に
		{
			// ↑キー
			if (input->IsKeyDown(KEY_INPUT_UP) || input->IsButtonDown(XINPUT_GAMEPAD_DPAD_UP)) {
				menuIndex--;
				if (menuIndex < 0)
					menuIndex = MENU_COUNT - 1;   // 上に行きすぎたらループ
				AudioManager::GetInstance().PlayOneShot("Select");
			}

			// ↓キー
			if (input->IsKeyDown(KEY_INPUT_DOWN) || input->IsButtonDown(XINPUT_GAMEPAD_DPAD_DOWN)) {
				menuIndex++;
				if (menuIndex >= MENU_COUNT)
					menuIndex = 0;                // 下に行きすぎたらループ
				AudioManager::GetInstance().PlayOneShot("Select");
			}

			// Enterで選択
			if (input->IsKeyUp(KEY_INPUT_RETURN) || input->IsButtonUp(XINPUT_GAMEPAD_B)) {
				isMenuSelected = true;
				blinkTime = 0.0f;
				blinkVisible = true;
				AudioManager::GetInstance().PlayOneShot("Decision");
			}
		}

		// Backspaceで解除
		if (input->IsKeyDown(KEY_INPUT_BACK) || input->IsButtonUp(XINPUT_GAMEPAD_A)) {
			isMenuSelected = false;
			blinkVisible = true; // 常に表示状態に戻す
			AudioManager::GetInstance().PlayOneShot("Decision");
		}
	}

	// 点滅制御
	if (isMenuSelected) {
		blinkTime += 1.0f / 60.0f; // 1フレーム1/60秒進む想定
		if (blinkTime >= 0.3f) // 約0.3秒ごとに切り替え
		{
			blinkVisible = !blinkVisible;
			blinkTime = 0.0f;
		}
	}

	else {
		blinkVisible = true;
	}
}

void Player::selectMenu() {
	if (isMenuUI == true) {
		switch (menuIndex) {
		case 0:
			isItemUI = true;
			isArtifactUI = false;
			isTitleUI = false;
			break;
		case 1:
			isItemUI = false;
			isArtifactUI = true;
			isTitleUI = false;
			break;
		case 2:
			isItemUI = false;
			isArtifactUI = false;
			isTitleUI = true;
			titleUI.Open();    // TitleUI の Open メソッドを呼び出し
			break;
		default:
			break;
		}
	}
	else return;
}

/// <summary>
/// メニューの描画
/// </summary>
void Player::DrawMenu() {
	if (!isMenuUI) return;

	const int x = 20;
	const int y = 50;
	const int width = 200;
	const int height = 40;
	const int margin = 10;

	// ← ここにメニューを好きなだけ並べるだけで増やせる！
	const char* menuNames[] = { "アイテム", "アーティファクト","ゲーム終了" };
	const int menuCount = sizeof(menuNames) / sizeof(menuNames[0]);

	for (int i = 0; i < menuCount; i++) {
		int boxY = y + i * (height + margin);

		// ★ 選択中かどうか判定（menuIndex を使う）
		bool isCurrent = (menuIndex == i);

		if (isCurrent && (!isMenuSelected || (isMenuSelected && blinkVisible))) {
			// 選択中：ピンク枠 + 中黒
			DrawBox(x - 4, boxY - 4, x + width + 4, boxY + height + 4, GetColor(255, 0, 255), TRUE);
			DrawBox(x, boxY, x + width, boxY + height, GetColor(0, 0, 0), TRUE);
			
		}
		else {
			// 未選択：白枠 + 中黒
			DrawBox(x - 2, boxY - 2, x + width + 2, boxY + height + 2, GetColor(255, 255, 255), FALSE);
			DrawBox(x, boxY, x + width, boxY + height, GetColor(0, 0, 0), TRUE);
		}

		// 文字
		DrawStringToHandle(x + 40, boxY + 10, menuNames[i], GetColor(255, 255, 255), MainFont);
	}

	PlayerStatusRender();
}

void Player::GetCoin() {
	if (coinArtifact)
		coinArtifact->OnGetCoin(this);
}

void Player::GetCoin_Item() {
	if (itemArtifact)
		itemArtifact->OnGetCoin_Item(this);
}

void Player::AddItemRender() {
	int StartX = (WINDOW_WIDTH / 2) - 200;
	int StartY = (WINDOW_HEIGHT)-200;
	int GoalX = (WINDOW_WIDTH / 2) + 200;
	int GoalY = (WINDOW_HEIGHT)-150;
	int textX = StartX + 80;
	int textY = StartY + 17;
	if (hitItem) {
		DrawBox(StartX, StartY, GoalX, GoalY, gray, TRUE);
		DrawBox(StartX + 2, StartY + 2, GoalX - 2, GoalY - 2, white, FALSE);
		DrawFormatStringToHandle(textX + 10, textY, black, MainFont, "キー/  ボタン:アイテムを取る");
		DrawFormatStringToHandle(textX, textY, white, MainFont, "F");
		DrawFormatStringToHandle(textX + 53, textY, white, MainFont, "B");


	}
	else return;
}

/// <summary>
/// アーティファクトの取得
/// </summary>
void Player::GetArtifact() {
	if (hitChest) {
		if (!isSelectArtifact && !isSelectBossArtifact) {  // ボス報酬選択中は開始しない
			if (input->IsKeyDown(KEY_INPUT_F) || input->IsButtonDown(XINPUT_GAMEPAD_B)) {
				SetMouseDispFlag(TRUE);
				artifactChioces = ArtifactManager::GetInstance().ApplyArtifact();

				// 選択肢がない場合は処理を中断
				if (artifactChioces.empty()) {
					isSelectArtifact = false;
					return;
				}

				artifactSelectUI.StartSelection();
				isSelectArtifact = true;
				GameSystem::GetInstance()->SetGameStatus(GameStatus::Stop);
			}
		}
		else if (isSelectArtifact && !isSelectBossArtifact) {  // ボス報酬選択中は更新しない
			int Selected = artifactSelectUI.UpdateSelection(artifactChioces);
			if (Selected != -1) {

				if (GetInstance() && Selected >= 0 && Selected < (int)artifactChioces.size()) {
					ArtifactManager::GetInstance().ApplySelectedArtifact(this, artifactChioces[Selected]);

				}

				GameSystem::GetInstance()->SetGameStatus(GameStatus::Playing);
			}
		}
	}
	else return;

}

void Player::GetBossArtifact() {
	if (ArtifactManager::GetInstance().GetBossDesiegen() == true) {
		bossArtifactChioces = ArtifactManager::GetInstance().GenerateArtifactChoices();

		// 選択肢がない場合は処理を中断
		if (bossArtifactChioces.empty()) {
			ArtifactManager::GetInstance().SetBossDesiegen(false);
			isSelectBossArtifact = false;
			return;
		}

		SetMouseDispFlag(TRUE);
		artifactSelectUI.StartSelection();
		ArtifactManager::GetInstance().SetBossDesiegen(false);
		isSelectBossArtifact = true;
		GameSystem::GetInstance()->SetGameStatus(GameStatus::Stop);
	}
	else if (isSelectBossArtifact) {  // elseではなくelse ifに変更
		int bossSelected = artifactSelectUI.UpdateSelection(bossArtifactChioces);
		if (bossSelected != -1) {

			if (GetInstance() && bossSelected >= 0 && bossSelected < (int)bossArtifactChioces.size()) {
				ArtifactManager::GetInstance().ApplySelectedArtifact(this, bossArtifactChioces[bossSelected]);
			}

			GameSystem::GetInstance()->SetGameStatus(GameStatus::Playing);
			isSelectBossArtifact = false;
		}
	}
}

void Player::GetArtifactRender() {
	int StartX = (WINDOW_WIDTH / 2) - 200;
	int StartY = (WINDOW_HEIGHT)-200;
	int GoalX = (WINDOW_WIDTH / 2) + 200;
	int GoalY = (WINDOW_HEIGHT)-150;
	int textX = StartX + 80;
	int textY = StartY + 17;
	if (!isSelectArtifact) {
		DrawBox(StartX, StartY, GoalX, GoalY, gray, TRUE);
		DrawBox(StartX + 2, StartY + 2, GoalX - 2, GoalY - 2, white, FALSE);
		DrawFormatStringToHandle(textX + 10, textY, black, MainFont, "キー/  ボタン:宝箱を開ける");
		DrawFormatStringToHandle(textX, textY, white, MainFont, "F");
		DrawFormatStringToHandle(textX + 53, textY, white, MainFont, "B");


	}
	else if (isSelectArtifact) {
		DrawBox(StartX, StartY, GoalX, GoalY, gray, TRUE);
		DrawBox(StartX + 2, StartY + 2, GoalX - 2, GoalY - 2, white, FALSE);
		DrawFormatStringToHandle(textX + 40, textY, black, MainFont, "中身は空っぽだ");

	}
	else return;
}

/// <summary>
/// プレイヤーステータスの描画
/// </summary>
void Player::PlayerStatusRender() {

	// --- 【1200x800基準のレスポンシブ計算】 ---
	// 右端から270pxの位置を基準にする (270 / 1200 = 0.225)
	int StatusX = WINDOW_WIDTH - (int)(WINDOW_WIDTH * 0.225f);

	// パネル全体の横幅 (280 / 1200 = 0.233)
	int panelLeft = WINDOW_WIDTH - (int)(WINDOW_WIDTH * 0.233f);
	int panelBottom = (int)(WINDOW_HEIGHT * 0.3f); // 240 / 800 = 0.3
	int titleHeight = (int)(WINDOW_HEIGHT * 0.05f); // 40 / 800 = 0.05

	// 文字の間隔 (20px刻み -> 800で2.5%)
	int lineGap = (int)(WINDOW_HEIGHT * 0.025f);
	int startTextY = (int)(WINDOW_HEIGHT * 0.075f); // 60 / 800 = 0.075

	// --- 描画処理 ---
	// 背景の黒枠
	DrawBox(panelLeft, 20, WINDOW_WIDTH, panelBottom, black, TRUE);
	// タイトル部分の白枠
	DrawBox(panelLeft, 20, WINDOW_WIDTH, titleHeight, white, FALSE);

	// 「ステータス」の文字（右端から一定の距離に配置）
	// 930pxは1200pxにおいて右から270pxなので、StatusXと同じ基準が使えます
	DrawFormatStringToHandle(StatusX, 20, white, MainFont, "ステータス");

	if (Lv == MAX_LV) {
		DrawFormatStringToHandle(StatusX, startTextY, white, MainFont, "レベル　　　 : MAX");
		DrawFormatStringToHandle(StatusX, startTextY + lineGap, white, MainFont, "経験値　　 　: MAX");
	}
	else {
		DrawFormatStringToHandle(StatusX, startTextY, white, MainFont, "レベル　　　 : %d", Lv);
		DrawFormatStringToHandle(StatusX, startTextY + lineGap, white, MainFont, "経験値　　 　: %d / %d", exp, maxExp);
	}

	// 各項目を lineGap (2.5%) ずつずらして描画
	DrawFormatStringToHandle(StatusX, startTextY + lineGap * 2, white, MainFont, "体力 　 　 　 : %d / %d", hp, maxHp);
	DrawFormatStringToHandle(StatusX, startTextY + lineGap * 3, white, MainFont, "攻撃力　　　 : %d", atk);
	DrawFormatStringToHandle(StatusX, startTextY + lineGap * 4, white, MainFont, "防御力　　　 : %d", def);
	DrawFormatStringToHandle(StatusX, startTextY + lineGap * 5, white, MainFont, "会心率　　　 : %.1f%%", criticalHitRate);
	DrawFormatStringToHandle(StatusX, startTextY + lineGap * 6, white, MainFont, "会心ダメージ : %.1f%%", criticalDamage);
	DrawFormatStringToHandle(StatusX, startTextY + lineGap * 7, white, MainFont, "コイン　　　 : %d枚", coinValue);
}

/// <summary>
/// プレイヤーのセットアップ
/// </summary>
void Player::PlayerSetUp() {
	// 全ての宝箱を元に戻す
	// インベントリのクリア
	GetInventory()->Clear();

	// スキルのクリア（Player ポインタを渡して個別解除）
	SkillManager::GetInstance().ClearSkills(this);

	// アーティファクトのクリア
	ArtifactManager::GetInstance().ClearArtifact(this);

	maxHp = 100;
	hp = maxHp;
	baseAttack = 5;
	def = 2;
	exp = 0;
	maxExp = 100;
	Lv = 1;
	criticalHitRate = 10;
	criticalDamage = 100;
	SetSpeed(1);
	CollisionManager::GetInstance().CheckRegister(pCollider);
	isDead = false;
	hitChest = false;
	isSelectArtifact = false;
	isTitleUI = false;
	isMenuUI = false;        // メニュー画面

	StatusEnhancement* se = StatusEnhancement::GetInstance();
	if (se) {
		se->ApplyAllStatsToPlayer(this);
	}

	// 木の棒アイテムを生成（ItemFactory を使う場合は CreateItem でも可）
	std::unique_ptr<ItemBase> stick = std::make_unique<ItemStick>(
		VZero, "木の棒", "そこら辺に落ちてる木の棒\n世界で一つの木の棒", 10, 5, "Res/ItemIcon/stick.png"
	);

	// インベントリに追加
	GetInventory()->AddItem(std::move(stick));
	Inventory::InventoryItem* lastItem = GetInventory()->GetLastItem();

	// 装備（GetLastItem が nullptr でないことを確認してから）
	if (lastItem != nullptr && lastItem->item) {
		GetInventory()->EquipItem(lastItem->item.get());
		lastItem->item->Use();
		// 装備処理後に確実に攻撃力を再計算して反映する
		UpdateAtkFromEquipment();
	}
}

void Player::NewPlayerSetUp() {
	// 全ての宝箱を元に戻す
	// インベントリのクリア
	GetInventory()->Clear();

	// スキルのクリア（Player ポインタを渡して個別解除）
	SkillManager::GetInstance().ClearSkills(this);

	// アーティファクトのクリア
	ArtifactManager::GetInstance().ClearArtifact(this);

	maxHp = 100;
	hp = maxHp;
	baseAttack = 5;
	def = 2;
	exp = 0;
	maxExp = 100;
	Lv = 1;
	criticalHitRate = 10;
	criticalDamage = 100;
	SetSpeed(1);
	CollisionManager::GetInstance().CheckRegister(pCollider);
	isDead = false;
	hitChest = false;
	isSelectArtifact = false;
	isTitleUI = false;
	isMenuUI = false;        // メニュー画面
	coinValue = 100;

	// 木の棒アイテムを生成（ItemFactory を使う場合は CreateItem でも可）
	std::unique_ptr<ItemBase> stick = std::make_unique<ItemStick>(
		VZero, "木の棒", "そこら辺に落ちてる木の棒\n世界で一つの木の棒", 10, 5, "Res/ItemIcon/stick.png"
	);

	// インベントリに追加
	GetInventory()->AddItem(std::move(stick));
	Inventory::InventoryItem* lastItem = GetInventory()->GetLastItem();

	// 装備（GetLastItem が nullptr でないことを確認してから）
	if (lastItem != nullptr && lastItem->item) {
		GetInventory()->EquipItem(lastItem->item.get());
		lastItem->item->Use();
		// 装備処理後に確実に攻撃力を再計算して反映する
		UpdateAtkFromEquipment();
	}
}

void Player::UpdateAtkFromEquipment() {
	// 基本的にはインベントリの装備中アイテムを参照して effectValue を取得し、
// baseAttack + (近接 or 遠距離補正) + effectValue を atk に適用する。
	if (!GetInventory()) {
		// フォールバック: 基本攻撃力のみ
		SetAtk(GetBaseAtk() + GetProximityCorrection());
		return;
	}

	std::string eqId = GetInventory()->GetEquippedItemID();
	if (eqId.empty()) {
		// 装備なしなら近接補正を使っておく（デフォルト）
		SetAtk(GetBaseAtk() + GetProximityCorrection());
		return;
	}

	Inventory::InventoryItem* pInvItem = GetInventory()->FindItemByID(eqId);
	if (!pInvItem || !pInvItem->item) {
		SetAtk(GetBaseAtk() + GetProximityCorrection());
		return;
	}

	int bonus = pInvItem->item->GetEffectValue();
	std::string name = pInvItem->item->GetName();
	std::string icon = pInvItem->item->GetItemIcon();

	// 武器が銃（遠距離）かどうかの簡易判定
	bool isRanged = false;
	if (name.find("銃") != std::string::npos || name.find("Gun") != std::string::npos || icon.find("Gun.png") != std::string::npos) {
		isRanged = true;
	}

	if (isRanged) {
		SetAtk(GetBaseAtk() + GetRangedCorrection() + bonus);
	}
	else {
		SetAtk(GetBaseAtk() + GetProximityCorrection() + bonus);
	}



}

void Player::ResetUIStates()
{
	isMenuUI = false;        // メニュー画面
	isItemUI = false;        // アイテムUI
	isArtifactUI = false;    // アーティファクトUI
	isTitleUI = false;       // タイトル確認画面
	isSaveUI = false;        // セーブUI
	isSelectingSkill = false;  // スキル選択
	blinkTime = 0.0f;        // 選択の点滅状態
	blinkVisible = true;
	isOpenMenu = false;
	titleUI.ResetConfirmation();

}

float Player::RuneCost(int L) {
	long k = std::max<long>(L - 6, 5);   // 下限 5 (L<=11 のとき a=0.1 に対応)
	long t = L + 81;
	return  maxExp = (float)(k * t * t) / 50 + 1;  // 整数除算で floor 相当
}

/// <summary>
/// 当たった瞬間
/// </summary>
/// <param name="_pCol"></param>
void Player::OnTriggerEnter(Collider* _pCol) {
	//	当たった相手のタグが "Goblin" だったら
	if (_pCol->GetGameObject()->GetTag() == "Goblin") {

	}
	if (_pCol->GetGameObject()->GetTag() == "Coin") {


		auto& coins = Coin::GetInstance()->GetCoin();

		for (auto& coin : coins) {
			// 衝突したコインオブジェクトと一致した場合のみ処理
			if (coin.get() == _pCol->GetGameObject()) {
				coin->ApplyCoin(this);
				GetCoin();  // ← プレイヤー経由で発動
				GetCoin_Item();
				coin.get()->SetActive(false);
				coin.get()->GetCollider()->SetEnable(false);
				break; // 削除したのでループを抜ける
			}
		}
	}
}

/// <summary>
/// 当たっている間
/// </summary>
/// <param name="_pCol"></param>
void Player::OnTriggerStay(Collider* _pCol) {
	if (_pCol->GetGameObject()->GetTag() == "item") {
		// 触れているアイテムを記録
		hitItem = dynamic_cast<ItemEntity*>(_pCol->GetGameObject());
	}
	if (_pCol->GetGameObject()->GetTag() == "TreasureChest") {
		hitChest = true;
		// 接触中の宝箱インスタンスを保持（nullptr チェックのため dynamic_cast）
		hitChestObj = dynamic_cast<StartTreasureChest*>(_pCol->GetGameObject());
	

	}
}

/// <summary>
/// 離れた瞬間
/// </summary>
/// <param name="_pCol"></param>
void Player::OnTriggerExit(Collider* _pCol) {
	if (_pCol->GetGameObject()->GetTag() == "item") {
		// 離れたアイテムなら解除
		if (hitItem == _pCol->GetGameObject()) {
			hitItem = nullptr;
		}
	}
	if (_pCol->GetGameObject()->GetTag() == "TreasureChest") {
		hitChest = false;
		// 接触終了したらポインタをクリア
		if (hitChestObj == _pCol->GetGameObject()) {
			hitChestObj = nullptr;

		}
	}
}
