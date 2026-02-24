#pragma once
#include "GameObject.h"

class StageBlackOut : public GameObject {
private:
	explicit StageBlackOut(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit StageBlackOut(CONST GameObject& _RHS);
	virtual ~StageBlackOut();

public:
	virtual			HRESULT		Ready_GameObject() { return S_OK; }
	virtual			INT			Update_GameObject(const _float& _DT);
	virtual			VOID		LateUpdate_GameObject(const _float& _DT);
	virtual			VOID		Render_GameObject();
	HRESULT						Make_TextureList(wstring _FileName, UINT _WIDTH, UINT _HEIGHT, INT _OPACITY, INT iCnt);
	HRESULT						Ready_Effect( _vec3* vPos);
	
	void					    Set_Pos(_vec3 vPos, _bool bRe, _float fFrame) { m_vPos = vPos; m_bRestart = bRe; m_fFrame = fFrame; }
	void						Set_FrameCnt() { ++m_iFrameCnt; }
public:
		static	StageBlackOut* Create(LPDIRECT3DDEVICE9 _GRPDEV, _vec3* vPos);
private:
	HRESULT						Component_Initialize();
	virtual	VOID				Free();
private:
	_float							 m_fFrame;
	_int							 m_iFrameCnt;
	_vec3							 m_vPos;
	_bool							 m_bStop;
	_bool						     m_bRestart;

	ID3DXSprite*					 m_pDrawSprite;
	vector<LPDIRECT3DTEXTURE9>		 m_pSprtieList;
	
	Transform*			Component_Transform;


};

