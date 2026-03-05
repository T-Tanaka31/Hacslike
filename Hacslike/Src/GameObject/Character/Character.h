#pragma once
#include "../GameObject.h"
#include "../../Component/Animator.h"
#include "../../Manager/StageManager.h"

class Character : public GameObject {
protected:	// メンバ変数
	int modelHandle;		// モデルハンドル
	Animator* pAnimator;	// アニメーター
	int maxHp;
	int hp;					// 体力
	int atk;				// 攻撃力
	bool atking;			// 攻撃フラグ
	int def = 0;				// 防御力
	bool blocking;			// 防御フラグ
	float speed;

	float criticalHitRate;
	float criticalDamage;
	bool isCritical = false;

	int Lv;
	int exp;

	VECTOR prevPos;
	VECTOR wallCheckPos;

	int baseAttack;			//	基礎攻撃力
	float proximityCorrection;	//	近接攻撃力補正
	float rangedCorrection;		//	遠距離攻撃力補正
	bool isDead;

public:	// コンストラクタとデストラクタ

	Character(VECTOR _pos = VZero, std::string tag = "", int _Lv = 1, int _Exp = 5, int _speed = 0);

	virtual ~Character();
public:	// オーバーライドしたメンバ関数
	// あえてオーバーライドをせずに、
	// Characterクラスも抽象クラスとして扱う

public:
	//int SetAnimation();


public:	// ゲッターとセッター

#pragma region ModelHandle
	/*
	 * @function	GetModelHandle
	 * @brief		モデルハンドルの取得
	 * @return		int
	 */
	inline int GetModelHandle() const { return modelHandle; }

	/*
	 * @function	SetModelHandle
	 * @brief		モデルハンドルの変更
	 * @param[in]	int _v
	 */
	inline void SetModelHandle(int& _v) { modelHandle = _v; }
	inline void SetModelHandleDup(int _v) { modelHandle = _v; }
#pragma endregion

#pragma region Animator
	inline Animator* GetAnimator() const {
		return pAnimator;
	}
#pragma endregion

#pragma region Hp
	inline void AddHp(int heal) {
		if (hp + heal < maxHp)
			hp += heal;
		else
			hp = maxHp;
	}
	inline void SubHp(int damage) { hp -= (damage - def); }
	inline void AddMaxHp(int max) { maxHp += max; }
	inline void SetMaxHp(int _maxHp) { maxHp = _maxHp; }
	inline int GetMaxHp() const { return maxHp; }
	inline void SetHp(int _hp) { hp = _hp; }
	inline int GetHp() const { return hp; }
	void Damage(Character* attacker, int rawDamage);
#pragma endregion

#pragma region Attack
	// 攻撃力の取得
	inline int GetAtk() const { return atk; }
	//	クリティカル込みの攻撃力の取得
	inline int GetCriticalAtk() {
		isCritical = false; 
		if (GetRand(99) < criticalHitRate) {
			isCritical = true;
			return (int)(atk * (1.0f + (criticalDamage / 100.0f)));
		}
		return atk;
	}
	// 攻撃力の設定
	inline void SetAtk(int _atk) { atk = _atk; }
	// 攻撃フラグの取得
	inline bool isAttack() const { return atking; }
	// 攻撃フラグの設定
	inline void SetAttacking(bool _fact) { atking = _fact; }
	//	基礎攻撃力の取得
	inline int GetBaseAtk() const { return baseAttack; }
	//	基礎攻撃力の設定
	inline void SetBaseAtk(int _baseAtk) { baseAttack = _baseAtk; }
	//	近接攻撃力補正の取得
	inline float GetProximityCorrection() const { return proximityCorrection; }
	//	近接攻撃力補正の設定
	inline void SetProximityCorrection(float _proximityCorrection) { proximityCorrection = _proximityCorrection; }
	//	遠距離攻撃力補正の取得
	inline float GetRangedCorrection() const { return rangedCorrection; }
	//	遠距離攻撃力補正の設定
	inline void SetRangedCorrection(float _rangedCorrection) { rangedCorrection = _rangedCorrection; }

	inline bool IsCritical() const { return isCritical; }
#pragma endregion

#pragma region Defence
	// 防御力の取得
	inline int GetDef() const { return def; }
	// 防御力の設定
	inline void SetDef(int _def) { def = _def; }
	// 防御フラグの取得
	inline bool isBlocking() const { return blocking; }
	// 防御フラグの設定
	inline void SetBlocking(bool _fact) { blocking = _fact; }
#pragma endregion

#pragma region Speed
	//	速度の取得
	inline float GetSpeed() const { return speed; }
	//	速度の設定
	inline void SetSpeed(float _speed) { speed = _speed; }
#pragma endregion

#pragma region exp
	//	経験値の取得
	inline int GetExp() const { return exp; }
	//	経験値の設定
	inline void SetExp(int _Exp) { exp = _Exp; }
	//	経験値を足す処理
	inline void AddExp(int _addExp) {
		if (Lv == MAX_LV)
			exp = 0;
		else
			exp += _addExp;
	}
#pragma endregion

#pragma region Lv
	//	レベルの取得
	inline int GetLv() const { return Lv; }
	//	レベルの設定
	inline void SetLv(int _Lv) { Lv = _Lv; }
	//	レベルアップ
	inline void LvUp(int _Lv) {
		if (Lv == MAX_LV) {
			Lv += 0;
		}
		else
			Lv += _Lv;
	}
#pragma endregion

	// 壁判定の計算
	void CheckWall();
	// 壁摺りありの壁判定の計算
	VECTOR CheckWallToWallRubbing(VECTOR dir);

	inline void SetPosition(VECTOR set) override { position = set; }

	inline void SetPosition(float x, float y, float z) override {
		VECTOR set = VGet(x, y, z);
		position = set;
	}

	/// <summary>
	/// 死んだかどうか
	/// </summary>
	/// <returns></returns>
	inline virtual bool IsDead() { return isDead; }

	/// <summary>
	/// 死んだときの処理
	/// </summary>
	virtual void DeadExecute() = 0;




};

