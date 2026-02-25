#include "../Include/PCH.h"
#include "FinalBoss.h"

FinalBoss::FinalBoss(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV)	{}
FinalBoss::FinalBoss(CONST GameObject& _RHS)	: GameObject(_RHS)		{}
FinalBoss::~FinalBoss()													{}

HRESULT	FinalBoss::Ready_GameObject()						{
	if (FAILED(Component_Initialize()))	return E_FAIL;
	if (FAILED(Texture_Initialize()))	return E_FAIL;

	PlayerObject = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_GameObject(L"Player"));
	PlayerPos = { 0.f, 0.f, 0.f };

	Camera = dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_GameObject(L"Camera"));

	FSM = StateMachine::Create(GRPDEV);
	FSM->FSM_StateInit(AppearState::GetInstance()->Instance());
	FSM->FSM_SetOwner(this);

	//Invalidate_Mode = TRUE;		// 피해 무효화
	Invalidate_Mode = FALSE;		// 디버깅용
	Action_Mode		= TRUE;		// 다른 행동 간섭 방지
	//Action_Mode = FALSE;		// 디버깅용
	Rage_Mode		= FALSE;	// 폭주화 단계
	//Rage_Mode		= TRUE;	// 디버깅용
	Death_Mode		= FALSE;	// 사망 단계

	Animation_Timer		 = 0.f;
	Animation_Interval	 = 0.07f;
	Animation_CurrentIndex		= 0;
	Animation_PreviousIndex		= 0;
	Animation_FrameCount = ANIMATION_NORMAL_STAND_FRAMECOUNT;

	Animation_TexList = &Animation_Normal_Stand_TexList;

	Action_Selector = 0;
	Action_Timer = 0.f;

	DoubleSlam = TRUE;
	Enable_GroundExplosion = FALSE;
	Enable_GroundQuadExplosion = FALSE;
	Explosion_Timer = 0.f;

	Staging_Timer = 0.f;
	Enable_BossAppearStaging = FALSE;

	PlayerToAxisXDegree = 0.f;
	RSwing_Timer = 0.f;
	GeneratePos = { 0.f, 0.f, 0.f };
	Numbering = 0;
	memset(MeteorTransform, 0, sizeof(MeteorTransform));
	memset(STAGING_TRIGGER, TRUE, sizeof(STAGING_TRIGGER));
	memset(EXPLOSION_TRIGGER, TRUE, sizeof(EXPLOSION_TRIGGER));
	memset(METEOR_TRIGGER, TRUE, sizeof(METEOR_TRIGGER));
	memset(FIREBALL_TRIGGER, TRUE, sizeof(FIREBALL_TRIGGER));

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

	if (Animation_TexList == &Animation_NonAnim_TexList && 
		dynamic_cast<Transform*>(PlayerObject->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position()->z >= 90.f)
		Enable_BossAppearStaging = TRUE;

	Skill_GroundExplosion(_DT);
	Skill_MeteorExplosion(_DT);
	Skill_RSwingFireBall(_DT);
	Animation_Appear_Staging(_DT);

	Animation_PreviousIndex = Animation_CurrentIndex;
	if (Animation_Timer > Animation_Interval) {
		Animation_CurrentIndex = Animation_CurrentIndex + 1;
		if (Animation_CurrentIndex == Animation_FrameCount)	Animation_CurrentIndex = 0;
		Animation_Timer = 0.f;
	}

	Animation_Timer += _DT;
	if (Invalidate_Mode == FALSE || Action_Mode == FALSE)
		Action_Timer += _DT;
	
	if (Action_Timer > 3.f) {
		srand(time(NULL));
		Action_Selector = 1;//rand() % 4 + 1;
		Action_Timer = 0.f;
	}
	if (KEY_DOWN(DIK_I)) Component_Collider->Set_Hp(500);

	if (Rage_Mode == FALSE) {
		// < Stand -> RSwing >
		if (Animation_TexList == &Animation_Normal_Stand_TexList && Action_Selector == 1) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Normal_RSwing_TexList;
			Animation_FrameCount = ANIMATION_NORMAL_RSWING_FRAMECOUNT;

			FSM->FSM_StateChange(RSwingState::GetInstance()->Instance());
			Action_Mode		= TRUE;
			Action_Selector = 0;
		}
		// < Stand -> FSwing >
		if (Animation_TexList == &Animation_Normal_Stand_TexList && Action_Selector == 2) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Normal_FSwing_TexList;
			Animation_FrameCount = ANIMATION_NORMAL_FSWING_FRAMECOUNT;

			FSM->FSM_StateChange(FSwingState::GetInstance()->Instance());
			Action_Mode = TRUE;
			Action_Selector = 0;
		}
		// < Stand -> Normal Slam >
		if (Animation_TexList == &Animation_Normal_Stand_TexList && Action_Selector == 3) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Normal_Slam_TexList;
			Animation_FrameCount = ANIMATION_NORMAL_SLAM_FRAMECOUNT;

			FSM->FSM_StateChange(NormalSlamState::GetInstance()->Instance());
			Action_Mode = TRUE;
			Action_Selector = 0;
			//DoubleSlam = TRUE;
		}
		// < Stand -> Meteor Slam >
		if (Animation_TexList == &Animation_Normal_Stand_TexList && Action_Selector == 4) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Normal_Slam_TexList;
			Animation_FrameCount = ANIMATION_NORMAL_SLAM_FRAMECOUNT;

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
		if	(	(Animation_TexList == &Animation_Normal_RSwing_TexList	&& Animation_CurrentIndex == ANIMATION_NORMAL_RSWING_FRAMECOUNT - 1	)
			||	(Animation_TexList == &Animation_Normal_Slam_TexList	&& Animation_CurrentIndex == ANIMATION_NORMAL_SLAM_FRAMECOUNT - 1	) 
			||	(Animation_TexList == &Animation_Normal_FSwing_TexList			&& Animation_CurrentIndex == ANIMATION_NORMAL_FSWING_FRAMECOUNT - 1		))			{
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Normal_Stand_TexList;
			Animation_FrameCount = ANIMATION_NORMAL_STAND_FRAMECOUNT;

			Action_Mode = FALSE;
			FSM->FSM_StateChange(StandState::GetInstance()->Instance());
		}

		// < Appear -> Stand >
		if (Animation_TexList == &Animation_Appear_TexList && Animation_CurrentIndex == ANIMATION_APPEAR_FRAMECOUNT - 1) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Normal_Stand_TexList;
			Animation_FrameCount = ANIMATION_NORMAL_STAND_FRAMECOUNT;

			FSM->FSM_StateChange(StandState::GetInstance()->Instance());
			Invalidate_Mode = FALSE;
			Action_Mode		= FALSE;
		}

		// <<< RageMode >>>
		if (Component_Collider->Get_Hp() <= 500.f && Rage_Mode == FALSE) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Stunning_TexList;
			Animation_FrameCount = ANIMATION_STUNNING_FRAMECOUNT;
			Animation_Interval	= 0.25f;

			FSM->FSM_StateChange(RageUpState::GetInstance()->Instance());
			Invalidate_Mode = TRUE;
			Rage_Mode = TRUE;
		}
	}
	else if (Rage_Mode == TRUE) {
		// < Death >
		if (Component_Collider->Get_Hp() <= 0 && Death_Mode == FALSE) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Death_TexList;
			Animation_FrameCount = ANIMATION_DEATH_FRAMECOUNT;

			Invalidate_Mode = TRUE;
			Death_Mode = TRUE;
			FSM->FSM_StateChange(DeadState::GetInstance()->Instance());
		}
		// < Stand -> RSwing >
		if (Animation_TexList == &Animation_Rage_Stand_TexList && Action_Selector == 1) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Rage_RSwing_TexList;
			Animation_FrameCount = ANIMATION_RAGE_RSWING_FRAMECOUNT;

			FSM->FSM_StateChange(Rage_RSwingState::GetInstance()->Instance());
			Action_Mode = TRUE;
			Action_Selector = 0;
		}
		// < Stand -> Normal Slam >
		if (Animation_TexList == &Animation_Rage_Stand_TexList && Action_Selector == 2) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Rage_Slam_TexList;
			Animation_FrameCount = ANIMATION_RAGE_SLAM_FRAMECOUNT;

			FSM->FSM_StateChange(Rage_NormalSlamState::GetInstance()->Instance());
			Action_Mode = TRUE;
			Action_Selector = 0;
		}
		// < Stand -> Charge >
		if (Animation_TexList == &Animation_Rage_Stand_TexList && Action_Selector == 3) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Rage_Charge_TexList;
			Animation_FrameCount = ANIMATION_RAGE_CHARGE_FRAMECOUNT;

			FSM->FSM_StateChange(Rage_ChargeState::GetInstance()->Instance());
			Action_Mode = TRUE;
			Action_Selector = 0;
		}
		// < Stand -> Supporter >
		if (Animation_TexList == &Animation_Rage_Stand_TexList && Action_Selector == 4) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Rage_Stand_TexList;
			Animation_FrameCount = ANIMATION_RAGE_STAND_FRAMECOUNT;

			FSM->FSM_StateChange(Rage_SupporterState::GetInstance()->Instance());
			Action_Mode = TRUE;
			Action_Selector = 0;
		}
		if	(	(Animation_TexList == &Animation_Rage_RSwing_TexList	&& Animation_CurrentIndex == ANIMATION_RAGE_RSWING_FRAMECOUNT	- 1	)
			||	(Animation_TexList == &Animation_Rage_Slam_TexList		&& Animation_CurrentIndex == ANIMATION_RAGE_SLAM_FRAMECOUNT		- 1	)
			||	(Animation_TexList == &Animation_RageUp_TexList			&& Animation_CurrentIndex == ANIMATION_RAGEUP_FRAMECOUNT		- 1	)
			||	(Animation_TexList == &Animation_Rage_Charge_TexList	&& Animation_CurrentIndex == ANIMATION_RAGE_CHARGE_FRAMECOUNT	- 1 )
			||	(Animation_TexList == &Animation_Rage_Stand_TexList		&& Animation_CurrentIndex == ANIMATION_RAGE_STAND_FRAMECOUNT	- 1 
				&& FSM->FSM_GetCurrentState() != Rage_ChargeState::GetInstance()->Instance())) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Rage_Stand_TexList;
			Animation_FrameCount = ANIMATION_RAGE_STAND_FRAMECOUNT;

			Action_Mode = FALSE;
			FSM->FSM_StateChange(Rage_StandState::GetInstance()->Instance());
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

	GRPDEV->SetTexture(0, (*Animation_TexList)[Animation_CurrentIndex]);

	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

