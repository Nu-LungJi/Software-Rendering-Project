#pragma once
#include "GameObject.h"

class SupplyBigCat : public GameObject
{
public:
	explicit SupplyBigCat(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit SupplyBigCat(CONST GameObject& _RHS);
	virtual ~SupplyBigCat();

public:
	virtual			HRESULT		Ready_GameObject();
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
	static			SupplyBigCat* Create(LPDIRECT3DDEVICE9 _GRPDEV);
	BOOL			OnCollisionEnter(GameObject* _Other)	override;
	BOOL			OnCollisionStay(GameObject* _Other)		override;
	BOOL			OnCollisionExit(GameObject* _Other)		override;
private:
	virtual VOID Free();

};