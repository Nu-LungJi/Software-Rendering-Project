#pragma once
#include "GameObject.h"
#include "FinalBoss.h"
#include "BossFireBall.h"

#define ANIMATION_SUPPORTER_IDLE 24

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

	VOID	Supporter_Transform(CONST FLOAT& _DT);
	VOID	Generate_FireBall(CONST FLOAT& _DT);

	VOID	Set_StartAngle(FLOAT _Angle)	{ CurrentAngle = _Angle; };
	VOID	Set_TickAngle(FLOAT _Angle)		{ TickAngle = _Angle; };
	VOID	Set_StartPos(_vec3 _Pos)		{ Component_Transform->Set_Pos(_Pos); };

private:
	vector<LPDIRECT3DTEXTURE9>	Animation_IdleTexList;
	INT							Animation_CurrentIndex;
	INT							Animation_PreviousIndex;

	FLOAT	Effect_Timer;

	FLOAT	Animation_Timer;
	INT		Animation_FrameCount;
	FLOAT	Animation_Interval;

	_vec3	Direction;
	FLOAT	CurrentAngle;
	FLOAT	TickAngle;
	INT		FBNumbering;

private:
	Buffer*			Component_Buffer;
	Transform*		Component_Transform;
	Texture*		Component_Texture;
	Collider*		Component_Collider;

private:
	virtual		VOID	Free();
};