BOOL	FinalBoss::OnCollisionEnter(GameObject* _Other) {
	if (_Other->Get_ObjectTag() == L"PlayerArrow" && Invalidate_Mode == FALSE) {
		Component_Collider->Set_Hp(Component_Collider->Get_Hp() - dynamic_cast<Arrow*>(_Other)->Get_Atk());
		// Delete Arrow Code
	}
	if (_Other->Get_ObjectTag() == L"Supporter1" || _Other->Get_ObjectTag() == L"Supporter2" || _Other->Get_ObjectTag() == L"Supporter3") {
		// RageUp 마무리
		Animation_CurrentIndex = 0;
		Animation_TexList = &Animation_RageUp_TexList;
		Animation_FrameCount = ANIMATION_RAGEUP_FRAMECOUNT;
		Animation_Interval = 0.1f;
	}
	return TRUE;
}
BOOL	FinalBoss::OnCollisionStay(GameObject* _Other) { return TRUE; }
BOOL	FinalBoss::OnCollisionExit(GameObject* _Other) { return TRUE; }

HRESULT	FinalBoss::Component_Initialize() {
	Component_Buffer		= ADD_COMPONENT_RECTTEX;
	Component_Texture		= ADD_COMPONENT_TEXTURE;

	Component_Transform		= ADD_COMPONENT_TRANSFORM;
	Component_Transform->Set_Pos(-0.4f, 1.7f, 1.7f);
	Component_Transform->Set_Scale(9.f, 9.f, 9.f);

	Component_Collider		= ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);
	Component_Collider->Set_Offset({ -0.5f, -1.75f, -3.5f });
	Component_Collider->Set_Scale(2.5f, 1.5f, 3.f);
	Component_Collider->Set_Hp(1000.f);

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

	for (INT PIC = 1; PIC <= ANIMATION_NORMAL_RSWING_FRAMECOUNT; ++PIC) {
		Base = L"Boss_Normal_RSwing" + to_wstring(PIC) + L".png";
		Animation_Normal_RSwing_TexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}

	for (INT PIC = 1; PIC <= ANIMATION_RAGE_RSWING_FRAMECOUNT; ++PIC) {
		Base = L"Boss_Rage_RSwing" + to_wstring(PIC) + L".png";
		Animation_Rage_RSwing_TexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}
	for (INT PIC = 1; PIC <= ANIMATION_STUNNING_FRAMECOUNT; ++PIC) {
		Base = L"Boss_Stunning" + to_wstring(PIC) + L".png";
		Animation_Stunning_TexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}

	for (INT PIC = 1; PIC <= ANIMATION_NORMAL_FSWING_FRAMECOUNT; ++PIC) {
		Base = L"Boss_FullSwing" + to_wstring(PIC) + L".png";
		Animation_Normal_FSwing_TexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}

	for (INT PIC = 1; PIC <= ANIMATION_NORMAL_STAND_FRAMECOUNT; ++PIC) {
		Base = L"Boss_Normal_Stand" + to_wstring(PIC) + L".png";
		Animation_Normal_Stand_TexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}

	for (INT PIC = 1; PIC <= ANIMATION_RAGE_STAND_FRAMECOUNT; ++PIC) {
		Base = L"Boss_Rage_Stand" + to_wstring(PIC) + L".png";
		Animation_Rage_Stand_TexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}

	for (INT PIC = 1; PIC <= ANIMATION_NORMAL_SLAM_FRAMECOUNT; ++PIC) {
		Base = L"Boss_Normal_THSlam" + to_wstring(PIC) + L".png";
		Animation_Normal_Slam_TexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}

	for (INT PIC = 1; PIC <= ANIMATION_RAGE_SLAM_FRAMECOUNT; ++PIC) {
		Base = L"Boss_Rage_THSlam" + to_wstring(PIC) + L".png";
		Animation_Rage_Slam_TexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}

	for (INT PIC = 1; PIC <= ANIMATION_NONANIM_FRAMECOUNT; ++PIC) {
		Base = L"DumpTexture" + to_wstring(PIC) + L".png";
		Animation_NonAnim_TexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}

	for (INT PIC = 1; PIC <= ANIMATION_RAGE_CHARGE_FRAMECOUNT; ++PIC) {
		Base = L"Boss_Charge" + to_wstring(PIC) + L".png";
		Animation_Rage_Charge_TexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}

	return S_OK;
}

