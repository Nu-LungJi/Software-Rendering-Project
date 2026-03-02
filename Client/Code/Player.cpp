#include "../Include/PCH.h"
#include "Player.h"

Player::Player(LPDIRECT3DDEVICE9 _GRPDEV)	: GameObject(_GRPDEV)	{}
Player::Player(const GameObject& _RHS)		: GameObject(_RHS)		{}
Player::~Player()													{}

HRESULT Player::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;

	//Temp
	Component_Collider->Set_Hp(5.f);
	Component_Collider->Set_Att(1.f);

	memset(_weaponSlot, 0, sizeof(Bow*) * 4);
	memset(_artifactSlot, 0, sizeof(GameObject*) * 4);
	memset(_inventory, 0, sizeof(GameObject*) * 10);
	_pState				= pState::STATE_IDLE;
	_eState				= eState::STATE_STANDING;
	_see				= pSee::SEE_DOWN;
	_skillState			= skillState::NONE;
	_defaultSpeed		= 6.f;
	_dashStart			= false;
	_dashTime			= 0.f;
	_dashG				= 30.f;
	_speed				= 0.f;
	_slideTime			= 0.f;
	_g					= 30.f;
	_frame				= 1;
	_arrowCount			= 0;
	_isStop				= false;
	_skillTimer			= 0.f;
	_animSpeed			= 1.f;
	_isInvincible		= false;
	_invincibleTimer	= 0.f;
	_alphaRatio			= 1.f;
	_alphaDelayTimer	= 0.f;
	_partnerTimer		= 0.f;
	_Skill2				= false;

	// UI
	Component_Collider->Set_Hp(50.f);
	Component_Collider->Set_Att(1.f);
	_dashstock			= 3;
	_key				= 0;
	_coin				= 0;
	_crystal			= 0;
	_token				= 2;
	_atk				= 1;
	_critical			= 0;
	_chargingSpeed		= 0.3f;
	_range				= 1.f;
	_arrowSize			= 1.f;
	_arrowSpeed			= 1.f;
	_slowTime			= 4.f;
	_hit_inv_Time		= 2.f;
	_dash_inv_Time		= 2.f;
	_MaxArrow			= 1.f;
	//연출용
	CameraMove = false;

	CameraObject* Camera = dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->
		Get_GameObject(L"Camera"));

	_vec3 cameraDir = *(Camera->Get_EyeVec()) - *(Camera->Get_AtVec());
	_vec3 planeDir = { 0.f, 1.f, 0.f };
	
	_float angle = acosf(D3DXVec3Dot(D3DXVec3Normalize(&cameraDir, &cameraDir), D3DXVec3Normalize(&planeDir, &planeDir)));
	_cameraAngle = angle / D3DX_PI * 180.f;

	Component_Transform->Set_Scale({ 2.f, 2.f, 2.f });
	Component_Transform->Rotation(ROT_X, 90.f - _cameraAngle);
	//Component_Transform->Set_Pos({ 5.f, 0.5f, 5.f });
	Component_Transform->Set_Pos({  28.814f, 0.5f, 34.78f }); // 광윤 디버깅용
	// 활 생성
	{
		SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<Bow>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_PLAYER, L"FairyBow");
		dynamic_cast<Bow*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"FairyBow"))->Set_PlayerPos(Component_Transform->Get_Position());
		_weaponSlot[0] = dynamic_cast<Bow*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"FairyBow"));
		_weaponSlot[0]->Set_Bow_Type(BowType::FairyBow);
		_weaponSlot[0]->Set_Bow_Equip(true);

		SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<Bow>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_PLAYER, L"IceBow");
		dynamic_cast<Bow*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"IceBow"))->Set_PlayerPos(Component_Transform->Get_Position());
		_weaponSlot[1] = dynamic_cast<Bow*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"IceBow"));
		_weaponSlot[1]->Set_Bow_Type(BowType::IceBow);
		_weaponSlot[1]->Set_Bow_Equip(false);

		SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<Bow>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_PLAYER, L"EvilHeadBow");
		dynamic_cast<Bow*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"EvilHeadBow"))->Set_PlayerPos(Component_Transform->Get_Position());
		_weaponSlot[2] = dynamic_cast<Bow*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"EvilHeadBow"));
		_weaponSlot[2]->Set_Bow_Type(BowType::EvilHeadBow);
		_weaponSlot[2]->Set_Bow_Equip(false);

		SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<Bow>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_PLAYER, L"WindBow");
		dynamic_cast<Bow*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"WindBow"))->Set_PlayerPos(Component_Transform->Get_Position());
		_weaponSlot[3] = dynamic_cast<Bow*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"WindBow"));
		_weaponSlot[3]->Set_Bow_Type(BowType::WindBow);
		_weaponSlot[3]->Set_Bow_Equip(false);

		SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<Bow>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_PLAYER, L"IceBow2");
		dynamic_cast<Bow*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"IceBow2"))->Set_PlayerPos(Component_Transform->Get_Position());
		_inventory[0] = dynamic_cast<Bow*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"IceBow2"));
		dynamic_cast<Bow*>(_inventory[0])->Set_Bow_Type(BowType::IceBow);
		dynamic_cast<Bow*>(_inventory[0])->Set_Bow_Equip(false);
	}

	CollisionManager::GetInstance()->Add_ColliderObject(this);
	Debug = false;

	return MiniGameInit();
}

