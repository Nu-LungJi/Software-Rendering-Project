#pragma once
#include "GameObject.h"

#define SCROPOINEVILSOULIMGX 169
#define SCROPOINEVILSOULIMGY 284


class DocheolAppearMark : public GameObject
{
public:
	explicit DocheolAppearMark(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit DocheolAppearMark(CONST GameObject& _RHS);
	virtual ~DocheolAppearMark();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			INT			Update_GameObject(CONST FLOAT& _DT);
	virtual			VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
	virtual			VOID		Render_GameObject();

private:
	HRESULT			Component_Initialize();

private:
	Buffer*			Component_Buffer;
	Transform*		Component_Transform;

public:
	static			DocheolAppearMark* Create(LPDIRECT3DDEVICE9 _GRPDEV);
	MONSTERINFO*	Get_Info() { return &m_tInfo; }

private:
	virtual VOID Free();

	VOID Set_Target(CONST TCHAR* _TAG);

	MONINFO m_tInfo;
	_float	m_fFlyingDis;

	VOID State_Summon(const _float& _DT);
	VOID State_Idle(const _float& _DT);
	VOID State_Tracking(const _float& _DT);
	VOID State_Casting(const _float& _DT);
	VOID State_Channeling(const _float& _DT);
	VOID State_Dead();
};