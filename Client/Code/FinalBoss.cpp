#include "../Include/PCH.h"
#include "FinalBoss.h"

FinalBoss::FinalBoss(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
FinalBoss::FinalBoss(CONST GameObject& _RHS) : GameObject(_RHS) {}
FinalBoss::~FinalBoss() {}

HRESULT	FinalBoss::Ready_GameObject() {
	if (FAILED(Component_Initialize()))	return E_FAIL;
	if (FAILED(Texture_Initialize()))	return E_FAIL;

	PlayerObject	= static_cast<Player*>(SceneManager::GetInstance()->Get_GameObject(L"Player"));
	PlayerTransform = static_cast<Transform*>(PlayerObject->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
	PlayerPos = { 0.f, 0.f, 0.f };

	Camera = static_cast<CameraObject*>(SceneManager::GetInstance()->Get_GameObject(L"Camera"));

	Component_FSM = StateMachine::Create(GRPDEV);
	Component_FSM->FSM_StateInit(StandState::GetInstance()->Instance()); // NO ACTION
	Component_FSM->FSM_SetOwner(this);

	ObjectTAG = L"Docheol";
	dynamic_cast<MainUI*>(SceneManager::GetInstance()->Get_GameObject(L"MainUI"))->Set_BossMaxHP(Component_Collider->Get_Hp());

	BossMode[(LONG)BOSSMODE::MODE_INVALIDATE]		= TRUE;
	BossMode[(LONG)BOSSMODE::MODE_ACTION_AVAILABLE] = TRUE;		// 다른 행동 간섭 방지
	BossMode[(LONG)BOSSMODE::MODE_RAGE]				= FALSE;	// 폭주화 단계
	BossMode[(LONG)BOSSMODE::MODE_DEATH]			= FALSE;	// 사망 단계

	Animation_Timer				= 0.f;
	Animation_Interval			= 0.07f;
	Animation_CurrentIndex		= 0;
	Animation_PreviousIndex		= 0;

	//Animation_FrameCount	= ANIMATION_NORMAL_STAND_FRAMECOUNT;
	//Animation_TexList		= &Animation_Normal_Stand_TexList;

	Animation_FrameCount	= ANIMATION_NONANIM_FRAMECOUNT;
	Animation_TexList		= &Animation_NonAnim_TexList;

	Action_Selector = 0;

	memset(BossTimer, 0, sizeof(BossTimer));

	Enable_GroundExplosion		= FALSE;
	Enable_GroundQuadExplosion	= FALSE;
	Enable_BossAppearStaging	= FALSE;
	Enable_RageUpFireBall		= FALSE;
	Enable_BossDisappearStaging = FALSE;
	Enable_SupporterFlame		= FALSE;
	Enable_ExplosionRush		= FALSE;

	PlayerToAxisXDegree = 0.f;
	GeneratePos = { 0.f, 0.f, 0.f };

	memset(MeteorTransform	, 0	  , sizeof(MeteorTransform));
	memset(STAGING_TRIGGER	, TRUE, sizeof(STAGING_TRIGGER));
	memset(EXPLOSION_TRIGGER, TRUE, sizeof(EXPLOSION_TRIGGER));
	memset(METEOR_TRIGGER	, TRUE, sizeof(METEOR_TRIGGER));
	memset(FIREBALL_TRIGGER	, TRUE, sizeof(FIREBALL_TRIGGER));
	memset(SUPPORTER_TRIGGER, TRUE, sizeof(SUPPORTER_TRIGGER));
	memset(ERUSH_TRIGGER	, TRUE, sizeof(ERUSH_TRIGGER));
	memset(BBTrap			, TRUE, sizeof(BBTrap));

	SoundTransition = FALSE;
	SoundVolume = 0.f;
	return S_OK;
}
INT		FinalBoss::Update_GameObject(CONST FLOAT& _DT) {
	if (ObjectDead) return -1;
	GameObject::Update_GameObject(_DT);
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	Component_FSM->Update_GameObject(_DT);

	if (Animation_TexList == &Animation_NonAnim_TexList 
		&& PlayerTransform->Get_Position()->z >= 88.5f && PlayerTransform->Get_Position()->x >= 51.5f
		&& Component_FSM->FSM_GetCurrentState() != AppearState::GetInstance()->Instance()) {

		Component_FSM->FSM_StateChange(AppearState::GetInstance()->Instance());
		Camera->Set_SmoothCamera(TRUE);
		Enable_BossAppearStaging = TRUE;

		SoundTransition = (INT)SOUNDPLAYER::ESCAPING_BC;
	}

	if (Animation_TexList == &Animation_Death_TexList && Animation_CurrentIndex == ANIMATION_DEATH_FRAMECOUNT - 1) {
		Animation_Timer = 0.f;
		BossTimer[(LONG)BOSSTIMER::TIMER_ACTION] = 0.f;
		Animation_CurrentIndex = ANIMATION_DEATH_FRAMECOUNT - 1;
	}

	Skill_GroundExplosion(_DT);
	Skill_MeteorExplosion(_DT);
	Skill_RSwingFireBall(_DT);
	Skill_FSwingFireBall(_DT);
	Skill_RageUpFireBall(_DT);
	Skill_SupporterFlame(_DT);
	Skill_ExplosionRush(_DT);
	BoobieTrap(_DT);

	
	Animation_Appear_Staging(_DT);
	Animation_Disappear_Staging(_DT);
	
	Animation_PreviousIndex = Animation_CurrentIndex;
	if (Animation_Timer > Animation_Interval) {
		Animation_CurrentIndex = Animation_CurrentIndex + 1;
		if (Animation_CurrentIndex == Animation_FrameCount)  Animation_CurrentIndex = 0;
		Animation_Timer = 0.f;
	}
	Animation_Timer += _DT;
	if (BossMode[(LONG)BOSSMODE::MODE_INVALIDATE] == FALSE || BossMode[(LONG)BOSSMODE::MODE_ACTION_AVAILABLE] == FALSE)
		BossTimer[(LONG)BOSSTIMER::TIMER_ACTION] += _DT;

	if (BossTimer[(LONG)BOSSTIMER::TIMER_ACTION] > 3.5f) {
		srand(time(NULL));
		if		(BossMode[(LONG)BOSSMODE::MODE_RAGE] == FALSE)		{ Action_Selector = rand() % 4 + 1; }
		else if (BossMode[(LONG)BOSSMODE::MODE_RAGE] == TRUE)		{ Action_Selector = rand() % 4 + 1; } //rand() % 5 + 1; } 보스 패턴 추가 시 적용

		BossTimer[(LONG)BOSSTIMER::TIMER_ACTION] = 0.f;
	}

	if (BossMode[(LONG)BOSSMODE::MODE_RAGE] == FALSE) {
		// < Stand -> RSwing >
		if (Animation_TexList == &Animation_Normal_Stand_TexList && Action_Selector == 1) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Normal_RSwing_TexList;
			Animation_FrameCount = ANIMATION_NORMAL_RSWING_FRAMECOUNT;

			Component_FSM->FSM_StateChange(RSwingState::GetInstance()->Instance());
			BossMode[(LONG)BOSSMODE::MODE_ACTION_AVAILABLE] = TRUE;
			Action_Selector = 0;
		}
		// < Stand -> FSwing >
		if (Animation_TexList == &Animation_Normal_Stand_TexList && Action_Selector == 2) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Normal_FSwing_TexList;
			Animation_FrameCount = ANIMATION_NORMAL_FSWING_FRAMECOUNT;

			Component_FSM->FSM_StateChange(FSwingState::GetInstance()->Instance());
			BossMode[(LONG)BOSSMODE::MODE_ACTION_AVAILABLE] = TRUE;
			Action_Selector = 0;
		}
		// < Stand -> Normal Slam >
		if (Animation_TexList == &Animation_Normal_Stand_TexList && Action_Selector == 3) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Normal_Slam_TexList;
			Animation_FrameCount = ANIMATION_NORMAL_SLAM_FRAMECOUNT;

			Component_FSM->FSM_StateChange(NormalSlamState::GetInstance()->Instance());
			BossMode[(LONG)BOSSMODE::MODE_ACTION_AVAILABLE] = TRUE;
			Action_Selector = 0;
			//DoubleSlam = TRUE;
		}
		// < Stand -> Meteor Slam >
		if (Animation_TexList == &Animation_Normal_Stand_TexList && Action_Selector == 4) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Normal_Slam_TexList;
			Animation_FrameCount = ANIMATION_NORMAL_SLAM_FRAMECOUNT;

			Component_FSM->FSM_StateChange(MeteorSlamState::GetInstance()->Instance());
			BossMode[(LONG)BOSSMODE::MODE_ACTION_AVAILABLE] = TRUE;
			Action_Selector = 0;
		}

		if ((Animation_TexList == &Animation_Normal_RSwing_TexList	&& Animation_CurrentIndex == ANIMATION_NORMAL_RSWING_FRAMECOUNT - 1)
		||  (Animation_TexList == &Animation_Normal_Slam_TexList	&& Animation_CurrentIndex == ANIMATION_NORMAL_SLAM_FRAMECOUNT - 1)
		||  (Animation_TexList == &Animation_Normal_FSwing_TexList	&& Animation_CurrentIndex == ANIMATION_NORMAL_FSWING_FRAMECOUNT - 1)) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Normal_Stand_TexList;
			Animation_FrameCount = ANIMATION_NORMAL_STAND_FRAMECOUNT;

			BossMode[(LONG)BOSSMODE::MODE_ACTION_AVAILABLE] = FALSE;
			Component_FSM->FSM_StateChange(StandState::GetInstance()->Instance());
		}

		// < Appear -> Stand >
		if (Animation_TexList == &Animation_Appear_TexList && Animation_CurrentIndex == ANIMATION_APPEAR_FRAMECOUNT - 1) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Normal_Stand_TexList;
			Animation_FrameCount = ANIMATION_NORMAL_STAND_FRAMECOUNT;

			Component_FSM->FSM_StateChange(StandState::GetInstance()->Instance());
			BossMode[(LONG)BOSSMODE::MODE_INVALIDATE] = FALSE;
			BossMode[(LONG)BOSSMODE::MODE_ACTION_AVAILABLE] = FALSE;
		}
		// <<< RageMode >>>
		if (Component_Collider->Get_Hp() <= 5000.f && BossMode[(LONG)BOSSMODE::MODE_RAGE] == FALSE) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Stunning_TexList;
			Animation_FrameCount = ANIMATION_STUNNING_FRAMECOUNT;
			Animation_Interval = 0.25f;

			Component_FSM->FSM_StateChange(StunState::GetInstance()->Instance());
			PLAY_SOUND_ONCE(L"Docheol/Stun_State.wav", CHANNELID::SOUND_EFFECT09);
			BossMode[(LONG)BOSSMODE::MODE_INVALIDATE] = TRUE;
			BossMode[(LONG)BOSSMODE::MODE_RAGE] = TRUE;
		}
	}
	else if (BossMode[(LONG)BOSSMODE::MODE_RAGE] == TRUE) {
		// < Death >
		if (Component_Collider->Get_Hp() <= 0 && BossMode[(LONG)BOSSMODE::MODE_DEATH] == FALSE) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Death_TexList;
			Animation_FrameCount = ANIMATION_DEATH_FRAMECOUNT;

			BossMode[(LONG)BOSSMODE::MODE_INVALIDATE] = TRUE;
			BossMode[(LONG)BOSSMODE::MODE_DEATH] = TRUE;

			BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] = 0.f;

			Enable_BossAppearStaging = FALSE;
			Enable_BossDisappearStaging = TRUE;
			Enable_GroundExplosion = FALSE;
			Enable_GroundQuadExplosion = FALSE;
			Enable_RageUpFireBall = FALSE;
			Enable_SupporterFlame = FALSE;
			Enable_ExplosionRush = FALSE;

			memset(STAGING_TRIGGER, TRUE, sizeof(STAGING_TRIGGER));

			for (auto& ORU : ObjectPool_RageUp) 
				ORU->Set_ObjectDead(TRUE);
			for (auto& ORU : ObjectPool_Supporter)
				ORU->Set_ObjectDead(TRUE);
			for (auto& ORU : ObjectPool_RSwing)
				ORU->Set_ObjectDead(TRUE);
			for (auto& ORU : ObjectPool_FSwing)
				ORU->Set_ObjectDead(TRUE);
			Component_FSM->FSM_StateChange(DeadState::GetInstance()->Instance());
		}
		// < Stand -> RSwing >
		if (Animation_TexList == &Animation_Rage_Stand_TexList && Action_Selector == 1) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Rage_RSwing_TexList;
			Animation_FrameCount = ANIMATION_RAGE_RSWING_FRAMECOUNT;

			Component_FSM->FSM_StateChange(Rage_RSwingState::GetInstance()->Instance());
			BossMode[(LONG)BOSSMODE::MODE_ACTION_AVAILABLE] = TRUE;
			Action_Selector = 0;
		}
		// < Stand -> Normal Slam >
		if (Animation_TexList == &Animation_Rage_Stand_TexList && Action_Selector == 2) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Rage_Slam_TexList;
			Animation_FrameCount = ANIMATION_RAGE_SLAM_FRAMECOUNT;

			Component_FSM->FSM_StateChange(Rage_NormalSlamState::GetInstance()->Instance());
			BossMode[(LONG)BOSSMODE::MODE_ACTION_AVAILABLE] = TRUE;
			Action_Selector = 0;
		}
		// < Stand -> Rush >
		if (Animation_TexList == &Animation_Rage_Stand_TexList && Action_Selector == 3 && BossTimer[(LONG)BOSSTIMER::TIMER_RUSH] <= 1.f) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Rage_Rush_TexList;
			Animation_FrameCount = ANIMATION_RAGE_RUSH_FRAMECOUNT;

			Enable_ExplosionRush = TRUE;
			Component_FSM->FSM_StateChange(Rage_ExplosionRushState::GetInstance()->Instance());
			BossMode[(LONG)BOSSMODE::MODE_ACTION_AVAILABLE] = TRUE;
			Action_Selector = 0;
		}
		// < Stand -> Supporter >
		if (Animation_TexList == &Animation_Rage_Stand_TexList && Action_Selector == 4 && BossTimer[(LONG)BOSSTIMER::TIMER_SUPPORT] <= 1.f) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Rage_Charge_TexList;
			Animation_FrameCount = ANIMATION_RAGE_CHARGE_FRAMECOUNT;

			PLAY_SOUND_ONCE(L"Docheol/Supporter_Electric.wav", CHANNELID::SOUND_EFFECT01, 10.f);
			PLAY_SOUND_ONCE(L"Docheol/Supporter_Bump.wav", CHANNELID::SOUND_EFFECT02, 10.f);
			Camera->Camera_Shaking(70, 0.5f);

			Component_FSM->FSM_StateChange(Rage_SupporterState::GetInstance()->Instance());
			Enable_SupporterFlame = TRUE;
			BossMode[(LONG)BOSSMODE::MODE_ACTION_AVAILABLE] = TRUE;
			Action_Selector = 0;
		}
		// < Stand -> Charge >
		//if (Animation_TexList == &Animation_Rage_Stand_TexList && Action_Selector == 3) {
		//		Animation_CurrentIndex = 0;
		//		Animation_TexList = &Animation_Rage_Charge_TexList;
		//		Animation_FrameCount = ANIMATION_RAGE_CHARGE_FRAMECOUNT;
		//
		//		FSM->FSM_StateChange(Rage_ChargeState::GetInstance()->Instance());
		//		Action_Mode = TRUE;
		//		Action_Selector = 0;
		//	}
		// < All State -> Stand >
		if ((Animation_TexList == &Animation_Rage_RSwing_TexList && Animation_CurrentIndex == ANIMATION_RAGE_RSWING_FRAMECOUNT - 1)
			|| (Animation_TexList == &Animation_Rage_Slam_TexList && Animation_CurrentIndex == ANIMATION_RAGE_SLAM_FRAMECOUNT - 1)
			|| (Animation_TexList == &Animation_RageUp_TexList && Animation_CurrentIndex == ANIMATION_RAGEUP_FRAMECOUNT - 1)
			//|| (Animation_TexList == &Animation_Rage_Charge_TexList && Animation_CurrentIndex == ANIMATION_RAGE_CHARGE_FRAMECOUNT - 1)
			|| (Animation_TexList == &Animation_Rage_Stand_TexList && Animation_CurrentIndex == ANIMATION_RAGE_STAND_FRAMECOUNT - 1
				&& Component_FSM->FSM_GetCurrentState() != Rage_SupporterState::GetInstance()->Instance() )
			|| (Animation_TexList == &Animation_Rage_Stand_TexList && Animation_CurrentIndex == ANIMATION_RAGE_STAND_FRAMECOUNT - 1
				&& Component_FSM->FSM_GetCurrentState() != Rage_ExplosionRushState::GetInstance()->Instance())) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Rage_Stand_TexList;
			Animation_FrameCount = ANIMATION_RAGE_STAND_FRAMECOUNT;
			Animation_Interval = 0.07f;
			BossMode[(LONG)BOSSMODE::MODE_ACTION_AVAILABLE] = FALSE;
			Component_FSM->FSM_StateChange(Rage_StandState::GetInstance()->Instance());
		}
	}

	return 0;
}
VOID	FinalBoss::LateUpdate_GameObject(CONST FLOAT& _DT) {
	GameObject::LateUpdate_GameObject(_DT);
	BGM_Player(_DT);
	//if (KEY_DOWN(DIK_O)) Enable_ExplosionRush = TRUE;
	//if (KEY_DOWN(DIK_I)) {
	//	Enable_ExplosionRush = FALSE;
	//	BossTimer[(LONG)BOSSTIMER::TIMER_RUSH] = 0.f;
	//	BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] = 0.f;
	//	memset(ERUSH_TRIGGER, TRUE, sizeof(ERUSH_TRIGGER));
	//	memset(BBTrap, TRUE, sizeof(BBTrap));
	//}
	//
	//if (KEY_HOLD(DIK_LSHIFT) && KEY_DOWN(DIK_P)) {
	//	Component_Collider->Set_Hp(10);
	//	//BossMode[(LONG)BOSSMODE::MODE_RAGE] = TRUE;
	//	//Animation_TexList = &Animation_Stunning_TexList;
	//	//Animation_FrameCount = ANIMATION_STUNNING_FRAMECOUNT;
	//	//Animation_CurrentIndex = 0;
	//}
	//else if (KEY_DOWN(DIK_P)) {
	//	Component_Collider->Set_Hp(5000);
	//	BossMode[(LONG)BOSSMODE::MODE_RAGE] = TRUE;
	//	Animation_TexList = &Animation_Rage_Stand_TexList;
	//	Animation_FrameCount = ANIMATION_RAGE_STAND_FRAMECOUNT;
	//	Animation_CurrentIndex = 0;
	//}
}
VOID	FinalBoss::Render_GameObject() {
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	GRPDEV->SetTexture(0, (*Animation_TexList)[Animation_CurrentIndex]);

	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

BOOL	FinalBoss::OnCollisionEnter	(GameObject* _Other) {
	if (_Other->Get_ObjectTag() == L"Supporter1" || _Other->Get_ObjectTag() == L"Supporter2" || _Other->Get_ObjectTag() == L"Supporter3") {
		Animation_CurrentIndex = 0;
		Animation_TexList		= &Animation_RageUp_TexList;
		Animation_FrameCount	= ANIMATION_RAGEUP_FRAMECOUNT;
		Animation_Interval = 0.1f;

		Component_FSM->FSM_StateChange(RageUpState::GetInstance()->Instance());
	}
	return TRUE;
}
BOOL	FinalBoss::OnCollisionStay	(GameObject* _Other) { return TRUE; }
BOOL	FinalBoss::OnCollisionExit	(GameObject* _Other) { return TRUE; }

HRESULT	FinalBoss::Component_Initialize() {
	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Texture = ADD_COMPONENT_TEXTURE;

	Component_Transform = ADD_COMPONENT_TRANSFORM;
	Component_Transform->Set_Pos(-0.4f, 1.7f, 1.7f);
	Component_Transform->Set_Scale(9.f, 9.f, 9.f);
	Component_Transform->Rotation(ROT_X, 80.f);

	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);
	Component_Collider->Set_Offset({ -0.5f, -1.75f, -3.5f });
	Component_Collider->Set_Scale(2.5f, 1.5f, 3.f);
	Component_Collider->Set_Hp(10000.f);
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

	for (INT PIC = 1; PIC <= ANIMATION_RAGE_RUSH_FRAMECOUNT; ++PIC) {
		Base = L"Boss_Rush" + to_wstring(PIC) + L".png";
		Animation_Rage_Rush_TexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}

	return S_OK;
}

