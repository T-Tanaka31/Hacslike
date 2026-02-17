#pragma once
#include <memory>
#include "../Character.h"
#include "../../Weapon/Weapon.h"
#include "../../Slash/Slash.h"
#include "../../Item/Inventory.h"
#include "../../Item/ItemBase.h"
#include "../../Coin/Coin.h"
#include "../../Artifact/ArtifactUI.h"
#include "../../Artifact/ArtifactBase.h"
#include "../../../Manager/ItemDropManager.h" 
#include "../../../Manager/WeaponManager.h"

#include "PlayerAttack.h"
#include "PlayerMovement.h"
#include "../../../Skill/SkillSelectUI.h"
#include "../../Artifact/ArtifactSelectUI.h"
#include "../../../Manager/SaveManager.h"
#include "../../../UI/MenuSaveLoad.h"
#include "../../../UI/Gauge.h"
#include "../../../UI/CircleGauge.h"
#include "../../../UI/TitleUI.h"

class StartTreasureChest;

/// <summary>
/// プレイヤークラス
/// </summary>
class Player : public Character {
private:
	static Player* instance; // シングルトン用
	static bool fullResetRequested; // 完全リセットフラグ（追加）
#pragma region メンバ変数
private:	//	メンバ変数

#pragma region クラス関連
	Weapon* pWeapon;		//	武器
	InputManager* input;	//	入力
	PlayerAttack* playerAttack;		//	Playerの攻撃
	PlayerMovement* playerMovement;	//	Playerの移動
	WeaponData* weaponData;			//	武器データ
#pragma endregion

#pragma region 武器関連
	int currentWeaponId;            // 初期武器ID
	int maxWeaponId;                // 武器の最大ID（JSONの数に合わせる）
#pragma endregion

#pragma region プレイヤーステータス関連
	float hpRate;	//	HP量
	int coinValue;	//	コイン枚数
	int expValue;	//	Exp量
	int maxExp;		//	最大経験値
	int remainExp;	//	経験値の超過数

	bool canEnhance = false; // 強化可能フラグ
#pragma endregion

#pragma region 死亡処理関連
	bool isDead;	//	死んでるかどうか	
	float deadTime;	//	死んでからの時間
#pragma endregion

	bool changeWeaponButtonPressed; // ボタン押下フラグ

#pragma region インベントリ/アイテム関連
	Inventory inventory; //アイテムインベントリ
	bool isItemUI;		 //アイテムのUIを開いているかどうか
	ItemEntity* hitItem = nullptr;
	float prevHP = -1.0f;
	ItemBase* equippedItem;
#pragma endregion

#pragma region アーティファクト/コイン
	ArtifactUI artifactUI;
	bool isArtifactUI = false;
	int coin_acquisition_value;
	bool isGetCoin = false;
	attactPower_raise_GetCoin* coinArtifact;  // コイン取得系のアーティファクト
	itemDropRateUpwardOnCoinAcquisition* itemArtifact;
	// ... 既存メンバ ...
	bool hitChest = false;

	// 追加: 接触中の宝箱オブジェクトを保持するポインタ
	StartTreasureChest* hitChestObj = nullptr;
	bool isSelectArtifact = false;
	bool isSelectBossArtifact = false;
	ArtifactSelectUI artifactSelectUI;
	std::vector<std::shared_ptr<ArtifactBase>> artifactChioces;
	std::vector<std::shared_ptr<ArtifactBase>> bossArtifactChioces;

#pragma endregion

#pragma region スキル
	SkillSelectUI skillUI;
	std::vector<std::shared_ptr<Skill>> skillChoices;
	bool isSelectingSkill = false;

#pragma endregion

#pragma region メニュー
	bool isMenuUI = false;
	MenuType menu;
	bool isMenuSelected = false;   // メニュー項目が選択中かどうか
	float blinkTime = 0.0f;       // 点滅用のタイマー
	bool blinkVisible = true;      // 現在表示中かどうか
	bool isSaveUI = false;
	int menuIndex = 0;
	bool isOpenMenu = false;
	// 最大メニュー数（増えてもここを変えるだけ）
	const int MENU_COUNT = 3;
	// セーブメニュー表示用ポインタ（インベントリやアーティファクトと同様に管理する）
	MenuSaveLoad* pSaveMenu = nullptr;
	int saveLoadChoice = 0; // 0: セーブ, 1: ロード
	bool isTitleUI = false;
	TitleUI titleUI;

#pragma endregion

#pragma region ゲージ関連
	Gauge<int>* hpBar;
	int hpBarPosX = 145;
	int hpBarPosY = 100;
	int hpBarWidth = 300;
	int hpBarHeight = 25;
	CircleGauge<int>* expBar;
	int expGaugePosX = 100;
	int expGaugePosY = 100;
	int expGaugeRadius = 50;
	int expGaugeThickness = 20;
	int expGaugeStartDeg = 195;
	int expGaugeEndDeg = -15;
	int uX = 100;
	int uY = 100;
	int lX = 100;
	int lY = 70;
#pragma endregion

#pragma endregion

	

#pragma region コンストラクタとデストラクタ
public:		//	コンストラクタとデストラクタ

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pos"></param>
	Player(VECTOR _pos = VZero);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();
#pragma endregion

#pragma region	オーバーライドしたメンバ関数
public:		//	オーバーライドしたメンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Start() override;

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() override;

	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Render() override;

public:		//	オーバーライドした衝突検知
	/// <summary>
	/// 当たった瞬間
	/// </summary>
	/// <param name="_pCol"></param>
	void OnTriggerEnter(Collider* _pCol) override;

