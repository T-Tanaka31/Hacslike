#include "ItemEquip.h"
#include <iostream>
#include"../../Character/Character.h"
#include"../../Character/Player/Player.h"
#include"../../../Manager/AudioManager.h"

#pragma region 剣
ItemSword::ItemSword(VECTOR _pos, const std::string& _name, const std::string& _desc, int _value, int _effectValue, const std::string& _weaponType)
	: ItemBase(VZero, "item", "Sword_Iron", _name, _desc, "Equipment","Normal", _value, _effectValue, "Res/ItemIcon/sword.png")
	, attackValue(_effectValue) {
	Start();
}

void ItemSword::Start()
{
	AudioManager::GetInstance().Load("Res/SE/UseItem.mp3", "UseEquip", false);
}

void ItemSword::Render()
{
}

void ItemSword::Use()
{
	AudioManager::GetInstance().PlayOneShot("UseEquip");
	Player::GetInstance()->ChangeWeapon(modelPath);
	Player::GetInstance()->SetAtk(Player::GetInstance()->GetBaseAtk() + Player::GetInstance()->GetProximityCorrection() + attackValue);

	std::cout << name << " を装備!攻撃力 +" << attackValue << "!\n";
}

void ItemSword::UnEquip()
{
}

void ItemSword::SaveTo(BinaryWriter& w)
{
	w.WritePOD(attackValue);
}

void ItemSword::LoadFrom(BinaryReader& r)
{
	r.ReadPOD(attackValue);
}
#pragma endregion

#pragma region 斧
ItemAxe::ItemAxe(VECTOR _pos, const std::string& _name, const std::string& _desc, int _value, int _effectValue, const std::string& _weaponType)
	: ItemBase(VZero, "item", "Axe", _name, _desc, "Equipment", "Normal", _value, _effectValue, "Res/ItemIcon/Axe.png")
	, attackValue(_effectValue) {
	Start();
}

void ItemAxe::Start()
{
	AudioManager::GetInstance().Load("Res/SE/UseItem.mp3", "UseEquip", false);
}

void ItemAxe::Render()
{
}

void ItemAxe::Use()
{
	AudioManager::GetInstance().PlayOneShot("UseEquip");
	Player::GetInstance()->ChangeWeapon(modelPath);
	Player::GetInstance()->SetAtk(Player::GetInstance()->GetBaseAtk() + Player::GetInstance()->GetProximityCorrection() + attackValue);
}

void ItemAxe::UnEquip()
{
}

void ItemAxe::SaveTo(BinaryWriter& w)
{
	w.WritePOD(attackValue);
}

void ItemAxe::LoadFrom(BinaryReader& r)
{
	r.ReadPOD(attackValue);
}
#pragma endregion

#pragma region 木の棒
ItemStick::ItemStick(VECTOR _pos, const std::string& _name, const std::string& _desc, int _value, int _effectValue, const std::string& _weaponType)
	: ItemBase(VZero, "item", "Stick", _name, _desc, "Equipment", "Normal", _value, _effectValue, "Res/ItemIcon/stick.png")
	, attackValue(_effectValue) {
}

void ItemStick::Start()
{
}

void ItemStick::Render()
{
}

void ItemStick::Use()
{
	Player::GetInstance()->ChangeWeapon(modelPath);
	Player::GetInstance()->SetAtk(Player::GetInstance()->GetBaseAtk() + Player::GetInstance()->GetProximityCorrection() + attackValue);
}

void ItemStick::UnEquip()
{
}

void ItemStick::SaveTo(BinaryWriter& w)
{
	w.WritePOD(attackValue);
}

void ItemStick::LoadFrom(BinaryReader& r)
{
	r.ReadPOD(attackValue);
}
#pragma endregion

#pragma region グレソ
Greatsword::Greatsword(VECTOR _pos, const std::string& _name, const std::string& _desc, int _value, int _effectValue, const std::string& _weaponType)
	: ItemBase(VZero, "item", "Greatsword", _name, _desc, "Equipment", "Normal", _value, _effectValue, "Res/ItemIcon/rune-sword.png")
	, attackValue(_effectValue) {
	Start();
}

void Greatsword::Start()
{
	AudioManager::GetInstance().Load("Res/SE/UseItem.mp3", "UseEquip", false);
}

void Greatsword::Render()
{
}

