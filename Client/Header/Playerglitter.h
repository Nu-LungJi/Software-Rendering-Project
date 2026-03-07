#pragma once
#include "GameObject.h"
class Playerglitter : public GameObject {
private:
	explicit Playerglitter(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit Playerglitter(CONST GameObject& _RHS);
	virtual ~Playerglitter();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			INT			Update_GameObject(CONST FLOAT& _DT);
	virtual			VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
	virtual			VOID		Render_GameObject();

private:
	HRESULT			Component_Initialize();

private:
	ParticleEffect* Component_Buffer;
	Transform* Component_Transform;

	IDirect3DBaseTexture9* StaticTexture;

private:
	_float			m_fSpeed;
	_vec3			m_vOriginLook;
	pSee			m_pCurr;
	pSee			m_pOthe;
public:
	static	Playerglitter* Create(LPDIRECT3DDEVICE9 _GRPDEV);
private:
	virtual VOID Free();

};