INT	Player::Update_GameObject(const _float& _DT) {
	GameObject::Update_GameObject(_DT);

	_vec3 pPos = *Component_Transform->Get_Position();
	pPos.y = 0.5f;
	Component_Transform->Set_Pos(pPos);

	if (_isStop) return S_OK;

	Component_Transform->Set_Scale(2.5f, 2.5f, 2.5f);

	if (Component_Collider->Get_Hp() <= 0 && _eState != eState::STATE_DEAD) {
		_frame = 1;
		_pState = pState::STATE_DEATH;
		_weaponSlot[_equipNum]->Set_Bow_Equip(false);
	}

	if(!_isInvincible) _alphaRatio = 1.f;

	// 피격무적
	if ( _isInvincible && _pState != pState::STATE_DEATH && _pState != pState::STATE_DASH) {
		_invincibleTimer += _DT;
		_alphaDelayTimer += _DT;
		if (_invincibleTimer > _hit_inv_Time) {
			_invincibleTimer = 0.f;
			_alphaDelayTimer = 0.f;
			_isInvincible = false;
		}
		if (_alphaDelayTimer < 0.1f) {
			_alphaRatio -= _DT * 6;
			_alphaRatio = max(_alphaRatio, 0.f);
		}
		else if (_alphaDelayTimer > 0.1f && _alphaDelayTimer < 0.2f) {
			_alphaRatio += _DT * 6;
			_alphaRatio = min(_alphaRatio, 1.f);
		}
		else _alphaDelayTimer = 0.f;
	}

	if (Component_Collider->Get_Hp() <= 0 && _eState != eState::STATE_DEAD) {
		_frame = 1;
		_pState = pState::STATE_DEATH;
	}
	//SetOnTerrain(); - 광윤 디버그

	_frameTick += _DT;
	if (_isStop) return 0;

	// 대시 리필
	if (_dashstock == 3)
		_dashRefillTimer = 0.f;
	else {
		_dashRefillTimer += _DT;
		if (_dashRefillTimer > 3.f) {
			_dashstock++;
			_dashRefillTimer = 0.f;
		}
	}

	// 스킬

	switch (_skillState) {
	case skillState::STATE_TIMESLOW :
		SKILL_TIMESLOW(_DT);
		break;
	case skillState::STATE_ATOMIC:
		SKILL_ATOMIC(_DT);
		break;
	case skillState::NONE :
		SKILL_NONE(_DT);
		break;
	}
	switch (_pState)
	{
	case pState::STATE_IDLE:
		Idle_Final_Input(_DT);
		IDLE_STATE(_DT);
		break;
	case pState::STATE_DASH:
		DASH_STATE(_DT);
		break;
	case pState::STATE_ATTACK:
		ATTACK_STATE(_DT);
		break;
	case pState::STATE_LANDING :
		LANDING_STATE(_DT);
		break;
	case pState::STATE_DEATH:
		DEATH_STATE(_DT);
		break;
	default:
		break;
	}

	if (KEY_DOWN(DIK_Y)) {
		GameObject* DamageFont = nullptr;

		DamageFont = DamageFont::Create(GRPDEV, 20, Component_Transform->Get_Position());
		DamageFont->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_UI);
		DamageFont->Set_ObjectTag(L"DamageFont");

		SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(DamageFont);

	}

	AlphaSorting(Component_Transform->Get_Position());
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return S_OK;
}
VOID Player::LateUpdate_GameObject(const _float& _DT) {
	GameObject::LateUpdate_GameObject(_DT);

	//if (m_eCurrScene == SCENE_TYPE::Minigame) 
	//	AlphaZValue = Monster::BillBoard(Component_Transform, GRPDEV);
	CheonLog_Spawn();

	if (_isStop) return;
	Set_Effect(_DT);
}
VOID Player::Render_GameObject() {
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
HRESULT Player::Component_Initialize() {
	Component_Buffer	= ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;
	Component_Texture	= ADD_COMPONENT_TEXTURE;
	//Component_FSM		= ADD_COMPONENT_FSM;

	Component_Collider = ADD_COMPONENT_COLLIDER;					// 충돌체 컴포넌트 추가
	Component_Collider->Set_CenterPos(Component_Transform);			// 충돌체가 오브젝트를 따라 다니도록
	Component_Collider->Set_Scale(0.3f, 0.5f, 0.3f);				// 충돌체의 범위 조절
	Component_Collider->Set_Hp(5.f);
	Component_Collider->Set_Att(1.f);

	Component_Texture->Import_TextureFromFolder(L"../../Resource/Player/Stand");
	Component_Texture->Import_TextureFromFolder(L"../../Resource/Player/Run");
	Component_Texture->Import_TextureFromFolder(L"../../Resource/Player/Slide");
	Component_Texture->Import_TextureFromFolder(L"../../Resource/Player/Attack");
	Component_Texture->Import_TextureFromFolder(L"../../Resource/Player/NewDash");
	Component_Texture->Import_TextureFromFolder(L"../../Resource/Player/Death");
	Component_Texture->Import_TextureFromFolder(L"../../Resource/Player/Landing");

	return S_OK;
}
void Player::Reset()
{
	_defaultSpeed = 6.f;
	_dashStart = false;
	_dashTime = 0.f;
	_dashG = 30.f;
	_speed = 0.f;
	_slideTime = 0.f;
	_g = 30.f;
	_frame = 1;
	_arrowCount = 0;
	_isStop = false;
	_isInvincible = false;

	// UI
	//Component_Collider->Set_Hp(5);
	_dashstock			= 3;
	_key				= 0;
	_coin				= 0;
	_crystal			= 0;
	_token				= 2;
	_equipNum			= 0; // 지금 장착한 무기 idx
	_atk				= 1;
	_critical			= 0;
	_chargingSpeed		= 1.f;
	_range				= 1.f;
	_arrowSize			= 1.f;
	_arrowSpeed			= 1.f;
	_MaxArrow			= 1.f;
	_hit_inv_Time		= 2.f;
	_dash_inv_Time		= 2.f;

	MainUI* mainUI = dynamic_cast<MainUI*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"MainUI"));
	mainUI->Player_ReFillHP(5);

	for (int i = 1; i < 4; i++) {
		if (_weaponSlot[i] != nullptr) {
			_weaponSlot[i]->Set_Destroy();
			_weaponSlot[i] = nullptr;
		}
	}
}
void Player::IDLE_STATE(const _float& _DT)
{
	if (KEY_DOWN(DIK_F3)) {	//	마우스 커서 고정 여부 TRUE = 고정, FALSE = 고정 해제
		Debug ? Debug = FALSE : Debug = TRUE;
	}
	if (!Debug)
	{
		_vec3		upDir, rightDir;
		upDir = { 0.f, 0.f, 1.f };
		rightDir = { 1.f, 0.f, 0.f };
		D3DXVec3Normalize(&upDir, &upDir);
		D3DXVec3Normalize(&rightDir, &rightDir);

		if (_speed == 0.f)
		{
			if (_eState != eState::STATE_STANDING)
			{
				_eState = eState::STATE_STANDING;
			}
		}

		//bool mouseLB = KeyManager::GetInstance()->Get_MouseState(DIM_LB) & 0x80;

		//if (!mouseLB && _frame > 8)
		//	_frame = 1;

		// 장비 스왑
		if (KEY_DOWN(DIK_1)) {
			if (_weaponSlot[0] != nullptr) {
				if (_equipNum != 0) {
					_weaponSlot[_equipNum]->Set_Bow_Equip(false);
					_equipNum = 0;
					_weaponSlot[_equipNum]->Set_Bow_Equip(true);
				}
			}
		}
		else if (KEY_DOWN(DIK_2)) {
			if (_weaponSlot[1] != nullptr) {
				if (_equipNum != 1) {
					_weaponSlot[_equipNum]->Set_Bow_Equip(false);
					_equipNum = 1;
					_weaponSlot[_equipNum]->Set_Bow_Equip(true);
				}
			}
		}
		else if (KEY_DOWN(DIK_3)) {
			if (_weaponSlot[2] != nullptr) {
				if (_equipNum != 2) {
					_weaponSlot[_equipNum]->Set_Bow_Equip(false);
					_equipNum = 2;
					_weaponSlot[_equipNum]->Set_Bow_Equip(true);
				}
			}
		}
		else if (KEY_DOWN(DIK_4)) {
			if (_weaponSlot[3] != nullptr) {
				if (_equipNum != 3) {
					_weaponSlot[_equipNum]->Set_Bow_Equip(false);
					_equipNum = 3;
					_weaponSlot[_equipNum]->Set_Bow_Equip(true);
				}
			}
		}

		if (KEY_HOLD(DIK_W) && KEY_HOLD(DIK_A))
		{
			if (_eState != eState::STATE_RUN_LU) {
				_eState = eState::STATE_RUN_LU;
				_see = pSee::SEE_LU;
			}
			_speed = _defaultSpeed;
			_speed = _speed * cos(D3DX_PI * 0.25f);
			Component_Transform->Move_Pos(D3DXVec3Normalize(&upDir, &upDir), _speed, _DT);
			Component_Transform->Move_Pos(D3DXVec3Normalize(&rightDir, &rightDir), -_speed, _DT);
		}
		else if (KEY_HOLD(DIK_S) && KEY_HOLD(DIK_A))
		{
			if (_eState != eState::STATE_RUN_LD) {
				_eState = eState::STATE_RUN_LD;
				_see = pSee::SEE_LD;
			}
			_speed = _defaultSpeed;
			_speed = _speed * cos(D3DX_PI * 0.25f);
			Component_Transform->Move_Pos(D3DXVec3Normalize(&upDir, &upDir), -_speed, _DT);
			Component_Transform->Move_Pos(D3DXVec3Normalize(&rightDir, &rightDir), -_speed, _DT);

		}
		else if (KEY_HOLD(DIK_W) && KEY_HOLD(DIK_D))
		{
			if (_eState != eState::STATE_RUN_RU) {
				_eState = eState::STATE_RUN_RU;
				_see = pSee::SEE_RU;
			}
			_speed = _defaultSpeed;
			_speed = _speed * cos(D3DX_PI * 0.25f);
			Component_Transform->Move_Pos(D3DXVec3Normalize(&upDir, &upDir), _speed, _DT);
			Component_Transform->Move_Pos(D3DXVec3Normalize(&rightDir, &rightDir), _speed, _DT);

		}
		else if (KEY_HOLD(DIK_S) && KEY_HOLD(DIK_D))
		{
			if (_eState != eState::STATE_RUN_RD) {
				_eState = eState::STATE_RUN_RD;
				_see = pSee::SEE_RD;
			}
			_speed = _defaultSpeed;
			_speed = _speed * cos(D3DX_PI * 0.25f);
			Component_Transform->Move_Pos(D3DXVec3Normalize(&upDir, &upDir), -_speed, _DT);
			Component_Transform->Move_Pos(D3DXVec3Normalize(&rightDir, &rightDir), _speed, _DT);

		}
		else if (KEY_HOLD(DIK_W))
		{
			if (_eState != eState::STATE_RUN_UP) {
				_eState = eState::STATE_RUN_UP;
				_see = pSee::SEE_UP;
			}
			_speed = _defaultSpeed;
			Component_Transform->Move_Pos(D3DXVec3Normalize(&upDir, &upDir), _speed, _DT);
		}

		else if (KEY_HOLD(DIK_S))
		{
			if (_eState != eState::STATE_RUN_DOWN) {
				_eState = eState::STATE_RUN_DOWN;
				_see = pSee::SEE_DOWN;
			}
			_speed = _defaultSpeed;
			Component_Transform->Move_Pos(D3DXVec3Normalize(&upDir, &upDir), -_speed, _DT);
		}

		else if (KEY_HOLD(DIK_A))
		{
			if (_eState != eState::STATE_RUN_LEFT) {
				_eState = eState::STATE_RUN_LEFT;
				_see = pSee::SEE_LEFT;
			}
			_speed = _defaultSpeed;
			Component_Transform->Move_Pos(D3DXVec3Normalize(&rightDir, &rightDir), -_speed, _DT);
		}
		else if (KEY_HOLD(DIK_D))
		{
			if (_eState != eState::STATE_RUN_RIGHT) {
				_eState = eState::STATE_RUN_RIGHT;
				_see = pSee::SEE_RIGHT;
			}
			_speed = _defaultSpeed;
			Component_Transform->Move_Pos(D3DXVec3Normalize(&rightDir, &rightDir), _speed, _DT);
		}
		else
		{
			if (_speed > 0.f)
			{
				if (_slideTime < 0.001f && _speed <= _defaultSpeed * 0.5) {
					_speed = 0.f; _slideTime = 0.f;
				}

				_slideTime += _DT;
				_speed -= _DT * _g;
				if (_speed < 0.f){
					_speed = 0.f;
					_slideTime = 0.f;
				}
					
				float tempSpeed = _speed;
				switch (_see)
				{
				case pSee::SEE_LEFT :
					_eState = eState::STATE_STANDING;
					Component_Transform->Move_Pos(D3DXVec3Normalize(&rightDir, &rightDir), -_speed, _DT);
					break;
				case pSee::SEE_RIGHT:
					_eState = eState::STATE_STANDING;
					Component_Transform->Move_Pos(D3DXVec3Normalize(&rightDir, &rightDir), _speed, _DT);
					break;
				case pSee::SEE_UP:
					_eState = eState::STATE_STANDING;
					Component_Transform->Move_Pos(D3DXVec3Normalize(&upDir, &upDir), _speed, _DT);
					break;
				case pSee::SEE_DOWN:
					_eState = eState::STATE_STANDING;
					Component_Transform->Move_Pos(D3DXVec3Normalize(&upDir, &upDir), -_speed, _DT);
					break;
				case pSee::SEE_LU:
					_eState = eState::STATE_STANDING;
					tempSpeed = tempSpeed * cos(D3DX_PI * 0.25f);
					Component_Transform->Move_Pos(D3DXVec3Normalize(&rightDir, &rightDir), -tempSpeed, _DT);
					Component_Transform->Move_Pos(D3DXVec3Normalize(&upDir, &upDir), tempSpeed, _DT);
					break;
				case pSee::SEE_RU:
					_eState = eState::STATE_STANDING;
					tempSpeed = tempSpeed * cos(D3DX_PI * 0.25f);
					Component_Transform->Move_Pos(D3DXVec3Normalize(&rightDir, &rightDir), tempSpeed, _DT);
					Component_Transform->Move_Pos(D3DXVec3Normalize(&upDir, &upDir), tempSpeed, _DT);
					break;
				case pSee::SEE_LD:
					_eState = eState::STATE_STANDING;
					tempSpeed = tempSpeed * cos(D3DX_PI * 0.25f);
					Component_Transform->Move_Pos(D3DXVec3Normalize(&rightDir, &rightDir), -tempSpeed, _DT);
					Component_Transform->Move_Pos(D3DXVec3Normalize(&upDir, &upDir), -tempSpeed, _DT);
					break;
				case pSee::SEE_RD:
					_eState = eState::STATE_STANDING;
					tempSpeed = tempSpeed * cos(D3DX_PI * 0.25f);
					Component_Transform->Move_Pos(D3DXVec3Normalize(&rightDir, &rightDir), tempSpeed, _DT);
					Component_Transform->Move_Pos(D3DXVec3Normalize(&upDir, &upDir), -tempSpeed, _DT);
					break;
				default:
					_speed = 0.f;
					break;
				}
			}
		}
	}
}
void Player::DASH_STATE(const _float& _DT)
{
	_vec3		upDir, rightDir;
	upDir = { 0.f, 0.f, 1.f };
	rightDir = { 1.f, 0.f, 0.f };
	D3DXVec3Normalize(&upDir, &upDir);
	D3DXVec3Normalize(&rightDir, &rightDir);

	_dashTime += _DT;
	_isInvincible = true;

	_weaponSlot[_equipNum]->Set_Bow_Equip(false);

	if (_dashStart)
	{
		if (KEY_HOLD(DIK_W) && KEY_HOLD(DIK_A)) {
			_eState = eState::STATE_DASH_LU;
			_see = pSee::SEE_LU;
		}
		else if (KEY_HOLD(DIK_W) && KEY_HOLD(DIK_D)) {
			_eState = eState::STATE_DASH_RU;
			_see = pSee::SEE_RU;
		}
		else if (KEY_HOLD(DIK_S) && KEY_HOLD(DIK_A)) {
			_eState = eState::STATE_DASH_LD;
			_see = pSee::SEE_LD;
		}
		else if (KEY_HOLD(DIK_S) && KEY_HOLD(DIK_D)) {
			_eState = eState::STATE_DASH_RD;
			_see = pSee::SEE_RD;
		}
		else if (KEY_HOLD(DIK_A))
		{
			_eState = eState::STATE_DASH_LEFT;
			_see = pSee::SEE_LEFT;
		}
		else if (KEY_HOLD(DIK_D))
		{
			_eState = eState::STATE_DASH_RIGHT;
			_see = pSee::SEE_RIGHT;
		}
		else if (KEY_HOLD(DIK_W))
		{
			_eState = eState::STATE_DASH_UP;
			_see = pSee::SEE_UP;
		}
		else if (KEY_HOLD(DIK_S))
		{
			_eState = eState::STATE_DASH_DOWN;
			_see = pSee::SEE_DOWN;
		}
		else {
			_eState = eState::STATE_DASH_DOWN;
			_see = pSee::SEE_DOWN;
		}
		_speed = _defaultSpeed + 15.f;
		_dashStart = false;
	}

	_speed -= _dashG * _DT;
	float tempSpeed = _speed;
	switch (_eState)
	{
	case eState::STATE_DASH_LU:
		tempSpeed = _speed * cos(D3DX_PI * 0.25f);
		Component_Transform->Move_Pos(D3DXVec3Normalize(&upDir, &upDir), tempSpeed, _DT);
		Component_Transform->Move_Pos(D3DXVec3Normalize(&rightDir, &rightDir), -tempSpeed, _DT);
		break;
	case eState::STATE_DASH_RU:
		tempSpeed = _speed * cos(D3DX_PI * 0.25f);
		Component_Transform->Move_Pos(D3DXVec3Normalize(&upDir, &upDir), tempSpeed, _DT);
		Component_Transform->Move_Pos(D3DXVec3Normalize(&rightDir, &rightDir), tempSpeed, _DT);
		break;
	case eState::STATE_DASH_LD:
		tempSpeed = _speed * cos(D3DX_PI * 0.25f);
		Component_Transform->Move_Pos(D3DXVec3Normalize(&upDir, &upDir), -tempSpeed, _DT);
		Component_Transform->Move_Pos(D3DXVec3Normalize(&rightDir, &rightDir), -tempSpeed, _DT);
		break;
	case eState::STATE_DASH_RD:
		tempSpeed = _speed * cos(D3DX_PI * 0.25f);
		Component_Transform->Move_Pos(D3DXVec3Normalize(&upDir, &upDir), -tempSpeed, _DT);
		Component_Transform->Move_Pos(D3DXVec3Normalize(&rightDir, &rightDir), tempSpeed, _DT);
		break;
	case eState::STATE_DASH_UP :
		Component_Transform->Move_Pos(D3DXVec3Normalize(&upDir, &upDir), _speed, _DT);
		break;
	case eState::STATE_DASH_DOWN :
		Component_Transform->Move_Pos(D3DXVec3Normalize(&upDir, &upDir), -_speed, _DT);
		break;
	case eState::STATE_DASH_LEFT :
		Component_Transform->Move_Pos(D3DXVec3Normalize(&rightDir, &rightDir), -_speed, _DT);
		break;
	case eState::STATE_DASH_RIGHT :
		Component_Transform->Move_Pos(D3DXVec3Normalize(&rightDir, &rightDir), _speed, _DT);
		break;

	default:
		break;
	}

	if (_speed < 0.f)
	{
		_pState = pState::STATE_IDLE;
		_speed = 0;
		_dashTime = 0.f;
		_weaponSlot[_equipNum]->Set_Bow_Equip(true);
		if (_invincibleTimer == 0.f) _isInvincible = false;
	}
		
}
void Player::ATTACK_STATE(const _float& _DT)
{
	bool mouseLB = KeyManager::GetInstance()->Get_MouseState(DIM_LB) & 0x80;
	_attackDelay += _DT;

	_vec3		upDir, rightDir;
	upDir = { 0.f, 0.f, 1.f };
	rightDir = { 1.f, 0.f, 0.f };
	D3DXVec3Normalize(&upDir, &upDir);
	D3DXVec3Normalize(&rightDir, &rightDir);

	POINT point;
	GetCursorPos(&point);
	ScreenToClient(hWnd, &point);
	if (MOUSE_RBUTTON && _dashstock > 0 ) {
		_pState = pState::STATE_DASH;
		_dashStart = true;
		_frame = 1;
		_dashstock--;
		_weaponSlot[_equipNum]->Set_Bow_Equip(false);
		_isInvincible = true;
	}
	else if (!mouseLB && !KEY_HOLD(DIK_SPACE)) {
		_pState = pState::STATE_IDLE;
	}

	_vec2 playerPos = { WINCX / 2 , WINCY / 2 };

	if (point.x <= playerPos.x && point.y >= playerPos.y) {
		_see = pSee::SEE_LD;
		_eState = eState::STATE_ATTACK_LD;
	}
	else if (point.x > playerPos.x && point.y > playerPos.y) {
		_see = pSee::SEE_RD;
		_eState = eState::STATE_ATTACK_RD;
	}
	else if (point.x < playerPos.x && point.y < playerPos.y) {
		_see = pSee::SEE_LU;
		_eState = eState::STATE_ATTACK_LU;
	}
	else if (point.x >= playerPos.x && point.y <= playerPos.y) {
		_see = pSee::SEE_RU;
		_eState = eState::STATE_ATTACK_RU;
	}

	_speed = _defaultSpeed * 0.5;
	float tempSpeed = _speed;
	if (KEY_HOLD(DIK_W) && KEY_HOLD(DIK_A))
	{
		switch (_eState)
		{
		case eState::STATE_ATTACK_LU:
			_eState = eState::STATE_ATTACK_RUN_LU;
			break;
		case eState::STATE_ATTACK_RU:
			_eState = eState::STATE_ATTACK_RUN_BACK_RU;
			break;
		case eState::STATE_ATTACK_LD:
			_eState = eState::STATE_ATTACK_RUN_LD;
			break;
		case eState::STATE_ATTACK_RD:
			_eState = eState::STATE_ATTACK_RUN_BACK_RD;
			break;
		}
		tempSpeed = _speed * cos(D3DX_PI * 0.25f);
		Component_Transform->Move_Pos(D3DXVec3Normalize(&upDir, &upDir), tempSpeed, _DT);
		Component_Transform->Move_Pos(D3DXVec3Normalize(&rightDir, &rightDir), -tempSpeed, _DT);
	}
	else if (KEY_HOLD(DIK_S) && KEY_HOLD(DIK_A))
	{
		switch (_eState)
		{
		case eState::STATE_ATTACK_LU:
			_eState = eState::STATE_ATTACK_RUN_LU;
			break;
		case eState::STATE_ATTACK_RU:
			_eState = eState::STATE_ATTACK_RUN_BACK_RU;
			break;
		case eState::STATE_ATTACK_LD:
			_eState = eState::STATE_ATTACK_RUN_LD;
			break;
		case eState::STATE_ATTACK_RD:
			_eState = eState::STATE_ATTACK_RUN_BACK_RD;
			break;
		}
		tempSpeed = _speed * cos(D3DX_PI * 0.25f);
		Component_Transform->Move_Pos(D3DXVec3Normalize(&upDir, &upDir), -tempSpeed, _DT);
		Component_Transform->Move_Pos(D3DXVec3Normalize(&rightDir, &rightDir), -tempSpeed, _DT);

	}
	else if (KEY_HOLD(DIK_W) && KEY_HOLD(DIK_D))
	{
		switch (_eState)
		{
		case eState::STATE_ATTACK_LU:
			_eState = eState::STATE_ATTACK_RUN_BACK_LU;
			break;
		case eState::STATE_ATTACK_RU:
			_eState = eState::STATE_ATTACK_RUN_RU;
			break;
		case eState::STATE_ATTACK_LD:
			_eState = eState::STATE_ATTACK_RUN_BACK_LD;
			break;
		case eState::STATE_ATTACK_RD:
			_eState = eState::STATE_ATTACK_RUN_RD;
			break;
		}
		tempSpeed = _speed * cos(D3DX_PI * 0.25f);
		Component_Transform->Move_Pos(D3DXVec3Normalize(&upDir, &upDir), tempSpeed, _DT);
		Component_Transform->Move_Pos(D3DXVec3Normalize(&rightDir, &rightDir), tempSpeed, _DT);

	}
	else if (KEY_HOLD(DIK_S) && KEY_HOLD(DIK_D))
	{
		switch (_eState)
		{
		case eState::STATE_ATTACK_LU:
			_eState = eState::STATE_ATTACK_RUN_BACK_LU;
			break;
		case eState::STATE_ATTACK_RU:
			_eState = eState::STATE_ATTACK_RUN_RU;
			break;
		case eState::STATE_ATTACK_LD:
			_eState = eState::STATE_ATTACK_RUN_BACK_LD;
			break;
		case eState::STATE_ATTACK_RD:
			_eState = eState::STATE_ATTACK_RUN_RD;
			break;
		}
		tempSpeed = _speed * cos(D3DX_PI * 0.25f);
		Component_Transform->Move_Pos(D3DXVec3Normalize(&upDir, &upDir), -tempSpeed, _DT);
		Component_Transform->Move_Pos(D3DXVec3Normalize(&rightDir, &rightDir), tempSpeed, _DT);

	}
	else if (KEY_HOLD(DIK_W))
	{
		switch (_eState)
		{
		case eState::STATE_ATTACK_LU:
			_eState = eState::STATE_ATTACK_RUN_LU;
			break;
		case eState::STATE_ATTACK_RU:
			_eState = eState::STATE_ATTACK_RUN_RU;
			break;
		case eState::STATE_ATTACK_LD:
			_eState = eState::STATE_ATTACK_RUN_BACK_LD;
			break;
		case eState::STATE_ATTACK_RD:
			_eState = eState::STATE_ATTACK_RUN_BACK_RD;
			break;
		}
		Component_Transform->Move_Pos(D3DXVec3Normalize(&upDir, &upDir), _speed, _DT);
	}

	else if (KEY_HOLD(DIK_S))
	{
		switch (_eState)
		{
		case eState::STATE_ATTACK_LU:
			_eState = eState::STATE_ATTACK_RUN_BACK_LU;
			break;
		case eState::STATE_ATTACK_RU:
			_eState = eState::STATE_ATTACK_RUN_BACK_RU;
			break;
		case eState::STATE_ATTACK_LD:
			_eState = eState::STATE_ATTACK_RUN_LD;
			break;
		case eState::STATE_ATTACK_RD:
			_eState = eState::STATE_ATTACK_RUN_RD;
			break;
		}
		Component_Transform->Move_Pos(D3DXVec3Normalize(&upDir, &upDir), -_speed, _DT);
	}

	else if (KEY_HOLD(DIK_A))
	{
		switch (_eState)
		{
		case eState::STATE_ATTACK_LU:
			_eState = eState::STATE_ATTACK_RUN_LU;
			break;
		case eState::STATE_ATTACK_RU:
			_eState = eState::STATE_ATTACK_RUN_BACK_RU;
			break;
		case eState::STATE_ATTACK_LD:
			_eState = eState::STATE_ATTACK_RUN_LD;
			break;
		case eState::STATE_ATTACK_RD:
			_eState = eState::STATE_ATTACK_RUN_BACK_RD;
			break;
		}
		Component_Transform->Move_Pos(D3DXVec3Normalize(&rightDir, &rightDir), -_speed, _DT);
	}
	else if (KEY_HOLD(DIK_D))
	{
		switch (_eState)
		{
		case eState::STATE_ATTACK_LU:
			_eState = eState::STATE_ATTACK_RUN_BACK_LU;
			break;
		case eState::STATE_ATTACK_RU:
			_eState = eState::STATE_ATTACK_RUN_RU;
			break;
		case eState::STATE_ATTACK_LD:
			_eState = eState::STATE_ATTACK_RUN_BACK_LD;
			break;
		case eState::STATE_ATTACK_RD:
			_eState = eState::STATE_ATTACK_RUN_RD;
			break;
		case eState::STATE_RUN_RIGHT:
			break;
		default:
			//_frame = 1;
			_eState = eState::STATE_RUN_RIGHT;
			_see = pSee::SEE_RIGHT;
		}
		Component_Transform->Move_Pos(D3DXVec3Normalize(&rightDir, &rightDir), _speed, _DT);
	}
}
void Player::LANDING_STATE(const _float& _DT)
{
	_eState = eState::STATE_LAND;

	if (_frame == 10) {
		_weaponSlot[0]->Set_Bow_Equip(true);
		_pState = pState::STATE_IDLE;
		_eState = eState::STATE_STANDING;
		_see = pSee::SEE_DOWN;
	}
}
bool Player::DEATH_STATE(const _float& _DT)
{
	_eState = eState::STATE_DEAD;
	_weaponSlot[_equipNum]->Set_Bow_Equip(false);

	if (KEY_DOWN(DIK_T)) {
		Reset();
		_frame = 1;
		_pState = pState::STATE_LANDING;
		_eState = eState::STATE_LAND;
	}

	return false;
}
void Player::Idle_Final_Input(const _float& _DT)
{
	bool mouseLB = KeyManager::GetInstance()->Get_MouseState(DIM_LB) & 0x80;

	if (MOUSE_RBUTTON && _dashstock > 0) {
		SoundManager::GetInstance()->Play_Sound_Once(L"Player/Player_Dash.wav", CHANNELID::SOUND_EFFECT01, 0.5f);
		_pState = pState::STATE_DASH;
		_dashStart = true;
		_frame = 1;
		_dashstock--;
		_weaponSlot[_equipNum]->Set_Bow_Equip(false);
		_isInvincible = true;
	}
	else if (mouseLB) {
		_pState = pState::STATE_ATTACK;
		_attackDelay = 2.0f;
		_frame = 1;
	}
	else if (KEY_HOLD(DIK_SPACE)) {
		_pState = pState::STATE_ATTACK;
		_frame = 1;
	}
}

