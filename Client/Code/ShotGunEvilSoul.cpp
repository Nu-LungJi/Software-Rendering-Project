#include "../Include/PCH.h"

ShotGunEvilSoul::ShotGunEvilSoul(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
ShotGunEvilSoul::ShotGunEvilSoul(const GameObject& _RHS) : GameObject(_RHS) {}
ShotGunEvilSoul::~ShotGunEvilSoul() {}
HRESULT ShotGunEvilSoul::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;


	return S_OK;
}
HRESULT ShotGunEvilSoul::Ready_GameObject(_vec3 vPos, BOOL bMini) {
	if (FAILED(Component_Initialize())) return E_FAIL;

	m_tInfo.eState[0] = MONSTER_STATE_SUMMON;
	Component_Collider->Set_Hp(SHOTGUNEVILSOUL_HP);

	m_tInfo.bMiniGame = bMini;
	if (m_tInfo.bMiniGame)
	{
		ObjectTAG = L"Monster";
		m_tInfo.eState[0] = MONSTER_STATE_TRACKING;
		Component_Transform->Set_Pos(vPos);
	}
	return S_OK;
}
INT	ShotGunEvilSoul::Update_GameObject(const _float& _DT)
{
	if (m_tInfo.bMiniGame)
	{// 창준 추가
		GameObject::Update_GameObject(_DT);
		ShotGunEvilSoul::State_Tracking(_DT);
		MYPOS->y = MYSCALE->y * 0.5f;
		Component_Collider->Set_Scale(MYSCALE->x * 0.5f, 1.f, MYSCALE->x * 0.5f);
		RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

		return 1;
	}
  
	if (m_tInfo.eState[0] == MONSTER_STATE_MINIGAME_IDLE) {
		ObjectDead = false;
		return 0;
	}
	else if (m_tInfo.eState[0] == MONSTER_STATE_MINIGAME_MOVE) {
		ObjectDead = false;
		return 0;
	}
	else
	{
		MYPOS->y = MYSCALE->y * 0.5f;
	}

	Component_Collider->Set_Scale(MYSCALE->x * 0.5f, MYSCALE->y, MYSCALE->x * 0.5f);

	Component_Buffer->Update_Component(_DT);
	Component_Collider->Update_Component(_DT);
	
	if (Component_Collider->Get_Hp() <= 0.f)
		m_tInfo.eState[0] = MONSTER_STATE_DEAD;
	switch (m_tInfo.eState[0])
	{
	default:
		break;
	case MONSTER_STATE_SUMMON:
		ShotGunEvilSoul::State_Summon(_DT);
		break;
	case MONSTER_STATE_IDLE:
		ShotGunEvilSoul::State_Idle(_DT);
		break;
	case MONSTER_STATE_TRACKING:
		ShotGunEvilSoul::State_Tracking(_DT);
		break;
	case MONSTER_STATE_CASTING:
		ShotGunEvilSoul::State_Casting(_DT);
		break;
	case MONSTER_STATE_CHANNELING:
		ShotGunEvilSoul::State_Channeling(_DT);
		break;
	case MONSTER_STATE_DEAD:
		ShotGunEvilSoul::State_Dead();
		break;
	}


	if (ObjectDead)
		return -1;


	return 0;
}
VOID ShotGunEvilSoul::LateUpdate_GameObject(const _float& _DT) {

	GameObject::LateUpdate_GameObject(_DT);

	
	m_tInfo.vDirection.y = 0.f;
	Component_Transform->Move_Pos(D3DXVec3Normalize(&m_tInfo.vDirection, &m_tInfo.vDirection), m_tInfo.fSpeed, _DT);

	switch (m_tInfo.eState[0])
	{
	default:
		m_tInfo.Textureinfo._frameTick += _DT;
		if (m_tInfo.Textureinfo._frameTick >= FRAMETICK)
		{
			m_tInfo.Textureinfo._frameTick = 0.f;

			_uint HalfFrame = (m_tInfo.Textureinfo._Endframe + 1) * 0.5f;
			_uint BaseFrame = (m_tInfo.Textureinfo._frame + 1) % HalfFrame;
			_uint Offset = (fabsf(m_tInfo.vDirection.z) > 0.1f) * (m_tInfo.vDirection.z > 0.f) * HalfFrame;

			m_tInfo.Textureinfo._frame = BaseFrame + Offset;
		}
		if (FAILED(Monster::Flip_Horizontal(Component_Transform, &m_tInfo.vDirection, SHOTGUNEVILSOUL_HORIZONTALFLIP_BUFFER)))
		{
			m_tInfo.Change_State(MONSTER_STATE_DEAD);
			return;
		}

		if (m_tInfo.Textureinfo._frame < m_tInfo.Textureinfo._Endframe / 2)
		break;

	case MONSTER_STATE_APPEAR:
	case MONSTER_STATE_SUMMON:
	case MONSTER_STATE_DEAD:
		break;
	}
	if (static_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Camera"))->IsIn_Frustum(*MYPOS, 10.f)) {
		Monster::Flip_Horizontal(Component_Transform, &m_tInfo.vDirection, BAT_HORIZONTALFLIP_BUFFER);
		AlphaZValue = Monster::BillBoard(Component_Transform, GRPDEV);
		RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	}
}
VOID ShotGunEvilSoul::Render_GameObject() {

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	if (m_tInfo.bMiniGame)
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

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
HRESULT ShotGunEvilSoul::Component_Initialize() {

	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;

	Component_Transform->Set_Pos(10.f, 0.5f, 0.f);
	Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
	Component_Transform->Set_Scale(SHOTGUNEVILSOUL_WIDTH, SHOTGUNEVILSOUL_HEIGHT, 1.f);
	
	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);
	Component_Collider->Set_Hp(SHOTGUNEVILSOUL_HP);

	m_tInfo.eState[0] = MONSTER_STATE_SUMMON;

	m_tInfo.ID = MonsterManager::Make_Key((uint8_t)MONSTER_SEP::Monster,
										(uint8_t)MONSTER_TYPE::ShotGunEvilSoul,
										(uint8_t)MONSTER_ANIM::Stand);

	return Monster::Set_TextureList(m_tInfo.ID, &m_tInfo.Textureinfo);
}
ShotGunEvilSoul* ShotGunEvilSoul::Create(LPDIRECT3DDEVICE9 _GRPDEV, _vec3 vPos, BOOL bMini) {
	ShotGunEvilSoul* MST = new ShotGunEvilSoul(_GRPDEV);
	if (FAILED(MST->Ready_GameObject(vPos, bMini))) {
		MSG_BOX("Cannot Create ShotGunEvilSoul.");
		Safe_Release(MST);
		return nullptr;
	}
	return MST;
}
BOOL ShotGunEvilSoul::OnCollisionEnter(GameObject* _Other)
{
	wstring Tag = _Other->Get_ObjectTag();

	switch (m_tInfo.eState[0])
	{
	default:
		if (Tag == L"PlayerArrow") {

			Component_Collider->Set_Hp(Component_Collider->Get_Hp() - COLLIDER(_Other)->Get_Att());
			return TRUE;
		}
		else if (Tag == L"Player")
		{
			Component_Collider->Set_Hp(Component_Collider->Get_Hp() - COLLIDER(_Other)->Get_Att());
			return TRUE;
		}
		break;	
	case MONSTER_STATE_APPEAR:
	case MONSTER_STATE_SUMMON:
	case MONSTER_STATE_DISAPPEAR:
	case MONSTER_STATE_DEAD:
	case MONSTER_STATE_MINIGAME_IDLE:
	case MONSTER_STATE_MINIGAME_MOVE:
		break;
	}

	return FALSE;
}
BOOL ShotGunEvilSoul::OnCollisionStay(GameObject* _Other)
{
	wstring Tag = _Other->Get_ObjectTag();
	switch (m_tInfo.eState[0])
	{
	default:
		break;
	case MONSTER_STATE_MINIGAME_IDLE:
	case MONSTER_STATE_MINIGAME_MOVE:
		if (Tag == L"Player")
			return	Monster::Hurdle_CollisionStay(this, _Other);
	}
	return FALSE;
}
BOOL ShotGunEvilSoul::OnCollisionExit(GameObject* _Other)
{
	return FALSE;
}
VOID ShotGunEvilSoul::Free() {

	GameObject::Free();
}

