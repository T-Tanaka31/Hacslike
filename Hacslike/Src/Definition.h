#pragma once
//	=================================================================
//		ヘッダーをインクルードする場所
//	=================================================================
#include <DxLib.h>
#include <string>
#include "Manager/FontManager.h"



//	=================================================================
//		定数を定義する場所
//	=================================================================
#define WINDOW_WIDTH			(1920)	//	ウィンドウの横幅
#define WINDOW_HEIGHT			(1080)	//	ウィンドウの縦幅

#define FPS						(60)	//	FPS

//	引数付きマクロ定義 変換マクロ
#define Deg2Rad(x) ( x * (DX_PI_F / 180.0f )) //　デグリー角→ラジアン角
#define Rad2Deg(x) ( x * (180.0f / DX_PI_F )) //  ラジアン角→デグリー角

// ワールドベクトル
#define VRight	 (VGet(1.0f,0,0))
#define VLeft	 (VGet(-1.0f,0,0))
#define VUp		 (VGet(0,1.0f,0))
#define VDown	 (VGet(0,-1.0f,0))
#define VForward (VGet(0,0,1.0f))
#define VBack    (VGet(0,0,-1.0f))

// ゼロベクトル
#define VZero   (VGet(0,0,0))
#define VOne   (VGet(1,1,1))
#define VMinus (VGet(-1,-1,-1))

#define NoSpawn (VGet(-1,0,-1))

// DxLibの読み込みエラ-
#define INVALID (-1)

#define PLAYER_MODEL_HANDLE (MV1LoadModel("Res/PlayerModel/maya_Player.mv1"))



// ステージ関連
#define mapWidth_Large (64)	// マップの横サイズ
#define mapHeight_Large (64)	// マップの縦サイズ
#define mapWidth_Middle (48)	// マップの横サイズ
#define mapHeight_Middle (48)	// マップの縦サイズ
#define mapWidth_Small (32)	// マップの横サイズ
#define mapHeight_Small (32)	// マップの縦サイズ
#define offsetWall (2)	// 壁から離す距離 
#define offsetS (1)		// 調節用
#define CellSize (200)	// 1マスの大きさ 
#define CellCorrection (CellSize / 2) // 壁判定用の補正値
#define ExchangeCellSize(x) (std::floor(CellSize / x))
#define MAP_SIZE (4)

#define NextCellEnd (300) // 隣のセルの端っこ
#define CellEnd (100) // セルの端っこ

#define RoomMax_Small (6)
#define RoomMax_Middle (10)
#define RoomMax_Large (12)

#define EnemyMax (50)

#define MainFont (FontManager::GetInstance().UseFontHandle("MainFont"))
#define MainFont_Bold (FontManager::GetInstance().UseFontHandle("MainFont_Bold"))

//	武器関連
#define ATTACK_SPEED_NUM (4)
#define DEFAULT_ATTACK_SPEED std::array<float, ATTACK_SPEED_NUM>{1.0f, 1.0f, 1.0f, 1.0f}
#define COL_LENGTH_NUM (3)
#define DEFAULT_COL_LENGTH std::array<float, COL_LENGTH_NUM>{30.0f, 40.0f, 0.0f}
#define COL_RADIUS_NUM (3)
#define DEFAULT_COL_RADIUS std::array<float, COL_RADIUS_NUM>{80.0f, 110.0f, 150.0f}

//	プレイヤー関連
#define MAX_LV (42)

//	=================================================================
//		定数を定義する場所
//	=================================================================
//	色
const unsigned int red = GetColor(255, 0, 0);
const unsigned int green = GetColor(0, 255, 0);
const unsigned int blue = GetColor(0, 0, 255);
const unsigned int magenta = GetColor(255, 0, 255);
const unsigned int cyan = GetColor(0, 255, 255);
const unsigned int yellow = GetColor(255, 255, 0);
const unsigned int white = GetColor(255, 255, 255);
const unsigned int gray = GetColor(120, 120, 120);
const unsigned int black = GetColor(0, 0, 0);
const unsigned int darkGray = GetColor(24, 24, 24);//黒よりのグレー
const unsigned int skyblue = GetColor(144, 196, 255); // 薄い青
const unsigned int palegreen = GetColor(144, 238, 144); // 薄い緑
const unsigned int orange = GetColor(255, 165, 0);

enum class SceneType {
	Title,
	Game,
	Sekino,
	Max,
};

enum class FadeState {
	FadeIn = -1,
	FadeOut = 1,
	FadeEnd,
};

enum ObjectType {
	Room,
	Wall,
	Road,
	Stair,
	Chest
};

enum WeaponType {
	Sward,
	Big,
	Blunt,
	Gun,
};

enum EnemyType {
	Goblin,
	Spider,
	Wolf,
	Troll,
	Zombie,
	HellHound,
	Ouger,
	Ketbleperz,
	Durahan,
	HobGoblin,
	eMax
};

enum RoomStatus {	// 部屋の配列ステータス
	x,	// マップX座標
	y,	// マップY座標
	w,	// 分割した幅
	h,	// 分割した高さ

	rx,	// 部屋の生成位置
	ry,	// 部屋の生成位置
	rw,	// 部屋の幅
	rh,	// 部屋の高さ
	Max,
};

enum MenuType {
	menuInventory = 0,
	menuArtifact = 1,
	menuSave = 2,
};
