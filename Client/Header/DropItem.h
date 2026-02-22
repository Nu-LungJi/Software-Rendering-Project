#pragma once
#include "GameObject.h"

class DropItem : public GameObject
{
public:
	explicit DropItem(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit DropItem(CONST GameObject& _RHS);
	virtual ~DropItem();

public:
	virtual			HRESULT		Ready_GameObject(ItemINFO* _ItemInfo);
	virtual			INT			Update_GameObject(CONST FLOAT& _DT);
	virtual			VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
	virtual			VOID		Render_GameObject();

private:
	HRESULT			Component_Initialize();

private:
	Buffer*		Component_Buffer;
	Transform*	Component_Transform;
	Collider*	Component_Collider;
	Texture*	Component_Texture;

public:
	static			DropItem* Create(LPDIRECT3DDEVICE9 _GRPDEV, ItemINFO* _Info = nullptr);
	BOOL			OnCollisionEnter(GameObject* _Other)	override;
	BOOL			OnCollisionStay(GameObject* _Other)		override;
	BOOL			OnCollisionExit(GameObject* _Other)		override;
	ItemINFO*		Get_Info() { return m_pInfo; }
private:
	virtual VOID Free();

	ItemINFO*			m_pInfo;

};