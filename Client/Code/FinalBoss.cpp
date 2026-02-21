#include "../Include/PCH.h"
#include "FinalBoss.h"

FinalBoss::FinalBoss(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV)	{}
FinalBoss::FinalBoss(CONST GameObject& _RHS)	: GameObject(_RHS)		{}
FinalBoss::~FinalBoss()													{}

HRESULT	FinalBoss::Ready_GameObject()						{
	//ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/Appear");
	//ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/Stand");
	//ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/NoneAnimation");
	////ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/RageUp");
	//ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/RightSwing");
	//ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/FullSwing");
	//ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/TwoHandSlam");
	////ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/Death");
	//
	//ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/Effect");

	BossStartPos = {- 0.4f, 1.5f, 1.7f };

	if (FAILED(Component_Initialize()))	return E_FAIL;
	if (FAILED(Texture_Initialize()))	return E_FAIL;

	PlayerObject = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_GameObject(L"Player"));
	PlayerPos = { 0.f, 0.f, 0.f };

	Camera = dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_GameObject(L"Camera"));

	FSM = StateMachine::Create(GRPDEV);
	FSM->FSM_StateInit(AppearState::GetInstance()->Instance());
	FSM->FSM_SetOwner(this);

	BossHP = 1000.f;

	TEMP1 = 0.f; TEMP2 = 0.f; TEMP3 = 0.f;

	Invalidate_Mode = TRUE;		// 피해 무효화
	Rage_Mode		= FALSE;	// 폭주화 단계
	Action_Mode		= TRUE;		// 다른 행동 간섭 방지
	Death_Mode		= FALSE;

	Animation_Timer		 = 0.f;
	Animation_Interval	 = 0.07f;
	Animation_CurrentIndex		= 0;
	Animation_PreviousIndex		= 0;
	Animation_FrameCount = ANIMATION_STAND_NORMAL_FRAMECOUNT;

	Animation_TexList = &Animation_Stand_Normal_TexList;

	Invalidate_Mode = FALSE;		// 피해 무효화
	Action_Mode = FALSE;		// 다른 행동 간섭 방지

	Action_Selector = 0;
	Action_Timer = 0.f;

	DoubleSlam = TRUE;
	Enable_GroundExplosion = FALSE;
	Enable_QuadGroundExplosion = FALSE;
	Explosion_Timer = 0.f;

	Staging_Timer = 0.f;
	Enable_Staging = FALSE;
	memset(STAGING_TRIGGER, TRUE, sizeof(STAGING_TRIGGER));

	CameraObject* Camera = dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Camera"));

	_vec3 cameraDir = *(Camera->Get_EyeVec()) - *(Camera->Get_AtVec());
	_vec3 planeDir = { 0.f, 1.f, 0.f };

	_float angle = acosf(D3DXVec3Dot(D3DXVec3Normalize(&cameraDir, &cameraDir), D3DXVec3Normalize(&planeDir, &planeDir)));
	_float _cameraAngle = angle / D3DX_PI * 180.f;

	Component_Transform->Rotation(ROT_X, 80.f);//90.f - _cameraAngle);

	return S_OK;
}
INT		FinalBoss::Update_GameObject(CONST FLOAT& _DT)		{
	GameObject::Update_GameObject(_DT);
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	FSM->Update_GameObject(_DT);

	if (KEY_DOWN(DIK_L)) 
		Enable_Staging = true;

	Skill_GroundExplosion(_DT);
	Animation_Appear_Staging(_DT);

	Animation_PreviousIndex = Animation_CurrentIndex;
	if (Animation_Timer > Animation_Interval) {
		Animation_CurrentIndex = Animation_CurrentIndex + 1;
		if (Animation_CurrentIndex == Animation_FrameCount)	Animation_CurrentIndex = 0;
		Animation_Timer = 0.f;
	}

	Animation_Timer += _DT;
	if (Invalidate_Mode == FALSE || Action_Mode == FALSE) {
		Action_Timer += _DT;
	}
	
	if (Action_Timer > 3.f) {
		srand(time(NULL));
		Action_Selector = 2;//rand() % 4 + 1;
		Action_Timer = 0.f;
	}

	if (Rage_Mode == FALSE) {
		// < Stand -> RSwing >
		if (Animation_TexList == &Animation_Stand_Normal_TexList && Action_Selector == 1) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_RSwing_Normal_TexList;
			Animation_FrameCount = ANIMATION_RSWING_NORMAL_FRAMECOUNT;

			FSM->FSM_StateChange(RSwingState::GetInstance()->Instance());
			Action_Mode		= TRUE;
			Action_Selector = 0;
		}
		// < Stand -> FSwing >
		if (Animation_TexList == &Animation_Stand_Normal_TexList && Action_Selector == 2) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_FSwing_TexList;
			Animation_FrameCount = ANIMATION_FSWING_FRAMECOUNT;

			FSM->FSM_StateChange(FSwingState::GetInstance()->Instance());
			Action_Mode = TRUE;
			Action_Selector = 0;
		}
		// < Stand -> Normal Slam >
		if (Animation_TexList == &Animation_Stand_Normal_TexList && Action_Selector == 3) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Slam_Normal_TexList;
			Animation_FrameCount = ANIMATION_SLAM_NORMAL_FRAMECOUNT;

			FSM->FSM_StateChange(NormalSlamState::GetInstance()->Instance());
			Action_Mode = TRUE;
			Action_Selector = 0;
			//DoubleSlam = TRUE;
		}

		// < Stand -> Normal Slam >
		if (Animation_TexList == &Animation_Stand_Normal_TexList && Action_Selector == 4) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Slam_Normal_TexList;
			Animation_FrameCount = ANIMATION_SLAM_NORMAL_FRAMECOUNT;

			FSM->FSM_StateChange(MeteorSlamState::GetInstance()->Instance());
			Action_Mode = TRUE;
			Action_Selector = 0;
		}
		// < Double Slam >
		//if (DoubleSlam && Animation_CurrentIndex == ANIMATION_SLAM_NORMAL_FRAMECOUNT - 1) {
		//	Animation_CurrentIndex = 0;
		//	DoubleSlam = FALSE;
		//}
		// < RSwing/FSwing/Slam -> Stand >
		if ((Animation_TexList == &Animation_RSwing_Normal_TexList	&& Animation_CurrentIndex == ANIMATION_RSWING_NORMAL_FRAMECOUNT - 1)
			|| (Animation_TexList == &Animation_Slam_Normal_TexList && Animation_CurrentIndex == ANIMATION_SLAM_NORMAL_FRAMECOUNT - 1) 
			|| (Animation_TexList == &Animation_FSwing_TexList		&& Animation_CurrentIndex == ANIMATION_FSWING_FRAMECOUNT - 1))			{
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Stand_Normal_TexList;
			Animation_FrameCount = ANIMATION_STAND_NORMAL_FRAMECOUNT;

			Action_Mode = FALSE;
			FSM->FSM_StateChange(StandState::GetInstance()->Instance());
		}

		// < Appear -> Stand >
		if (Animation_TexList == &Animation_Appear_TexList && Animation_CurrentIndex == ANIMATION_APPEAR_FRAMECOUNT - 1) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Stand_Normal_TexList;
			Animation_FrameCount = ANIMATION_STAND_NORMAL_FRAMECOUNT;

			FSM->FSM_StateChange(StandState::GetInstance()->Instance());
			Invalidate_Mode = FALSE;
			Action_Mode		= FALSE;
		}

		// <<< RageMode >>>
		if (BossHP <= 500 && Rage_Mode == FALSE) {
			Animation_CurrentIndex = 0;
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
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Death_TexList;
			Animation_FrameCount = ANIMATION_DEATH_FRAMECOUNT;

			Invalidate_Mode = TRUE;
			Death_Mode = TRUE;
			FSM->FSM_StateChange(DeadState::GetInstance()->Instance());
		}
	}
	
	if (KEY_HOLD(DIK_LSHIFT)) {
		if (KEY_DOWN(DIK_I)) {
			TEMP1 -= 1.f;
		}
		if (KEY_DOWN(DIK_O)) {
			TEMP2 -= 1.f;
		}
		if (KEY_DOWN(DIK_P)) {
			TEMP3 -= 1.f;
		}
	}
	else {
		if (KEY_DOWN(DIK_I)) {
			TEMP1 += 1.f;
		}
		if (KEY_DOWN(DIK_O)) {
			TEMP2 += 1.f;
		}
		if (KEY_DOWN(DIK_P)) {
			TEMP3 += 1.f;
		}
	}
	
	return 0;
}
VOID	FinalBoss::LateUpdate_GameObject(CONST FLOAT& _DT)	{
	GameObject::LateUpdate_GameObject(_DT);
	
}
VOID	FinalBoss::Render_GameObject() {
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());
	GRPDEV->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GRPDEV->SetTexture(0, (*Animation_TexList)[Animation_CurrentIndex]);

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
	Component_Transform->Set_Pos(BossStartPos.x, BossStartPos.y , BossStartPos.z);
	Component_Transform->Set_Scale(9.f, 9.f, 9.f);

	Component_Collider		= ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);
	Component_Collider->Set_Offset({ -0.5f, -1.75f, -3.5f });
	Component_Collider->Set_Scale(2.5f, 1.5f, 3.f);

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

	for (INT PIC = 1; PIC <= ANIMATION_NONANIM_FRAMECOUNT; ++PIC) {
		Base = L"DumpTexture" + to_wstring(PIC) + L".png";
		Animation_NonAnim_TexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}

	return S_OK;
}

