#pragma once
#include "GameObject.h"

class CameraObject : public GameObject {
private:
	explicit CameraObject(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit CameraObject(CONST GameObject& _RHS);
	virtual ~CameraObject();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			INT			Update_GameObject(CONST FLOAT& _DT);
	virtual			VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
	virtual			VOID		Render_GameObject() {};

	VOID			Camera_Transform_Control(CONST FLOAT& _DT);
	VOID			Camera_Rotation_Control(CONST FLOAT& _DT);
	VOID			Camera_Shaking(INT _Strength, FLOAT _Time);

	_matrix*		Get_ViewMatrix() { return &ViewMatrix; }
	_matrix*		Get_ProjMatrix() { return &ProjMatrix; }
	_matrix*		Get_WorldMatrix() { return Component_Transform->Get_World(); }

	_vec3*			Get_EyeVec() { return &EyeVec; }
	BOOL			Set_EyeVec(_vec3 _EyeVec) { AtVec += (_EyeVec - EyeVec); EyeVec = _EyeVec; return TRUE; }

	_vec3*			Get_AtVec() { return &AtVec; }
	BOOL			Set_AtVec(_vec3 _EyeVec) { EyeVec = _EyeVec; AtVec = _EyeVec;  AtVec.z += 1; return TRUE;}

	FLOAT*			Get_FOV()				{ return &FOVValue; }
	BOOL			Set_FOV(FLOAT _Value)	{ FOVValue = _Value; return TRUE;}

	FLOAT*			Get_Speed() { return &CameraSpeed; }
	BOOL			Set_Speed(FLOAT _Value) { CameraSpeed = _Value; return TRUE; }

	void			Set_Tracking_Player(BOOL bCameMove) { Camera_Move = bCameMove; }
	void			Set_Obj(GameObject* pDst, _vec3 Center) { pObj = pDst; vCenter = Center; }
	void			Set_Move(BOOL bMove) { StopMove = bMove; }
	void			CheonLog_Respawn(CONST FLOAT& _DT);
private:
	HRESULT			Component_Initialize();

private:
	Transform*	Component_Transform;

	_matrix		ViewMatrix, ProjMatrix;

	_vec3		DefaultEyeVec, DefaultAtVec;
	_vec3		EyeVec, AtVec, RightVec, UpVec, LookVec;
	FLOAT		FOVValue, AspectValue, NearValue, FarValue, CameraSpeed;

	FLOAT		RotationX, RotationY;

	_vec3		Angle;

	BOOL		MouseFix;
	BOOL		MouseCheck;
	BOOL		StopMove;

	//GameObject* Player;

	bool		Camera_Show;
	bool		Camera_Move;

	INT			Shake_Strength;
	FLOAT		Shake_Time;
	_vec3		OriginEye;
	_vec3		OriginAt;

	_vec3		vCenter;
	_vec3		vPlayer;
	_vec3		m_vVelocity;

	GameObject*		pObj;

public:
	static CameraObject* Create(LPDIRECT3DDEVICE9 _GRPDEV);
private:
	virtual VOID Free();
};