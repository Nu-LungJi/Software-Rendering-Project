#include "../Include/PCH.h"
#include "ScorpionEvilSoul.h"

ScorpionEvilSoul::ScorpionEvilSoul(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
ScorpionEvilSoul::ScorpionEvilSoul(const GameObject& _RHS) : GameObject(_RHS) {}
ScorpionEvilSoul::~ScorpionEvilSoul() {}
HRESULT ScorpionEvilSoul::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;

	m_tInfo.eState[0] = MONSTER_STATE_APPEAR;
	Component_Collider->Set_Hp(SCORPIONEVILSOUL_HP);

	m_tInfo.vDirection = { -1.f,0.f,0.f };

	return S_OK;
}
HRESULT ScorpionEvilSoul::Ready_GameObject(_vec3 vPos, BOOL bMini) {
	if (FAILED(Component_Initialize())) return E_FAIL;

	m_tInfo.eState[0] = MONSTER_STATE_APPEAR;
	m_tInfo.bMiniGame = bMini;
	Component_Collider->Set_Hp(SCORPIONEVILSOUL_HP);
	vPos = { 0.f, 0.f, 0.f };
	m_tInfo.vDirection = { -1.f,0.f,0.f };


	if (m_tInfo.bMiniGame)
	{// â�� �߰�
		ObjectTAG = L"Monster";

		m_tInfo.eState[0] = MONSTER_STATE_TRACKING;
		Component_Transform->Set_Pos(vPos);
		Component_Transform->Set_Scale({2.3f,2.3f,2.3f});
	}
	return S_OK;
}
INT	ScorpionEvilSoul::Update_GameObject(const _float& _DT)
{

	Component_Collider->Update_Component(_DT);

	if (ObjectTYPE == GAMEOBJECT_TYPE::OBJECT_HURDLE) {
		MonsterManager::Update_Key(m_tInfo.ID, (uint8_t)MONSTER_ANIM::Stand);
		Monster::Set_TextureList(m_tInfo.ID, &m_tInfo.Textureinfo);
	}

	Monster::Minigame_Update(_DT, &m_tInfo, MYPOS);

	if (m_tInfo.bMiniGame)
	{// 창준 추가
		GameObject::Update_GameObject(_DT);
		ScorpionEvilSoul::State_Tracking(_DT);
		MYPOS->y = 0.5f;
		Component_Collider->Set_Scale(MYSCALE->x * 0.5f, 1.f, MYSCALE->x * 0.5f);
		RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

		return 1;
	}

	if (m_tInfo.eState[0] != MONSTER_STATE_MINIGAME_MOVE &&
		m_tInfo.eState[0] != MONSTER_STATE_MINIGAME_IDLE)
		MYPOS->y = MYSCALE->y * 0.5f;
	Component_Collider->Set_Scale(MYSCALE->x * 0.5f, MYSCALE->x * 0.5f, MYSCALE->x * 0.5f);

	if (Component_Collider->Get_Hp() <= 0.f)
		m_tInfo.Change_State(MONSTER_STATE_DISAPPEAR);


	if (Component_Collider->Get_Hp() <= 0.f)
		m_tInfo.eState[0] = MONSTER_STATE_DISAPPEAR;

	switch (m_tInfo.eState[0])
	{
	default:
		break;
	case MONSTER_STATE_APPEAR:
		ScorpionEvilSoul::State_Appear(_DT);
		break;
	case MONSTER_STATE_DISAPPEAR:
		if (m_tInfo.Textureinfo._frame >= m_tInfo.Textureinfo._Endframe)
		{
			// SoundManager::GetInstance()->Play_Sound_Once(L"Monster/Monster_Death.wav", CHANNELID::SOUND_EFFECT05, 0.3f);
			if (TileManager::GetInstance()->Get_Stage() != TILE_STAGE::TILE_DEFENSE) {
				std::random_device rd;
				std::uniform_int_distribution<int> distribution(0, 180);

				Player* player = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Player"));
				player->Set_Coin(player->Get_Coin() + 10 + (distribution(rd) % 5));
			}
			ObjectDead = true;
		}
		break;
	case MONSTER_STATE_IDLE:
		ScorpionEvilSoul::State_Idle(_DT);
		break;
	case MONSTER_STATE_TRACKING:
		ScorpionEvilSoul::State_Tracking(_DT);
		break;
	case MONSTER_STATE_CASTING:
		ScorpionEvilSoul::State_Casting(_DT);
		break;
	case MONSTER_STATE_CHANNELING:
		ScorpionEvilSoul::State_Channeling(_DT);
		break;
	case MONSTER_STATE_DEAD:
		ScorpionEvilSoul::State_Dead();
		break;
	}

	if (KEY_DOWN(DIK_K)) {
		m_tInfo.Change_State(MONSTER_STATE_DISAPPEAR);
	}

	if (ObjectDead)
	{
		TileManager::GetInstance()->Set_StageArray();
		return -1;
	}


	if (KEY_DOWN(DIK_P))
	{
		m_tInfo.Change_State(MONSTER_STATE_DEAD);
	}
	return 0;
}

