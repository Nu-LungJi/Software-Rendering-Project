#include "../Include/PCH.h"
#include "Camera.h"

CameraObject::CameraObject(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV), StopMove(true), FrustumPlane{} {}
CameraObject::CameraObject(const GameObject& _RHS)		: GameObject(_RHS)		{}
CameraObject::~CameraObject() {}

HRESULT CameraObject::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;

	//DefaultEyeVec = { 0.f,10.f * 1.35f,0.f };	DefaultAtVec = { 0.f,8.f * 1.35f, 1.35f };
	DefaultEyeVec = { 0.f,10.f * 1.35f * 1.3f,0.f };	DefaultAtVec = { 0.f,8.f * 1.35f * 1.3f, 1.35f * 1.3f };
	EyeVec = DefaultEyeVec;			AtVec = DefaultAtVec;				UpVec = { 0.f,1.f,0.f };
	FOVValue = D3DXToRadian(60.f);		AspectValue = (_float)WINCX / WINCY;	NearValue = 0.1f; FarValue = 1000.f;

	Angle = { 0.f, 0.f, 0.f };			CameraSpeed = 10.f;

	Shake_Strength = 0;
	Shake_Time = 0.f;

	MouseCheck = FALSE;

	Camera_Show = TRUE;
	Camera_Move = FALSE;

	D3DXMatrixLookAtLH(&ViewMatrix, &EyeVec, &AtVec, &UpVec);
	D3DXMatrixPerspectiveFovLH(&ProjMatrix, FOVValue, AspectValue, NearValue, FarValue);

	GRPDEV->SetTransform(D3DTS_VIEW, &ViewMatrix);
	GRPDEV->SetTransform(D3DTS_PROJECTION, &ProjMatrix);

	m_vVelocity = {0.f , 0.f, 0.f};

	ObjectTAG = L"Camera";

	OriginEye = EyeVec;
	OriginAt = AtVec;

	return S_OK;
}
INT	CameraObject::Update_GameObject(const _float& _DT) {

	CameraObject::Update_Frustum();

	if (KEY_DOWN(DIK_F2)) {	//	마우스 커서 고정 여부 TRUE = 고정, FALSE = 고정 해제
		MouseCheck ? MouseCheck = FALSE : MouseCheck = TRUE;
		Camera_Move ? Camera_Move = FALSE : Camera_Move = TRUE;
	}

	CheonLog_Respawn(_DT);

	if (!Camera_Move)
	{
		Player* player = dynamic_cast<Player*> (SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Player"));

		_vec3* playerPos = (dynamic_cast<Transform*>(player->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM)))->Get_Position();
	
		_vec3 eyeCalc = { 0.f, DefaultEyeVec.y - 1.f, -5.f };
		_vec3 atCalc = { 0.f, DefaultAtVec.y - 1.f, -4.f };

		EyeVec = (*playerPos) + eyeCalc;
		AtVec = (*playerPos) + atCalc;

		_float distance = player->Get_MouseDistance();
		_vec3 dir = player->Get_MouseDir();
		_float offset = 2.f;

		_vec3 targetEye = EyeVec;
		_vec3 targetAt = AtVec;

		_float moveAmount = (distance - offset) * 3;
		if (distance > offset)
		{
			targetEye += dir * moveAmount;
			targetAt  += dir * moveAmount;
		}

		//float smoothSpeed = 8.f;
		//
		//EyeVec += (targetEye - EyeVec) * smoothSpeed * 0.01;
		//AtVec += (targetAt - AtVec) * smoothSpeed * 0.01;

		_float stiffness = 40.f;
		_float damping = 2.f * sqrtf(stiffness);
		
		_vec3 toTarget = targetEye - EyeVec;
		
		_vec3 accel = toTarget * stiffness - m_vVelocity * damping;
		
		if(moveAmount > 0.1f)
			m_vVelocity += accel * 0.02;
		
		EyeVec += m_vVelocity * 0.02;
		AtVec += m_vVelocity * 0.02;
	}
	if (Shake_Time > 0.f) {
		OriginEye = EyeVec;
		OriginAt = AtVec;

		FLOAT RADX = (FLOAT)(rand() % (2 * Shake_Strength + 1) - Shake_Strength) / 100.f;
		FLOAT RADY = (FLOAT)(rand() % (2 * Shake_Strength + 1) - Shake_Strength) / 100.f;
		FLOAT RADZ = (FLOAT)(rand() % (2 * Shake_Strength + 1) - Shake_Strength) / 100.f;
	
		EyeVec = { EyeVec.x + RADX, EyeVec.y + RADY, EyeVec.z + RADZ };
		AtVec = { AtVec.x + RADX, AtVec.y + RADY, AtVec.z + RADZ };
	}

	D3DXMatrixLookAtLH(&ViewMatrix, &EyeVec, &AtVec, &UpVec);
	GRPDEV->SetTransform(D3DTS_VIEW, &ViewMatrix);

	if (Shake_Time > 0.f) {
		Shake_Time -= _DT;
		EyeVec = OriginEye;
		AtVec = OriginAt;
	}
	

	return 0;
}
VOID CameraObject::LateUpdate_GameObject(const _float& _DT) {
	Camera_Transform_Control(_DT);

	if (MouseCheck) {
		POINT       ptMouse{ WINCX >> 1, WINCY >> 1 };

		ClientToScreen(hWnd, &ptMouse);
		SetCursorPos(ptMouse.x, ptMouse.y);

		Camera_Rotation_Control(_DT);
	}
}

