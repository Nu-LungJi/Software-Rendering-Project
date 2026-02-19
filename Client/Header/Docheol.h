#pragma once
#include "GameObject.h"


class Docheol : public GameObject
{
public:
	explicit Docheol(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit Docheol(CONST GameObject& _RHS);
	virtual ~Docheol();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			INT			Update_GameObject(CONST FLOAT& _DT);
	virtual			VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
	virtual			VOID		Render_GameObject();

	static			Docheol*	Create(LPDIRECT3DDEVICE9 _GRPDEV);
	BOOL			OnCollisionEnter(GameObject* _Other)	override;
	BOOL			OnCollisionStay(GameObject* _Other)		override;
	BOOL			OnCollisionExit(GameObject* _Other)		override;
	MONSTERINFO* Get_Info() { return &m_tInfo; }
private:
	VOID			Free();
	HRESULT			Component_Initialize();

private:
	Buffer*		Component_Buffer;
	Transform*	Component_Transform;
	Collider*	Component_Collider;

private:

	MONSTERINFO		m_tInfo;

	VOID State_Summon(const _float& _DT);
	VOID State_HandUpAppear(const _float& _DT);
	VOID State_Appear(const _float& _DT);
	VOID State_Idle(const _float& _DT);
	VOID State_Tracking(const _float& _DT);
	VOID State_Punch(const _float& _DT);
	VOID State_Casting(const _float& _DT);
	VOID State_Channeling(const _float& _DT);
	VOID State_Dead(const _float& _DT);

};

