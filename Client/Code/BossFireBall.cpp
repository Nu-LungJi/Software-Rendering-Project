#include "../Include/PCH.h"
#include "BossFireBall.h"

BossFireBall::BossFireBall(LPDIRECT3DDEVICE9 _GRPDEV)	: GameObject(_GRPDEV)	{}
BossFireBall::BossFireBall(CONST GameObject& _RHS)		: GameObject(_RHS)		{}
BossFireBall::~BossFireBall()													{}

HRESULT	BossFireBall::Ready_GameObject() {
	if (FAILED(Component_Initialize()))	return E_FAIL;
	if (FAILED(Texture_Initialize()))	return E_FAIL;

	CollisionManager::GetInstance()->Add_ColliderObject(this);

	Boss = static_cast<FinalBoss*>(SceneManager::GetInstance()->Get_GameObject(L"Docheol"));
	if (Boss != nullptr && dynamic_cast<FinalBoss*>(Boss)->Get_ModeState(BOSSMODE::MODE_RAGE) == TRUE)		{ Animation_TexList = &Animation_RageTexList;	}
	else																									{ Animation_TexList = &Animation_NormalTexList; }

	Animation_Timer			= 0.f;
	Animation_CurrentIndex	= 0;
	Animation_PreviousIndex = 0;
	Animation_Interval		= 0.035f;
	Animation_FrameCount	= ANIMATION_BOSS_FIREBALL;

	Component_Transform->Rotation(ROT_X, 80.f);
	Direction = { 1.f, 0.f, 0.f };
	FireBall_DirectionAngle = 0.f;
	FireBall_Speed = 0.25f;
	FireBall_Duration = 2.f;
	FireBall_Timer = 0.f;

	ObjectTAG = L"BossFB";

	return S_OK;
}
INT		BossFireBall::Update_GameObject(CONST FLOAT& _DT) { 
	FireBall_Timer += _DT;
	if (ObjectDead == TRUE || Boss->Get_ObjectDead() == TRUE) {
		ObjectDead = TRUE;
		CollisionManager::GetInstance()->Delete_ColliderObject(this);
		return -1;
	}
		
	if (FireBall_Timer >= FireBall_Duration) {
		ObjectDead = TRUE;
		_vec3 Scale = { 2.f, 2.f, 2.f };
		if		(Boss != nullptr && Boss->Get_ModeState(BOSSMODE::MODE_RAGE) == TRUE) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_FIREBALL_EFFECT, L"FireBall Disappear", Component_Transform->Get_Position(), Scale, 0.8f);
		}
		else if (Boss != nullptr && Boss->Get_ModeState(BOSSMODE::MODE_RAGE) == FALSE) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::FIREBALL_EFFECT, L"FireBall Disappear", Component_Transform->Get_Position(), Scale, 0.8f);
		}
		else if (Boss == nullptr) {
			Boss = dynamic_cast<FinalBoss*>(SceneManager::GetInstance()->Get_GameObject(L"Docheol"));
			if (Boss->Get_ModeState(BOSSMODE::MODE_RAGE) == TRUE) {
				PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_FIREBALL_EFFECT, L"FireBall Disappear", Component_Transform->Get_Position(), Scale, 0.8f);
			}
			else {
				PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::FIREBALL_EFFECT, L"FireBall Disappear", Component_Transform->Get_Position(), Scale, 0.8f);
			}
		}
	}
	GameObject::Update_GameObject(_DT);
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	
	FireBall_Linear_Movement(&Direction, FireBall_DirectionAngle, FireBall_Speed);

	Animation_Timer += _DT;
	Animation_PreviousIndex = Animation_CurrentIndex;
	if (Animation_Timer > Animation_Interval) {
		Animation_CurrentIndex = Animation_CurrentIndex + 1;
		if (Animation_CurrentIndex == Animation_FrameCount)	Animation_CurrentIndex = 0;
		Animation_Timer = 0.f;
	}

	return 0; 
}
VOID	BossFireBall::LateUpdate_GameObject(CONST FLOAT& _DT) {
	if (ObjectDead == TRUE || Boss->Get_ObjectDead() == TRUE) {
		ObjectDead = TRUE;
		CollisionManager::GetInstance()->Delete_ColliderObject(this);
		return;
	}
	GameObject::LateUpdate_GameObject(_DT);
	if ((ObjectTAG == L"Sup FireBall0" || ObjectTAG == L"Sup FireBall1" || ObjectTAG == L"Sup FireBall2")) return;
	if (Boss->Get_ObjectDead() != TRUE 
		&& Component_Transform->Get_Position()->z > static_cast<Transform*>(Boss->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position()->z - 7.f) {
		AlphaZValue = 1.f;
	}
	else {
		AlphaZValue = -1.f;
	}
}
VOID	BossFireBall::Render_GameObject() {
	if (ObjectDead == TRUE || Boss->Get_ObjectDead() == TRUE) {
		ObjectDead = TRUE;
		CollisionManager::GetInstance()->Delete_ColliderObject(this);
		return;
	}

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	GRPDEV->SetTexture(0, (*Animation_TexList)[Animation_CurrentIndex]);

	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

BOOL	BossFireBall::OnCollisionEnter(GameObject* _Other)	{ 
	if (_Other->Get_ObjectTag() == L"Player" && dynamic_cast<Player*>(_Other)->Get_Invincible() == FALSE) {
		ObjectDead = TRUE;
		dynamic_cast<MainUI*>(SceneManager::GetInstance()->Get_GameObject(L"MainUI"))->Player_LostHP();
		_vec3 Scale = { 2.f, 2.f, 2.f };
		if		(Boss != nullptr && Boss->Get_ModeState(BOSSMODE::MODE_RAGE) == TRUE) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_FIREBALL_EFFECT, L"FireBall Disappear", Component_Transform->Get_Position(), Scale, 0.8f);
		}
		else if (Boss != nullptr && Boss->Get_ModeState(BOSSMODE::MODE_RAGE) == FALSE) {
			PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::FIREBALL_EFFECT, L"FireBall Disappear", Component_Transform->Get_Position(), Scale, 0.8f);
		}
		else if (Boss == nullptr) {
			Boss = dynamic_cast<FinalBoss*>(SceneManager::GetInstance()->Get_GameObject(L"Docheol"));
			if (Boss->Get_ModeState(BOSSMODE::MODE_RAGE) == TRUE) {
				PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::RAGE_FIREBALL_EFFECT, L"FireBall Disappear", Component_Transform->Get_Position(), Scale, 0.8f);
			}
			else {
				PLAY_BOSS_FRONTEFFECT_ONCE(BOSS_EFFECT::FIREBALL_EFFECT, L"FireBall Disappear", Component_Transform->Get_Position(), Scale, 0.8f);
			}
		}
	}
	return TRUE; 
}
BOOL	BossFireBall::OnCollisionStay(GameObject* _Other)	{ return TRUE; }
BOOL	BossFireBall::OnCollisionExit(GameObject* _Other)	{ return TRUE; }

