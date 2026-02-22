#pragma once
#include "GameObject.h";
enum class LEAF_ATTACK;
enum class CL_EFFECT { LEFT_HORN, RIGHT_HORN, CL_BODY, LEAF_FIRST, LEAF_EXPLOSION_CIRCLE, LEAF_CHARGING,LEAF_SPIN, LEAF_SPIN_DEATH, SPAWN_BOOM,SPAWN_THUNDER ,SPAWN_BOOM_CIRCLE, SPAWN_L, SPAWN_R,
};

class CLEffect : public GameObject {
private:
	explicit CLEffect(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit CLEffect(const GameObject& _RHS);
	virtual ~CLEffect();

public:
	virtual			HRESULT		Ready_GameObject() { return S_OK; }
	virtual			INT			Update_GameObject(const _float& _DT);
	virtual			void		LateUpdate_GameObject(const _float& _DT);
	virtual			void		Render_GameObject();

public:
	HRESULT						Ready_Effect(CL_EFFECT eEffect, _vec3 vPos, _bool bDead, _vec3 vScale, _vec3 vRot, FLOAT fFrame, _vec3 vLook, _bool bNext);
	HRESULT						Make_TextureList(wstring _FileName);
	static	CLEffect*			Create(LPDIRECT3DDEVICE9 _GRPDEV, CL_EFFECT eEffect, _vec3 vPos, _bool bDead, _vec3 vScale = { 1.5f,1.5f,1.5f }, _vec3 vRot = { 45,0,0 }, FLOAT fFrame = 0.1f, _vec3 vLook = {0,0,1},_bool bNext = false);
	

public:

	_bool						AlphaStart(CL_EFFECT eid);
	_bool						AlphaEnd(CL_EFFECT eid);
	void						Move_Normal(const _float& _DT);
	void						Move_Frame(const _float& _DT);
	void						Move_Pos();
	void						Effect_Bill();
	void						Effect_Dead_After(LEAF_ATTACK eid, _vec3 vLook, _bool bSpin = false);
	void						Pos_Check(_float x ,_float y, _float z);
private:
	HRESULT						Component_Initialize(CL_EFFECT eEffect);
	virtual	void				Free();


private:
	CL_EFFECT						m_eEffect;
	_bool							m_bDead;
	vector<IDirect3DBaseTexture9*>	TextureList;

	_int							m_TextureIndex;
	_int							m_iCnt;
	_int							m_iBulletCnt;

	_float							m_fRotY;
	_float							m_FrameTick;
	_float							m_fSpeed;
	_float						    m_fFrame;
	_float							m_fAngle;
	_float							m_fAlpha;

	_bool							m_bNextEffect;
				
	Buffer*							Component_Buffer;
	Transform*						Component_Transform;
	Texture*						Component_Texture;

	
	_vec3							m_vPos;
	_vec3							m_vScale;
	_vec3						    m_vLook;
};

