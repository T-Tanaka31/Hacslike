#include "AttackArea.h"

void AttackArea::CreateArea(float _rad, float _time, VECTOR pos,float speed, std::function<void()> spawnFunc) {
	areaObjectArray.push_back(new AttackAreaObject(_rad, _time, pos,speed, spawnFunc));
}

void AttackArea::Update() {
	if (areaObjectArray.size() == 0 || owner == nullptr) return;

	std::list<AttackAreaObject*> deleteArray;

	for (auto a : areaObjectArray) {
		if (a == nullptr) continue;

		a->Update();

		if (a->useFunc) {
			deleteArray.push_back(a);
		}
	}

	for (auto d : deleteArray) {
		areaObjectArray.remove(d);
		delete d;
	}

}

void AttackArea::Render() {
	if (areaObjectArray.size() == 0) return;


	for (auto a : areaObjectArray) {
		if (a == nullptr) continue;

		a->Render();
	}
}

void AttackArea::DeleteObject() {
	if (areaObjectArray.size() == 0) return;

	for (auto o : areaObjectArray) {
		delete o;
	}

	areaObjectArray.clear();
}