VOID ScorpionEvilSoul::LateUpdate_GameObject(const _float& _DT) {

	GameObject::LateUpdate_GameObject(_DT);
	
	m_tInfo.vDirection.y = 0.f;
	Component_Transform->Move_Pos(D3DXVec3Normalize(&m_tInfo.vDirection, &m_tInfo.vDirection), m_tInfo.fSpeed, _DT);

	m_tInfo.Textureinfo._frameTick += _DT;

	switch (m_tInfo.eState[0])
	{
	default:
		//m_tInfo.ID = MonsterManager::Update_Key(m_tInfo.ID, (uint8_t)MONSTER_ANIM::Stand);
		//if (FAILED(Monster::Set_TextureList(m_tInfo.ID, &m_tInfo.Textureinfo))) return;

		if (m_tInfo.Textureinfo._frameTick >= FRAMETICK)
		{
			m_tInfo.Textureinfo._frameTick = 0.f;

			_uint HalfFrame = (m_tInfo.Textureinfo._Endframe + 1) * 0.5f;
			_uint BaseFrame = (m_tInfo.Textureinfo._frame + 1) % HalfFrame;
			_uint Offset = (fabsf(m_tInfo.vDirection.z) > 0.1f) * (m_tInfo.vDirection.z > 0.f)* HalfFrame;

			m_tInfo.Textureinfo._frame = BaseFrame + Offset;
		}

		if (FAILED(Monster::Flip_Horizontal(Component_Transform, &m_tInfo.vDirection, SCORPIONEVILSOUL_HORIZONTALFLIP_BUFFER)))
		{
			m_tInfo.Change_State(MONSTER_STATE_DEAD);
			return;
		}

		break;

	case MONSTER_STATE_APPEAR:

		break;
	case MONSTER_STATE_DISAPPEAR:
		m_tInfo.ID = MonsterManager::Update_Key(m_tInfo.ID, (uint8_t)MONSTER_ANIM::Disappear);
		if (FAILED(Monster::Set_TextureList(m_tInfo.ID, &m_tInfo.Textureinfo))) return;

		m_tInfo.Textureinfo._frameTick += _DT;
		if (m_tInfo.Textureinfo._frameTick > FRAMETICK)
		{
			m_tInfo.Textureinfo._frameTick = 0.f;
			++m_tInfo.Textureinfo._frame;


		}
		break;
	case MONSTER_STATE_SUMMON:
	case MONSTER_STATE_DEAD:
		break;
	}

	if (Monster::Minigame_LateUpdate(_DT, &m_tInfo) ||
		(static_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Camera"))->IsIn_Frustum(*MYPOS, 10.f))) {
		Monster::Flip_Horizontal(Component_Transform, &m_tInfo.vDirection, BAT_HORIZONTALFLIP_BUFFER);
		AlphaZValue = Monster::BillBoard(Component_Transform, GRPDEV);
		RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	}
	
}
VOID ScorpionEvilSoul::Render_GameObject() {

	if (m_tInfo.Textureinfo._frame > m_tInfo.Textureinfo._Endframe) return;
		

	GRPDEV->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	switch (m_tInfo.eState[0])
	{
	default:
		if (m_tInfo.bMiniGame)// 창준 추가
		{
			GRPDEV->SetTexture(0, (*m_tInfo.Textureinfo.pTexture)[m_tInfo.Textureinfo._frame]);
			Component_Buffer->Render_Buffer();
		}
		else
		{
			switch (m_tInfo.eState[0])
			{
			default:
				GRPDEV->SetTexture(0, (*m_tInfo.Textureinfo.pTexture)[m_tInfo.Textureinfo._frame]);
				Component_Buffer->Render_Buffer();
				break;
			case MONSTER_STATE_SUMMON:
			case MONSTER_STATE_APPEAR:
			case MONSTER_STATE_DEAD:
				break;
			}
		}
		break;
	case MONSTER_STATE_SUMMON:
	case MONSTER_STATE_DEAD:
		break;
	}

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
HRESULT ScorpionEvilSoul::Component_Initialize() {

	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;

	Component_Transform->Set_Pos(0.f, 0.5f, 0.f);
	Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
	Component_Transform->Set_Scale(SCORPIONEVILSOUL_WIDTH, SCORPIONEVILSOUL_HEIGHT, 1.f);
	
	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);
	Component_Collider->Set_Hp(SCORPIONEVILSOUL_HP);
	Component_Collider->Set_Scale(SCORPIONEVILSOUL_WIDTH * 0.5f, SCORPIONEVILSOUL_HEIGHT, SCORPIONEVILSOUL_WIDTH * 0.5f);

	m_tInfo.eState[0]	= MONSTER_STATE_APPEAR;
	m_tInfo.vDirection	= { 1.f,0.f,0.f };

	m_tInfo.ID = MonsterManager::Make_Key((uint8_t)MONSTER_SEP::Monster,
		(uint8_t)MONSTER_TYPE::ScorpionEvilSoul,
		(uint8_t)MONSTER_ANIM::Stand);

	return Monster::Set_TextureList(m_tInfo.ID, &m_tInfo.Textureinfo);
}

ScorpionEvilSoul* ScorpionEvilSoul::Create(LPDIRECT3DDEVICE9 _GRPDEV, _vec3 vPos, BOOL bMini) {
	ScorpionEvilSoul* MST = new ScorpionEvilSoul(_GRPDEV);
	if (FAILED(MST->Ready_GameObject(vPos, bMini))) {
		MSG_BOX("Cannot Create ScorpionEvilSoul.");
		Safe_Release(MST);
		return nullptr;
    }
	return MST;
 }
ScorpionEvilSoul* ScorpionEvilSoul::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	ScorpionEvilSoul* MST = new ScorpionEvilSoul(_GRPDEV);
	if (FAILED(MST->Ready_GameObject())) {
		MSG_BOX("Cannot Create ScorpionEvilSoul.");

		Safe_Release(MST);
		return nullptr;
	}
	return MST;
}
BOOL ScorpionEvilSoul::OnCollisionEnter(GameObject* _Other)
{
	wstring Tag = _Other->Get_ObjectTag();
	switch (m_tInfo.eState[0])
	{
	default:
		if (Tag == L"PlayerArrow") {
			return Monster::Damaged_by_Arrow(_Other, this);
		}
		break;
	case MONSTER_STATE_SUMMON:
	case MONSTER_STATE_APPEAR:
	case MONSTER_STATE_DEAD:
	case MONSTER_STATE_DISAPPEAR:
	case EVILSLIME_FISSION:
		return 0;
	}

	return FALSE;
}
BOOL ScorpionEvilSoul::OnCollisionStay(GameObject* _Other) {
	wstring Tag = _Other->Get_ObjectTag();
	switch (m_tInfo.eState[0])
	{
	default:
		break;
	case MONSTER_STATE_MINIGAME_IDLE:
	case MONSTER_STATE_MINIGAME_MOVE:
		if (Tag == L"Player") {
			if (static_cast<Player*>(_Other)->Get_Invincible()) {
				return false;
			}
			_vec3 vGravity = Monster::Get_Gravity();
			return	Monster::Hurdle_CollisionStay(this, _Other, (!vGravity.x),(!vGravity.y),(!vGravity.z));
		}
	}
	return FALSE;
}
BOOL ScorpionEvilSoul::OnCollisionExit(GameObject* _Other)
{
	return FALSE;
}
VOID ScorpionEvilSoul::Free() {
	
	Monster::Release_Hurdle(&m_tInfo);
	GameObject::Free();
}

