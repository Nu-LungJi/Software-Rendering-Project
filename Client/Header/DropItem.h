#pragma once
#include "GameObject.h"

class DropItem : public GameObject
{
public:
	explicit DropItem(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit DropItem(CONST GameObject& _RHS);
	virtual ~DropItem();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			INT			Update_GameObject(CONST FLOAT& _DT);
	virtual			VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
	virtual			VOID		Render_GameObject();

private:
	HRESULT			Component_Initialize();

private:
	Buffer*				Component_Buffer;
	Transform*			Component_Transform;
	Collider*			Component_Collider;
	IDirect3DTexture9*	m_pTexture;

public:
	static			DropItem* Create(LPDIRECT3DDEVICE9 _GRPDEV);
	BOOL			OnCollisionEnter(GameObject* _Other)	override;
	BOOL			OnCollisionStay(GameObject* _Other)		override;
	BOOL			OnCollisionExit(GameObject* _Other)		override;

private:
	virtual VOID Free();

public:
	void	Set_EventTrigger(int* pEventTrigger) { m_pEventTrigger = pEventTrigger; }

private:
	int*	m_pEventTrigger = nullptr;
	int		m_iPickUpEvent = 0;
	float	m_fTimer = 0.f;

};