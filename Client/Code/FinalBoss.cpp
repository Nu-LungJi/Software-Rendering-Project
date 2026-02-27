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

	ObjectTAG = L"Docheol";
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
	Enable_RageUpFireBall = FALSE;
	RageUp_Timer = 0.f;
	Rush_Timer = 0.f;

	Enable_BossDisappearStaging = FALSE;
	Enable_SupporterFlame = FALSE;
	Enable_ExplosionRush = FALSE;

	memset(MeteorTransform, 0, sizeof(MeteorTransform));
	memset(STAGING_TRIGGER, TRUE, sizeof(STAGING_TRIGGER));
	memset(EXPLOSION_TRIGGER, TRUE, sizeof(EXPLOSION_TRIGGER));
	memset(METEOR_TRIGGER, TRUE, sizeof(METEOR_TRIGGER));
	memset(FIREBALL_TRIGGER, TRUE, sizeof(FIREBALL_TRIGGER));
	memset(SUPPORTER_TRIGGER, TRUE, sizeof(SUPPORTER_TRIGGER));
	memset(ERUSH_TRIGGER, TRUE, sizeof(ERUSH_TRIGGER));
	memset(BBTrap, TRUE, sizeof(BBTrap));

	_vec3 cameraDir = *(Camera->Get_EyeVec()) - *(Camera->Get_AtVec());
	_vec3 planeDir = { 0.f, 1.f, 0.f };

	_float angle = acosf(D3DXVec3Dot(D3DXVec3Normalize(&cameraDir, &cameraDir), D3DXVec3Normalize(&planeDir, &planeDir)));
	_float _cameraAngle = angle / D3DX_PI * 180.f;

	Component_Transform->Rotation(ROT_X, 80.f);//90.f - _cameraAngle);

	return S_OK;
}
INT		FinalBoss::Update_GameObject(CONST FLOAT& _DT) {
	if (ObjectDead) return -1;
	
	GameObject::Update_GameObject(_DT);
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	FSM->Update_GameObject(_DT);

	if (Animation_TexList == &Animation_NonAnim_TexList &&
		dynamic_cast<Transform*>(PlayerObject->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position()->z >= 90.f)
		Enable_BossAppearStaging = TRUE;

	if (Animation_TexList == &Animation_Death_TexList && Animation_CurrentIndex == ANIMATION_DEATH_FRAMECOUNT - 1) {
		Animation_Timer = 0.f;
		Action_Timer = 0.f;
		Animation_CurrentIndex = ANIMATION_DEATH_FRAMECOUNT - 1;
	}
	Skill_GroundExplosion(_DT);
	Skill_MeteorExplosion(_DT);
	Skill_RSwingFireBall(_DT);
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
	if (Invalidate_Mode == FALSE || Action_Mode == FALSE)
		Action_Timer += _DT;

	if (Action_Timer > 3.f) {
		srand(time(NULL));
		if (Rage_Mode == FALSE) { Action_Selector = rand() % 4 + 1; }
		else if (Rage_Mode == TRUE) { 
			Action_Selector = 4; 
		} //rand() % 4 + 1;}//rand() % 5 + 1; } 보스 패턴 추가 시 적용
	
		Action_Timer = 0.f;
	}

	if		(Rage_Mode == FALSE) {
		// < Stand -> RSwing >
		if (Animation_TexList == &Animation_Normal_Stand_TexList && Action_Selector == 1) {
				Animation_CurrentIndex = 0;
				Animation_TexList = &Animation_Normal_RSwing_TexList;
				Animation_FrameCount = ANIMATION_NORMAL_RSWING_FRAMECOUNT;

				FSM->FSM_StateChange(RSwingState::GetInstance()->Instance());
				Action_Mode = TRUE;
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

		if ((Animation_TexList == &Animation_Normal_RSwing_TexList && Animation_CurrentIndex == ANIMATION_NORMAL_RSWING_FRAMECOUNT - 1)
			|| (Animation_TexList == &Animation_Normal_Slam_TexList && Animation_CurrentIndex == ANIMATION_NORMAL_SLAM_FRAMECOUNT - 1)
			|| (Animation_TexList == &Animation_Normal_FSwing_TexList && Animation_CurrentIndex == ANIMATION_NORMAL_FSWING_FRAMECOUNT - 1)) {
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
				Action_Mode = FALSE;
			}
		// <<< RageMode >>>
		if (Component_Collider->Get_Hp() <= 500.f && Rage_Mode == FALSE) {
				Animation_CurrentIndex = 0;
				Animation_TexList = &Animation_Stunning_TexList;
				Animation_FrameCount = ANIMATION_STUNNING_FRAMECOUNT;
				Animation_Interval = 0.25f;

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
		// < Stand -> Rush >
		if (Animation_TexList == &Animation_Rage_Stand_TexList && Action_Selector == 3 && Rush_Timer <= 1.f) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Rage_Rush_TexList;
			Animation_FrameCount = ANIMATION_RAGE_RUSH_FRAMECOUNT;

			Enable_ExplosionRush = TRUE;
			FSM->FSM_StateChange(Rage_ExplosionRushState::GetInstance()->Instance());
			Action_Mode = TRUE;
			Action_Selector = 0;
		}
		// < Stand -> Supporter >
		if (Animation_TexList == &Animation_Rage_Stand_TexList && Action_Selector == 4 && Supporter_Timer <= 1.f) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Rage_Charge_TexList;
			Animation_FrameCount = ANIMATION_RAGE_CHARGE_FRAMECOUNT;

			FSM->FSM_StateChange(Rage_SupporterState::GetInstance()->Instance());
			Enable_SupporterFlame = TRUE;
			Action_Mode = TRUE;
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
		if ((	Animation_TexList == &Animation_Rage_RSwing_TexList && Animation_CurrentIndex == ANIMATION_RAGE_RSWING_FRAMECOUNT - 1)
			|| (Animation_TexList == &Animation_Rage_Slam_TexList && Animation_CurrentIndex == ANIMATION_RAGE_SLAM_FRAMECOUNT - 1)
			|| (Animation_TexList == &Animation_RageUp_TexList && Animation_CurrentIndex == ANIMATION_RAGEUP_FRAMECOUNT - 1)
			//|| (Animation_TexList == &Animation_Rage_Charge_TexList && Animation_CurrentIndex == ANIMATION_RAGE_CHARGE_FRAMECOUNT - 1)
			|| (Animation_TexList == &Animation_Rage_Stand_TexList && Animation_CurrentIndex == ANIMATION_RAGE_STAND_FRAMECOUNT - 1
			|| (Animation_TexList == &Animation_Rage_Rush_TexList && Animation_CurrentIndex == ANIMATION_RAGE_RUSH_FRAMECOUNT - 1)	
				&& FSM->FSM_GetCurrentState() != Rage_ChargeState::GetInstance()->Instance())) {
				Animation_CurrentIndex = 0;
				Animation_TexList = &Animation_Rage_Stand_TexList;
				Animation_FrameCount = ANIMATION_RAGE_STAND_FRAMECOUNT;
				Animation_Interval = 0.07f;
				Action_Mode = FALSE;
				FSM->FSM_StateChange(Rage_StandState::GetInstance()->Instance());
			}
	}

	return 0;
}
VOID	FinalBoss::LateUpdate_GameObject(CONST FLOAT& _DT)	{
	GameObject::LateUpdate_GameObject(_DT);

	if (KEY_DOWN(DIK_O)) Enable_ExplosionRush = TRUE;
	if (KEY_DOWN(DIK_I)) {
		Enable_ExplosionRush = FALSE;
		Rush_Timer = 0.f;
		RageUp_Timer = 0.f;
		memset(ERUSH_TRIGGER, TRUE, sizeof(ERUSH_TRIGGER));
		memset(BBTrap, TRUE, sizeof(BBTrap));
	}
	if (KEY_DOWN(DIK_P)) {
		Rage_Mode = TRUE;
		Animation_TexList = &Animation_Rage_Stand_TexList;
		Animation_FrameCount = ANIMATION_RAGE_STAND_FRAMECOUNT;
		Animation_CurrentIndex = 0;
	}
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

	for (INT PIC = 1; PIC <= ANIMATION_RAGE_RUSH_FRAMECOUNT; ++PIC) {
		Base = L"Boss_Rush" + to_wstring(PIC) + L".png";
		Animation_Rage_Rush_TexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
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
		if		(Staging_Timer > 2.f	&& STAGING_TRIGGER	[(INT)APPEAR_STAGING::SPOOL_APPEAR]) {
			_vec3 SpoolAppearSca = { 250.f / 100.f * GloabalScale, 112.5f / 100.f * GloabalScale, 300.f / 100.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 1.f, GlobalPosition.z + 1.65f};
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::SPOOL_APPEAR_EFFECT, L"SPOOL_APPEAR_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::SPOOL_APPEAR] = FALSE;
		}
		else if (Staging_Timer > 3.f	&& STAGING_TRIGGER	[(INT)APPEAR_STAGING::SPOOL_FLOW1]) {
			_vec3 SpoolAppearSca = { 250.f / 100.f * GloabalScale, 112.5f / 100.f * GloabalScale, 300.f / 100.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 1.f, GlobalPosition.z + 1.65f };
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::SPOOL_FLOW_EFFECT, L"SPOOL_FLOW_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::SPOOL_FLOW1] = FALSE;
		}
		else if (Staging_Timer > 4.f	&& STAGING_TRIGGER	[(INT)APPEAR_STAGING::SPOOL_FLOW2]) {
			_vec3 SpoolAppearSca = { 250.f / 100.f * GloabalScale, 112.5f / 100.f * GloabalScale, 300.f / 100.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 1.f, GlobalPosition.z + 1.65f };
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::SPOOL_FLOW_EFFECT, L"SPOOL_FLOW_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::SPOOL_FLOW2] = FALSE;
		}
		else if (Staging_Timer > 5.f	&& STAGING_TRIGGER	[(INT)APPEAR_STAGING::SPOOL_FLOW3]) {
			_vec3 SpoolAppearSca = { 250.f / 100.f * GloabalScale, 112.5f / 100.f * GloabalScale, 300.f / 100.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 1.f, GlobalPosition.z + 1.65f };
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::SPOOL_FLOW_EFFECT, L"SPOOL_FLOW_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::SPOOL_FLOW3] = FALSE;
		}
		// Emblem Staging
		if		(Staging_Timer > 4.f	&& STAGING_TRIGGER	[(INT)APPEAR_STAGING::EMBLEM_APPEAR]) {
			_vec3 SpoolAppearSca = { 1.f * GloabalScale, 1.f * GloabalScale, 1.2f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 1.f, GlobalPosition.z + 1.7f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::EMBLEM_APPEAR_EFFECT, L"EMBLEM_APPEAR", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::EMBLEM_APPEAR] = FALSE;
		}
		else if (Staging_Timer > 5.f	&& STAGING_TRIGGER	[(INT)APPEAR_STAGING::EMBLEM_DESTROY]) {
			_vec3 SpoolAppearSca = { 1.f * GloabalScale, 1.f * GloabalScale, 1.2f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 1.f, GlobalPosition.z + 1.7f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::EMBLEM_DESTROY_EFFECT, L"EMBLEM_DESTROY", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::EMBLEM_DESTROY] = FALSE;
		}
		if		(Staging_Timer > 5.8f	&& STAGING_TRIGGER	[(INT)APPEAR_STAGING::ANIMATION]) {
			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Appear_TexList;
			Animation_FrameCount = ANIMATION_APPEAR_FRAMECOUNT;
			Component_Transform->Set_Pos(Component_Transform->Get_Position()->x - 0.3f, Component_Transform->Get_Position()->y, Component_Transform->Get_Position()->z + 0.5f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::ANIMATION] = FALSE;
		}
		// Blue Water Staging
		if		(Staging_Timer > 6.f	&& STAGING_TRIGGER	[(INT)APPEAR_STAGING::WATER_POPUP]) {
			Camera->Camera_Shaking(10, 0.5f);
			_vec3 SpoolAppearSca = { 3.f * GloabalScale, 2.5f * 2.f * GloabalScale, 5.f * 2.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x, GlobalPosition.y + 2.f, GlobalPosition.z + 8.4f };
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::WATER_POPUP_EFFECT, L"WATER_POPUP_EFFECT", &SpoolAppearPos, SpoolAppearSca, 0.7f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::WATER_POPUP] = FALSE;
		}
		// Small Flame Staging
		if		(Staging_Timer > 6.f	&& STAGING_TRIGGER	[(INT)APPEAR_STAGING::SMALL_FLAMEL]) {
			_vec3 SpoolAppearSca = { 3.5f * GloabalScale, 3.5f * GloabalScale, 3.5f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 4.5f, GlobalPosition.y + 3.f / 2.f, GlobalPosition.z + 4.f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::FLAME_EFFECT, L"SMALL_FLAMEL", &SpoolAppearPos, SpoolAppearSca, 1.f);
			dynamic_cast<BossEffect*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"SMALL_FLAMEL"))->Set_EffectRotation(0.f, -60.f, 10.f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::SMALL_FLAMEL] = FALSE;
		}
		if		(Staging_Timer > 6.2f	&& STAGING_TRIGGER	[(INT)APPEAR_STAGING::SMALL_FLAMER]) {
			_vec3 SpoolAppearSca = { 4.5f * GloabalScale, 4.5f * GloabalScale, 4.5f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x + 4.5f, GlobalPosition.y + 2.5f, GlobalPosition.z + 6.3f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::FLAME_EFFECT, L"SMALL_FLAMER", &SpoolAppearPos, SpoolAppearSca, 1.f);
			dynamic_cast<BossEffect*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"SMALL_FLAMER"))->Set_EffectRotation(0.f, 60.f, -10.f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::SMALL_FLAMER] = FALSE;
		}
		if		(Staging_Timer > 6.2f	&& STAGING_TRIGGER	[(INT)APPEAR_STAGING::SMALL_FLAMEC]) {
			_vec3 SpoolAppearSca = { 4.5f * GloabalScale, 4.5f * GloabalScale, 4.5f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x + 2.5f, GlobalPosition.y + 2.5f , GlobalPosition.z + 6.3f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::FLAME_EFFECT, L"SMALL_FLAMEC", &SpoolAppearPos, SpoolAppearSca, 1.f);
			dynamic_cast<BossEffect*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"SMALL_FLAMEC"))->Set_EffectRotation(0.f, 20.f, 0.f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::SMALL_FLAMEC] = FALSE;
			Animation_Interval = 0.14f;
		}

		// Big Flame Staging
		if		(Staging_Timer > 8.f	&& STAGING_TRIGGER	[(INT)APPEAR_STAGING::BIG_FLAME]) {
			Camera->Camera_Shaking(30, 1.f);
			_vec3 SpoolAppearSca = { 5.f * GloabalScale, 5.f * GloabalScale, 5.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.25f, GlobalPosition.y + 3.1f / 2.f, GlobalPosition.z + 6.f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::FLAME_EFFECT, L"BIG_FLAME_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::BIG_FLAME] = FALSE;
		}
		if		(Staging_Timer > 8.f	&& STAGING_TRIGGER	[(INT)APPEAR_STAGING::BIG_CIRCLE_FLAME]) {
			_vec3 SpoolAppearSca = { 6.f * GloabalScale, 6.f / 2.f * GloabalScale, 6.f / 2.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x , GlobalPosition.y + 1.f, GlobalPosition.z + 1.f };
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::CIRCLE_FLAME_EFFECT, L"BIG_CIRCLE_FLAME", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::BIG_CIRCLE_FLAME] = FALSE;
		}
		// Circle Flame Staging
		if		(Staging_Timer > 8.f	&& STAGING_TRIGGER	[(INT)APPEAR_STAGING::SPIRAL_FLAME]) {
			_vec3 SpoolAppearSca = { 6.f * GloabalScale, 6.f / 2.f * GloabalScale, 6.f / 2.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x , GlobalPosition.y + 2.1f / 2.f , GlobalPosition.z + 3.5f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SPIRAL_FLAME_EFFECT, L"SPIRAL_FLAME_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.4f);
			STAGING_TRIGGER[(INT)APPEAR_STAGING::SPIRAL_FLAME] = FALSE;

			Component_Transform->Set_Pos(Component_Transform->Get_Position()->x + 0.3f, Component_Transform->Get_Position()->y, Component_Transform->Get_Position()->z - 0.5f);
		}

		if		(Staging_Timer > 9.5f	&& STAGING_TRIGGER	[(INT)APPEAR_STAGING::CAMERA_SHAKE]) {
			Camera->Camera_Shaking(30, 1.f);
			Animation_Interval = 0.07f;
			STAGING_TRIGGER[(INT)APPEAR_STAGING::CAMERA_SHAKE] = FALSE;
			Staging_Timer = 0.f;
		}
	}
}
VOID FinalBoss::Animation_Disappear_Staging(CONST FLOAT& _DT) {
	if (Enable_BossDisappearStaging) {
		Staging_Timer += _DT;
		_vec3 GlobalPosition = { Component_Transform->Get_Position()->x, Component_Transform->Get_Position()->y - 1.5f,  Component_Transform->Get_Position()->z - 6.5f };
		FLOAT GloabalScale = 2.f;

		if		(						STAGING_TRIGGER[(INT)DEATH_STAGING::SPOOL_APPEAR]) {
			Camera->Camera_Shaking(1.f, 3.f);
			_vec3 SpoolAppearSca = { 250.f / 100.f * GloabalScale * 1.5f, 112.5f / 100.f * GloabalScale * 1.5f , 300.f / 100.f * GloabalScale * 1.5f };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 0.3f, GlobalPosition.z };
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::SPOOL_APPEAR_EFFECT, L"SPOOL_APPEAR_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::SPOOL_APPEAR] = FALSE;
		}
		else if (Staging_Timer > 1.f && STAGING_TRIGGER[(INT)DEATH_STAGING::SPOOL_FLOW1]) {
			_vec3 SpoolAppearSca = { 250.f / 100.f * GloabalScale * 1.5f, 112.5f / 100.f * GloabalScale * 1.5f, 300.f / 100.f * GloabalScale * 1.5f };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 0.3f, GlobalPosition.z };
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::SPOOL_FLOW_EFFECT, L"SPOOL_APPEAR_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::SPOOL_FLOW1] = FALSE;
		}
		else if (Staging_Timer > 2.f && STAGING_TRIGGER[(INT)DEATH_STAGING::SPOOL_FLOW2]) {
			_vec3 SpoolAppearSca = { 250.f / 100.f * GloabalScale * 1.5f, 112.5f / 100.f * GloabalScale * 1.5f, 300.f / 100.f * GloabalScale * 1.5f };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 0.3f, GlobalPosition.z };
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::SPOOL_FLOW_EFFECT, L"SPOOL_APPEAR_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::SPOOL_FLOW2] = FALSE;
		}
		else if (Staging_Timer > 3.f && STAGING_TRIGGER[(INT)DEATH_STAGING::SPOOL_FLOW3]) {
			_vec3 SpoolAppearSca = { 250.f / 100.f * GloabalScale * 1.5f, 112.5f / 100.f * GloabalScale * 1.5f, 300.f / 100.f * GloabalScale * 1.5f };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 0.3f, GlobalPosition.z };
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::SPOOL_FLOW_EFFECT, L"SPOOL_APPEAR_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::SPOOL_FLOW3] = FALSE;
		}
		else if (Staging_Timer > 4.f && STAGING_TRIGGER[(INT)DEATH_STAGING::SPOOL_FLOW4]) {
			_vec3 SpoolAppearSca = { 250.f / 100.f * GloabalScale * 1.5f, 112.5f / 100.f * GloabalScale * 1.5f, 300.f / 100.f * GloabalScale * 1.5f };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 0.3f, GlobalPosition.z };
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::SPOOL_FLOW_EFFECT, L"SPOOL_APPEAR_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::SPOOL_FLOW4] = FALSE;
		}
		else if (Staging_Timer > 5.f && STAGING_TRIGGER[(INT)DEATH_STAGING::SPOOL_FLOW5]) {
			_vec3 SpoolAppearSca = { 250.f / 100.f * GloabalScale * 1.5f, 112.5f / 100.f * GloabalScale * 1.5f, 300.f / 100.f * GloabalScale * 1.5f };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.2f, GlobalPosition.y + 0.3f, GlobalPosition.z };
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::SPOOL_DISAPPEAR_EFFECT, L"SPOOL_APPEAR_EFFECT", &SpoolAppearPos, SpoolAppearSca, 0.5f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::SPOOL_FLOW5] = FALSE;
		}
		
		if		(Staging_Timer > 3.f  && STAGING_TRIGGER[(INT)DEATH_STAGING::WATER_POPUP1]) {
			Camera->Camera_Shaking(10.f, 1.f);
			_vec3 SpoolAppearSca = { 1.2f * GloabalScale, 1.4f * GloabalScale, 1.4f * GloabalScale};
			_vec3 SpoolAppearPos = { GlobalPosition.x - 2.f, GlobalPosition.y + 2.f, GlobalPosition.z + 3.f};
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::WATER_POPUP_EFFECT, L"WATER_POPUP_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.2f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::WATER_POPUP1] = FALSE;
		}
		else if (Staging_Timer > 3.5f && STAGING_TRIGGER[(INT)DEATH_STAGING::WATER_POPUP2]) {
			Camera->Camera_Shaking(10.f, 1.f);
			_vec3 SpoolAppearSca = { 1.4f * GloabalScale, 1.7f * GloabalScale, 1.7f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x + 1.f, GlobalPosition.y + 1.5f, GlobalPosition.z + 0.5f};
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::WATER_POPUP_EFFECT, L"WATER_POPUP_EFFECT", &SpoolAppearPos, SpoolAppearSca, 0.6f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::WATER_POPUP2] = FALSE;
		}
		else if (Staging_Timer > 4.0f && STAGING_TRIGGER[(INT)DEATH_STAGING::WATER_POPUP3]) {
			Camera->Camera_Shaking(10.f, 1.f);
			_vec3 SpoolAppearSca = { 1.3f * GloabalScale, 1.6f * GloabalScale, 1.6f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x + 1.5f, GlobalPosition.y + 2.f, GlobalPosition.z + 1.f};
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::WATER_POPUP_EFFECT, L"WATER_POPUP_EFFECT", &SpoolAppearPos, SpoolAppearSca, 0.8f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::WATER_POPUP3] = FALSE;
		}
		else if (Staging_Timer > 4.5f && STAGING_TRIGGER[(INT)DEATH_STAGING::WATER_POPUP4]) {
			Camera->Camera_Shaking(10.f, 1.f);
			_vec3 SpoolAppearSca = { 2.f * GloabalScale, 3.f * GloabalScale, 4.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 1.f, GlobalPosition.y + 1.f, GlobalPosition.z + 4.4f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::WATER_POPUP_EFFECT, L"WATER_POPUP_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::WATER_POPUP4] = FALSE;
		}
		if (Staging_Timer > 5.f && STAGING_TRIGGER[(INT)DEATH_STAGING::BIG_FLAME]) {
			Camera->Camera_Shaking(30.f, 15.f);
			_vec3 SpoolAppearSca = { 5.f * GloabalScale, 5.f * GloabalScale, 5.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x - 0.25f, GlobalPosition.y + 1.4f, GlobalPosition.z + 5.f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::FLAME_EFFECT, L"BIG_FLAME_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::BIG_FLAME] = FALSE;
		}
		if (Staging_Timer > 5.f && STAGING_TRIGGER[(INT)DEATH_STAGING::BIG_CIRCLE_FLAME]) {
			_vec3 SpoolAppearSca = { 6.f * GloabalScale, 6.f / 2.f * GloabalScale, 6.f / 2.f * GloabalScale };
			_vec3 SpoolAppearPos = { GlobalPosition.x , GlobalPosition.y + 1.f, GlobalPosition.z - 1.f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::CIRCLE_FLAME_EFFECT, L"BIG_CIRCLE_FLAME_EFFECT", &SpoolAppearPos, SpoolAppearSca, 1.f);
			STAGING_TRIGGER[(INT)DEATH_STAGING::BIG_CIRCLE_FLAME] = FALSE;

			memset(STAGING_TRIGGER, TRUE, sizeof(STAGING_TRIGGER));
			Enable_BossDisappearStaging = FALSE;
			Staging_Timer = 0.f;
		}
	}
}
VOID FinalBoss::Set_StartPos(_vec3 _StartPos) {
	Component_Transform->Set_Pos(Component_Transform->Get_Position()->x + _StartPos.x, 
		Component_Transform->Get_Position()->y + _StartPos.y, Component_Transform->Get_Position()->z + _StartPos.z);
}

