#include "EnemyManager.h"
#include "../GameObject/Character/Enemy/Goblin/EnemyGoblin.h"
#include "../GameObject/Character/Enemy/Spider/EnemySpider.h"
#include "../GameObject/Character/Enemy/Wolf/EnemyWolf.h"
#include "../GameObject/Character/Enemy/Troll/EnemyTroll.h"
#include "../GameObject/Character/Enemy/Zombie/EnemyZombie.h"
#include "../GameObject/Character/Enemy/HellHound/EnemyHellHound.h"
#include "../GameObject/Character/Enemy/Ouger/EnemyOuger.h"
#include "../GameObject/Character/Enemy/Ketbleperz/EnemyKetbleperz.h"
#include "../GameObject/Character/Enemy/Durahan/EnemyDurahan.h"
#include "../GameObject/Character/Enemy/HobGoblin/EnemyHobGoblin.h"
#include "../GameObject/Character/Enemy/Enemy.h"
#include "../GameObject/Character/Enemy/Boss/BossBase.h"
#include "../GameObject/Character/Enemy/Boss/Goblin/BossGoblin.h"
#include "../GameObject/Character/Enemy/Boss/HellHound/BossHellHound.h"
#include "../GameObject/Character/Enemy/Boss/Ouger/BossOuger.h"
#include "../GameObject/Character/Enemy/Boss/Ketbleperz/BossKetbleperz.h"
#include "../GameObject/Character/Enemy/Boss/Durahan/BossDurahan.h"
#include "../Manager/AudioManager.h"
#include "../Save/SaveIO.h"
#include <filesystem>

EnemyManager::EnemyManager() {
	LoadEnemyData();
	Start();
}

EnemyManager::~EnemyManager() {
	DeleteAllEnemy();
}

void EnemyManager::Start() {
	// 2. 音声リソースのロード
	AudioManager* manager = &AudioManager::GetInstance();
	manager->Load(audioFilePath + "SwordSwing.mp3", "SwordSwing", false);
	manager->Load(audioFilePath + "Impact.mp3", "Impact", false);
	manager->Load(audioFilePath + "Dawn.mp3", "Dawn", false);
	manager->Load(audioFilePath + "SpiderAttack.mp3", "SpiderAttack", false);
	manager->Load(audioFilePath + "Bite1.mp3", "Bite1", false);
	manager->Load(audioFilePath + "Bite2.mp3", "Bite2", false);
	manager->Load(audioFilePath + "Axe.mp3", "Axe", false);
	manager->Load(audioFilePath + "Punch1.mp3", "Punch1", false);
	manager->Load(audioFilePath + "Punch2.mp3", "Punch2", false);
	manager->Load(audioFilePath + "HeadBang.mp3", "HeadBang", false);

	// 3. 各エネミーの生成用ユーティリティを初期化
	goblin = new EnemyUtility([this]() {return new EnemyGoblin(MV1DuplicateModel(goblin->originModelHandle)); }, MV1LoadModel("Res/Model/Enemy/Goblin/model.mv1"));
	spider = new EnemyUtility([this]() {return new EnemySpider(MV1DuplicateModel(spider->originModelHandle)); }, MV1LoadModel("Res/Model/Enemy/Spider/model.mv1"));
	wolf = new EnemyUtility([this]() {return new EnemyWolf(MV1DuplicateModel(wolf->originModelHandle)); }, MV1LoadModel("Res/Model/Enemy/Wolf/model.mv1"));
	troll = new EnemyUtility([this]() {return new EnemyTroll(MV1DuplicateModel(troll->originModelHandle)); }, MV1LoadModel("Res/Model/Enemy/Troll/model.mv1"));
	zombie = new EnemyUtility([this]() {return new EnemyZombie(MV1DuplicateModel(zombie->originModelHandle)); }, MV1LoadModel("Res/Model/Enemy/Zombie/model.mv1"));
	hellhound = new EnemyUtility([this]() {return new EnemyHellHound(MV1DuplicateModel(hellhound->originModelHandle)); }, MV1LoadModel("Res/Model/Enemy/HellHound/model.mv1"));
	ouger = new EnemyUtility([this]() {return new EnemyOuger(MV1DuplicateModel(ouger->originModelHandle)); }, MV1LoadModel("Res/Model/Enemy/Ouger/model.mv1"));
	ketbleperz = new EnemyUtility([this]() {return new EnemyKetbleperz(MV1DuplicateModel(ketbleperz->originModelHandle)); }, MV1LoadModel("Res/Model/Enemy/Ketbleperz/model.mv1"));
	durahan = new EnemyUtility([this]() {return new EnemyDurahan(MV1DuplicateModel(durahan->originModelHandle)); }, MV1LoadModel("Res/Model/Enemy/Durahan/model.mv1"));
	hobgoblin = new EnemyUtility([this]() {return new EnemyHobGoblin(MV1DuplicateModel(hobgoblin->originModelHandle)); }, MV1LoadModel("Res/Model/Enemy/HobGoblin/model.mv1"));

	pUnuseEnemiesArray.push_back(goblin);
	pUnuseEnemiesArray.push_back(spider);
	pUnuseEnemiesArray.push_back(wolf);
	pUnuseEnemiesArray.push_back(troll);
	pUnuseEnemiesArray.push_back(zombie);
	pUnuseEnemiesArray.push_back(hellhound);
	pUnuseEnemiesArray.push_back(ouger);
	pUnuseEnemiesArray.push_back(ketbleperz);
	pUnuseEnemiesArray.push_back(durahan);
	pUnuseEnemiesArray.push_back(hobgoblin);

	EffectManager::GetInstance().Load("Res/Effect/Death.efk", "Dead", 10.0f);
}

