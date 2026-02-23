#include "../Include/PCH.h"

EvilSlime::EvilSlime(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
EvilSlime::EvilSlime(const GameObject& _RHS) : GameObject(_RHS) {}
EvilSlime::~EvilSlime() {}

HRESULT EvilSlime::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;

	m_tInfo.eState[0] = MONSTER_STATE_SUMMON;
	Component_Collider->Set_Hp(EVILSLIME_HP);
	Component_Collider->Set_Att(1.f);
	m_tInfo.vDirection = { -1.f,0.f,0.f };

	return S_OK;
}
INT	EvilSlime::Update_GameObject(const _float& _DT)
{
	MYPOS->y = MYSCALE->y * 0.5f;
	Component_Collider->Set_Scale(MYSCALE->x * 0.5f, MYSCALE->y, MYSCALE->x * 0.5f);

	// <플레이어 업데이트 시점>

	GameObject::Update_GameObject(_DT);


	if (KEY_DOWN(DIK_H))
	{
		m_tInfo.bTrigger[2] = true;
		m_tInfo.Change_State(EVILSLIME_FISSION);
	}


	if (Component_Collider->Get_Hp() <= 0.f)
	{
		if (MYSCALE->x < 0.5f)
		{
			m_tInfo.Change_State(MONSTER_STATE_DEAD);
		}
		else
		{
			m_tInfo.bTrigger[2] = true;
			m_tInfo.Change_State(EVILSLIME_FISSION);
		}
	}


	switch (m_tInfo.eState[0])
	{
	default:
		break;
	case EVILSLIME_FISSION:
		EvilSlime::State_Fission(_DT);
		break;
	case MONSTER_STATE_SUMMON:
		EvilSlime::State_Summon(_DT);
		break;
	case MONSTER_STATE_IDLE:
		EvilSlime::State_Idle(_DT);
		break;
	case MONSTER_STATE_TRACKING:
		EvilSlime::State_Tracking(_DT);
		break;
	case MONSTER_STATE_CASTING:
		EvilSlime::State_Casting(_DT);
		break;
	case MONSTER_STATE_CHANNELING:
		EvilSlime::State_Channeling(_DT);
		break;
	case MONSTER_STATE_DEAD:
		EvilSlime::State_Dead();
		break;
	}


	if (ObjectDead)
	{
		for (int i = 0; i < _countof(m_tInfo.pGameObj); ++i)
			if (m_tInfo.pGameObj[i] != nullptr) m_tInfo.pGameObj[i]->Set_ObjectDead(true);

		return -1;
	}

	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}