VOID FinalBoss::Skill_GroundExplosion(CONST FLOAT& _DT) {
	if		(Enable_GroundExplosion == 0 && Enable_GroundQuadExplosion == 0) return;
	else if (Enable_GroundExplosion == 1) {
		Explosion_Timer += _DT;
		if (Explosion_Timer > 0.5f) {

			_vec3 Scale = { 6.f, 6.f, 6.f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SLAM_GROUND_EXP_EFFECT, L"Ground Explosion", &PlayerPos, Scale, 0.7f);
			dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion")
				->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Rotation(85.f, 0.f, 0.f);

			PlayerPos = { 0.f, 0.f, 0.f };
			Enable_GroundExplosion = FALSE;
			Explosion_Timer = 0.f;
		}
	}
	else if (Enable_GroundExplosion == 2) {
		Explosion_Timer += _DT;
		if (Explosion_Timer > 0.5f) {

			_vec3 Scale = { 6.f, 6.f, 6.f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion", &PlayerPos, Scale, 0.7f);
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
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SLAM_GROUND_EXP_EFFECT, L"Ground Explosion1", &vecvec, Scale, 0.7f);
			dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion1")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
				->Set_Rotation(85.f, 0.f, 0.f);
			EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION1] = FALSE;
		}
		if (Explosion_Timer > 0.7f && EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION2]) {
			vecvec = vecvec * 8.f;
			vecvec += BossBottomPos;
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SLAM_GROUND_EXP_EFFECT, L"Ground Explosion2", &vecvec, Scale, 0.7f);
			dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion2")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
				->Set_Rotation(85.f, 0.f, 0.f);
			EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION2] = FALSE;
		}
		if (Explosion_Timer > 0.9f && EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION3]) {
			vecvec = vecvec * 12.f;
			vecvec += BossBottomPos;
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SLAM_GROUND_EXP_EFFECT, L"Ground Explosion3", &vecvec, Scale, 0.7f);
			dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion3")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
				->Set_Rotation(85.f, 0.f, 0.f);
			EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION3] = FALSE;
		}
		if (Explosion_Timer > 1.1f && EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION4]) {
			vecvec = vecvec * 16.f;
			vecvec += BossBottomPos;
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SLAM_GROUND_EXP_EFFECT, L"Ground Explosion4", &vecvec, Scale, 0.7f);
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
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion1", &vecvec, Scale, 0.7f);
			dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion1")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
				->Set_Rotation(85.f, 0.f, 0.f);
			EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION1] = FALSE;
		}
		if (Explosion_Timer > 0.7f && EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION2]) {
			vecvec = vecvec * 8.f;
			vecvec += BossBottomPos;
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion2", &vecvec, Scale, 0.7f);
			dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion2")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
				->Set_Rotation(85.f, 0.f, 0.f);
			EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION2] = FALSE;
		}
		if (Explosion_Timer > 0.9f && EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION3]) {
			vecvec = vecvec * 12.f;
			vecvec += BossBottomPos;
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion3", &vecvec, Scale, 0.7f);
			dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Ground Explosion3")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
				->Set_Rotation(85.f, 0.f, 0.f);
			EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION3] = FALSE;
		}
		if (Explosion_Timer > 1.1f && EXPLOSION_TRIGGER[(INT)EXPLOSION::METEOR_SLAM_EXPLOSION4]) {
			vecvec = vecvec * 16.f;
			vecvec += BossBottomPos;
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion4", &vecvec, Scale, 0.7f);
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
				PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"Explosion Warning" + to_wstring(IDX), &Pos, Scale, 0.6f);
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
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::METEOR_EFFECT, L"Meteor1", &Pos0, Scale, 0.65f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::METEOR_EFFECT, L"Meteor2", &Pos1, Scale, 0.65f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::METEOR_EFFECT, L"Meteor3", &Pos2, Scale, 0.65f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::METEOR_EFFECT, L"Meteor4", &Pos3, Scale, 0.65f);

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

			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::METEOR_EXP_EFFECT, L"MeteorExp1", &Pos0, Scale, 0.4f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::METEOR_EXP_EFFECT, L"MeteorExp2", &Pos1, Scale, 0.4f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::METEOR_EXP_EFFECT, L"MeteorExp3", &Pos2, Scale, 0.4f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::METEOR_EXP_EFFECT, L"MeteorExp4", &Pos3, Scale, 0.4f);

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
VOID FinalBoss::Skill_RSwingFireBall (CONST FLOAT& _DT) {
	if		(Enable_CreateFireBall == 0) return;
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
			Enable_CreateFireBall = FALSE;
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
		else if (RSwing_Timer > 0.85f && FIREBALL_TRIGGER[(INT)FIREBALL::FIREBALL_POOL]) {
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

			FIREBALL_TRIGGER[(INT)FIREBALL::FIREBALL_POOL] = FALSE;

			RSwing_Timer = 0.f;
			memset(FIREBALL_TRIGGER, TRUE, sizeof(FIREBALL_TRIGGER));
			Enable_CreateFireBall = false;
		}
	}		
}
VOID FinalBoss::Skill_RageUpFireBall (CONST FLOAT& _DT) {
	if (Enable_RageUpFireBall == FALSE)	return;
	if (Enable_RageUpFireBall == TRUE ) {
		RageUp_Timer += _DT;
	
		FLOAT SectorAngle = 20.f;

		if		(RageUp_Timer > 0.05f && FIREBALL_TRIGGER[(INT)FIREBALL::FIREBALL_POOL]) {
			for (INT IDX = 0; IDX < 60; ++IDX) {
				GameObject* Pull = BossFireBall::Create(GRPDEV);
				dynamic_cast<BossFireBall*>(Pull)->Set_Boss(this);
				dynamic_cast<BossFireBall*>(Pull)->Set_BossPosition(Component_Transform);
				dynamic_cast<Transform*>(Pull->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(
					{ Component_Transform->Get_Position()->x, Component_Transform->Get_Position()->y - 0.5f, Component_Transform->Get_Position()->z - 5.f }
				);
				Pull->Set_ObjectTag(TEXT("RageUp_FireBall%d", IDX));
				FireBallObjectPool.push_back(Pull);
			}
			FIREBALL_TRIGGER[(INT)FIREBALL::FIREBALL_POOL] = FALSE;
		}
		else if	(RageUp_Timer > 0.25f && FIREBALL_TRIGGER[(INT)FIREBALL::FIRST_FIREBALL]) {
			for (INT IDX = 0; IDX < 15; ++IDX) {
				wstring temp = L"RAGE_FIREBALL" + to_wstring(Numbering++);
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, FireBallObjectPool[IDX]);
				dynamic_cast<BossFireBall*>(FireBallObjectPool[IDX])->Set_FireBall_Angle(IDX * 24);
			}
			FIREBALL_TRIGGER[(INT)FIREBALL::FIRST_FIREBALL] = FALSE;
		}
		else if (RageUp_Timer > 0.85f && FIREBALL_TRIGGER[(INT)FIREBALL::SECOND_FIREBALL]) {
			for (INT IDX = 0; IDX < 15; ++IDX) {
				wstring temp = L"RAGE_FIREBALL" + to_wstring(Numbering++);
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, FireBallObjectPool[15 + IDX]);
				dynamic_cast<BossFireBall*>(FireBallObjectPool[15 + IDX])->Set_FireBall_Angle(5 + IDX * 24);
			}
			FIREBALL_TRIGGER[(INT)FIREBALL::SECOND_FIREBALL] = FALSE;
		}
		else if (RageUp_Timer > 1.45f && FIREBALL_TRIGGER[(INT)FIREBALL::THIRD_FIREBALL]) {
			for (INT IDX = 0; IDX < 15; ++IDX) {
				wstring temp = L"RAGE_FIREBALL" + to_wstring(Numbering++);
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, FireBallObjectPool[15 * 2 + IDX]);
				dynamic_cast<BossFireBall*>(FireBallObjectPool[15 * 2 + IDX])->Set_FireBall_Angle(10 + IDX * 24);
			}
			FIREBALL_TRIGGER[(INT)FIREBALL::THIRD_FIREBALL] = FALSE;
		}
		else if (RageUp_Timer > 2.05f && FIREBALL_TRIGGER[(INT)FIREBALL::FOURTH_FIREBALL]) {
			for (INT IDX = 0; IDX < 15; ++IDX) {
				wstring temp = L"RAGE_FIREBALL" + to_wstring(Numbering++);
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, FireBallObjectPool[15 * 3 + IDX]);
				dynamic_cast<BossFireBall*>(FireBallObjectPool[15 * 3 + IDX])->Set_FireBall_Angle(15 + IDX * 24);
			}
			
			RageUp_Timer = 0.f;
			memset(FIREBALL_TRIGGER, TRUE, sizeof(FIREBALL_TRIGGER));
			Enable_RageUpFireBall = FALSE;
		}
	}
}
VOID FinalBoss::Skill_SupporterFlame (CONST FLOAT& _DT) {
	if (Enable_SupporterFlame) {
		Supporter_Timer += _DT;
		_vec3 EmblemScale = { 1.f, 1.f, 1.f };
		_vec3 CenterPosition = { Component_Transform->Get_Position()->x - 1.f, Component_Transform->Get_Position()->y - 0.5f, Component_Transform->Get_Position()->z - 3.f};
		
		_vec3 Sup1_Pos = { CenterPosition.x, CenterPosition.y + 0.1f, CenterPosition.z };
		_vec3 Sup2_pos = { CenterPosition.x - 1.2f, CenterPosition.y + 1.9f * sinf(D3DXToRadian(10)) + 0.1f, CenterPosition.z + 1.9f * cosf(D3DXToRadian(10)) };
		_vec3 Sup3_pos = { CenterPosition.x + 1.2f, CenterPosition.y + 1.9f * sinf(D3DXToRadian(10)) + 0.1f, CenterPosition.z + 1.9f * cosf(D3DXToRadian(10)) };
		_vec3 Sup4_pos = { CenterPosition.x - 1.2f, CenterPosition.y - 1.9f * sinf(D3DXToRadian(10)) + 0.1f, CenterPosition.z - 1.9f * cosf(D3DXToRadian(10)) };
		_vec3 Sup5_pos = { CenterPosition.x + 1.2f, CenterPosition.y - 1.9f * sinf(D3DXToRadian(10)) + 0.1f, CenterPosition.z - 1.9f * cosf(D3DXToRadian(10)) };
		_vec3 Sup6_pos = { CenterPosition.x + 2.3f, CenterPosition.y + 0.1f, CenterPosition.z };
		_vec3 Sup7_pos = { CenterPosition.x - 2.3f, CenterPosition.y + 0.1f, CenterPosition.z };

		if (Supporter_Timer > 0.1f && SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP_POOLING]) {
			for (INT IDX = 0; IDX < 7; ++IDX) {
				GameObject* SUP = Supporter::Create(GRPDEV);
				dynamic_cast<Supporter*>(SUP)->Set_SupporterType(2);
				dynamic_cast<Transform*>(SUP->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Scale(2.f, 2.f, 2.f);
				SupporterObjectPool.push_back(SUP);
			}
			SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP_POOLING] = FALSE;
		}
		// 중앙
		else if (Supporter_Timer > 0.75f && SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP1_SPAWN]) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SUPPORTER_APPEAR_EFFECT, L"SUP1_SPAWN", &Sup1_Pos, EmblemScale, 1.f);
			SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP1_SPAWN] = FALSE;
		}
		// 좌 상단
		else if (Supporter_Timer > 1.0f && SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP2_SPAWN]) {
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<Supporter>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, SupporterObjectPool[0]);
			dynamic_cast<Supporter*>(SupporterObjectPool[0])->Set_ScaleInc(TRUE);
			dynamic_cast<Supporter*>(SupporterObjectPool[0])->Set_DirectionVec({ 0.f, 0.f, 0.f });
			Sup1_Pos.y -= 0.1f;
			dynamic_cast<Transform*>(SupporterObjectPool[0]->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(Sup1_Pos);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SUPPORTER_APPEAR_EFFECT, L"SUP2_SPAWN", &Sup2_pos, EmblemScale, 1.f);

			SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP2_SPAWN] = FALSE;
		}
		// 우 상단
		else if (Supporter_Timer > 1.25f && SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP3_SPAWN]) {
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<Supporter>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, SupporterObjectPool[1]);
			dynamic_cast<Supporter*>(SupporterObjectPool[1])->Set_ScaleInc(TRUE);
			dynamic_cast<Supporter*>(SupporterObjectPool[1])->Set_DirectionVec({ Sup2_pos.x - CenterPosition.x , 0.f, Sup2_pos.z - CenterPosition.z });
			Sup2_pos.y -= 0.1f;
			dynamic_cast<Transform*>(SupporterObjectPool[1]->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(Sup2_pos);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SUPPORTER_APPEAR_EFFECT, L"SUP3_SPAWN", &Sup3_pos, EmblemScale, 1.f);

			SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP3_SPAWN] = FALSE;
		}
		// 좌 하단
		else if (Supporter_Timer > 1.5f && SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP4_SPAWN]) {
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<Supporter>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, SupporterObjectPool[2]);
			dynamic_cast<Supporter*>(SupporterObjectPool[2])->Set_ScaleInc(TRUE);
			dynamic_cast<Supporter*>(SupporterObjectPool[2])->Set_DirectionVec({ Sup3_pos.x - CenterPosition.x , 0.f, Sup3_pos.z - CenterPosition.z });
			Sup3_pos.y -= 0.1f;
			dynamic_cast<Transform*>(SupporterObjectPool[2]->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(Sup3_pos);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SUPPORTER_APPEAR_EFFECT, L"SUP4_SPAWN", &Sup4_pos, EmblemScale, 1.f);

			SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP4_SPAWN] = FALSE;
		}
		// 우 하단
		else if (Supporter_Timer > 1.75f && SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP5_SPAWN]) {
			PLAY_BOSS_FRONTEFFECT(BOSS_EFFECT::SUPPORTER_STAY_EFFECT, L"SUP1_STAY", &Sup1_Pos, EmblemScale, 1.75f);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<Supporter>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, SupporterObjectPool[3]);
			dynamic_cast<Supporter*>(SupporterObjectPool[3])->Set_ScaleInc(TRUE);
			dynamic_cast<Supporter*>(SupporterObjectPool[3])->Set_DirectionVec({ Sup4_pos.x - CenterPosition.x , 0.f, Sup4_pos.z - CenterPosition.z });
			Sup4_pos.y -= 0.1f;
			dynamic_cast<Transform*>(SupporterObjectPool[3]->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(Sup4_pos);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SUPPORTER_APPEAR_EFFECT, L"SUP5_SPAWN", &Sup5_pos, EmblemScale, 1.f);

			SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP5_SPAWN] = FALSE;
		}
		// 중앙 우측
		else if (Supporter_Timer > 2.0f && SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP6_SPAWN]) {
			PLAY_BOSS_FRONTEFFECT(BOSS_EFFECT::SUPPORTER_STAY_EFFECT, L"SUP2_STAY", &Sup2_pos, EmblemScale, 1.5f);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<Supporter>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, SupporterObjectPool[4]);
			dynamic_cast<Supporter*>(SupporterObjectPool[4])->Set_ScaleInc(TRUE);
			dynamic_cast<Supporter*>(SupporterObjectPool[4])->Set_DirectionVec({ Sup5_pos.x - CenterPosition.x , 0.f, Sup5_pos.z - CenterPosition.z });
			Sup5_pos.y -= 0.1f;
			dynamic_cast<Transform*>(SupporterObjectPool[4]->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(Sup5_pos);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SUPPORTER_APPEAR_EFFECT, L"SUP6_SPAWN", &Sup6_pos, EmblemScale, 1.f);

			SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP6_SPAWN] = FALSE;

			Animation_CurrentIndex = 0;
			Animation_TexList = &Animation_Rage_Stand_TexList;
			Animation_FrameCount = ANIMATION_RAGE_STAND_FRAMECOUNT;
		}
		// 중앙 좌측
		else if (Supporter_Timer > 2.25f && SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP7_SPAWN]) {
			PLAY_BOSS_FRONTEFFECT(BOSS_EFFECT::SUPPORTER_STAY_EFFECT, L"SUP3_STAY", &Sup3_pos, EmblemScale, 1.25f);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<Supporter>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, SupporterObjectPool[5]);
			dynamic_cast<Supporter*>(SupporterObjectPool[5])->Set_ScaleInc(TRUE);
			dynamic_cast<Supporter*>(SupporterObjectPool[5])->Set_DirectionVec({ 1.f, 0.f, 0.f });
			Sup6_pos.y -= 0.1f;
			dynamic_cast<Transform*>(SupporterObjectPool[5]->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(Sup6_pos);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SUPPORTER_APPEAR_EFFECT, L"SUP7_SPAWN", &Sup7_pos, EmblemScale, 1.f);

			SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP7_SPAWN] = FALSE;
		}
		else if (Supporter_Timer > 2.5f && SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP_SPAWN_END]) {
			PLAY_BOSS_FRONTEFFECT(BOSS_EFFECT::SUPPORTER_STAY_EFFECT, L"SUP4_STAY", &Sup4_pos, EmblemScale, 1.f);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<Supporter>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, SupporterObjectPool[6]);
			dynamic_cast<Supporter*>(SupporterObjectPool[6])->Set_ScaleInc(TRUE);
			dynamic_cast<Supporter*>(SupporterObjectPool[6])->Set_DirectionVec({ -1.f, 0.f, 0.f });
			Sup7_pos.y -= 0.1f;
			dynamic_cast<Transform*>(SupporterObjectPool[6]->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(Sup7_pos);

			SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP_SPAWN_END] = FALSE;
		}
		else if (Supporter_Timer > 2.75f && SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP_SPAWNFADEIN1]) {
			PLAY_BOSS_FRONTEFFECT(BOSS_EFFECT::SUPPORTER_STAY_EFFECT, L"SUP5_STAY", &Sup5_pos, EmblemScale, 0.75f);
			SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP_SPAWNFADEIN1] = FALSE;
		}
		else if (Supporter_Timer > 3.f && SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP_SPAWNFADEIN2]) {
			PLAY_BOSS_FRONTEFFECT(BOSS_EFFECT::SUPPORTER_STAY_EFFECT, L"SUP6_STAY", &Sup6_pos, EmblemScale, 0.5f);
			SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP_SPAWNFADEIN2] = FALSE;
		}
		else if (Supporter_Timer > 3.25f && SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP_SPAWNFADEIN3]) {
			PLAY_BOSS_FRONTEFFECT(BOSS_EFFECT::SUPPORTER_STAY_EFFECT, L"SUP7_STAY", &Sup7_pos, EmblemScale, 0.25f);
			SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP_SPAWNFADEIN3] = FALSE;
			}
		else if (Supporter_Timer > 3.5f && Supporter_Timer <= 4.5f) {
			if (Supporter_Timer > 3.5f && SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP_MOVE]) {
				for (INT IDX = 0; IDX < SupporterObjectPool.size(); ++IDX) {
					dynamic_cast<Supporter*>(SupporterObjectPool[IDX])->Set_RageMove(TRUE);
					wstring SUPTAG = L"SUP" + to_wstring(IDX + 1) + L"_STAY";
					EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, SUPTAG.c_str())->Set_ObjectDead(TRUE);
					SUPTAG = L"SUP" + to_wstring(IDX + 1) + L"_Disappear";
					PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SUPPORTER_DISAPPEAR_EFFECT, SUPTAG, dynamic_cast<Transform*>(SupporterObjectPool[IDX]->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position(), EmblemScale, 1.5f)
				}
				SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP_MOVE] = FALSE;
			}
			for (INT IDX = 0; IDX < SupporterObjectPool.size(); ++IDX) {
				GameObject* Effect = EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"SUP" + to_wstring(IDX + 1) + L"_Disappear");
				if (Effect != nullptr) {
					_vec3 EffectPos = *dynamic_cast<Transform*>(SupporterObjectPool[IDX]->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position();
					dynamic_cast<Transform*>(Effect->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(EffectPos);
				}
			}
		}
		else if (Supporter_Timer > 3.f && EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"SUP7_Disappear") != nullptr) {
			for (INT IDX = 0; IDX < SupporterObjectPool.size(); ++IDX) {
				wstring SUPTAG = L"SUP" + to_wstring(IDX + 1) + L"_Disappear";
				GameObject* Effect = EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, SUPTAG);
				dynamic_cast<Transform*>(Effect->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
					->Set_Pos(*dynamic_cast<Transform*>(SupporterObjectPool[IDX]->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position());
			}
		}
		else if (Supporter_Timer > 10.f && SUPPORTER_TRIGGER[(INT)SUPPORTER::SUP_DISAPPEAR]) {
			memset(SUPPORTER_TRIGGER, TRUE, sizeof(SUPPORTER_TRIGGER));
			for (auto& i : SupporterObjectPool)
				Safe_Release(i);
			SupporterObjectPool.clear();
			Supporter_Timer = 0.f;
			Enable_SupporterFlame = FALSE;
		}
	}
}
VOID FinalBoss::Skill_ExplosionRush	(CONST FLOAT& _DT) {
	if (Enable_ExplosionRush) {
		Rush_Timer += _DT;
		if		(ERUSH_TRIGGER[(INT)RUSH::RUSH_POOL]) {
			FireBallObjectPool.clear();
			for (INT IDX = 0; IDX < 60; ++IDX) {
				GameObject* Pull = BossFireBall::Create(GRPDEV);
				dynamic_cast<BossFireBall*>(Pull)->Set_Boss(this);
				dynamic_cast<BossFireBall*>(Pull)->Set_BossPosition(Component_Transform);
				dynamic_cast<BossFireBall*>(Pull)->Set_FireBall_Angle((24 * IDX) % 360 + 15 * (IDX / 15));
				dynamic_cast<Transform*>(Pull->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(
					{ Component_Transform->Get_Position()->x, Component_Transform->Get_Position()->y - 0.5f, Component_Transform->Get_Position()->z - 5.f }
				);
				Pull->Set_ObjectTag(TEXT("ERush_FireBall%d", IDX));
				FireBallObjectPool.push_back(Pull);
			}
			Animation_Interval = 0.35f;
			ERUSH_TRIGGER[(INT)RUSH::RUSH_POOL] = FALSE;
		}
		else if (Rush_Timer > 0.25f && ERUSH_TRIGGER[(INT)RUSH::RUSH_FIREBALL1]) {
			for (INT IDX = 0; IDX < 15; ++IDX) 
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, FireBallObjectPool[IDX]);
			
			ERUSH_TRIGGER[(INT)RUSH::RUSH_FIREBALL1] = FALSE;
		}
		else if (Rush_Timer > 0.85f && ERUSH_TRIGGER[(INT)RUSH::RUSH_FIREBALL2]) {
			for (INT IDX = 15; IDX < 30; ++IDX)
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, FireBallObjectPool[IDX]);
			ERUSH_TRIGGER[(INT)RUSH::RUSH_FIREBALL2] = FALSE;
		}
		else if (Rush_Timer > 1.45f && ERUSH_TRIGGER[(INT)RUSH::RUSH_FIREBALL3]) {
			for (INT IDX = 30; IDX < 45; ++IDX)
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, FireBallObjectPool[IDX]);
			ERUSH_TRIGGER[(INT)RUSH::RUSH_FIREBALL3] = FALSE;
		}
		else if (Rush_Timer > 2.05f && ERUSH_TRIGGER[(INT)RUSH::RUSH_FIREBALL4]) {
			for (INT IDX = 45; IDX < 60; ++IDX)
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, FireBallObjectPool[IDX]);
			ERUSH_TRIGGER[(INT)RUSH::RUSH_FIREBALL4] = FALSE;
		}

		if		(Rush_Timer > 0.25f && ERUSH_TRIGGER[(INT)RUSH::RUSH_EXPLOSION1]) {
			Enable_BBTrap = TRUE;

			ERUSH_TRIGGER[(INT)RUSH::RUSH_EXPLOSION1] = FALSE;
		}
		else if (Rush_Timer > 0.85f && ERUSH_TRIGGER[(INT)RUSH::RUSH_EXPLOSION2]) {
			
			ERUSH_TRIGGER[(INT)RUSH::RUSH_EXPLOSION2] = FALSE;
		}
		else if (Rush_Timer > 1.45f && ERUSH_TRIGGER[(INT)RUSH::RUSH_EXPLOSION3]) {

			ERUSH_TRIGGER[(INT)RUSH::RUSH_EXPLOSION3] = FALSE;
		}	
		else if (Rush_Timer > 2.05f && ERUSH_TRIGGER[(INT)RUSH::RUSH_EXPLOSION4]) {

			ERUSH_TRIGGER[(INT)RUSH::RUSH_EXPLOSION4] = FALSE;
		}

		if		(Rush_Timer > 2.05f && ERUSH_TRIGGER[(INT)RUSH::RUSH_FLAME1]) {

			ERUSH_TRIGGER[(INT)RUSH::RUSH_FLAME1] = FALSE;
		}
		else if (Rush_Timer > 2.05f && ERUSH_TRIGGER[(INT)RUSH::RUSH_FLAME2]) {

			ERUSH_TRIGGER[(INT)RUSH::RUSH_FLAME2] = FALSE;
		}
		else if (Rush_Timer > 2.05f && ERUSH_TRIGGER[(INT)RUSH::RUSH_FLAME3]) {

			ERUSH_TRIGGER[(INT)RUSH::RUSH_FLAME3] = FALSE;
		}
		else if (Rush_Timer > 5.05f && ERUSH_TRIGGER[(INT)RUSH::RUSH_FLAME4]) {

			ERUSH_TRIGGER[(INT)RUSH::RUSH_FLAME4] = FALSE;

			Rush_Timer = 0.f;
			Enable_ExplosionRush = FALSE;
			memset(ERUSH_TRIGGER, TRUE, sizeof(ERUSH_TRIGGER));
			Enable_BBTrap = FALSE;
			FireBallObjectPool.clear();
		}
	}
}

