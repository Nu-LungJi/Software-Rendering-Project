#include "../Include/PCH.h"
#include "StateMachine.h"

IMPLEMENT_SINGLETON(AppearState)
IMPLEMENT_SINGLETON(DeadState)
IMPLEMENT_SINGLETON(RageUpState)
IMPLEMENT_SINGLETON(RSwingState)
IMPLEMENT_SINGLETON(FSwingState)
IMPLEMENT_SINGLETON(NormalSlamState)
IMPLEMENT_SINGLETON(MeteorSlamState)
IMPLEMENT_SINGLETON(StandState)

StateMachine:: StateMachine(LPDIRECT3DDEVICE9 _GRPDEV)	:	GameObject(_GRPDEV)	{}
StateMachine:: StateMachine(CONST GameObject& _RHS)		:	GameObject(_RHS)	{}
StateMachine::~StateMachine()													{}

INT			  StateMachine::Update_GameObject(const _float& _DT){
	CurrentState->FSM_StateUpdate(Owner);
	return 0;
}
VOID		  StateMachine::FSM_StateChange(State* _State) {
	if (CurrentState != nullptr) {
		CurrentState->FSM_StateExit(Owner);
		PreviousState = CurrentState;
	}
	for (INT IDX = 0; IDX < StateHistory.size(); ++IDX) {
		if (StateHistory[IDX] == _State) 
			break;
		else if (IDX == StateHistory.size() - 1) 
			StateHistory.push_back(_State);
	}

	CurrentState = _State;

	CurrentState->FSM_StateEnter(Owner);
}
StateMachine* StateMachine::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	StateMachine* FSM = new StateMachine(_GRPDEV);
	if (FAILED(FSM->Ready_GameObject())) {
		MSG_BOX("Cannot Create StateMachine.");
		Safe_Release(FSM);
		return nullptr;
	}
	return FSM;
}
VOID		  StateMachine::Free() {
	for (auto& ST : StateHistory)
		Safe_Delete(ST);
	GameObject::Free();
}

VOID AppearState::FSM_StateEnter(GameObject* _Owner)	{}
VOID AppearState::FSM_StateUpdate(GameObject* _Owner)	{}
VOID AppearState::FSM_StateExit(GameObject* _Owner)		{}

VOID DeadState	::FSM_StateEnter(GameObject* _Owner)	{}
VOID DeadState	::FSM_StateUpdate(GameObject* _Owner)	{}
VOID DeadState	::FSM_StateExit(GameObject* _Owner)		{}

VOID RageUpState::FSM_StateEnter(GameObject* _Owner)	{}
VOID RageUpState::FSM_StateUpdate(GameObject* _Owner)	{
	if (dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_CurrentIndex() == 1 && dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_PreviousIndex() != 1) {
		LPDIRECT3DDEVICE9 GRPDEV = GraphicDevice::GetInstance()->Get_Device();
		dynamic_cast<FinalBoss*>(_Owner)->Set_Animation_Interval(0.07f);
		float Circlepow = 7.f;
		_vec3 CircleScale = { 1.f * Circlepow, 0.7f * Circlepow, 0.7f * Circlepow };
		_vec3 CirclePos = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x ,
					  (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y - 2.553f,
					  (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z - 5.f
		};
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::FSWING_CIRCLE_EFFET, L"FSwing Circle Effect", &CirclePos, CircleScale, 0.4f);
	}
}
VOID RageUpState::FSM_StateExit(GameObject* _Owner)		{}

VOID RSwingState::FSM_StateEnter(GameObject* _Owner)	{}
VOID RSwingState::FSM_StateUpdate(GameObject* _Owner)	{
	// RSWING - PUNCH FLAME EFFECT
	if (dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_CurrentIndex() == 5 && dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_PreviousIndex() != 5) {
		LPDIRECT3DDEVICE9 GRPDEV = GraphicDevice::GetInstance()->Get_Device();
		float pow = 6.f;
		_vec3 Scale = { 1.f * pow, 0.7f * pow, 0.7f * pow };
		_vec3 Pos = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x - 1.5f,
					  (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y,
					  (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z - 4.f,
		};
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::RSWING_EFFECT, L"Punch Flame Effect", &Pos, Scale, 0.35f);
	}
	// RSWING - CREATE PROJECTILE 
	if (dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_CurrentIndex() == 7 && dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_PreviousIndex() != 7) {
		LPDIRECT3DDEVICE9 GRPDEV = GraphicDevice::GetInstance()->Get_Device();
		_vec3 Scale = { 2.4f / 2.f, 1.f/ 2.f, 1.f / 2.f };
		PLAY_BOSS_EFFECT(BOSS_EFFECT::RSWING_PROJ_EFFECT, L"Flame Projectile" ,dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position(),
			Scale, 0.2f);
	}
}
VOID RSwingState::FSM_StateExit(GameObject* _Owner)		{}

