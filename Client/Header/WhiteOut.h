#pragma once
#include "GameObject.h"

class DropItem;

class StageWhiteOut : public GameObject {
private:
	explicit StageWhiteOut(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit StageWhiteOut(CONST GameObject& _RHS);
	virtual ~StageWhiteOut();

public:
	virtual			HRESULT		Ready_GameObject() { return S_OK; }
	virtual			INT			Update_GameObject(const _float& _DT);
	virtual			VOID		LateUpdate_GameObject(const _float& _DT);
	virtual			VOID		Render_GameObject();
	HRESULT						Ready_Effect();

public:
	static	StageWhiteOut* Create(LPDIRECT3DDEVICE9 _GRPDEV);
private:
	HRESULT						Component_Initialize();
	virtual	VOID				Free();
private:
	ID3DXSprite*		m_pDrawSprite;
	IDirect3DTexture9*	m_pTexture;
	DropItem*			m_pDropitem;
	_float				m_fTimer = 0.f;
	_float				m_fAlpha = 100.f;
	
};

