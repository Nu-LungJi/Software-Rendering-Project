#include "../Include/PCH.h"
#include "PlayerEffect.h"

PlayerEffect:: PlayerEffect(LPDIRECT3DDEVICE9 _GRPDEV)	: GameObject(_GRPDEV), TextureIndex(0), FrameTick(0.f){}
PlayerEffect:: PlayerEffect(CONST GameObject& _RHS)		: GameObject(_RHS), TextureIndex(0), FrameTick(0.f) {}
PlayerEffect::~PlayerEffect(){}

HRESULT PlayerEffect::Ready_Effect(PLAYER_SKILL _SKILLTYPE, _vec3* _PlayerPOS, BOOL _Repeatable, FLOAT _PlayTime, _vec3 _Size, BOOL PosChase) {
	SKILL_TYPE = _SKILLTYPE;
	if (FAILED(Component_Initialize())) return E_FAIL;

	Player* player = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Player"));

	if		(_SKILLTYPE == PLAYER_SKILL::SKILL_1)	{ Make_TextureList(L"BalckHole_Loop");		}
	else if (_SKILLTYPE == PLAYER_SKILL::SKILL_2)	{ Make_TextureList(L"Sheep_LD");	}
	else if (_SKILLTYPE == PLAYER_SKILL::SKILL_3)	{ Make_TextureList(L"Chaos_Pulse");		}
	else if (_SKILLTYPE == PLAYER_SKILL::FAIRY_PULSE) { Make_TextureList(L"Fairy_Pulse"); }
	else if (_SKILLTYPE == PLAYER_SKILL::ICEARROW_PULSE) { Make_TextureList(L"IceArrow_Pulse"); }
	else if (_SKILLTYPE == PLAYER_SKILL::EVILHEAD_PULSE) { Make_TextureList(L"EvilHeadBow_Pulse"); }
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
	else if (_SKILLTYPE == PLAYER_SKILL::NPC_TIMESLOW_LOOP) { Make_TextureList(L"NPC_TimeSlow_Loof"); }
	else if (_SKILLTYPE == PLAYER_SKILL::SHADOW) { Make_TextureList(L"Shadow"); }
	else if (_SKILLTYPE == PLAYER_SKILL::BLUE_SHADER) { Make_TextureList(L""); }
	else if (_SKILLTYPE == PLAYER_SKILL::NPC_AREA) { Make_TextureList(L"NPC_Area"); }
	else if (_SKILLTYPE == PLAYER_SKILL::ICE_BOMB) { Make_TextureList(L"IceArrow_Bomb"); }
	else if (_SKILLTYPE == PLAYER_SKILL::WIND_SPIRIT) { Make_TextureList(L"Wind_Spirit"); }
	else if (_SKILLTYPE == PLAYER_SKILL::WIND_WINDY) { Make_TextureList(L"Wind_Windy"); }
	else if (_SKILLTYPE == PLAYER_SKILL::WIND_HITEFFECT) { Make_TextureList(L"Wind_HitEffect"); }
	else if (_SKILLTYPE == PLAYER_SKILL::WIND_PULSE) { Make_TextureList(L"Wind_Charge"); }
	else if (_SKILLTYPE == PLAYER_SKILL::WIND_CHARGING) { Make_TextureList(L"Wind_Charging"); }
	else if (_SKILLTYPE == PLAYER_SKILL::WIND_PULSE2) { Make_TextureList(L"IRA_Pulse"); }
	else if (_SKILLTYPE == PLAYER_SKILL::GREEN_SHADER) { Make_TextureList(L"GreenShader"); }
	else if (_SKILLTYPE == PLAYER_SKILL::ANERGYPAR) { Make_TextureList(L"AnergyPar"); }
	else if (_SKILLTYPE == PLAYER_SKILL::PAREND) { Make_TextureList(L"ParEnd"); }
	else if (_SKILLTYPE == PLAYER_SKILL::CHAOS_PULSE) { Make_TextureList(L"Chaos_Pulse"); }
	else if (_SKILLTYPE == PLAYER_SKILL::BLACKHOLE_DEATH) { Make_TextureList(L"Spr_Effect_ChaosPhase02Death_"); }
	else if (_SKILLTYPE == PLAYER_SKILL::SKILL2_HAND) { Make_TextureList(L"Skill2_Hand"); }
	else if (_SKILLTYPE == PLAYER_SKILL::IRA_HITEFFECT) { Make_TextureList(L"Ice_HitEffect"); }
	else if (_SKILLTYPE == PLAYER_SKILL::IRA_CHARGING) { Make_TextureList(L"IRA_Charging"); }
	else if (_SKILLTYPE == PLAYER_SKILL::IRA_CHARGED) { Make_TextureList(L"IRA_Charged"); }
	else if (_SKILLTYPE == PLAYER_SKILL::ATOMIC_BOW) { Make_TextureList(L"IRA_Charged"); }
	else if (_SKILLTYPE == PLAYER_SKILL::NPC_ATOMIC) { Make_TextureList(L"NPC_Atomic"); }
	else if (_SKILLTYPE == PLAYER_SKILL::ATOMIC_READY) { Make_TextureList(L"Atomic_Ready"); }
	else if (_SKILLTYPE == PLAYER_SKILL::ATOMIC_BOMB_EFFECT) { Make_TextureList(L"Atomic_Bomb_Effect"); }
	else if (_SKILLTYPE == PLAYER_SKILL::NPC_ATOMIC_CHARGED) { Make_TextureList(L"IRA_Charged"); }
	else if (_SKILLTYPE == PLAYER_SKILL::NPC_ATOMIC_CHARGED_END) { Make_TextureList(L"IRA_Charging"); }
	else if (_SKILLTYPE == PLAYER_SKILL::NPC_ATOMIC_AREA) { Make_TextureList(L"NPC_Atomic_Area"); }
	else if (_SKILLTYPE == PLAYER_SKILL::PLAYER_DASHEFFECT) { Make_TextureList(L"Player_DashEffect"); }
	else if (_SKILLTYPE == PLAYER_SKILL::SUMMON_EFFECT) { Make_TextureList(L"SpawnEffect"); }
	else if (_SKILLTYPE == PLAYER_SKILL::SPAWN_BOTTOM) { Make_TextureList(L"Spawn_bottom"); }


	if (_SKILLTYPE == PLAYER_SKILL::SHADOW_PARTNER) {
		Make_TextureList(player->Get_FileName());
	}

	{
		if (PosChase) {
			_playerPos = _PlayerPOS;
		}
		else {
			_effectPos = { _PlayerPOS->x, _PlayerPOS->y, _PlayerPOS->z };
			_playerPos = &_effectPos;
		}

		_effectSize = _Size;
		if (_SKILLTYPE == PLAYER_SKILL::ANERGYPAR) _effectSize.y *= 2.f;

		Repeatable = _Repeatable;
		_angleChase = PosChase;
		_effectTimer = 0.f;
		
		CameraObject* Camera = dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Camera"));
		_vec3 cameraDir = *(Camera->Get_EyeVec()) - *(Camera->Get_AtVec());
		D3DXVec3Normalize(&cameraDir, &cameraDir);

		_vec3 MouseDir = player->Get_MouseDir();

		_vec2 dir2D = { MouseDir.x, MouseDir.z };
		D3DXVec2Normalize(&dir2D, &dir2D);

		_angle = atan2f(-dir2D.y, dir2D.x);

		_vec3 eye = { 0.f, 0.f, 0.f };
		_vec3 at = cameraDir;
		_vec3 up = { 0.f, 1.f, 0.f };

		_matrix matSize;
		D3DXMatrixIdentity(&matSize);
		D3DXMatrixScaling(&matSize, 0.f, 0.f, 0.f);

		_matrix matBillboard;
		D3DXMatrixLookAtLH(&matBillboard, &eye, &at, &up);
		D3DXMatrixInverse(&matBillboard, nullptr, &matBillboard);

		float radian = D3DX_PI / 180.f;

		_matrix matRotX;

		_matrix matRotZ;
		D3DXMatrixRotationZ(&matRotZ, _angle - D3DX_PI);

		_matrix matRotY;
		D3DXMatrixRotationY(&matRotY, D3DX_PI);

		_matrix matWorld;
		matWorld = matSize * matRotY * matBillboard;

		if (_SKILLTYPE == PLAYER_SKILL::ATOMIC_BOMB_EFFECT) {
			D3DXMatrixRotationX(&matRotX, D3DXToRadian(90.f));
			matWorld = matSize * matRotY * matRotX;
		}
			

		matWorld._41 = (*_playerPos).x;
		matWorld._42 = (*_playerPos).y;
		matWorld._43 = (*_playerPos).z;

		Component_Transform->Set_World(&matWorld);

		if (_SKILLTYPE == PLAYER_SKILL::PAREND || _SKILLTYPE == PLAYER_SKILL::ATOMIC_BOMB_EFFECT
			|| _SKILLTYPE == PLAYER_SKILL::WIND_SPIRIT)
  			CollisionManager::GetInstance()->Add_ColliderObject(this);
	}

	ObjectTAG = L"Player_Effect";

	if (_SKILLTYPE == PLAYER_SKILL::NPC_TIMESLOW || _SKILLTYPE == PLAYER_SKILL::NPC_TIMESLOW_LOOP || _SKILLTYPE == PLAYER_SKILL::SHADOW_PARTNER)
		ObjectTAG = L"NPC_TIMESLOW";

	//CollisionManager::GetInstance()->Add_ColliderObject(this);
	_alphaRatio = 1.f;
	if (_SKILLTYPE == PLAYER_SKILL::ICE_SHADER || _SKILLTYPE == PLAYER_SKILL::BLUE_SHADER
		|| _SKILLTYPE == PLAYER_SKILL::GREEN_SHADER || _SKILLTYPE == PLAYER_SKILL::SHADOW_PARTNER
		|| _SKILLTYPE == PLAYER_SKILL::PLAYER_DASHEFFECT)
		_alphaRatio = 0.5f;
	else if (_SKILLTYPE == PLAYER_SKILL::NPC_ATOMIC)
		_alphaRatio = 0.f;

	PlayTime = _PlayTime;
	Critical = false;

	return S_OK;
}