VOID EvilSlime::LateUpdate_GameObject(const _float& _DT) {

	GameObject::LateUpdate_GameObject(_DT);

	m_tInfo.vDirection.y = 0.f;
	Component_Transform->Move_Pos(D3DXVec3Normalize(&m_tInfo.vDirection, &m_tInfo.vDirection), m_tInfo.fSpeed, _DT);

	m_tInfo.Textureinfo._frameTick += _DT;

	switch (m_tInfo.eState[0])
	{
	default:
		if (FAILED(Monster::Set_TextureList(L"Spr_Monster_BlueEvilSlime_Move", &m_tInfo)))
		{
			m_tInfo.Change_State(MONSTER_STATE_DEAD);
			return;
		}

		if (m_tInfo.Textureinfo._frameTick > 2.f * D3DX_PI)
			m_tInfo.Textureinfo._frameTick -= (2.f * D3DX_PI);
		
		{
			_float fNormalized = m_tInfo.Textureinfo._frameTick / (2.f * D3DX_PI);
			_float fTriangle = 1.f - fabsf(fNormalized * 2.f - 1.f);

			m_tInfo.Textureinfo._frame = (int)(fTriangle * 4.99f);
		}
		break;

	case MONSTER_STATE_TRACKING:
		if (FAILED(Monster::Set_TextureList(L"Spr_Monster_BlueEvilSlime_Move", &m_tInfo)))
		{
			m_tInfo.Change_State(MONSTER_STATE_DEAD);
			return;
		}

		if (m_tInfo.Textureinfo._frameTick >= FRAMETICK)
		{
			++m_tInfo.Textureinfo._frame %= m_tInfo.Textureinfo._Endframe;
			m_tInfo.Textureinfo._frameTick = 0.f;
		}
		break;

	case	MONSTER_STATE_CASTING:
	case MONSTER_STATE_CHANNELING:
		if (FAILED(Monster::Set_TextureList(L"Spr_Monster_BlueEvilSlime_Attack", &m_tInfo)))
		{
			m_tInfo.Change_State(MONSTER_STATE_DEAD);
			return;
		}

		if (m_tInfo.Textureinfo._frameTick >= FRAMETICK)
		{
			if(m_tInfo.fTimer[0] <= FRAMETICK * 5.f || m_tInfo.fTimer[0] >= EVILSLIME_CASTING_TIME - FRAMETICK * 4.f)
				++m_tInfo.Textureinfo._frame %= m_tInfo.Textureinfo._Endframe;
			m_tInfo.Textureinfo._frameTick = 0.f;
		}
		break;
	case MONSTER_STATE_APPEAR:
	case MONSTER_STATE_SUMMON:
	case MONSTER_STATE_DEAD:
		break;
	}
	Monster::BillBoard(Component_Transform, GRPDEV);
}
VOID EvilSlime::Render_GameObject() {
	if (m_tInfo.Textureinfo._Endframe < m_tInfo.Textureinfo._frame) return;
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	switch (m_tInfo.eState[0])
	{
	default:
		GRPDEV->SetTexture(0, m_tInfo.Textureinfo._vecTexture[m_tInfo.Textureinfo._frame]);
		Component_Buffer->Render_Buffer();
		break;
	case MONSTER_STATE_APPEAR:
	case MONSTER_STATE_SUMMON:
	case MONSTER_STATE_DEAD:
		break;
	}

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
HRESULT EvilSlime::Component_Initialize() {

	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;

	Component_Transform->Set_Pos(10.f, 0.5f, 0.f);
	Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
	Component_Transform->Set_Scale(EVILSLIME_WIDTH, EVILSLIME_HEIGHT, 1.f);

	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);
	Component_Collider->Set_Scale(EVILSLIME_WIDTH, 1.f, EVILSLIME_HEIGHT);

	return S_OK;
}
EvilSlime* EvilSlime::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	EvilSlime* MST = new EvilSlime(_GRPDEV);
	if (FAILED(MST->Ready_GameObject())) {
		MSG_BOX("Cannot Create EvilSlime.");
		Safe_Release(MST);
		return nullptr;
	}
	return MST;
}
BOOL EvilSlime::OnCollisionEnter(GameObject* _Other)
{
	wstring Tag;
	switch (m_tInfo.eState[0])
	{
	default:
		Tag = _Other->Get_ObjectTag();
		if (Tag == L"PlayerArrow")		Component_Collider->Set_Hp(Component_Collider->Get_Hp() - COLLIDER(_Other)->Get_Att()); return TRUE;
		return 0;
	case MONSTER_STATE_SUMMON:
	case MONSTER_STATE_APPEAR:
	case MONSTER_STATE_DEAD:
	case MONSTER_STATE_DISAPPEAR:
	case MONSTER_STATE_CASTING:
	case EVILSLIME_FISSION:
		return 0;
	}

	return FALSE;
}
BOOL EvilSlime::OnCollisionStay(GameObject* _Other)
{
	//switch (_Other->Get_ObjectType())
	//{
	//default:
	//	break;
	//case GAMEOBJECT_TYPE::OBJECT_MONSTER:
	//	_vec3	vDir = *POS(_Other) - *MYPOS;
	//	if (D3DXVec3Dot(&m_tInfo.vDirection, &vDir) > 0.f)
	//	{
	//		switch (m_tInfo.eState[0])
	//		{
	//		default:
	//			m_tInfo.fSpeed = 0.f;
	//			break;
	//		case MONSTER_STATE_CASTING:
	//		case MONSTER_STATE_CHANNELING:
	//			break;
	//		}
	//	}
	//}
	return FALSE;
}
BOOL EvilSlime::OnCollisionExit(GameObject* _Other)
{
	return FALSE;
}
VOID EvilSlime::Free() {

	GameObject::Free();
}

VOID EvilSlime::State_Summon(const _float& _DT)
{
	m_tInfo.fTimer[0] += _DT;
	if (FAILED(MonsterEffect::Monster_SummonEffect_Set(GRPDEV, Component_Transform, &m_tInfo.bTrigger[0], &m_tInfo.fTimer[0]))) { ObjectDead = true; return; }
	if (m_tInfo.bTrigger[0] > 3)	m_tInfo.Change_State(MONSTER_STATE_IDLE);
}

