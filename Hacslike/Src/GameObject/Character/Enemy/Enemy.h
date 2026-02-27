#pragma once
#include "../Character.h"
#include "../../../Manager/CollisionManager.h"
#include "../../../Manager/EffectManager.h"
#include "../../../Component/Collider/SphereHitBox.h"
#include "../../../Manager/AudioManager.h"
#include "Other/AttackArea.h"
#include <msgpack.hpp>

class Enemy : public Character {
protected:

	struct Ray_Fan {
		float rayLenght = 1000.0f;	// レイの長さ
		float rayAngle = 100.0f;		// レイの角度
		int rayCount = 30;			// レイの数
		float raySpan = 0.1f;		// レイが更新される間隔
		float rayTime = raySpan;	// レイが更新される時間
	};

	struct Point {
		VECTOR position;
	};

	Point point;

	struct Fan {
		VECTOR position;		// 中心
		float rangeDegree;		// 範囲
		float length;			// 長さ
		float directionDegree;	// 方向
	};

	Fan fan;

protected:
	// 敵の種類
	EnemyType type;
	// 名前
	std::string name;
	// モデルやアニメーションのファイルパス
	std::string mPath;
	// 動きの速さ
	float moveSpeed;
	// レイに入ってるかどうか
	bool rayAnswer;
	// プレイヤーと接触しているか
	bool isTouch;
	// 攻撃判定更新用
	std::vector<SphereHitBox*> attackColliderList;
	// 攻撃アニメーションIDリスト
	std::vector<int> attackAnimationList;
	// 次攻撃するまでの時間
	float atkTime;
	// 攻撃する間隔
	float atkSpan;
	// 徘徊するときにゴールとする座標
	VECTOR goalPos;
	// 次徘徊するまでの時間
	float nextWanderTime;
	float nextWanderSpan;
	// 視界
	Ray_Fan vision;
	// 攻撃前の攻撃範囲表示
	AttackArea area;
	// ボスか
	bool isBoss = false;

	EnemyData m_param; // JSONから読み込んだデータを保持する変数

	// 道のり
	std::list<VECTOR> moveRoots;
	VECTOR currentRoot;
	VECTOR prevRoot;
	// 来た道を戻ってるか
	bool isBack;

	// 攻撃関連
	float attack01ColliderSpawnTime;
	float attack02ColliderSpawnTime;
	float attack03ColliderSpawnTime;
	float colliderLifeTime = 2;
	float attack01ColliderRadius;
	float attack02ColliderRadius;
	float attack03ColliderRadius;

	float deadAnimationTime;
public:
	Enemy();
	~Enemy();

	virtual void Start() override;
	virtual void Update() override;
	virtual void Render() override;

	// 再使用時の初期化
	void Setup();
	// 未使用状態
	void Teardown();
	// 死んだとき実行する処理
	void DeadExecute() override;
	// Jsonファイルからステータスを持ってくる
	void SetStatusData(int enemyID);
	// アニメーションのロード
	void LoadAnimation();
	// 敵の種類を取得
	inline EnemyType GetType() const { return type; }
	inline void SetType(EnemyType _type) { type = _type; }
	// アニメーションイベントの登録
	void SetAnimEvent(std::string animName, std::function<void()> func, float time = 0);
	void SetAnimEventForAttackCollider(std::string animName, float colliderspawnTime, float colliderLifeTime, float radius, float dis, float mag = 1);
	void SetAnimEventForAttackCollider(std::string animName, float colliderspawnTime, float colliderLifeTime, float radius, VECTOR pos, float dis = 1, float mag = 1);

protected:
	// 攻撃の当たり判定の座標計算
	VECTOR AttackAreaPos(float dis);
	VECTOR AttackAreaPos(VECTOR pos, float dis = 1);
	// ターゲットを見る
	void LookTarget(VECTOR targetPos, VECTOR axis = VUp);
	// 追跡行動
	virtual void Tracking();
	// 移動
	virtual void Move(VECTOR targetPos);
	// 攻撃行動
	void Attack();
	// 視界
	virtual bool Vision_Ray();
	virtual bool Vision_Circle(float r);
	virtual bool Vision_Fan(VECTOR targetPos);
	// 壁に遮られる視界
	virtual bool WallDetectionVision_Fan(VECTOR targetPos);
	// Vision_Fanのデバッグ表示
	void DrawVisionFanDebug();
	// 徘徊行動
	virtual void Wander();

public:
	// 入ったとき
	virtual void OnTriggerEnter(Collider* _pOther) override;
	// 入っているとき
	virtual void OnTriggerStay(Collider* _pOther) override;
	// 出たとき
	virtual void OnTriggerExit(Collider* _pOther) override;

public:

	// 既に存在するメソッド群の補完として追加
	VECTOR GetPosition() const { return position; }            // もし Character にあるなら不要
	float GetRotationY() const { return rotation.y; }
	void SetRotationY(float y) { rotation.y = y; }

	float GetHP() const { return hp; }
	void SetHP(float v) { hp = v; }

	bool IsDead() const { return isDead; }
	void SetDeadState(bool d) {
		isDead = d;
		if (d) {
			hp = 0;
			// 表示やアニメ切替（報酬は与えない）
			if (pAnimator) pAnimator->Play("dead");
			SetVisible(false);
		}
	}
	// 必要なら collider 取得
	Collider* GetCollider() const { return pCollider; }

	bool IsBoss() { return isBoss; }

	void InitializeData(const EnemyData* data) {
		if (data) {
			m_param = *data; // 構造体をコピー
		}
	}

};

