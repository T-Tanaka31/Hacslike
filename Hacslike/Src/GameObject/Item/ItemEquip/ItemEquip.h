#pragma once
#include "../ItemBase.h"
#include"../../../Save/SaveIO.h"

class Player;

class ItemWeapon : public ItemBase {
private:
	int attackValue;
	int modelPath;
	std::string weaponType;
};

#pragma region 剣
class ItemSword : public ItemBase
{
private:
	int attackValue;		 // 攻撃力
	int modelPath = 4;
	std::string weaponType;  // 武器の種類

public:
	ItemSword(VECTOR _pos = VZero, const std::string& _name = "", const std::string& _desc = "", int _value = 0, int _effectValue = 0, const std::string& _weaponType = "");

	void Start()override;
	void Render()override;
	void Use()override;
	void UnEquip()override;

	void SaveTo(BinaryWriter& w) override;
	void LoadFrom(BinaryReader& r) override;

	inline int GetEffectValue() const override { return attackValue; }
	inline const std::string& GetWeaponType() const { return weaponType; }
	ItemType GetItemType() const override { return ItemType::Invaled; }
	HealSize GetHealType() const override { return HealSize::Invaled; }
	inline void SetAttackValue(int _v) { attackValue = _v; }
};
#pragma endregion

#pragma region 斧
class ItemAxe : public ItemBase {
private:
	int attackValue;
	int modelPath = 3;
	std::string weaponType;

public:
	ItemAxe(VECTOR _pos = VZero, const std::string& _name = "", const std::string& _desc = "", int _value = 0, int _effectValue = 0, const std::string& _weaponType = "");

	void Start()override;
	void Render()override;
	void Use()override;
	void UnEquip()override;

	void SaveTo(BinaryWriter& w) override;
	void LoadFrom(BinaryReader& r) override;

	inline int GetEffectValue() const override { return attackValue; }
	inline const std::string& GetWeaponType() const { return weaponType; }
	ItemType GetItemType() const override { return ItemType::Invaled; }
	HealSize GetHealType() const override { return HealSize::Invaled; }
	inline void SetAttackValue(int _v) { attackValue = _v; }
};
#pragma endregion

#pragma region 木の棒
class ItemStick : public ItemBase {
private:
	int attackValue;
	int modelPath = 10;
	std::string weaponType;

public:
	ItemStick(VECTOR _pos = VZero, const std::string& _name = "", const std::string& _desc = "", int _value = 0, int _effectValue = 0, const std::string& _weaponType = "");

	void Start()override;
	void Render()override;
	void Use()override;
	void UnEquip()override;

	void SaveTo(BinaryWriter& w) override;
	void LoadFrom(BinaryReader& r) override;

	inline int GetEffectValue() const override { return attackValue; }
	inline const std::string& GetWeaponType() const { return weaponType; }
	ItemType GetItemType() const override { return ItemType::Invaled; }
	HealSize GetHealType() const override { return HealSize::Invaled; }
	inline void SetAttackValue(int _v) { attackValue = _v; }
};
#pragma endregion

#pragma region グレソ
class Greatsword : public ItemBase {
private:
	int attackValue;
	int modelPath = 9;
	std::string weaponType;

public:
	Greatsword(VECTOR _pos = VZero, const std::string& _name = "", const std::string& _desc = "", int _value = 0, int _effectValue = 0, const std::string& _weaponType = "");

	void Start()override;
	void Render()override;
	void Use()override;
	void UnEquip()override;

	void SaveTo(BinaryWriter& w) override;
	void LoadFrom(BinaryReader& r) override;

	inline int GetEffectValue() const override { return attackValue; }
	inline const std::string& GetWeaponType() const { return weaponType; }
	ItemType GetItemType() const override { return ItemType::Invaled; }
	HealSize GetHealType() const override { return HealSize::Invaled; }
	inline void SetAttackValue(int _v) { attackValue = _v; }
};
#pragma endregion

#pragma region 槍
class Spear : public ItemBase {
private:
	int attackValue;
	int modelPath = 2;
	std::string weaponType;

public:
	Spear(VECTOR _pos = VZero, const std::string& _name = "", const std::string& _desc = "", int _value = 0, int _effectValue = 0, const std::string& _weaponType = "");

	void Start()override;
	void Render()override;
	void Use()override;
	void UnEquip()override;

	void SaveTo(BinaryWriter& w) override;
	void LoadFrom(BinaryReader& r) override;

	inline int GetEffectValue() const override { return attackValue; }

	inline const std::string& GetWeaponType() const { return weaponType; }
	ItemType GetItemType() const override { return ItemType::Invaled; }
	HealSize GetHealType() const override { return HealSize::Invaled; }
	inline void SetAttackValue(int _v) { attackValue = _v; }
};
#pragma endregion

