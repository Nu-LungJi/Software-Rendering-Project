#pragma once
#include "GameObject.h"


enum  CL_CHECK { IDEL, ATTACK_A, ATTACK_B, ATTACK_C, CHECK_END };
enum  CL_STATU { CL_IDELR, CL_LJUMP, CL_RJUMP, CL_END};

class Cheonlog : public GameObject
{
public:
	explicit Cheonlog(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit Cheonlog(const GameObject& _RHS);
	virtual ~Cheonlog();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			INT			Update_GameObject(const _float& _DT);
	virtual			void		LateUpdate_GameObject(const _float& _DT);
	virtual			void		Render_GameObject();
	HRESULT						Component_Initialize();



private:
	void        Texture_Initalize(_int iCnt, const _tchar* pName, CL_STATU CheongLog);
	void		Set_Statu();
	void	    Change_Statu(const _float& _DT, _int iMaxCnt);
	void		AttackLeaf_First(const _float& _DT, _vec3 vPos);
	void		AttackLeaf_Second(const _float& _DT, _vec3 vPos);
	void		AttackLeaf_Third(const _float& _DT, _vec3 vPos);
	void		AttackLeaf_Four(const _float& _DT, _vec3 vPos);

	void		Create_Crystal();
	_bool		Create_Leaf(const _float& _DT);
	void		Create_Leaf_Third(_vec3 vPos);
	void	    Create_Leaf_Third_S(_vec3 vPos);
	void		Create_Leaf_Four(_vec3 vPos);

	void		CL_Jump(const _float& _DT, _int iMaxCnt);
	void		CL_JumpCenter(const _float& _DT, _int iMaxCnt);

public:
	void		Set_Effect(_bool bEffect)     { m_EndEffect = bEffect; }
	void        Set_StartAttack(_bool bStart) { m_StartAttack = bStart; }

	CL_STATU		Get_Statu() { return m_eStatu; }
public:
	void        Debug_ButtonStyle();
	void        Debug_Button(const char pName[32], _vec3* vPivot, _float iLinePivot);

private:
	Buffer*		Component_Buffer;
	Transform*  Component_Transform;
	Collider*	Component_Collider;

	IDirect3DBaseTexture9* m_pTexture;
	vector<IDirect3DBaseTexture9*>  m_vecCheonlogTexture[CL_END];

private:
	GameObject*		m_pTarget;


	_float			m_frameTick;
	_float			m_frameAttack;
	_float			m_fAttackSecondTick;
	_float			m_fRotY;

	_int			m_iFrameCnt;
	_int			m_iSkillDelay;
	_int			m_iSkillMaxCnt;
	_int			m_iSkillCnt;
	_int			m_iStatuCnt;
	_int			m_iBulletCnt;
	UINT			m_frame; 

	_bool			m_bMoveEffect;
	_bool			m_StartAttack;
	_bool			m_EndEffect;
	_bool			m_bCrystal;
private:
	//디버그용
	_float			m_fPivot;
	_vec3			m_vDebug;
private:
	CL_CHECK		m_eCheck;
	CL_STATU		m_eStatu;
public:
	static			Cheonlog* Create(LPDIRECT3DDEVICE9 _GRPDEV);
private:
	virtual			void			Free();


};

