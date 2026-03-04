#include "../Include/PCH.h"
#include "Bow.h"
#include <random>

Bow::Bow(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
Bow::Bow(const GameObject& _RHS) : GameObject(_RHS) {}
Bow::~Bow() {}

HRESULT Bow::Ready_GameObject()
{
	if (FAILED(Component_Initialize())) return E_FAIL;

	_lateReady = true;
	CameraObject* Camera = dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->
		Get_GameObject(L"Camera"));

	_cameraDir = *(Camera->Get_EyeVec()) - *(Camera->Get_AtVec());
	_vec3 planeDir = { 0.f, 1.f, 0.f };

	_cameraAngle = acosf(D3DXVec3Dot(D3DXVec3Normalize(&_cameraDir, &_cameraDir), D3DXVec3Normalize(&planeDir, &planeDir)));
	_cameraAngle = _cameraAngle / D3DX_PI * 180.f;

	_alphaRatio = 0.f;

	Component_Transform->Set_Scale({ 1.f, 1.f, 1.f });

	_Charging = 0;
	_attackDelay = 0.4;
	if (_type == BowType::WindBow) _attackDelay = 1.f;

	return S_OK;
}

INT Bow::Update_GameObject(const _float& _DT)
{
	if (_isDestroied) return -1;

	if (_lateReady) {
		Late_Ready();
		_lateReady = false;
	}

	if (_isEquip) {
		GameObject::Update_GameObject(_DT);

		Player* player = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Player"));
		ObjectTAG = L"Bow";
		_playerAtk = player->Get_Atk();
		_playerCritical = player->Get_Critical();
		_playerChargingSpeed = player->Get_ChargingSpeed();

		_Stat.maxArrow *= (player->Get_AddMaxArrow());
		_chargingTime = 2.f;
		_chargingTime *= *_playerChargingSpeed;

		float alphaSpeed = 3.f;

		bool mouseLB = KeyManager::GetInstance()->Get_MouseState(DIM_LB) & 0x80;

		_attackTimer += _DT;

		if (mouseLB || KEY_HOLD(DIK_SPACE)) {
			if (_alphaRatio < 1.f)
				_alphaRatio += _DT * alphaSpeed;
			if (_alphaRatio > 1.f)
				_alphaRatio = 1.f;
		}
		else
		{
			if (_alphaRatio > 0.f)
				_alphaRatio -= _DT * alphaSpeed;
			if (_alphaRatio < 0.f)
				_alphaRatio = 0.f;

			_ChargingTime = 0;
			_Charging = 0;
			_Charge = 0;
		}
		_vec3 MouseDir = player->Get_MouseDir();

		_vec2 dir2D = { MouseDir.x, MouseDir.z };
		D3DXVec2Normalize(&dir2D, &dir2D);

		float angle = atan2f(-dir2D.y, dir2D.x);

		float radius = 1.f;
		if (_type == BowType::FairyBow) radius = 1.5f;

		float offsetX = cosf(angle) * radius;
		float offsetY = sinf(angle) * radius;

		_vec3 eye = { 0.f, 0.f, 0.f };
		_vec3 at = _cameraDir;
		_vec3 up = { 0.f, 1.f, 0.f };

		_matrix matSize;
		D3DXMatrixIdentity(&matSize);
		D3DXMatrixScaling(&matSize, 1.0f, 2.0f, 1.0f);

		_matrix matBillboard;
		D3DXMatrixLookAtLH(&matBillboard, &eye, &at, &up);
		D3DXMatrixInverse(&matBillboard, nullptr, &matBillboard);

		float radian = D3DX_PI / 180.f;
		_matrix matRotZ;
		D3DXMatrixRotationZ(&matRotZ, angle - D3DX_PI);

		_matrix matWorld = matSize * matRotZ * matBillboard;

		matWorld._41 = (*_bowPos).x + offsetX;
		matWorld._42 = (*_bowPos).y;
		matWorld._43 = (*_bowPos).z - offsetY;

		Component_Transform->Set_World(&matWorld);
		Component_Transform->Set_Pos({ matWorld._41 , matWorld._42 , matWorld._43 });

		// 차징
		if (KEY_HOLD(DIK_SPACE)) {
			_ChargingTime += _DT;
			if (_ChargingTime > _chargingTime) _Charging++;
			if (_ChargingTime < _chargingTime) {
				if (_Charge++ == 0) {
					float pulseRadius = 1.f;
					if (_type == BowType::FairyBow) pulseRadius = 1.1f;
					else if (_type == BowType::IceBow) pulseRadius = 1.6f;
					else if (_type == BowType::EvilHeadBow) pulseRadius = 2.f;
					else if (_type == BowType::WindBow) pulseRadius = 1.6f;
					
					float offsetX = cosf(angle) * pulseRadius;
					float offsetY = sinf(angle) * pulseRadius;

					_pulsepos = { _bowPos->x + offsetX , _bowPos->y, _bowPos->z - offsetY };
					_vec3 Size = { 1.f, 1.f, 1.f };
					
					switch (_type) {
					case BowType::FairyBow :
						PLAY_PLAYER_EFFECT(PLAYER_SKILL::WIND_CHARGING, &_pulsepos, 0.9f, Size, true);
						break;
					case BowType::IceBow:
						PLAY_PLAYER_EFFECT(PLAYER_SKILL::ICE_CHARGE, &_pulsepos, 1.1f, Size, true);
						break;
					case BowType::EvilHeadBow:
						Size = { 1.5f, 1.5f, 1.5f };
						PLAY_PLAYER_EFFECT(PLAYER_SKILL::EVIL_CHARGE, &_pulsepos, 1.1f, Size, true);
						break;
					case BowType::WindBow:
						Size = { 1.f, 1.f, 1.f };
						PLAY_PLAYER_EFFECT(PLAYER_SKILL::IRA_CHARGING, &_pulsepos, 1.f, Size, true);
						break;
					}
				}
				_Charging = 0;
			}

			CreateChargingEffect(_DT);

			if (_ChargingTime > _chargingTime && MOUSE_LBUTTON) {
				CreateArrow(_DT);
				CreateEffect(_DT);
				_ChargingTime = 0.f;
				_Charge = 0;
				_Charging = 0;
			}
		}
		else {
			CreateArrow(_DT);
			CreateEffect(_DT);
		}

		RenderManager::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);
	}
	else
		_alphaRatio = 0.f;

	AlphaSorting(Component_Transform->Get_Position());
	
	return S_OK;
}

