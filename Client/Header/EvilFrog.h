#pragma once
#include "GameObject.h"

#define EVILFROGIMGX 79
#define EVILFROGIMGY 129

class EvilFrog :  public GameObject
{
	explicit EvilFrog(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit EvilFrog(CONST GameObject& _RHS);
	virtual ~EvilFrog();

public:
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
	Buffer* Component_Buffer;
	Transform* Component_Transform;
	Collider* Component_Collider;
public:
	static			EvilFrog* Create(LPDIRECT3DDEVICE9 _GRPDEV);
	MONSTERINFO* Get_Info() { return &m_tInfo; }

private:
	virtual VOID Free();

	MONINFO m_tInfo;
	_vec3	m_vFissionDst;

  VOID State_Summon(const _float& _DT);
	VOID State_Idle(const _float& _DT);
	VOID State_Tracking(const _float& _DT);
	VOID State_Casting(const _float& _DT);
	VOID State_Channeling(const _float& _DT);
	VOID State_Dead();

};