void Player::SKILL_NONE(const _float& _DT)
{
	_skillTimer = 0.f;
	if (KEY_DOWN(DIK_Q) && _token > 0) {
		_vec3 Size = { 0.2f, 0.2f, 0.2f };
		_NPC_Pos = *Component_Transform->Get_Position();
		_NPC_Pos.y += 1.f;
		Calc_Near();
		PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::BLUE_SHADER, &_nearPos, 4.f, Size, true);
		Size = { 1.5f, 1.5f, 1.5f };
		PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::NPC_TIMESLOW, &_NPC_Pos, 1.f, Size, false);
		SoundManager::GetInstance()->Play_Sound_Once(L"Player/Deva_Tif on.wav", CHANNELID::SOUND_EFFECT06,0.5f);
		_skillState = skillState::STATE_TIMESLOW;
		_skillNPC_On = false;
		_skillArea_On = false;
		SceneManager::GetInstance()->Set_TimeSlow(true);

		_originArrowSpeed = _arrowSpeed;
		_originDefualtSpeed = _defaultSpeed;

		MainUI* mainUI = dynamic_cast<MainUI*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"MainUI"));
		mainUI->Player_UseSkill();

		wstring txt = L"시간이여 멈춰라";
		mainUI->Speech_PopUp_Skill(txt);
	}

	if (KEY_DOWN(DIK_R)) {
		_vec3 Size = { 0.2f, 0.2f, 0.2f };
		_NPC_Pos = *Component_Transform->Get_Position();
		_NPC_Pos.y += 4.f;
		_NPC_Pos.z -= 3.5f;
		Size = { 1.5f, 1.5f, 1.5f };
		PLAY_PLAYER_EFFECT(PLAYER_SKILL::NPC_ATOMIC, &_NPC_Pos, 1.f, Size, false);
		Size = { 1.f, 1.f, 1.f };
		_NPC_Pos.x += 1.f;
		_NPC_Pos.z += 0.2f;
		PLAY_PLAYER_EFFECT(PLAYER_SKILL::NPC_ATOMIC_CHARGED, &_NPC_Pos, 1.f, Size, false);

		_skillState = skillState::STATE_ATOMIC;
		_skillNPC_On = false;
		_skillArea_On = false;
		_arrowTimer = 0.f;
		_skillTimer = 0.f;
		_ReadyAtomicCount = 0;
		_atomicCount = 0;
		_atomicTotal = 0;
		_atomicReady = 0;

		MainUI* mainUI = dynamic_cast<MainUI*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"MainUI"));
		mainUI->Player_UseSkill();
	}
}