VOID CameraObject::Camera_Transform_Control(CONST FLOAT& _DT) {

	_matrix CameraMatrix;
	_vec3	SideVector, FrontVector, UpVector;

	D3DXMatrixInverse(&CameraMatrix, 0, &ViewMatrix);

	if (Camera_Move)
	{
		if (KEY_HOLD(DIK_W)) {
			memcpy(&FrontVector, &CameraMatrix.m[2][0], sizeof(_vec3));

			_vec3 Length = *D3DXVec3Normalize(&FrontVector, &FrontVector) * _DT * CameraSpeed;
			EyeVec += Length; AtVec += Length;
		}
		if (KEY_HOLD(DIK_S)) {
			memcpy(&FrontVector, &CameraMatrix.m[2][0], sizeof(_vec3));

			_vec3 Length = *D3DXVec3Normalize(&FrontVector, &FrontVector) * _DT * CameraSpeed;
			EyeVec -= Length; AtVec -= Length;
		}
		if (KEY_HOLD(DIK_D)) {
			memcpy(&SideVector, &CameraMatrix.m[0][0], sizeof(_vec3));

			_vec3 Length = *D3DXVec3Normalize(&SideVector, &SideVector) * _DT * CameraSpeed;
			EyeVec += Length; AtVec += Length;
		}
		if (KEY_HOLD(DIK_A)) {
			memcpy(&SideVector, &CameraMatrix.m[0][0], sizeof(_vec3));

			_vec3 Length = *D3DXVec3Normalize(&SideVector, &SideVector) * _DT * CameraSpeed;
			EyeVec -= Length; AtVec -= Length;
		}
		if (KEY_HOLD(DIK_RCONTROL)) {
			memcpy(&UpVector, &CameraMatrix.m[1][0], sizeof(_vec3));

			_vec3 Length = *D3DXVec3Normalize(&UpVector, &UpVector) * _DT * CameraSpeed;
			EyeVec += Length; AtVec += Length;
		}
		if (KEY_HOLD(DIK_LCONTROL)) {
			memcpy(&UpVector, &CameraMatrix.m[1][0], sizeof(_vec3));

			_vec3 Length = *D3DXVec3Normalize(&UpVector, &UpVector) * _DT * CameraSpeed;
			EyeVec -= Length; AtVec -= Length;
		}
		if (KEY_DOWN(DIK_F1)) {		//	GUI 상태 바 숨김 여부 TRUE = Visible, FALSE = Hide
			Camera_Show ? Camera_Show = FALSE : Camera_Show = TRUE;
		}
	}
}
VOID CameraObject::Camera_Rotation_Control(CONST FLOAT& _DT){
	_matrix CameraMatrix;
	D3DXMatrixInverse(&CameraMatrix, 0, &ViewMatrix);

	LONG MouesAngle = 0;

	if (Camera_Move)
	{
		if (MouesAngle = MOUSE_MOVE(DIMS_Y)) {
			_matrix RotationMat;
			memcpy(&RightVec, &CameraMatrix.m[0][0], sizeof(_vec3));

			LookVec = AtVec - EyeVec;

			D3DXMatrixRotationAxis(&RotationMat, &RightVec, D3DXToRadian(MouesAngle / 10.f));

			D3DXVec3TransformNormal(&LookVec, &LookVec, &RotationMat);

			AtVec = EyeVec + LookVec;
		}
		if (MouesAngle = MOUSE_MOVE(DIMS_X)) {
			_matrix RotationMat;

			LookVec = AtVec - EyeVec;

			D3DXMatrixRotationAxis(&RotationMat, &UpVec, D3DXToRadian(MouesAngle / 10.f));

			D3DXVec3TransformNormal(&LookVec, &LookVec, &RotationMat);

			AtVec = EyeVec + LookVec;
		}
	}
}

