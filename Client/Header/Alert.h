#pragma once
#include "GameObject.h"

#define SCROPOINEVILSOULIMGX 169
#define SCROPOINEVILSOULIMGY 284


class Alert : public GameObject
{
public:
	explicit Alert(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit Alert(CONST GameObject& _RHS);
	virtual ~Alert();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			INT			Update_GameObject(CONST FLOAT& _DT);
	virtual			VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
	virtual			VOID		Render_GameObject();

private:
	HRESULT			Component_Initialize();

private:
	Buffer*		Component_Buffer;
	Transform*	Component_Transform;

public:
	static			Alert* Create(LPDIRECT3DDEVICE9 _GRPDEV);
	MONSTERINFO*	Get_Info() { return &m_tInfo; }

private:
	virtual VOID Free();

	VOID Set_Target(CONST TCHAR* _TAG);

	MONINFO m_tInfo;
};