VOID FinalBoss::Animation_Appear_Staging(const FLOAT& _DT) {
	if (Enable_Staging) {
		Staging_Timer += _DT;
		_vec3 GlobalPosition = { Component_Transform->Get_Position()->x, Component_Transform->Get_Position()->y - 1.5f,  Component_Transform->Get_Position()->z - 6.5f};
		FLOAT GloabalScale = 2.f;
		// Floor Glow
		// Floor Pool
		if		(Staging_Timer > 2.f && STAGING_TRIGGER[(INT)STAGING::SPOOL_APPEAR]) {
			_vec3 SpoolAppearSca = { 250.f / 100.f * GloabalScale, 112.5f / 100.f * GloabalScale, 300.f / 100.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 1.f, GlobalPosition.z + 0.3f};
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SPOOL_APPEAR_EFFECT, L"SPOOL_APPEAR_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)STAGING::SPOOL_APPEAR] = FALSE;
		}
		else if (Staging_Timer > 3.f && STAGING_TRIGGER[(INT)STAGING::SPOOL_FLOW1]) {
			_vec3 SpoolAppearSca = { 250.f / 100.f * GloabalScale, 112.5f / 100.f * GloabalScale, 300.f / 100.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 1.f, GlobalPosition.z + 0.3f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SPOOL_FLOW_EFFECT, L"SPOOL_FLOW_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)STAGING::SPOOL_FLOW1] = FALSE;
		}
		else if (Staging_Timer > 4.f && STAGING_TRIGGER[(INT)STAGING::SPOOL_FLOW2]) {
			_vec3 SpoolAppearSca = { 250.f / 100.f * GloabalScale, 112.5f / 100.f * GloabalScale, 300.f / 100.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 1.f, GlobalPosition.z + 0.3f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SPOOL_FLOW_EFFECT, L"SPOOL_FLOW_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)STAGING::SPOOL_FLOW2] = FALSE;
		}
		else if (Staging_Timer > 5.f && STAGING_TRIGGER[(INT)STAGING::SPOOL_FLOW3]) {
			_vec3 SpoolAppearSca = { 250.f / 100.f * GloabalScale, 112.5f / 100.f * GloabalScale, 300.f / 100.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 1.f, GlobalPosition.z + 0.3f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SPOOL_FLOW_EFFECT, L"SPOOL_FLOW_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)STAGING::SPOOL_FLOW3] = FALSE;
		}
		// Emblem Staging
		if		(Staging_Timer > 4.f && STAGING_TRIGGER[(INT)STAGING::EMBLEM_APPEAR]) {
			_vec3 SpoolAppearSca = { 1.f * GloabalScale, 1.f * GloabalScale, 1.2f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 1.f, GlobalPosition.z + 1.7f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::EMBLEM_APPEAR_EFFECT, L"EMBLEM_APPEAR", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)STAGING::EMBLEM_APPEAR] = FALSE;
		}
		else if (Staging_Timer > 5.f && STAGING_TRIGGER[(INT)STAGING::EMBLEM_DESTROY]) {
			_vec3 SpoolAppearSca = { 1.f * GloabalScale, 1.f * GloabalScale, 1.2f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 1.f, GlobalPosition.z + 1.7f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::EMBLEM_DESTROY_EFFECT, L"EMBLEM_DESTROY", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)STAGING::EMBLEM_DESTROY] = FALSE;
		}
		if (Staging_Timer > 5.8f && STAGING_TRIGGER[(INT)STAGING::ANIMATION]) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Appear_TexList;
			Animation_FrameCount = ANIMATION_APPEAR_FRAMECOUNT;
			Component_Transform->Set_Pos(Component_Transform->Get_Position()->x - 0.3f, Component_Transform->Get_Position()->y, Component_Transform->Get_Position()->z + 0.5f);
			STAGING_TRIGGER[(INT)STAGING::ANIMATION] = FALSE;
		}
		// Blue Water Staging
		if		(Staging_Timer > 6.f && STAGING_TRIGGER[(INT)STAGING::WATER_POPUP]) {
			Camera->Camera_Shaking(10, 0.5f);
			_vec3 SpoolAppearSca = { 3.f * GloabalScale, 2.5f * 2.f * GloabalScale, 5.f * 2.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x, GlobalPosition.y + 2.f, GlobalPosition.z + 8.4f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::WATER_POPUP_EFFECT, L"WATER_POPUP_EFFECT", &SpoolAppearPos, SpoolAppearSca, 0.7f);
			STAGING_TRIGGER[(INT)STAGING::WATER_POPUP] = FALSE;
		}
		// Small Flame Staging
		if (Staging_Timer > 6.f && STAGING_TRIGGER[(INT)STAGING::SMALL_FLAMEL]) {
			_vec3 SpoolAppearSca = { 3.5f * GloabalScale, 3.5f * GloabalScale, 3.5f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 4.5f, GlobalPosition.y + 3.f / 2.f, GlobalPosition.z + 4.f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::FLAME_EFFECT, L"SMALL_FLAMEL", &SpoolAppearPos, SpoolAppearSca, 1.f);
			dynamic_cast<BossEffect*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"SMALL_FLAMEL"))->Set_EffectRotation(0.f, -60.f, 10.f);
			STAGING_TRIGGER[(INT)STAGING::SMALL_FLAMEL] = FALSE;
		}
		if (Staging_Timer > 6.2f && STAGING_TRIGGER[(INT)STAGING::SMALL_FLAMER]) {
			_vec3 SpoolAppearSca = { 4.5f * GloabalScale, 4.5f * GloabalScale, 4.5f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x + 4.5f, GlobalPosition.y + 2.5f, GlobalPosition.z + 6.3f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::FLAME_EFFECT, L"SMALL_FLAMER", &SpoolAppearPos, SpoolAppearSca, 1.f);
			dynamic_cast<BossEffect*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"SMALL_FLAMER"))->Set_EffectRotation(0.f, 60.f, -10.f);
			STAGING_TRIGGER[(INT)STAGING::SMALL_FLAMER] = FALSE;
		}
		if (Staging_Timer > 6.2f && STAGING_TRIGGER[(INT)STAGING::SMALL_FLAMEC]) {
			_vec3 SpoolAppearSca = { 4.5f * GloabalScale, 4.5f * GloabalScale, 4.5f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x + 2.5f, GlobalPosition.y + 2.5f , GlobalPosition.z + 6.3f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::FLAME_EFFECT, L"SMALL_FLAMEC", &SpoolAppearPos, SpoolAppearSca, 1.f);
			dynamic_cast<BossEffect*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"SMALL_FLAMEC"))->Set_EffectRotation(0.f, 20.f, 0.f);
			STAGING_TRIGGER[(INT)STAGING::SMALL_FLAMEC] = FALSE;
			Animation_Interval = 0.14f;
		}

		// Big Flame Staging
		if (Staging_Timer > 8.f && STAGING_TRIGGER[(INT)STAGING::BIG_FLAME]) {
			Camera->Camera_Shaking(30, 1.f);
			_vec3 SpoolAppearSca = { 5.f * GloabalScale, 5.f * GloabalScale, 5.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.25f, GlobalPosition.y + 3.1f / 2.f, GlobalPosition.z + 6.f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::FLAME_EFFECT, L"BIG_FLAME_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)STAGING::BIG_FLAME] = FALSE;
		}
		if (Staging_Timer > 8.f && STAGING_TRIGGER[(INT)STAGING::BIG_CIRCLE_FLAME]) {
			_vec3 SpoolAppearSca = { 6.f * GloabalScale, 6.f / 2.f * GloabalScale, 6.f / 2.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x , GlobalPosition.y + 1.f, GlobalPosition.z + 1.f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::CIRCLE_FLAME_EFFECT, L"BIG_CIRCLE_FLAME", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)STAGING::BIG_CIRCLE_FLAME] = FALSE;
		}
		// Circle Flame Staging
		if (Staging_Timer > 8.f && STAGING_TRIGGER[(INT)STAGING::SPIRAL_FLAME]) {
			_vec3 SpoolAppearSca = { 6.f * GloabalScale, 6.f / 2.f * GloabalScale, 6.f / 2.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x , GlobalPosition.y + 2.1f / 2.f , GlobalPosition.z + 3.5f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SPIRAL_FLAME_EFFECT, L"SPIRAL_FLAME_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.4f);
			STAGING_TRIGGER[(INT)STAGING::SPIRAL_FLAME] = FALSE;

			Component_Transform->Set_Pos(Component_Transform->Get_Position()->x + 0.3f, Component_Transform->Get_Position()->y, Component_Transform->Get_Position()->z - 0.5f);
		}

		if (Staging_Timer > 9.5f && STAGING_TRIGGER[(INT)STAGING::CAMERA_SHAKE]) {
			Camera->Camera_Shaking(30, 1.f);
			STAGING_TRIGGER[(INT)STAGING::CAMERA_SHAKE] = FALSE;
			//Component_Transform->Set_Pos(0.f, 1.f, 0.f);
		}
		//if (Staging_Timer > 11.5f) {
		//	Animation_Interval = 0.07f;
		//	Animation_CurrentIndex = 0;
		//	Animation_TexList = &Animation_NonAnim_TexList;
		//	Animation_FrameCount = ANIMATION_NONANIM_FRAMECOUNT;
		//
		//	FSM->FSM_StateChange(AppearState::GetInstance()->Instance());
		//	Staging_Timer = 0.f;
		//	Enable_Staging = 0.f;
		//	memset(STAGING_TRIGGER, TRUE, sizeof(STAGING_TRIGGER));
		//}
		// Animation

	}
}

