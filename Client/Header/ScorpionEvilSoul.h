#pragma once
#include "GameObject.h"

class ScorpionEvilSoul : public GameObject
{
public:
	explicit ScorpionEvilSoul(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit ScorpionEvilSoul(CONST GameObject& _RHS);
	virtual ~ScorpionEvilSoul();

public:
					HRESULT		Ready_GameObject(_vec3 vPos, BOOL bMini = false);
	virtual			HRESULT		Ready_GameObject();
	virtual			INT			Update_GameObject(CONST FLOAT& _DT);
	virtual			VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
	virtual			VOID		Render_GameObject();

private:
	HRESULT			Component_Initialize();
	BOOL			OnCollisionEnter(GameObject* _Other)	override;
	BOOL			OnCollisionStay(GameObject* _Other)		override;
	BOOL			OnCollisionExit(GameObject* _Other)		override;

private:
	Buffer*			Component_Buffer;
	Transform*		Component_Transform;
	Collider*		Component_Collider;

public:

	static			ScorpionEvilSoul* Create(LPDIRECT3DDEVICE9 _GRPDEV, _vec3 vPos = {}, BOOL bMini = false);
	static			ScorpionEvilSoul* Create(LPDIRECT3DDEVICE9 _GRPDEV);

	MONSTERINFO*		Get_Info() { return &m_tInfo; }

private:
	virtual VOID Free();

	VOID Set_Target(CONST TCHAR* _TAG);
	
	MONINFO m_tInfo;

	VOID State_Summon(const _float& _DT);
	VOID State_Appear(const _float& _DT);
	VOID State_Idle(const _float& _DT);
	VOID State_Tracking(const _float& _DT);
	VOID State_Casting(const _float& _DT);
	VOID State_Channeling(const _float& _DT);
	VOID State_Disappear(const _float& _DT);
	VOID State_Dead();
};