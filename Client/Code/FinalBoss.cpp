#include "../Include/PCH.h"
#include "FinalBoss.h"

FinalBoss::FinalBoss(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV)	{}
FinalBoss::FinalBoss(CONST GameObject& _RHS)	: GameObject(_RHS)		{}
FinalBoss::~FinalBoss()													{}

HRESULT	FinalBoss::Ready_GameObject()						{
	//ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/Appear");
	ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/Stand");
	//ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/RageUp");
	//ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/RightSwing");
	//ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/FullSwing");
	ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/TwoHandSlam");
	ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/Effect");
	//ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/Death");
	if (FAILED(Component_Initialize()))	return E_FAIL;
	if (FAILED(Texture_Initialize()))	return E_FAIL;

	PlayerObject = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_GameObject(L"Player"));

	FSM = StateMachine::Create(GRPDEV);
	FSM->FSM_StateInit(AppearState::GetInstance()->Instance());
	FSM->FSM_SetOwner(this);

	BossHP = 1000.f;

	Invalidate_Mode = TRUE;		// 피해 무효화
	Rage_Mode		= FALSE;	// 폭주화 단계
	Action_Mode		= TRUE;		// 다른 행동 간섭 방지
	Death_Mode		= FALSE;

	Animation_Timer		 = 0.f;
	Animation_Interval	 = 0.07f;
	Animation_Index		 = 0;
	Animation_FrameCount = ANIMATION_STAND_NORMAL_FRAMECOUNT;

	Animation_TexList = &Animation_Stand_Normal_TexList;

	Invalidate_Mode = FALSE;		// 피해 무효화
	Action_Mode = FALSE;		// 다른 행동 간섭 방지

	Action_Selector = 0;
	Action_Timer = 0.f;

	DoubleSlam = TRUE;

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
	FSM->Update_GameObject(_DT);

	Animation_Timer += _DT;
	if (Invalidate_Mode == FALSE || Action_Mode == FALSE) {
		Action_Timer += _DT;
	}
	
	if (Action_Timer > 3.f) {
		srand(time(NULL));
		Action_Selector = 3;// rand() % 3 + 1;
		Action_Timer = 0.f;
	}

	if (Rage_Mode == FALSE) {
		// < Stand -> RSwing >
		if (Animation_TexList == &Animation_Stand_Normal_TexList && Action_Selector == 1) {
			Animation_Index = 0;
			Animation_TexList = &Animation_RSwing_Normal_TexList;
			Animation_FrameCount = ANIMATION_RSWING_NORMAL_FRAMECOUNT;

			FSM->FSM_StateChange(RSwingState::GetInstance()->Instance());
			Action_Mode		= TRUE;
			Action_Selector = 0;
		}
		// < Stand -> FSwing >
		if (Animation_TexList == &Animation_Stand_Normal_TexList && Action_Selector == 2) {
			Animation_Index = 0;
			Animation_TexList = &Animation_FSwing_TexList;
			Animation_FrameCount = ANIMATION_FSWING_FRAMECOUNT;

			FSM->FSM_StateChange(FSwingState::GetInstance()->Instance());
			Action_Mode = TRUE;
			Action_Selector = 0;
		}
		// < Stand -> Slam >
		if (Animation_TexList == &Animation_Stand_Normal_TexList && Action_Selector == 3) {
			Animation_Index = 0;
			Animation_TexList = &Animation_Slam_Normal_TexList;
			Animation_FrameCount = ANIMATION_SLAM_NORMAL_FRAMECOUNT;

			FSM->FSM_StateChange(SlamState::GetInstance()->Instance());
			Action_Mode = TRUE;
			Action_Selector = 0;
			DoubleSlam = TRUE;
		}
		// < Double Slam >
		if (DoubleSlam && Animation_Index == ANIMATION_SLAM_NORMAL_FRAMECOUNT - 1) {
			Animation_Index = 0;
			DoubleSlam = FALSE;
		}
		// < RSwing/FSwing/Slam -> Stand >
		if ((Animation_TexList == &Animation_RSwing_Normal_TexList	&& Animation_Index == ANIMATION_RSWING_NORMAL_FRAMECOUNT - 1)
			|| (Animation_TexList == &Animation_Slam_Normal_TexList && Animation_Index == ANIMATION_SLAM_NORMAL_FRAMECOUNT - 1) 
			|| (Animation_TexList == &Animation_FSwing_TexList		&& Animation_Index == ANIMATION_FSWING_FRAMECOUNT - 1))			{
			Animation_Index = 0;
			Animation_TexList = &Animation_Stand_Normal_TexList;
			Animation_FrameCount = ANIMATION_STAND_NORMAL_FRAMECOUNT;

			Action_Mode = FALSE;
			FSM->FSM_StateChange(StandState::GetInstance()->Instance());
		}

		// < Appear -> Stand >
		if (Animation_TexList == &Animation_Appear_TexList && Animation_Index == ANIMATION_APPEAR_FRAMECOUNT - 1) {
			Animation_Index = 0;
			Animation_TexList = &Animation_Stand_Normal_TexList;
			Animation_FrameCount = ANIMATION_STAND_NORMAL_FRAMECOUNT;

			FSM->FSM_StateChange(StandState::GetInstance()->Instance());
			Invalidate_Mode = FALSE;
			Action_Mode		= FALSE;
		}

		// <<< RageMode >>>
		if (BossHP <= 500 && Rage_Mode == FALSE) {
			Animation_Index = 0;
			Animation_TexList = &Animation_RageUp_TexList;
			Animation_FrameCount = ANIMATION_RAGEUP_FRAMECOUNT;

			FSM->FSM_StateChange(RageUpState::GetInstance()->Instance());
			Invalidate_Mode = TRUE;
			Rage_Mode = TRUE;
		}
	}
	else if (Rage_Mode == TRUE) {
		// < Death >
		if (BossHP <= 0 && Death_Mode == FALSE) {
			Animation_Index = 0;
			Animation_TexList = &Animation_Death_TexList;
			Animation_FrameCount = ANIMATION_DEATH_FRAMECOUNT;

			Invalidate_Mode = TRUE;
			Death_Mode = TRUE;
			FSM->FSM_StateChange(DeadState::GetInstance()->Instance());
		}
	}

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
	Component_Transform->Set_Scale(8.f, 8.f, 8.f);

	Component_Collider		= ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);
	Component_Collider->Set_Scale(1.f, 1.f, 1.f);

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

	for (INT PIC = 1; PIC <= ANIMATION_FSWING_FRAMECOUNT; ++PIC) {
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
	Safe_Release(FSM);
	GameObject::Free();
}