#pragma once
#include "GameObject.h"
#include "FinalBoss.h"

class Supporter : public GameObject {
private:
	explicit Supporter(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit Supporter(CONST GameObject& _RHS);
	virtual ~Supporter();

public:
	virtual	HRESULT		Ready_GameObject();
	virtual INT			Update_GameObject(CONST FLOAT& _DT);
	virtual VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
	virtual VOID		Render_GameObject();

	virtual BOOL		OnCollisionEnter(GameObject* _Other);
	virtual BOOL		OnCollisionStay(GameObject* _Other);
	virtual BOOL		OnCollisionExit(GameObject* _Other);

public:
	HRESULT	Component_Initialize();
	HRESULT Texture_Initialize();

	static	Supporter* Create(LPDIRECT3DDEVICE9 _GRPDEV);

private:
	FinalBoss* Boss;

private:
	virtual		VOID	Free();
};

