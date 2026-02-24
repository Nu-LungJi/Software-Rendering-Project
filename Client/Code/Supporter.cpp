#include "../Include/PCH.h"
#include "Supporter.h"

Supporter::Supporter(LPDIRECT3DDEVICE9 _GRPDEV)	: GameObject(_GRPDEV)	{}
Supporter::Supporter(CONST GameObject& _RHS)	: GameObject(_RHS)		{}
Supporter::~Supporter()													{}

HRESULT	Supporter::Ready_GameObject(){
	if (FAILED(Component_Initialize()))	return E_FAIL;
	if (FAILED(Texture_Initialize()))	return E_FAIL;

	CollisionManager::GetInstance()->Add_ColliderObject(this);

	Animation_Timer = 0.f;
	Animation_CurrentIndex = 0;
	Animation_PreviousIndex = 0;
	Animation_Interval = 0.07f;
	Animation_FrameCount = ANIMATION_SUPPORTER_IDLE;

	Direction = { 0.f, 0.f, 1.f };

	CurrentAngle = 0.f;
	TickAngle = 0.f;

	Component_Transform->Rotation(ROT_X, 80.f);

	Effect_Timer = 0.f;

	return S_OK;
}
INT		Supporter::Update_GameObject(CONST FLOAT& _DT) { 
	if (ObjectDead == TRUE) {
		
		return -1;
	}
	
	GameObject::Update_GameObject(_DT);
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	Supporter_Transform(_DT);
	Generate_FireBall(_DT);

	for (auto& BFB : BossFB)
		BFB->Update_GameObject(_DT);

	Animation_Timer += _DT;
	Effect_Timer += _DT;

	if (Effect_Timer > 1.5f) {
		_vec3 Scale = { 4.f, 4.f, 4.f };
		_vec3 Pos = { Component_Transform->Get_Position()->x,  Component_Transform->Get_Position()->y + 2.f,  Component_Transform->Get_Position()->z + 2.f };
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SUPPORTER_EFFECT, L"Supporter Effect", Component_Transform->Get_Position(), Scale, 0.5f);

		for (INT IDX = 0; IDX < 3; IDX++) {
			BossFB.push_back(BossFireBall::Create(GRPDEV));
			BossFB.back()->Set_FireBall_Pos(*Component_Transform->Get_Position());
			BossFB.back()->Set_FireBall_Speed(0.15f);
			BossFB.back()->Set_FireBall_Duration(30.f);
			if (IDX == 0)
				BossFB.back()->Set_FireBall_Angle(CurrentAngle);
			else if (IDX == 1)
				BossFB.back()->Set_FireBall_Angle(CurrentAngle - 120);
			else if (IDX == 2)
				BossFB.back()->Set_FireBall_Angle(CurrentAngle + 120);
		}

		Effect_Timer = 0.f;
	}

	Animation_PreviousIndex = Animation_CurrentIndex;
	if (Animation_Timer > Animation_Interval) {
		Animation_CurrentIndex = Animation_CurrentIndex + 1;
		if (Animation_CurrentIndex == Animation_FrameCount)	Animation_CurrentIndex = 0;
		Animation_Timer = 0.f;
	}
	CurrentAngle += TickAngle;
	if (CurrentAngle >= 180.f) {
		int a = 0;
	}
	Direction = { 5.f * sinf(D3DXToRadian(CurrentAngle)), 0.f, 5.f * cosf(D3DXToRadian(CurrentAngle)) };

	Component_Transform->Move_Pos(&Direction, 0.5f, _DT);

	return 0;
}
VOID	Supporter::LateUpdate_GameObject(CONST FLOAT& _DT){
	if (ObjectDead == TRUE)  return;
	GameObject::LateUpdate_GameObject(_DT);
	for (auto& BFB : BossFB)
		BFB->LateUpdate_GameObject(_DT);
}
VOID	Supporter::Render_GameObject(){
	if (ObjectDead == TRUE)  return;
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	GRPDEV->SetTexture(0, Animation_IdleTexList[Animation_CurrentIndex]);

	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	for (auto& BFB : BossFB)
		BFB->Render_GameObject();
}

BOOL	Supporter::OnCollisionEnter(GameObject* _Other)	{ 
	if (_Other->Get_ObjectTag() == L"Docheol") {
		// Disappear Effect
		for (auto& BFB : BossFB) 
			CollisionManager::GetInstance()->Delete_ColliderObject(BFB);
		for (auto& BFB : BossFB) 
			Safe_Release(BFB);
		
		BossFB.clear();
		ObjectDead = TRUE;
		_vec3 Scale = { 4.f, 4.f, 4.f };
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SUPPORTER_EFFECT, L"Supporter Disappear Effect", Component_Transform->Get_Position(), Scale, 0.5f);
		return FALSE;
	}
	if (_Other->Get_ObjectTag() == L"PlayerArrow") {
		// Disappear Effect
		ObjectDead = TRUE;
		_vec3 Scale = { 4.f, 4.f, 4.f };
		PLAY_BOSS_EFFECT_ONCE(BOSS_EFFECT::SUPPORTER_EFFECT, L"Supporter Disappear Effect", Component_Transform->Get_Position(), Scale, 0.5f);
		return FALSE;
	}
	return TRUE; 
}
BOOL	Supporter::OnCollisionStay(GameObject* _Other)	{ return TRUE; }
BOOL	Supporter::OnCollisionExit(GameObject* _Other)	{ return TRUE; }

HRESULT	Supporter::Component_Initialize()	{ 
	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Texture = ADD_COMPONENT_TEXTURE;

	Component_Transform = ADD_COMPONENT_TRANSFORM;
	Component_Transform->Set_Pos(0.f, 0.f, 0.f);
	Component_Transform->Set_Scale(3.f, 3.f, 3.f);
	Component_Transform->Set_Pos({ 64.115f, 1.5f, 104.83 });

	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);
	Component_Collider->Set_Scale(1.5f, 0.5f, 1.5f);
	Component_Collider->Set_Hp(1.f);

	return S_OK; 
}
HRESULT Supporter::Texture_Initialize()		{ 
	wstring Base = L"";
	for (INT PIC = 1; PIC <= ANIMATION_SUPPORTER_IDLE; ++PIC) {
		Base = L"Supporter_Idle" + to_wstring(PIC) + L".png";
		Animation_IdleTexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}
	return S_OK; 
}
Supporter* Supporter::Create(LPDIRECT3DDEVICE9 _GRPDEV) { 
	Supporter* SPT = new Supporter(_GRPDEV);
	if (FAILED(SPT->Ready_GameObject())) {
		MSG_BOX("Cannot Create Supporter.");
		Safe_Release(SPT);
		return nullptr;
	}
	return SPT;
}
VOID Supporter::Supporter_Transform(CONST FLOAT& _DT) {
	//D3DXVec3Normalize(&Direction, &Direction);
	//Component_Transform->Move_Pos(&Direction, 1.f, _DT);
}
VOID Supporter::Generate_FireBall(CONST FLOAT& _DT) {
	
}
VOID		Supporter::Free(){
	GameObject::Free();
}