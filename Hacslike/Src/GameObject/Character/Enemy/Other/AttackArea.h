#pragma once
#include <functional>
#include "../../../Character/Character.h"
#include "../../../../Manager/TimeManager.h"

//template <class... R>
struct AttackAreaObject : public GameObject {
	float maxRadius;
	float radius;
	float limitTime;
	float time;
	float speed;
	std::function<void()> spawnColliderFunction;
	bool useFunc;

	AttackAreaObject(float _rad, float _time,VECTOR pos,float _speed, std::function<void()> spawnFunc)
		:maxRadius(_rad)
		,radius(0)
		,limitTime(_time)
		,spawnColliderFunction(spawnFunc)
		,time(0)
		,speed(0.3f)
		,useFunc(false)
	{
		SetPosition(pos);
	}
public:
	void Start(){}

	void Update() {
		if (useFunc) return;

		GameObject::Update();

		if (time >= limitTime) {
			spawnColliderFunction();
			useFunc = true;
		}
		else {
			time += speed;
			float progress = time / limitTime;
			radius = progress * maxRadius;
		}
	} 

	void Render() {
		if (useFunc) return;
		////DrawSphere3D(position, maxRadius, 16, red, red, false);


		////DrawSphere3D(position, radius, 16, c, c, false);

		SetWriteZBuffer3D(FALSE);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
		//SetDrawBlendMode(DX_BLENDMODE_ADD, 255);

		unsigned int c = GetColor(0, 0, 0);

		if (time / limitTime >= 0.7)
			c = red;
		else if (time / limitTime >= 0.4)
			c = yellow;
		else
			c = green;

		// â~êçÇÃí∏ì_Ç∆â∫ñ ÇÃçÇÇ≥ÇÃí≤êÆ
		VECTOR top = VGet(position.x, position.y + 0.15f, position.z);
		VECTOR bottom = VGet(position.x, position.y + 0.1f, position.z);

		// äOòg
		DrawCone3D(top, bottom, maxRadius, 32, red, red, TRUE);

		SetUseLighting(FALSE);
		// ì‡ë§
		DrawCone3D(top, bottom, radius, 32, GetColor(255, 50, 50), GetColor(255, 50, 50), TRUE);

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		SetUseLighting(TRUE);
		SetWriteZBuffer3D(TRUE);

	}
};

class AttackArea{

	Character* owner;

	std::list<AttackAreaObject*> areaObjectArray;

public:
	AttackArea() = default;
	~AttackArea() = default;

	void CreateArea(float _rad, float _time, VECTOR pos,float speed, std::function<void()> spawnFunc);

	void Update();
	void Render();
	void SetOwner(Character* chara) { owner = chara; }
	void DeleteObject();
};
