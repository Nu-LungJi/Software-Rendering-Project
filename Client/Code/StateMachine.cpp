#include "../Include/PCH.h"
#include "StateMachine.h"

IMPLEMENT_SINGLETON(AppearState)
IMPLEMENT_SINGLETON(DeadState)
IMPLEMENT_SINGLETON(RageUpState)
IMPLEMENT_SINGLETON(RSwingState)
IMPLEMENT_SINGLETON(FSwingState)
IMPLEMENT_SINGLETON(SlamState)
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

	StateHistory.push_back(_State);

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
VOID RageUpState::FSM_StateUpdate(GameObject* _Owner)	{}
VOID RageUpState::FSM_StateExit(GameObject* _Owner)		{}

VOID RSwingState::FSM_StateEnter(GameObject* _Owner)	{}
VOID RSwingState::FSM_StateUpdate(GameObject* _Owner)	{
	if (dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_Index() == 5) {		// RSWING - PUNCH FLAME EFFECT
		LPDIRECT3DDEVICE9 GRPDEV = GraphicDevice::GetInstance()->Get_Device();
		float pow = 6.f;
		_vec3 Scale = { 1.f * pow, 0.7f * pow, 0.7f * pow };
		_vec3 Pos = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x - 1.5f,
					  (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y,
					  (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z - 4.f,
		};
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::RSWING_EFFECT, &Pos, Scale, 0.35f);
	}

	//if (dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_Index() == 7) {
	//	LPDIRECT3DDEVICE9 GRPDEV = GraphicDevice::GetInstance()->Get_Device();
	//	_vec3 Scale = { 2.4f / 2.f, 1.f/ 2.f, 1.f / 2.f };
	//	PLAY_BOSS_EFFECT(BOSS_EFFECT::RSWING_PROJ_EFFECT, dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position(),
	//		Scale, 0.2f);
	//}
}
VOID RSwingState::FSM_StateExit(GameObject* _Owner)		{}

VOID FSwingState::FSM_StateEnter(GameObject* _Owner)	{}
VOID FSwingState::FSM_StateUpdate(GameObject* _Owner)	{}
VOID FSwingState::FSM_StateExit(GameObject* _Owner)		{}

VOID SlamState	::FSM_StateEnter(GameObject* _Owner)	{}
VOID SlamState	::FSM_StateUpdate(GameObject* _Owner)	{
	if (dynamic_cast<FinalBoss*>(_Owner)->Get_Animation_Index() == 7) {		// SLAM - CIRCLE EFFECT
		LPDIRECT3DDEVICE9 GRPDEV = GraphicDevice::GetInstance()->Get_Device();
		float pow = 5.f;
		_vec3 Scale = { 1.f * pow, 0.7f * pow, 0.7f * pow };
		_vec3 PosL = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x - 4.5f,
					  (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y - 2.952f,
					  (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z - 5.8f
		};
		_vec3 PosR = { (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->x - 0.5f,
					  (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->y - 2.952f,
					  (*dynamic_cast<Transform*>(_Owner->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))).Get_Position()->z - 5.8f,
		};
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SLAM_EFFET, &PosL, Scale, 0.4f);
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SLAM_EFFET, &PosR, Scale, 0.4f);
	}
}
VOID SlamState	::FSM_StateExit(GameObject* _Owner)		{}

VOID StandState	::FSM_StateEnter(GameObject* _Owner)	{}
VOID StandState	::FSM_StateUpdate(GameObject* _Owner)	{}
VOID StandState	::FSM_StateExit(GameObject* _Owner)		{}