void Greatsword::Use()
{
	AudioManager::GetInstance().PlayOneShot("UseEquip");
	Player::GetInstance()->ChangeWeapon(modelPath);
	Player::GetInstance()->SetAtk(Player::GetInstance()->GetBaseAtk() + Player::GetInstance()->GetProximityCorrection() + attackValue);
}

void Greatsword::UnEquip()
{
}

void Greatsword::SaveTo(BinaryWriter& w)
{
	w.WritePOD(attackValue);
}

void Greatsword::LoadFrom(BinaryReader& r)
{
	r.ReadPOD(attackValue);
}
#pragma endregion

#pragma region 槍
Spear::Spear(VECTOR _pos, const std::string& _name, const std::string& _desc, int _value, int _effectValue, const std::string& _weaponType)
	: ItemBase(VZero, "item", "Spear", _name, _desc, "Equipment", "Normal", _value, _effectValue, "Res/ItemIcon/coiled-nail.png")
	, attackValue(_effectValue) {
	Start();
}

void Spear::Start()
{
	AudioManager::GetInstance().Load("Res/SE/UseItem.mp3", "UseEquip", false);
}

void Spear::Render()
{
}

void Spear::Use()
{
	AudioManager::GetInstance().PlayOneShot("UseEquip");
	Player::GetInstance()->ChangeWeapon(2);
	Player::GetInstance()->SetAtk(Player::GetInstance()->GetBaseAtk() + Player::GetInstance()->GetProximityCorrection() + attackValue);
}

void Spear::UnEquip()
{
}

void Spear::SaveTo(BinaryWriter& w)
{
	w.WritePOD(attackValue);
}

void Spear::LoadFrom(BinaryReader& r)
{
	r.ReadPOD(attackValue);
}
#pragma endregion

#pragma region ハンマー
Hammer::Hammer(VECTOR _pos, const std::string& _name, const std::string& _desc, int _value, int _effectValue, const std::string& _weaponType)
	: ItemBase(VZero, "item", "Hammer", _name, _desc, "Equipment", "Rare", _value, _effectValue, "Res/ItemIcon/Hammer.png")
	, attackValue(_effectValue) {
	Start();
}

void Hammer::Start()
{
}

void Hammer::Render()
{
}

void Hammer::Use()
{
	AudioManager::GetInstance().PlayOneShot("UseEquip");
	Player::GetInstance()->ChangeWeapon(modelPath);
	Player::GetInstance()->SetAtk(Player::GetInstance()->GetBaseAtk() + Player::GetInstance()->GetProximityCorrection() + attackValue);
	Player::GetInstance()->SetCriticalHitRate(Player::GetInstance()->GetCriticalHitRate() - criticalHitRate);
	Player::GetInstance()->SetCriticalDamage(Player::GetInstance()->GetCriticalDamage() + criticalDamage);


}

void Hammer::UnEquip()
{
	Player::GetInstance()->SetCriticalHitRate(Player::GetInstance()->GetCriticalHitRate() + criticalHitRate);
	Player::GetInstance()->SetCriticalDamage(Player::GetInstance()->GetCriticalDamage() - criticalDamage);
}

void Hammer::SaveTo(BinaryWriter& w)
{
	w.WritePOD(attackValue);
}

void Hammer::LoadFrom(BinaryReader& r)
{
	r.ReadPOD(attackValue);
}
#pragma endregion

#pragma region 鎌
Sickle::Sickle(VECTOR _pos, const std::string& _name, const std::string& _desc, int _value, int _effectValue, const std::string& _weaponType)
	: ItemBase(VZero, "item", "Sickle", _name, _desc, "Equipment", "Rare", _value, _effectValue, "Res/ItemIcon/Sickle.png")
	, attackValue(_effectValue) {
	Start();
}

void Sickle::Start()
{
}

void Sickle::Render()
{
}

void Sickle::Use()
{
	AudioManager::GetInstance().PlayOneShot("UseEquip");
	Player::GetInstance()->ChangeWeapon(modelPath);
	Player::GetInstance()->SetAtk(Player::GetInstance()->GetBaseAtk() + Player::GetInstance()->GetProximityCorrection() + attackValue);
	Player::GetInstance()->SetCriticalHitRate(Player::GetInstance()->GetCriticalHitRate() + criticalHitRate);
	Player::GetInstance()->SetCriticalDamage(Player::GetInstance()->GetCriticalDamage() + criticalDamage);
}