VOID Bow::LateUpdate_GameObject(const _float& _DT)
{
	if(_isEquip)
		GameObject::LateUpdate_GameObject(_DT);
}

VOID Bow::Render_GameObject()
{
	if (_isEquip) {
		GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

		SetGrahpic();

		Component_Buffer->Render_Buffer();

		GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

		// 초기화
		GRPDEV->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);
		GRPDEV->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		GRPDEV->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	}
	
	return VOID();
}

HRESULT Bow::Component_Initialize()
{
	Component_Transform = ADD_COMPONENT_TRANSFORM;
	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Texture = ADD_COMPONENT_TEXTURE;
	//Component_Texture->Import_TextureFromFolder(L"../../Resource/Weapon/IceBow");

	//D3DXCreateTextureFromFile(g_pd3dDevice, L"image.png", &g_pTexture);
	//D3DXCreateSprite(g_pd3dDevice, &g_pSprite);
	return S_OK;
}

void Bow::SetGrahpic()
{
	TCHAR FileName[128] = L"";

	switch (_type) {
	case BowType::FairyBow:
		wsprintfW(FileName, L"FairyBow.png");
		break;
	case BowType::IceBow:
		wsprintfW(FileName, L"IceBowL.png");
		break;
	case BowType::EvilHeadBow:
		wsprintfW(FileName, L"EvilHeadBow.png");
		break;
	case BowType::WindBow:
		wsprintfW(FileName, L"WindBow.png");
		break;
	}

	DWORD tfactor = D3DCOLOR_ARGB(
		(BYTE)(_alphaRatio * 255.f),
		255, 255, 255
	);

	GRPDEV->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	GRPDEV->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GRPDEV->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GRPDEV->SetRenderState(D3DRS_TEXTUREFACTOR, tfactor);

	GRPDEV->SetTexture(0, (ResourceManager::GetInstance()->Find_Texture(FileName)));
	
	// COLOR = Texture * TFACTOR
	GRPDEV->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	GRPDEV->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	GRPDEV->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

	// ALPHA = TextureAlpha * TFACTORAlpha
	GRPDEV->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	GRPDEV->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	GRPDEV->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
}

