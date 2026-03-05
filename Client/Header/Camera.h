#pragma once
#include "GameObject.h"
#include "Player.h"

enum class FRUSTUMPLANE : uint8_t { Left, Right, bottom, Top, Far, Near, End };

class Player;

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

	VOID			 Update_Frustum();
	const D3DXPLANE* Get_FrustumPlane(FRUSTUMPLANE side = FRUSTUMPLANE::End) { return (side == FRUSTUMPLANE::End) ? FrustumPlane : &FrustumPlane[(uint8_t)side]; }
	BOOL			 IsIn_Frustum(_vec3 _vPos, _float _fRadius);

	_matrix* Get_ViewMatrix() { return &ViewMatrix; }
	_matrix* Get_ProjMatrix() { return &ProjMatrix; }
	_matrix* Get_WorldMatrix() { return Component_Transform->Get_World(); }

	_vec3* Get_EyeVec() { return &EyeVec; }
	BOOL			Set_EyeVec(_vec3 _EyeVec) { AtVec += (_EyeVec - EyeVec); EyeVec = _EyeVec; return TRUE; }

	_vec3* Get_AtVec() { return &AtVec; }
	BOOL			Set_AtVec(_vec3 _EyeVec) { EyeVec = _EyeVec; AtVec = _EyeVec;  AtVec.z += 1; return TRUE; }

	FLOAT* Get_FOV() { return &FOVValue; }
	BOOL			Set_FOV(FLOAT _Value) { FOVValue = _Value; return TRUE; }

	FLOAT* Get_Speed() { return &CameraSpeed; }
	BOOL			Set_Speed(FLOAT _Value) { CameraSpeed = _Value; return TRUE; }

	void			Set_Tracking_Player(BOOL bCameMove) { Camera_Move = bCameMove; }
	void			Set_Obj(GameObject* pDst, _vec3 Center) { pObj = pDst; vCenter = Center; }
	void			Set_Move(BOOL bMove) { StopMove = bMove; }
	void			CheonLog_Respawn(CONST FLOAT& _DT);
	void			Docheol_Spawn(CONST FLOAT& _DT);

	VOID			Set_FocusOnBoss(BOOL _FOB);
	BOOL			Get_FocusOnBoss() { return FocusOn_Boss; }

	VOID			Set_ButtonLock(BOOL _BTL) { Button_Lock = _BTL; }
	BOOL			Get_ButtonLock() { return Button_Lock; }

	VOID			Set_EnableQuickZoom(INT _QZM) { Enable_QuickZoom = _QZM; }
	BOOL			Get_EnableQuickZoom() { return Enable_QuickZoom; }
private:
	HRESULT			Component_Initialize();
	VOID			Camera_QuickZoom(CONST FLOAT& _DT);
private:
	Transform* Component_Transform;

	_matrix		ViewMatrix, ProjMatrix;

	_vec3		DefaultEyeVec, DefaultAtVec;
	_vec3		EyeVec, AtVec, RightVec, UpVec, LookVec;
	FLOAT		FOVValue, AspectValue, NearValue, FarValue, CameraSpeed;

	FLOAT		RotationX, RotationY;

	_vec3		Angle;

	BOOL		MouseFix;
	BOOL		MouseCheck;
	BOOL		StopMove;
	BOOL		Velocity_Lock;
	BOOL		Button_Lock;

	Player* PlayerObject;

	bool		Camera_Show;
	bool		Camera_Move;

	INT			Shake_Strength;
	FLOAT		Original_Shake_Time;
	FLOAT		Shake_Time;
	_vec3		OriginEye;
	_vec3		OriginAt;

	_vec3		vCenter;
	_vec3		vPlayer;
	_vec3		m_vVelocity;

	GameObject* pObj;
	D3DXPLANE	FrustumPlane[(uint8_t)FRUSTUMPLANE::End];

	////////////////////////////////////////////// 보스전 활용 변수들
	BOOL		FocusOn_Boss;
	FLOAT		Focusing_Timer;
	_vec3		OriginCameraPos;
	_vec3		OriginCameraAt;

	INT			Enable_QuickZoom;
	FLOAT		QZoom_Timer;
	////////////////////////////////////////////// 보스전 활용 변수들
public:
	static CameraObject* Create(LPDIRECT3DDEVICE9 _GRPDEV);
private:
	virtual VOID Free();


	/////////// 미니게임 b 활용 변수들
public:
	void		Set_Target(Player* _Obj) { m_pTarget = _Obj; }
	void		Start_MiniGame();
	void		Exit_MiniGame();

private:
	SCENE_TYPE	m_eCurrScene;
	_float		m_fOffset[(uint8_t)FRUSTUMPLANE::End];

	Player*		m_pTarget;
	HRESULT		MiniGame(const _float& _DT);

	_float		m_fElapsedTime;
	_float		m_fDuration = 1.f;
	_vec3		m_vCurrEye,		m_vCurrUp;
	_vec3		m_vStartEye,	m_vTargetEye;
	_vec3		m_vStartUp,		m_vTargetUp;
	bool		Is_Changing;

};