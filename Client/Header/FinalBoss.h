#pragma once
#include "GameObject.h"
#include "Player.h"

#define		ANIMATION_APPEAR_FRAMECOUNT			39
#define		ANIMATION_DEATH_FRAMECOUNT			27
#define		ANIMATION_RAGEUP_FRAMECOUNT			33

#define		ANIMATION_RSWING_NORMAL_FRAMECOUNT	12
#define		ANIMATION_RSWING_RAGE_FRAMECOUNT	11

#define		ANIMATION_FSWING_FRAMECOUNT			12

#define		ANIMATION_STAND_NORMAL_FRAMECOUNT	14
#define		ANIMATION_STAND_RAGE_FRAMECOUNT		16

#define		ANIMATION_SLAM_NORMAL_FRAMECOUNT	11
#define		ANIMATION_SLAM_RAGE_FRAMECOUNT		11

class FinalBoss : public GameObject {
private:
	explicit FinalBoss(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit FinalBoss(CONST GameObject& _RHS);
	virtual ~FinalBoss();

public:
	virtual	HRESULT		Ready_GameObject();
	virtual INT			Update_GameObject(CONST FLOAT& _DT);
	virtual VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
	virtual VOID		Render_GameObject();

	virtual BOOL		OnCollisionEnter(GameObject* _Other);
	virtual BOOL		OnCollisionStay(GameObject* _Other);
	virtual BOOL		OnCollisionExit(GameObject* _Other);

public:
	HRESULT	Component_Initialize();
	HRESULT Texture_Initialize();

	INT		Get_Animation_Index()				{ return Animation_Index;		}
	VOID	Set_Animation_Index(INT _IDX)		{ Animation_Index = _IDX;		}

	INT		Get_Animation_Interval()			{ return Animation_Interval;	}
	VOID	Set_Animation_Interval(FLOAT _ITV)	{ Animation_Interval = _ITV;	}

	static	FinalBoss* Create(LPDIRECT3DDEVICE9 _GRPDEV);

private:
	virtual	VOID	Free();

private:
	Player*			PlayerObject;
	_vec3			PlayerPos;

	FLOAT			BossHP;

	BOOL			Invalidate_Mode;
	BOOL			Rage_Mode;
	BOOL			Action_Mode;
	BOOL			Death_Mode;

	INT				Action_Selector;
	FLOAT			Action_Timer;

	BOOL			DoubleSlam;

	vector<LPDIRECT3DTEXTURE9>*	Animation_TexList;
	FLOAT						Animation_Timer;
	FLOAT						Animation_Interval;
	INT							Animation_Index;
	INT							Animation_FrameCount;

	vector<LPDIRECT3DTEXTURE9>	Animation_Appear_TexList;
	vector<LPDIRECT3DTEXTURE9>	Animation_Death_TexList;
	vector<LPDIRECT3DTEXTURE9>	Animation_RageUp_TexList;

	vector<LPDIRECT3DTEXTURE9>	Animation_RSwing_Normal_TexList;
	vector<LPDIRECT3DTEXTURE9>	Animation_RSwing_Rage_TexList;

	vector<LPDIRECT3DTEXTURE9>	Animation_FSwing_TexList;

	vector<LPDIRECT3DTEXTURE9>	Animation_Stand_Normal_TexList;
	vector<LPDIRECT3DTEXTURE9>	Animation_Stand_Rage_TexList;

	vector<LPDIRECT3DTEXTURE9>	Animation_Slam_Normal_TexList;
	vector<LPDIRECT3DTEXTURE9>	Animation_Slam_Rage_TexList;

	vector<LPDIRECT3DTEXTURE9>	Animation_Slam_TexList;
	vector<LPDIRECT3DTEXTURE9>	Animation_Stand_TexList;

private:
	Buffer*			Component_Buffer;
	Texture*		Component_Texture;
	Transform*		Component_Transform;
	Collider*		Component_Collider;

	StateMachine*	FSM;
};