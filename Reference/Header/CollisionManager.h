#pragma once
#include "Base.h"
#include "Engine_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class ENGINE_DLL CollisionManager : public Base {
	DECLARE_SINGLETON(CollisionManager)
private:
	explicit CollisionManager();
	virtual ~CollisionManager();

public:
	INT		Update_CollisionManager();
	VOID	LateUpdate_CollisionManager();
	VOID	Render_CollisionManager();
	
public:
	BOOL	AABB_Collision();
	VOID	Get_AllObjectOfScene();
	VOID	Add_ColliderObject(GameObject* _Effect); 
  //{
	//	int before = SceneObjectList.size();
	//	SceneObjectList.push_back(_Effect);
	//	int after = SceneObjectList.size();
	//}
  // 충돌처리 오류 해결로 인한 주석처리
	VOID	Delete_ColliderObject(GameObject* _Effect);

private:
	list<GameObject*>		SceneObjectList;

	BOOL					CollisionLine_Visibility;
	BOOL					ret;
private:
	virtual VOID	Free();
};

END