VOID CameraObject::Camera_Shaking(INT _Strength, FLOAT _Time) {
	Shake_Strength = _Strength;
	Shake_Time = _Time;
}

VOID CameraObject::Update_Frustum()
{
	_matrix matVP = ViewMatrix * ProjMatrix;

	FrustumPlane[(uint8_t)FRUSTUMPLANE::Left].a   = matVP._14 + matVP._11;
	FrustumPlane[(uint8_t)FRUSTUMPLANE::Left].b   = matVP._24 + matVP._21;
	FrustumPlane[(uint8_t)FRUSTUMPLANE::Left].c   = matVP._34 + matVP._31;
	FrustumPlane[(uint8_t)FRUSTUMPLANE::Left].d   = matVP._44 + matVP._41;

	FrustumPlane[(uint8_t)FRUSTUMPLANE::Right].a  = matVP._14 - matVP._11;
	FrustumPlane[(uint8_t)FRUSTUMPLANE::Right].b  = matVP._24 - matVP._21;
	FrustumPlane[(uint8_t)FRUSTUMPLANE::Right].c  = matVP._34 - matVP._31;
	FrustumPlane[(uint8_t)FRUSTUMPLANE::Right].d  = matVP._44 - matVP._41;

	FrustumPlane[(uint8_t)FRUSTUMPLANE::bottom].a = matVP._14 + matVP._12;
	FrustumPlane[(uint8_t)FRUSTUMPLANE::bottom].b = matVP._24 + matVP._22;
	FrustumPlane[(uint8_t)FRUSTUMPLANE::bottom].c = matVP._34 + matVP._32;
	FrustumPlane[(uint8_t)FRUSTUMPLANE::bottom].d = matVP._44 + matVP._42;

	FrustumPlane[(uint8_t)FRUSTUMPLANE::Top].a    = matVP._14 - matVP._12;
	FrustumPlane[(uint8_t)FRUSTUMPLANE::Top].b    = matVP._24 - matVP._22;
	FrustumPlane[(uint8_t)FRUSTUMPLANE::Top].c    = matVP._34 - matVP._32;
	FrustumPlane[(uint8_t)FRUSTUMPLANE::Top].d    = matVP._44 - matVP._42;

	FrustumPlane[(uint8_t)FRUSTUMPLANE::Near].a   = matVP._13;
	FrustumPlane[(uint8_t)FRUSTUMPLANE::Near].b   = matVP._23;
	FrustumPlane[(uint8_t)FRUSTUMPLANE::Near].c   = matVP._33;
	FrustumPlane[(uint8_t)FRUSTUMPLANE::Near].d   = matVP._43;

	FrustumPlane[(uint8_t)FRUSTUMPLANE::Far].a    = matVP._14 - matVP._13;
	FrustumPlane[(uint8_t)FRUSTUMPLANE::Far].b    = matVP._24 - matVP._23;
	FrustumPlane[(uint8_t)FRUSTUMPLANE::Far].c    = matVP._34 - matVP._33;
	FrustumPlane[(uint8_t)FRUSTUMPLANE::Far].d    = matVP._44 - matVP._43;

	for (uint8_t i = 0; i < (uint8_t)FRUSTUMPLANE::End; ++i)
	{
		D3DXPlaneNormalize(&FrustumPlane[i], &FrustumPlane[i]);
	}
}