VOID	FinalBoss::Animation_Appear_Staging(CONST FLOAT& _DT) {
	if (Enable_BossAppearStaging) {
		BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] += _DT;
		_vec3 GlobalPosition = { Component_Transform->Get_Position()->x, Component_Transform->Get_Position()->y - 1.5f,  Component_Transform->Get_Position()->z - 6.5f };
		FLOAT GloabalScale = 2.f;

		if		(BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 2.0f && STAGING_TRIGGER[(INT)APPEAR_STAGING::SPOOL_APPEAR]) {
			_vec3 SpoolAppearSca = { 250.f / 70.f * GloabalScale, 112.5f / 70.f * GloabalScale, 300.f / 70.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 1.f, GlobalPosition.z + 1.65f };
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::SPOOL_APPEAR_EFFECT, L"SPOOL_APPEAR_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::SPOOL_APPEAR] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 3.0f && STAGING_TRIGGER[(INT)APPEAR_STAGING::SPOOL_FLOW1]) {
			_vec3 SpoolAppearSca = { 250.f / 70.f * GloabalScale, 112.5f / 70.f * GloabalScale, 300.f / 70.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 1.f, GlobalPosition.z + 1.65f };
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::SPOOL_FLOW_EFFECT, L"SPOOL_FLOW_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::SPOOL_FLOW1] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 4.0f && STAGING_TRIGGER[(INT)APPEAR_STAGING::SPOOL_FLOW2]) {
			_vec3 SpoolAppearSca = { 250.f / 70.f * GloabalScale, 112.5f / 70.f * GloabalScale, 300.f / 70.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 1.f, GlobalPosition.z + 1.65f };
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::SPOOL_FLOW_EFFECT, L"SPOOL_FLOW_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::SPOOL_FLOW2] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 5.0f && STAGING_TRIGGER[(INT)APPEAR_STAGING::SPOOL_FLOW3]) {
			_vec3 SpoolAppearSca = { 250.f / 70.f * GloabalScale, 112.5f / 70.f * GloabalScale, 300.f / 70.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 1.f, GlobalPosition.z + 1.65f };
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::SPOOL_FLOW_EFFECT, L"SPOOL_FLOW_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::SPOOL_FLOW3] = FALSE;
		}										  
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 4.0f && STAGING_TRIGGER[(INT)APPEAR_STAGING::EMBLEM_APPEAR]) {
			_vec3 SpoolAppearSca = { 1.f * GloabalScale, 1.f * GloabalScale, 1.2f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 1.f, GlobalPosition.z + 1.7f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::EMBLEM_APPEAR_EFFECT, L"EMBLEM_APPEAR", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::EMBLEM_APPEAR] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 5.0f && STAGING_TRIGGER[(INT)APPEAR_STAGING::EMBLEM_DESTROY]) {
			_vec3 SpoolAppearSca = { 1.f * GloabalScale, 1.f * GloabalScale, 1.2f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 1.f, GlobalPosition.z + 1.7f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::EMBLEM_DESTROY_EFFECT, L"EMBLEM_DESTROY", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::EMBLEM_DESTROY] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 5.8f && STAGING_TRIGGER[(INT)APPEAR_STAGING::ANIMATION]) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Appear_TexList;
			Animation_FrameCount = ANIMATION_APPEAR_FRAMECOUNT;
			Component_Transform->Set_Pos(Component_Transform->Get_Position()->x - 0.3f, Component_Transform->Get_Position()->y, Component_Transform->Get_Position()->z + 0.5f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::ANIMATION] = FALSE;
		}								  
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 6.0f && STAGING_TRIGGER[(INT)APPEAR_STAGING::WATER_POPUP]) {
			Camera->Camera_Shaking(100, 3.f);
			_vec3 SpoolAppearSca = { 3.f * GloabalScale, 2.5f * 2.f * GloabalScale, 5.f * 2.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x, GlobalPosition.y + 2.f, GlobalPosition.z + 8.4f };
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::WATER_POPUP_EFFECT, L"WATER_POPUP_EFFECT", &SpoolAppearPos, SpoolAppearSca, 0.7f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::WATER_POPUP] = FALSE;
		}									  
		if		(BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 6.0f && STAGING_TRIGGER[(INT)APPEAR_STAGING::SMALL_FLAMEL]) {
			_vec3 SpoolAppearSca = { 3.5f * GloabalScale, 3.5f * GloabalScale, 3.5f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 4.5f, GlobalPosition.y + 3.f / 2.f, GlobalPosition.z + 4.f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::FLAME_EFFECT, L"SMALL_FLAMEL", &SpoolAppearPos, SpoolAppearSca, 1.f);
			dynamic_cast<BossEffect*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"SMALL_FLAMEL"))->Set_EffectRotation(0.f, -60.f, 10.f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::SMALL_FLAMEL] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 6.2f && STAGING_TRIGGER[(INT)APPEAR_STAGING::SMALL_FLAMER]) {

			_vec3 SpoolAppearSca = { 4.5f * GloabalScale, 4.5f * GloabalScale, 4.5f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x + 4.5f, GlobalPosition.y + 2.5f, GlobalPosition.z + 6.3f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::FLAME_EFFECT, L"SMALL_FLAMER", &SpoolAppearPos, SpoolAppearSca, 1.f);
			dynamic_cast<BossEffect*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"SMALL_FLAMER"))->Set_EffectRotation(0.f, 60.f, -10.f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::SMALL_FLAMER] = FALSE;
		}
		if		(BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 6.2f && STAGING_TRIGGER[(INT)APPEAR_STAGING::SMALL_FLAMEC]) {
			_vec3 SpoolAppearSca = { 4.5f * GloabalScale, 4.5f * GloabalScale, 4.5f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x + 2.5f, GlobalPosition.y + 2.5f , GlobalPosition.z + 6.3f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::FLAME_EFFECT, L"SMALL_FLAMEC", &SpoolAppearPos, SpoolAppearSca, 1.f);
			dynamic_cast<BossEffect*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"SMALL_FLAMEC"))->Set_EffectRotation(0.f, 20.f, 0.f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::SMALL_FLAMEC] = FALSE;
			Animation_Interval = 0.14f;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 7.2f && STAGING_TRIGGER[(INT)APPEAR_STAGING::ROCK_CAMERA_SHAKE]) {

			STAGING_TRIGGER[(INT)APPEAR_STAGING::ROCK_CAMERA_SHAKE] = FALSE;
		}									  
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 7.6f && STAGING_TRIGGER[(INT)APPEAR_STAGING::BIG_FLAME]) {
			Camera->Camera_Shaking(120, 1.f);
			_vec3 SpoolAppearSca = { 7.f * GloabalScale, 7.f * GloabalScale, 7.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x, GlobalPosition.y + 3.8f / 2.f, GlobalPosition.z + 8.f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::FLAME_EFFECT, L"BIG_FLAME_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::BIG_FLAME] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 7.6f && STAGING_TRIGGER[(INT)APPEAR_STAGING::BIG_CIRCLE_FLAME]) {
			_vec3 SpoolAppearSca = { 7.f * GloabalScale, 7.f / 2.f * GloabalScale, 7.f / 2.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x , GlobalPosition.y + 1.f, GlobalPosition.z + 1.f };
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::CIRCLE_FLAME_EFFECT, L"BIG_CIRCLE_FLAME", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::BIG_CIRCLE_FLAME] = FALSE;
		}							  
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 7.6f && STAGING_TRIGGER[(INT)APPEAR_STAGING::SPIRAL_FLAME]) {
			_vec3 SpoolAppearSca = { 6.f * GloabalScale, 6.f / 2.f * GloabalScale, 6.f / 2.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x , GlobalPosition.y + 2.1f / 2.f , GlobalPosition.z + 3.5f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SPIRAL_FLAME_EFFECT, L"SPIRAL_FLAME_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.4f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::SPIRAL_FLAME] = FALSE;

			Component_Transform->Set_Pos(Component_Transform->Get_Position()->x + 0.3f, Component_Transform->Get_Position()->y, Component_Transform->Get_Position()->z - 0.5f);
		}							  
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 9.5f && STAGING_TRIGGER[(INT)APPEAR_STAGING::CAMERA_SHAKE]) {
			Camera->Camera_Shaking(100, 4.f);
			Animation_Interval = 0.07f;
			STAGING_TRIGGER[(INT)APPEAR_STAGING::CAMERA_SHAKE] = FALSE;
			BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] = 0.f;

			BossMode[(LONG)BOSSMODE::MODE_INVALIDATE] = FALSE;
		}
	
		//////////////////////////////////////////////////// SOUNDPLAY ////////////////////////////////////////////////////
		if		(BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 4.8f && STAGING_TRIGGER[(INT)APPEAR_STAGING::SOUND_PLAY1]) {
			SoundManager::GetInstance()->Play_Sound_Once(L"Docheol/Appear_Emblem_Staging.wav", CHANNELID::SOUND_EFFECT05, 0.75f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::SOUND_PLAY1] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 5.3f && STAGING_TRIGGER[(INT)APPEAR_STAGING::SOUND_PLAY2]) {
			SoundManager::GetInstance()->Play_Sound_Once(L"Docheol/Appear_HandOut.wav", CHANNELID::SOUND_EFFECT06, 2.f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::SOUND_PLAY2] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 7.4f && STAGING_TRIGGER[(INT)APPEAR_STAGING::SOUND_PLAY3]) {
			SoundManager::GetInstance()->Play_Sound_Once(L"Docheol/Appear_AfterWave.wav", CHANNELID::SOUND_EFFECT07, 3.f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::SOUND_PLAY3] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 8.7f && STAGING_TRIGGER[(INT)APPEAR_STAGING::SOUND_PLAY4]) {
			SoundManager::GetInstance()->Play_Sound_Once(L"Docheol/Appear_Roar.wav", CHANNELID::SOUND_EFFECT08, 2.f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::SOUND_PLAY4] = FALSE;
		}
	}
}
VOID	FinalBoss::Animation_Disappear_Staging(CONST FLOAT& _DT) {
	if (Enable_BossDisappearStaging) {

		BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] += _DT;
		_vec3 GlobalPosition = { Component_Transform->Get_Position()->x, Component_Transform->Get_Position()->y - 1.5f,  Component_Transform->Get_Position()->z - 6.5f };
		FLOAT GloabalScale = 2.f;

		if		(STAGING_TRIGGER[(INT)DEATH_STAGING::SPOOL_APPEAR]) {
			Camera->Camera_Shaking(10.f, 3.f);
			SoundTransition == (INT)SOUNDPLAYER::ESCAPING_WC;

			_vec3 SpoolAppearSca = { 250.f / 100.f * GloabalScale * 1.8f, 112.5f / 100.f * GloabalScale * 1.8f , 300.f / 100.f * GloabalScale * 1.8f };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 0.3f, GlobalPosition.z };
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::SPOOL_APPEAR_EFFECT, L"SPOOL_APPEAR_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::SPOOL_APPEAR] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 1.0f && STAGING_TRIGGER[(INT)DEATH_STAGING::SPOOL_FLOW1]) {
			_vec3 SpoolAppearSca = { 250.f / 100.f * GloabalScale * 1.5f, 112.5f / 100.f * GloabalScale * 1.5f, 300.f / 100.f * GloabalScale * 1.5f };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 0.3f, GlobalPosition.z };
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::SPOOL_FLOW_EFFECT, L"SPOOL_APPEAR_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::SPOOL_FLOW1] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 2.0f && STAGING_TRIGGER[(INT)DEATH_STAGING::SPOOL_FLOW2]) {
			_vec3 SpoolAppearSca = { 250.f / 100.f * GloabalScale * 1.5f, 112.5f / 100.f * GloabalScale * 1.5f, 300.f / 100.f * GloabalScale * 1.5f };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 0.3f, GlobalPosition.z };
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::SPOOL_FLOW_EFFECT, L"SPOOL_APPEAR_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::SPOOL_FLOW2] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 3.0f && STAGING_TRIGGER[(INT)DEATH_STAGING::SPOOL_FLOW3]) {
			_vec3 SpoolAppearSca = { 250.f / 100.f * GloabalScale * 1.5f, 112.5f / 100.f * GloabalScale * 1.5f, 300.f / 100.f * GloabalScale * 1.5f };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 0.3f, GlobalPosition.z };
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::SPOOL_FLOW_EFFECT, L"SPOOL_APPEAR_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::SPOOL_FLOW3] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 4.0f && STAGING_TRIGGER[(INT)DEATH_STAGING::SPOOL_FLOW4]) {
			_vec3 SpoolAppearSca = { 250.f / 100.f * GloabalScale * 1.5f, 112.5f / 100.f * GloabalScale * 1.5f, 300.f / 100.f * GloabalScale * 1.5f };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 0.3f, GlobalPosition.z };
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::SPOOL_FLOW_EFFECT, L"SPOOL_APPEAR_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::SPOOL_FLOW4] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 5.0f && STAGING_TRIGGER[(INT)DEATH_STAGING::SPOOL_FLOW5]) {
			_vec3 SpoolAppearSca = { 250.f / 100.f * GloabalScale * 1.5f, 112.5f / 100.f * GloabalScale * 1.5f, 300.f / 100.f * GloabalScale * 1.5f };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 0.3f, GlobalPosition.z };
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::SPOOL_DISAPPEAR_EFFECT, L"SPOOL_APPEAR_EFFECT", &SpoolAppearPos, SpoolAppearSca, 0.5f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::SPOOL_FLOW5] = FALSE;
		}

		if		(BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 3.0f && STAGING_TRIGGER[(INT)DEATH_STAGING::WATER_POPUP1]) {
			Camera->Camera_Shaking(30.f, 1.f);
			_vec3 SpoolAppearSca = { 1.2f * GloabalScale, 1.4f * GloabalScale, 1.4f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 2.f, GlobalPosition.y + 2.f, GlobalPosition.z + 3.f };
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::WATER_POPUP_EFFECT, L"WATER_POPUP_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.2f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::WATER_POPUP1] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 3.5f && STAGING_TRIGGER[(INT)DEATH_STAGING::WATER_POPUP2]) {
			Camera->Camera_Shaking(30.f, 1.f);
			_vec3 SpoolAppearSca = { 1.4f * GloabalScale, 1.7f * GloabalScale, 1.7f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x + 1.f, GlobalPosition.y + 1.5f, GlobalPosition.z + 0.5f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::WATER_POPUP_EFFECT, L"WATER_POPUP_EFFECT", &SpoolAppearPos, SpoolAppearSca, 0.6f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::WATER_POPUP2] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 4.0f && STAGING_TRIGGER[(INT)DEATH_STAGING::WATER_POPUP3]) {
			Camera->Camera_Shaking(30.f, 1.f);
			_vec3 SpoolAppearSca = { 1.3f * GloabalScale, 1.6f * GloabalScale, 1.6f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x + 1.5f, GlobalPosition.y + 2.f, GlobalPosition.z + 1.f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::WATER_POPUP_EFFECT, L"WATER_POPUP_EFFECT", &SpoolAppearPos, SpoolAppearSca, 0.8f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::WATER_POPUP3] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 4.5f && STAGING_TRIGGER[(INT)DEATH_STAGING::WATER_POPUP4]) {
			Camera->Camera_Shaking(30.f, 1.f);
			_vec3 SpoolAppearSca = { 2.f * GloabalScale, 3.f * GloabalScale, 4.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 1.f, GlobalPosition.y + 1.f, GlobalPosition.z + 4.4f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::WATER_POPUP_EFFECT, L"WATER_POPUP_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::WATER_POPUP4] = FALSE;
		}
		if		(BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 5.0f && STAGING_TRIGGER[(INT)DEATH_STAGING::BIG_FLAME]) {
			Camera->Camera_Shaking(30.f, 15.f);
			_vec3 SpoolAppearSca = { 5.f * GloabalScale, 5.f * GloabalScale, 5.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.25f, GlobalPosition.y + 1.4f, GlobalPosition.z + 5.f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::FLAME_EFFECT, L"BIG_FLAME_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::BIG_FLAME] = FALSE;
		}
		if		(BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 5.0f && STAGING_TRIGGER[(INT)DEATH_STAGING::BIG_CIRCLE_FLAME]) {
			_vec3 SpoolAppearSca = { 6.f * GloabalScale, 6.f / 2.f * GloabalScale, 6.f / 2.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x , GlobalPosition.y + 1.f, GlobalPosition.z - 1.f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::CIRCLE_FLAME_EFFECT, L"BIG_CIRCLE_FLAME_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::BIG_CIRCLE_FLAME] = FALSE;

			memset(STAGING_TRIGGER, TRUE, sizeof(STAGING_TRIGGER));
			Enable_BossDisappearStaging = FALSE;
			BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] = 0.f;
			ObjectDead = TRUE;

			//dynamic_cast<UIEffect*>(EffectManager::GetInstance()->Find_GlobalEffect(L"CLEAR_BREAK"))->Set_All_Visible(TRUE);
			//dynamic_cast<UIEffect*>(EffectManager::GetInstance()->Find_GlobalEffect(L"CLEAR_CHARGE"))->Set_All_Visible(TRUE);
			//dynamic_cast<UIEffect*>(EffectManager::GetInstance()->Find_GlobalEffect(L"CLEAR_MARK"))->Set_All_Visible(TRUE);
			//dynamic_cast<UIEffect*>(EffectManager::GetInstance()->Find_GlobalEffect(L"CLEAR_LINE"))->Set_All_Visible(TRUE);

			static_cast<MainUI*>(SceneManager::GetInstance()->Get_GameObject(L"MainUI"))->Set_BossClearUI(TRUE);
		}
	
		//////////////////////////////////////////////////// SOUNDPLAY ////////////////////////////////////////////////////
		if		(BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 0.1f && STAGING_TRIGGER[(INT)DEATH_STAGING::SOUND_PLAY1]) {
			SoundManager::GetInstance()->Play_Sound_Once(L"Docheol/Death_Groan.wav", CHANNELID::SOUND_EFFECT05, 1.5f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::SOUND_PLAY1] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 2.3f && STAGING_TRIGGER[(INT)DEATH_STAGING::SOUND_PLAY2]) {
			SoundManager::GetInstance()->Play_Sound_Once(L"Docheol/Death_Swamp.wav", CHANNELID::SOUND_EFFECT06, 2.f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::SOUND_PLAY2] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_STAGING] > 4.6f && STAGING_TRIGGER[(INT)DEATH_STAGING::SOUND_PLAY3]) {
			SoundManager::GetInstance()->Play_Sound_Once(L"Docheol/Death_Explosion.wav", CHANNELID::SOUND_EFFECT07, 2.f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::SOUND_PLAY3] = FALSE;
		}
	}
}