VOID FinalBoss::Animation_Appear_Staging(CONST FLOAT& _DT) {
	if (Enable_BossAppearStaging) {
		Staging_Timer += _DT;
		_vec3 GlobalPosition = { Component_Transform->Get_Position()->x, Component_Transform->Get_Position()->y - 1.5f,  Component_Transform->Get_Position()->z - 6.5f};
		FLOAT GloabalScale = 2.f;
		// Floor Glow
		// Floor Pool
		if		(Staging_Timer > 2.f && STAGING_TRIGGER	[(INT)STAGING::SPOOL_APPEAR]) {
			_vec3 SpoolAppearSca = { 250.f / 100.f * GloabalScale, 112.5f / 100.f * GloabalScale, 300.f / 100.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 1.f, GlobalPosition.z + 0.3f};
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SPOOL_APPEAR_EFFECT, L"SPOOL_APPEAR_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)STAGING::SPOOL_APPEAR] = FALSE;
		}
		else if (Staging_Timer > 3.f && STAGING_TRIGGER	[(INT)STAGING::SPOOL_FLOW1]) {
			_vec3 SpoolAppearSca = { 250.f / 100.f * GloabalScale, 112.5f / 100.f * GloabalScale, 300.f / 100.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 1.f, GlobalPosition.z + 0.3f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SPOOL_FLOW_EFFECT, L"SPOOL_FLOW_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)STAGING::SPOOL_FLOW1] = FALSE;
		}
		else if (Staging_Timer > 4.f && STAGING_TRIGGER	[(INT)STAGING::SPOOL_FLOW2]) {
			_vec3 SpoolAppearSca = { 250.f / 100.f * GloabalScale, 112.5f / 100.f * GloabalScale, 300.f / 100.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 1.f, GlobalPosition.z + 0.3f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SPOOL_FLOW_EFFECT, L"SPOOL_FLOW_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)STAGING::SPOOL_FLOW2] = FALSE;
		}
		else if (Staging_Timer > 5.f && STAGING_TRIGGER	[(INT)STAGING::SPOOL_FLOW3]) {
			_vec3 SpoolAppearSca = { 250.f / 100.f * GloabalScale, 112.5f / 100.f * GloabalScale, 300.f / 100.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 1.f, GlobalPosition.z + 0.3f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SPOOL_FLOW_EFFECT, L"SPOOL_FLOW_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)STAGING::SPOOL_FLOW3] = FALSE;
		}
		// Emblem Staging
		if		(Staging_Timer > 4.f && STAGING_TRIGGER	[(INT)STAGING::EMBLEM_APPEAR]) {
			_vec3 SpoolAppearSca = { 1.f * GloabalScale, 1.f * GloabalScale, 1.2f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 1.f, GlobalPosition.z + 1.7f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::EMBLEM_APPEAR_EFFECT, L"EMBLEM_APPEAR", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)STAGING::EMBLEM_APPEAR] = FALSE;
		}
		else if (Staging_Timer > 5.f && STAGING_TRIGGER	[(INT)STAGING::EMBLEM_DESTROY]) {
			_vec3 SpoolAppearSca = { 1.f * GloabalScale, 1.f * GloabalScale, 1.2f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 1.f, GlobalPosition.z + 1.7f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::EMBLEM_DESTROY_EFFECT, L"EMBLEM_DESTROY", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)STAGING::EMBLEM_DESTROY] = FALSE;
		}
		if		(Staging_Timer > 5.8f && STAGING_TRIGGER[(INT)STAGING::ANIMATION]) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Appear_TexList;
			Animation_FrameCount = ANIMATION_APPEAR_FRAMECOUNT;
			Component_Transform->Set_Pos(Component_Transform->Get_Position()->x - 0.3f, Component_Transform->Get_Position()->y, Component_Transform->Get_Position()->z + 0.5f);
			STAGING_TRIGGER[(INT)STAGING::ANIMATION] = FALSE;
		}
		// Blue Water Staging
		if		(Staging_Timer > 6.f && STAGING_TRIGGER	[(INT)STAGING::WATER_POPUP]) {
			Camera->Camera_Shaking(10, 0.5f);
			_vec3 SpoolAppearSca = { 3.f * GloabalScale, 2.5f * 2.f * GloabalScale, 5.f * 2.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x, GlobalPosition.y + 2.f, GlobalPosition.z + 8.4f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::WATER_POPUP_EFFECT, L"WATER_POPUP_EFFECT", &SpoolAppearPos, SpoolAppearSca, 0.7f);
			STAGING_TRIGGER[(INT)STAGING::WATER_POPUP] = FALSE;
		}
		// Small Flame Staging
		if		(Staging_Timer > 6.f && STAGING_TRIGGER	[(INT)STAGING::SMALL_FLAMEL]) {
			_vec3 SpoolAppearSca = { 3.5f * GloabalScale, 3.5f * GloabalScale, 3.5f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 4.5f, GlobalPosition.y + 3.f / 2.f, GlobalPosition.z + 4.f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::FLAME_EFFECT, L"SMALL_FLAMEL", &SpoolAppearPos, SpoolAppearSca, 1.f);
			dynamic_cast<BossEffect*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"SMALL_FLAMEL"))->Set_EffectRotation(0.f, -60.f, 10.f);
			STAGING_TRIGGER[(INT)STAGING::SMALL_FLAMEL] = FALSE;
		}
		if		(Staging_Timer > 6.2f && STAGING_TRIGGER[(INT)STAGING::SMALL_FLAMER]) {
			_vec3 SpoolAppearSca = { 4.5f * GloabalScale, 4.5f * GloabalScale, 4.5f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x + 4.5f, GlobalPosition.y + 2.5f, GlobalPosition.z + 6.3f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::FLAME_EFFECT, L"SMALL_FLAMER", &SpoolAppearPos, SpoolAppearSca, 1.f);
			dynamic_cast<BossEffect*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"SMALL_FLAMER"))->Set_EffectRotation(0.f, 60.f, -10.f);
			STAGING_TRIGGER[(INT)STAGING::SMALL_FLAMER] = FALSE;
		}
		if		(Staging_Timer > 6.2f && STAGING_TRIGGER[(INT)STAGING::SMALL_FLAMEC]) {
			_vec3 SpoolAppearSca = { 4.5f * GloabalScale, 4.5f * GloabalScale, 4.5f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x + 2.5f, GlobalPosition.y + 2.5f , GlobalPosition.z + 6.3f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::FLAME_EFFECT, L"SMALL_FLAMEC", &SpoolAppearPos, SpoolAppearSca, 1.f);
			dynamic_cast<BossEffect*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"SMALL_FLAMEC"))->Set_EffectRotation(0.f, 20.f, 0.f);
			STAGING_TRIGGER[(INT)STAGING::SMALL_FLAMEC] = FALSE;
			Animation_Interval = 0.14f;
		}

		// Big Flame Staging
		if		(Staging_Timer > 8.f && STAGING_TRIGGER	[(INT)STAGING::BIG_FLAME]) {
			Camera->Camera_Shaking(30, 1.f);
			_vec3 SpoolAppearSca = { 5.f * GloabalScale, 5.f * GloabalScale, 5.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.25f, GlobalPosition.y + 3.1f / 2.f, GlobalPosition.z + 6.f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::FLAME_EFFECT, L"BIG_FLAME_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)STAGING::BIG_FLAME] = FALSE;
		}
		if		(Staging_Timer > 8.f && STAGING_TRIGGER	[(INT)STAGING::BIG_CIRCLE_FLAME]) {
			_vec3 SpoolAppearSca = { 6.f * GloabalScale, 6.f / 2.f * GloabalScale, 6.f / 2.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x , GlobalPosition.y + 1.f, GlobalPosition.z + 1.f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::CIRCLE_FLAME_EFFECT, L"BIG_CIRCLE_FLAME", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)STAGING::BIG_CIRCLE_FLAME] = FALSE;
		}
		// Circle Flame Staging
		if		(Staging_Timer > 8.f && STAGING_TRIGGER	[(INT)STAGING::SPIRAL_FLAME]) {
			_vec3 SpoolAppearSca = { 6.f * GloabalScale, 6.f / 2.f * GloabalScale, 6.f / 2.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x , GlobalPosition.y + 2.1f / 2.f , GlobalPosition.z + 3.5f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SPIRAL_FLAME_EFFECT, L"SPIRAL_FLAME_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.4f);
			STAGING_TRIGGER[(INT)STAGING::SPIRAL_FLAME] = FALSE;

			Component_Transform->Set_Pos(Component_Transform->Get_Position()->x + 0.3f, Component_Transform->Get_Position()->y, Component_Transform->Get_Position()->z - 0.5f);
		}

		if		(Staging_Timer > 9.5f && STAGING_TRIGGER[(INT)STAGING::CAMERA_SHAKE]) {
			Camera->Camera_Shaking(30, 1.f);
			Animation_Interval = 0.07f;
			STAGING_TRIGGER[(INT)STAGING::CAMERA_SHAKE] = FALSE;
			//Component_Transform->Set_Pos(0.f, 1.f, 0.f);
		}
	}
}
VOID FinalBoss::Set_StartPos(_vec3 _StartPos) {
	Component_Transform->Set_Pos(Component_Transform->Get_Position()->x + _StartPos.x, 
		Component_Transform->Get_Position()->y + _StartPos.y, Component_Transform->Get_Position()->z + _StartPos.z);
}

