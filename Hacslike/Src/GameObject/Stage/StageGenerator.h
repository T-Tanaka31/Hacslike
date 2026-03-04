#pragma once
#include "StageCell.h"
#include <random>
#include <list>
#include "../../CommonModule.h"
#include"../../Manager/SaveManager.h"
#include <msgpack.hpp>


class StageCell;

// forward
class SaveObject;
class StartTreasureChest;
class EnhancementStone;
class TitleReturner;
class ItemShop;

namespace msgpack {
	MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
		namespace adaptor {
			template<>
			struct convert<VECTOR> {
				msgpack::object const& operator()(msgpack::object const& o, VECTOR& v) const {
					if (o.type != msgpack::type::ARRAY || o.via.array.size != 3) {
						throw msgpack::type_error();
					}
					v.x = o.via.array.ptr[0].as<float>();
					v.y = o.via.array.ptr[1].as<float>();
					v.z = o.via.array.ptr[2].as<float>();
					return o;
				}
			};

			template<>
			struct pack<VECTOR> {
				template <typename Stream>
				msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, VECTOR const& v) const {
					o.pack_array(3);
					o.pack(v.x);
					o.pack(v.y);
					o.pack(v.z);
					return o;
				}
			};
		}
	}
}

struct StageData {
	int id;
	int stageData[mapWidth_Large][mapHeight_Large];
	VECTOR playerSpawnPos;
	VECTOR saveObjectPos;
	VECTOR chestObjectPos;
	VECTOR enhancementStonePos;
	VECTOR itemShopPos;
	VECTOR bossSpawnPos;
	VECTOR stairSpawnPos;
	VECTOR returnerSpawnPos;
	std::vector<VECTOR> closePosArray;
	int bossType;
	std::string bgmName;

	//	Msgpack用
	MSGPACK_DEFINE(
		id,
		stageData,
		playerSpawnPos,
		saveObjectPos,
		chestObjectPos,
		enhancementStonePos,
		itemShopPos,
		bossSpawnPos,
		stairSpawnPos,
		returnerSpawnPos,
		closePosArray,
		bossType,
		bgmName
	);
};

class StageGenerator {
public:
	int groundModel = -1;
	int wallModel = -1;
	int roadModel = -1;
	int stairModel = -1;

	VECTOR defaultPos = VGet(0, 0, 0);

	int mapWidth = 0;
	int mapHeight = 0;
	int roomMinNum = 3;	// 部屋の最小数
	int roomNum;	// 部屋の数
	int parentNum;	// 分割する部屋番号
	int maxArea;		// 最大面積
	int roomCount;	// 部屋カウント
	int line;		// 分割点
	int mapSize ;
	VECTOR mapOffset;

	StageData stage;

	std::unordered_map<int, StageData> stageCache;

public:
	
	int map[mapWidth_Large][mapHeight_Large];	// マップ管理配列
	bool mapObjects[mapWidth_Large][mapHeight_Large];	// マップ上のオブジェクトの配置
	int roomStatus[RoomStatus::Max][RoomMax_Large];	// 部屋の配列ステータス
	bool stageMap[mapWidth_Large][mapHeight_Large];

	std::list<StageCell*> cells ;
	std::list<StageCell*> unuseWall;
	std::list<StageCell*> unuseRoad;
	std::list<StageCell*> unuseRoom;
	StageCell* unuseStair;
	StageCell* useStair;
	int gr;
	SaveObject* pSaveObject = nullptr;
	StartTreasureChest* pChest = nullptr;
	EnhancementStone* pStone = nullptr;
	TitleReturner* pReturner = nullptr;
	ItemShop* pItemShop = nullptr;

public:
	StageGenerator();
	~StageGenerator();

	void Update();
	void Render();

	// ステージの初期化
	void ClearStage();
	// ステージデータのランダム生成
	void GenerateStageData();
	// ステージデータの読み込み生成
	void LoadStageData(int stageID);
	// ステージデータのロード
	void LoadStageMeta(int stageID);
	// ステージのオブジェクト生成
	void GenerateStageObject();
	// 分割点2点のうち大きいほうを分割する
	bool SplitPoint(int x, int y);
	// 八方向チェック
	bool CheckEightDir(int x, int y);
	// オブジェクトのランダム設置
	void SetGameObjectRandomPos(GameObject* obj);
	// オブジェクトの設置
	void SetGameObject(GameObject* _obj, VECTOR _pos);

	void AppearStair();
	void SpawnReturnCircle();

	// ステージオブジェクトのプーリング
	StageCell* UseObject(ObjectType type);
	void UnuseObject(StageCell*& cell);

	StageCell* GetStageObjectFromPos(VECTOR _deltaPos);

	void DrawStairMap();

	// マップの描画
	void DrawMap();
	// 壁の透過をするかどうか
	bool TransparencyWall(StageCell* cell);

	// ステージオブジェクトのテクスチャ張替
	void ChangeObjectTexture(int num,ObjectType changeObject);
	// 座標から今いる部屋番号を返す
	int GetNowRoomNum(VECTOR pos);
	int GetNowRoomNum(int x,int z);
	// ランダムな部屋のランダムな座標を返す
	VECTOR GetRandomRoomRandomPos();
	// 読み込んだステージデータの取得
	inline StageData GetStageData() { return stage; }
	
	// ボス部屋に入ったら退路を塞ぐ
	void CloseRoom();
	void OpenRoom();

	// シリアライズ / デシリアライズ
	void SaveTo(BinaryWriter& w);
	void LoadFrom(BinaryReader& r, uint32_t ver);
};