VOID FinalBoss::Set_StartPos(_vec3 _StartPos) {
	Component_Transform->Set_Pos(Component_Transform->Get_Position()->x + _StartPos.x, 
		Component_Transform->Get_Position()->y + _StartPos.y, Component_Transform->Get_Position()->z + _StartPos.z);
}

VOID	FinalBoss::Skill_GroundExplosion(CONST FLOAT& _DT) {
	if (Enable_GroundExplosion) {
		Explosion_Timer += _DT;
		if (Explosion_Timer > 0.5f) {

			_vec3 Scale = { 4.f, 4.f, 4.f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SLAM_GROUND_EXP_EFFECT, L"Ground Explosion", &PlayerPos, Scale, 0.7f);

			PlayerPos = { 0.f, 0.f, 0.f };
			Enable_GroundExplosion = FALSE;
			Explosion_Timer = 0.f;
		}
	}
	else if (Enable_QuadGroundExplosion) {
		Explosion_Timer += _DT;

		_vec3 Scale = { 4.f, 4.f, 4.f };
		_vec3 BossBottomPos = { Component_Transform->Get_Position()->x, 0.5f, Component_Transform->Get_Position()->z - 7.f };
		_vec3 vecvec = PlayerPos - BossBottomPos;
		
		D3DXVec3Normalize(&vecvec, &vecvec);

		if (Explosion_Timer > 0.5f && Explosion_Timer < 0.505f) {
			vecvec = vecvec * 3.f;
			vecvec += BossBottomPos;
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SLAM_GROUND_EXP_EFFECT, L"Ground Explosion", &vecvec, Scale, 0.7f);
		}
		if (Explosion_Timer > 0.7f && Explosion_Timer < 0.705f) {
			vecvec = vecvec * 6.f;
			vecvec += BossBottomPos;
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SLAM_GROUND_EXP_EFFECT, L"Ground Explosion", &vecvec, Scale, 0.7f);
		}
		if (Explosion_Timer > 0.9f && Explosion_Timer < 0.905f) {
			vecvec = vecvec * 9.f;
			vecvec += BossBottomPos;
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SLAM_GROUND_EXP_EFFECT, L"Ground Explosion", &vecvec, Scale, 0.7f);
		}
		if (Explosion_Timer > 1.1f && Explosion_Timer < 1.105f) {
			vecvec = vecvec * 12.f;
			vecvec += BossBottomPos;
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SLAM_GROUND_EXP_EFFECT, L"Ground Explosion", &vecvec, Scale, 0.7f);

			PlayerPos = { 0.f, 0.f, 0.f };
			Enable_QuadGroundExplosion = FALSE;
			Explosion_Timer = 0.f;
		}
	}
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