void Sickle::UnEquip()
{
	Player::GetInstance()->SetCriticalHitRate(Player::GetInstance()->GetCriticalHitRate() - criticalHitRate);
	Player::GetInstance()->SetCriticalDamage(Player::GetInstance()->GetCriticalDamage() - criticalDamage);
}

void Sickle::SaveTo(BinaryWriter& w)
{
	w.WritePOD(attackValue);

}

void Sickle::LoadFrom(BinaryReader& r)
{
	r.ReadPOD(attackValue);

}
#pragma endregion

#pragma region 牙剣
FangSword::FangSword(VECTOR _pos, const std::string& _name, const std::string& _desc, int _value, int _effectValue, const std::string& _weaponType)
	: ItemBase(VZero, "item", "FangSword", _name, _desc, "Equipment", "Rare", _value, _effectValue, "Res/ItemIcon/FangSword.png")
	, attackValue(_effectValue) {
	Start();
}

void FangSword::Start()
{
}

void FangSword::Render()
{
}

void FangSword::Use()
{
	AudioManager::GetInstance().PlayOneShot("UseEquip");
	Player::GetInstance()->ChangeWeapon(modelPath);
	Player::GetInstance()->SetAtk(Player::GetInstance()->GetBaseAtk() + Player::GetInstance()->GetProximityCorrection() + attackValue);

}

void FangSword::UnEquip()
{
}

void FangSword::SaveTo(BinaryWriter& w)
{
	w.WritePOD(attackValue);
}

void FangSword::LoadFrom(BinaryReader& r)
{
	r.ReadPOD(attackValue);
}
#pragma endregion

#pragma region デュラハンハンマー
DuraHammmer::DuraHammmer(VECTOR _pos, const std::string& _name, const std::string& _desc, int _value, int _effectValue, const std::string& _weaponType)
	: ItemBase(VZero, "item", "DuraHammmer", _name, _desc, "Equipment", "Rare", _value, _effectValue, "Res/ItemIcon/DuraHammmer.png")
	, attackValue(_effectValue) {
	Start();
}

void DuraHammmer::Start()
{
}

void DuraHammmer::Render()
{
}

void DuraHammmer::Use()
{
	AudioManager::GetInstance().PlayOneShot("UseEquip");
	Player::GetInstance()->ChangeWeapon(modelPath);
	Player::GetInstance()->SetAtk(Player::GetInstance()->GetBaseAtk() + Player::GetInstance()->GetProximityCorrection() + attackValue);
	Player::GetInstance()->SetCriticalHitRate(Player::GetInstance()->GetCriticalHitRate() + criticalHitRate);
	Player::GetInstance()->SetCriticalDamage(Player::GetInstance()->GetCriticalDamage() + criticalDamage);
}

void DuraHammmer::UnEquip()
{
	Player::GetInstance()->SetCriticalHitRate(Player::GetInstance()->GetCriticalHitRate() - criticalHitRate);
	Player::GetInstance()->SetCriticalDamage(Player::GetInstance()->GetCriticalDamage() - criticalDamage);
}

void DuraHammmer::SaveTo(BinaryWriter& w)
{
	w.WritePOD(attackValue);
}

void DuraHammmer::LoadFrom(BinaryReader& r)
{
	r.ReadPOD(attackValue);
}


#pragma region 銃
gun::gun(VECTOR _pos, const std::string& _name, const std::string& _desc, int _value, int _effectValue, const std::string& _weaponType)
	: ItemBase(VZero, "item", "Gun", _name, _desc, "Equipment", "Normal", _value, _effectValue, "Res/ItemIcon/Gun.png")
	, attackValue(_effectValue) {
	Start();
}

void gun::Start()
{
	AudioManager::GetInstance().Load("Res/SE/UseItem.mp3", "UseEquip", false);
}

void gun::Render()
{
}

void gun::Use()
{
	AudioManager::GetInstance().PlayOneShot("UseEquip");
	Player::GetInstance()->ChangeWeapon(11);
	Player::GetInstance()->SetAtk(Player::GetInstance()->GetBaseAtk() + Player::GetInstance()->GetRangedCorrection() + attackValue);
}

void gun::UnEquip()
{
}

void gun::SaveTo(BinaryWriter& w)
{
	w.WritePOD(attackValue);
}

void gun::LoadFrom(BinaryReader& r)
{
	r.ReadPOD(attackValue);
}
#pragma endregion

