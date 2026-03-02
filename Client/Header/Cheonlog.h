#pragma once
#include "GameObject.h"

#define CHEONLOG  dynamic_cast<Cheonlog*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"CheonLog")) 

enum  CL_CHECK { IDEL, SPAWN_AFTER, ATTACK_A, ATTACK_B, ATTACK_C, ATTACK_D, CHECK_END };
enum  CL_STATU { SPAWN, CL_IDELR, CL_LJUMP, CL_RJUMP,CL_LUJUMP,CL_RUJUMP, CL_DEAD, CL_END};
enum class LEAF_ATTACK { LEAF_FIRST, LEAF_SECOND, LEAF_THIRD, LEAF_FOUR, LEAF_EXPLOSION, LEAF_BOOM_CIRCLE, LEAF_END };
enum class CL_EFFECT {
	LEFT_HORN, RIGHT_HORN, CL_BODY, LEAF_FIRST, LEAF_EXPLOSION_CIRCLE, LEAF_CHARGING, LEAF_SPIN, LEAF_SPIN_DEATH, SPAWN_BOOM, SPAWN_THUNDER, SPAWN_BOOM_CIRCLE, SPAWN_L, SPAWN_R,SPAWN_POTAL,
CL_EFFECTEND};

class CLAttack;
class Cheonlog : public GameObject
{
public:
	explicit Cheonlog(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit Cheonlog(const GameObject& _RHS);
	virtual ~Cheonlog();

public:
	virtual			HRESULT		Ready_GameObject(_vec3 vPos);
	virtual			INT			Update_GameObject(const _float& _DT);
	virtual			void		LateUpdate_GameObject(const _float& _DT);
	virtual			void		Render_GameObject();
	HRESULT						Component_Initialize();



private:
	void        Texture_Initalize(_int iCnt, const _tchar* pName, CL_STATU CheongLog);
	void		Set_Statu();
	void	    Change_Statu(const _float& _DT, _int iMaxCnt);
	void		Change_Pattern(const _float& _DT);
	void		Reset_Pattern(CL_CHECK eCheck, CL_STATU eStatu);

	void		AttackLeaf_First(const _float& _DT, _vec3 vPos);
	void		AttackLeaf_Second(const _float& _DT, _vec3 vPos);
	void		AttackLeaf_Third(const _float& _DT, _vec3 vPos);
	void		AttackLeaf_Four(const _float& _DT, _vec3 vPos);

	void		Create_Cheonlog(const _float& _DT, _vec3 vPos);
	void		Create_Cheonlog_After(const _float& _DT, _vec3 vPos);

	void		Create_Crystal();
	_bool		Create_Leaf(const _float& _DT);
	void		Create_Leaf_Third(_vec3 vPos);
	void	    Create_Leaf_Third_S(_vec3 vPos);
	void		Create_Leaf_Four(_vec3 vPos, _float fRot);

	void		Create_Pool(LEAF_ATTACK eid ,_vec3 vPos, _vec3 vL,_float fFrameSpeed = 0.1f);
	void		CL_Jump(const _float& _DT, _int iMaxCnt);
	void		CL_JumpCenter(const _float& _DT, _int iMaxCnt);

public:
	void		Set_Effect(_bool bEffect)     { m_EndEffect = bEffect; }
	void        Set_StartAttack(_bool bStart) { m_StartAttack = bStart; }


	HRESULT	Make_TextureList(wstring _FileName, LEAF_ATTACK eid);
	HRESULT	Make_EffectTextureList(wstring _FileName, CL_EFFECT eid);

	CL_STATU		Get_Statu() { return m_eStatu; }
	CL_CHECK		Get_Check() { return m_eCheck; }
	void			Set_Statu(_vec3 vPos) { m_eStatu = CL_IDELR; m_eCheck = SPAWN_AFTER;
											m_iSkillDelay = 0;  
											Component_Transform->Set_Pos(vPos.x,3.5,vPos.z); }
	vector<IDirect3DBaseTexture9*> Get_BulletTexture(LEAF_ATTACK eid) {return m_vecBullet[(int)eid]; }
	vector<IDirect3DBaseTexture9*> Get_EffectTexture(CL_EFFECT eid) { return m_vecEffect[(int)eid]; }
	
	vector<GameObject*>& Get_PollMainBullet(LEAF_ATTACK eid) { return m_vecPoolBullet[(int)eid]; }
	vector<GameObject*>& Get_OriginBullet(LEAF_ATTACK eid) { return m_vecOrignBullet[(int)eid]; }

public:
			
private:
	Buffer*		Component_Buffer;
	Transform*  Component_Transform;
	Collider*	Component_Collider;



	vector<IDirect3DBaseTexture9*>  m_vecEffect[(int)CL_EFFECT::CL_EFFECTEND];
	vector<IDirect3DBaseTexture9*>  m_vecBullet[(int)LEAF_ATTACK::LEAF_END];
	vector<IDirect3DBaseTexture9*>  m_vecCheonlogTexture[CL_END];

	vector<GameObject*>				m_vecOrignBullet[(int)LEAF_ATTACK::LEAF_END];
	vector<GameObject*>				m_vecPoolBullet[(int)LEAF_ATTACK::LEAF_END];

private:
	GameObject*		m_pTarget;


	_float			m_frameTick;
	_float			m_frameAttack;
	_float			m_framePattern;
	_float			m_fAttackSecondTick;
	_float			m_fRotY;

	_int			m_iFrameCnt;			
	_int			m_iNextSkill;
	_int			m_iSkillDelay;
	_int			m_iSkillMaxCnt;
	_int			m_iSkillCnt;
	_int			m_iStatuCnt;
	_int			m_iBulletCnt;
	UINT			m_frame; 

	_bool			m_bSpawn;
	_bool			m_bStartPattern;
	_bool			m_bMoveEffect;
	_bool			m_StartAttack;
	_bool			m_EndEffect;
	_bool			m_bCrystal;
	_bool			m_bDeadBgm;
	_bool			m_bBgm;
	_bool		    m_bDead;

	_vec3			m_vCenter;
private:
	//µð¹ö±×¿ë
	_float			m_fPivot;
	_vec3			m_vDebug;
private:
	CL_CHECK		m_eCheck;
	CL_STATU		m_eStatu;
	CL_STATU		m_eCurr;
public:
	static			Cheonlog* Create(LPDIRECT3DDEVICE9 _GRPDEV,_vec3 vPos);
private:
	virtual			void			Free();


};

