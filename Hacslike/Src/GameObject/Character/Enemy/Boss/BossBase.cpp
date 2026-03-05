#include "BossBase.h"
#include "../../../../CommonModule.h"
#include"../../../../Manager/ArtifactManager.h"
#include "../../../../GameSystem/GameSystem.h"
#include"../../Player/Player.h"
#include "../../../../UI/BossSlainUI.h"
#include "../../../Returner/TitleReturner.h"

BossBase::BossBase(VECTOR _appearPos)
	:appearPos(_appearPos)
	, attackSpanBar(NULL)
	, circlePos(VZero){
}

BossBase::~BossBase() {
	delete hpBar;
	delete attackSpanBar;
}



void BossBase::Start() {
	isBoss = true;
	SetModelHandleDup(MV1LoadModel(MergeString(mPath, "boss.mv1").c_str()));
	Enemy::Start();
	SetScale(VGet(0.2f, 0.2f, 0.2f));
	// アニメーションの設定
	pAnimator->SetModelHandle(modelHandle);
	// アニメーションのロード
	LoadAnimation();
	hpBar = new Gauge(hp,maxHp, WINDOW_WIDTH / 4, 800.0f, WINDOW_WIDTH / 2, 15.0f);
	attackSpanBar = new Gauge(atkTime, atkSpan, WINDOW_WIDTH / 4, 815.0f, WINDOW_WIDTH / 2, 5.0f,false);
	attackSpanBar->ChangeColor(cyan, blue, black, blue);
	StageData data = StageManager::GetInstance().generator->GetStageData();
	SetAppearPos(data.stairSpawnPos);
	SetReturnerPos(data.returnerSpawnPos);
}

void BossBase::Update() {
	if (!isVisible || !GameSystem::GetInstance()->IsPlayable()) return;
	// 座標の更新等
	GameObject::Update();
	MV1SetMatrix(modelHandle, matrix);

	if (isDead) BossSlainUI::GetInstance()->Update();

	// アニメーションの更新
	if (pAnimator != nullptr)
		pAnimator->Update();

	if (IsDead())
		return;

	if (atking) {
		area.Update();
		// 攻撃当たり判定の更新
		for (auto c : attackColliderList) {
			if (c->GetCollider() == nullptr) continue;

			c->Update();
		}
	}


	// 当たり判定の更新
	if (pCollider != nullptr) {
		pCollider->SetMatrix(matrix);
		pCollider->Update();
	}

	// 攻撃当たり判定の削除
	for (auto itr = attackColliderList.begin(); itr != attackColliderList.end(); ) {
		SphereHitBox* c = *itr;
		if (c->GetActive()) {
			++itr;
			continue;
		}

		CollisionManager::GetInstance().UnRegister(c->GetCollider());
		delete c;
		itr = attackColliderList.erase(itr); // eraseの戻り値を使って次の要素へ
	}

	if (isAttack()) return;

	// レイの更新
	WallDetectionVision_Fan(Player::GetInstance()->GetPosition());
	// 追跡行動
	Tracking();


	if (rayAnswer && !isTouch)
		pAnimator->Play("run");
	if (rayAnswer && isTouch)
		Attack();


	// 攻撃のリキャスト
	if (atkTime >= atkSpan)
		atkTime = atkSpan;
	else
		atkTime += TimeManager::GetInstance().deltaTime;
	

	
}

void BossBase::Render() {
	Enemy::Render();
	
	if (!isDead && rayAnswer) {
		hpBar->Render();
		attackSpanBar->Render();
		DrawStringToHandle(WINDOW_WIDTH / 4, 800 - 20, name.c_str(), white,MainFont);
		
	}
	if(isDead) BossSlainUI::GetInstance()->Draw();
}

void BossBase::DeadExecute() {
	if (hp > 0) return;

	if (!isDead) {
		ArtifactManager::GetInstance().SetBossDesiegen(true);
		AudioManager::GetInstance().Stop("all");
		AudioManager::GetInstance().PlayOneShot("BossKill");
		// ボス専用アイテムをドロップ（100 % の確率で）
			int currentFloor = StageManager::GetInstance().floorCount;
		ItemDropManager::GetInstance().TryDropBossItem(1.0f, GetPosition(), currentFloor);
	}
	Enemy::DeadExecute();

	BossSlainUI::GetInstance()->Start();
	StageManager::GetInstance().generator->AppearStair();
	StageManager::GetInstance().generator->SpawnReturnCircle();
	StageManager::GetInstance().SetisBossSpawn(true);
	StageManager::GetInstance().OpenRoom();
}

bool BossBase::WallDetectionVision_Fan(VECTOR targetPos) {
	bool hit = Enemy::WallDetectionVision_Fan(targetPos);

	// 初めてレイに引っかかったら
	if (hit && !firstRayHit) {
		StageManager::GetInstance().CloseRoom();
		firstRayHit = true;
	}

	return hit;
}