VOID FSwingState::FSM_StateEnter(GameObject* _Owner)	{}
VOID FSwingState::FSM_StateUpdate(GameObject* _Owner)	{
	// FSWING - ELECTRIC
	if (dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_CurrentIndex() == 0 && dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_PreviousIndex() != 0) {
		LPDIRECT3DDEVICE9 GRPDEV = GraphicDevice::GetInstance()->Get_Device();
		dynamic_cast<FinalBoss*>(_Owner)->Set_Animation_Interval(0.12f);
		float pow = 3.f;
		_vec3 Scale = { 1.f * pow, 1.f * pow, 1.f * pow };
		_vec3 PosL = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x	- 6.5f,
						 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y	,
						 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z	- 1.f
		};
		_vec3 PosR = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x	- 3.f,
						 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y  ,
						 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z	- 1.f
		};

		EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::BOSS, BossEffect::Create(GRPDEV, L"FSwing Charge ElectricL", BOSS_EFFECT::FSWING_CHARGE_ELECTRIC_EFFECT,
			&PosL, FALSE, 0.6f, Scale, {0.f, 30.f, 0.f}));
		EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::BOSS, BossEffect::Create(GRPDEV, L"FSwing Charge ElectricR", BOSS_EFFECT::FSWING_CHARGE_ELECTRIC_EFFECT,
			&PosR, FALSE, 0.6f, Scale, { 0.f, 100.f, 0.f }));
	}
	// FSWING - CHARGE EXPLOSION
	else if (dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_CurrentIndex() == 3 && dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_PreviousIndex() != 3) {
		LPDIRECT3DDEVICE9 GRPDEV = GraphicDevice::GetInstance()->Get_Device();
		float pow = 6.f;
		_vec3 Scale = { 1.f * pow, 1.f * pow, 1.f * pow };
		_vec3 Pos = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x		- 4.5f,
						 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y	,
						 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z	- 1.f
		};
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::FSWING_CHARGE_EFFECT,		  L"FSwing Charge", &Pos, Scale, 0.4f);
	}
	// FSWING - CIRCLE & ELECTRIC
	else if (dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_CurrentIndex() == 5 && dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_PreviousIndex() != 5) {
		LPDIRECT3DDEVICE9 GRPDEV = GraphicDevice::GetInstance()->Get_Device();
		dynamic_cast<FinalBoss*>(_Owner)->Set_Animation_Interval(0.07f);
		float Circlepow = 10.f;
		_vec3 CircleScale = { 1.f * Circlepow, 0.7f * Circlepow, 0.7f * Circlepow };
		_vec3 CirclePos = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x ,
					  (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y - 1.f,
					  (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z - 5.5f
		};
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::FSWING_CIRCLE_EFFET, L"FSwing Circle Effect", &CirclePos, CircleScale, 0.4f);
		dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"FSwing Circle Effect")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
			->Set_Rotation(85.f, 0.f, 0.f);

		float Elecpow = 6.f;
		_vec3 ElecScale = { 1.f * Elecpow, 1.f * Elecpow, 1.f * Elecpow };
		_vec3 ElecPos = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x ,
					  (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y - 0.95f,
					  (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z - 5.f
		};
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::FSWING_ELECTRIC_EFFECT, L"FSwing Electric Effect", &ElecPos, ElecScale, 0.5f);
		dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"FSwing Electric Effect")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
			->Set_Rotation(85.f, 0.f, 0.f);
	}
	// FSWING - EXPLOSION
	else if (dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_CurrentIndex() == 6 && dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_PreviousIndex() != 6) {
		LPDIRECT3DDEVICE9 GRPDEV = GraphicDevice::GetInstance()->Get_Device();
		float Firstpow = 9.f;
		_vec3 FirstScale = { 1.f * Firstpow, 1.f * Firstpow, 1.f * Firstpow };
		_vec3 FirstPos = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x - 4.5f,
						 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y + 0.1003f,
						 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z - 4.f
		};
		float Secondpow = 7.f;
		_vec3 SecondScale = { 1.f * Secondpow, 1.f * Secondpow, 1.f * Secondpow };
		_vec3 SecondPos = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x - 1.5f,
						 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y + 0.1004f,
						 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z - 7.f
		};
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::FSWING_EXP_EFFECT, L"FSwing EXP", &FirstPos, FirstScale, 0.4f);
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::FSWING_EXP_EFFECT, L"FSwing EXP", &SecondPos, SecondScale, 0.7f);
	}
	// FSWING - EXPLOSION
	else if (dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_CurrentIndex() == 7 && dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_PreviousIndex() != 7) {
		LPDIRECT3DDEVICE9 GRPDEV = GraphicDevice::GetInstance()->Get_Device();
		float Firstpow = 3.f;
		_vec3 FirstScale = { 1.f * Firstpow, 1.f * Firstpow, 1.f * Firstpow };
		_vec3 FirstPos = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x - 2.5f,
						 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y + 0.1005f,
						 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z - 7.f
		};
		float Secondpow = 2.f;
		_vec3 SecondScale = { 1.f * Secondpow, 1.f * Secondpow, 1.f * Secondpow };
		_vec3 SecondPos = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x + 1.5f,
						 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y + 0.1006f,
						 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z - 6.f
		};
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::FSWING_EXP_EFFECT, L"FSwing EXP", &FirstPos, FirstScale, 0.3f);
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::FSWING_EXP_EFFECT, L"FSwing EXP", &SecondPos, SecondScale, 0.5f);
	}
	// FSWING - EXPLOSION
	else if (dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_CurrentIndex() == 8 && dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_PreviousIndex() != 8) {
		LPDIRECT3DDEVICE9 GRPDEV = GraphicDevice::GetInstance()->Get_Device();
		float Firstpow = 6.f;
		_vec3 FirstScale = { 1.f * Firstpow, 1.f * Firstpow, 1.f * Firstpow };
		_vec3 FirstPos = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x - 0.5f,
						 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y + 0.1007f,
						 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z - 8.f
		};
		float Secondpow = 3.f;
		_vec3 SecondScale = { 1.f * Secondpow, 1.f * Secondpow, 1.f * Secondpow };
		_vec3 SecondPos = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x + 2.5f,
						 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y + 0.1008f,
						 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z - 9.f
		};
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::FSWING_EXP_EFFECT, L"FSwing EXP", &FirstPos, FirstScale, 0.5f);
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::FSWING_EXP_EFFECT, L"FSwing EXP", &SecondPos, SecondScale, 0.4f);
	}
	// FSWING - EXPLOSION
	else if (dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_CurrentIndex() == 9 && dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_PreviousIndex() != 9) {
		LPDIRECT3DDEVICE9 GRPDEV = GraphicDevice::GetInstance()->Get_Device();
		float Firstpow = 4.f;
		_vec3 FirstScale = { 1.f * Firstpow, 1.f * Firstpow, 1.f * Firstpow };
		_vec3 FirstPos = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x + 0.5f,
						 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y + 0.1009f,
						 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z - 9.f
		};
		float Secondpow = 7.f;
		_vec3 SecondScale = { 1.f * Secondpow, 1.f * Secondpow, 1.f * Secondpow };
		_vec3 SecondPos = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x + 2.5f,
						 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y + 0.101f,
						 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z - 8.f
		};
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::FSWING_EXP_EFFECT, L"FSwing EXP", &FirstPos, FirstScale, 0.7f);
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::FSWING_EXP_EFFECT, L"FSwing EXP", &SecondPos, SecondScale, 0.3f);
	}
	// FSWING - EXPLOSION
	else if (dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_CurrentIndex() == 10 && dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_PreviousIndex() != 10) {
		LPDIRECT3DDEVICE9 GRPDEV = GraphicDevice::GetInstance()->Get_Device();
		float Firstpow = 3.f;
		_vec3 FirstScale = { 1.f * Firstpow, 1.f * Firstpow, 1.f * Firstpow };
		_vec3 FirstPos = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x + 2.5f,
						 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y + 0.1011f,
						 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z - 8.f
		};
		float Secondpow = 4.f;
		_vec3 SecondScale = { 1.f * Secondpow, 1.f * Secondpow, 1.f * Secondpow };
		_vec3 SecondPos = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x + 3.f,
						 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y + 0.1012f,
						 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z - 7.f
		};
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::FSWING_EXP_EFFECT, L"FSwing EXP", &FirstPos, FirstScale, 0.3f);
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::FSWING_EXP_EFFECT, L"FSwing EXP", &SecondPos, SecondScale, 0.6f);
	}
}
VOID FSwingState::FSM_StateExit(GameObject* _Owner)		{
	LPDIRECT3DDEVICE9 GRPDEV = GraphicDevice::GetInstance()->Get_Device();
	float Firstpow = 3.5f;
	_vec3 FirstScale = { 1.f * Firstpow, 1.f * Firstpow, 1.f * Firstpow };
	_vec3 FirstPos = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x + 5.5f,
					 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y + 0.1011f,
					 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z - 7.f
	};
	float Secondpow = 2.5f;
	_vec3 SecondScale = { 1.f * Secondpow, 1.f * Secondpow, 1.f * Secondpow };
	_vec3 SecondPos = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x + 6.f,
					 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y + 0.1012f,
					 (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z - 6.f
	};
	PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::FSWING_EXP_EFFECT, L"FSwing EXP", &FirstPos, FirstScale, 0.35f);
	PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::FSWING_EXP_EFFECT, L"FSwing EXP", &SecondPos, SecondScale, 0.45f);
}

