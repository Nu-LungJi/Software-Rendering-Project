#include "../Include/PCH.h"
#include "PlayerEffect.h"

PlayerEffect:: PlayerEffect(LPDIRECT3DDEVICE9 _GRPDEV)	: GameObject(_GRPDEV), TextureIndex(0), FrameTick(0.f){}
PlayerEffect:: PlayerEffect(CONST GameObject& _RHS)		: GameObject(_RHS), TextureIndex(0), FrameTick(0.f) {}
PlayerEffect::~PlayerEffect(){}

HRESULT PlayerEffect::Ready_Effect(PLAYER_SKILL _SKILLTYPE, _vec3* _PlayerPOS, BOOL _Repeatable, FLOAT _PlayTime, _vec3 _Size, BOOL PosChase) {
	if (FAILED(Component_Initialize())) return E_FAIL;

	if		(_SKILLTYPE == PLAYER_SKILL::SKILL_1)	{ Make_TextureList(L"Spr_Effect_ExplosionNormal02_");		}
	else if (_SKILLTYPE == PLAYER_SKILL::SKILL_2)	{ Make_TextureList(L"Spr_Ui_Effect_BossClear_lraCharge_");	}
	else if (_SKILLTYPE == PLAYER_SKILL::SKILL_3)	{ Make_TextureList(L"Spr_Ui_Stage01_TureMapEffect_");		}
	else if (_SKILLTYPE == PLAYER_SKILL::FAIRY_PULSE) { Make_TextureList(L"Fairy_Pulse"); }
	else if (_SKILLTYPE == PLAYER_SKILL::ICEARROW_PULSE) { Make_TextureList(L"IceArrow_Pulse"); }
	else if (_SKILLTYPE == PLAYER_SKILL::EVILHEAD_PULSE) { Make_TextureList(L"EvilHeadBow_Pulse"); }
	else if (_SKILLTYPE == PLAYER_SKILL::WIND_PULSE) { Make_TextureList(L"Wind_Charge"); }
	else if (_SKILLTYPE == PLAYER_SKILL::WIND_CHARGING) { Make_TextureList(L"Wind_Charging"); }
	else if (_SKILLTYPE == PLAYER_SKILL::FAIRY_HITEFFECT) { Make_TextureList(L"Fariy_HitEffect"); }
	else if (_SKILLTYPE == PLAYER_SKILL::ICE_CHARGING) { Make_TextureList(L"IceBowCharging"); }
	else if (_SKILLTYPE == PLAYER_SKILL::ICE_CHARGE) { Make_TextureList(L"IceArrow_Charge"); }
	else if (_SKILLTYPE == PLAYER_SKILL::ICE_SHADER) { Make_TextureList(L"IceShader"); }
	else if (_SKILLTYPE == PLAYER_SKILL::FAIRY_CHARGING) { Make_TextureList(L"FariyCharging"); }
	else if (_SKILLTYPE == PLAYER_SKILL::ICE_HITEFFECT) { Make_TextureList(L"IceCharge_Death"); }
	else if (_SKILLTYPE == PLAYER_SKILL::ICE_THORN) { Make_TextureList(L"IceThorn"); }
	else if (_SKILLTYPE == PLAYER_SKILL::EVIL_HITEFFECT) { Make_TextureList(L"EvilHead_HitEffect"); }
	else if (_SKILLTYPE == PLAYER_SKILL::EVIL_WAVE_EFFECT) { Make_TextureList(L"EvilWave_Effect"); }
	else if (_SKILLTYPE == PLAYER_SKILL::EVIL_THUNDER) { Make_TextureList(L"Evil_Thunder"); }
	else if (_SKILLTYPE == PLAYER_SKILL::EVIL_CHARGING) { Make_TextureList(L"EvilCharging"); }
	else if (_SKILLTYPE == PLAYER_SKILL::EVIL_CHARGE) { Make_TextureList(L"Evil_Charge"); }
	else if (_SKILLTYPE == PLAYER_SKILL::ARROW_HITEFFECT) { Make_TextureList(L"Arrow_HitEffect"); }
	else if (_SKILLTYPE == PLAYER_SKILL::WIND_AURA) { Make_TextureList(L"Wind_Aura"); }
	else if (_SKILLTYPE == PLAYER_SKILL::NPC_TIMESLOW) { Make_TextureList(L"NPC_TimeSlow"); }
	else if (_SKILLTYPE == PLAYER_SKILL::NPC_TIMESLOW_LOOF) { Make_TextureList(L"NPC_TimeSlow_Loof"); }

	//if (!AngleChase)
	//{
	//	Component_Transform->Set_Pos(*_PlayerPOS);		// 기본 위치 : 플레이어 중심 
	//	Repeatable = _Repeatable;
	//
	//	CameraObject* Camera = dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Camera"));
	//
	//	_vec3 cameraDir = *(Camera->Get_EyeVec()) - *(Camera->Get_AtVec());
	//	_vec3 planeDir = { 0.f, 1.f, 0.f };
	//
	//	_float angle = acosf(D3DXVec3Dot(D3DXVec3Normalize(&cameraDir, &cameraDir), D3DXVec3Normalize(&planeDir, &planeDir)));
	//	angle = angle / D3DX_PI * 180.f;
	//
	//	Component_Transform->Rotation(ROT_X, 90.f - angle);
	//}
	{
		if (PosChase) {
			_playerPos = _PlayerPOS;
		}
		else {
			_effectPos = { _PlayerPOS->x, _PlayerPOS->y, _PlayerPOS->z };
			_playerPos = &_effectPos;
		}
		_effectSize = _Size;
		SKILL_TYPE = _SKILLTYPE;
		Repeatable = _Repeatable;
		_angleChase = PosChase;
		_effectTimer = 0.f;
		
		CameraObject* Camera = dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Camera"));
		_vec3 cameraDir = *(Camera->Get_EyeVec()) - *(Camera->Get_AtVec());
		D3DXVec3Normalize(&cameraDir, &cameraDir);

		Player* player = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Player"));
		_vec3 MouseDir = player->Get_MouseDir();

		_vec2 dir2D = { MouseDir.x, MouseDir.z };
		D3DXVec2Normalize(&dir2D, &dir2D);

		_angle = atan2f(-dir2D.y, dir2D.x);

		_vec3 eye = { 0.f, 0.f, 0.f };
		_vec3 at = cameraDir;
		_vec3 up = { 0.f, 1.f, 0.f };

		_matrix matSize;
		D3DXMatrixIdentity(&matSize);
		D3DXMatrixScaling(&matSize, _effectSize.x, _effectSize.y, _effectSize.z);

		_matrix matBillboard;
		D3DXMatrixLookAtLH(&matBillboard, &eye, &at, &up);
		D3DXMatrixInverse(&matBillboard, nullptr, &matBillboard);

		float radian = D3DX_PI / 180.f;
		_matrix matRotZ;
		D3DXMatrixRotationZ(&matRotZ, _angle - D3DX_PI);

		_matrix matWorld = matSize * matBillboard;

		matWorld._41 = (*_playerPos).x;
		matWorld._42 = (*_playerPos).y;
		matWorld._43 = (*_playerPos).z;

		Component_Transform->Set_World(&matWorld);
	}
	
	PlayTime = _PlayTime;

	ObjectTAG = L"Player_Effect";

	if (_SKILLTYPE == PLAYER_SKILL::NPC_TIMESLOW || _SKILLTYPE == PLAYER_SKILL::NPC_TIMESLOW_LOOF)
		ObjectTAG = L"NPC_TIMESLOW";

	//CollisionManager::GetInstance()->Add_ColliderObject(this);

	return S_OK;
}


