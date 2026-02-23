#include "CollisionManager.h"
#include "SceneManager.h"
#include "KeyManager.h"

IMPLEMENT_SINGLETON(CollisionManager)

CollisionManager::CollisionManager()	: CollisionLine_Visibility(TRUE) {}
CollisionManager::~CollisionManager()									 {}

INT CollisionManager::Update_CollisionManager() {
	return 0;
}

VOID CollisionManager::LateUpdate_CollisionManager() {
	if (KeyManager::GetInstance()->KEY_STATE_DOWN(DIK_F4) == TRUE)
		CollisionLine_Visibility ? CollisionLine_Visibility = FALSE : CollisionLine_Visibility = TRUE;
	AABB_Collision();
}

VOID CollisionManager::Render_CollisionManager() {
	for (auto& OBJ : SceneObjectList) {
		Component* ColCom = OBJ->Get_Component(COMPONENT_TYPE::COMPONENT_COLLIDER);
		if (ColCom == nullptr || !CollisionLine_Visibility) continue;
		ColCom->Render_Component();
	}
}

BOOL CollisionManager::AABB_Collision() {
	for (auto& SOBJ : SceneObjectList) {
		if (SOBJ->Get_ObjectDead() || SOBJ == nullptr)	continue;
		Collider* SRC = dynamic_cast<Collider*>(SOBJ->Get_Component(COMPONENT_TYPE::COMPONENT_COLLIDER));
		if (SRC == nullptr) continue;
		for (auto& DOBJ : SceneObjectList) {
			if (SOBJ->Get_ObjectDead() || SOBJ == nullptr || SOBJ == DOBJ)	continue;
			Collider* DEST = dynamic_cast<Collider*>(DOBJ->Get_Component(COMPONENT_TYPE::COMPONENT_COLLIDER));
			if (DEST == nullptr) continue;

			_vec3 DstMax, DstMin, SrcMax, SrcMin ;
	
			DstMax = { floor(DEST->Get_MaxPoint().x),floor(DEST->Get_MaxPoint().y),floor(DEST->Get_MaxPoint().z)};
			DstMin = { floor(DEST->Get_MinPoint().x),floor(DEST->Get_MinPoint().y),floor(DEST->Get_MinPoint().z) };
			
			SrcMax = { floor(SRC->Get_MaxPoint().x),floor(SRC->Get_MaxPoint().y),floor(SRC->Get_MaxPoint().z) };
			SrcMin = { floor(SRC->Get_MinPoint().x),floor(SRC->Get_MinPoint().y),floor(SRC->Get_MinPoint().z) };

			if ((SrcMax.x >= DstMin.x) && (DstMax.x >= SrcMin.x) &&
				(SrcMax.y >= DstMin.y) && (DstMax.y >= SrcMin.y) &&
				(SrcMax.z >= DstMin.z) && (DstMax.z >= SrcMin.z)) {
				if (SOBJ->Search_CollisionObject(DOBJ) == FALSE && DOBJ->Search_CollisionObject(SOBJ) == FALSE) {
					SOBJ->Add_CollisionObject(DOBJ);
					DOBJ->Add_CollisionObject(SOBJ);
					SOBJ->OnCollisionEnter(DOBJ);
					DOBJ->OnCollisionEnter(SOBJ);
				}
				SOBJ->OnCollisionStay(DOBJ);
				DOBJ->OnCollisionStay(SOBJ);
	
				return TRUE;
			}
			else {
				if (SOBJ->Search_CollisionObject(DOBJ) == TRUE && DOBJ->Search_CollisionObject(SOBJ) == TRUE)	{
					SOBJ->Delete_CollisionObject(DOBJ);
					DOBJ->Delete_CollisionObject(SOBJ);
					DOBJ->OnCollisionExit(SOBJ);
					SOBJ->OnCollisionExit(DOBJ);
				}
			}
		}
	}
	
	return FALSE;

	return FALSE;
}

VOID CollisionManager::Get_AllObjectOfScene() {
	Layer* SceneLayer = SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT);
	list<GameObject*>* GOList = SceneLayer->Get_GameObjectList();

	for (auto& GOBJ : *GOList) {
		if(GOBJ->Get_Component(COMPONENT_TYPE::COMPONENT_COLLIDER) != nullptr)
			SceneObjectList.push_back(GOBJ);
	}

	SceneLayer = SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_STATIC_OBJECT);
	GOList = SceneLayer->Get_GameObjectList();

	for (auto& GOBJ : *GOList) {
		if (GOBJ->Get_Component(COMPONENT_TYPE::COMPONENT_COLLIDER) != nullptr)
			SceneObjectList.push_back(GOBJ);
	}
}

VOID CollisionManager::Delete_ColliderObject(GameObject* _OBJ) {
	for (auto iter = SceneObjectList.begin(); iter != SceneObjectList.end();) {
		if (*iter == _OBJ) {
			Safe_Release((*iter));
			iter = SceneObjectList.erase(iter);
			continue;
		}
		else { ++iter; }
	}
}

VOID CollisionManager::Free() {

}