void Player::SKILL_TIMESLOW(const _float& _DT)
{
	_skillTimer += _DT;
	_partnerTimer += _DT;
	Calc_Near();
	if (_skillTimer > 0.4f && !_skillArea_On) {
		_vec3 Size = { 5.f, 5.f, 5.f };
		PLAY_PLAYER_EFFECT(PLAYER_SKILL::NPC_AREA, &_NPC_Pos, 1.f, Size, false);
		_skillArea_On = true;
	}

	if (_skillTimer > 0.9f && !_skillNPC_On) {
		_vec3 Size = { 1.5f, 1.5f, 1.5f };
		PLAY_PLAYER_EFFECT(PLAYER_SKILL::NPC_TIMESLOW_LOOP, &_NPC_Pos, 1.f, Size, false);

		_skillNPC_On = true;
	}

	if (_skillTimer > _slowTime) {
		_skillState = skillState::NONE;
		SceneManager::GetInstance()->Set_TimeSlow(false);
	}

	if (_skillTimer < 0.8f) {
		_defaultSpeed = 1.f;
		_animSpeed = 5.f;
		_arrowSpeed = 0.1f;
	}
	else {
		_defaultSpeed = _originDefualtSpeed;
		_animSpeed = 1.f;
		_arrowSpeed = _originArrowSpeed;
	}
	
	if (_partnerTimer > 0.2f) {
		_vec3 dest = *Component_Transform->Get_Position();
		dest.z += 0.01;
		_vec3 Size = { 2.f, 2.f, 2.f };
		PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::SHADOW_PARTNER, &dest, max(0.f, _slowTime - _skillTimer), Size, false);
		_partnerTimer = 0.f;
	}
}