HRESULT PlayerEffect::Make_TextureList(wstring _FileName) {
	INT FRAME = 0;
	while (++FRAME){
		wstring FileName = _FileName + to_wstring(FRAME) + L".png";
		if (SKILL_TYPE == PLAYER_SKILL::SHADOW_PARTNER) FileName = _FileName;
		IDirect3DBaseTexture9* TEX = ResourceManager::GetInstance()->Find_Texture(FileName.c_str());
		if (TEX == nullptr) break;
		else { TextureList.push_back(TEX); if (SKILL_TYPE == PLAYER_SKILL::SHADOW_PARTNER) break;}
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

	if (SKILL_TYPE == PLAYER_SKILL::BLUE_SHADER) {
		if (_effectTimer > 0.4f && _effectTimer < 2.2f) {
			_effectSize = {_effectSize.x + _DT * 80, _effectSize.y + _DT * 80, _effectSize.z + _DT * 80 };
		}
		else if(_effectTimer > 2.2f && _effectTimer < 4.f){
			_effectSize = { _effectSize.x - _DT * 80, _effectSize.y - _DT * 80, _effectSize.z - _DT * 80 };
		}
	}
	if (SKILL_TYPE == PLAYER_SKILL::NPC_TIMESLOW_LOOP) {
		if (_effectTimer > 2.2f) {
			_alphaRatio -= _DT / 3.f;
			_alphaRatio = max(_alphaRatio, 0.f);
		}
	}
	if (SKILL_TYPE == PLAYER_SKILL::GREEN_SHADER || SKILL_TYPE == PLAYER_SKILL::ICE_SHADER){
		if (_effectTimer < 0.1f) {
		_effectSize = { _effectSize.x - _DT * 0.3f, _effectSize.y - _DT * 0.3f, _effectSize.z - _DT * 0.3f };
		}
	}
	if (SKILL_TYPE == PLAYER_SKILL::SHADOW_PARTNER) {
		_alphaRatio -= _DT * 0.3;
		_alphaRatio = max(_alphaRatio, 0.f);
	}
	if (SKILL_TYPE == PLAYER_SKILL::NPC_ATOMIC) {
		if (_effectTimer < 0.5f) {
			_alphaRatio += _DT / 1.f;
			_alphaRatio = min(_alphaRatio, 1.f);
		}
		else if (_effectTimer > 1.f) {
			_alphaRatio -= _DT / 1.f;
			_alphaRatio = max(_alphaRatio, 0.f);
		}
		else _alphaRatio = 1.f;
	}
	if (SKILL_TYPE == PLAYER_SKILL::NPC_ATOMIC_CHARGED_END) {
			_alphaRatio -= _DT / 2.f;
			_alphaRatio = max(_alphaRatio, 0.f);
	}

	if (SKILL_TYPE == PLAYER_SKILL::SUMMON_EFFECT) {
		_effectPos.z -= _DT * 80.f;
	}
	//if (SKILL_TYPE == PLAYER_SKILL::SPAWN_BOTTOM) {
	//	_alphaRatio -= _DT * 2.f;
	//	_alphaRatio = max(_alphaRatio, 0.f);
	//}

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
	case PLAYER_SKILL::IRA_CHARGED:
		if (!(KEY_HOLD(DIK_SPACE)) || player->GetBowCharging() == 0) ObjectDead = true;
		break;
	case PLAYER_SKILL::IRA_CHARGING:
		if (!(KEY_HOLD(DIK_SPACE)) || player->GetBowCharging() != 0) ObjectDead = true;
		break;
	case PLAYER_SKILL::NPC_TIMESLOW_LOOP:
		if (_effectTimer > (*player->Get_SlowTime()) - 1.f ) ObjectDead = true;
		break;
	case PLAYER_SKILL::NPC_AREA:
		if (_effectTimer > (*player->Get_SlowTime()) - 0.5f) ObjectDead = true;
		break;
	case PLAYER_SKILL::NPC_ATOMIC:
		if (_effectTimer > 1.5f) ObjectDead = true;
		break;
	case PLAYER_SKILL::NPC_ATOMIC_CHARGED:
		if (_effectTimer > 1.3f) {
			_vec3 Size = { 1.f, 1.f, 1.f };
			PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::NPC_ATOMIC_CHARGED_END, Component_Transform->Get_Position(), 0.5f, Size, false);
			ObjectDead = true;
		}
		break;
	case PLAYER_SKILL::SUMMON_EFFECT:
		_vec3* playerPos = static_cast<Transform*>(player->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position();
		if (playerPos->z >= _effectPos.z) {
			ObjectDead = true;
			player->Set_Start_Effect_Dead(TRUE);
		}
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

		_matrix matRotX;
		D3DXMatrixRotationX(&matRotX, D3DXToRadian(80.f));

		_matrix matRotZ;
		D3DXMatrixRotationZ(&matRotZ, _angle - D3DX_PI);

		_matrix matWorld;
		if (_angleChase) matWorld = matSize * matRotZ * matBillboard;
		else matWorld = matSize * matRotX;

		matWorld._41 = (*_playerPos).x;
		matWorld._42 = (*_playerPos).y;
		matWorld._43 = (*_playerPos).z;

		if (SKILL_TYPE == PLAYER_SKILL::NPC_ATOMIC || SKILL_TYPE == PLAYER_SKILL::NPC_ATOMIC_CHARGED) {
			if (_effectTimer < 0.5)			matWorld._43 -= 1.f * _effectTimer;
			else							matWorld._43 -= 1.f * 0.5;
		}
		Component_Transform->Set_World(&matWorld);
		Component_Transform->Set_Pos({ matWorld._41 , matWorld._42 , matWorld._43 });
	}

	if (SKILL_TYPE == PLAYER_SKILL::PAREND ) {
		RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	}
	AlphaSorting(Component_Transform->Get_Position());

	return 0;
}
VOID PlayerEffect::LateUpdate_GameObject(CONST FLOAT& _DT) {
	GameObject::LateUpdate_GameObject(_DT);

	if (ObjectDead)	return ;
	if (FrameTick > PlayTime / ENDFRAME) {
		if (TextureIndex++ >= ENDFRAME - 2) {	
			if (Repeatable) { TextureIndex = 0;}
			else {
				TextureIndex = ENDFRAME - 2;
				ObjectDead = TRUE;
				CollisionManager::GetInstance()->Delete_ColliderObject(this);
			}
		}
		FrameTick = 0.f;
	}
	if (ObjectDead == TRUE) {
		if (SKILL_TYPE == PLAYER_SKILL::ATOMIC_READY) {
			_vec3 Size = { 7.f, 7.f, 7.f };
			PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::ATOMIC_BOMB_EFFECT, Component_Transform->Get_Position(), 1.f, Size, false);
			CameraObject* Camera = dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->
				Get_GameObject(L"Camera"));
			Camera->Camera_Shaking(10.f, 0.5f);
			SoundManager::GetInstance()->Play_Sound_Once(L"Bow/Wind_Bow/BossCav - Explosion.wav", CHANNELID::SOUND_EFFECT05, 0.5f);
		}
	}
		
}
VOID PlayerEffect::Render_GameObject() {
	if (ObjectDead)	return;
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//KJJ 03 06
	if (nullptr != dynamic_cast<MiniGameScene*>(SceneManager::GetInstance()->Get_CurrentScene())) {
		Monster::BillBoard(Component_Transform, GRPDEV, { 1.f,0.f,0.f},0);
	}

	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());


	DWORD tfactor = D3DCOLOR_ARGB(
		(BYTE)(_alphaRatio * 255.f),
		255, 255, 255
	);

	GRPDEV->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	GRPDEV->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GRPDEV->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GRPDEV->SetRenderState(D3DRS_TEXTUREFACTOR, tfactor);

	GRPDEV->SetTexture(0, TextureList[TextureIndex]);

	// COLOR = Texture * TFACTOR
	GRPDEV->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	GRPDEV->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	GRPDEV->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

	// ALPHA = TextureAlpha * TFACTORAlpha
	GRPDEV->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	GRPDEV->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	GRPDEV->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	
	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// 초기화
	GRPDEV->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);
	GRPDEV->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GRPDEV->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
}
BOOL PlayerEffect::OnCollisionEnter(GameObject* _Other) {
	wstring Tag = _Other->Get_ObjectTag();
	int hp = Component_Collider->Get_Hp();
	int atk = COLLIDER(_Other)->Get_Att();

	std::random_device rd;
	std::uniform_int_distribution<int> distribution(0, 100);

	Player* player = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Player"));

	int originATK = Component_Collider->Get_Att();

	bool _isCritical = false;
	if (distribution(rd) % 100 <= *player->Get_Critical()) {
		Component_Collider->Set_Att(originATK * 2.f);
		_isCritical = true;
	}

	if (Tag == L"Monster") {
		atk = 1.f;
		Component_Collider->Set_Hp(hp - atk);
		COLLIDER(_Other)->Set_Hp(COLLIDER(_Other)->Get_Hp() - Component_Collider->Get_Att());
		DamageFontManager::GetInstance()->Add_DamageFont(_Other, Component_Collider->Get_Att(), _isCritical);
		Component_Collider->Set_Att(originATK);
		return TRUE;
	}

	else if (Tag == L"CheonLog") {
		atk = 1.f;
		COLLIDER(_Other)->Set_Hp(COLLIDER(_Other)->Get_Hp() - Component_Collider->Get_Att());
		Component_Collider->Set_Hp(hp - atk);
		DamageFontManager::GetInstance()->Add_DamageFont(_Other, Component_Collider->Get_Att(), _isCritical);
		Component_Collider->Set_Att(originATK);
		return TRUE;
	}
	else if (_Other->Get_ObjectTag() == L"Docheol") {
		atk = 1.f;
		COLLIDER(_Other)->Set_Hp(COLLIDER(_Other)->Get_Hp() - Component_Collider->Get_Att());
		Component_Collider->Set_Hp(hp - atk);
		DamageFontManager::GetInstance()->Add_DamageFont(_Other, Component_Collider->Get_Att(), _isCritical);
		Component_Collider->Set_Att(originATK);

		return TRUE;
	}
	Component_Collider->Set_Att(originATK);

	return FALSE;
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

	if (SKILL_TYPE == PLAYER_SKILL::PAREND || SKILL_TYPE == PLAYER_SKILL::ATOMIC_BOMB_EFFECT
		|| SKILL_TYPE == PLAYER_SKILL::WIND_SPIRIT) {
		Component_Collider = ADD_COMPONENT_COLLIDER;
		Component_Collider->Set_CenterPos(Component_Transform);			// 충돌체가 오브젝트를 따라 다니도록

		if(SKILL_TYPE == PLAYER_SKILL::WIND_SPIRIT) Component_Collider->Set_Scale(3.f, 3.f, 3.f);
		else Component_Collider->Set_Scale(5.f, 5.f, 5.f);					// 충돌체의 범위 조절
		
		if(SKILL_TYPE == PLAYER_SKILL::WIND_SPIRIT) Component_Collider->Set_Att(43.f);
		else if(SKILL_TYPE == PLAYER_SKILL::PAREND)Component_Collider->Set_Att(60.f);
		else if (SKILL_TYPE == PLAYER_SKILL::ATOMIC_BOMB_EFFECT)Component_Collider->Set_Att(80.f);
	}


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
