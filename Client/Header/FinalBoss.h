#pragma once
#include "GameObject.h"
#include "Player.h"
#include "StateMachine.h"

#define		ANIMATION_NONANIM_FRAMECOUNT		3

#define		ANIMATION_APPEAR_FRAMECOUNT			39
#define		ANIMATION_DEATH_FRAMECOUNT			28
#define		ANIMATION_RAGEUP_FRAMECOUNT			28
#define		ANIMATION_STUNNING_FRAMECOUNT		10

#define		ANIMATION_NORMAL_STAND_FRAMECOUNT	14
#define		ANIMATION_NORMAL_RSWING_FRAMECOUNT	12
#define		ANIMATION_NORMAL_FSWING_FRAMECOUNT	12
#define		ANIMATION_NORMAL_SLAM_FRAMECOUNT	11

#define		ANIMATION_RAGE_STAND_FRAMECOUNT		16
#define		ANIMATION_RAGE_RSWING_FRAMECOUNT	11
#define		ANIMATION_RAGE_SLAM_FRAMECOUNT		11
#define		ANIMATION_RAGE_CHARGE_FRAMECOUNT	8

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

	VOID	Animation_Appear_Staging(CONST FLOAT& _DT);
	VOID	Animation_Disappear_Staging(CONST FLOAT& _DT);

	INT		Get_Animation_CurrentIndex()				{ return Animation_CurrentIndex;  }
	VOID	Set_Animation_CurrentIndex(INT _IDX)		{ Animation_CurrentIndex = _IDX;  }

	INT		Get_Animation_PreviousIndex()				{ return Animation_PreviousIndex; }
	VOID	Set_Animation_PreviousIndex(INT _IDX)		{ Animation_PreviousIndex = _IDX; }

	INT		Get_Animation_Interval()					{ return Animation_Interval;	  }
	VOID	Set_Animation_Interval(FLOAT _ITV)			{ Animation_Interval = _ITV;	  }

	_vec3	Get_PlayerPosition()						{ return PlayerPos;	 }
	VOID	Set_PlayerPosition(_vec3 _PPos)				{ PlayerPos = _PPos; }

	INT		Get_EnableGroundExp()						{ return Enable_GroundExplosion; }
	VOID	Set_EnableGroundExp(INT _EXP)				{ Enable_GroundExplosion = _EXP; }

	INT		Get_EnableQuadGroundExp()					{ return Enable_GroundQuadExplosion; }
	VOID	Set_EnableQuadGroundExp(INT _EXP)			{ Enable_GroundQuadExplosion = _EXP; }

	INT		Get_EnableMeteorExp()						{ return Enable_MeteorExplosion; }
	VOID	Set_EnableMeteorExp(INT _EXP)				{ Enable_MeteorExplosion = _EXP; }

	INT		Get_EnableCreateFireBall()					{ return Enable_CreateFireBall; }
	VOID	Set_EnableCreateFireBall(INT _EXP)			{ Enable_CreateFireBall = _EXP; }

	INT		Get_EnableRageUpFireBall()					{ return Enable_RageUpFireBall; }
	VOID	Set_EnableRageUpFireBall(INT _EXP)			{ Enable_RageUpFireBall = _EXP; }

	INT		Get_EnableExplosionRush()					{ return Enable_ExplosionRush; }
	VOID	Set_EnableExplosionRush(INT _EXP)			{ Enable_ExplosionRush = _EXP; }

	BOOL	Get_RageMode()								{ return Rage_Mode; }
	VOID	Set_RageMode(BOOL _EXP)						{ Rage_Mode = _EXP; }

	VOID	Set_StartPos(_vec3 _StartPos);

	VOID	Skill_GroundExplosion(CONST FLOAT& _DT);
	VOID	Skill_MeteorExplosion(CONST FLOAT& _DT);
	VOID	Skill_RSwingFireBall(CONST FLOAT& _DT);
	VOID	Skill_RageUpFireBall(CONST FLOAT& _DT);
	VOID	Skill_SupporterFlame(CONST FLOAT& _DT);
	VOID	Skill_ExplosionRush(CONST FLOAT& _DT);

	static	FinalBoss* Create(LPDIRECT3DDEVICE9 _GRPDEV);

private:
	virtual	VOID	Free();