#pragma region ハンマー
class Hammer : public ItemBase {
private:
	int attackValue;
	int criticalHitRate = 40;
	int criticalDamage = 80;
	int modelPath = 11;
	std::string weaponType;

public:
	Hammer(VECTOR _pos = VZero, const std::string& _name = "", const std::string& _desc = "", int _value = 0, int _effectValue = 0, const std::string& _weaponType = "");

	void Start()override;
	void Render()override;
	void Use()override;
	void UnEquip()override;

	void SaveTo(BinaryWriter& w) override;
	void LoadFrom(BinaryReader& r) override;

	inline int GetEffectValue() const override { return attackValue; }

	inline const std::string& GetWeaponType() const { return weaponType; }
	ItemType GetItemType() const override { return ItemType::Invaled; }
	HealSize GetHealType() const override { return HealSize::Invaled; }
	inline void SetAttackValue(int _v) { attackValue = _v; }
};
#pragma endregion

#pragma region 鎌
class Sickle : public ItemBase {
private:
	int attackValue;
	int criticalHitRate = 10;
	int criticalDamage = 30;
	int modelPath = 12;
	std::string weaponType;

public:
	Sickle(VECTOR _pos = VZero, const std::string& _name = "", const std::string& _desc = "", int _value = 0, int _effectValue = 0, const std::string& _weaponType = "");

	void Start()override;
	void Render()override;
	void Use()override;
	void UnEquip()override;

	void SaveTo(BinaryWriter& w) override;
	void LoadFrom(BinaryReader& r) override;

	inline int GetEffectValue() const override { return attackValue; }

	inline const std::string& GetWeaponType() const { return weaponType; }
	ItemType GetItemType() const override { return ItemType::Invaled; }
	HealSize GetHealType() const override { return HealSize::Invaled; }
	inline void SetAttackValue(int _v) { attackValue = _v; }
};
#pragma endregion

#pragma region 牙剣
class FangSword : public ItemBase {
private:
	int attackValue;
	int modelPath = 14;
	std::string weaponType;

public:
	FangSword(VECTOR _pos = VZero, const std::string& _name = "", const std::string& _desc = "", int _value = 0, int _effectValue = 0, const std::string& _weaponType = "");

	void Start()override;
	void Render()override;
	void Use()override;
	void UnEquip()override;

	void SaveTo(BinaryWriter& w) override;
	void LoadFrom(BinaryReader& r) override;

	inline int GetEffectValue() const override { return attackValue; }

	inline const std::string& GetWeaponType() const { return weaponType; }
	ItemType GetItemType() const override { return ItemType::Invaled; }
	HealSize GetHealType() const override { return HealSize::Invaled; }
	inline void SetAttackValue(int _v) { attackValue = _v; }
};
#pragma endregion

#pragma region デュラハンハンマー
class DuraHammmer : public ItemBase {
private:
	int attackValue;
	int criticalHitRate = 30;
	int criticalDamage = 90;
	int modelPath = 13;
	std::string weaponType;

public:
	DuraHammmer(VECTOR _pos = VZero, const std::string& _name = "", const std::string& _desc = "", int _value = 0, int _effectValue = 0, const std::string& _weaponType = "");

	void Start()override;
	void Render()override;
	void Use()override;
	void UnEquip()override;

	void SaveTo(BinaryWriter& w) override;
	void LoadFrom(BinaryReader& r) override;

	inline int GetEffectValue() const override { return attackValue; }

	inline const std::string& GetWeaponType() const { return weaponType; }
	ItemType GetItemType() const override { return ItemType::Invaled; }
	HealSize GetHealType() const override { return HealSize::Invaled; }
	inline void SetAttackValue(int _v) { attackValue = _v; }
};
#pragma endregion

#pragma region 銃
class gun : public ItemBase {
private:
	int attackValue;
	std::string weaponType;

public:
	gun(VECTOR _pos = VZero, const std::string& _name = "", const std::string& _desc = "", int _value = 0, int _effectValue = 0, const std::string& _weaponType = "");

	void Start()override;
	void Render()override;
	void Use()override;
	void UnEquip()override;

	void SaveTo(BinaryWriter& w) override;
	void LoadFrom(BinaryReader& r) override;

	inline int GetEffectValue() const override { return attackValue; }
	inline const std::string& GetWeaponType() const { return weaponType; }
	ItemType GetItemType() const override { return ItemType::Invaled; }
	HealSize GetHealType() const override { return HealSize::Invaled; }
	inline void SetAttackValue(int _v) { attackValue = _v; }
};
#pragma endregion