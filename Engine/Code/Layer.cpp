#include "Layer.h"
#include "CollisionManager.h"

Layer::Layer() {}
Layer::~Layer() {}

HRESULT		Layer::Ready_Layer() {
	_isTimeSlow = false;
	return S_OK;
}
INT			Layer::Update_Layer(const FLOAT& _DT) {
	if (!_isTimeSlow) {
		for (auto iter = GameObjectList.begin(); iter != GameObjectList.end();) {
			if ((*iter)->Get_ObjectDead())
			{
				CollisionManager::GetInstance()->Delete_ColliderObject((*iter));
				++iter;
				continue;
			}
			
			int ObjectResult = (*iter)->Update_GameObject(_DT);
			if (ObjectResult == -1) (*iter)->Set_ObjectDead(true);
			++iter;
			//if ((*iter)->Get_ObjectDead() == TRUE || ObjectResult == -1) {
			//	CollisionManager::GetInstance()->Delete_ColliderObject((*iter));
			//	GameObject* OBJ = *iter;
			//	iter = GameObjectList.erase(iter);
			//	Safe_Release(OBJ);
			//	continue;
			//}
			//else { ++iter; }
			//if (_isTimeSlow) break;
		}
	}
	else {
		for (auto iter = GameObjectList.begin(); iter != GameObjectList.end();) {
			int ObjectResult = 0;
			if ((*iter)->Get_ObjectTag() == L"Player" || (*iter)->Get_ObjectTag() == L"PlayerArrow"
				|| (*iter)->Get_ObjectTag() == L"NPC_TIMESLOW" || (*iter)->Get_ObjectTag() == L"Camera"
				|| (*iter)->Get_ObjectType() == GAMEOBJECT_TYPE::OBJECT_UI || (*iter)->Get_ObjectTag() == L"Bow")
				ObjectResult = (*iter)->Update_GameObject(_DT);
			else {
				ObjectResult = (*iter)->Update_GameObject_Component(_DT);
			}
			if ((*iter)->Get_ObjectDead() == TRUE || ObjectResult == -1) {
				CollisionManager::GetInstance()->Delete_ColliderObject((*iter));
				GameObject* OBJ = *iter;
				iter = GameObjectList.erase(iter);
				Safe_Release(OBJ);
				continue;
			}
			else { ++iter; }
		}
	}

	return 0;
}
VOID		Layer::LateUpdate_Layer(const FLOAT& _DT) {
	if (!_isTimeSlow) {
		for (auto& GOBJ : GameObjectList) {
			if (!GOBJ->Get_ObjectDead())
				GOBJ->LateUpdate_GameObject(_DT);
		}
	}
	else {
		for (auto iter = GameObjectList.begin(); iter != GameObjectList.end(); iter++) {
			if ((*iter)->Get_ObjectTag() == L"Player" || (*iter)->Get_ObjectTag() == L"PlayerArrow"
				|| (*iter)->Get_ObjectTag() == L"NPC_TIMESLOW" || (*iter)->Get_ObjectTag() == L"Camera"
				|| (*iter)->Get_ObjectType() == GAMEOBJECT_TYPE::OBJECT_UI || (*iter)->Get_ObjectTag() == L"Bow")
					(*iter)->LateUpdate_GameObject(_DT);
			else {
				(*iter)->LateUpdate_GameObject_Component(_DT);
			}
		}
	}

}
HRESULT Layer::Delete_Object(GameObject* _OBJ) {
	for (auto iter = GameObjectList.begin(); iter != GameObjectList.end();) {
		if (*iter == _OBJ) {
			Safe_Release(*iter);
			iter = GameObjectList.erase(iter);
			return S_OK;
		}
		else { ++iter; }
	}
	return E_FAIL;
}
GameObject* Layer::Get_GameObject(CONST TCHAR* _TAG) {
	for (auto& OBJ : GameObjectList) {
		if (OBJ->Get_ObjectTag() == _TAG) 
			return OBJ;
	}
	return nullptr;
}
HRESULT		Layer::Add_GameObject(GameObject* _GOBJ) {
	if (_GOBJ == nullptr) return E_FAIL;
	GameObjectList.push_back(_GOBJ);
	return S_OK;
}
Layer*		Layer::Create() {
	Layer* Instance = new Layer;
	if (FAILED(Instance->Ready_Layer())) {
		MSG_BOX("Cannot Create Layer.");
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}
void		Layer::Free() {
	for (auto& GOBJ : GameObjectList)
		Safe_Release(GOBJ);
}

_vec3* Layer::Search_Target(_vec3* myPos, _float radius, CONST TCHAR* _TAG)
{
	_vec3* targetPos = nullptr;
	_float minLength = radius + 1.f;
	for (auto& OBJ : GameObjectList) {
		if (OBJ->Get_ObjectTag() == _TAG) {
			_vec3* tempPos = (dynamic_cast<Transform*>(OBJ->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position());
			_vec3 vlength = *tempPos - *myPos;
			float length = D3DXVec3Length(&vlength);
			if (length < minLength) {
				minLength = length;
				targetPos = tempPos;
			}
		}
	}

	return targetPos;
}

GameObject* Layer::Search_Target_Object(_vec3* myPos, _float radius, const TCHAR* _TAG)
{
	GameObject* target = nullptr;
	_vec3* targetPos = nullptr;
	_float minLength = radius + 1.f;
	for (auto& OBJ : GameObjectList) {
		if (OBJ->Get_ObjectTag() == _TAG) {
			_vec3* tempPos = (dynamic_cast<Transform*>(OBJ->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position());
			_vec3 vlength = *tempPos - *myPos;
			float length = D3DXVec3Length(&vlength);
			if (length < minLength) {
				minLength = length;
				targetPos = tempPos;
				target = OBJ;
			}
		}
	}

	return target;
}