void EnemyManager::Update() {
	for (auto e : pEnemyArray) {
		if (e == nullptr || !e->IsVisible()) continue;
		e->Update();
	}

	// 未使用リストへの移動処理
	for (auto it = unuseEnemy.begin(); it != unuseEnemy.end(); ) {
		Enemy* e = *it;
		pEnemyArray.remove(e);
		it = unuseEnemy.erase(it);
	}
}

void EnemyManager::Render() {
	for (auto e : pEnemyArray) {
		if (e == nullptr) continue;
		e->Render();
	}
}

void EnemyManager::SpawnEnemy(EnemyType type, VECTOR pos) {
	// 1. インスタンス取得
	Enemy* e = UseEnemy(type);
	if (!e) return;

	// 2. データを注入（Setupより先に呼ぶことで、データに基づいた初期化を可能にする）
	EnemyData* data = GetEnemyData(static_cast<int>(type));
	if (data) {
		e->InitializeData(data);
	}
	else {
		OutputDebugStringA("EnemyManager: Warning! No Data for current EnemyType.\n");
	}

	pEnemyArray.push_back(e);
	e->SetPosition(pos);

	// 3. データがセットされた状態で最終セットアップ
	e->Setup();
}

void EnemyManager::SpawnBoss(EnemyType type, VECTOR pos) {
	BossBase* boss = nullptr;
	switch (type) {
	case Goblin:
		boss = new BossGoblin(pos);
		break;
	case HellHound:
		boss = new BossHellHound(pos);
		break;
	case Ouger:
		boss = new BossOuger(pos);
		break;
	case Ketbleperz:
		boss = new BossKetbleperz(pos);
		break;
	case Durahan:
		boss = new BossDurahan(pos);
		break;
	default:
		return;
	}
	if (boss) {
		boss->SetPosition(VGet(pos.x * CellSize, 0, pos.z * CellSize));
		boss->SetVisible(true);
		boss->SetType(type);
		pEnemyArray.push_back(boss);
	}
}

Enemy* EnemyManager::UseEnemy(EnemyType type) {
	int index = static_cast<int>(type);

	// ★重要：範囲外アクセスをガード（これが Assertion Failed の直接の対策）
	if (index < 0 || index >= (int)pUnuseEnemiesArray.size()) {
		OutputDebugStringA("EnemyManager: UseEnemy Error - Index out of range for pUnuseEnemiesArray.\n");
		return nullptr;
	}

	Enemy* e = nullptr;
	if (pUnuseEnemiesArray[index]->unuseArray.size() == 0) {
		e = pUnuseEnemiesArray[index]->CreateEnemy();
	}
	else {
		e = pUnuseEnemiesArray[index]->unuseArray.front();
		pUnuseEnemiesArray[index]->unuseArray.pop_front();
	}

	if (e) {
		e->SetType(type);
		// ここでは Setup() は呼ばない。SpawnEnemy でデータ注入後に呼ぶ。
	}
	return e;
}