public:
	enum class APPEAR_STAGING	{ SPOOL_APPEAR, SPOOL_FLOW1, SPOOL_FLOW2, SPOOL_FLOW3, EMBLEM_APPEAR, EMBLEM_DESTROY, ANIMATION,
								  WATER_POPUP, SMALL_FLAMEL, SMALL_FLAMER, SMALL_FLAMEC, BIG_FLAME, BIG_CIRCLE_FLAME, SPIRAL_FLAME, CAMERA_SHAKE };

	enum class DEATH_STAGING	{ SPOOL_APPEAR, SPOOL_FLOW1, SPOOL_FLOW2, SPOOL_FLOW3, SPOOL_FLOW4, SPOOL_FLOW5, SPOOL_FLOW6, 
								  WATER_POPUP1, WATER_POPUP2, WATER_POPUP3, WATER_POPUP4, BIG_FLAME, BIG_CIRCLE_FLAME, ELECTRIC, ENDING };

	enum class EXPLOSION		{ NORMAL_EXPLOSION, METEOR_SLAM_EXPLOSION1, METEOR_SLAM_EXPLOSION2, METEOR_SLAM_EXPLOSION3, METEOR_SLAM_EXPLOSION4 };

	enum class METEOR			{ DANGER_AREA, METEOR_CREATE, METEOR_EXPLOSION };

	enum class RUSH				{ RUSH_POOL, RUSH_FIREBALL1, RUSH_FIREBALL2, RUSH_FIREBALL3, RUSH_FIREBALL4, RUSH_EXPLOSION1, RUSH_EXPLOSION2, RUSH_EXPLOSION3, RUSH_EXPLOSION4, 
									RUSH_FLAME1, RUSH_FLAME2, RUSH_FLAME3, RUSH_FLAME4, RUSH_END };

	enum class SUPPORTER		{ SUP_POOLING, SUP1_SPAWN, SUP2_SPAWN, SUP3_SPAWN, SUP4_SPAWN, SUP5_SPAWN, SUP6_SPAWN, SUP7_SPAWN, SUP_SPAWN_END, 
									SUP_MOVE, SUP_FIRST_FIREBALL, SUP_SECOND_FIREBALL, SUP_DISAPPEAR, SUP_END};

	enum class FIREBALL			{ ANGLE_GENERATE, FIRST_FIREBALL, SECOND_FIREBALL, THIRD_FIREBALL, FOURTH_FIREBALL, FIFTH_FIREBALL, FIREBALL_POOL, END_FIREBALL };

private:
	Player*			PlayerObject;
	_vec3			PlayerPos;

	CameraObject*	Camera;

	BOOL			Invalidate_Mode;
	BOOL			Rage_Mode;
	BOOL			Action_Mode;
	BOOL			Death_Mode;

	FLOAT			Staging_Timer;
	FLOAT			Action_Timer;
	FLOAT			Explosion_Timer;
	FLOAT			MeteorExplosion_Timer;
	FLOAT			RSwing_Timer;
	FLOAT			RageUp_Timer;
	FLOAT			Supporter_Timer;
	FLOAT			Death_Timer;
	FLOAT			Rush_Timer;

	BOOL			STAGING_TRIGGER[20];
	BOOL			EXPLOSION_TRIGGER[5];
	BOOL			METEOR_TRIGGER[5];
	BOOL			FIREBALL_TRIGGER[(INT)FIREBALL::END_FIREBALL];
	BOOL			SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP_END];
	BOOL			ERUSH_TRIGGER[(INT)RUSH::RUSH_END];

	INT				Action_Selector;

	BOOL			DoubleSlam;

	INT				Enable_BossAppearStaging;
	INT				Enable_BossDisappearStaging;
	INT				Enable_GroundExplosion;
	INT				Enable_GroundQuadExplosion;
	INT				Enable_MeteorExplosion;
	INT				Enable_CreateFireBall;
	INT				Enable_RageUpFireBall;
	INT				Enable_SupporterFlame;
	INT				Enable_ExplosionRush;

	Transform*		MeteorTransform[4];
	FLOAT			RanPosX[4], RanPosZ[4];
	vector<GameObject*>	FireBallObjectPool;
	vector<GameObject*>	SupporterObjectPool;

	FLOAT			PlayerToAxisXDegree;
	_vec3			GeneratePos;
	INT				Numbering;

	vector<LPDIRECT3DTEXTURE9>*	Animation_TexList;
	FLOAT						Animation_Timer;
	FLOAT						Animation_Interval;
	INT							Animation_CurrentIndex;
	INT							Animation_PreviousIndex;
	INT							Animation_FrameCount;

	vector<LPDIRECT3DTEXTURE9>	Animation_NonAnim_TexList;
	vector<LPDIRECT3DTEXTURE9>	Animation_Appear_TexList;
	vector<LPDIRECT3DTEXTURE9>	Animation_Death_TexList;
	vector<LPDIRECT3DTEXTURE9>	Animation_Stunning_TexList;
	vector<LPDIRECT3DTEXTURE9>	Animation_RageUp_TexList;

	vector<LPDIRECT3DTEXTURE9>	Animation_Normal_Stand_TexList;
	vector<LPDIRECT3DTEXTURE9>	Animation_Normal_RSwing_TexList;
	vector<LPDIRECT3DTEXTURE9>	Animation_Normal_FSwing_TexList;
	vector<LPDIRECT3DTEXTURE9>	Animation_Normal_Slam_TexList;


	vector<LPDIRECT3DTEXTURE9>	Animation_Rage_Stand_TexList;
	vector<LPDIRECT3DTEXTURE9>	Animation_Rage_RSwing_TexList;
	vector<LPDIRECT3DTEXTURE9>	Animation_Rage_Slam_TexList;
	vector<LPDIRECT3DTEXTURE9>	Animation_Rage_Charge_TexList;

private:
	Buffer*			Component_Buffer;
	Texture*		Component_Texture;
	Transform*		Component_Transform;
	Collider*		Component_Collider;

	StateMachine*	FSM;
};