HRESULT PlayerEffect::Make_TextureList(wstring _FileName) {
	INT FRAME = 0;
	while (++FRAME){
		wstring FileName = _FileName + to_wstring(FRAME) + L".png";
		IDirect3DBaseTexture9* TEX = ResourceManager::GetInstance()->Find_Texture(FileName.c_str());
		if (TEX == nullptr) break;
		else { TextureList.push_back(TEX); }
	}

	ENDFRAME = TextureList.size() + 1;

	return S_OK;
}

INT  PlayerEffect::Update_GameObject(CONST FLOAT& _DT) {
	if (ObjectDead)	return 0;
	GameObject::Update_GameObject(_DT);
	 
	FrameTick += _DT;
	_effectTimer += _DT;
	Player* player = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Player"));



	switch (SKILL_TYPE)
	{
	case PLAYER_SKILL::ICE_CHARGING :
		if (!(KEY_HOLD(DIK_SPACE)) || player->GetBowCharging() == 0) ObjectDead = true;
		break;
	case PLAYER_SKILL::ICE_CHARGE:
		if(!(KEY_HOLD(DIK_SPACE)) || player->GetBowCharging() != 0) ObjectDead = true;
		break;
	case PLAYER_SKILL::FAIRY_CHARGING:
		if (!(KEY_HOLD(DIK_SPACE)) || player->GetBowCharging() == 0) ObjectDead = true;
		break;
	case PLAYER_SKILL::EVIL_CHARGING:
		if (!(KEY_HOLD(DIK_SPACE)) || player->GetBowCharging() == 0) ObjectDead = true;
		break;
	case PLAYER_SKILL::EVIL_CHARGE:
		if (!(KEY_HOLD(DIK_SPACE)) || player->GetBowCharging() != 0) ObjectDead = true;
		break;
	case PLAYER_SKILL::WIND_CHARGING:
		if (!(KEY_HOLD(DIK_SPACE)) || player->GetBowCharging() != 0) ObjectDead = true;
		break;
	case PLAYER_SKILL::WIND_PULSE:
		if (!(KEY_HOLD(DIK_SPACE)) || player->GetBowCharging() == 0) ObjectDead = true;
		break;
	case PLAYER_SKILL::NPC_TIMESLOW_LOOF:
		if (_effectTimer > (*player->Get_SlowTime()) - 1.f ) ObjectDead = true;
		break;
	}

	if (true)
	{
		CameraObject* Camera = dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Camera"));
		_vec3 cameraDir = *(Camera->Get_EyeVec()) - *(Camera->Get_AtVec());
		D3DXVec3Normalize(&cameraDir, &cameraDir);

		if (_angleChase) {
			Player* player = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Player"));
			_vec3 MouseDir = player->Get_MouseDir();
			_vec2 dir2D = { MouseDir.x, MouseDir.z };
			D3DXVec2Normalize(&dir2D, &dir2D);
			_angle = atan2f(-dir2D.y, dir2D.x);
		}

		_vec3 eye = { 0.f, 0.f, 0.f };
		_vec3 at = cameraDir;
		_vec3 up = { 0.f, 1.f, 0.f };

		_matrix matSize;
		D3DXMatrixIdentity(&matSize);
		D3DXMatrixScaling(&matSize, _effectSize.x, _effectSize.y, _effectSize.z);

		_matrix matBillboard;
		D3DXMatrixLookAtLH(&matBillboard, &eye, &at, &up);
		D3DXMatrixInverse(&matBillboard, nullptr, &matBillboard);

		_matrix matRotZ;
		D3DXMatrixRotationZ(&matRotZ, _angle - D3DX_PI);
		_matrix matWorld;
		if (_angleChase) matWorld = matSize * matRotZ * matBillboard;
		else matWorld = matSize * matBillboard;

		matWorld._41 = (*_playerPos).x;
		matWorld._42 = (*_playerPos).y;
		matWorld._43 = (*_playerPos).z;

		Component_Transform->Set_World(&matWorld);
	}

	return 0;
}
VOID PlayerEffect::LateUpdate_GameObject(CONST FLOAT& _DT) {
	if (ObjectDead)	return ;
	if (FrameTick > PlayTime / ENDFRAME) {
		if (TextureIndex++ >= ENDFRAME - 2) {	
			if (Repeatable) { TextureIndex = 0;}
			else {
				TextureIndex = ENDFRAME - 2; 
				ObjectDead = TRUE;
			}
		}
		FrameTick = 0.f;
	}
}
VOID PlayerEffect::Render_GameObject() {
	if (ObjectDead)	return;
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	GRPDEV->SetTexture(0, TextureList[TextureIndex]);
	
	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
BOOL PlayerEffect::OnCollisionEnter(GameObject* _Other) {
	return TRUE;
}
BOOL PlayerEffect::OnCollisionStay(GameObject* _Other) {
	return TRUE;
}
BOOL PlayerEffect::OnCollisionExit(GameObject* _Other) {
	return TRUE;
}
HRESULT	PlayerEffect::Component_Initialize() {
	Component_Buffer	= ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;

	Component_Collider	= ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);			// 충돌체가 오브젝트를 따라 다니도록
	Component_Collider->Set_Scale(0.1f, 0.1f, 0.1f);				// 충돌체의 범위 조절

	Component_Texture	= ADD_COMPONENT_TEXTURE;

	return S_OK;
}
PlayerEffect* PlayerEffect::Create(LPDIRECT3DDEVICE9 _GRPDEV, PLAYER_SKILL _SKILLTYPE, _vec3* _PlayerPOS, BOOL _Repeatable, FLOAT _PlayTime, _vec3 _Size, BOOL PosChase) {
	PlayerEffect* EFT = new PlayerEffect(_GRPDEV);
	if (FAILED(EFT->Ready_Effect(_SKILLTYPE, _PlayerPOS, _Repeatable, _PlayTime, _Size, PosChase))) {
		MSG_BOX("Cannot Create Effect.");
		Safe_Release(EFT);
		return nullptr;
	}
	return EFT;
}
VOID PlayerEffect::Free() {
	GameObject::Free();
}