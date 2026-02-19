#pragma once
#include "GameObject.h";

enum class CL_EFFECT { LEFT_HORN, RIGHT_HORN, CL_BODY, LEAF_FIRST, LEAF_EXPLOSION, LEAF_EXPLOSION_CIRCLE, LEAF_CHARGING,LEAF_SPIN
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

	virtual			 BOOL		OnCollisionEnter(GameObject* _Other) { return 0; };
	virtual			 BOOL		OnCollisionStay(GameObject* _Other)  { return 0; };
	virtual			 BOOL		OnCollisionExit(GameObject* _Other)  { return 0; };
public:
	HRESULT						Ready_Effect(CL_EFFECT eEffect, _vec3 vPos, _bool bDead, _vec3 vScale, _vec3 vRot, FLOAT fFrame);
	HRESULT						Make_TextureList(wstring _FileName);
	static	CLEffect*			Create(LPDIRECT3DDEVICE9 _GRPDEV, CL_EFFECT eEffect, _vec3 vPos, _bool bDead, _vec3 vScale = { 1.5f,1.5f,1.5f }, _vec3 vRot = { 45,0,0 }, FLOAT fFrame = 0.1f);
public:
	void						Move_Frame(const _float& _DT);
	void						Move_Pos();
	void						Pos_Check(_float x ,_float y, _float z);
private:
	HRESULT						Component_Initialize(CL_EFFECT eEffect);
	virtual	void				Free();


private:
	CL_EFFECT						m_eEffect;
	_bool							m_bDead;
	vector<IDirect3DBaseTexture9*>	TextureList;

	INT								m_TextureIndex;
	INT							    m_iCnt;
	FLOAT							m_FrameTick;
	FLOAT							m_PlayTime;
	FLOAT						    m_fFrame;
	BOOL							m_Repeatable;

	Buffer*							Component_Buffer;
	Transform*						Component_Transform;
	Texture*						Component_Texture;
	Collider*						Component_Collider;

	
	_vec3							m_vScale;

};

