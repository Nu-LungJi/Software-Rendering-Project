#pragma once
#include "GameObject.h"

class DropItemEffect : public GameObject
{
public:
	explicit DropItemEffect(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit DropItemEffect(CONST GameObject& _RHS);
	virtual ~DropItemEffect();

public:
	virtual			HRESULT		Ready_GameObject(ItemINFO* _ItemInfo);
	virtual			INT			Update_GameObject(CONST FLOAT& _DT);
	virtual			VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
	virtual			VOID		Render_GameObject();

private:
	HRESULT			Component_Initialize();

private:
	Buffer* Component_Buffer;
	Transform* Component_Transform;
	Collider* Component_Collider;
	Texture* Component_Texture;

public:
	static			DropItemEffect* Create(LPDIRECT3DDEVICE9 _GRPDEV, ItemINFO* _Info = nullptr);
	ItemINFO* Get_Info() { return m_pInfo; }
private:
	virtual VOID Free();

	ItemINFO* m_pInfo;

};