void EnemyManager::UnuseEnemy(Enemy* enemy) {
	if (!enemy) return;
	enemy->Teardown();

	int typeIdx = static_cast<int>(enemy->GetType());
	if (typeIdx >= 0 && typeIdx < (int)pUnuseEnemiesArray.size()) {
		pUnuseEnemiesArray[typeIdx]->unuseArray.push_back(enemy);
	}

	unuseEnemy.push_back(enemy);
}

void EnemyManager::UnuseAllEnemy() {
	while (pEnemyArray.size() > 0) {
		Enemy* e = pEnemyArray.front();
		if (e->IsBoss()) {
			pEnemyArray.remove(e);
			DeleteEnemy(e);
		}
		else {
			e->Teardown();
			int typeIdx = static_cast<int>(e->GetType());
			if (typeIdx >= 0 && typeIdx < (int)pUnuseEnemiesArray.size()) {
				pUnuseEnemiesArray[typeIdx]->unuseArray.push_back(e);
			}
			pEnemyArray.remove(e);
		}
	}
}

void EnemyManager::DeleteEnemy(Enemy* enemy) {
	CollisionManager::GetInstance().UnRegister(enemy->GetCollider());
	unuseEnemy.push_back(enemy);
	delete enemy;
	enemy = nullptr;
}

void EnemyManager::DeleteAllEnemy() {
	for (auto e : pEnemyArray) {
		pEnemyArray.remove(e);
		if (e == nullptr) continue;
		delete e;
		e = nullptr;
	}

	pEnemyArray.clear();

	for (auto list : pUnuseEnemiesArray) {
		list->DeleteAllEnemy();
	}
}

void EnemyManager::SaveTo(BinaryWriter& w) {
	uint32_t count = static_cast<uint32_t>(pEnemyArray.size());
	w.WritePOD(count);
	for (auto e : pEnemyArray) {
		uint32_t type = static_cast<uint32_t>(e->GetType());
		w.WritePOD(type);

		VECTOR pos = e->GetPosition();
		w.WritePOD(pos.x);
		w.WritePOD(pos.y);
		w.WritePOD(pos.z);

		float rotY = e->GetRotationY();
		w.WritePOD(rotY);

		float hp = e->GetHP();
		w.WritePOD(hp);

		uint8_t dead = e->IsDead() ? 1u : 0u;
		w.WritePOD(dead);
	}
}

void EnemyManager::LoadFrom(BinaryReader& r, uint32_t ver) {
	UnuseAllEnemy();
	//DeleteAllEnemy();

	uint32_t count = 0;
	r.ReadPOD(count);
	for (uint32_t i = 0; i < count; ++i) {
		uint32_t type;
		r.ReadPOD(type);

		float x, y, z;
		r.ReadPOD(x); r.ReadPOD(y); r.ReadPOD(z);

		float rotY;
		r.ReadPOD(rotY);

		float hp;
		r.ReadPOD(hp);

		uint8_t dead;
		r.ReadPOD(dead);

		Enemy* e = UseEnemy(static_cast<EnemyType>(type));
		if (!e) continue;

		e->SetPosition(VGet(x, y, z));
		e->SetRotationY(rotY);
		e->SetHP(hp);

		if (dead) {
			e->SetDeadState(true);
			e->SetVisible(false);
			CollisionManager::GetInstance().UnRegister(e->GetCollider());
		}
	}
}

