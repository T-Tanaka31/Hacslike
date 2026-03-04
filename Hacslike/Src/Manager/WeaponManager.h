#pragma once
#include <unordered_map>
#include <string>
#include "../Data/json.hpp"
#include "../Component/Singleton.h"
#include "../Definition.h"
#include <DxLib.h> // DxLibのヘッダ
#include <msgpack.hpp>

struct WeaponData {
    int id;
    std::string name;
    std::string modelPath;
    int type;
    float attackSpeed[ATTACK_SPEED_NUM];
    float colLength[COL_LENGTH_NUM];
    float colRadius[COL_RADIUS_NUM];

    // メモリ上のハンドル（保存不要）
    int modelHandle = -1;

    // Msgpackで保存・復元する項目
    MSGPACK_DEFINE(id, name, modelPath, type, attackSpeed, colLength, colRadius);
};

class WeaponManager : public Singleton<WeaponManager>  {
#pragma region シングルトン
public:
    WeaponManager();
    ~WeaponManager();
#pragma endregion

private:
    std::unordered_map<int, WeaponData> weaponTable;

public:
    void LoadWeapons();       // JSONロード＋モデルロード
    WeaponData* GetWeapon(int id);                   // IDで取得
    void UnloadAllWeapons();                         // モデル解放
    WeaponData* GetWeaponByName(const std::string& name);

    

    int GetMaxWeaponId() const;
};
