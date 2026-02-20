#pragma once
#include "GameObject.h"
enum class LEAF_ATTACK{LEAF_FIRST, LEAF_SECOND, LEAF_THIRD, LEAF_FOUR};

class CLAttack : public GameObject
{
private:
	explicit CLAttack(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit CLAttack(const GameObject& _RHS);
	virtual ~CLAttack();

public:
	virtual			HRESULT		Ready_GameObject(LEAF_ATTACK eLeaft, _vec3 vPos, _vec3 vLook, _bool bSpin);
	virtual			INT			Update_GameObject(const _float& _DT);
	virtual			void		LateUpdate_GameObject(const _float& _DT);
	virtual			void		Render_GameObject();

private:
	HRESULT					 Make_TextureList(wstring _FileName);
	HRESULT				     Component_Initialize();

public:
	void					 Move_Frame(const _float& _DT);
	void					 Move_Leaf(const _float& _DT);
	void					 Leaf_First(const _float& _DT);
	void				     Leaf_Second(const _float& _DT);
	void				     Leaf_Third(const _float& _DT);
	void					 Leaf_Four(const _float& _DT);
	void					 Leaf_Bill(const _float& _DT);

private:
	_float							m_FrameTick;
	_float							m_fAttackTick;

	_float							m_fSpeed;
	_int						    m_TextureIndex;
	_int							m_iAttackIndex;
	_int						    m_iRandCnt;

	_vec3							m_vLook;
	_vec3							m_CLPos;
	_vec3							m_vRight;
	_vec3							m_vPlayerPos;
	_bool					        m_bCheck;
	_bool							m_bSpin;
private:
	LEAF_ATTACK						m_eLeaf;
private:
	Buffer*							Component_Buffer;
	Transform*						Component_Transform;
	Collider*						Component_Collider;

	vector<IDirect3DBaseTexture9*>	TextureList;

public:
	static CLAttack* Create(LPDIRECT3DDEVICE9 _GRPDEV, LEAF_ATTACK eLeaft, _vec3 vPos, _vec3 vLook, _bool bSpin = false);
	BOOL			 OnCollisionEnter(GameObject* _Other);
private:
	virtual void Free();

private:
	
};

