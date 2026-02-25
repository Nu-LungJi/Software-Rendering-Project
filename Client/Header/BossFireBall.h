#pragma once
#include "GameObject.h"
#include "FinalBoss.h"

#define ANIMATION_BOSS_FIREBALL	10

class BossFireBall : public GameObject {
private:
	explicit BossFireBall(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit BossFireBall(CONST GameObject& _RHS);
	virtual ~BossFireBall();

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

	static	BossFireBall* Create(LPDIRECT3DDEVICE9 _GRPDEV);

	VOID	Set_FireBall_Angle(FLOAT _Degree)	{ FireBall_DirectionAngle = _Degree;	}
	FLOAT	Get_FireBall_Angle()				{ return FireBall_DirectionAngle;		}

	VOID	Set_FireBall_Speed(FLOAT _Speed)	{ FireBall_Speed = _Speed;	}
	FLOAT	Get_FireBall_Speed()				{ return FireBall_Speed;	}

	VOID	Set_FireBall_Duration(FLOAT _Dur)	{ FireBall_Duration = _Dur; }
	FLOAT	Get_FireBall_Duration()				{ return FireBall_Duration; }

	VOID	Set_FireBall_Pos(_vec3 _Pos)		{ Component_Transform->Set_Pos(_Pos);			}
	_vec3	Get_FireBall_Pos()					{ return *Component_Transform->Get_Position();	}

	VOID	FireBall_Linear_Movement(_vec3* _Direction, FLOAT _Angle, FLOAT _Speed);
private:
	FinalBoss* Boss;
	Transform* BossPosition;

	vector<LPDIRECT3DTEXTURE9>* Animation_TexList;
	INT							Animation_CurrentIndex;
	INT							Animation_PreviousIndex;

	vector<LPDIRECT3DTEXTURE9>	Animation_NormalTexList;
	vector<LPDIRECT3DTEXTURE9>	Animation_RageTexList;

	FLOAT	Animation_Timer;
	INT		Animation_FrameCount;
	FLOAT	Animation_Interval;

	FLOAT	FireBall_Timer;
	FLOAT	FireBall_Duration;

	_vec3	Direction;

	_vec3	FireBallType;

	FLOAT	FireBall_DirectionAngle;
	FLOAT	FireBall_Speed;
	_vec3	FireBall_Pos;

private:
	Buffer*		Component_Buffer;
	Transform*	Component_Transform;
	Texture*	Component_Texture;
	Collider*	Component_Collider;

private:
	virtual		VOID	Free();
};