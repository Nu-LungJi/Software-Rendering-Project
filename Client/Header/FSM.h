#pragma once
#include "StateMachine.h"

class AppearState : public State {
	DECLARE_FSM_SINGLETON(AppearState);
public:
	virtual void FSM_StateEnter();
	virtual void FSM_StateUpdate();
	virtual void FSM_StateExit();
};

class DeadState : public State {
	DECLARE_FSM_SINGLETON(DeadState);
public:
	virtual void FSM_StateEnter();
	virtual void FSM_StateUpdate();
	virtual void FSM_StateExit();
};

class RageUpState : public State {
	DECLARE_FSM_SINGLETON(RageUpState);
public:
	virtual void FSM_StateEnter();
	virtual void FSM_StateUpdate();
	virtual void FSM_StateExit();
};

class RSwingState : public State {
	DECLARE_FSM_SINGLETON(RSwingState);
public:
	virtual void FSM_StateEnter();
	virtual void FSM_StateUpdate();
	virtual void FSM_StateExit();
};

class SlamState : public State {
	DECLARE_FSM_SINGLETON(SlamState);
public:
	virtual void FSM_StateEnter();
	virtual void FSM_StateUpdate();
	virtual void FSM_StateExit();
};

class StandState : public State {
	DECLARE_FSM_SINGLETON(StandState);
public:
	virtual void FSM_StateEnter();
	virtual void FSM_StateUpdate();
	virtual void FSM_StateExit();
};