VOID ShotGunEvilSoul::State_Summon(const _float& _DT)
{
	m_tInfo.fTimer[0] += _DT;
	if (FAILED(MonsterEffect::Monster_SummonEffect_Set(GRPDEV, Component_Transform, &m_tInfo.bTrigger[0], &m_tInfo.fTimer[0]))) { ObjectDead = true; return; }
	if (m_tInfo.bTrigger[0] > 3)	m_tInfo.Change_State(MONSTER_STATE_IDLE);
}

VOID ShotGunEvilSoul::State_Idle(const _float& _DT)
{
	if (m_tInfo.pGameObj[0] == nullptr)
		m_tInfo.pGameObj[0] = (Monster::Set_Target(L"Player"));

	_vec3 vDir = *POS(m_tInfo.pGameObj[0]) - *MYPOS;
	vDir.y = 0.f;

	if (D3DXVec3Length(&vDir) < SHOTGUNEVILSOUL_TRACKINGDIS)
		m_tInfo.Change_State(MONSTER_STATE_TRACKING);
}

VOID ShotGunEvilSoul::State_Tracking(const _float& _DT)
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

	if (fDis < SHOTGUNEVILSOUL_TRACKINGDIS)
	{
		m_tInfo.fTimer[0] += _DT;
		m_tInfo.fTimer[1] = 0.f;
	}
	else
	{
		m_tInfo.fTimer[1] += _DT;
	}

	if (m_tInfo.fTimer[0] >= SHOTGUNEVILSOUL_TRACKING_TIME)
	{
		m_tInfo.Change_State(MONSTER_STATE_CASTING);
	}
	else if (m_tInfo.fTimer[1] >= SHOTGUNEVILSOUL_LOST_TIME)
	{
		m_tInfo.Change_State(MONSTER_STATE_IDLE);
	}
}