VOID EvilSlime::State_Idle(const _float& _DT)
{
	if (m_tInfo.pGameObj[0] == nullptr)
		m_tInfo.pGameObj[0] = (Monster::Set_Target(L"Player"));

	_vec3 vDir = *POS(m_tInfo.pGameObj[0]) - *MYPOS;
	vDir.y = 0.f;

	if (D3DXVec3Length(&vDir) < BAT_TRACKINGDIS)
		m_tInfo.Change_State(MONSTER_STATE_TRACKING);
}

VOID EvilSlime::State_Tracking(const _float& _DT)
{
	if (nullptr == m_tInfo.pGameObj[0] || m_tInfo.pGameObj[0]->Get_ObjectDead())
		m_tInfo.Change_State(MONSTER_STATE_IDLE);

	m_tInfo.vDirection = *POS(m_tInfo.pGameObj[0]) - *MYPOS;

	_float fDis = D3DXVec3Length(&m_tInfo.vDirection);

	m_tInfo.fSpeed = (fDis > SCORPIONEVILSOUL_TRACKINGMIN) * SCORPIONEVILSOUL_SPEED;

	if (fDis < EVILSLIME_TRACKINGDIS)
	{
		m_tInfo.fTimer[0] += _DT;
		m_tInfo.fTimer[1] = 0.f;
	}
	else
	{
		m_tInfo.fTimer[1] += _DT;
	}

	if (m_tInfo.fTimer[0] >= EVILSLIME_TRACKING_TIME)
	{
		m_tInfo.Change_State(MONSTER_STATE_CASTING);
	}
	else if (m_tInfo.fTimer[1] >= EVILSLIME_LOST_TIME)
	{
		m_tInfo.Change_State(MONSTER_STATE_IDLE);
	}
}

VOID EvilSlime::State_Casting(const _float& _DT)
{

	if (nullptr == m_tInfo.pGameObj[0] || m_tInfo.pGameObj[0]->Get_ObjectDead())
		m_tInfo.Change_State(MONSTER_STATE_IDLE);

	if (nullptr == m_tInfo.pGameObj[1])
	{
		_vec3 vPos = *POS(m_tInfo.pGameObj[0]);
		vPos *= RANDOM::Get_float(1.1f, 0.9f, this);

		_vec2 vDis = { MYSCALE->x, MYSCALE->y };
		_float fDis = D3DXVec2Length(&vDis);
		_vec3 vDir = vPos - *MYPOS;
		
		m_tInfo.fSpeed = D3DXVec3Length(&vDir) / EVILSLIME_CASTING_TIME;
		D3DXVec3Normalize(&m_tInfo.vDirection, &vDir);

		m_tInfo.pGameObj[1] = Monster::Create<EVILSLIME_BULLET_TYPE>(GRPDEV,vPos);

		for (int i = 1; i < EVILSLIME_BULLET_NUM + 1; ++i)
		{
			if (i > 1)
			{
				_float fRadian = (i - 1.5f) * D3DX_PI / 2.f;
				_vec3 vDst = vPos;
				_float fDst = (i > 5) ? fDis * 2.f : fDis;
				vDst.x += cosf(fRadian) * fDst;
				vDst.z += sinf(fRadian) * fDst;
				m_tInfo.pGameObj[i] = Monster::Create<EVILSLIME_BULLET_TYPE>(GRPDEV, vDst, EVILSLIME_BULLET_SCALEMULT);
			}	

			if (m_tInfo.pGameObj[i] == nullptr)
			{
				m_tInfo.Change_State(MONSTER_STATE_IDLE);

				for (int i = 0; i < _countof(m_tInfo.pGameObj); ++i)
				{
					m_tInfo.pGameObj[i]->Set_ObjectDead(true);
					m_tInfo.pGameObj[i] = nullptr;
				}
				break;
			}

			EVILSLIME_BULLET_TYPE* pBullet = static_cast<EVILSLIME_BULLET_TYPE*>(m_tInfo.pGameObj[i]);
			pBullet->Set_Master(this);
			pBullet->Get_Info()->fTimer[1] = EVILSLIME_CASTING_TIME;
			*SCALE(m_tInfo.pGameObj[i]) = *MYSCALE;

			Monster::Add_Monster_to_Scene(m_tInfo.pGameObj[i], L"MonsterBullet", GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);

		}
	}
	else
	{
		m_tInfo.fTimer[0] += _DT;
		MYPOS->y += EVILSIME_JUMP_HEIGHT * sinf(D3DX_PI * (m_tInfo.fTimer[0] / EVILSLIME_CASTING_TIME));
		for (int i = 1; i < EVILSLIME_BULLET_NUM + 1; ++i)
		{
			static_cast<EVILSLIME_BULLET_TYPE*>(m_tInfo.pGameObj[i])->Get_Info()->fTimer[0] = m_tInfo.fTimer[0];
		}
	}

	if (m_tInfo.fTimer[0] >= EVILSLIME_CASTING_TIME)
	{
		for (int i = 1; i < EVILSLIME_BULLET_NUM + 1; ++i) {
			static_cast<EVILSLIME_BULLET_TYPE*>(m_tInfo.pGameObj[i])->Get_Info()->bTrigger[0] = true;
			m_tInfo.pGameObj[i] = nullptr;
		}
		m_tInfo.Change_State(MONSTER_STATE_CHANNELING);
	}
}

