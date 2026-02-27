#pragma once
#include "../Component/Singleton.h"
#include <vector>
#include <list>          // listも使っているので追加
#include <string>        // stringを使っているので追加
#include <unordered_map> // これが最重要
#include <fstream>
#include "../Definition.h"
#include"../Manager/SaveManager.h"
#include <functional>
#include <msgpack.hpp>
#include "../Data/json.hpp"


class Enemy;

struct EnemyData {
	int id;
	int typeID;
	std::string name;
	std::string mPath; // モデルパス
	int hp;
	int atk;
	int def;
	int exp;
	float spd;
	float cRate;
	float cDamageRate;

	// 視界（Vision）関連
	float rAngle;
	int rCount;
	float rLenght;

	// Msgpackで保存・復元する項目
	MSGPACK_DEFINE(id, typeID, name, mPath, hp, atk, def, exp, spd, cRate, cDamageRate, rAngle, rCount, rLenght);
};

struct EnemyUtility {
	std::list<Enemy*> unuseArray;
	std::function<Enemy* ()> CreateEnemy;
	int originModelHandle;

	EnemyUtility(std::function<Enemy* ()> _createFunc,int originMHandle)
		:CreateEnemy(_createFunc)
		,originModelHandle(originMHandle)
	{}

	~EnemyUtility() {
		// 未使用リスト内のデータ削除
		DeleteAllEnemy();

		// モデルの削除
		MV1DeleteModel(originModelHandle);
	}

	void DeleteAllEnemy() {
		while(unuseArray.size() != 0) {
			Enemy* e = unuseArray.front();
			unuseArray.remove(e);
			if (e == nullptr) continue;
			delete e;
			e = nullptr;
		}

		unuseArray.clear();
	}

	inline void SetCreateFunc(std::function<Enemy* ()> _createFunc) { CreateEnemy = _createFunc; }
};

class EnemyManager : public Singleton<EnemyManager> {
private:

	// 敵の一括管理
	std::list<Enemy*> pEnemyArray;
	// unuse配列の一括管理
	std::vector<EnemyUtility*> pUnuseEnemiesArray;
	std::list<Enemy*> unuseGoblinArray;
	std::list<Enemy*> unuseSpiderArray;
	std::list<Enemy*> unuseWolfArray;
	std::list<Enemy*> unuseTrollArray;
	std::list<Enemy*> unuseZombieArray;
	std::list<Enemy*> unuseHellHoundArray;
	std::list<Enemy*> unuseOugerArray;
	std::list<Enemy*> unuseKetbleperzArray;
	std::list<Enemy*> unuseDurahanArray;
	std::list<Enemy*> unuseHobGoblinArray;
	// pEnemyArrayからデータを削除するための配列
	std::list<Enemy*> unuseEnemy;

	const std::string audioFilePath = "Res/Audio/SE/Enemy/";

	int killEnemyCount = 0;

private:
	EnemyUtility* goblin;
	EnemyUtility* spider;
	EnemyUtility* wolf;
	EnemyUtility* troll;
	EnemyUtility* zombie;
	EnemyUtility* hellhound;
	EnemyUtility* ouger;
	EnemyUtility* ketbleperz;
	EnemyUtility* durahan;
	EnemyUtility* hobgoblin;

	std::unordered_map<int, EnemyData> enemyTable;

public:
	EnemyManager();
	~EnemyManager();

	void Start();
	void Update();
	void Render();

	void SpawnEnemy(EnemyType type, VECTOR pos);
	void SpawnBoss(EnemyType type, VECTOR pos);
	Enemy* UseEnemy(EnemyType type);
	void UnuseEnemy(Enemy* enemy);
	void UnuseAllEnemy();

	void DeleteEnemy(Enemy* enemy);
	void DeleteAllEnemy();

	// セーブ / ロード
	void SaveTo(BinaryWriter& w);
	void LoadFrom(BinaryReader& r, uint32_t ver);
	void AddKillCount() { killEnemyCount++; }
	int GetKillCount() { return killEnemyCount; }

	void LoadEnemyData();

	EnemyData* GetEnemyData(int id);
};