VOID	FinalBoss::Skill_GroundExplosion(CONST FLOAT& _DT) {
	if		(Enable_GroundExplosion == 0 && Enable_GroundQuadExplosion == 0) return;
	else if (Enable_GroundExplosion == 1) {
		BossTimer[(LONG)BOSSTIMER::TIMER_EXP] += _DT;
		if		(BossTimer[(LONG)BOSSTIMER::TIMER_EXP] > 0.23f && EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION4]) {
			SoundManager::GetInstance()->Play_Sound_Once(L"Docheol/Slam_Bump.wav", CHANNELID::SOUND_EFFECT07, 1.F);
			EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION4] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_EXP] > 0.50f) {
			_vec3 Scale = { 6.f, 6.f, 6.f };
			if (PlayerPos.z >= Component_Transform->Get_Position()->z - 7.f) {
				PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::SLAM_GROUND_EXP_EFFECT, L"Ground Explosion", &PlayerPos, Scale, 0.7f);
			}
			else{
				PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SLAM_GROUND_EXP_EFFECT, L"Ground Explosion", &PlayerPos, Scale, 0.7f);
			}
			
			dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion")
				->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Rotation(85.f, 0.f, 0.f);

			PlayerPos = { 0.f, 0.f, 0.f };
			Enable_GroundExplosion = FALSE;
			BossTimer[(LONG)BOSSTIMER::TIMER_EXP] = 0.f;

			EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION4] = TRUE; 
		}
	}
	else if (Enable_GroundExplosion == 2) {
		BossTimer[(LONG)BOSSTIMER::TIMER_EXP] += _DT;
		if		(BossTimer[(LONG)BOSSTIMER::TIMER_EXP] > 0.23f && EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION4]) {
			SoundManager::GetInstance()->Play_Sound_Once(L"Docheol/Slam_Bump.wav", CHANNELID::SOUND_EFFECT07, 1.F);
			EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION4] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_EXP] > 0.50f) {
			_vec3 Scale = { 6.f, 6.f, 6.f };
			if (PlayerPos.z >= Component_Transform->Get_Position()->z - 7.f) {
				PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion", &PlayerPos, Scale, 0.7f);
			}
			else {
				PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion", &PlayerPos, Scale, 0.7f);
			}
			dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
				->Set_Rotation(85.f, 0.f, 0.f);

			PlayerPos = { 0.f, 0.f, 0.f };
			Enable_GroundExplosion = FALSE;
			BossTimer[(LONG)BOSSTIMER::TIMER_EXP] = 0.f;

			EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION4] = TRUE;
		}
	}
	else if (Enable_GroundQuadExplosion == 1) {
		BossTimer[(LONG)BOSSTIMER::TIMER_EXP] += _DT;

		_vec3 Scale = { 6.f, 6.f, 6.f };
		_vec3 BossBottomPos = { Component_Transform->Get_Position()->x, 1.f, Component_Transform->Get_Position()->z - 7.f };
		_vec3 vecvec = PlayerPos - BossBottomPos;

		D3DXVec3Normalize(&vecvec, &vecvec);
		//////////////////////////////////////////////////// SOUNDPLAY ////////////////////////////////////////////////////
		if		(BossTimer[(LONG)BOSSTIMER::TIMER_EXP] > 0.23f	&& EXPLOSION_TRIGGER[(INT)EXPLOSION::SOUND_PLAY2]) {
			SoundManager::GetInstance()->Play_Sound_Once(L"Docheol/Slam_Bump.wav", CHANNELID::SOUND_EFFECT07, 0.75f);
			EXPLOSION_TRIGGER[(INT)EXPLOSION::SOUND_PLAY2] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_EXP] > 0.41f	&& EXPLOSION_TRIGGER[(INT)EXPLOSION::SOUND_PLAY3]) {
			SoundManager::GetInstance()->Play_Sound_Once(L"Docheol/Slam_Bump.wav", CHANNELID::SOUND_EFFECT07, 0.75f);
			EXPLOSION_TRIGGER[(INT)EXPLOSION::SOUND_PLAY3] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_EXP] > 0.59f	&& EXPLOSION_TRIGGER[(INT)EXPLOSION::SOUND_PLAY4]) {
			SoundManager::GetInstance()->Play_Sound_Once(L"Docheol/Slam_Bump.wav", CHANNELID::SOUND_EFFECT07, 0.75f);
			EXPLOSION_TRIGGER[(INT)EXPLOSION::SOUND_PLAY4] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_EXP] > 0.77f	&& EXPLOSION_TRIGGER[(INT)EXPLOSION::SOUND_PLAY5]) {
			SoundManager::GetInstance()->Play_Sound_Once(L"Docheol/Slam_Bump.wav", CHANNELID::SOUND_EFFECT07, 0.75f);
			EXPLOSION_TRIGGER[(INT)EXPLOSION::SOUND_PLAY5] = FALSE;
		}
		
		if		(BossTimer[(LONG)BOSSTIMER::TIMER_EXP] > 0.5f	&& EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION1]) {
				vecvec = vecvec * 6.f;
				vecvec += BossBottomPos;
				if (vecvec.z >= Component_Transform->Get_Position()->z - 7.f) {
					PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::SLAM_GROUND_EXP_EFFECT, L"Ground Explosion1", &vecvec, Scale, 0.7f);
				}
				else {
					PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SLAM_GROUND_EXP_EFFECT, L"Ground Explosion1", &vecvec, Scale, 0.7f);
				}
				dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion1")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
					->Set_Rotation(85.f, 0.f, 0.f);
				EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION1] = FALSE;
			}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_EXP] > 0.7f	&& EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION2]) {
			vecvec = vecvec * 9.f;
			vecvec += BossBottomPos;
			if (vecvec.z >= Component_Transform->Get_Position()->z - 7.f) {
				PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::SLAM_GROUND_EXP_EFFECT, L"Ground Explosion2", &vecvec, Scale, 0.7f);
			}
			else {
				PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SLAM_GROUND_EXP_EFFECT, L"Ground Explosion2", &vecvec, Scale, 0.7f);
			}
			dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion2")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
				->Set_Rotation(85.f, 0.f, 0.f);
			EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION2] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_EXP] > 0.9f	&& EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION3]) {
			vecvec = vecvec * 12.f;
			vecvec += BossBottomPos;
			if (vecvec.z >= Component_Transform->Get_Position()->z - 7.f) {
				PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::SLAM_GROUND_EXP_EFFECT, L"Ground Explosion3", &vecvec, Scale, 0.7f);
			}
			else {
				PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SLAM_GROUND_EXP_EFFECT, L"Ground Explosion3", &vecvec, Scale, 0.7f);
			}
			dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion3")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
				->Set_Rotation(85.f, 0.f, 0.f);
			EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION3] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_EXP] > 1.1f	&& EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION4]) {
			vecvec = vecvec * 15.f;
			vecvec += BossBottomPos;
			if (vecvec.z >= Component_Transform->Get_Position()->z - 7.f) {
				PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::SLAM_GROUND_EXP_EFFECT, L"Ground Explosion4", &vecvec, Scale, 0.7f);
			}
			else {
				PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SLAM_GROUND_EXP_EFFECT, L"Ground Explosion4", &vecvec, Scale, 0.7f);
			}
			dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion4")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
				->Set_Rotation(85.f, 0.f, 0.f);

			PlayerPos = { 0.f, 0.f, 0.f };
			memset(EXPLOSION_TRIGGER, TRUE, sizeof(EXPLOSION_TRIGGER));
			Enable_GroundQuadExplosion = FALSE;
			BossTimer[(LONG)BOSSTIMER::TIMER_EXP] = 0.f;
		}
	}
	//else if (Enable_GroundQuadExplosion == 2) {
	//	BossTimer[(LONG)BOSSTIMER::TIMER_EXP] += _DT;
	//
	//	_vec3 Scale = { 6.f, 6.f, 6.f };
	//	_vec3 BossBottomPos = { Component_Transform->Get_Position()->x, 1.f, Component_Transform->Get_Position()->z - 7.f };
	//	_vec3 vecvec = PlayerPos - BossBottomPos;
	//
	//	D3DXVec3Normalize(&vecvec, &vecvec);
	//	if		(BossTimer[(LONG)BOSSTIMER::TIMER_EXP] > 0.5f && EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION1]) {
	//		vecvec = vecvec * 4.f;
	//		vecvec += BossBottomPos;
	//		PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion1", &vecvec, Scale, 0.7f);
	//		dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion1")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
	//			->Set_Rotation(85.f, 0.f, 0.f);
	//		EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION1] = FALSE;
	//	}
	//	else if (BossTimer[(LONG)BOSSTIMER::TIMER_EXP] > 0.7f && EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION2]) {
	//		vecvec = vecvec * 8.f;
	//		vecvec += BossBottomPos;
	//		PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion2", &vecvec, Scale, 0.7f);
	//		dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion2")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
	//			->Set_Rotation(85.f, 0.f, 0.f);
	//		EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION2] = FALSE;
	//	}
	//	else if (BossTimer[(LONG)BOSSTIMER::TIMER_EXP] > 0.9f && EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION3]) {
	//		vecvec = vecvec * 12.f;
	//		vecvec += BossBottomPos;
	//		PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion3", &vecvec, Scale, 0.7f);
	//		dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion3")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
	//			->Set_Rotation(85.f, 0.f, 0.f);
	//		EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION3] = FALSE;
	//	}
	//	else if (BossTimer[(LONG)BOSSTIMER::TIMER_EXP] > 1.1f && EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION4]) {
	//		vecvec = vecvec * 16.f;
	//		vecvec += BossBottomPos;
	//		PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion4", &vecvec, Scale, 0.7f);
	//		dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion4")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
	//			->Set_Rotation(85.f, 0.f, 0.f);
	//
	//		PlayerPos = { 0.f, 0.f, 0.f };
	//		memset(EXPLOSION_TRIGGER, TRUE, sizeof(EXPLOSION_TRIGGER));
	//		Enable_GroundQuadExplosion = FALSE;
	//		BossTimer[(LONG)BOSSTIMER::TIMER_EXP] = 0.f;
	//	}
	//}
}
VOID	FinalBoss::Skill_MeteorExplosion(CONST FLOAT& _DT) {
	if (Enable_MeteorExplosion == 0) return;
	if (Enable_MeteorExplosion == 1) {
		BossTimer[(LONG)BOSSTIMER::TIMER_MEXP] += _DT;
		//////////////////////////////////////////////////// SOUNDPLAY ////////////////////////////////////////////////////
		if		(BossTimer[(LONG)BOSSTIMER::TIMER_MEXP] > 0.7f && METEOR_TRIGGER[(INT)METEOR::SOUND_PLAY1]) {
			SoundManager::GetInstance()->Play_Sound_Once(L"Docheol/Slam_Exp.wav", CHANNELID::SOUND_EFFECT08, 0.75f);
			METEOR_TRIGGER[(INT)METEOR::SOUND_PLAY1] = FALSE;
		}

		if		(BossTimer[(LONG)BOSSTIMER::TIMER_MEXP] > 0.2f	&& METEOR_TRIGGER[(INT)METEOR::DANGER_AREA]) {
			_vec3 Scale = { 1.f, 1.f, 1.f };

			srand(time(NULL));
			for (INT IDX = 0; IDX < 4; ++IDX)
				RanPosX[IDX] = (rand() % 20 - 10) + ((FLOAT)(rand() % 20 - 10) / 10.f);
			for (INT IDX = 0; IDX < 4; ++IDX)
				RanPosZ[IDX] = (rand() % 20 - 20) + ((FLOAT)(rand() % 20 - 20) / 10.f);

			for (INT IDX = 0; IDX < 4; ++IDX) {
				_vec3 Pos = { Component_Transform->Get_Position()->x + RanPosX[IDX], Component_Transform->Get_Position()->y - 1.5f, Component_Transform->Get_Position()->z + RanPosZ[IDX] };
				if (Pos.z >= Component_Transform->Get_Position()->z - 7.f) {
					PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"Explosion Warning" + to_wstring(IDX), &Pos, Scale, 0.6f);
				}
				else {
					PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"Explosion Warning" + to_wstring(IDX), &Pos, Scale, 0.6f);
				}
				dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Explosion Warning" + to_wstring(IDX))->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
					->Set_Rotation(85.f, 0.f, 0.f);
			}
			METEOR_TRIGGER[(INT)METEOR::DANGER_AREA] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_MEXP] > 0.25f && METEOR_TRIGGER[(INT)METEOR::METEOR_CREATE]) {
			_vec3 Pos0 = { Component_Transform->Get_Position()->x + RanPosX[0] + 2.f, Component_Transform->Get_Position()->y - 1.5f + 6.f, Component_Transform->Get_Position()->z + RanPosZ[0] + 12.f };
			_vec3 Pos1 = { Component_Transform->Get_Position()->x + RanPosX[1] + 2.f, Component_Transform->Get_Position()->y - 1.5f + 6.f, Component_Transform->Get_Position()->z + RanPosZ[1] + 12.f };
			_vec3 Pos2 = { Component_Transform->Get_Position()->x + RanPosX[2] - 2.f, Component_Transform->Get_Position()->y - 1.5f + 6.f, Component_Transform->Get_Position()->z + RanPosZ[2] + 12.f };
			_vec3 Pos3 = { Component_Transform->Get_Position()->x + RanPosX[3] - 2.f, Component_Transform->Get_Position()->y - 1.5f + 6.f, Component_Transform->Get_Position()->z + RanPosZ[3] + 12.f };
			_vec3 Scale = { 1.f, 1.f, 1.f };
			if (Pos0.z >= Component_Transform->Get_Position()->z - 7.f) { PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::METEOR_EFFECT, L"Meteor1", &Pos0, Scale, 0.65f); }
			else														{ PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::METEOR_EFFECT, L"Meteor1", &Pos0, Scale, 0.65f); }
			
			if (Pos1.z >= Component_Transform->Get_Position()->z - 7.f) { PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::METEOR_EFFECT, L"Meteor2", &Pos1, Scale, 0.65f); }
			else														{ PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::METEOR_EFFECT, L"Meteor2", &Pos1, Scale, 0.65f); }

			if (Pos2.z >= Component_Transform->Get_Position()->z - 7.f) { PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::METEOR_EFFECT, L"Meteor3", &Pos2, Scale, 0.65f); }
			else														{ PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::METEOR_EFFECT, L"Meteor3", &Pos2, Scale, 0.65f); }

			if (Pos3.z >= Component_Transform->Get_Position()->z - 7.f) { PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::METEOR_EFFECT, L"Meteor4", &Pos3, Scale, 0.65f); }
			else														{ PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::METEOR_EFFECT, L"Meteor4", &Pos3, Scale, 0.65f); }

			METEOR_TRIGGER[(INT)METEOR::METEOR_CREATE] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_MEXP] > 0.5f	&& BossTimer[(LONG)BOSSTIMER::TIMER_MEXP] <= 0.9f) {
			if (MeteorTransform[0] == nullptr) {
				for (INT IDX = 0; IDX < 4; ++IDX) {
					GameObject* Effect = EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Meteor" + to_wstring(IDX + 1));
					MeteorTransform[IDX] = dynamic_cast<Transform*>(Effect->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
				}
			}
			FLOAT MeteorSpeed = 0.01f;
			MeteorTransform[0]->Set_Pos(MeteorTransform[0]->Get_Position()->x - MeteorSpeed * 9, MeteorTransform[0]->Get_Position()->y - MeteorSpeed * 27, MeteorTransform[0]->Get_Position()->z - MeteorSpeed * 50);
			MeteorTransform[1]->Set_Pos(MeteorTransform[1]->Get_Position()->x - MeteorSpeed * 9, MeteorTransform[1]->Get_Position()->y - MeteorSpeed * 27, MeteorTransform[1]->Get_Position()->z - MeteorSpeed * 50);
			MeteorTransform[2]->Set_Pos(MeteorTransform[2]->Get_Position()->x + MeteorSpeed * 9, MeteorTransform[2]->Get_Position()->y - MeteorSpeed * 27, MeteorTransform[2]->Get_Position()->z - MeteorSpeed * 50);
			MeteorTransform[3]->Set_Pos(MeteorTransform[3]->Get_Position()->x + MeteorSpeed * 9, MeteorTransform[3]->Get_Position()->y - MeteorSpeed * 27, MeteorTransform[3]->Get_Position()->z - MeteorSpeed * 50);
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_MEXP] > 0.9f	&& METEOR_TRIGGER[(INT)METEOR::METEOR_EXPLOSION]) {
			Camera->Camera_Shaking(10, 0.25f);
			_vec3 Scale = { 3.f, 4.f, 4.f };
			if (MeteorTransform[0] == nullptr) {
				for (INT IDX = 0; IDX < 4; ++IDX) {
					MeteorTransform[IDX] = dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Meteor" + to_wstring(IDX + 1))->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
				}
			}
			_vec3 Pos0 = { MeteorTransform[0]->Get_Position()->x, MeteorTransform[0]->Get_Position()->y + 1.5f, MeteorTransform[0]->Get_Position()->z + 3.f };
			_vec3 Pos1 = { MeteorTransform[1]->Get_Position()->x, MeteorTransform[1]->Get_Position()->y + 1.5f, MeteorTransform[1]->Get_Position()->z + 3.f };
			_vec3 Pos2 = { MeteorTransform[2]->Get_Position()->x, MeteorTransform[2]->Get_Position()->y + 1.5f, MeteorTransform[2]->Get_Position()->z + 3.f };
			_vec3 Pos3 = { MeteorTransform[3]->Get_Position()->x, MeteorTransform[3]->Get_Position()->y + 1.5f, MeteorTransform[3]->Get_Position()->z + 3.f };
			if (Pos0.z >= Component_Transform->Get_Position()->z - 7.f) { PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::METEOR_EXP_EFFECT, L"MeteorExp1", &Pos0, Scale, 0.4f); }
			else														{ PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::METEOR_EXP_EFFECT, L"MeteorExp1", &Pos0, Scale, 0.4f); }

			if (Pos1.z >= Component_Transform->Get_Position()->z - 7.f) { PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::METEOR_EXP_EFFECT, L"MeteorExp2", &Pos1, Scale, 0.4f); }
			else														{ PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::METEOR_EXP_EFFECT, L"MeteorExp2", &Pos1, Scale, 0.4f); }

			if (Pos2.z >= Component_Transform->Get_Position()->z - 7.f) { PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::METEOR_EXP_EFFECT, L"MeteorExp3", &Pos2, Scale, 0.4f); }
			else														{ PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::METEOR_EXP_EFFECT, L"MeteorExp3", &Pos2, Scale, 0.4f); }

			if (Pos3.z >= Component_Transform->Get_Position()->z - 7.f) { PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::METEOR_EXP_EFFECT, L"MeteorExp4", &Pos3, Scale, 0.4f); }
			else														{ PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::METEOR_EXP_EFFECT, L"MeteorExp4", &Pos3, Scale, 0.4f); }

			METEOR_TRIGGER[(INT)METEOR::METEOR_EXPLOSION] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_MEXP] > 1.3f) {
			memset(MeteorTransform, 0, sizeof(MeteorTransform));
			memset(METEOR_TRIGGER, TRUE, sizeof(METEOR_TRIGGER));
			Enable_MeteorExplosion = FALSE;
			BossTimer[(LONG)BOSSTIMER::TIMER_MEXP] = 0.f;
		}
	}
}
VOID	FinalBoss::Skill_RSwingFireBall	(CONST FLOAT& _DT) {
	if		(Enable_CreateFireBall == 0) return;
	else if (Enable_CreateFireBall == 1) {
		BossTimer[(LONG)BOSSTIMER::TIMER_RSWING] += _DT;
		FLOAT SectorAngle = 10.f;

		if		(BossTimer[(LONG)BOSSTIMER::TIMER_RSWING] > 0.10f && FIREBALL_TRIGGER[(INT)FIREBALL::ANGLE_GENERATE]) {
			GeneratePos = { Component_Collider->Get_CenterPos()->Get_Position()->x - 0.5f, Component_Collider->Get_CenterPos()->Get_Position()->y - 1.f, Component_Collider->Get_CenterPos()->Get_Position()->z - 4.f };
			_vec3 PlayerPos = *PlayerTransform->Get_Position() - GeneratePos;
			_vec3 AxisXVec = { GeneratePos.x + 1, GeneratePos.y, GeneratePos.z };
			AxisXVec = AxisXVec - GeneratePos;
			D3DXVec3Normalize(&AxisXVec, &AxisXVec);
			D3DXVec3Normalize(&PlayerPos, &PlayerPos);

			if (GeneratePos.z >= PlayerTransform->Get_Position()->z)
				PlayerToAxisXDegree = D3DXToDegree(acosf(D3DXVec3Dot(&PlayerPos, &AxisXVec)));
			else
				PlayerToAxisXDegree = 360 - D3DXToDegree(acosf(D3DXVec3Dot(&PlayerPos, &AxisXVec)));

			for (INT IDX = 0; IDX < 15; ++IDX) {
				GameObject* GOBJ = BossFireBall::Create(GRPDEV);
				wstring FBall = L"FireBall" + to_wstring(IDX);
				GOBJ->Set_ObjectTag(FBall.c_str());
				ObjectPool_RageUp.push_back(GOBJ);
			}

			FIREBALL_TRIGGER[(INT)FIREBALL::ANGLE_GENERATE] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RSWING] > 0.15f && FIREBALL_TRIGGER[(INT)FIREBALL::FIRST_FIREBALL]) {
			Camera->Camera_Shaking(10, 1.f);
			for (INT IDX = 0; IDX < 5; ++IDX) {
				wstring FBTAG = L"FireBall" + to_wstring(IDX);
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, ObjectPool_RageUp[IDX]);
				dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(FBTAG.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree + SectorAngle * (IDX - 2));
				dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_GameObject(FBTAG.c_str())->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
					->Set_Pos(GeneratePos);
			}
			FIREBALL_TRIGGER[(INT)FIREBALL::FIRST_FIREBALL] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RSWING] > 0.25f && FIREBALL_TRIGGER[(INT)FIREBALL::SECOND_FIREBALL]) {
			GeneratePos = { GeneratePos.x, GeneratePos.y + 0.3f, GeneratePos.z };
			for (INT IDX = 0; IDX < 5; ++IDX) {
				wstring FBTAG = L"FireBall" + to_wstring(IDX + 5);
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, ObjectPool_RageUp[IDX + 5]);
				dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(FBTAG.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree + SectorAngle * (IDX - 2));
				dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(FBTAG.c_str()))->Set_FireBall_Pos(GeneratePos);
			}
			FIREBALL_TRIGGER[(INT)FIREBALL::SECOND_FIREBALL] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RSWING] > 0.35f && FIREBALL_TRIGGER[(INT)FIREBALL::THIRD_FIREBALL]) {
			GeneratePos = { GeneratePos.x, GeneratePos.y + 0.3f, GeneratePos.z };
			for (INT IDX = 0; IDX < 5; ++IDX) {
				wstring FBTAG = L"FireBall" + to_wstring(IDX + 10);
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, ObjectPool_RageUp[IDX + 10]);
				dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(FBTAG.c_str()))->Set_FireBall_Angle(PlayerToAxisXDegree + SectorAngle * (IDX - 2));
				dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(FBTAG.c_str()))->Set_FireBall_Pos(GeneratePos);
			}
			FIREBALL_TRIGGER[(INT)FIREBALL::THIRD_FIREBALL] = FALSE;

			ObjectPool_RageUp.clear();

			BossTimer[(LONG)BOSSTIMER::TIMER_RSWING] = 0.f;
			memset(FIREBALL_TRIGGER, TRUE, sizeof(FIREBALL_TRIGGER));
			Enable_CreateFireBall = FALSE;
		}
	}
	else if (Enable_CreateFireBall == 2) {
		BossTimer[(LONG)BOSSTIMER::TIMER_RSWING] += _DT;;
		FLOAT SectorAngle = 10.f;

		if		(BossTimer[(LONG)BOSSTIMER::TIMER_RSWING] > 0.10f && FIREBALL_TRIGGER[(INT)FIREBALL::ANGLE_GENERATE]) {
			GeneratePos = { Component_Collider->Get_CenterPos()->Get_Position()->x - 0.5f, Component_Collider->Get_CenterPos()->Get_Position()->y - 1.f, Component_Collider->Get_CenterPos()->Get_Position()->z - 4.f };
			_vec3 PlayerPos = *PlayerTransform->Get_Position() - GeneratePos;
			_vec3 AxisXVec = { GeneratePos.x + 1, GeneratePos.y, GeneratePos.z };
			AxisXVec = AxisXVec - GeneratePos;
			D3DXVec3Normalize(&AxisXVec, &AxisXVec);
			D3DXVec3Normalize(&PlayerPos, &PlayerPos);

			if (GeneratePos.z >= PlayerTransform->Get_Position()->z)
				PlayerToAxisXDegree = D3DXToDegree(acosf(D3DXVec3Dot(&PlayerPos, &AxisXVec)));
			else
				PlayerToAxisXDegree = 360 - D3DXToDegree(acosf(D3DXVec3Dot(&PlayerPos, &AxisXVec)));

			for (INT IDX = 0; IDX < 15; ++IDX) {
				GameObject* Pull = BossFireBall::Create(GRPDEV);
				BossFireBall* BFB = static_cast<BossFireBall*>(Pull);
				BFB->Set_FireBall_Angle(PlayerToAxisXDegree + SectorAngle * (IDX % 5 - 2));
				BFB->Set_FireBall_Pos(GeneratePos);
				wstring a = L"BPBP" + to_wstring(IDX);
				Pull->Set_ObjectTag(a.c_str());
				ObjectPool_RSwing.push_back(Pull);
			}
			for (INT IDX = 0; IDX < 18; ++IDX) {
				GameObject* Pull = BossFireBall::Create(GRPDEV);
				BossFireBall* BFB = static_cast<BossFireBall*>(Pull);
				BFB->Set_FireBall_Angle(PlayerToAxisXDegree - 25 + 10 * (IDX % 6));
				BFB->Set_FireBall_Pos(GeneratePos);
				wstring a = L"BPBP" + to_wstring(IDX);
				Pull->Set_ObjectTag(a.c_str());
				ObjectPool_RSwing.push_back(Pull);
			}

			FIREBALL_TRIGGER[(INT)FIREBALL::ANGLE_GENERATE] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RSWING] > 0.15f && FIREBALL_TRIGGER[(INT)FIREBALL::FIRST_FIREBALL]) {
			Camera->Camera_Shaking(10, 1.f);
			for (INT IDX = 0; IDX < 5; ++IDX)
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, ObjectPool_RSwing[IDX]);

			FIREBALL_TRIGGER[(INT)FIREBALL::FIRST_FIREBALL] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RSWING] > 0.25f && FIREBALL_TRIGGER[(INT)FIREBALL::SECOND_FIREBALL]) {
			GeneratePos = { GeneratePos.x, GeneratePos.y + 0.3f, GeneratePos.z };
			for (INT IDX = 0; IDX < 5; ++IDX)
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, ObjectPool_RSwing[5 + IDX]);
			FIREBALL_TRIGGER[(INT)FIREBALL::SECOND_FIREBALL] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RSWING] > 0.35f && FIREBALL_TRIGGER[(INT)FIREBALL::THIRD_FIREBALL]) {
			GeneratePos = { GeneratePos.x, GeneratePos.y + 0.3f, GeneratePos.z };
			for (INT IDX = 0; IDX < 5; ++IDX)
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, ObjectPool_RSwing[10 + IDX]);
			FIREBALL_TRIGGER[(INT)FIREBALL::THIRD_FIREBALL] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RSWING] > 0.65f && FIREBALL_TRIGGER[(INT)FIREBALL::FOURTH_FIREBALL]) {
			Camera->Camera_Shaking(10, 1.f);
			GeneratePos = { GeneratePos.x, GeneratePos.y + 0.3f, GeneratePos.z };
			for (INT IDX = 0; IDX < 6; ++IDX)
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, ObjectPool_RSwing[15 + IDX]);
			FIREBALL_TRIGGER[(INT)FIREBALL::FOURTH_FIREBALL] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RSWING] > 0.75f && FIREBALL_TRIGGER[(INT)FIREBALL::FIFTH_FIREBALL]) {
			GeneratePos = { GeneratePos.x, GeneratePos.y + 0.3f, GeneratePos.z };
			for (INT IDX = 0; IDX < 6; ++IDX)
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, ObjectPool_RSwing[21 + IDX]);
			FIREBALL_TRIGGER[(INT)FIREBALL::FIFTH_FIREBALL] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RSWING] > 0.85f && FIREBALL_TRIGGER[(INT)FIREBALL::FIREBALL_POOL]) {
			GeneratePos = { GeneratePos.x, GeneratePos.y + 0.3f, GeneratePos.z };
			for (INT IDX = 0; IDX < 6; ++IDX)
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, ObjectPool_RSwing[27 + IDX]);
			FIREBALL_TRIGGER[(INT)FIREBALL::FIREBALL_POOL] = FALSE;

			ObjectPool_RSwing.clear();
			BossTimer[(LONG)BOSSTIMER::TIMER_RSWING] = 0.f;
			memset(FIREBALL_TRIGGER, TRUE, sizeof(FIREBALL_TRIGGER));
			Enable_CreateFireBall = false;
		}
	}
}
VOID	FinalBoss::Skill_FSwingFireBall	(CONST FLOAT& _DT) {
	if (Enable_FSwingFireBall) {
		BossTimer[(INT)BOSSTIMER::TIMER_RSWING] += _DT;
		if (BossTimer[(INT)BOSSTIMER::TIMER_RSWING] <= 0.05f && ObjectPool_FSwing.size() <= 0) {
			for (INT IDX = 0; IDX < 30; IDX++) {
				GameObject* BPB = BossFireBall::Create(GRPDEV);
				BossFireBall* DBPB = static_cast<BossFireBall*>(BPB);
				_vec3 GeneratePos = {
					Component_Transform->Get_Position()->x - 0.5f,
					Component_Transform->Get_Position()->y - 1.0f,
					Component_Transform->Get_Position()->z - 4.0f
				};

				srand(time(NULL));
				random_device rd;
				mt19937 gen(rd());

				uniform_int_distribution<int> dis(0, 359);
				DBPB->Set_FireBall_Angle(dis(gen));
				DBPB->Set_FireBall_Pos(GeneratePos);

				ObjectPool_FSwing.push_back(BPB);
			}
		}
		else if (Animation_TexList == &Animation_Normal_FSwing_TexList && Animation_CurrentIndex == 6) {
			for (auto& BPB : ObjectPool_FSwing)
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, BPB);

			BossTimer[(INT)BOSSTIMER::TIMER_RSWING] = 0.f;
			Enable_FSwingFireBall = FALSE;
			ObjectPool_FSwing.clear();
		}
	}
}
VOID	FinalBoss::Skill_RageUpFireBall	(CONST FLOAT& _DT) {
	if (Enable_RageUpFireBall == FALSE)	return;
	if (Enable_RageUpFireBall == TRUE) {
		BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] += _DT;

		FLOAT SectorAngle = 20.f;

		//////////////////////////////////////////////////// SOUNDPLAY ////////////////////////////////////////////////////
		if		(BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 0.3f && FIREBALL_TRIGGER[(INT)FIREBALL::SOUND_PLAY1]) {
			PLAY_SOUND_ONCE(L"Docheol/FireBall_Sound.wav", CHANNELID::SOUND_EFFECT10, 1.f);
			FIREBALL_TRIGGER[(INT)FIREBALL::SOUND_PLAY1] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 0.9f && FIREBALL_TRIGGER[(INT)FIREBALL::SOUND_PLAY2]) {
			PLAY_SOUND_ONCE(L"Docheol/FireBall_Sound.wav", CHANNELID::SOUND_EFFECT10, 1.f);
			FIREBALL_TRIGGER[(INT)FIREBALL::SOUND_PLAY2] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 1.5f && FIREBALL_TRIGGER[(INT)FIREBALL::SOUND_PLAY3]) {
			PLAY_SOUND_ONCE(L"Docheol/FireBall_Sound.wav", CHANNELID::SOUND_EFFECT10, 1.f);
			FIREBALL_TRIGGER[(INT)FIREBALL::SOUND_PLAY3] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 2.1f && FIREBALL_TRIGGER[(INT)FIREBALL::SOUND_PLAY4]) {
			PLAY_SOUND_ONCE(L"Docheol/FireBall_Sound.wav", CHANNELID::SOUND_EFFECT10, 1.f);
			FIREBALL_TRIGGER[(INT)FIREBALL::SOUND_PLAY4] = FALSE;
		}

		if		(BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 0.05f && FIREBALL_TRIGGER[(INT)FIREBALL::FIREBALL_POOL]) {
			for (INT IDX = 0; IDX < 48; ++IDX) {
				GameObject* Pull = BossFireBall::Create(GRPDEV);
				BossFireBall* BFB = static_cast<BossFireBall*>(Pull);
				BFB->Set_Boss(this);
				BFB->Set_BossPosition(Component_Transform);
				BFB->Set_FireBall_Angle(10 * (IDX / 12) + IDX * 30);
				static_cast<Transform*>(Pull->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(
					{ Component_Transform->Get_Position()->x, Component_Transform->Get_Position()->y - 0.5f, Component_Transform->Get_Position()->z - 5.f }
				);
				ObjectPool_RageUp.push_back(Pull);
			}
			FIREBALL_TRIGGER[(INT)FIREBALL::FIREBALL_POOL] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 0.45f && FIREBALL_TRIGGER[(INT)FIREBALL::FIRST_FIREBALL]) {
			for (INT IDX = 0; IDX < 12; ++IDX) {
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, ObjectPool_RageUp[IDX]);
			}
			FIREBALL_TRIGGER[(INT)FIREBALL::FIRST_FIREBALL] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 1.05f && FIREBALL_TRIGGER[(INT)FIREBALL::SECOND_FIREBALL]) {
			for (INT IDX = 0; IDX < 12; ++IDX) {
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, ObjectPool_RageUp[12 + IDX]);
			}
			FIREBALL_TRIGGER[(INT)FIREBALL::SECOND_FIREBALL] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 1.65f && FIREBALL_TRIGGER[(INT)FIREBALL::THIRD_FIREBALL]) {
			for (INT IDX = 0; IDX < 12; ++IDX) {
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, ObjectPool_RageUp[12 * 2 + IDX]);
			}
			FIREBALL_TRIGGER[(INT)FIREBALL::THIRD_FIREBALL] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 2.25f && FIREBALL_TRIGGER[(INT)FIREBALL::FOURTH_FIREBALL]) {
			for (INT IDX = 0; IDX < 12; ++IDX) {
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, ObjectPool_RageUp[12 * 3 + IDX]);
			}
			BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] = 0.f;
			BossMode[(LONG)BOSSMODE::MODE_INVALIDATE] = false;
			memset(FIREBALL_TRIGGER, TRUE, sizeof(FIREBALL_TRIGGER));
			Enable_RageUpFireBall = FALSE;
		}
	}
}
VOID	FinalBoss::Skill_SupporterFlame	(CONST FLOAT& _DT) {
	if (Enable_SupporterFlame) {
		BossTimer[(LONG)BOSSTIMER::TIMER_SUPPORT] += _DT;
		_vec3 EmblemScale = { 1.f, 1.f, 1.f };
		_vec3 CenterPosition = { Component_Transform->Get_Position()->x - 1.f, Component_Transform->Get_Position()->y - 0.5f, Component_Transform->Get_Position()->z - 3.f };

		_vec3 Sup1_Pos = { CenterPosition.x, CenterPosition.y + 0.1f, CenterPosition.z};
		_vec3 Sup2_pos = { CenterPosition.x - 1.2f, CenterPosition.y + 1.9f * sinf(D3DXToRadian(10)) + 0.1f, CenterPosition.z + 1.9f * cosf(D3DXToRadian(10)) };
		_vec3 Sup3_pos = { CenterPosition.x + 1.2f, CenterPosition.y + 1.9f * sinf(D3DXToRadian(10)) + 0.1f, CenterPosition.z + 1.9f * cosf(D3DXToRadian(10)) };
		_vec3 Sup4_pos = { CenterPosition.x - 1.2f, CenterPosition.y - 1.9f * sinf(D3DXToRadian(10)) + 0.1f, CenterPosition.z - 1.9f * cosf(D3DXToRadian(10)) };
		_vec3 Sup5_pos = { CenterPosition.x + 1.2f, CenterPosition.y - 1.9f * sinf(D3DXToRadian(10)) + 0.1f, CenterPosition.z - 1.9f * cosf(D3DXToRadian(10)) };
		_vec3 Sup6_pos = { CenterPosition.x + 2.3f, CenterPosition.y + 0.1f, CenterPosition.z };
		_vec3 Sup7_pos = { CenterPosition.x - 2.3f, CenterPosition.y + 0.1f, CenterPosition.z };

		if		(BossTimer[(LONG)BOSSTIMER::TIMER_SUPPORT] > 0.1f	&& SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP_POOLING]) {
			for (INT IDX = 0; IDX < 7; ++IDX) {
				GameObject* SUP = Supporter::Create(GRPDEV);
				dynamic_cast<Supporter*>(SUP)->Set_SupporterType(2);
				wstring tag = L"Supporter_Bullet" + to_wstring(IDX + 1);
				SUP->Set_ObjectTag(tag.c_str());
				dynamic_cast<Transform*>(SUP->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Scale(2.f, 2.f, 2.f);
				ObjectPool_Supporter.push_back(SUP);
			}
			SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP_POOLING] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_SUPPORT] > 0.75f  && SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP1_SPAWN]) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SUPPORTER_APPEAR_EFFECT, L"SUP1_SPAWN", &Sup1_Pos, EmblemScale, 0.5f);
			SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP1_SPAWN] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_SUPPORT] > 1.00f	&& SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP2_SPAWN]) {
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<Supporter>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, ObjectPool_Supporter[0]);
			dynamic_cast<Supporter*>(ObjectPool_Supporter[0])->Set_ScaleInc(TRUE);
			dynamic_cast<Supporter*>(ObjectPool_Supporter[0])->Set_DirectionVec({ 0.f, 0.f, 0.f });
			Sup1_Pos.y -= 0.1f;
			dynamic_cast<Transform*>(ObjectPool_Supporter[0]->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(Sup1_Pos);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SUPPORTER_APPEAR_EFFECT, L"SUP2_SPAWN", &Sup2_pos, EmblemScale, 0.5f);

			SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP2_SPAWN] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_SUPPORT] > 1.25f	&& SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP3_SPAWN]) {
			PLAY_BOSS_FRONTEFFECT(BOSS_EFFECT::SUPPORTER_STAY_EFFECT, L"SUP1_STAY", &Sup1_Pos, EmblemScale, 2.25f);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<Supporter>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, ObjectPool_Supporter[1]);
			dynamic_cast<Supporter*>(ObjectPool_Supporter[1])->Set_ScaleInc(TRUE);
			dynamic_cast<Supporter*>(ObjectPool_Supporter[1])->Set_DirectionVec({ Sup2_pos.x - CenterPosition.x , 0.f, Sup2_pos.z - CenterPosition.z });
			Sup2_pos.y -= 0.1f;
			dynamic_cast<Transform*>(ObjectPool_Supporter[1]->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(Sup2_pos);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SUPPORTER_APPEAR_EFFECT, L"SUP3_SPAWN", &Sup3_pos, EmblemScale, 0.5f);

			SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP3_SPAWN] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_SUPPORT] > 1.50f	&& SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP4_SPAWN]) {
			PLAY_BOSS_FRONTEFFECT(BOSS_EFFECT::SUPPORTER_STAY_EFFECT, L"SUP2_STAY", &Sup2_pos, EmblemScale, 2.f);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<Supporter>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, ObjectPool_Supporter[2]);
			dynamic_cast<Supporter*>(ObjectPool_Supporter[2])->Set_ScaleInc(TRUE);
			dynamic_cast<Supporter*>(ObjectPool_Supporter[2])->Set_DirectionVec({ Sup3_pos.x - CenterPosition.x , 0.f, Sup3_pos.z - CenterPosition.z });
			Sup3_pos.y -= 0.1f;
			dynamic_cast<Transform*>(ObjectPool_Supporter[2]->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(Sup3_pos);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SUPPORTER_APPEAR_EFFECT, L"SUP4_SPAWN", &Sup4_pos, EmblemScale, 0.5f);

			SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP4_SPAWN] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_SUPPORT] > 1.75f	&& SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP5_SPAWN]) {
			PLAY_BOSS_FRONTEFFECT(BOSS_EFFECT::SUPPORTER_STAY_EFFECT, L"SUP3_STAY", &Sup3_pos, EmblemScale, 1.75f);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<Supporter>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, ObjectPool_Supporter[3]);
			dynamic_cast<Supporter*>(ObjectPool_Supporter[3])->Set_ScaleInc(TRUE);
			dynamic_cast<Supporter*>(ObjectPool_Supporter[3])->Set_DirectionVec({ Sup4_pos.x - CenterPosition.x , 0.f, Sup4_pos.z - CenterPosition.z });
			Sup4_pos.y -= 0.1f;
			dynamic_cast<Transform*>(ObjectPool_Supporter[3]->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(Sup4_pos);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SUPPORTER_APPEAR_EFFECT, L"SUP5_SPAWN", &Sup5_pos, EmblemScale, 0.5f);

			SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP5_SPAWN] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_SUPPORT] > 2.00f	&& SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP6_SPAWN]) {
			PLAY_BOSS_FRONTEFFECT(BOSS_EFFECT::SUPPORTER_STAY_EFFECT, L"SUP4_STAY", &Sup4_pos, EmblemScale, 1.5f);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<Supporter>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, ObjectPool_Supporter[4]);
			dynamic_cast<Supporter*>(ObjectPool_Supporter[4])->Set_ScaleInc(TRUE);
			dynamic_cast<Supporter*>(ObjectPool_Supporter[4])->Set_DirectionVec({ Sup5_pos.x - CenterPosition.x , 0.f, Sup5_pos.z - CenterPosition.z });
			Sup5_pos.y -= 0.1f;
			dynamic_cast<Transform*>(ObjectPool_Supporter[4]->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(Sup5_pos);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SUPPORTER_APPEAR_EFFECT, L"SUP6_SPAWN", &Sup6_pos, EmblemScale, 0.5f);

			SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP6_SPAWN] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_SUPPORT] > 2.25f	&& SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP7_SPAWN]) {
			PLAY_BOSS_FRONTEFFECT(BOSS_EFFECT::SUPPORTER_STAY_EFFECT, L"SUP5_STAY", &Sup5_pos, EmblemScale, 1.25f);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<Supporter>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, ObjectPool_Supporter[5]);
			dynamic_cast<Supporter*>(ObjectPool_Supporter[5])->Set_ScaleInc(TRUE);
			dynamic_cast<Supporter*>(ObjectPool_Supporter[5])->Set_DirectionVec({ 1.f, 0.f, 0.f });
			Sup6_pos.y -= 0.1f;
			dynamic_cast<Transform*>(ObjectPool_Supporter[5]->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(Sup6_pos);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SUPPORTER_APPEAR_EFFECT, L"SUP7_SPAWN", &Sup7_pos, EmblemScale, 0.5f);

			SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP7_SPAWN] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_SUPPORT] > 2.50f	&& SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP_SPAWN_END]) {
			PLAY_BOSS_FRONTEFFECT(BOSS_EFFECT::SUPPORTER_STAY_EFFECT, L"SUP6_STAY", &Sup6_pos, EmblemScale, 1.f);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<Supporter>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, ObjectPool_Supporter[6]);
			dynamic_cast<Supporter*>(ObjectPool_Supporter[6])->Set_ScaleInc(TRUE);
			dynamic_cast<Supporter*>(ObjectPool_Supporter[6])->Set_DirectionVec({ -1.f, 0.f, 0.f });
			Sup7_pos.y -= 0.1f;
			dynamic_cast<Transform*>(ObjectPool_Supporter[6]->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(Sup7_pos);

			SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP_SPAWN_END] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_SUPPORT] > 2.75f	&& SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP_SPAWNFADEIN1]) {
			PLAY_BOSS_FRONTEFFECT(BOSS_EFFECT::SUPPORTER_STAY_EFFECT, L"SUP7_STAY", &Sup7_pos, EmblemScale, 0.75f);
			SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP_SPAWNFADEIN1] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_SUPPORT] > 3.50f	&& BossTimer[(LONG)BOSSTIMER::TIMER_SUPPORT] <= 4.5f) {
			if (BossTimer[(LONG)BOSSTIMER::TIMER_SUPPORT] > 3.5f && SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP_MOVE]) {
				for (INT IDX = 0; IDX < ObjectPool_Supporter.size(); ++IDX) {
					dynamic_cast<Supporter*>(ObjectPool_Supporter[IDX])->Set_RageMove(TRUE);
					wstring SUPTAG = L"SUP" + to_wstring(IDX + 1) + L"_STAY";
					EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, SUPTAG.c_str())->Set_ObjectDead(TRUE);
					SUPTAG = L"SUP" + to_wstring(IDX + 1) + L"_Disappear";
					PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SUPPORTER_DISAPPEAR_EFFECT, SUPTAG, dynamic_cast<Transform*>(ObjectPool_Supporter[IDX]->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position(), EmblemScale, 1.5f)
				}
				SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP_MOVE] = FALSE;
			}
			for (INT IDX = 0; IDX < ObjectPool_Supporter.size(); ++IDX) {
				GameObject* Effect = EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"SUP" + to_wstring(IDX + 1) + L"_Disappear");
				if (Effect != nullptr) {
					_vec3 EffectPos = *dynamic_cast<Transform*>(ObjectPool_Supporter[IDX]->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position();
					dynamic_cast<Transform*>(Effect->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(EffectPos);
				}
			}
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_SUPPORT] > 3.00f	&& EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"SUP7_Disappear") != nullptr) {
			for (INT IDX = 0; IDX < ObjectPool_Supporter.size(); ++IDX) {
				wstring SUPTAG = L"SUP" + to_wstring(IDX + 1) + L"_Disappear";
				GameObject* Effect = EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, SUPTAG);
				dynamic_cast<Transform*>(Effect->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
					->Set_Pos(*dynamic_cast<Transform*>(ObjectPool_Supporter[IDX]->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position());
			}
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_SUPPORT] > 6.00f	&& SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP_DISAPPEAR]) {
			memset(SUPPORTER_TRIGGER, TRUE, sizeof(SUPPORTER_TRIGGER));
			//for (auto& i : ObjectPool_Supporter)
			//	Safe_Release(i);
			ObjectPool_Supporter.clear();
			Animation_TexList = &Animation_Rage_Stand_TexList;
			Animation_CurrentIndex = 0;
			Animation_FrameCount = ANIMATION_RAGE_STAND_FRAMECOUNT;
			BossTimer[(LONG)BOSSTIMER::TIMER_SUPPORT] = 0.f;
			Enable_SupporterFlame = FALSE;
			Component_FSM->FSM_StateChange(Rage_StandState::GetInstance()->Instance());
		}
	
		//////////////////////////////////////////////////// SOUNDPLAY ////////////////////////////////////////////////////
		
		if		(BossTimer[(LONG)BOSSTIMER::TIMER_SUPPORT] > 0.6f			&& SUPPORTER_TRIGGER[(INT)SUPPORTER::SOUND_PLAY1]) {
			VOLUME(CHANNELID::SOUND_EFFECT09, 0.1f);
			PLAY_SOUND_ONCE(L"Docheol/Supporter_Summon.wav", CHANNELID::SOUND_EFFECT09);
			SUPPORTER_TRIGGER[(INT)SUPPORTER::SOUND_PLAY1] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_SUPPORT] > 0.6f + 0.3f * 1 && SUPPORTER_TRIGGER[(INT)SUPPORTER::SOUND_PLAY2]) {
			PLAY_SOUND_ONCE(L"Docheol/Supporter_Summon.wav", CHANNELID::SOUND_EFFECT09);
			SUPPORTER_TRIGGER[(INT)SUPPORTER::SOUND_PLAY2] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_SUPPORT] > 0.6f + 0.3f * 2 && SUPPORTER_TRIGGER[(INT)SUPPORTER::SOUND_PLAY3]) {
			PLAY_SOUND_ONCE(L"Docheol/Supporter_Summon.wav", CHANNELID::SOUND_EFFECT09);
			SUPPORTER_TRIGGER[(INT)SUPPORTER::SOUND_PLAY3] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_SUPPORT] > 0.6f + 0.3f * 3 && SUPPORTER_TRIGGER[(INT)SUPPORTER::SOUND_PLAY4]) {
			PLAY_SOUND_ONCE(L"Docheol/Supporter_Summon.wav", CHANNELID::SOUND_EFFECT09);
			SUPPORTER_TRIGGER[(INT)SUPPORTER::SOUND_PLAY4] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_SUPPORT] > 0.6f + 0.3f * 4 && SUPPORTER_TRIGGER[(INT)SUPPORTER::SOUND_PLAY5]) {
			PLAY_SOUND_ONCE(L"Docheol/Supporter_Summon.wav", CHANNELID::SOUND_EFFECT09);
			SUPPORTER_TRIGGER[(INT)SUPPORTER::SOUND_PLAY5] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_SUPPORT] > 0.6f + 0.3f * 5 && SUPPORTER_TRIGGER[(INT)SUPPORTER::SOUND_PLAY6]) {
			PLAY_SOUND_ONCE(L"Docheol/Supporter_Summon.wav", CHANNELID::SOUND_EFFECT09);
			SUPPORTER_TRIGGER[(INT)SUPPORTER::SOUND_PLAY6] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_SUPPORT] > 0.6f + 0.3f * 6 && SUPPORTER_TRIGGER[(INT)SUPPORTER::SOUND_PLAY7]) {
			PLAY_SOUND_ONCE(L"Docheol/Supporter_Summon.wav", CHANNELID::SOUND_EFFECT09);
			SUPPORTER_TRIGGER[(INT)SUPPORTER::SOUND_PLAY7] = FALSE;
		}
	}
}
VOID	FinalBoss::Skill_ExplosionRush	(CONST FLOAT& _DT) {
	if (Enable_ExplosionRush) {
		BossTimer[(LONG)BOSSTIMER::TIMER_RUSH] += _DT;

		if		(ERUSH_TRIGGER[(INT)RUSH::RUSH_POOL]) {
			ObjectPool_RageUp.clear();
			for (INT IDX = 0; IDX < 60; ++IDX) {
				GameObject* Pull = BossFireBall::Create(GRPDEV);
				dynamic_cast<BossFireBall*>(Pull)->Set_Boss(this);
				dynamic_cast<BossFireBall*>(Pull)->Set_BossPosition(Component_Transform);
				dynamic_cast<BossFireBall*>(Pull)->Set_FireBall_Angle((24 * IDX) % 360 + 15 * (IDX / 15));
				dynamic_cast<Transform*>(Pull->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(
					{ Component_Transform->Get_Position()->x, Component_Transform->Get_Position()->y - 0.5f, Component_Transform->Get_Position()->z - 5.f }
				);
				Pull->Set_ObjectTag(TEXT("ERush_FireBall%d", IDX));
				ObjectPool_RageUp.push_back(Pull);
			}
			Animation_Interval = 0.35f;
			ERUSH_TRIGGER[(INT)RUSH::RUSH_POOL] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RUSH] > 0.25f && ERUSH_TRIGGER[(INT)RUSH::RUSH_FIREBALL1]) {
			for (INT IDX = 0; IDX < 15; ++IDX)
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, ObjectPool_RageUp[IDX]);

			ERUSH_TRIGGER[(INT)RUSH::RUSH_FIREBALL1] = FALSE;
			Enable_BBTrap = TRUE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RUSH] > 0.85f && ERUSH_TRIGGER[(INT)RUSH::RUSH_FIREBALL2]) {
			for (INT IDX = 15; IDX < 30; ++IDX)
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, ObjectPool_RageUp[IDX]);
			ERUSH_TRIGGER[(INT)RUSH::RUSH_FIREBALL2] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RUSH] > 1.45f && ERUSH_TRIGGER[(INT)RUSH::RUSH_FIREBALL3]) {
			for (INT IDX = 30; IDX < 45; ++IDX)
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, ObjectPool_RageUp[IDX]);
			ERUSH_TRIGGER[(INT)RUSH::RUSH_FIREBALL3] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RUSH] > 2.05f && ERUSH_TRIGGER[(INT)RUSH::RUSH_FIREBALL4]) {
			for (INT IDX = 45; IDX < 60; ++IDX)
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, ObjectPool_RageUp[IDX]);
			ERUSH_TRIGGER[(INT)RUSH::RUSH_FIREBALL4] = FALSE;
		}
		
		//////////////////////////////////////////////////// SOUNDPLAY ////////////////////////////////////////////////////
		if		(ERUSH_TRIGGER[(INT)RUSH::RUSH_EXPLOSION1]) {
			PLAY_SOUND_ONCE(L"Docheol/Rush_Ready.wav", CHANNELID::SOUND_EFFECT09, 2.f);
			ERUSH_TRIGGER[(INT)RUSH::RUSH_EXPLOSION1] = FALSE;
		}
		else if	(BossTimer[(LONG)BOSSTIMER::TIMER_RUSH] > 0.10f && ERUSH_TRIGGER[(INT)RUSH::RUSH_EXPLOSION2]) {
			PLAY_SOUND_ONCE(L"Docheol/FireBall_Sound.wav", CHANNELID::SOUND_EFFECT10, 1.f);
			ERUSH_TRIGGER[(INT)RUSH::RUSH_EXPLOSION2] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RUSH] > 0.70f && ERUSH_TRIGGER[(INT)RUSH::RUSH_EXPLOSION3]) {
			PLAY_SOUND_ONCE(L"Docheol/FireBall_Sound.wav", CHANNELID::SOUND_EFFECT10, 1.f);
			ERUSH_TRIGGER[(INT)RUSH::RUSH_EXPLOSION3] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RUSH] > 1.30f && ERUSH_TRIGGER[(INT)RUSH::RUSH_EXPLOSION4]) {
			PLAY_SOUND_ONCE(L"Docheol/FireBall_Sound.wav", CHANNELID::SOUND_EFFECT10, 1.f);
			ERUSH_TRIGGER[(INT)RUSH::RUSH_EXPLOSION4] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RUSH] > 1.90f && ERUSH_TRIGGER[(INT)RUSH::RUSH_EXPLOSION5]) {
			PLAY_SOUND_ONCE(L"Docheol/FireBall_Sound.wav", CHANNELID::SOUND_EFFECT10, 1.f);
			ERUSH_TRIGGER[(INT)RUSH::RUSH_EXPLOSION5] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RUSH] > 1.55f && ERUSH_TRIGGER[(INT)RUSH::RUSH_FLAME1]) {
			Animation_TexList = &Animation_Rage_Stand_TexList;
			Animation_CurrentIndex = 0;
			Animation_FrameCount = ANIMATION_RAGE_STAND_FRAMECOUNT;
			Animation_Interval = 0.07f;

			ERUSH_TRIGGER[(INT)RUSH::RUSH_FLAME1] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RUSH] > 4.55f && ERUSH_TRIGGER[(INT)RUSH::RUSH_FLAME2]) {
			BossTimer[(LONG)BOSSTIMER::TIMER_RUSH] = 0.f;
			Enable_ExplosionRush = FALSE;
			memset(ERUSH_TRIGGER, TRUE, sizeof(ERUSH_TRIGGER));
			ObjectPool_RageUp.clear();
		}
	}
}