VOID ShotGunEvilSoul::State_Casting(const _float& _DT)
{
	if (nullptr == m_tInfo.pGameObj[0] || m_tInfo.pGameObj[0]->Get_ObjectDead())
		m_tInfo.Change_State(MONSTER_STATE_IDLE);

	m_tInfo.fTimer[0] += _DT;
	m_tInfo.fSpeed = 0.f;

	if (m_tInfo.fTimer[0] >= SHOTGUNEVILSOUL_CASTING_TIME)
	{
		m_tInfo.Change_State(MONSTER_STATE_CHANNELING);
	}
}

VOID ShotGunEvilSoul::State_Channeling(const _float& _DT)
{
	if (nullptr == m_tInfo.pGameObj[0] || m_tInfo.pGameObj[0]->Get_ObjectDead())
		m_tInfo.Change_State(MONSTER_STATE_IDLE);

	if (m_tInfo.fTimer[0] == 0.f)
	{
		_vec3	vDir = *POS(m_tInfo.pGameObj[0]) - *MYPOS;
		_float	fBaseRadian = atan2f(vDir.z, vDir.x);

		for (int i = 0; i < SHOTGUNEVILSOUL_BULLET_NUM; ++i)
		{
			_float fRandom = D3DXToRadian(RANDOM::Get_float((SHOTGUNEVILSOUL_SPREAD * -0.5f), (SHOTGUNEVILSOUL_SPREAD * 0.5f), this));

			m_tInfo.pGameObj[i+1] = Monster::Create<SHOTGUNEVILSOUL_BULLET_TYPE>(GRPDEV, *MYPOS, SHOTGUNEVILSOUL_BULLET_SCALEMULT);

			_float fRadian = fBaseRadian + fRandom;
			static_cast<SHOTGUNEVILSOUL_BULLET_TYPE*>(m_tInfo.pGameObj[i+1])->Set_Dir(cosf(fRadian), 0.f, sinf(fRadian));

			Monster::Add_Monster_to_Scene(m_tInfo.pGameObj[i+1], L"MonsterBullet", GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
		}
	}

	m_tInfo.fTimer[0] += _DT;

	if (m_tInfo.fTimer[0] >= SHOTGUNEVILSOUL_CHANNELING_TIME)
	{
		for (int i = 0; i < SHOTGUNEVILSOUL_BULLET_NUM; ++i)
		{
			static_cast<SHOTGUNEVILSOUL_BULLET_TYPE*>(m_tInfo.pGameObj[i + 1])->Get_Info()->bTrigger[0] = true;
			m_tInfo.pGameObj[i + 1] = nullptr;
		}
		m_tInfo.Change_State(MONSTER_STATE_IDLE);
	}
	else
	{
		for (int i = 0; i < SHOTGUNEVILSOUL_BULLET_NUM; ++i)
		{
			SHOTGUNEVILSOUL_BULLET_TYPE* pBullet = static_cast<SHOTGUNEVILSOUL_BULLET_TYPE*>(m_tInfo.pGameObj[i + 1]);
			pBullet->Get_Info()->fSpeed = pBullet->Get_Info()->fTimer[1] * cosf((D3DX_PI /2.f) * (m_tInfo.fTimer[0] / SHOTGUNEVILSOUL_CHANNELING_TIME));
		}
	}
}
VOID ShotGunEvilSoul::State_Dead()
{
	EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER,
		MonsterEffect::Create(GRPDEV, MONSTER_EFFECT::MONSTER_DEATH, *MYPOS, MYSCALE->x));
	for (int i = 1; i < _countof(m_tInfo.pGameObj); ++i)
	{
		if (m_tInfo.pGameObj[i] != nullptr)
			m_tInfo.pGameObj[i]->Set_ObjectDead(true);
	}
	TileManager::GetInstance()->Set_StageArray();
	ObjectDead = true;
}