VOID FinalBoss::Skill_GroundExplosion(CONST FLOAT& _DT) {
	if (Enable_GroundExplosion == 0 && Enable_GroundQuadExplosion == 0) return;
	else if (Enable_GroundExplosion == 1) {
		Explosion_Timer += _DT;
		if (Explosion_Timer > 0.5f) {

			_vec3 Scale = { 6.f, 6.f, 6.f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SLAM_GROUND_EXP_EFFECT, L"Ground Explosion", &PlayerPos, Scale, 0.7f);
			dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
				->Set_Rotation(85.f, 0.f, 0.f);

			PlayerPos = { 0.f, 0.f, 0.f };
			Enable_GroundExplosion = FALSE;
			Explosion_Timer = 0.f;
		}
	}
	else if (Enable_GroundExplosion == 2) {
		Explosion_Timer += _DT;
		if (Explosion_Timer > 0.5f) {

			_vec3 Scale = { 6.f, 6.f, 6.f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion", &PlayerPos, Scale, 0.7f);
			dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
				->Set_Rotation(85.f, 0.f, 0.f);

			PlayerPos = { 0.f, 0.f, 0.f };
			Enable_GroundExplosion = FALSE;
			Explosion_Timer = 0.f;
		}
	}
	else if (Enable_GroundQuadExplosion == 1) {
		Explosion_Timer += _DT;

		_vec3 Scale = { 6.f, 6.f, 6.f };
		_vec3 BossBottomPos = { Component_Transform->Get_Position()->x, 1.f, Component_Transform->Get_Position()->z - 7.f };
		_vec3 vecvec = PlayerPos - BossBottomPos;
		
		D3DXVec3Normalize(&vecvec, &vecvec);

		if (Explosion_Timer > 0.5f && EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION1]) {
			vecvec = vecvec * 4.f;
			vecvec += BossBottomPos;
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SLAM_GROUND_EXP_EFFECT, L"Ground Explosion1", &vecvec, Scale, 0.7f);
			dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion1")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
				->Set_Rotation(85.f, 0.f, 0.f);
			EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION1] = FALSE;
		}
		if (Explosion_Timer > 0.7f && EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION2]) {
			vecvec = vecvec * 8.f;
			vecvec += BossBottomPos;
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SLAM_GROUND_EXP_EFFECT, L"Ground Explosion2", &vecvec, Scale, 0.7f);
			dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion2")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
				->Set_Rotation(85.f, 0.f, 0.f);
			EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION2] = FALSE;
		}
		if (Explosion_Timer > 0.9f && EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION3]) {
			vecvec = vecvec * 12.f;
			vecvec += BossBottomPos;
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SLAM_GROUND_EXP_EFFECT, L"Ground Explosion3", &vecvec, Scale, 0.7f);
			dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion3")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
				->Set_Rotation(85.f, 0.f, 0.f);
			EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION3] = FALSE;
		}
		if (Explosion_Timer > 1.1f && EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION4]) {
			vecvec = vecvec * 16.f;
			vecvec += BossBottomPos;
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SLAM_GROUND_EXP_EFFECT, L"Ground Explosion4", &vecvec, Scale, 0.7f);
			dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion4")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
				->Set_Rotation(85.f, 0.f, 0.f);

			PlayerPos = { 0.f, 0.f, 0.f };
			memset(EXPLOSION_TRIGGER, TRUE, sizeof(EXPLOSION_TRIGGER));
			Enable_GroundQuadExplosion = FALSE;
			Explosion_Timer = 0.f;
		}
	}
	else if (Enable_GroundQuadExplosion == 2) {
		Explosion_Timer += _DT;

		_vec3 Scale = { 6.f, 6.f, 6.f };
		_vec3 BossBottomPos = { Component_Transform->Get_Position()->x, 1.f, Component_Transform->Get_Position()->z - 7.f };
		_vec3 vecvec = PlayerPos - BossBottomPos;

		D3DXVec3Normalize(&vecvec, &vecvec);

		if (Explosion_Timer > 0.5f && EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION1]) {
			vecvec = vecvec * 4.f;
			vecvec += BossBottomPos;
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion1", &vecvec, Scale, 0.7f);
			dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion1")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
				->Set_Rotation(85.f, 0.f, 0.f);
			EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION1] = FALSE;
		}
		if (Explosion_Timer > 0.7f && EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION2]) {
			vecvec = vecvec * 8.f;
			vecvec += BossBottomPos;
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion2", &vecvec, Scale, 0.7f);
			dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion2")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
				->Set_Rotation(85.f, 0.f, 0.f);
			EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION2] = FALSE;
		}
		if (Explosion_Timer > 0.9f && EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION3]) {
			vecvec = vecvec * 12.f;
			vecvec += BossBottomPos;
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion3", &vecvec, Scale, 0.7f);
			dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion3")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
				->Set_Rotation(85.f, 0.f, 0.f);
			EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION3] = FALSE;
		}
		if (Explosion_Timer > 1.1f && EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION4]) {
			vecvec = vecvec * 16.f;
			vecvec += BossBottomPos;
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion4", &vecvec, Scale, 0.7f);
			dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion4")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
				->Set_Rotation(85.f, 0.f, 0.f);

			PlayerPos = { 0.f, 0.f, 0.f };
			memset(EXPLOSION_TRIGGER, TRUE, sizeof(EXPLOSION_TRIGGER));
			Enable_GroundQuadExplosion = FALSE;
			Explosion_Timer = 0.f;
		}
	}
}
VOID FinalBoss::Skill_MeteorExplosion(CONST FLOAT& _DT) {
	if (Enable_MeteorExplosion == 0) return;
	if (Enable_MeteorExplosion == 1) {
		MeteorExplosion_Timer += _DT;

		// Activate Danger Area
		if (MeteorExplosion_Timer > 0.2f && METEOR_TRIGGER[(INT)METEOR::DANGER_AREA]) {
			_vec3 Scale = { 1.f, 1.f, 1.f };

			srand(time(NULL));
			for (INT IDX = 0; IDX < 4; ++IDX)
				RanPosX[IDX] = (rand() % 20 - 10) + ((FLOAT)(rand() % 20 - 10) / 10.f);
			for (INT IDX = 0; IDX < 4; ++IDX)
				RanPosZ[IDX] = (rand() % 20 - 20) + ((FLOAT)(rand() % 20 - 20) / 10.f);

			for (INT IDX = 0; IDX < 4; ++IDX) {
				_vec3 Pos = { Component_Transform->Get_Position()->x + RanPosX[IDX], Component_Transform->Get_Position()->y - 1.5f, Component_Transform->Get_Position()->z + RanPosZ[IDX] };
				PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"Explosion Warning" + to_wstring(IDX), &Pos, Scale, 0.6f);
				dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Explosion Warning" + to_wstring(IDX))->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
					->Set_Rotation(85.f, 0.f, 0.f);
				METEOR_TRIGGER[(INT)METEOR::DANGER_AREA] = FALSE;
			}
		}
		// Create Meteor & Place
		if (MeteorExplosion_Timer > 0.45f && METEOR_TRIGGER[(INT)METEOR::METEOR_CREATE]) {
			_vec3 Pos0 = { Component_Transform->Get_Position()->x + RanPosX[0] + 2.f, Component_Transform->Get_Position()->y - 1.5f + 6.f, Component_Transform->Get_Position()->z + RanPosZ[0] + 12.f };
			_vec3 Pos1 = { Component_Transform->Get_Position()->x + RanPosX[1] + 2.f, Component_Transform->Get_Position()->y - 1.5f + 6.f, Component_Transform->Get_Position()->z + RanPosZ[1] + 12.f };
			_vec3 Pos2 = { Component_Transform->Get_Position()->x + RanPosX[2] - 2.f, Component_Transform->Get_Position()->y - 1.5f + 6.f, Component_Transform->Get_Position()->z + RanPosZ[2] + 12.f };
			_vec3 Pos3 = { Component_Transform->Get_Position()->x + RanPosX[3] - 2.f, Component_Transform->Get_Position()->y - 1.5f + 6.f, Component_Transform->Get_Position()->z + RanPosZ[3] + 12.f };
			_vec3 Scale = { 1.f, 1.f, 1.f };
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::METEOR_EFFECT, L"Meteor1", &Pos0, Scale, 0.65f);
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::METEOR_EFFECT, L"Meteor2", &Pos1, Scale, 0.65f);
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::METEOR_EFFECT, L"Meteor3", &Pos2, Scale, 0.65f);
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::METEOR_EFFECT, L"Meteor4", &Pos3, Scale, 0.65f);

			METEOR_TRIGGER[(INT)METEOR::METEOR_CREATE] = FALSE;
		}
		// Drop Meteor
		if (MeteorExplosion_Timer > 0.5f && MeteorExplosion_Timer <= 0.9f) {
			if (MeteorTransform[0] == nullptr) {
				for (INT IDX = 0; IDX < 4; ++IDX) {
					MeteorTransform[IDX] = dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Meteor" + to_wstring(IDX + 1))->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
				}
			}
			FLOAT MeteorSpeed = 0.01f;
			MeteorTransform[0]->Set_Pos(MeteorTransform[0]->Get_Position()->x - MeteorSpeed * 9, MeteorTransform[0]->Get_Position()->y - MeteorSpeed * 27, MeteorTransform[0]->Get_Position()->z- MeteorSpeed * 50);
			MeteorTransform[1]->Set_Pos(MeteorTransform[1]->Get_Position()->x - MeteorSpeed * 9, MeteorTransform[1]->Get_Position()->y - MeteorSpeed * 27, MeteorTransform[1]->Get_Position()->z- MeteorSpeed * 50);
			MeteorTransform[2]->Set_Pos(MeteorTransform[2]->Get_Position()->x + MeteorSpeed * 9, MeteorTransform[2]->Get_Position()->y - MeteorSpeed * 27, MeteorTransform[2]->Get_Position()->z- MeteorSpeed * 50);
			MeteorTransform[3]->Set_Pos(MeteorTransform[3]->Get_Position()->x + MeteorSpeed * 9, MeteorTransform[3]->Get_Position()->y - MeteorSpeed * 27, MeteorTransform[3]->Get_Position()->z- MeteorSpeed * 50);
		}
		// Ground Explosion
		if (MeteorExplosion_Timer > 0.9f && METEOR_TRIGGER[(INT)METEOR::METEOR_EXPLOSION]) {
			_vec3 Scale = { 3.f, 4.f, 4.f };
			if (MeteorTransform[0] == nullptr) {
				for (INT IDX = 0; IDX < 4; ++IDX) {
					MeteorTransform[IDX] = dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Meteor" + to_wstring(IDX + 1))->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
				}
			}
			_vec3 Pos0 = { MeteorTransform[0]->Get_Position()->x, MeteorTransform[0]->Get_Position()->y + 1.5f, MeteorTransform[0]->Get_Position()->z + 3.f};
			_vec3 Pos1 = { MeteorTransform[1]->Get_Position()->x, MeteorTransform[1]->Get_Position()->y + 1.5f, MeteorTransform[1]->Get_Position()->z + 3.f};
			_vec3 Pos2 = { MeteorTransform[2]->Get_Position()->x, MeteorTransform[2]->Get_Position()->y + 1.5f, MeteorTransform[2]->Get_Position()->z + 3.f};
			_vec3 Pos3 = { MeteorTransform[3]->Get_Position()->x, MeteorTransform[3]->Get_Position()->y + 1.5f, MeteorTransform[3]->Get_Position()->z + 3.f};

			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::METEOR_EXP_EFFECT, L"MeteorExp1", &Pos0, Scale, 0.4f);
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::METEOR_EXP_EFFECT, L"MeteorExp2", &Pos1, Scale, 0.4f);
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::METEOR_EXP_EFFECT, L"MeteorExp3", &Pos2, Scale, 0.4f);
			PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::METEOR_EXP_EFFECT, L"MeteorExp4", &Pos3, Scale, 0.4f);

			METEOR_TRIGGER[(INT)METEOR::METEOR_EXPLOSION] = FALSE;
		}
		// All Reset
		if (MeteorExplosion_Timer > 1.3f) {
			memset(MeteorTransform, 0, sizeof(MeteorTransform));
			memset(METEOR_TRIGGER, TRUE, sizeof(METEOR_TRIGGER));
			Enable_MeteorExplosion = FALSE;
			MeteorExplosion_Timer = 0.f;
		}
	}
}
VOID FinalBoss::Skill_RSwingFireBall(CONST FLOAT& _DT) {
	if (Enable_CreateFireBall == 0)		return;
	else if (Enable_CreateFireBall == 1) {
		RSwing_Timer += _DT;

		FLOAT SectorAngle = 20.f;

		if		(RSwing_Timer > 0.10f && FIREBALL_TRIGGER[(INT)FIREBALL::ANGLE_GENERATE]) {
			GeneratePos = { Component_Collider->Get_CenterPos()->Get_Position()->x - 0.5f, Component_Collider->Get_CenterPos()->Get_Position()->y - 1.f, Component_Collider->Get_CenterPos()->Get_Position()->z - 4.f };
			_vec3 PlayerPos = *dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_GameObject(L"Player")
				->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position() - GeneratePos;
			_vec3 AxisXVec = { GeneratePos.x + 1, GeneratePos.y, GeneratePos.z };
			AxisXVec = AxisXVec - GeneratePos;
			D3DXVec3Normalize(&AxisXVec, &AxisXVec);
			D3DXVec3Normalize(&PlayerPos, &PlayerPos);

			if (GeneratePos.z >= dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_GameObject(L"Player")
				->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position()->z)
				PlayerToAxisXDegree = D3DXToDegree(acosf(D3DXVec3Dot(&PlayerPos, &AxisXVec)));
			else
				PlayerToAxisXDegree = 360 - D3DXToDegree(acosf(D3DXVec3Dot(&PlayerPos, &AxisXVec)));

			FIREBALL_TRIGGER[(INT)FIREBALL::ANGLE_GENERATE] = FALSE;
		}
		else if (RSwing_Timer > 0.15f && FIREBALL_TRIGGER[(INT)FIREBALL::FIRST_FIREBALL]) {

			wstring temp = L"FIRST_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree - SectorAngle);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"FIRST_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree - SectorAngle / 2);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"FIRST_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"FIRST_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree + SectorAngle / 2);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"FIRST_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree + SectorAngle);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			FIREBALL_TRIGGER[(INT)FIREBALL::FIRST_FIREBALL] = FALSE;
		}
		else if (RSwing_Timer > 0.25f && FIREBALL_TRIGGER[(INT)FIREBALL::SECOND_FIREBALL]) {
			GeneratePos = { GeneratePos.x, GeneratePos.y + 0.3f, GeneratePos.z };

			wstring temp = L"SECOND_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree - SectorAngle);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"SECOND_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree - SectorAngle / 2);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"SECOND_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"SECOND_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree + SectorAngle / 2);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"SECOND_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree + SectorAngle);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			FIREBALL_TRIGGER[(INT)FIREBALL::SECOND_FIREBALL] = FALSE;
		}
		else if (RSwing_Timer > 0.35f && FIREBALL_TRIGGER[(INT)FIREBALL::THIRD_FIREBALL]) {
			GeneratePos = { GeneratePos.x, GeneratePos.y + 0.3f, GeneratePos.z };

			wstring temp = L"THIRD_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree - SectorAngle);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"THIRD_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree - SectorAngle / 2);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"THIRD_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"THIRD_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree + SectorAngle / 2);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"THIRD_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree + SectorAngle);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			FIREBALL_TRIGGER[(INT)FIREBALL::THIRD_FIREBALL] = FALSE;

			RSwing_Timer = 0.f;
			memset(FIREBALL_TRIGGER, TRUE, sizeof(FIREBALL_TRIGGER));
			Enable_CreateFireBall = false;
		}
	}
	else if (Enable_CreateFireBall == 2) {
		RSwing_Timer += _DT;

		FLOAT SectorAngle = 20.f;

		if		(RSwing_Timer > 0.10f && FIREBALL_TRIGGER[(INT)FIREBALL::ANGLE_GENERATE]) {
			GeneratePos = { Component_Collider->Get_CenterPos()->Get_Position()->x - 0.5f, Component_Collider->Get_CenterPos()->Get_Position()->y - 1.f, Component_Collider->Get_CenterPos()->Get_Position()->z - 4.f };
			_vec3 PlayerPos = *dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_GameObject(L"Player")
				->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position() - GeneratePos;
			_vec3 AxisXVec = { GeneratePos.x + 1, GeneratePos.y, GeneratePos.z };
			AxisXVec = AxisXVec - GeneratePos;
			D3DXVec3Normalize(&AxisXVec, &AxisXVec);
			D3DXVec3Normalize(&PlayerPos, &PlayerPos);

			if (GeneratePos.z >= dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_GameObject(L"Player")
				->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position()->z)
				PlayerToAxisXDegree = D3DXToDegree(acosf(D3DXVec3Dot(&PlayerPos, &AxisXVec)));
			else
				PlayerToAxisXDegree = 360 - D3DXToDegree(acosf(D3DXVec3Dot(&PlayerPos, &AxisXVec)));

			FIREBALL_TRIGGER[(INT)FIREBALL::ANGLE_GENERATE] = FALSE;
		}
		else if (RSwing_Timer > 0.15f && FIREBALL_TRIGGER[(INT)FIREBALL::FIRST_FIREBALL]) {

			wstring temp = L"FIRST_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree - SectorAngle);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"FIRST_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree - SectorAngle / 2);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"FIRST_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"FIRST_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree + SectorAngle / 2);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"FIRST_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree + SectorAngle);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			FIREBALL_TRIGGER[(INT)FIREBALL::FIRST_FIREBALL] = FALSE;
		}
		else if (RSwing_Timer > 0.25f && FIREBALL_TRIGGER[(INT)FIREBALL::SECOND_FIREBALL]) {
			GeneratePos = { GeneratePos.x, GeneratePos.y + 0.3f, GeneratePos.z };

			wstring temp = L"SECOND_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree - SectorAngle);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"SECOND_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree - SectorAngle / 2);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"SECOND_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"SECOND_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree + SectorAngle / 2);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"SECOND_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree + SectorAngle);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			FIREBALL_TRIGGER[(INT)FIREBALL::SECOND_FIREBALL] = FALSE;
		}
		else if (RSwing_Timer > 0.35f && FIREBALL_TRIGGER[(INT)FIREBALL::THIRD_FIREBALL]) {
			GeneratePos = { GeneratePos.x, GeneratePos.y + 0.3f, GeneratePos.z };

			wstring temp = L"THIRD_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree - SectorAngle);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"THIRD_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree - SectorAngle / 2);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"THIRD_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"THIRD_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree + SectorAngle / 2);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"THIRD_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree + SectorAngle);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			FIREBALL_TRIGGER[(INT)FIREBALL::THIRD_FIREBALL] = FALSE;
		}
		else if (RSwing_Timer > 0.65f && FIREBALL_TRIGGER[(INT)FIREBALL::FOURTH_FIREBALL]) {

			GeneratePos = { GeneratePos.x, GeneratePos.y + 0.3f, GeneratePos.z };

			wstring temp = L"FOURTH_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree + 25);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"FOURTH_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree + 15);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"FOURTH_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree + 5);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"FOURTH_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree - 5);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"FOURTH_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree - 15);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"FOURTH_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree - 25);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			FIREBALL_TRIGGER[(INT)FIREBALL::FOURTH_FIREBALL] = FALSE;
		}
		else if (RSwing_Timer > 0.75f && FIREBALL_TRIGGER[(INT)FIREBALL::FIFTH_FIREBALL]) {
			GeneratePos = { GeneratePos.x, GeneratePos.y + 0.3f, GeneratePos.z };

			wstring temp = L"FIFTH_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree + 25);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"FIFTH_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree + 15);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"FIFTH_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree + 5);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"FIFTH_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree - 5);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"FIFTH_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree - 15);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"FIFTH_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree - 25);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			FIREBALL_TRIGGER[(INT)FIREBALL::FIFTH_FIREBALL] = FALSE;
		}
		else if (RSwing_Timer > 0.85f && FIREBALL_TRIGGER[(INT)FIREBALL::SIXTH_FIREBALL]) {
			GeneratePos = { GeneratePos.x, GeneratePos.y + 0.3f, GeneratePos.z };

			wstring temp = L"SIXTH_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree + 25);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"SIXTH_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree + 15);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"SIXTH_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree + 5);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"SIXTH_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree - 5);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"SIXTH_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree - 15);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			temp = L"SIXTH_FIREBALL" + to_wstring(Numbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, temp.c_str());
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree - 25);
			dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(temp.c_str()))->Set_FireBall_Pos(GeneratePos);

			FIREBALL_TRIGGER[(INT)FIREBALL::SIXTH_FIREBALL] = FALSE;

			RSwing_Timer = 0.f;
			memset(FIREBALL_TRIGGER, TRUE, sizeof(FIREBALL_TRIGGER));
			Enable_CreateFireBall = false;
		}
		
	}		
}
VOID FinalBoss::Skill_RageUpFireBall(CONST FLOAT& _DT) {
	//if()
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