VOID	FinalBoss::BoobieTrap(CONST FLOAT& _DT) {
	if (Enable_BBTrap) {
		_vec3 BossBottomPos = { Component_Transform->Get_Position()->x, 1.f, Component_Transform->Get_Position()->z - 7.f };

		_vec3 DGR000[5], DGR090[5], DGR180[5], DGR270[5],
			DGR330[5], DGR060[5], DGR150[5], DGR240[5],
			DGR300[5], DGR030[5], DGR120[5], DGR210[5];

		for (INT IDX = 0; IDX < 5; ++IDX) {
			DGR000[IDX] = { BossBottomPos.x + (IDX + 1.5f) * 3 * cosf(D3DXToRadian(0.00f)),	BossBottomPos.y, BossBottomPos.z - (IDX + 1.5f) * 2.5f * sinf(D3DXToRadian(0.00f)) };
			DGR090[IDX] = { BossBottomPos.x + (IDX + 1.5f) * 3 * cosf(D3DXToRadian(90.0f)),	BossBottomPos.y, BossBottomPos.z - (IDX + 1.5f) * 2.5f * sinf(D3DXToRadian(90.0f)) };
			DGR180[IDX] = { BossBottomPos.x + (IDX + 1.5f) * 3 * cosf(D3DXToRadian(180.f)),	BossBottomPos.y, BossBottomPos.z - (IDX + 1.5f) * 2.5f * sinf(D3DXToRadian(180.f)) };
			DGR270[IDX] = { BossBottomPos.x + (IDX + 1.5f) * 3 * cosf(D3DXToRadian(270.f)),	BossBottomPos.y, BossBottomPos.z - (IDX + 1.5f) * 2.5f * sinf(D3DXToRadian(270.f)) };

			DGR330[IDX] = { BossBottomPos.x + (IDX + 1.5f) * 3 * cosf(D3DXToRadian(330.f)),	BossBottomPos.y, BossBottomPos.z - (IDX + 1.5f) * 2.5f * sinf(D3DXToRadian(330.f)) };
			DGR060[IDX] = { BossBottomPos.x + (IDX + 1.5f) * 3 * cosf(D3DXToRadian(60.0f)),	BossBottomPos.y, BossBottomPos.z - (IDX + 1.5f) * 2.5f * sinf(D3DXToRadian(60.0f)) };
			DGR150[IDX] = { BossBottomPos.x + (IDX + 1.5f) * 3 * cosf(D3DXToRadian(150.f)),	BossBottomPos.y, BossBottomPos.z - (IDX + 1.5f) * 2.5f * sinf(D3DXToRadian(150.f)) };
			DGR240[IDX] = { BossBottomPos.x + (IDX + 1.5f) * 3 * cosf(D3DXToRadian(240.f)),	BossBottomPos.y, BossBottomPos.z - (IDX + 1.5f) * 2.5f * sinf(D3DXToRadian(240.f)) };

			DGR300[IDX] = { BossBottomPos.x + (IDX + 1.5f) * 3 * cosf(D3DXToRadian(300.f)),	BossBottomPos.y, BossBottomPos.z - (IDX + 1.5f) * 2.5f * sinf(D3DXToRadian(300.f)) };
			DGR030[IDX] = { BossBottomPos.x + (IDX + 1.5f) * 3 * cosf(D3DXToRadian(30.0f)),	BossBottomPos.y, BossBottomPos.z - (IDX + 1.5f) * 2.5f * sinf(D3DXToRadian(30.0f)) };
			DGR120[IDX] = { BossBottomPos.x + (IDX + 1.5f) * 3 * cosf(D3DXToRadian(120.f)),	BossBottomPos.y, BossBottomPos.z - (IDX + 1.5f) * 2.5f * sinf(D3DXToRadian(120.f)) };
			DGR210[IDX] = { BossBottomPos.x + (IDX + 1.5f) * 3 * cosf(D3DXToRadian(210.f)),	BossBottomPos.y, BossBottomPos.z - (IDX + 1.5f) * 2.5f * sinf(D3DXToRadian(210.f)) };
		}
		_vec3 EXPScale = { 5.f, 5.f, 5.f };
		_vec3 Scale = { 3.f, 3.f, 3.f };

		BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] += _DT;
		if		(BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 0.1f && BBTrap[0]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR000N0", &DGR000[0], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR090N0", &DGR090[0], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR180N0", &DGR180[0], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR270N0", &DGR270[0], Scale, 0.3f);

			BBTrap[0] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 0.2f && BBTrap[1]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR000N1", &DGR000[1], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR090N1", &DGR090[1], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR180N1", &DGR180[1], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR270N1", &DGR270[1], Scale, 0.3f);

			BBTrap[1] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 0.3f && BBTrap[2]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR000N2", &DGR000[2], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR090N2", &DGR090[2], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR180N2", &DGR180[2], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR270N2", &DGR270[2], Scale, 0.3f);

			BBTrap[2] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 0.4f && BBTrap[3]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR000N3", &DGR000[3], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR090N3", &DGR090[3], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR180N3", &DGR180[3], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR270N3", &DGR270[3], Scale, 0.3f);

			BBTrap[3] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 0.5f && BBTrap[4]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR000N4", &DGR000[4], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR090N4", &DGR090[4], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR180N4", &DGR180[4], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR270N4", &DGR270[4], Scale, 0.3f);

			BBTrap[4] = FALSE;
		}

		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 0.7f && BBTrap[5]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR330N0", &DGR330[0], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR060N0", &DGR060[0], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR150N0", &DGR150[0], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR240N0", &DGR240[0], Scale, 0.3f);

			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion000N0", &DGR000[0], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion090N0", &DGR090[0], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion180N0", &DGR180[0], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion270N0", &DGR270[0], EXPScale, 0.7f);

			BBTrap[5] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 0.8f && BBTrap[6]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR330N1", &DGR330[1], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR060N1", &DGR060[1], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR150N1", &DGR150[1], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR240N1", &DGR240[1], Scale, 0.3f);

			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion000N1", &DGR000[1], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion090N1", &DGR090[1], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion180N1", &DGR180[1], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion270N1", &DGR270[1], EXPScale, 0.7f);
			BBTrap[6] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 0.9f && BBTrap[7]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR330N2", &DGR330[2], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR060N2", &DGR060[2], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR150N2", &DGR150[2], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR240N2", &DGR240[2], Scale, 0.3f);

			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion000N2", &DGR000[2], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion090N2", &DGR090[2], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion180N2", &DGR180[2], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion270N2", &DGR270[2], EXPScale, 0.7f);
			BBTrap[7] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 1.0f && BBTrap[8]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR330N3", &DGR330[3], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR060N3", &DGR060[3], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR150N3", &DGR150[3], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR240N3", &DGR240[3], Scale, 0.3f);

			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion000N3", &DGR000[3], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion090N3", &DGR090[3], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion180N3", &DGR180[3], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion270N3", &DGR270[3], EXPScale, 0.7f);
			BBTrap[8] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 1.1f && BBTrap[9]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR330N4", &DGR330[4], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR060N4", &DGR060[4], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR150N4", &DGR150[4], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR240N4", &DGR240[4], Scale, 0.3f);

			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion000N4", &DGR000[4], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion090N4", &DGR090[4], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion180N4", &DGR180[4], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion270N4", &DGR270[4], EXPScale, 0.7f);
			BBTrap[9] = FALSE;
		}

		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 1.2f && BBTrap[10]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR330N0", &DGR300[0], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR060N0", &DGR030[0], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR150N0", &DGR120[0], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR240N0", &DGR210[0], Scale, 0.3f);

			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion330N0", &DGR330[0], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion060N0", &DGR060[0], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion150N0", &DGR150[0], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion240N0", &DGR240[0], EXPScale, 0.7f);
			BBTrap[10] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 1.3f && BBTrap[11]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR330N1", &DGR300[1], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR060N1", &DGR030[1], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR150N1", &DGR120[1], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR240N1", &DGR210[1], Scale, 0.3f);

			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion330N1", &DGR330[1], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion060N1", &DGR060[1], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion150N1", &DGR150[1], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion240N1", &DGR240[1], EXPScale, 0.7f);
			BBTrap[11] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 1.4f && BBTrap[12]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR330N2", &DGR300[2], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR060N2", &DGR030[2], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR150N2", &DGR120[2], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR240N2", &DGR210[2], Scale, 0.3f);

			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion330N2", &DGR330[2], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion060N2", &DGR060[2], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion150N2", &DGR150[2], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion240N2", &DGR240[2], EXPScale, 0.7f);
			BBTrap[12] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 1.5f && BBTrap[13]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR330N3", &DGR300[3], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR060N3", &DGR030[3], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR150N3", &DGR120[3], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR240N3", &DGR210[3], Scale, 0.3f);

			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion330N3", &DGR330[3], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion060N3", &DGR060[3], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion150N3", &DGR150[3], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion240N3", &DGR240[3], EXPScale, 0.7f);
			BBTrap[13] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 1.6f && BBTrap[14]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR330N4", &DGR300[4], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR060N4", &DGR030[4], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR150N4", &DGR120[4], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR240N4", &DGR210[4], Scale, 0.3f);

			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion330N4", &DGR330[4], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion060N4", &DGR060[4], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion150N4", &DGR150[4], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion240N4", &DGR240[4], EXPScale, 0.7f);
			BBTrap[14] = FALSE;
		}

		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 1.7f && BBTrap[15]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR000N0", &DGR000[0], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR090N0", &DGR090[0], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR180N0", &DGR180[0], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR270N0", &DGR270[0], Scale, 0.3f);

			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion300N0", &DGR300[0], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion030N0", &DGR030[0], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion120N0", &DGR120[0], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion210N0", &DGR210[0], EXPScale, 0.7f);
			BBTrap[15] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 1.8f && BBTrap[16]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR000N1", &DGR000[1], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR090N1", &DGR090[1], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR180N1", &DGR180[1], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR270N1", &DGR270[1], Scale, 0.3f);

			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion300N1", &DGR300[1], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion030N1", &DGR030[1], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion120N1", &DGR120[1], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion210N1", &DGR210[1], EXPScale, 0.7f);
			BBTrap[16] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 1.9f && BBTrap[17]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR000N2", &DGR000[2], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR090N2", &DGR090[2], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR180N2", &DGR180[2], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR270N2", &DGR270[2], Scale, 0.3f);

			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion300N2", &DGR300[2], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion030N2", &DGR030[2], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion120N2", &DGR120[2], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion210N2", &DGR210[2], EXPScale, 0.7f);
			BBTrap[17] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 2.0f && BBTrap[18]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR000N3", &DGR000[3], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR090N3", &DGR090[3], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR180N3", &DGR180[3], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR270N3", &DGR270[3], Scale, 0.3f);

			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion300N3", &DGR300[3], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion030N3", &DGR030[3], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion120N3", &DGR120[3], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion210N3", &DGR210[3], EXPScale, 0.7f);
			BBTrap[18] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 2.1f && BBTrap[19]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR000N4", &DGR000[4], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR090N4", &DGR090[4], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR180N4", &DGR180[4], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR270N4", &DGR270[4], Scale, 0.3f);

			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion300N4", &DGR300[4], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion030N4", &DGR030[4], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion120N4", &DGR120[4], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion210N4", &DGR210[4], EXPScale, 0.7f);
			BBTrap[19] = FALSE;
		}

		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 2.2f && BBTrap[20]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion000N0L", &DGR000[0], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion090N0L", &DGR090[0], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion180N0L", &DGR180[0], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion270N0L", &DGR270[0], EXPScale, 0.7f);
			BBTrap[20] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 2.3f && BBTrap[21]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion000N1L", &DGR000[1], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion090N1L", &DGR090[1], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion180N1L", &DGR180[1], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion270N1L", &DGR270[1], EXPScale, 0.7f);
			BBTrap[21] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 2.4f && BBTrap[22]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion000N2L", &DGR000[2], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion090N2L", &DGR090[2], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion180N2L", &DGR180[2], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion270N2L", &DGR270[2], EXPScale, 0.7f);
			BBTrap[22] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 2.5f && BBTrap[23]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion000N3L", &DGR000[3], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion090N3L", &DGR090[3], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion180N3L", &DGR180[3], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion270N3L", &DGR270[3], EXPScale, 0.7f);
			BBTrap[23] = FALSE;
		}
		else if (BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] > 2.6f && BBTrap[24]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion000N4L", &DGR000[4], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion090N4L", &DGR090[4], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion180N4L", &DGR180[4], EXPScale, 0.7f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion270N4L", &DGR270[4], EXPScale, 0.7f);
			memset(BBTrap, TRUE, sizeof(BBTrap));
			Enable_BBTrap = FALSE;
			BossTimer[(LONG)BOSSTIMER::TIMER_RAGEUP] = 0.f;
		}
		BossTimer[(LONG)BOSSTIMER::TIMER_SOUND] += _DT;
		//////////////////////////////////////////////////// SOUNDPLAY ////////////////////////////////////////////////////
		if		(BossTimer[(LONG)BOSSTIMER::TIMER_SOUND] > 0.6f && ERUSH_TRIGGER[(INT)RUSH::RUSH_SOUND1]) {
			SoundManager::GetInstance()->Play_Sound_Once(L"Docheol/Slam_Bump.wav", CHANNELID::SOUND_EFFECT11, 1.F);
			ERUSH_TRIGGER[(INT)RUSH::RUSH_SOUND1] = FALSE;
		}
	}
}

