#include "../Include/PCH.h"
#include "FSM.h"

IMPLEMENT_SINGLETON(AppearState)
IMPLEMENT_SINGLETON(DeadState)
IMPLEMENT_SINGLETON(RageUpState)
IMPLEMENT_SINGLETON(RSwingState)
IMPLEMENT_SINGLETON(SlamState)
IMPLEMENT_SINGLETON(StandState)

void AppearState::FSM_StateEnter()	{}
void AppearState::FSM_StateUpdate()	{
	//FinalBoss* Owner = 
}
void AppearState::FSM_StateExit()	{}

void DeadState	::FSM_StateEnter()	{}
void DeadState	::FSM_StateUpdate()	{}
void DeadState	::FSM_StateExit()	{}

void RageUpState::FSM_StateEnter()	{}
void RageUpState::FSM_StateUpdate() {}
void RageUpState::FSM_StateExit()	{}

void RSwingState::FSM_StateEnter()	{}
void RSwingState::FSM_StateUpdate() {}
void RSwingState::FSM_StateExit()	{}

void SlamState	::FSM_StateEnter()	{}
void SlamState	::FSM_StateUpdate()	{}
void SlamState	::FSM_StateExit()	{}

void StandState	::FSM_StateEnter()	{}
void StandState	::FSM_StateUpdate() {}
void StandState	::FSM_StateExit()	{}