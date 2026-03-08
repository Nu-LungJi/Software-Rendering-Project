#include "CollisionManager.h"
#include "SceneManager.h"
#include "KeyManager.h"

IMPLEMENT_SINGLETON(CollisionManager)

CollisionManager::CollisionManager()	: CollisionLine_Visibility(FALSE) {}
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
		bool ret = false;
		for (auto& DOBJ : SceneObjectList) {
			if (SOBJ->Get_ObjectDead() || SOBJ == nullptr || SOBJ == DOBJ || DOBJ == nullptr || DOBJ->Get_ObjectDead())	continue;
			Collider* DEST = dynamic_cast<Collider*>(DOBJ->Get_Component(COMPONENT_TYPE::COMPONENT_COLLIDER));
			if (DEST == nullptr) continue;
			if ((SRC->Get_MaxPoint().x >= DEST->Get_MinPoint().x) && (DEST->Get_MaxPoint().x >= SRC->Get_MinPoint().x) &&
				(SRC->Get_MaxPoint().y >= DEST->Get_MinPoint().y) && (DEST->Get_MaxPoint().y >= SRC->Get_MinPoint().y) &&
				(SRC->Get_MaxPoint().z >= DEST->Get_MinPoint().z) && (DEST->Get_MaxPoint().z >= SRC->Get_MinPoint().z)) {
				if (SOBJ->Search_CollisionObject(DOBJ) == FALSE ) {
					SOBJ->Add_CollisionObject(DOBJ);
					SOBJ->OnCollisionEnter(DOBJ);
				}
				SOBJ->OnCollisionStay(DOBJ);

				ret = true;

			}
			else {
				if (SOBJ->Search_CollisionObject(DOBJ) == TRUE && DOBJ->Search_CollisionObject(SOBJ) == TRUE) {
					SOBJ->Delete_CollisionObject(DOBJ);
					DOBJ->Delete_CollisionObject(SOBJ);
					SOBJ->OnCollisionExit(DOBJ);
					DOBJ->OnCollisionExit(SOBJ);
				}
			}
		}
	}

	return ret;
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

VOID CollisionManager::Add_ColliderObject(GameObject* _OBJ) {
	SceneObjectList.push_back(_OBJ);
}

VOID CollisionManager::Delete_ColliderObject(GameObject* _OBJ) {
	for (auto iter = SceneObjectList.begin(); iter != SceneObjectList.end();) {
		if (*iter == _OBJ) {
			//Safe_Release((*iter));
			iter = SceneObjectList.erase(iter);
			continue;
		}
		else { ++iter; }
	}
}

VOID CollisionManager::Free() {

}