	/// <summary>
	/// 当たっている間
	/// </summary>
	/// <param name="_pCol"></param>
	void OnTriggerStay(Collider* _pCol) override;

	/// <summary>
	/// 離れた瞬間
	/// </summary>
	/// <param name="_pCol"></param>
	void OnTriggerExit(Collider* _pCol) override;
#pragma endregion


#pragma region メンバ関数
public:		//	メンバ関数
	/// <summary>
	/// 武器切り替え
	/// </summary>
	/// <param name="weaponId"></param>
	void ChangeWeapon(int weaponId);

	/// <summary>
	/// 武器切り替え
	/// </summary>
	void WeaponInput();

	/// <summary>
	/// アイテムの取得
	/// </summary>
	void AddItem();

	/// <summary>
	/// メニューを開く
	/// </summary>
	void OpenMenu();

	/// <summary>
	/// メニューセレクト
	/// </summary>
	void selectMenu();

	/// <summary>
	/// メニューの表示
	/// </summary>
	void DrawMenu();

	/// <summary>
	/// コイン取得時の処理
	/// </summary>
	void GetCoin();

	/// <summary>
	/// コイン取得時の処理
	/// </summary>
	void GetCoin_Item();

	/// <summary>
	/// アイテムを取るキーの描画
	/// </summary>
	void AddItemRender();

	void GetArtifact();

	void GetBossArtifact();

	void GetArtifactRender();

	/// <summary>
	/// プレイヤーステータスの描画
	/// </summary>
	void PlayerStatusRender();

	/// <summary>
	/// 強くてニューゲーム用のセットアップ
	/// </summary>
	void PlayerSetUp();

	/// <summary>
	/// ニューゲーム用のせセットアップ
	/// </summary>
	void NewPlayerSetUp();


	void ResetHitItem() { hitItem = nullptr; }

	// 装備の効果を考慮して atk を再計算して適用する
	void UpdateAtkFromEquipment();

	void ResetUIStates();

	// 追加: 完全リセットフラグの操作
	static void RequestFullReset() { fullResetRequested = true; }
	static bool IsFullResetRequested() { return fullResetRequested; }
	static void ClearFullResetFlag() { fullResetRequested = false; }
	
#pragma endregion

#pragma region GetterとSetter
public:		//	Getter と Setter
	/*
	 *	@function	GetWeapon
	 *	@brief		武器の取得
	 *	@return		Weapon*
	 */
	inline Weapon* GetWeapon() const { return pWeapon; }

	/*
	 *	@function	SetWeapon
	 *	@brief		武器の変更
	 *	@param[in]	Weapon* _pWeapon
	 */
	inline void SetWeapon(Weapon* _pWeapon) { pWeapon = _pWeapon; }

	/// <summary>
	/// インベントリの取得
	/// </summary>
	/// <returns></returns>
	inline Inventory* GetInventory() { return &inventory; }

	/// <summary>
	/// コインの取得
	/// </summary>
	/// <returns></returns>
	inline int GetCoinValue() const { return coinValue; }

	/// <summary>
	/// コインの変更
	/// </summary>
	/// <param name="_coin"></param>
	inline void SetCoinValue(int _coin) { coinValue = _coin; }

	/// <summary>
	/// コインの加算
	/// </summary>
	/// <param name="_coin"></param>
	inline void AddCoinValue(int _coin) { coinValue += _coin; }