VOID FinalBoss::BoobieTrap(CONST FLOAT& _DT) {
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

		RageUp_Timer += _DT;
		if		(RageUp_Timer > 0.1f && BBTrap[0]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR000N0", &DGR000[0], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR090N0",&DGR090[0], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR180N0", &DGR180[0], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR270N0", &DGR270[0], Scale, 0.3f);

			BBTrap[0] = FALSE;
		}
		else if (RageUp_Timer > 0.2f && BBTrap[1]) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR000N1", &DGR000[1], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR090N1", &DGR090[1], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR180N1", &DGR180[1], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT,  L"DGR270N1", &DGR270[1], Scale, 0.3f);

			BBTrap[1] = FALSE;
		}
		else if (RageUp_Timer > 0.3f && BBTrap[2]) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR000N2",  &DGR000[2], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR090N2",  &DGR090[2], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR180N2",  &DGR180[2], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT,  L"DGR270N2",  &DGR270[2], Scale, 0.3f);

			BBTrap[2] = FALSE;
		}
		else if (RageUp_Timer > 0.4f && BBTrap[3]) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR000N3", &DGR000[3], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR090N3", &DGR090[3], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR180N3", &DGR180[3], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT,  L"DGR270N3", &DGR270[3], Scale, 0.3f);

			BBTrap[3] = FALSE;
		}
		else if (RageUp_Timer > 0.5f && BBTrap[4]) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR000N4", &DGR000[4], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR090N4", &DGR090[4], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR180N4", &DGR180[4], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT,  L"DGR270N4", &DGR270[4], Scale, 0.3f);

			BBTrap[4] = FALSE;
		}

		else if (RageUp_Timer > 0.7f && BBTrap[5]) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR330N0", &DGR330[0], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR060N0", &DGR060[0], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR150N0", &DGR150[0], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE (BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR240N0", &DGR240[0], Scale, 0.3f);

			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion000N0", &DGR000[0], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion090N0", &DGR090[0], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion180N0", &DGR180[0], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion270N0", &DGR270[0], EXPScale, 0.7f);

			BBTrap[5] = FALSE;
		}
		else if (RageUp_Timer > 0.8f && BBTrap[6]) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR330N1", &DGR330[1], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR060N1", &DGR060[1], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR150N1", &DGR150[1], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE (BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR240N1", &DGR240[1], Scale, 0.3f);

			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion000N1", &DGR000[1], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion090N1", &DGR090[1], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion180N1", &DGR180[1], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion270N1", &DGR270[1], EXPScale, 0.7f);
			BBTrap[6] = FALSE;
		}
		else if (RageUp_Timer > 0.9f && BBTrap[7]) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR330N2", &DGR330[2], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR060N2", &DGR060[2], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR150N2", &DGR150[2], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE (BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR240N2", &DGR240[2], Scale, 0.3f);

			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion000N2", &DGR000[2], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion090N2", &DGR090[2], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion180N2", &DGR180[2], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion270N2", &DGR270[2], EXPScale, 0.7f);
			BBTrap[7] = FALSE;
		}
		else if (RageUp_Timer > 1.0f && BBTrap[8]) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR330N3", &DGR330[3], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR060N3", &DGR060[3], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR150N3", &DGR150[3], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE (BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR240N3", &DGR240[3], Scale, 0.3f);

			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion000N3", &DGR000[3], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion090N3", &DGR090[3], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion180N3", &DGR180[3], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion270N3", &DGR270[3], EXPScale, 0.7f);
			BBTrap[8] = FALSE;
		}
		else if (RageUp_Timer > 1.1f && BBTrap[9]) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR330N4", &DGR330[4], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR060N4", &DGR060[4], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR150N4", &DGR150[4], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE (BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR240N4", &DGR240[4], Scale, 0.3f);

			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion000N4", &DGR000[4], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion090N4", &DGR090[4], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion180N4", &DGR180[4], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion270N4", &DGR270[4], EXPScale, 0.7f);
			BBTrap[9] = FALSE;
		}
	
		else if (RageUp_Timer > 1.2f && BBTrap[10]) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR330N0", &DGR300[0], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR060N0", &DGR030[0], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR150N0", &DGR120[0], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE (BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR240N0", &DGR210[0], Scale, 0.3f);

			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion330N0", &DGR330[0], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion060N0", &DGR060[0], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion150N0", &DGR150[0], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion240N0", &DGR240[0], EXPScale, 0.7f);
			BBTrap[10] = FALSE;
		}
		else if (RageUp_Timer > 1.3f && BBTrap[11]) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR330N1", &DGR300[1], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR060N1", &DGR030[1], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR150N1", &DGR120[1], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE (BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR240N1", &DGR210[1], Scale, 0.3f);

			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion330N1", &DGR330[1], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion060N1", &DGR060[1], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion150N1", &DGR150[1], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion240N1", &DGR240[1], EXPScale, 0.7f);
			BBTrap[11] = FALSE;
		}
		else if (RageUp_Timer > 1.4f && BBTrap[12]) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR330N2", &DGR300[2], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR060N2", &DGR030[2], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR150N2", &DGR120[2], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE (BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR240N2", &DGR210[2], Scale, 0.3f);

			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion330N2", &DGR330[2], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion060N2", &DGR060[2], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion150N2", &DGR150[2], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion240N2", &DGR240[2], EXPScale, 0.7f);
			BBTrap[12] = FALSE;
		}
		else if (RageUp_Timer > 1.5f && BBTrap[13]) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR330N3", &DGR300[3], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR060N3", &DGR030[3], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR150N3", &DGR120[3], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR240N3", &DGR210[3], Scale, 0.3f);

			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion330N3", &DGR330[3], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion060N3", &DGR060[3], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion150N3", &DGR150[3], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion240N3", &DGR240[3], EXPScale, 0.7f);
			BBTrap[13] = FALSE;
		}
		else if (RageUp_Timer > 1.6f && BBTrap[14]) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR330N4", &DGR300[4], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR060N4", &DGR030[4], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR150N4", &DGR120[4], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE (BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR240N4", &DGR210[4], Scale, 0.3f);
			
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion330N4", &DGR330[4], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion060N4", &DGR060[4], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion150N4", &DGR150[4], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion240N4", &DGR240[4], EXPScale, 0.7f);
			BBTrap[14] = FALSE;
		}
	
		else if (RageUp_Timer > 1.7f && BBTrap[15]) {
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR000N0", &DGR000[0], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT,L"DGR090N0",&DGR090[0], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR180N0", &DGR180[0], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR270N0", &DGR270[0], Scale, 0.3f);

			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion300N0", &DGR300[0], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion030N0", &DGR030[0], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion120N0", &DGR120[0], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion210N0", &DGR210[0], EXPScale, 0.7f);
			BBTrap[15] = FALSE;
		}
		else if (RageUp_Timer > 1.8f && BBTrap[16]) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR000N1", &DGR000[1], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR090N1", &DGR090[1], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR180N1", &DGR180[1], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR270N1", &DGR270[1], Scale, 0.3f);

			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion300N1", &DGR300[1], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion030N1", &DGR030[1], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion120N1", &DGR120[1], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion210N1", &DGR210[1], EXPScale, 0.7f);
			BBTrap[16] = FALSE;
		}
		else if (RageUp_Timer > 1.9f && BBTrap[17]) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR000N2", &DGR000[2], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR090N2", &DGR090[2], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR180N2", &DGR180[2], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR270N2", &DGR270[2], Scale, 0.3f);

			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion300N2", &DGR300[2], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion030N2", &DGR030[2], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion120N2", &DGR120[2], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion210N2", &DGR210[2], EXPScale, 0.7f);
			BBTrap[17] = FALSE;
		}
		else if (RageUp_Timer > 2.0f && BBTrap[18]) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR000N3", &DGR000[3], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR090N3", &DGR090[3], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR180N3", &DGR180[3], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR270N3", &DGR270[3], Scale, 0.3f);

			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion300N3", &DGR300[3], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion030N3", &DGR030[3], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion120N3", &DGR120[3], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion210N3", &DGR210[3], EXPScale, 0.7f);
			BBTrap[18] = FALSE;
		}
		else if (RageUp_Timer > 2.1f && BBTrap[19]) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR000N4", &DGR000[4], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR090N4", &DGR090[4], Scale, 0.3f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"DGR180N4", &DGR180[4], Scale, 0.3f);
			PLAY_BOSS_BACKEFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT,  L"DGR270N4", &DGR270[4], Scale, 0.3f);

			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion300N4", &DGR300[4], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion030N4", &DGR030[4], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion120N4", &DGR120[4], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion210N4", &DGR210[4], EXPScale, 0.7f);
			BBTrap[19] = FALSE;
		}
	
		else if (RageUp_Timer > 2.2f && BBTrap[20]) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion000N0L", &DGR000[0], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion090N0L", &DGR090[0], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion180N0L", &DGR180[0], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion270N0L", &DGR270[0], EXPScale, 0.7f);
			BBTrap[20] = FALSE;
		}
		else if (RageUp_Timer > 2.3f && BBTrap[21]) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion000N1L", &DGR000[1], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion090N1L", &DGR090[1], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion180N1L", &DGR180[1], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion270N1L", &DGR270[1], EXPScale, 0.7f);
			BBTrap[21] = FALSE;
		}
		else if (RageUp_Timer > 2.4f && BBTrap[22]) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion000N2L", &DGR000[2], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion090N2L", &DGR090[2], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion180N2L", &DGR180[2], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion270N2L", &DGR270[2], EXPScale, 0.7f);
			BBTrap[22] = FALSE;
		}
		else if (RageUp_Timer > 2.5f && BBTrap[23]) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion000N3L", &DGR000[3], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion090N3L", &DGR090[3], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion180N3L", &DGR180[3], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion270N3L", &DGR270[3], EXPScale, 0.7f);
			BBTrap[23] = FALSE;
		}
		else if (RageUp_Timer > 2.6f && BBTrap[24]) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion000N4L", &DGR000[4], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion090N4L", &DGR090[4], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion180N4L", &DGR180[4], EXPScale, 0.7f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_SLAM_GROUND_EXP_EFFECT, L"Ground Explosion270N4L", &DGR270[4], EXPScale, 0.7f);
			memset(BBTrap, TRUE, sizeof(BBTrap));
			Enable_BBTrap = FALSE;
			RageUp_Timer = 0.f;
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
	for (auto& i : SupporterObjectPool)
		Safe_Release(i);
	for (auto& i : FireBallObjectPool)
		Safe_Release(i);
	
	GameObject::Free();
}