VOID ScorpionEvilSoul::State_Summon(const _float& _DT)
{
	m_tInfo.fTimer[0] += _DT;
	if (nullptr == m_tInfo.pGameObj[0])
	{
		m_tInfo.bTrigger[0] = false;
		_vec3 vPos = { MYPOS->x, 0.01f, MYPOS->z };
		m_tInfo.pGameObj[0] = MonsterEffect::Create(GRPDEV, MONSTER_EFFECT::MONSTER_SUMMONS03, vPos, FALSE, MONSTER_SUMMON03_PLAYTIME);
		//*static_cast<Transform*>(pTarget->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Scale() = *Component_Transform->Get_Scale();
		EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, m_tInfo.pGameObj[0]);
	}

	if (m_tInfo.fTimer[0] >= MONSTER_SUMMON03_PLAYTIME)
	{
		_vec3 vPos = { MYPOS->x, 0.01f, MYPOS->z };
		m_tInfo.fTimer[0] = 0.f;
		m_tInfo.bTrigger[0] = true;
		m_tInfo.pGameObj[0] = MonsterEffect::Create(GRPDEV, MONSTER_EFFECT::MONSTER_SUMMONS01, vPos, FALSE, MONSTER_SUMMON01_PLAYTIME);
		EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, m_tInfo.pGameObj[0]);
		PLAY_MONSTER_EFFECT_ONCE(MONSTER_EFFECT::MONSTER_SUMMONS02, vPos, MONSTER_SUMMON02_PLAYTIME);
		SoundManager::GetInstance()->Play_Sound_Once(L"Monster/Scorpion_Chain.wav", CHANNELID::SOUND_EFFECT04, 0.4f);
	}

	if (m_tInfo.bTrigger[0])
		if (m_tInfo.fTimer[0] >= (MONSTER_SUMMON01_PLAYTIME * 0.5f))
		{
			m_tInfo.Change_State(MONSTER_STATE_IDLE);
			m_tInfo.pGameObj[0] = nullptr;
		}
}

