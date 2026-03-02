#pragma once
#include "GameObject.h"

class Stall : public GameObject
{
public:
	explicit Stall(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit Stall(CONST GameObject& _RHS);
	virtual ~Stall();

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
	Collider*	Component_Collider;
	Texture*	Component_Texture;

public:
	static	Stall*	Create(LPDIRECT3DDEVICE9 _GRPDEV);
	GameObject*		Get_Item()		{ return m_pItem; }
	GameObject*		Get_Effect()	{ return m_pEffect; }


	BOOL			OnCollisionEnter(GameObject* _Other)	override;
	BOOL			OnCollisionStay(GameObject* _Other)		override;
	BOOL			OnCollisionExit(GameObject* _Other)		override;

private:
	virtual VOID Free();

	GameObject* m_pItem;
	GameObject* m_pEffect;

};