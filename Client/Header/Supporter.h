#pragma once
#include "GameObject.h"
#include "FinalBoss.h"
#include "BossFireBall.h"

#define ANIMATION_SUPPORTER_IDLE	24
#define ANIMATION_SUPPORTER_APPEAR	10
#define ANIMATION_SUPPORTER_APPEAR	3

class Supporter : public GameObject {
private:
	explicit Supporter(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit Supporter(CONST GameObject& _RHS);
	virtual ~Supporter();

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

	static	Supporter* Create(LPDIRECT3DDEVICE9 _GRPDEV);

	VOID	Set_StartAngle(FLOAT _Angle)	{ CurrentAngle = _Angle;	}
	VOID	Set_TickAngle(FLOAT _Angle)		{ TickAngle = _Angle;		}
	VOID	Set_StartPos(_vec3 _Pos)		{ Component_Transform->Set_Pos(_Pos); }
	VOID	Scale_Increment(CONST FLOAT& _DT);

	VOID	Set_SupporterType(INT _TYPE)	{ Supporter_Type = _TYPE;	}
	INT		Get_SupporterType()				{ return Supporter_Type;	}

	VOID	Set_ScaleInc(BOOL _INC)			{ Enable_ScaleInc = _INC;	}
	BOOL	Get_ScaleInc()					{ return Enable_ScaleInc;	}

	VOID	Set_DirectionVec(_vec3 _DIR)	{ Direction = _DIR; }
	_vec3	Get_DirectionVec()				{ return Direction; }

	VOID	Set_RageMove(BOOL _INC)			{ Rage_Movement = _INC;}
	BOOL	Get_RageMove()					{ return Rage_Movement;	}

	VOID	Normal_Supporter_Action(CONST FLOAT& _DT);
	VOID	Rage_Supporter_Action(CONST FLOAT& _DT);

private:
	vector<LPDIRECT3DTEXTURE9>*	Animation_TexList;
	vector<LPDIRECT3DTEXTURE9>	Animation_IdleTexList;
	vector<LPDIRECT3DTEXTURE9>	Animation_AppearTexList;
	vector<LPDIRECT3DTEXTURE9>	Animation_NonAnimTexList;
	INT							Animation_CurrentIndex;
	INT							Animation_PreviousIndex;

	FinalBoss* Boss;

	FLOAT	Effect_Timer;

	INT		Supporter_Type;

	FLOAT	Animation_Timer;
	INT		Animation_FrameCount;
	FLOAT	Animation_Interval;

	_vec3	Direction;
	FLOAT	CurrentAngle;
	FLOAT	TickAngle;
	INT		FBNumbering;

	BOOL	Enable_ScaleInc;
	FLOAT	Scale_Stack;

	BOOL	Rage_Movement;
	BOOL	Spiral_FireBall;
	vector<GameObject*> BFBVec;

	BOOL	SoundChecker;
private:
	Buffer*			Component_Buffer;
	Transform*		Component_Transform;
	Texture*		Component_Texture;
	Collider*		Component_Collider;
	 
private:
	virtual		VOID	Free();
};