VOID NormalSlamState	::FSM_StateEnter(GameObject* _Owner)	{}
VOID NormalSlamState	::FSM_StateUpdate(GameObject* _Owner)	{
	// SLAM - DANGER AREA EFFECT
	if (dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_CurrentIndex() == 1 && dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_PreviousIndex() != 1) {
		LPDIRECT3DDEVICE9 GRPDEV = GraphicDevice::GetInstance()->Get_Device();
		float Playerpow = 3.5f;
		_vec3 PlayerScale = { 1.f * Playerpow, 1.f * Playerpow, 1.f * Playerpow };
		Transform* PL = dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_GameObject(L"Player")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
		_vec3 PlayerPos = { PL->Get_Position()->x, PL->Get_Position()->y, PL->Get_Position()->z - 1.5f };
		dynamic_cast<FinalBoss*>(_Owner)->Set_PlayerPosition({ PL->Get_Position()->x, PL->Get_Position()->y + 1.f, PL->Get_Position()->z + 0.01f });
		dynamic_cast<FinalBoss*>(_Owner)->Set_EnableGroundExp(TRUE);

		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"Explosion Warning", &PlayerPos, PlayerScale, 0.45f);
		dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Explosion Warning")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
			->Set_Rotation(85.f, 0.f, 0.f);
	}
	// SLAM - CIRCLE EFFECT
	if (dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_CurrentIndex() == 7 && dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_PreviousIndex() != 7) {
		LPDIRECT3DDEVICE9 GRPDEV = GraphicDevice::GetInstance()->Get_Device();
		float pow = 7.f;
		_vec3 Scale = { 1.f * pow, 0.7f * pow, 0.7f * pow };
		_vec3 PosL = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x - 5.7f,
					   (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y - 1.5f,
					   (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z - 7.6f
		};
		_vec3 PosR = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x - 0.5f,
					   (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y - 1.5f,
					   (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z - 8.3f
		};
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SLAM_CIRCLE_EFFET, L"Slam EffectL", &PosL, Scale, 0.4f);
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SLAM_CIRCLE_EFFET, L"Slam EffectR", &PosR, Scale, 0.4f);
		dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Slam EffectL")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
			->Set_Rotation(85.f, 0.f, 0.f);
		dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Slam EffectR")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
			->Set_Rotation(85.f, 0.f, 0.f);
	}
}
VOID NormalSlamState	::FSM_StateExit(GameObject* _Owner)		{}

