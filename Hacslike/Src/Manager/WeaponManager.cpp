#include "WeaponManager.h"
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

WeaponManager::WeaponManager() {}
WeaponManager::~WeaponManager() { UnloadAllWeapons(); }

void WeaponManager::LoadWeapons() {
    UnloadAllWeapons();

    // ファイル名に「s」を付けて統一
    const std::string jsonPath = "Src/Data/WeaponsData.json";
    const std::string datPath = "Src/Data/WeaponsData.dat";

    // フォルダの自動作成
    try { std::filesystem::create_directories("Src/Data"); }
    catch (...) {}

    std::ifstream jsonFile(jsonPath);

    if (jsonFile.is_open()) {
        // ==========================================
        // 【開発モード】JSONから読み込み
        // ==========================================
        nlohmann::json data;
        try {
            jsonFile >> data;
        }
        catch (const nlohmann::json::parse_error& e) {
            return;
        }

        for (auto& w : data) {
            if (!w.contains("id")) continue;

            WeaponData weapon;
            weapon.id = w["id"];
            weapon.name = w.value("name", "Unknown");
            weapon.modelPath = w.value("modelPath", "");
            weapon.type = w.value("type", 0);

            // 各配列の読み込み (attackSpeed, colLength, colRadius)
            auto fillArray = [&](const std::string& key, float* target, int size) {
                if (w.contains(key) && w[key].is_array()) {
                    for (int i = 0; i < size; i++) {
                        target[i] = (i < (int)w[key].size()) ? w[key][i].get<float>() : 1.0f;
                    }
                }
            };

            fillArray("attackSpeed", weapon.attackSpeed, ATTACK_SPEED_NUM);
            fillArray("colLength", weapon.colLength, COL_LENGTH_NUM);
            fillArray("colRadius", weapon.colRadius, COL_RADIUS_NUM);

            // モデルのロード
            if (!weapon.modelPath.empty()) {
                weapon.modelHandle = MV1LoadModel(weapon.modelPath.c_str());
            }

            weaponTable[weapon.id] = weapon;
        }

        // --- WeaponsData.dat として保存 ---
        std::ofstream outFile(datPath, std::ios::binary);
        if (outFile.is_open()) {
            msgpack::pack(outFile, weaponTable);
            outFile.close();
        }
    }
    else {
        // ==========================================
        // 【製品モード】バイナリ(.dat)から読み込み
        // ==========================================
        std::ifstream datFile(datPath, std::ios::binary);
        if (!datFile.is_open()) return;

        std::vector<char> buffer((std::istreambuf_iterator<char>(datFile)), std::istreambuf_iterator<char>());
        datFile.close();

        if (buffer.empty()) return;

        try {
            auto oh = msgpack::unpack(buffer.data(), buffer.size());
            oh.get().convert(weaponTable);

            // モデルの再紐付け
            for (auto& [id, weapon] : weaponTable) {
                if (!weapon.modelPath.empty()) {
                    weapon.modelHandle = MV1LoadModel(weapon.modelPath.c_str());
                }
            }
        }
        catch (...) {}
    }
}

WeaponData* WeaponManager::GetWeapon(int id) {
    auto it = weaponTable.find(id);
    if (it != weaponTable.end()) return &it->second;
    return nullptr;
}

void WeaponManager::UnloadAllWeapons() {
    for (auto& pair : weaponTable) {
        if (pair.second.modelHandle != -1) {
            MV1DeleteModel(pair.second.modelHandle);
            pair.second.modelHandle = -1;
        }
    }
    weaponTable.clear();
}

WeaponData* WeaponManager::GetWeaponByName(const std::string& name) {
    for (auto& [id, weapon] : weaponTable) {
        if (weapon.name == name) return &weapon;
    }
    return nullptr;
}

int WeaponManager::GetMaxWeaponId() const {
    if (weaponTable.empty()) return 0;

    int maxId = 0;
    for (auto& [id, w] : weaponTable) {
        if (id > maxId) maxId = id;
    }
    return maxId;
}