	/// <summary>
	/// コインの減算
	/// </summary>
	/// <param name="_coin"></param>
	inline void SubCoinValue(int _coin) { coinValue -= _coin; }

	/// <summary>
	/// 入手コインの取得
	/// </summary>
	/// <returns></returns>
	inline int GetCoinAcquisitionValue() const { return coin_acquisition_value; }

	/// <summary>
	/// 入手コインの設定
	/// </summary>
	/// <param name="_value"></param>
	inline void SetCoinAcquisitionValue(int _value) { coin_acquisition_value = _value; }

	/// <summary>
	/// メニューセレクトの設定
	/// </summary>
	/// <returns></returns>
	inline bool GetIsMenuSelected() { return isMenuSelected; }

	/// <summary>
	/// コインを取得下かどうかの取得
	/// </summary>
	/// <returns></returns>
	inline bool GetIsCoin() { return isGetCoin; }

	/// <summary>
	/// コインを取得下かどうかの設定
	/// </summary>
	/// <param name="isCoin"></param>
	inline void SetIsCoin(bool isCoin) { isGetCoin = isCoin; }

	/// <summary>
	/// アーティファクトの設定
	/// </summary>
	/// <param name="artifact"></param>
	void SetCoinArtifact(attactPower_raise_GetCoin* artifact) { coinArtifact = artifact; }
	void SetItemArtifact(itemDropRateUpwardOnCoinAcquisition* artifact) { itemArtifact = artifact; }

	/// <summary>
	/// 会心率の取得
	/// </summary>
	/// <returns></returns>
	inline float GetCriticalHitRate() const { return criticalHitRate; }

	/// <summary>
	/// 会心率の変更
	/// </summary>
	/// <param name="_criticalHitRate"></param>
	inline void SetCriticalHitRate(float _criticalHitRate) { criticalHitRate = _criticalHitRate; }

	/// <summary>
	/// 会心ダメージの取得
	/// </summary>
	/// <returns></returns>
	inline float GetCriticalDamage() const { return criticalDamage; }

	/// <summary>
	/// 会心ダメージの変更
	/// </summary>
	/// <param name="_criticalDamage"></param>
	inline void SetCriticalDamage(float _criticalDamage) { criticalDamage = _criticalDamage; }

	/// <summary>
	/// プレイヤーの攻撃処理の取得
	/// </summary>
	/// <returns></returns>
	inline PlayerAttack* GetPlayerAttack() const { return playerAttack; }

	/// <summary>
	/// プレイヤーの移動処理の所得
	/// </summary>
	/// <returns></returns>
	inline PlayerMovement* GetPlayerMovement() const { return playerMovement; }

	inline int GetPlayerLevel() { return Lv; }

	/// <summary>
	/// 強化可能フラグ
	/// </summary>
	/// <returns></returns>
	bool GetCanEnhance() const { return canEnhance; }

	/// <summary>
	/// 強化可能フラグの変更
	/// </summary>
	/// <param name="_flag"></param>
	void SetCanEnhance(bool _flag) { canEnhance = _flag; }

	/// <summary>
	/// 死んでるかどうか
	/// </summary>
	/// <returns></returns>
	inline bool GetIsDead() const { return isDead; }

	inline bool GetisItemUI() const { return isItemUI; }

	inline void SetIsOpenMenu(bool _isOpenMenu) { isOpenMenu = _isOpenMenu; }

	inline bool GetIsSelectArtifact() { return isSelectArtifact; }

	/// <summary>
	/// プレイヤーの前方向の取得
	/// </summary>
	/// <returns></returns>
	inline VECTOR GetForward() const {
		return VNorm(VGet(
			-sinf(Deg2Rad(rotation.y)),
			0.0f,
			-cosf(Deg2Rad(rotation.y))
		));
	}
#pragma endregion

	float RuneCost(int L);


public:
	// 🔹 シングルトン関連
	static Player* CreateInstance(VECTOR _pos = VZero);
	static Player* GetInstance();
	static void DestroyInstance();

	void DeadExecute() override;

public:
	// セーブ/ロード用の API（SaveManager 経由で呼ばれる）
	void SaveTo(BinaryWriter& w);
	void LoadFrom(BinaryReader& r, uint32_t saveVersion);
	// メタ情報更新（セーブ時にメタへ書き込むため）
	uint32_t GetLevelForSave() const { return (uint32_t)Lv; }
	uint32_t GetFloorForSave() const;
};