void Bow::CreateArrow(const _float& _DT)
{
	bool mouseLB = KeyManager::GetInstance()->Get_MouseState(DIM_LB) & 0x80;
	
	if (_type == BowType::WindBow && !mouseLB) {
		_attackDelay = 1.f;
	}
	if (_type == BowType::IceBow) {
		_attackDelay = 0.3f;
	}

	if (mouseLB)
	{
		if (_type == BowType::WindBow && _attackDelay > 0.2f) {
			_attackDelay -= _DT * 0.4;
		}
	
		if (_attackTimer > _attackDelay) {
			_Stat.curArrow -= 1;

			Player* player = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Player"));
			_vec3 MouseDir = player->Get_MouseDir();
	
			_vec2 dir2D = { MouseDir.x, MouseDir.z };
			D3DXVec2Normalize(&dir2D, &dir2D);
	
			float angle = atan2f(-dir2D.y, dir2D.x);
	
			float radius = 1.8f;
	
			float offsetX = cosf(angle) * radius;
			float offsetY = sinf(angle) * radius;
	
			_arrowPos = { _bowPos->x + offsetX , _bowPos->y, _bowPos->z - offsetY };
	
			_vec2 side = { dir2D.y, dir2D.x };
			D3DXVec2Normalize(&side, &side);
			_vec3 rightPos = _arrowPos;
			_vec3 leftPos = _arrowPos;
			float convergeAngle = D3DXToRadian(4.f);
			_vec2 rightDir = {
				cosf(angle + convergeAngle),
				-sinf(angle + convergeAngle)
			};
			
			_vec2 leftDir = {
				cosf(angle - convergeAngle),
				-sinf(angle - convergeAngle)
			};
	
			switch (_type)
			{
			case BowType::FairyBow :
				if (KEY_HOLD(DIK_SPACE)) {
					MakeArrow(_arrowPos, dir2D, true);
				}
				else
					MakeArrow(_arrowPos, dir2D);
				break;
			case BowType::IceBow:
				if (KEY_HOLD(DIK_SPACE)) {
					MakeArrow(_arrowPos, dir2D, true);
				}
				else{
					MakeArrow(_arrowPos, dir2D);
					rightPos.x -= side.x * 1.5f;
					rightPos.z += side.y * 1.5f;
					leftPos.x += side.x * 1.5f;
					leftPos.z -= side.y * 1.5f;
					MakeArrow(rightPos, rightDir);
					MakeArrow(leftPos, leftDir);
				}
				break;
			case BowType::EvilHeadBow:
				if (KEY_HOLD(DIK_SPACE)) {
					MakeArrow(_arrowPos, dir2D, true);
				}
				else {
					MakeArrow(_arrowPos, dir2D);
					MakeArrow(_arrowPos, dir2D);
				}
				break;
			case BowType::WindBow:
				if (KEY_HOLD(DIK_SPACE)) {
					MakeArrow(_arrowPos, dir2D, true);
				}
				else {
					MakeArrow(_arrowPos, dir2D);
				}
				break;
			default :
				MakeArrow(_arrowPos, dir2D);
	
			}
		}
	}

}

