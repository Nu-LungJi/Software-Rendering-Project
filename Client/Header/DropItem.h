#pragma once
#include "GameObject.h"

class Item : public GameObject
{
public:
	explicit Item(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit Item(CONST GameObject& _RHS);
	virtual ~Item();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			INT			Update_GameObject(CONST FLOAT& _DT);
	virtual			VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
	virtual			VOID		Render_GameObject();

private:
	HRESULT			Component_Initialize();

private:
	Buffer* Component_Buffer;
	Transform* Component_Transform;
	Collider* Component_Collider;

public:
	static			Item* Create(LPDIRECT3DDEVICE9 _GRPDEV);
	BOOL			OnCollisionEnter(GameObject* _Other)	override;
	BOOL			OnCollisionStay(GameObject* _Other)		override;
	BOOL			OnCollisionExit(GameObject* _Other)		override;
	MONSTERINFO* Get_Info() { return &m_tInfo; }
private:
	virtual VOID Free();
	MONSTERINFO		m_tInfo;

	VOID Set_Target(CONST TCHAR* _TAG);

	VOID State_Appear(const _float& _DT);
	VOID State_Idle();
	VOID State_Tracking(const _float& _DT);
	VOID State_Casting(const _float& _DT);
	VOID State_Channeling(const _float& _DT);
	VOID State_Dead();

};