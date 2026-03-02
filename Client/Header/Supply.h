#pragma once
#include "GameObject.h"

enum class SUPPLY_OBJECT{ 
	ShopKeeper, SupplyBigCat, 
	Stall1, Stall2, Stall3, Stall4, Stall5,Stall6, Reroll,
	End};

class Supply : public GameObject
{
public:
	explicit Supply(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit Supply(CONST GameObject& _RHS);
	virtual ~Supply();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			INT			Update_GameObject(CONST FLOAT& _DT);
	virtual			VOID		LateUpdate_GameObject(CONST FLOAT& _DT);

private:
	HRESULT			Component_Initialize();

private:
	Transform*		Component_Transform;

public:
	static			Supply* Create(LPDIRECT3DDEVICE9 _GRPDEV);

private:
	virtual VOID Free();

	GameObject* m_pObj[(long)SUPPLY_OBJECT::End];
};