VOID EvilSlime::State_Channeling(const _float& _DT)
{
	if (nullptr == m_tInfo.pGameObj[0] || m_tInfo.pGameObj[0]->Get_ObjectDead())
		m_tInfo.Change_State(MONSTER_STATE_IDLE);

	m_tInfo.fTimer[0] += _DT;

	if (m_tInfo.fTimer[0] >= EVILSLIME_CHANNELING_TIME)
	{
		for (int i = 0; i < EVILSLIME_BULLET_NUM; ++i)
		{
			//m_tInfo.pGameObj[1] = Monster::Create<Bullet_Standard>(GRPDEV, *MYPOS);

			//_float fRadian = 2.f * D3DX_PI * ((_float)i / EVILSLIME_BULLET_NUM);
			//fRadian = (fRadian > D3DX_PI) ? fRadian - (2.f * D3DX_PI) : fRadian;

			//static_cast<Bullet_Standard*>(m_tInfo.pGameObj[1])->Set_Dir(cosf(fRadian), 0.f, sinf(fRadian));

			//Monster::Add_Monster_to_Scene(m_tInfo.pGameObj[1], GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
		}
		m_tInfo.pGameObj[1] = nullptr;
		m_tInfo.Change_State(MONSTER_STATE_IDLE);
	}
}
VOID EvilSlime::State_Dead()
{
	PLAY_MONSTER_EFFECT_ONCE(MONSTER_EFFECT::MONSTER_DEATH, *MYPOS, 1.f);
	TileManager::GetInstance()->Set_StageArray();
	ObjectDead = true;
}

VOID EvilSlime::State_Fission(const _float& _DT)
{
	m_tInfo.fTimer[0] += _DT;
	if (m_tInfo.fTimer[0] <= 0.5f) {
		_float fHeight = (m_vFissionDst.y - MYSCALE->y * 0.5f) * sinf(D3DX_PI * (m_tInfo.fTimer[0] / 0.5f));
		MYPOS->y += fHeight;
	}
	else
	{
		m_tInfo.Change_State(MONSTER_STATE_IDLE);
	}

	if (m_tInfo.bTrigger[2]) {
		for (int i = 0; i < 4; ++i)
		{
			_vec3 vDst = { MYPOS->x + ((MYSCALE->x * RANDOM::Get_float(MYPOS->x, -MYPOS->x,this))),
							MYSCALE->y,
							MYPOS->z + ((MYSCALE->y * RANDOM::Get_float(MYPOS->z, -MYPOS->z,this)))};

			m_tInfo.pGameObj[i] = Monster::Create<EvilSlime>(GRPDEV, *MYPOS);

			*SCALE(m_tInfo.pGameObj[i]) = *MYSCALE * 0.5f;

			EvilSlime* pSlime = static_cast<EvilSlime*>(m_tInfo.pGameObj[i]);

			pSlime->Get_Info()->Change_State(EVILSLIME_FISSION);

			*pSlime->Get_FissionDst() = vDst;

			_vec3 vSpeed = { MYSCALE->x * 0.25f, 0.f, MYSCALE->y * 0.25f };
			pSlime->Get_Info()->fSpeed = D3DXVec3Length(&vSpeed) / 0.5f;

			vDst -= *MYPOS;
			D3DXVec3Normalize(&pSlime->Get_Info()->vDirection, &vDst);

			Monster::Add_Monster_to_Scene(m_tInfo.pGameObj[i], L"Monster", GAMEOBJECT_TYPE::OBJECT_MONSTER);
		}

		m_tInfo.bTrigger[2] = false;
		m_tInfo.pGameObj[0] = m_tInfo.pGameObj[1] = m_tInfo.pGameObj[2] = m_tInfo.pGameObj[3] = nullptr;
		ObjectDead = true;
	}
}
