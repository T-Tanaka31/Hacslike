#include <vector>
#include <DxLib.h>
#include "../Definition.h"

#pragma once
class LightManager {
private:
	int playerLightHandle;			//	プレイヤー用ライト
	std::vector<int> effectLights;	//	その他、魔法やギミック用
};

