#pragma once
#include "GameObject.h"
#include "Player.h"
#include "StateMachine.h"

#define		ANIMATION_NONANIM_FRAMECOUNT		3

#define		ANIMATION_APPEAR_FRAMECOUNT			39
#define		ANIMATION_DEATH_FRAMECOUNT			27
#define		ANIMATION_RAGEUP_FRAMECOUNT			28
#define		ANIMATION_STUNNING_FRAMECOUNT		10

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

	VOID	Animation_Appear_Staging(CONST FLOAT& _DT);

	INT		Get_Animation_CurrentIndex()				{ return Animation_CurrentIndex;  }
	VOID	Set_Animation_CurrentIndex(INT _IDX)		{ Animation_CurrentIndex = _IDX;  }

	INT		Get_Animation_PreviousIndex()				{ return Animation_PreviousIndex; }
	VOID	Set_Animation_PreviousIndex(INT _IDX)		{ Animation_PreviousIndex = _IDX; }

	INT		Get_Animation_Interval()					{ return Animation_Interval;	  }
	VOID	Set_Animation_Interval(FLOAT _ITV)			{ Animation_Interval = _ITV;	  }

	_vec3	Get_PlayerPosition()						{ return PlayerPos;		}
	VOID	Set_PlayerPosition(_vec3 _PPos)				{ PlayerPos = _PPos;	}

	INT		Get_EnableGroundExp()						{ return Enable_GroundExplosion; }
	VOID	Set_EnableGroundExp(BOOL _EXP)				{ Enable_GroundExplosion = _EXP; }

	INT		Get_EnableQuadGroundExp()					{ return Enable_GroundQuadExplosion; }
	VOID	Set_EnableQuadGroundExp(BOOL _EXP)			{ Enable_GroundQuadExplosion = _EXP; }

	INT		Get_EnableMeteorExp()						{ return Enable_MeteorExplosion; }
	VOID	Set_EnableMeteorExp(BOOL _EXP)				{ Enable_MeteorExplosion = _EXP; }

	INT		Get_EnableCreateFireBall()					{ return Enable_CreateFireBall; }
	VOID	Set_EnableCreateFireBall(BOOL _EXP)			{ Enable_CreateFireBall = _EXP; }

	VOID	Set_StartPos(_vec3 _StartPos);

	VOID	Skill_GroundExplosion(CONST FLOAT& _DT);
	VOID	Skill_MeteorExplosion(CONST FLOAT& _DT);
	VOID	Skill_RSwingFireBall(CONST FLOAT& _DT);
	VOID	Skill_FSwingFireBall(CONST FLOAT& _DT);
	VOID	Skill_RageUpFireBall(CONST FLOAT& _DT);

	static	FinalBoss* Create(LPDIRECT3DDEVICE9 _GRPDEV);

private:
	virtual	VOID	Free();

public:
	enum class STAGING		{ SPOOL_APPEAR, SPOOL_FLOW1, SPOOL_FLOW2, SPOOL_FLOW3, EMBLEM_APPEAR, EMBLEM_DESTROY, ANIMATION,
								WATER_POPUP, SMALL_FLAMEL, SMALL_FLAMER, SMALL_FLAMEC, BIG_FLAME, BIG_CIRCLE_FLAME, SPIRAL_FLAME, CAMERA_SHAKE };

	enum class EXPLOSION	{ NORMAL_EXPLOSION, METEOR_SLAM_EXPLOSION1, METEOR_SLAM_EXPLOSION2, METEOR_SLAM_EXPLOSION3, METEOR_SLAM_EXPLOSION4 };

	enum class METEOR		{ DANGER_AREA, METEOR_CREATE, METEOR_EXPLOSION };

	enum class FIREBALL		{ ANGLE_GENERATE, FIRST_FIREBALL, SECOND_FIREBALL, THIRD_FIREBALL };

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

	BOOL			STAGING_TRIGGER[20];
	BOOL			EXPLOSION_TRIGGER[5];
	BOOL			METEOR_TRIGGER[5];
	BOOL			FIREBALL_TRIGGER[5];

	INT				Action_Selector;

	BOOL			DoubleSlam;

	BOOL			Enable_BossAppearStaging;
	BOOL			Enable_GroundExplosion;
	BOOL			Enable_GroundQuadExplosion;
	BOOL			Enable_MeteorExplosion;
	BOOL			Enable_CreateFireBall;

	Transform*		MeteorTransform[4];
	FLOAT			RanPosX[4], RanPosZ[4];

	FLOAT			PlayerToAxisXDegree;
	_vec3			GeneratePos;

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