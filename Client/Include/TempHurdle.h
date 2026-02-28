#pragma once
#include "GameObject.h"

class Hurdle : public GameObject {
private:
	explicit Hurdle(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit Hurdle(CONST GameObject& _RHS);
	virtual ~Hurdle();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			INT			Update_GameObject(CONST FLOAT& _DT);
	virtual			VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
	virtual			VOID		Render_GameObject() {};

	
private:
	HRESULT			Component_Initialize();

public:
	static Hurdle* Create(LPDIRECT3DDEVICE9 _GRPDEV);
private:
	virtual VOID Free();

public:
	void		Set_Target(Player* _Obj) { m_pTarget = _Obj; }
private:
	SCENE_TYPE	m_eCurrScene;
	_float		m_fOffset[(uint8_t)FRUSTUMPLANE::End];

	Player* m_pTarget;
	HRESULT		MiniGame(const _float& _DT);
};