void Bow::CreateEffect(const _float& _DT)
{
	bool mouseLB = KeyManager::GetInstance()->Get_MouseState(DIM_LB) & 0x80;

	Player* player = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Player"));
	_vec3 MouseDir = player->Get_MouseDir();

	_vec2 dir2D = { MouseDir.x, MouseDir.z };
	D3DXVec2Normalize(&dir2D, &dir2D);

	float angle = atan2f(-dir2D.y, dir2D.x);

	float radius = 1.f;
	if (_type == BowType::FairyBow) radius = 1.1f;
	else if (_type == BowType::IceBow) radius = 1.6f;
	else if (_type == BowType::EvilHeadBow) radius = 1.0f;
	else if (_type == BowType::WindBow) {
		if (_attackDelay > 0.3f) radius = 1.6f;
		else radius = 2.5f;
	}

	float offsetX = cosf(angle) * radius;
	float offsetY = sinf(angle) * radius;

	_pulsepos = { _bowPos->x + offsetX , _bowPos->y, _bowPos->z - offsetY };

	// normal attack
	if (mouseLB)
	{
		// 이펙트
		if (_attackTimer > _attackDelay) {
			_vec3 Size = { 1.f, 1.f, 1.f };
			switch (_type)
			{
			case BowType::FairyBow :
				PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::FAIRY_PULSE, &_pulsepos, 0.2f, Size, true);
				SoundManager::GetInstance()->Play_Sound_Once(L"Bow/Fairy_Bow/Weapon_55_Fire.wav", CHANNELID::SOUND_EFFECT03, 0.2f);
				break;
			case BowType::IceBow:
				Size = { 1.5f, 1.5f, 1.5f };
				PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::ICEARROW_PULSE, &_pulsepos, 0.2f, Size, true);
				SoundManager::GetInstance()->Play_Sound_Once(L"Bow/Ice_Bow/Weapon_30_Ice_Fire_.mp3", CHANNELID::SOUND_EFFECT03, 0.2f);
				break;
			case BowType::EvilHeadBow:
				Size = { 2.f, 2.2f, 2.f };
				PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::EVILHEAD_PULSE, &_pulsepos, 0.5f, Size, true);
        SoundManager::GetInstance()->Play_Sound_Once(L"Bow/EvilHead_Bow/Weapon_51_1_Fire.wav", CHANNELID::SOUND_EFFECT03, 0.25f);
				break;
			case BowType::WindBow:
				if (_ChargingTime > _chargingTime) {
					Size = { 2.f, 2.f, 2.f };
					PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::WIND_PULSE2, &_pulsepos, 0.3f, Size, true);
					SoundManager::GetInstance()->Play_Sound_Once(L"Bow/Wind_Bow/Weapon_39_Fire.wav", CHANNELID::SOUND_EFFECT03, 0.25f);

				}
				else if (_attackDelay > 0.3f) {
					Size = { 1.5f, 1.5f, 1.5f };
					PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::WIND_PULSE, &_pulsepos, 0.3f, Size, true);
					SoundManager::GetInstance()->Play_Sound_Once(L"Bow/Wind_Bow/Weapon_39_Fire.wav", CHANNELID::SOUND_EFFECT03, 0.2f);

				}
				else {
					Size = { 1.f, 1.f, 1.f };
					PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::WIND_PULSE2, &_pulsepos, 0.3f, Size, true);
					SoundManager::GetInstance()->Play_Sound_Once(L"Bow/Wind_Bow/Weapon_39_Fire.wav", CHANNELID::SOUND_EFFECT03, 0.2f);
				}


				break;
			}
			_attackTimer = 0.f;
		}
	}
}

void Bow::CreateChargingArrow(const _float& _DT)
{
	POINT MousePoint{ 0, 0 };
	GetCursorPos(&MousePoint);
	ScreenToClient(hWnd, &MousePoint);

	Player* player = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Player"));
	_vec3 MouseDir = player->Get_MouseDir();

	_vec2 dir2D = { MouseDir.x, MouseDir.z };
	D3DXVec2Normalize(&dir2D, &dir2D);

	float angle = atan2f(-dir2D.y, dir2D.x);

	float radius = 1.8f;

	float offsetX = cosf(angle) * radius;
	float offsetY = sinf(angle) * radius;

	_arrowPos = { _bowPos->x + offsetX , _bowPos->y, _bowPos->z - offsetY };

	_vec2 side = { -dir2D.y, dir2D.x };
	D3DXVec2Normalize(&side, &side);
	_vec3 rightPos = _arrowPos;
	_vec3 leftPos = _arrowPos;

	MakeArrow(_arrowPos, dir2D, true);
	_Stat.curArrow -= 1;
}

