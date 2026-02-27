#include "../Include/PCH.h"
#include "Supporter.h"

Supporter::Supporter(LPDIRECT3DDEVICE9 _GRPDEV)	: GameObject(_GRPDEV)	{}
Supporter::Supporter(CONST GameObject& _RHS)	: GameObject(_RHS)		{}
Supporter::~Supporter()													{}

HRESULT	Supporter::Ready_GameObject(){
	if (FAILED(Component_Initialize()))	return E_FAIL;
	if (FAILED(Texture_Initialize()))	return E_FAIL;

	CollisionManager::GetInstance()->Add_ColliderObject(this);

	Animation_TexList = nullptr;
	Animation_Timer = 0.f;
	Animation_CurrentIndex = 0;
	Animation_PreviousIndex = 0;
	Animation_Interval = 0.14f;
	Animation_TexList = &Animation_AppearTexList;
	Animation_FrameCount = ANIMATION_SUPPORTER_APPEAR;//ANIMATION_SUPPORTER_IDLE;

	Direction = { 0.f, 0.f, 1.f };

	CurrentAngle = 0.f;
	TickAngle = 0.f;
	FBNumbering = 0;

	Supporter_Type = 1;

	Component_Transform->Rotation(ROT_X, 80.f);

	Effect_Timer = 0.f;

	Enable_ScaleInc = FALSE;
	Scale_Stack = 0.f;
	Rage_Movement = FALSE;

	return S_OK;
}
INT		Supporter::Update_GameObject(CONST FLOAT& _DT) { 
	if (ObjectDead == TRUE) return -1;
	
	GameObject::Update_GameObject(_DT);
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	
	Animation_Timer += _DT;

	Animation_PreviousIndex = Animation_CurrentIndex;
	if (Animation_Timer > Animation_Interval) {
		Animation_CurrentIndex = Animation_CurrentIndex + 1;
		if (Animation_CurrentIndex == Animation_FrameCount)	Animation_CurrentIndex = 0;
		Animation_Timer = 0.f;
	}

	Scale_Increment(_DT);

	if		(Supporter_Type == 1)
		Normal_Supporter_Action(_DT);
	else if (Supporter_Type == 2)
		Rage_Supporter_Action(_DT);

	if (Animation_TexList == &Animation_AppearTexList && Animation_CurrentIndex == ANIMATION_SUPPORTER_APPEAR - 1) {
		Animation_TexList = &Animation_IdleTexList;
		Animation_CurrentIndex = 0;
		Animation_Interval = 0.07f;
		Animation_FrameCount = ANIMATION_SUPPORTER_IDLE;
	}

	return 0;
}
VOID	Supporter::LateUpdate_GameObject(CONST FLOAT& _DT){
	if (ObjectDead == TRUE)  return;
	GameObject::LateUpdate_GameObject(_DT);
}
VOID	Supporter::Render_GameObject(){
	if (ObjectDead == TRUE)  return;
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	GRPDEV->SetTexture(0, (*Animation_TexList)[Animation_CurrentIndex]);

	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

BOOL	Supporter::OnCollisionEnter(GameObject* _Other)	{ 
	if (Supporter_Type == 1) {
		if (_Other->Get_ObjectTag() == L"Docheol") {
			ObjectDead = TRUE;
			_vec3 Scale = { 4.f, 4.f, 4.f };

			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SUPPORTER_EFFECT, L"Supporter Disappear Effect", Component_Transform->Get_Position(), Scale, 0.5f);
			return FALSE;
		}
		if (_Other->Get_ObjectTag() == L"PlayerArrow") {
			// Disappear Effect
			ObjectDead = TRUE;
			_vec3 Scale = { 4.f, 4.f, 4.f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SUPPORTER_EFFECT, L"Supporter Disappear Effect", Component_Transform->Get_Position(), Scale, 0.5f);
			return FALSE;
		}
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
		Base = L"Rage_Supporter_Idle" + to_wstring(PIC) + L".png";
		Animation_IdleTexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}
	for (INT PIC = 1; PIC <= ANIMATION_SUPPORTER_APPEAR; ++PIC) {
		Base = L"Rage_Supporter_Appear" + to_wstring(PIC) + L".png";
		Animation_AppearTexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}
	for (INT PIC = 1; PIC <= ANIMATION_NONANIM_FRAMECOUNT; ++PIC) {
		Base = L"DumpTexture" + to_wstring(PIC) + L".png";
		Animation_NonAnimTexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
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

VOID Supporter::Scale_Increment(CONST FLOAT& _DT) {
	if (Enable_ScaleInc) {
		Scale_Stack += _DT;
		if (Scale_Stack <= 0.25f) {
			Component_Transform->Set_Scale(8.f * Scale_Stack, 8.f * Scale_Stack, 8.f * Scale_Stack);
		}
		else {
			Enable_ScaleInc = FALSE;
			Scale_Stack = 0.f;
			Component_Transform->Set_Scale(2.f, 2.f, 2.f);
		}
	}
}

VOID Supporter::Normal_Supporter_Action(CONST FLOAT& _DT) {
	Effect_Timer += _DT;

	if (Effect_Timer > 1.5f) {
		_vec3 Scale = { 4.f, 4.f, 4.f };
		_vec3 Pos = { Component_Transform->Get_Position()->x,  Component_Transform->Get_Position()->y + 2.f,  Component_Transform->Get_Position()->z + 2.f };
		PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SUPPORTER_EFFECT, L"Supporter Effect", Component_Transform->Get_Position(), Scale, 0.5f);

		for (INT IDX = 0; IDX < 6; IDX++) {
			wstring FBTag = ObjectTAG + L"_FireBall" + to_wstring(FBNumbering++);
			SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_BOSS_FIREBALL, FBTag.c_str());
			BossFireBall* FB = dynamic_cast<BossFireBall*>(SceneManager::GetInstance()->Get_GameObject(FBTag.c_str()));
			FB->Set_FireBall_Pos(*Component_Transform->Get_Position());
			FB->Set_FireBall_Pos(*Component_Transform->Get_Position());
			FB->Set_FireBall_Speed(0.15f);
			FB->Set_FireBall_Duration(30.f);

			if		(IDX == 0)	FB->Set_FireBall_Angle(CurrentAngle);
			else if (IDX == 1)	FB->Set_FireBall_Angle(CurrentAngle - 60);
			else if (IDX == 2)	FB->Set_FireBall_Angle(CurrentAngle + 60);
			else if (IDX == 3)	FB->Set_FireBall_Angle(CurrentAngle - 120);
			else if (IDX == 4)	FB->Set_FireBall_Angle(CurrentAngle + 120);
			else if (IDX == 5)	FB->Set_FireBall_Angle(CurrentAngle - 180);
		}

		Effect_Timer = 0.f;
	}
	CurrentAngle += TickAngle;
	
	Direction = { 5.f * sinf(D3DXToRadian(CurrentAngle)), 0.f, 5.f * cosf(D3DXToRadian(CurrentAngle)) };

	Component_Transform->Move_Pos(&Direction, 0.5f, _DT);
}
VOID Supporter::Rage_Supporter_Action(CONST FLOAT& _DT) {
	if		(Rage_Movement) {
		Effect_Timer += _DT;
		if (Direction.x != 0.f || Direction.z != 0.f)
			D3DXVec3Normalize(&Direction, &Direction);
		Component_Transform->Set_Pos(
			Component_Transform->Get_Position()->x + Direction.x / 10.f * (cosf(Effect_Timer * D3DX_PI / 2) + 1),
			Component_Transform->Get_Position()->y + Direction.y / 10.f * (cosf(Effect_Timer * D3DX_PI / 2) + 1),
			Component_Transform->Get_Position()->z + Direction.z / 10.f * (cosf(Effect_Timer * D3DX_PI / 2) + 1)
		);
		if (Effect_Timer >= 1.5f) {
			_vec3 Scale = { 5.f, 5.f, 5.f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SUPPORTER_EFFECT, L"Boss Sup Effect", Component_Transform->Get_Position(), Scale, 0.5f);
			Animation_TexList = &Animation_NonAnimTexList;
			Animation_CurrentIndex = 0;
			Animation_Interval = 0.07f;
			Animation_FrameCount = ANIMATION_NONANIM_FRAMECOUNT;

			Rage_Movement = FALSE;
			Spiral_FireBall = TRUE;
			Effect_Timer = 0.f;
		}
	}
	else if (Spiral_FireBall) {
		Effect_Timer += _DT;
		if (Effect_Timer <= 2.0f) {
			if (BFBVec.size() == 0) {
				for (INT IDX = 0; IDX < 3; ++IDX) {
					wstring FBallTAG = L"Sup FireBall" + to_wstring(IDX);
					BFBVec.push_back(BossFireBall::Create(GRPDEV));
					BFBVec[IDX]->Set_ObjectTag(FBallTAG.c_str());
					SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, BFBVec[IDX]);
					dynamic_cast<BossFireBall*>(BFBVec[IDX])->Set_FireBall_Angle(120 * IDX);
					dynamic_cast<BossFireBall*>(BFBVec[IDX])->Set_FireBall_Speed(0.125f);
					dynamic_cast<Transform*>(BFBVec[IDX]->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(*Component_Transform->Get_Position());
					dynamic_cast<Transform*>(BFBVec[IDX]->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Scale(5.f / 3.f, 1.5f / 3.f, 2.f / 3.f);
				}
			}
			else {
				for (auto& BFB : BFBVec) {
					BossFireBall* Origin = dynamic_cast<BossFireBall*>(BFB);
					Origin->Set_FireBall_Angle(Origin->Get_FireBall_Angle() + 2.7f);
				}
			}
		}
		if (Effect_Timer >= 2.0f) {
			_vec3 Scale = { 1.5f, 1.5f, 1.5f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SUPPORTER_EFFECT, L"Boss Sup Effect", Component_Transform->Get_Position(), Scale * 2, 0.5f);
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SUPPORTER_APPEAR_EFFECT, L"Temporarily Sup Appear", Component_Transform->Get_Position(), Scale, 0.2f);
		}
		if (Effect_Timer >= 2.2f) {
			_vec3 Scale = { 1.5f, 1.5f, 1.5f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SUPPORTER_STAY_EFFECT, L"Temporarily Sup Stay", Component_Transform->Get_Position(), Scale, 0.5f);
		}
		if (Effect_Timer >= 2.7f) {
			_vec3 Scale = { 1.5f, 1.5f, 1.5f };
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::SUPPORTER_EFFECT, L"Boss Sup Effect", Component_Transform->Get_Position(), Scale, 0.5f);

			BFBVec.clear();
			for (INT IDX = 0; IDX < 3; ++IDX) {
				wstring FBallTAG = L"Sup FireBall" + to_wstring(IDX);
				BFBVec.push_back(BossFireBall::Create(GRPDEV));
				BFBVec[IDX]->Set_ObjectTag(FBallTAG.c_str());
				SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<BossFireBall>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, BFBVec[IDX]);
				dynamic_cast<BossFireBall*>(BFBVec[IDX])->Set_FireBall_Angle(120 * IDX);
				dynamic_cast<BossFireBall*>(BFBVec[IDX])->Set_FireBall_Speed(0.25f);
				dynamic_cast<Transform*>(BFBVec[IDX]->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(*Component_Transform->Get_Position());
				dynamic_cast<Transform*>(BFBVec[IDX]->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Scale(5.f / 2.5f, 1.5f / 2.5f, 2.f / 2.5f);
			}

			Spiral_FireBall = FALSE;
			ObjectDead = TRUE;
			Effect_Timer = 0.f;
		}
	}
}
VOID		Supporter::Free(){
	GameObject::Free();
}