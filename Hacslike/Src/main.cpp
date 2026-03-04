#include <iostream>
#include <Dxlib.h>
#include <random>
#include "Definition.h"
#include "Manager/TimeManager.h"
#include "Manager/InputManager.h"
#include "Manager/SceneManager.h"
#include "Manager/StageManager.h"
#include "Manager/FadeManager.h"
#include "Manager/AudioManager.h"
#include"Manager/SkillManager.h"
#include"Manager/ItemDropManager.h"
#include"Manager/FontManager.h"
#include"Manager/EffectManager.h"
#include <EffekseerForDXLib.h>
#include "GameObject/Coin/Coin.h"
#include "Manager/AudioManager.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
#pragma region // DxLibの初期化処理　触るべからず
	// ログファイルを残さない
#if _DEBUG
	SetOutApplicationLogValidFlag(TRUE);
#else
	SetOutApplicationLogValidFlag(FALSE);
#endif
	//SetWindowStyleMode(2);
	// ウィンドウのサイズを変更する
	SetGraphMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32, FPS);
	// 起動時のウィンドウのモードの設定
#if _DEBUG
	ChangeWindowMode(TRUE);	// TRUE : ウィンドウモード FALSE : フルスクリーン
#else 
	ChangeWindowMode(FALSE);	// TRUE : ウィンドウモード FALSE : フルスクリーン
#endif



	SetMainWindowText("Hacslike");

	// 背景色の設定
#if _DEBUG
	SetBackgroundColor(196, 196, 196);
#else 
	SetBackgroundColor(0, 0, 0);
#endif

	// Dxlibの初期化
	if (DxLib_Init() == -1)
		return 0;

	int i = SetWindowIconID(333);
	// Effekseerの初期化
	if (Effekseer_Init(8000) == -1) {
		DxLib_End();
		return 0;
	}
	// 他の初期化前に登録しておく
	ItemFactory::Instance().InitializeDefaultItems();

	// 描画する先を設定する 裏画面に変更する
	SetDrawScreen(DX_SCREEN_BACK);

	// 図形描画のZバッファの有効化
	{
		// Zバッファを使用するかどうか
		SetUseZBuffer3D(TRUE);	// default : FALSE
		// Zバッファに書き込みを行うか
		SetWriteZBuffer3D(TRUE); // default : FALSE
	}

	// ライティング
	{
		// ライトの計算をどうするか
		SetUseLighting(TRUE); // default : TRUE
		// 標準ライトを使用するかどうか
		SetLightEnable(TRUE);	// default : TRUE
		// グローバル環境光の設定
		SetGlobalAmbientLight(GetColorF(1, 0, 0, 0));
		//// 反射光の設定  Diffuse
		//SetLightDifColor(GetColorF(1, 0, 0, 0));
		//// 鏡面反射光の設定　Specular
		//SetLightSpcColor(GetColorF(1, 0, 0.25f, 1));
		//// 環境光の設定　Ambient
		//SetLightAmbColor(GetColorF(1, 1, 1, 1));


	}
#pragma endregion

	// 乱数調節(ガチ)
	std::random_device rd;
	std::mt19937_64 mt(rd());
	SRand(mt());

	//マウスの非表示
	SetMouseDispFlag(FALSE);
	SceneManager::GetInstance().ChangeScene(SceneType::Title);
	// ゲームのメインループ
	while (true) {
		if (SceneManager::GetInstance().GetEnd() == true || ProcessMessage() == -1) {
			break;
		}
		//DxLibのカメラとEffekseerのカメラを同期する
		Effekseer_Sync3DSetting();
		// 更新処理
		SceneManager::GetInstance().Update();
		TimeManager::GetInstance().Update();
		InputManager::GetInstance().Update();
		AudioManager::GetInstance().Update();

		// 画面をクリアする
		ClearDrawScreen();

		// 描画処理
		SceneManager::GetInstance().Render();

		// エスケープキーでウィンドウを閉じる
		//if (InputManager::GetInstance().IsKeyDown(KEY_INPUT_ESCAPE))
		//	break;

		// 裏画面と表画面を切り替える
		ScreenFlip();

		// 処理が速すぎたら待つ
		while (1) {
			if (GetNowCount() - TimeManager::GetInstance().GetCurrent() >= 1000 / FPS)
				break;
		}

	}

	Coin::DestroyInstance();
	AudioManager::GetInstance().DeleteData();
	EffectManager::GetInstance().DeleteData();
	EnemyManager::GetInstance().DeleteAllEnemy();
	FontManager::GetInstance().DeleteFont();
	SceneManager::GetInstance().DeleteScene();
	StageManager::GetInstance().DeleteStage();

	Effkseer_End();

	// DxLibの終了
	DxLib_End();
	return 0;
}