void EnemyManager::LoadEnemyData() {
	enemyTable.clear();

	const std::string jsonPath = "Src/Data/EnemyData.json";
	const std::string datPath = "Src/Data/EnemyData.dat";

	OutputDebugStringA("=== EnemyManager::LoadEnemyData START ===\n");

	// フォルダ作成
	try {
		std::filesystem::create_directories("Src/Data");
		OutputDebugStringA("[OK] Directory created or already exists.\n");
	}
	catch (const std::exception& e) {
		OutputDebugStringA("[ERROR] Folder creation failed: ");
		OutputDebugStringA(e.what());
		OutputDebugStringA("\n");
	}

	std::ifstream jsonFile(jsonPath);
	if (jsonFile.is_open()) {
		OutputDebugStringA("[OK] JSON file opened successfully.\n");

		nlohmann::json data;
		try {
			jsonFile >> data;
			OutputDebugStringA("[OK] JSON parsed successfully.\n");

			int count = 0;
			for (auto& e : data) {
				if (!e.contains("id")) {
					OutputDebugStringA("[WARN] Skipping entry without 'id' field.\n");
					continue;
				}

				EnemyData tempData;
				tempData.id = e["id"];
				tempData.name = e.value("name", "Unknown");
				tempData.typeID = e.value("typeID", 0);
				tempData.mPath = e.value("mPath", "");
				tempData.hp = e.value("hp", 100);
				tempData.atk = e.value("atk", 10);
				tempData.def = e.value("def", 5);
				tempData.exp = e.value("exp", 0);
				tempData.spd = static_cast<int>(e.value("spd", 0));
				tempData.cRate = e.value("cRate", 0);
				tempData.cDamageRate = e.value("cDamageRate", 0);
				tempData.rAngle = e.value("rAngle", 0);
				tempData.rCount = e.value("rCount", 0);
				tempData.rLenght = e.value("rLenght", 0);
				enemyTable[tempData.id] = tempData;
				count++;
			}

			char debugMsg[256];
			sprintf_s(debugMsg, "[OK] Loaded %d enemy entries from JSON.\n", count);
			OutputDebugStringA(debugMsg);

			// ✅ DAT 保存処理
			std::ofstream outFile(datPath, std::ios::binary);
			if (outFile.is_open()) {
				OutputDebugStringA("[OK] DAT file opened for writing.\n");

				try {
					msgpack::pack(outFile, enemyTable);
					outFile.close();
					OutputDebugStringA("[SUCCESS] DAT file saved successfully!\n");
				}
				catch (const std::exception& e) {
					OutputDebugStringA("[ERROR] msgpack::pack failed: ");
					OutputDebugStringA(e.what());
					OutputDebugStringA("\n");
					outFile.close();
				}
			}
			else {
				OutputDebugStringA("[ERROR] Failed to open DAT file for writing.\n");
			}
		}
		catch (const nlohmann::json::exception& e) {
			OutputDebugStringA("[ERROR] JSON parsing failed: ");
			OutputDebugStringA(e.what());
			OutputDebugStringA("\n");
		}
		catch (const std::exception& e) {
			OutputDebugStringA("[ERROR] Exception during JSON processing: ");
			OutputDebugStringA(e.what());
			OutputDebugStringA("\n");
		}
		jsonFile.close();
	}
	else {
		OutputDebugStringA("[ERROR] JSON file NOT found!\n");

		// ==========================================
		// 【製品モード】バイナリ(.dat)から読み込み
		// ==========================================
		
		std::ifstream datFile(datPath, std::ios::binary);
		if (!datFile.is_open()) {
			OutputDebugStringA("EnemyManager: CRITICAL - DAT file missing!\n");
			return;
		}

		datFile.seekg(0, std::ios::end);
		size_t fileSize = static_cast<size_t>(datFile.tellg());
		datFile.seekg(0, std::ios::beg);

		if (fileSize == 0) {
			OutputDebugStringA("EnemyManager: Error - DAT file is empty!\n");
			datFile.close();
			return;
		}

		std::vector<char> buffer(fileSize);
		datFile.read(buffer.data(), fileSize);
		datFile.close();

		try {
			msgpack::object_handle oh = msgpack::unpack(buffer.data(), buffer.size());
			msgpack::object obj = oh.get();

			enemyTable.clear();
			obj.convert(enemyTable);

			char debugMsg[128];
			sprintf_s(debugMsg, "EnemyManager: DAT loaded successfully. (Size: %zu)\n", enemyTable.size());
			OutputDebugStringA(debugMsg);
		}
		catch (const std::exception& e) {
			OutputDebugStringA("EnemyManager: DAT unpack FAILED! (Structure mismatch or Corrupted data)\n");
			OutputDebugStringA("Reason: ");
			OutputDebugStringA(e.what());
			OutputDebugStringA("\n");
		}
		catch (...) {
			OutputDebugStringA("EnemyManager: DAT unpack FAILED (Unknown Error).\n");
		}
		
	}

	OutputDebugStringA("=== EnemyManager::LoadEnemyData END ===\n");
}

EnemyData* EnemyManager::GetEnemyData(int id) {
	if (enemyTable.count(id) == 0) return nullptr;
	return &enemyTable[id];
}