HRESULT	BossFireBall::Component_Initialize()	{ 
	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Texture = ADD_COMPONENT_TEXTURE;

	Component_Transform = ADD_COMPONENT_TRANSFORM;
	Component_Transform->Set_Pos(0.f, 0.f, 0.f);
	Component_Transform->Set_Scale(5.f / 2.f, 1.5f / 2.f, 2.f / 2.f);

	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);
	Component_Collider->Set_Scale(0.3f, 0.3f, 0.3f);
	Component_Collider->Set_Hp(1.f);

	return S_OK; 
}
HRESULT BossFireBall::Texture_Initialize()		{ 
	wstring Base = L"";
	for (INT PIC = 1; PIC <= ANIMATION_BOSS_FIREBALL; ++PIC) {
		Base = L"Effect_FireBall" + to_wstring(PIC) + L".png";
		Animation_NormalTexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}
	for (INT PIC = 1; PIC <= ANIMATION_BOSS_FIREBALL; ++PIC) {
		Base = L"Effect_RageFireBall" + to_wstring(PIC) + L".png";
		Animation_RageTexList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}
	return S_OK;
}

BossFireBall*	BossFireBall::Create(LPDIRECT3DDEVICE9 _GRPDEV) { 
	BossFireBall* BFB = new BossFireBall(_GRPDEV);
	if (FAILED(BFB->Ready_GameObject())) {
		MSG_BOX("Cannot Create BossFireBall.");
		Safe_Release(BFB);
		return nullptr;
	}
	return BFB;
}
VOID	BossFireBall::FireBall_Linear_Movement(_vec3* _Direction, FLOAT _Angle, FLOAT _Speed) {
	_matrix RotMat;
	Direction = { 1.f, 0.f, 0.f };
	_vec3 XAxis = { 1.f, 0.f, 0.f };

	D3DXMatrixIdentity(&RotMat);
	D3DXMatrixRotationZ(&RotMat, D3DXToRadian(_Angle));
	D3DXVec3TransformCoord(_Direction, _Direction, &RotMat);
	D3DXMatrixRotationX(&RotMat, D3DXToRadian(80.f));
	D3DXVec3TransformCoord(_Direction, _Direction, &RotMat);
	D3DXMatrixRotationY(&RotMat, 0);
	D3DXVec3TransformCoord(_Direction, _Direction, &RotMat);
	
	Component_Transform->Set_Pos(
		Component_Transform->Get_Position()->x +_Direction->x * _Speed,
		Component_Transform->Get_Position()->y ,
		Component_Transform->Get_Position()->z -_Direction->z * _Speed
	);
	FLOAT Angle = 0.f, RealAngle = 0.f;
	
	
	if (((INT)fabsf(_Angle) % 360) < 180.f) {
		Angle = D3DXToDegree(acosf(D3DXVec3Dot(_Direction, &XAxis)));
	}
	else if(((INT)fabsf(_Angle) % 360) >= 180.f){
		Angle = -D3DXToDegree(acosf(D3DXVec3Dot(_Direction, &XAxis)));
	}
	Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
	Component_Transform->Set_Rotation(80.f, Angle, 0.f);
	Component_Collider->Set_Offset({ 1.5f * cosf(D3DXToRadian(Angle)), 0.f,  - 1.5f * sinf(D3DXToRadian(Angle)) });
}
VOID			BossFireBall::Free() {
	GameObject::Free();
}