void Player::SKILL_ATOMIC(const _float& _DT)
{
	_skillTimer += _DT;
	_arrowTimer += _DT;

	if (_skillTimer > 0.5f && !_skillArea_On) {
		_vec3 Size = { 5.f, 5.f, 5.f };
		PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::NPC_ATOMIC_AREA, &_NPC_Pos, 0.8f, Size, false);
		_skillArea_On = true;
	}

	if (_skillTimer > 2.f) {
		_skillState = skillState::NONE;
	}

	if (_arrowTimer > 0.05f && _atomicTotal <= 10 && _skillTimer > 0.5f) {
		_atomicTotal += 1;
		GameObject* arrow = nullptr;

		std::random_device rd;
		std::uniform_int_distribution<int> distribution(0, 180);
		float angle;
		if(_atomicCount % 2 == 0) angle = D3DXToRadian(distribution(rd) % 70 + 20);
		else angle = D3DXToRadian(distribution(rd) % 70 + 90);
		_vec2 dir = { cosf(angle), sinf(angle) };
		_atomicCount += 1;
		arrow = Arrow::Create(GRPDEV, BowType::AtomicBow, 1, 0, &_NPC_Pos, dir);

		TCHAR arrowTag[128] = L"";
		wsprintfW(arrowTag, L"PlayerArrow_%d", _arrowCount++);

		arrow->Set_ObjectTag(arrowTag);
		arrow->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_PLAYER);
		arrow->Set_ObjectTag(L"PlayerArrow");

		SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(arrow);

		_arrowTimer = 0.f;
	}
}