VOID MeteorSlamState	::FSM_StateEnter(GameObject* _Owner)	{}
VOID MeteorSlamState	::FSM_StateUpdate(GameObject* _Owner)	{
	// SLAM - DANGER AREA EFFECT
	if (dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_CurrentIndex() == 3 && dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_PreviousIndex() != 3) {
		LPDIRECT3DDEVICE9 GRPDEV = GraphicDevice::GetInstance()->Get_Device();
		float Playerpow = 3.5f;
		_vec3 PlayerScale = { 1.f * Playerpow, 1.f * Playerpow, 1.f * Playerpow };
		Transform* PL = dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_GameObject(L"Player")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
		_vec3 PlayerPos = { PL->Get_Position()->x, PL->Get_Position()->y, PL->Get_Position()->z - 1.5f };
		dynamic_cast<FinalBoss*>(_Owner)->Set_PlayerPosition({ PL->Get_Position()->x, PL->Get_Position()->y + 1.f, PL->Get_Position()->z + 0.01f });
		dynamic_cast<FinalBoss*>(_Owner)->Set_EnableQuadGroundExp(TRUE);
	
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::DANGER_AREA_EFFECT, L"Explosion Warning", &PlayerPos, PlayerScale, 0.45f);
		dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Explosion Warning")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
			->Set_Rotation(85.f, 0.f, 0.f);
	}
	// SLAM - SLAM EFFECT
	if (dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_CurrentIndex() == 7 && dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_PreviousIndex() != 7) {
		LPDIRECT3DDEVICE9 GRPDEV = GraphicDevice::GetInstance()->Get_Device();
		float pow = 7.f;
		_vec3 Scale = { 1.f * pow, 0.7f * pow, 0.7f * pow };
		_vec3 PosL = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x - 5.7f,
					   (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y - 1.5f,
					   (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z - 7.6f
		};
		_vec3 PosR = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x - 0.5f,
					   (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y - 1.5f,
					   (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z - 8.3f,
		}; 
		_vec3 PosFL = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x - 5.3f,
					   (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y ,
					   (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z - 1.3f
					};
		_vec3 PosFR = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x + 0.5f,
					   (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y ,
					   (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z - 3.f
					};
		float Elecpow = 6.f;
		_vec3 ElecScale = { 1.f * Elecpow, 1.f * Elecpow, 1.f * Elecpow };
		_vec3 ElecPos = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x ,
					  (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y - 0.95f,
					  (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z - 5.f
		};
		
		float Flamepow = 8.f;
		_vec3 FlameScale = { 1.f * Flamepow, 1.f * Flamepow, 1.f * Flamepow };
	
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::FSWING_ELECTRIC_EFFECT, L"FSwing Electric Effect", &ElecPos, ElecScale, 0.3f);
		dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"FSwing Electric Effect")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
			->Set_Rotation(85.f, 0.f, 0.f);
	
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SLAM_CIRCLE_EFFET, L"Slam EffectL", &PosL, Scale, 0.4f);
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SLAM_CIRCLE_EFFET, L"Slam EffectR", &PosR, Scale, 0.4f);
		dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Slam EffectL")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
			->Set_Rotation(85.f, 0.f, 0.f);
		dynamic_cast<Transform*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::BOSS, L"Slam EffectR")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))
			->Set_Rotation(85.f, 0.f, 0.f);
	
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::METEOR_SLAM_FLAME_EFFECT, L"Slam Flame EffectL", &PosFL, FlameScale, 0.8f);
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::METEOR_SLAM_FLAME_EFFECT, L"Slam Flame EffectR", &PosFR, FlameScale, 0.8f);
	}
	// SLAM - METEOR EFFECT
	if (dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_CurrentIndex() == 8 && dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_PreviousIndex() != 8) {
		LPDIRECT3DDEVICE9 GRPDEV = GraphicDevice::GetInstance()->Get_Device();
		dynamic_cast<FinalBoss*>(_Owner)->Set_EnableMeteorExp(TRUE);
	}
}
VOID MeteorSlamState	::FSM_StateExit(GameObject* _Owner)		{}

VOID StandState	::FSM_StateEnter(GameObject* _Owner)	{}
VOID StandState	::FSM_StateUpdate(GameObject* _Owner)	{}
VOID StandState	::FSM_StateExit(GameObject* _Owner)		{}