VOID ScorpionEvilSoul::State_Appear(const _float& _DT)
{
	m_tInfo.ID = MonsterManager::Update_Key(m_tInfo.ID, (uint8_t)MONSTER_ANIM::Appear);
	if (FAILED(Monster::Set_TextureList(m_tInfo.ID, &m_tInfo.Textureinfo))) return;

	m_tInfo.Textureinfo._frameTick += _DT;
	if (m_tInfo.Textureinfo._frameTick > FRAMETICK)
	{
		m_tInfo.Textureinfo._frameTick = 0.f;
		++m_tInfo.Textureinfo._frame;
	}
	if (m_tInfo.Textureinfo._frame >= m_tInfo.Textureinfo._Endframe)
	{		
		m_tInfo.ID = MonsterManager::Update_Key(m_tInfo.ID, (uint8_t)MONSTER_ANIM::Stand);
		if (FAILED(Monster::Set_TextureList(m_tInfo.ID, &m_tInfo.Textureinfo))) return;

		m_tInfo.Change_State(MONSTER_STATE_IDLE);
	}
}

VOID ScorpionEvilSoul::State_Idle(const _float& _DT)
{
	if (m_tInfo.pGameObj[0] == nullptr)
		m_tInfo.pGameObj[0] = (Monster::Set_Target(L"Player"));

	_vec3 vDir = *POS(m_tInfo.pGameObj[0]) - *MYPOS;
	vDir.y = 0.f;

	if (D3DXVec3Length(&vDir) < BAT_TRACKINGDIS)
		m_tInfo.Change_State(MONSTER_STATE_TRACKING);
}

VOID ScorpionEvilSoul::State_Tracking(const _float& _DT)
{
	if (m_tInfo.bMiniGame)
	{
		_vec3 vPos = *dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Player")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position();
		m_tInfo.vDirection = vPos - *MYPOS;
		m_tInfo.fSpeed = 3.f;
		return;
	}
	if (nullptr == m_tInfo.pGameObj[0] || m_tInfo.pGameObj[0]->Get_ObjectDead())
		m_tInfo.Change_State(MONSTER_STATE_IDLE);

	m_tInfo.vDirection = *POS(m_tInfo.pGameObj[0]) - *MYPOS;

	_float fDis = D3DXVec3Length(&m_tInfo.vDirection);

	m_tInfo.fSpeed = (fDis > SCORPIONEVILSOUL_TRACKINGMIN) * SCORPIONEVILSOUL_SPEED;

	if (fDis < SCORPIONEVILSOUL_TRACKINGDIS)
	{
		m_tInfo.fTimer[0] += _DT;
		m_tInfo.fTimer[1] = 0.f;
	}
	else
	{
		m_tInfo.fTimer[1] += _DT;
	}

	if (m_tInfo.fTimer[0] >= SCORPIONEVILSOUL_TRACKING_TIME)
	{
		m_tInfo.Change_State(MONSTER_STATE_CASTING);
	}
	else if (m_tInfo.fTimer[1] >= SCORPIONEVILSOUL_LOST_TIME)
	{
		m_tInfo.Change_State(MONSTER_STATE_IDLE);
	}
}