void Player::SetGrahpic()
{
	DWORD tfactor = D3DCOLOR_ARGB(
		(BYTE)(_alphaRatio * 255.f),
		255, 255, 255
	);

	GRPDEV->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	GRPDEV->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GRPDEV->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GRPDEV->SetRenderState(D3DRS_TEXTUREFACTOR, tfactor);

	switch (_eState)
	{
	case eState::STATE_STANDING :
		switch (_see)
		{
		case pSee::SEE_DOWN :
			if (_speed <= 0) {
				if (_frame > 8) _frame = 1;
				wsprintfW(FileName, L"Player_Stand_Down%d.png", _frame);
			}
			else {
				if (_frame > 4) _frame = 1;
				if (_slideTime < 0.1f) _frame = 1;
				wsprintfW(FileName, L"Player_Down_Slide%d.png", _frame);
			}
			break;
		case pSee::SEE_UP:
			if (_speed <= 0) {
				if (_frame > 8) _frame = 1;
				wsprintfW(FileName, L"Player_Stand_UP%d.png", _frame);
			}
			else {
				if (_frame > 4) _frame = 1;
				if (_slideTime < 0.1f) _frame = 1;
				wsprintfW(FileName, L"Player_Up_Slide%d.png", _frame);
			}
			break;
		case pSee::SEE_RIGHT:
			if (_speed <= 0) {
				if (_frame > 8) _frame = 1;
				wsprintfW(FileName, L"StandRS0%d.png", _frame);
			}
			else {
				if (_frame > 4) _frame = 1;
				if (_slideTime < 0.1f) _frame = 1;
				wsprintfW(FileName, L"Player_Right_Slide%d.png", _frame);
			}
			break;
		case pSee::SEE_LEFT:
			if (_speed <= 0) {
				if (_frame > 8) _frame = 1;
				wsprintfW(FileName, L"Player_Stand_Left%d.png", _frame);
			}
			else{
				if (_frame > 4) _frame = 1;
				if (_slideTime < 0.1f) _frame = 1;
				wsprintfW(FileName, L"Player_Left_Slide%d.png", _frame);
			}
			break;
		case pSee::SEE_LU:
			if (_speed <= 0) {
				if (_frame > 8) _frame = 1;
				wsprintfW(FileName, L"Player_Stand_LT%d.png", _frame);
			}
			else {
				if (_frame > 4) _frame = 1;
				if (_slideTime < 0.1f) _frame = 1;
				wsprintfW(FileName, L"Player_LU_Slide%d.png", _frame);
			}
			break;
		case pSee::SEE_RU:
			if (_speed <= 0) {
				if (_frame > 8) _frame = 1;
				wsprintfW(FileName, L"Player_Stand_RT%d.png", _frame);
			}
			else {
				if (_frame > 4) _frame = 1;
				if (_slideTime < 0.1f) _frame = 1;
				wsprintfW(FileName, L"Player_RU_Slide%d.png", _frame);
			}
			break;
		case pSee::SEE_LD:
			if (_speed <= 0) {
				if (_frame > 8) _frame = 1;
				wsprintfW(FileName, L"Stand_LB0%d.png", _frame);
			}
			else {
				if (_frame > 4) _frame = 1;
				if (_slideTime < 0.1f) _frame = 1;
				wsprintfW(FileName, L"Player_LD_Slide%d.png", _frame);
			}
			break;
		case pSee::SEE_RD:
			if (_speed <= 0) {
				if (_frame > 8) _frame = 1;
				wsprintfW(FileName, L"Stand_RB0%d.png", _frame);
			}
			else {
				if (_frame > 4) _frame = 1;
				if (_slideTime < 0.1f) _frame = 1;
				wsprintfW(FileName, L"Player_RD_Slide%d.png", _frame);
			}
			break;
		}
		if (_speed <= 0)
			Anim(FileName, 0.1f, 8);
		else
			Anim(FileName, 0.1f, 4);
		
		break;
	case eState::STATE_RUN_UP:
		if (_frame > 8) _frame = 1;
		wsprintfW(FileName, L"Player_Run_UP%d.png", _frame);
		Anim(FileName, 0.1f, 8);
		break;
	case eState::STATE_RUN_DOWN:
		if (_frame > 8) _frame = 1;
		wsprintfW(FileName, L"Player_Run_Down%d.png", _frame);
		Anim(FileName, 0.1f, 8);
		break;
	case eState::STATE_RUN_LEFT:
		if (_frame > 8) _frame = 1;
		wsprintfW(FileName, L"Player_Run_LEFT%d.png", _frame);
		Anim(FileName, 0.1f, 8);
		break;
	case eState::STATE_RUN_LU:
		if (_frame > 8) _frame = 1;
		wsprintfW(FileName, L"Player_Run_LU%d.png", _frame);
		Anim(FileName, 0.1f, 8);
		break;

	case eState::STATE_RUN_LD:
		if (_frame > 8) _frame = 1;
		wsprintfW(FileName, L"Player_Run_LD%d.png", _frame);
		Anim(FileName, 0.1f, 8);
		break;

	case eState::STATE_RUN_RIGHT:
		if (_frame > 8) _frame = 1;
		wsprintfW(FileName, L"Player_Run_Right%d.png", _frame);
		Anim(FileName, 0.1f, 8);
		break;

	case eState::STATE_RUN_RU:
		if (_frame > 8) _frame = 1;
		wsprintfW(FileName, L"RTRun0%d.png", _frame);
		Anim(FileName, 0.1f, 8);
		break;

	case eState::STATE_RUN_RD:
		if (_frame > 8) _frame = 1;
		wsprintfW(FileName, L"Player_Run_RD%d.png", _frame);
		Anim(FileName, 0.1f, 8);
		break;

	case eState::STATE_ATTACK_LU:
		wsprintfW(FileName, L"Player_Attack_Stand_LU%d.png", _frame);
		Anim(FileName, 0.1f, 10);
		break;

	case eState::STATE_ATTACK_LD:
		wsprintfW(FileName, L"Player_Attack_Stand_LD%d.png", _frame);
		Anim(FileName, 0.1f, 10);
		break;
	case eState::STATE_ATTACK_RU:
		wsprintfW(FileName, L"Player_Attack_Stand_RU%d.png", _frame);
		Anim(FileName, 0.1f, 10);
		break;

	case eState::STATE_ATTACK_RD:
		wsprintfW(FileName, L"Player_Attack_Stand_RD%d.png", _frame);
		Anim(FileName, 0.1f, 10);
		break;
	case eState::STATE_ATTACK_RUN_LU:
		wsprintfW(FileName, L"Player_Attack_LU%d.png", _frame);
		Anim(FileName, 0.1f, 10);
		break;
	case eState::STATE_ATTACK_RUN_LD:
		wsprintfW(FileName, L"Player_Attack_LD%d.png", _frame);
		Anim(FileName, 0.1f, 10);
		break;
	case eState::STATE_ATTACK_RUN_RU:
		wsprintfW(FileName, L"Player_Attack_RU%d.png", _frame);
		Anim(FileName, 0.1f, 10);
		break;
	case eState::STATE_ATTACK_RUN_RD:
		wsprintfW(FileName, L"Player_Attack_RD%d.png", _frame);
		Anim(FileName, 0.1f, 10);
		break;
	case eState::STATE_ATTACK_RUN_BACK_LU:
		wsprintfW(FileName, L"Player_Attack_LU%d.png", _frame);
		Anim(FileName, 0.1f, 10);
		break;
	case eState::STATE_ATTACK_RUN_BACK_LD:
		wsprintfW(FileName, L"Player_Attack_LD%d.png", _frame);
		Anim(FileName, 0.1f, 10);
		break;
	case eState::STATE_ATTACK_RUN_BACK_RU:
		wsprintfW(FileName, L"Player_Attack_RU%d.png", _frame);
		Anim(FileName, 0.1f, 10);
		break;
	case eState::STATE_ATTACK_RUN_BACK_RD:
		if (_frame > 10) _frame = 1;
		wsprintfW(FileName, L"Player_Attack_RD%d.png", _frame);
		Anim(FileName, 0.1f, 10);
		break;
	case eState::STATE_DASH_LU:
		if (_frame > 8) _frame = 1;
		wsprintfW(FileName, L"Player_Dash_LU%d.png", _frame);
		Anim(FileName, 0.1f, 8);
		break;
	case eState::STATE_DASH_RU:
		if (_frame > 8) _frame = 1;
		wsprintfW(FileName, L"Player_Dash_RU%d.png", _frame);
		Anim(FileName, 0.1f, 8);
		break;
	case eState::STATE_DASH_LD:
		if (_frame > 8) _frame = 1;
		wsprintfW(FileName, L"Player_Dash_LD%d.png", _frame);
		Anim(FileName, 0.1f, 8);
		break;
	case eState::STATE_DASH_RD:
		if (_frame > 8) _frame = 1;
		wsprintfW(FileName, L"Player_Dash_RD%d.png", _frame);
		Anim(FileName, 0.1f, 8);
		break;
	case eState::STATE_DASH_UP:
		if (_frame > 8) _frame = 1;
		wsprintfW(FileName, L"Player_Dash_Up%d.png", _frame);
		Anim(FileName, 0.1f, 8);
		break;
	case eState::STATE_DASH_DOWN:
		if (_frame > 8) _frame = 1;
		wsprintfW(FileName, L"Player_Dash_Down%d.png", _frame);
		Anim(FileName, 0.1f, 8);
		break;
	case eState::STATE_DASH_LEFT:
		if (_frame > 8) _frame = 1;
		wsprintfW(FileName, L"Player_Dash_Left%d.png", _frame);
		Anim(FileName, 0.1f, 8);
		break;
	case eState::STATE_DASH_RIGHT:
		if (_frame > 8) _frame = 1;
		wsprintfW(FileName, L"Player_Dash_Right%d.png", _frame);
		Anim(FileName, 0.1f, 8);
		break;
	case eState::STATE_DEAD:
		if (_frame > 12) _frame = 12;
		wsprintfW(FileName, L"Player_Death%d.png", _frame);
		Anim(FileName, 0.1f, 12);
		break;
	case eState::STATE_LAND:
		if (_frame > 10) _frame = 1;
		wsprintfW(FileName, L"Player_Landing%d.png", _frame);
		Anim(FileName, 0.1f, 10);
		break;
	}

	// COLOR = Texture * TFACTOR
	GRPDEV->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	GRPDEV->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	GRPDEV->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

	// ALPHA = TextureAlpha * TFACTORAlpha
	GRPDEV->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	GRPDEV->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	GRPDEV->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
}
void Player::Anim(TCHAR FileName[128], float delay, int maxIdx, bool reverse)
{
	Component_Texture->Set_Texture(FileName);
	delay *= _animSpeed;
	if (!reverse)
	{
		if (_frameTick > delay)
		{
			if (++_frame > maxIdx && _pState != pState::STATE_DEATH)
				_frame = 1;

			_frameTick = 0.f;
		}
	}
	else
	{
		if (_frameTick > delay)
		{
			if (--_frame < 1)
				_frame = maxIdx;

			_frameTick = 0.f;
		}
	}

}
void Player::Set_Effect(const _float& _DT)
{
	_vec3* playerPos = Component_Transform->Get_Position();

	POINT MousePoint{ 0, 0 };
	GetCursorPos(&MousePoint);
	ScreenToClient(hWnd, &MousePoint);

	_vec2 mousePos = { (float)MousePoint.x, (float)MousePoint.y };
	_vec2 screenCenter = { WINCX * 0.5f, WINCY * 0.5f };

	_vec2 dir2D = mousePos - screenCenter;
	D3DXVec2Normalize(&dir2D, &dir2D);

	float angle = atan2f(dir2D.y, dir2D.x);

	float radius = 2.3f;

	float offsetX = cosf(angle) * radius;
	float offsetY = sinf(angle) * radius;

	_pulsepos = { playerPos->x + offsetX , playerPos->y, playerPos->z - offsetY };

	//if (KEY_DOWN(DIK_1)) { PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::SKILL_1, playerPos, 0.5f); }
	//else if (KEY_DOWN(DIK_2)) { PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::SKILL_2, playerPos, 0.5f); }
	//else if (KEY_DOWN(DIK_3)) { PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::SKILL_3, playerPos, 0.5f); }
	//else if (KEY_DOWN(DIK_4)) { PLAY_PLAYER_EFFECT(PLAYER_SKILL::ICEARROW_PULSE, &_pulsepos, 0.2f); }
}
void Player::Calc_Near()
{
	CameraObject* Camera = dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->
		Get_GameObject(L"Camera"));
	_vec3 cameraPos = *Camera->Get_EyeVec();

	_vec3 dir = _NPC_Pos - cameraPos;
	D3DXVec3Normalize(&dir, &dir);

	_nearPos = cameraPos + dir * 5.f;
}
BOOL Player::OnCollisionEnter(GameObject* _Other)
{
	if (_pState == pState::STATE_DEATH || _pState == pState::STATE_LANDING) return FALSE;
	wstring Tag = _Other->Get_ObjectTag();
	MainUI* mainUI;
	if (Tag == L"MonsterBullet")
	{
		mainUI = dynamic_cast<MainUI*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"MainUI"));
		mainUI->Player_LostHP();

		return TRUE;
	}
	else if(Tag == L"Monster")
	{
		mainUI = dynamic_cast<MainUI*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"MainUI"));
		mainUI->Player_LostHP();

		return TRUE;
	}

	return FALSE;
}
BOOL Player::OnCollisionStay(GameObject* _Other)
{
	return 0;
}
BOOL Player::OnCollisionExit(GameObject* _Other)
{
	return 0;
}
HRESULT Player::MiniGameInit()
{
	if (nullptr == dynamic_cast<MiniGameScene*>(SceneManager::GetInstance()->Get_CurrentScene())) {
		m_eCurrScene = SCENE_TYPE::SCENE_END;
		return S_OK;
	}

	m_eCurrScene = SCENE_TYPE::Minigame;

	Component_Transform->Set_Pos(25.f, 0.f, 0.f);

	return S_OK;
}
D3DXVECTOR3 Player::MousePicker_NonTarget(HWND _hWnd, Buffer* _TerrainBuffer, Transform* _TerrainTransform) {

	POINT MousePoint {0, 0};
	GetCursorPos(&MousePoint);
	ScreenToClient(_hWnd, &MousePoint);

	D3DVIEWPORT9		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
	GRPDEV->GetViewport(&ViewPort);

	_vec3		MousePosition;

	MousePosition.x = MousePoint.x /  (ViewPort.Width * 0.5f)  - 1.f;
	MousePosition.y = MousePoint.y / -(ViewPort.Height * 0.5f) + 1.f;
	MousePosition.z = 0.f;

	D3DXMATRIX	WorldMat, ProjectionMat, ViewMat;

	GRPDEV->GetTransform(D3DTS_PROJECTION, &ProjectionMat);
	D3DXMatrixInverse(&ProjectionMat, 0, &ProjectionMat);
	D3DXVec3TransformCoord(&MousePosition, &MousePosition, &ProjectionMat);

	GRPDEV->GetTransform(D3DTS_VIEW, &ViewMat);
	D3DXMatrixInverse(&ViewMat, 0, &ViewMat);

	_vec3	RayPosition = { 0.f, 0.f, 0.f };//*Component_Transform->Get_Position();
	_vec3	RayDirection = MousePosition - RayPosition;

	D3DXVec3TransformCoord(&RayPosition, &RayPosition, &ViewMat);
	D3DXVec3TransformNormal(&RayDirection, &RayDirection, &ViewMat);

	WorldMat = *(_TerrainTransform->Get_World());
	D3DXMatrixInverse(&WorldMat, 0, &WorldMat);

	D3DXVec3TransformCoord(&RayPosition, &RayPosition, &WorldMat);
	D3DXVec3TransformNormal(&RayDirection, &RayDirection, &WorldMat);

	const _vec3* TerrainVertexPos = _TerrainBuffer->Get_BufferPos(0);

	ULONG	VertexNumb[3]{};
	FLOAT	U(0.f), V(0.f), Distance(0.f);

	for (ULONG Z = 0; Z < VTXCNTZ - 1; ++Z) {
		for (ULONG X = 0; X < VTXCNTX - 1; ++X) {
			ULONG INDEX = Z * VTXCNTX + X;

			// ������ ��
			VertexNumb[0] = INDEX + VTXCNTX;
			VertexNumb[1] = INDEX + VTXCNTX + 1;
			VertexNumb[2] = INDEX + 1;

			// V1 + U(V2 - V1) + V(V3 - V1)

			if (D3DXIntersectTri(&TerrainVertexPos[VertexNumb[1]], &TerrainVertexPos[VertexNumb[0]], &TerrainVertexPos[VertexNumb[2]],
								&RayPosition, &RayDirection, &U, &V, &Distance))
			{
				return _vec3(TerrainVertexPos[VertexNumb[1]].x + U * (TerrainVertexPos[VertexNumb[0]].x - TerrainVertexPos[VertexNumb[1]].x),
					0.f,
					TerrainVertexPos[VertexNumb[1]].z + V * (TerrainVertexPos[VertexNumb[2]].z - TerrainVertexPos[VertexNumb[1]].z));
			}



			// ���� �Ʒ�
			VertexNumb[0] = INDEX + VTXCNTX;
			VertexNumb[1] = INDEX + 1;
			VertexNumb[2] = INDEX;

			// V1 + U(V2 - V1) + V(V3 - V1)

			if (D3DXIntersectTri(&TerrainVertexPos[VertexNumb[2]],
				&TerrainVertexPos[VertexNumb[1]],
				&TerrainVertexPos[VertexNumb[0]],
				&RayPosition, &RayDirection,
				&U, &V, &Distance))
			{	
				return _vec3(TerrainVertexPos[VertexNumb[2]].x + U * (TerrainVertexPos[VertexNumb[1]].x - TerrainVertexPos[VertexNumb[2]].x),
					0.f,
					TerrainVertexPos[VertexNumb[2]].z + V * (TerrainVertexPos[VertexNumb[0]].z - TerrainVertexPos[VertexNumb[2]].z));
			}

		}
	}

	return _vec3(0.f, 0.f, 0.f);
}
D3DXVECTOR3 Player::RayOnTerrain() {
	Buffer* TerrainBuffer = dynamic_cast<Buffer*>(SceneManager::GetInstance()->Get_GameObject(L"Terrain")
		->Get_Component(COMPONENT_TYPE::COMPONENT_TERRAIN));

	if (TerrainBuffer == nullptr) return D3DXVECTOR3(0.f, 0.f, 0.f);

	Transform* TerrainTransform = dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_GameObject(L"Terrain")
		->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));

	if (TerrainTransform == nullptr) return D3DXVECTOR3(0.f, 0.f, 0.f);

	return MousePicker_NonTarget(hWnd, TerrainBuffer, TerrainTransform);
}
D3DXVECTOR3 Player::SetOnTerrain() {
	_vec3*	Position;
	Position = Component_Transform->Get_Position();

	Buffer* TerrainBuffer = dynamic_cast<Buffer*>(SceneManager::GetInstance()->Get_GameObject(L"Terrain")
		->Get_Component(COMPONENT_TYPE::COMPONENT_TERRAIN));

	if (TerrainBuffer == nullptr) return D3DXVECTOR3(0.f, 0.f, 0.f);

	return D3DXVECTOR3(Position->x, 1.f, Position->z);
}
void Player::Destroy_Weapon(int idx)
{
	_weaponSlot[idx]->Set_Destroy();
	_weaponSlot[idx] = nullptr;
}
Player* Player::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	Player* PLAYER = new Player(_GRPDEV);
	if (FAILED(PLAYER->Ready_GameObject())) {
		MSG_BOX("Cannot Create Player.");
		Safe_Release(PLAYER);
		return nullptr;
	}
	return PLAYER;
}
void Player::CheonLog_Spawn()
{
	if (CameraMove)
	{
		_pState = pState::STATE_IDLE;
		_eState = eState::STATE_RUN_UP;
		_see	= pSee::SEE_UP;
	}
}
_vec3 Player::Get_MouseDir()
{
	POINT MousePoint{ 0, 0 };
	GetCursorPos(&MousePoint);
	ScreenToClient(hWnd, &MousePoint);

	_vec3 vMouse;
	vMouse.x = (float)MousePoint.x;
	vMouse.y = (float)MousePoint.y;
	vMouse.z = 0.f;

	D3DVIEWPORT9		viewport;
	ZeroMemory(&viewport, sizeof(D3DVIEWPORT9));

	GRPDEV->GetViewport(&viewport);

	D3DXMATRIX		matProj;
	ZeroMemory(&matProj, sizeof(D3DVIEWPORT9));
	GRPDEV->GetTransform(D3DTS_PROJECTION, &matProj);

	D3DXMATRIX		matView;
	ZeroMemory(&matView, sizeof(D3DVIEWPORT9));
	GRPDEV->GetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIX matWorldIdentity;
	D3DXMatrixIdentity(&matWorldIdentity);

	vMouse.z = 0.f;
	D3DXVECTOR3 vNear;
	D3DXVec3Unproject(&vNear, &vMouse, &viewport, &matProj, &matView, &matWorldIdentity);

	vMouse.z = 1.f;
	D3DXVECTOR3 vFar;
	D3DXVec3Unproject(&vFar, &vMouse, &viewport, &matProj, &matView, &matWorldIdentity);

	_vec3 vDir = vFar - vNear;
	D3DXVec3Normalize(&vDir, &vDir);

	float t = -vNear.y / vDir.y;
	_vec3 vPickPos = vNear + vDir * t;

	_vec3 mouseDir = vPickPos - *Component_Transform->Get_Position();
	mouseDir.y = 0.f;
	D3DXVec3Normalize(&mouseDir, &mouseDir);

	return mouseDir;
}
_float Player::Get_MouseDistance()
{
	POINT MousePoint{ 0, 0 };
	GetCursorPos(&MousePoint);
	ScreenToClient(hWnd, &MousePoint);

	_vec3 vMouse;
	vMouse.x = (float)MousePoint.x;
	vMouse.y = (float)MousePoint.y;
	vMouse.z = 0.f;

	D3DVIEWPORT9		viewport;
	ZeroMemory(&viewport, sizeof(D3DVIEWPORT9));

	GRPDEV->GetViewport(&viewport);

	D3DXMATRIX		matProj;
	ZeroMemory(&matProj, sizeof(D3DVIEWPORT9));
	GRPDEV->GetTransform(D3DTS_PROJECTION, &matProj);

	D3DXMATRIX		matView;
	ZeroMemory(&matView, sizeof(D3DVIEWPORT9));
	GRPDEV->GetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIX matWorldIdentity;
	D3DXMatrixIdentity(&matWorldIdentity);

	vMouse.z = 0.f;
	D3DXVECTOR3 vNear;
	D3DXVec3Unproject(&vNear, &vMouse, &viewport, &matProj, &matView, &matWorldIdentity);

	vMouse.z = 1.f;
	D3DXVECTOR3 vFar;
	D3DXVec3Unproject(&vFar, &vMouse, &viewport, &matProj, &matView, &matWorldIdentity);

	_vec3 vDir = vFar - vNear;
	D3DXVec3Normalize(&vDir, &vDir);

	float t = -vNear.y / vDir.y;
	_vec3 vPickPos = vNear + vDir * t;
	_vec3 dis = vPickPos - *Component_Transform->Get_Position();

	_float mouseDistance = D3DXVec3Length(&dis);
	
	return mouseDistance;
}

