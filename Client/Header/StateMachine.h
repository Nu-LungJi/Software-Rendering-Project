#pragma once
#include "GameObject.h"

class State {
public:
	virtual VOID FSM_StateEnter(GameObject* _Owner) = 0;
	virtual VOID FSM_StateUpdate(GameObject* _Owner) = 0;
	virtual VOID FSM_StateExit(GameObject* _Owner) = 0;

	virtual ~State() {};
};

class StateMachine : public GameObject {
private:
	explicit StateMachine(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit StateMachine(CONST GameObject& _RHS);
	virtual ~StateMachine();

public:
	virtual	HRESULT		Ready_GameObject()							{ return S_OK;	}
	virtual	INT			Update_GameObject(const _float& _DT);
	virtual	VOID		LateUpdate_GameObject(const _float& _DT)	{				}
	virtual	VOID		Render_GameObject()							{				}

public:
	VOID		FSM_StateChange(State* _State);
	VOID		FSM_StateInit(State* _State)		{ CurrentState = _State; StateHistory.push_back(_State); }
	State*		FSM_GetCurrentState()				{ return CurrentState;	}
	VOID		FSM_SetOwner(GameObject* _Owner)	{ Owner = _Owner;		}

public:
	static		StateMachine*	Create(LPDIRECT3DDEVICE9 _GRPDEV);

private:
	GameObject* Owner;

	State*		CurrentState;
	State*		PreviousState;

	vector<State*>		StateHistory;
private:
	virtual		VOID	Free();
};

class AppearState : public State {
	DECLARE_FSM_SINGLETON(AppearState);
public:
	virtual VOID FSM_StateEnter(GameObject* _Owner);
	virtual VOID FSM_StateUpdate(GameObject* _Owner);
	virtual VOID FSM_StateExit(GameObject* _Owner);
};
class DeadState : public State {
	DECLARE_FSM_SINGLETON(DeadState);
public:
	virtual VOID FSM_StateEnter(GameObject* _Owner);
	virtual VOID FSM_StateUpdate(GameObject* _Owner);
	virtual VOID FSM_StateExit(GameObject* _Owner);
};
class RageUpState : public State {
	DECLARE_FSM_SINGLETON(RageUpState);
public:
	virtual VOID FSM_StateEnter(GameObject* _Owner);
	virtual VOID FSM_StateUpdate(GameObject* _Owner);
	virtual VOID FSM_StateExit(GameObject* _Owner);
};
class RSwingState : public State {
	DECLARE_FSM_SINGLETON(RSwingState);
public:
	virtual VOID FSM_StateEnter(GameObject* _Owner);
	virtual VOID FSM_StateUpdate(GameObject* _Owner);
	virtual VOID FSM_StateExit(GameObject* _Owner);
};
class FSwingState : public State {
	DECLARE_FSM_SINGLETON(FSwingState);
public:
	virtual VOID FSM_StateEnter(GameObject* _Owner);
	virtual VOID FSM_StateUpdate(GameObject* _Owner);
	virtual VOID FSM_StateExit(GameObject* _Owner);
};
class NormalSlamState : public State {
	DECLARE_FSM_SINGLETON(NormalSlamState);
public:
	virtual VOID FSM_StateEnter(GameObject* _Owner);
	virtual VOID FSM_StateUpdate(GameObject* _Owner);
	virtual VOID FSM_StateExit(GameObject* _Owner);
};
class MeteorSlamState : public State {
	DECLARE_FSM_SINGLETON(MeteorSlamState);
public:
	virtual VOID FSM_StateEnter(GameObject* _Owner);
	virtual VOID FSM_StateUpdate(GameObject* _Owner);
	virtual VOID FSM_StateExit(GameObject* _Owner);
};
class StandState : public State {
	DECLARE_FSM_SINGLETON(StandState);
public:
	virtual VOID FSM_StateEnter(GameObject* _Owner);
	virtual VOID FSM_StateUpdate(GameObject* _Owner);
	virtual VOID FSM_StateExit(GameObject* _Owner);
};