VOID ScorpionEvilSoul::State_Casting(const _float& _DT)
{
	if (nullptr == m_tInfo.pGameObj[0] || m_tInfo.pGameObj[0]->Get_ObjectDead())
		m_tInfo.Change_State(MONSTER_STATE_IDLE);

	m_tInfo.fTimer[0] += _DT;
	m_tInfo.fSpeed = 0.f;

	if (!m_tInfo.bTrigger[1])
	{
		m_tInfo.bTrigger[1] = true;
		MonsterEffect* pEffect = MonsterEffect::Create(GRPDEV, MONSTER_EFFECT::BULLET_STANDARD_CHARGE, *MYPOS, FALSE, SCORPIONEVILSOUL_CASTING_TIME);

		_vec3 vEffectScale = { MYSCALE->x, MYSCALE->x, MYSCALE->x };
		*static_cast<Transform*>(pEffect->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Scale() = vEffectScale * 2.f;
		EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, pEffect);

		SoundManager::GetInstance()->Play_Sound_Once(L"Monster/ScorpionBullet_Casting.wav", CHANNELID::SOUND_EFFECT08, 0.3f);
	}

	if (m_tInfo.fTimer[0] >= SCORPIONEVILSOUL_CASTING_TIME)
	{
		if (m_tInfo.pGameObj[1] == nullptr)
		{
			m_tInfo.pGameObj[1] = Monster::Create<SCORPIONEVILSOUL_BULLET_TYPE>(GRPDEV, *MYPOS, SCORPIONEVILSOUL_BULLET_SCALEMULT);

			SCORPIONEVILSOUL_BULLET_TYPE* pBullet = static_cast<SCORPIONEVILSOUL_BULLET_TYPE*>(m_tInfo.pGameObj[1]);
			pBullet->Set_Master(this);
			pBullet->Get_Info()->fSpeed *= SCORPIONEVILSOUL_BULLET_SPEEDMULT;
			_vec3 vDir = *POS(m_tInfo.pGameObj[0]) - *MYPOS;
			D3DXVec3Normalize(&vDir, &vDir);
			static_cast<ScorpionBullet*>(m_tInfo.pGameObj[1])->Set_Dir(vDir);

			Monster::Add_Monster_to_Scene(m_tInfo.pGameObj[1], L"Monster", GAMEOBJECT_TYPE::OBJECT_MONSTER);
			SoundManager::GetInstance()->Play_Sound_Once(L"Monster/ScorpionBullet_Fire.wav", CHANNELID::SOUND_EFFECT08, 0.3f);
			static_cast<ScorpionBullet*>(m_tInfo.pGameObj[1])->Get_Info()->fSpeed = SCORPIONBULLET_SPEED;
		}

		m_tInfo.bTrigger[1] = false;
		m_tInfo.Change_State(MONSTER_STATE_CHANNELING);
	}
}

VOID ScorpionEvilSoul::State_Channeling(const _float& _DT)
{
	if (nullptr == m_tInfo.pGameObj[0] || m_tInfo.pGameObj[0]->Get_ObjectDead())
		m_tInfo.Change_State(MONSTER_STATE_IDLE);

	m_tInfo.fTimer[0] += _DT;
	m_tInfo.fTimer[1] += _DT;

	if (m_tInfo.fTimer[0] >= SCORPIONEVILSOUL_CHANNELING_TIME)
	{
		m_tInfo.pGameObj[1] = nullptr;
		m_tInfo.Change_State(MONSTER_STATE_IDLE);
	}
}
VOID ScorpionEvilSoul::State_Dead()
{
	PLAY_MONSTER_EFFECT_ONCE(MONSTER_EFFECT::MONSTER_DEATH, *MYPOS, 1.f);
	ObjectDead = true;
}