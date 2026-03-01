#pragma once
#include "GameObject.h"
class Rain : public GameObject {
private:
	explicit Rain(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit Rain(CONST GameObject& _RHS);
	virtual ~Rain();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			INT			Update_GameObject(CONST FLOAT& _DT);
	virtual			VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
	virtual			VOID		Render_GameObject();

private:
	HRESULT			Component_Initialize();

private:
	ParticleRain* Component_Buffer;
	Transform* Component_Transform;

	IDirect3DBaseTexture9* StaticTexture;

public:
	static	Rain* Create(LPDIRECT3DDEVICE9 _GRPDEV);
private:
	virtual VOID Free();

};