void Bow::CreateChargingEffect(const _float& _DT)
{
	bool mouseLB = KeyManager::GetInstance()->Get_MouseState(DIM_LB) & 0x80;

	Player* player = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Player"));
	_vec3 MouseDir = player->Get_MouseDir();
	_vec3 playerPos = *(dynamic_cast<Transform*>(player->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position());

	_vec2 dir2D = { MouseDir.x, MouseDir.z };
	D3DXVec2Normalize(&dir2D, &dir2D);

	float angle = atan2f(-dir2D.y, dir2D.x);

	float radius = 1.f;
	if (_type == BowType::FairyBow) radius = 1.6f;
	else if (_type == BowType::IceBow) radius = 1.6f;
	else if (_type == BowType::EvilHeadBow) radius = 1.6f;
	else if (_type == BowType::WindBow) radius = 1.6f;

	float offsetX = cosf(angle) * radius;
	float offsetY = sinf(angle) * radius;

	_pulsepos = { _bowPos->x + offsetX , _bowPos->y, _bowPos->z - offsetY };

	_vec3 Size = { 1.f, 1.f, 1.f };
	if (_Charging == 1)
	{
		if (_ChargingTime > _chargingTime)
		{
			// 이펙트
			_vec3 Size = { 1.f, 1.f, 1.f };
			switch (_type)
			{
			case BowType::FairyBow:
        SoundManager::GetInstance()->Play_Sound_Once(L"Bow/Fairy_Bow/Weapon_55_Charge.wav", CHANNELID::SOUND_EFFECT03, 0.15f);
				PLAY_PLAYER_EFFECT(PLAYER_SKILL::FAIRY_CHARGING, &_pulsepos, 0.3f, Size, true);
				break;
			case BowType::IceBow:
				SoundManager::GetInstance()->Play_Sound_Once(L"Bow/Ice_Bow/Weapon_7_Charge.wav", CHANNELID::SOUND_EFFECT03, 0.15f);
				PLAY_PLAYER_EFFECT(PLAYER_SKILL::ICE_CHARGING, &_pulsepos, 0.3f, Size, true);
				break;
			case BowType::EvilHeadBow:
        SoundManager::GetInstance()->Play_Sound_Once(L"Bow/EvilHead_Bow/Weapon_51_Charge.wav", CHANNELID::SOUND_EFFECT03, 0.15f);
				PLAY_PLAYER_EFFECT(PLAYER_SKILL::EVIL_CHARGING, &_pulsepos, 0.3f, Size, true);
				break;
			case BowType::WindBow:
				SoundManager::GetInstance()->Play_Sound_Once(L"Bow/Wind_Bow/Weapon_ChargeComplete_Wind.wav", CHANNELID::SOUND_EFFECT03, 0.15f);
				Size = { 1.2f, 1.2f, 1.2f };
				PLAY_PLAYER_EFFECT(PLAYER_SKILL::IRA_CHARGED, &_pulsepos, 0.6f, Size, true);
				break;
			}
		}
	}

}

void Bow::Late_Ready()
{
	TCHAR txt[128] = L"";

	wstring name = L"";
	switch (_type) {
	case BowType::FairyBow:
		_Stat.bowLv = 1;
		_Stat.minAtk = 20;
		_Stat.maxAtk = 23;
		_Stat.maxArrow = 10000;
		_Stat.curArrow = 10000;
		_Stat.range = 10.f;
		_Stat.delay = 0.6f;
		_imgIDX = 0;
		break;
	case BowType::IceBow:
		_Stat.bowLv = 1;
		_Stat.minAtk = 20;
		_Stat.maxAtk = 23;
		_Stat.maxArrow = 180;
		_Stat.curArrow = 180;
		_Stat.range = 10.f;
		_Stat.delay = 0.6f;
		_imgIDX = 1;
		break;
	case BowType::EvilHeadBow:
		_Stat.bowLv = 1;
		_Stat.minAtk = 20;
		_Stat.maxAtk = 23;
		_Stat.maxArrow = 150;
		_Stat.curArrow = 150;
		_Stat.range = 10.f;
		_Stat.delay = 0.6f;
		_imgIDX = 2;
		break;
	case BowType::WindBow:
		_Stat.bowLv = 1;
		_Stat.minAtk = 20;
		_Stat.maxAtk = 23;
		_Stat.maxArrow = 180;
		_Stat.curArrow = 180;
		_Stat.range = 10.f;
		_Stat.delay = 0.6f;
		_imgIDX = 3;
		break;
	}
}

void Bow::MakeArrow(_vec3 pos, _vec2 arrowDir, bool charging)
{
	_arrowPos = pos;

	std::random_device rd;
	std::uniform_int_distribution<int> distribution(0, 100);

	int arrowAtk = distribution(rd) % (_Stat.maxAtk - _Stat.minAtk) + _Stat.minAtk + *_playerAtk;

	if (distribution(rd) % 100 < *_playerCritical) arrowAtk *= 2;

	GameObject* arrow = nullptr;

	if (charging) arrow = Arrow::Create(GRPDEV, _type, 3, arrowAtk, &_arrowPos, arrowDir);
	else arrow = Arrow::Create(GRPDEV, _type, _Stat.bowLv, arrowAtk, &_arrowPos, arrowDir);

	TCHAR arrowTag[128] = L"";
	wsprintfW(arrowTag, L"PlayerArrow_%d", _arrowCount++);

	arrow->Set_ObjectTag(arrowTag);
	arrow->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_PLAYER);
	arrow->Set_ObjectTag(L"PlayerArrow");

	SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(arrow);
}

Bow* Bow::Create(LPDIRECT3DDEVICE9 _GRPDEV)
{
	Bow* BOW = new Bow(_GRPDEV);
	if (FAILED(BOW->Ready_GameObject())) {
		MSG_BOX("Cannot Create BOW.");
		Safe_Release(BOW);
		return nullptr;
	}
	return BOW;
}

VOID Bow::Free()
{
	GameObject::Free();
}
