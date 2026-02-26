#pragma once
#include "GameObject.h"
//enum class LEAF_ATTACK{LEAF_FIRST, LEAF_SECOND, LEAF_THIRD, LEAF_FOUR, LEAF_EXPLOSION};


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
	BOOL						OnCollisionStay(GameObject* _Other);
	BOOL						OnCollisionEnter(GameObject* _Other);

private:
	HRESULT				     Component_Initialize();

public:
	void					 Move_Frame(const _float& _DT);
	void					 Move_Leaf(const _float& _DT);
	void					 Leaf_First(const _float& _DT);
	void				     Leaf_Second(const _float& _DT);
	void				     Leaf_Third(const _float& _DT);
	void					 Leaf_Four(const _float& _DT);
	void					 Leaf_Explosion(const _float& _DT);
	void					 Leaf_Bill(const _float& _DT);

private:
	_float							m_FrameTick;
	_float							m_fAttackTick;
	_float							m_fDeadTick;

	_float							m_fSpeed;
	_float							m_fFrameSpeed;

	_int						    m_TextureIndex;
	_int							m_iAttackIndex;
	_int						    m_iRandCnt;
	_int							m_iDeadCnt;

	_vec3							m_vLook;
	_vec3							m_CLPos;
	_vec3							m_vRight;
	_vec3							m_vPlayerPos;

	_bool					        m_bCheck;
	_bool							m_bSpin;
	_bool							m_bBgm;
private:
	LEAF_ATTACK						m_eLeaf;
private:
	Buffer*							Component_Buffer;
	Transform*						Component_Transform;
	Collider*						Component_Collider;

public:
	static CLAttack* Create(LPDIRECT3DDEVICE9 _GRPDEV, LEAF_ATTACK eLeaft, _vec3 vPos, _vec3 vLook, _bool bSpin = false);
private:
	virtual void Free();

private:
	
};