VOID	FinalBoss::BGM_Player(CONST FLOAT& _DT) {
	if (SoundTransition == (INT)SOUNDPLAYER::ESCAPING_DUNGEON) { // INC BEFORE COMBA
		SoundTransition = (INT)SOUNDPLAYER::ENTERING_BC;
		SoundManager::GetInstance()->Play_Sound(L"Docheol/BackGround_BeforeCombat.wav", CHANNELID::SOUND_BGM02, 0.f);
		SoundManager::GetInstance()->Set_ChannelVolume(CHANNELID::SOUND_BGM02, 0.f);

		SoundVolume = 0.f;
	}
	else if		(SoundTransition == (INT)SOUNDPLAYER::ENTERING_BC	)	{ // INC BEFORE COMBAT
		if (SoundManager::GetInstance()->Get_ChannelVolume(CHANNELID::SOUND_BGM02) <= 0.8f) {
			SoundVolume += _DT / 4.f;
			SoundManager::GetInstance()->Set_ChannelVolume(CHANNELID::SOUND_BGM02, SoundVolume);
		}
		else {
			SoundTransition = (INT)SOUNDPLAYER::STAY_BC;
			SoundVolume = 0.8f;
		}
	}
	else if (SoundTransition == (INT)SOUNDPLAYER::STAY_BC		)		{ // STAY BEFORE COMBAT
		SoundManager::GetInstance()->Set_ChannelVolume(CHANNELID::SOUND_BGM02, 0.8f);
		SoundVolume = 0.8f;
	}
	else if (SoundTransition == (INT)SOUNDPLAYER::ESCAPING_BC	)	{ // DEC BEFORE COMBAT
		if (SoundManager::GetInstance()->Get_ChannelVolume(CHANNELID::SOUND_BGM02) >= 0.f) {
			SoundVolume -= _DT / 4.f;
			SoundManager::GetInstance()->Set_ChannelVolume(CHANNELID::SOUND_BGM02, SoundVolume);
		}
		else {
			SoundTransition = (INT)SOUNDPLAYER::ENTERING_WC;
			SoundVolume = 0.f;
			STOP_ALLSOUND;
			SoundManager::GetInstance()->Play_Sound(L"Docheol/BackGround_Start.wav", CHANNELID::SOUND_BGM02, SoundVolume);
		}
	}
	else if (SoundTransition == (INT)SOUNDPLAYER::ENTERING_WC	)	{ // INC COMBAT
		if (SoundManager::GetInstance()->Get_ChannelVolume(CHANNELID::SOUND_BGM02) <= 0.8f) {
			SoundVolume += _DT / 4.f;
			SoundManager::GetInstance()->Set_ChannelVolume(CHANNELID::SOUND_BGM02, SoundVolume);
		}
		else {
			SoundTransition = (INT)SOUNDPLAYER::STAY_WC;
			SoundVolume = 0.5f;
		}
	}
	else if (SoundTransition == (INT)SOUNDPLAYER::STAY_WC		)		{ // STAY COMBAT
		SoundManager::GetInstance()->Set_ChannelVolume(CHANNELID::SOUND_BGM02, 0.8f);
		SoundVolume = 0.8f;
	}
	else if (SoundTransition == (INT)SOUNDPLAYER::ESCAPING_WC	)	{ // DEC COMBAT
		if (SoundManager::GetInstance()->Get_ChannelVolume(CHANNELID::SOUND_BGM02) >= 0.f) {
			SoundVolume -= _DT / 4.f;
			SoundManager::GetInstance()->Set_ChannelVolume(CHANNELID::SOUND_BGM02, SoundVolume);
		}
		else {
			SoundTransition = (INT)SOUNDPLAYER::ENTERING_WC;
			SoundVolume = 0.f;
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
	Safe_Release(Component_FSM);
	for (auto& i : ObjectPool_Supporter)
		Safe_Release(i);
	GameObject::Free();
}