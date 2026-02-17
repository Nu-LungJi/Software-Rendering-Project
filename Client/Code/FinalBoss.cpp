#include "../Include/PCH.h"
#include "FinalBoss.h"
#include "FSM.h"

FinalBoss::FinalBoss(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV)	{}
FinalBoss::FinalBoss(CONST GameObject& _RHS)	: GameObject(_RHS)		{}
FinalBoss::~FinalBoss()													{}

HRESULT	FinalBoss::Ready_GameObject()						{
	ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/Appear");
	//ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/Stand");
	//ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/Effect");
	//ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/Death");
	if (FAILED(Component_Initialize()))	return E_FAIL;
	if (FAILED(Texture_Initialize()))	return E_FAIL;

	PlayerObject = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_GameObject(L"Player"));

	BossHP = 1000.f;

	Invalidate_Mode = TRUE;
	Rage_Mode		= FALSE;

	Animation_Timer		 = 0.f;
	Animation_Interval	 = 0.15f;
	Animation_Index		 = 0;
	Animation_FrameCount = ANIMATION_APPEAR_FRAMECOUNT;

	Animation_TexList = &Animation_Appear_TexList;

	CameraObject* Camera = dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Camera"));

	_vec3 cameraDir = *(Camera->Get_EyeVec()) - *(Camera->Get_AtVec());
	_vec3 planeDir = { 0.f, 1.f, 0.f };

	_float angle = acosf(D3DXVec3Dot(D3DXVec3Normalize(&cameraDir, &cameraDir), D3DXVec3Normalize(&planeDir, &planeDir)));
	_float _cameraAngle = angle / D3DX_PI * 180.f;

	Component_Transform->Rotation(ROT_X, 90.f - _cameraAngle);

	return S_OK;
}
INT		FinalBoss::Update_GameObject(CONST FLOAT& _DT)		{
	GameObject::Update_GameObject(_DT);
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	//if (KEY_DOWN(DIK_I)) {
	//	_vec3 a = { 2.f, 2.f, 2.f };
	//	PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::APPEAR_EFFECT_EMBLEM, Component_Transform->Get_Position(), a, 0.5f);
	//	PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::APPEAR_EFFECT_ARM, Component_Transform->Get_Position(), a, 0.5f);
	//	PLAY_BOSS_EFFECT(BOSS_EFFECT::APPEAR_EFFECT_SPOOL, Component_Transform->Get_Position(), a, 0.5f);
	//}

	if (Animation_TexList == &Animation_Appear_TexList && Animation_Index == ANIMATION_APPEAR_FRAMECOUNT - 1) {
		Animation_Index = 0;
		Animation_TexList = &Animation_Stand_Normal_TexList;
		Animation_FrameCount = ANIMATION_STAND_NORMAL_FRAMECOUNT;
		Component_StateMachine->FSM_StateChange(StandState::GetInstance()->Instance());
	}

	if (BossHP <= 0) {
		Animation_Index = 0;
		Animation_TexList = &Animation_Death_TexList;
		Animation_FrameCount = ANIMATION_DEATH_FRAMECOUNT;
		Component_StateMachine->FSM_StateChange(DeadState::GetInstance()->Instance());
	}

	if (BossHP <= 500 && Rage_Mode == FALSE) {
		Animation_Index = 0;
		Animation_TexList = &Animation_Death_TexList;
		Animation_FrameCount = ANIMATION_DEATH_FRAMECOUNT;

		Component_StateMachine->FSM_StateChange(RageUpState::GetInstance()->Instance());
		Rage_Mode = TRUE;
	}
	Animation_Timer += _DT;

	return 0;
}
VOID	FinalBoss::LateUpdate_GameObject(CONST FLOAT& _DT)	{
	GameObject::LateUpdate_GameObject(_DT);
	if (Animation_Timer > Animation_Interval) {
		Animation_Index = Animation_Index % (Animation_FrameCount - 1) + 1;
		Animation_Timer = 0.f;
	}
}
VOID	FinalBoss::Render_GameObject() {
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	GRPDEV->SetTexture(0, (*Animation_TexList)[Animation_Index]);

	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

BOOL	FinalBoss::OnCollisionEnter(GameObject* _Other) {
	if (_Other->Get_ObjectTag() == L"Arrow" && Invalidate_Mode == FALSE) {
		BossHP -= dynamic_cast<Arrow*>(_Other)->Get_Atk();
		// Delete Arrow Code
	}
	else if (_Other->Get_ObjectTag() == L"Arrow" && Invalidate_Mode == TRUE) {
		// Delete Arrow Code
	}
	if (_Other->Get_ObjectTag() == L"Player") {
		Player* PlayerObject = dynamic_cast<Player*>(_Other);
		PlayerObject->Set_HP(PlayerObject->Get_HP() - 1);
	}
	return TRUE;
}
BOOL	FinalBoss::OnCollisionStay(GameObject* _Other) {
	return TRUE;
}
BOOL	FinalBoss::OnCollisionExit(GameObject* _Other) {
	return TRUE;
}

HRESULT	FinalBoss::Component_Initialize() {
	Component_Buffer		= ADD_COMPONENT_RECTTEX;
	Component_Texture		= ADD_COMPONENT_TEXTURE;

	Component_Transform		= ADD_COMPONENT_TRANSFORM;
	Component_Transform->Set_Pos(5.f, 1.f, 5.f);
	Component_Transform->Set_Scale(3.f, 3.f, 3.f);

	Component_Collider		= ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);
	Component_Collider->Set_Scale(1.f, 1.f, 1.f);

	Component_StateMachine	= ADD_COMPONENT_FSM;
	
	Component_StateMachine->FSM_StateInit(AppearState::GetInstance()->Instance());

	return S_OK;
}
HRESULT FinalBoss::Texture_Initialize() {
	wstring Base = L"";

	for (INT PIC = 1; PIC <= ANIMATION_APPEAR_FRAMECOUNT; ++PIC) {
		Base = L"Boss_Appear" + to_wstring(PIC) + L".png";
		Animation_Appear_TexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}

	for (INT PIC = 1; PIC <= ANIMATION_DEATH_FRAMECOUNT; ++PIC) {
		Base = L"Boss_Death" + to_wstring(PIC) + L".png";
		Animation_Death_TexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}

	for (INT PIC = 1; PIC <= ANIMATION_RAGEUP_FRAMECOUNT; ++PIC) {
		Base = L"Boss_RageUp" + to_wstring(PIC) + L".png";
		Animation_RageUp_TexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}

	for (INT PIC = 1; PIC <= ANIMATION_RSWING_NORMAL_FRAMECOUNT; ++PIC) {
		Base = L"Boss_Normal_RSwing" + to_wstring(PIC) + L".png";
		Animation_RSwing_Normal_TexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}

	for (INT PIC = 1; PIC <= ANIMATION_RSWING_RAGE_FRAMECOUNT; ++PIC) {
		Base = L"Boss_Rage_RSwing" + to_wstring(PIC) + L".png";
		Animation_RSwing_Rage_TexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}

	for (INT PIC = 1; PIC <= ANIMATION_FSWING_RAGE_FRAMECOUNT; ++PIC) {
		Base = L"Boss_FullSwing" + to_wstring(PIC) + L".png";
		Animation_FSwing_TexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}

	for (INT PIC = 1; PIC <= ANIMATION_STAND_NORMAL_FRAMECOUNT; ++PIC) {
		Base = L"Boss_Normal_Stand" + to_wstring(PIC) + L".png";
		Animation_Stand_Normal_TexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}

	for (INT PIC = 1; PIC <= ANIMATION_STAND_RAGE_FRAMECOUNT; ++PIC) {
		Base = L"Boss_Rage_Stand" + to_wstring(PIC) + L".png";
		Animation_Stand_Rage_TexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}

	for (INT PIC = 1; PIC <= ANIMATION_SLAM_NORMAL_FRAMECOUNT; ++PIC) {
		Base = L"Boss_Normal_THSlam" + to_wstring(PIC) + L".png";
		Animation_Slam_Normal_TexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}

	for (INT PIC = 1; PIC <= ANIMATION_SLAM_RAGE_FRAMECOUNT; ++PIC) {
		Base = L"Boss_Rage_THSlam" + to_wstring(PIC) + L".png";
		Animation_Slam_Rage_TexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}

	return S_OK;
}

FinalBoss*	FinalBoss::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	FinalBoss* FBS = new FinalBoss(_GRPDEV);
	if (FAILED(FBS->Ready_GameObject())) {
		MSG_BOX("Cannot Create FinalBoss.");
		Safe_Release(FBS);
		return nullptr;
	}
	return FBS;
}
VOID		FinalBoss::Free() {
	GameObject::Free();
}