VOID Player::Chage_Item(int src, int dst)
{
	GameObject* obj = nullptr;
	if (src < 4) {
		if (_weaponSlot[src] == nullptr) return;
		if (dst < 4) {
			if (_weaponSlot[dst] == nullptr) {
				_weaponSlot[dst] = _weaponSlot[src];
				_weaponSlot[src] = nullptr;
			}
			else {
				obj = _weaponSlot[dst];
				_weaponSlot[dst] = _weaponSlot[src];
				_weaponSlot[src] = static_cast<Bow*>(obj);
			}
		}
		else if(dst >= 4 && dst < 8){
			return;
		}
		else {
			if (_inventory[dst - 8] == nullptr) {
				_inventory[dst - 8] = _weaponSlot[src];
				if (src == _equipNum) {
					_weaponSlot[src]->Set_Bow_Equip(false);
					_equipNum = 0;
					_weaponSlot[0]->Set_Bow_Equip(true);
				}
				_weaponSlot[src] = nullptr;
			}
			else {
				if (src == _equipNum) {
					_weaponSlot[src]->Set_Bow_Equip(false);
					static_cast<Bow*>(_inventory[dst - 8])->Set_Bow_Equip(true);
				}
				obj = _inventory[dst - 8];
				_inventory[dst - 8] = _weaponSlot[src];
				_weaponSlot[src] = static_cast<Bow*>(obj);
			}
		}
	}
	else if (src >= 4 && src < 8) {
		if (_artifactSlot[src - 4] == nullptr) return;
		return;
	}
	else {
		if (_inventory[src - 8] == nullptr) return;
		if (dst < 4) {
			if (_weaponSlot[dst] == nullptr) {
				_weaponSlot[dst] = static_cast<Bow*>(_inventory[src - 8]);
				_inventory[src - 8] = nullptr;
			}
			else {
				if (dst == _equipNum) {
					_weaponSlot[dst]->Set_Bow_Equip(false);
					static_cast<Bow*>(_inventory[src - 8])->Set_Bow_Equip(true);
				}
				obj = _weaponSlot[dst];
				_weaponSlot[dst] = static_cast<Bow*>(_inventory[src - 8]);
				_inventory[src - 8] = obj;
			}
		}
		else if (src >= 4 && src < 8) {
			return;
		}
		else {
			obj = _inventory[dst - 8];
			_inventory[dst - 8] =_inventory[src - 8];
			_inventory[src - 8] = obj;
		}
	}

	return VOID();
}
VOID	Player::Free() {
	GameObject::Free();
}