BOOL CameraObject::IsIn_Frustum(_vec3 _vPos, _float _fRadius)
{
	if (this == NULL)	return false;

	for (uint8_t i = 0; i < (uint8_t)FRUSTUMPLANE::End; ++i)
	{
		if (FrustumPlane[i].a * _vPos.x +
			FrustumPlane[i].b * _vPos.y +
			FrustumPlane[i].c * _vPos.z +
			FrustumPlane[i].d < -_fRadius)
		{
			return FALSE;
		}
	}
	return true;
}


void CameraObject::CheonLog_Respawn(CONST FLOAT& _DT)
{
	_vec3 vLook, vDistance,vPos, vAt, vEye;
	_float fLength(0.f);
	if (!StopMove)
	{
		Player* player = dynamic_cast<Player*> (SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Player"));
		_vec3* playerPos = (dynamic_cast<Transform*>(player->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM)))->Get_Position();

		Camera_Move = true;
		vCenter.y = 0.5;
		_vec3 eyeCalc = { 0.f, DefaultEyeVec.y - 1.f, -5.f };
		_vec3 atCalc  = { 0.f, DefaultAtVec.y - 1.f, -4.f };

		EyeVec = vCenter + eyeCalc;
		AtVec  = vCenter + atCalc;
	
		if (player->Get_CameraMove())
		{
			vLook = vCenter - *playerPos;
		}
		else
		{
			vCenter.y = 6.f;
			vLook = vCenter - vPlayer;
		}
		
		fLength = D3DXVec3Length(&vLook);

		D3DXVec3Normalize(&vLook, &vLook);
		if (fLength <= 23)
		{
			EyeVec = vPlayer + eyeCalc + vLook * _DT * 6.f;
			AtVec  = vPlayer + atCalc  + vLook * _DT * 6.f;

			vPlayer += vLook * _DT * 6.f;

			if (fLength <= 11)
			{
				dynamic_cast<Spawner*>(pObj)->Set_Spawn(false);
				StopMove = true;
				return;
			}
		
			player->Set_CameraMove(false);
		}
		else
		{
			dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Player")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Move_Pos(&vLook, 5.f, _DT);
			EyeVec = (*playerPos) + eyeCalc;
			AtVec =  (*playerPos) + atCalc;
			vPlayer = (*playerPos);
		}
		
	}
	

}

HRESULT CameraObject::Component_Initialize() {

	//Component_Transform = ADD_COMPONENT_TRANSFORM;

	return S_OK;
}
CameraObject* CameraObject::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	CameraObject* CAM = new CameraObject(_GRPDEV);
	if (FAILED(CAM->Ready_GameObject())) {
		MSG_BOX("Cannot Create Camera.");
		Safe_Release(CAM);
		return nullptr;
	}
	return CAM;
}
VOID CameraObject::Free() {
	GameObject::Free();
}