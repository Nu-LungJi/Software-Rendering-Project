#include "../Include/PCH.h"

ScorpoinEvilSoul::ScorpoinEvilSoul(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
ScorpoinEvilSoul::ScorpoinEvilSoul(const GameObject& _RHS) : GameObject(_RHS) {}
ScorpoinEvilSoul::~ScorpoinEvilSoul() {}

HRESULT ScorpoinEvilSoul::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;

	m_tInfo.eState[0] = MONSTER_STATE_APPEAR;

	Component_Collider->Set_Hp(SCORPIONEVILSOUL_HP);

	m_tInfo.vDirection = { -1.f,0.f,0.f };

	return S_OK;
}
INT	ScorpoinEvilSoul::Update_GameObject(const _float& _DT)
{
	MYPOS->y = MYSCALE->y * 0.5f;
	Component_Collider->Set_Scale(MYSCALE->x * 0.5f, MYSCALE->y, MYSCALE->x * 0.5f);

	if (Component_Collider->Get_Hp() <= 0.f)
		m_tInfo.Change_State(MONSTER_STATE_DISAPPEAR);

	GameObject::Update_GameObject(_DT);

	if (Component_Collider->Get_Hp() <= 0.f)
		m_tInfo.eState[0] = MONSTER_STATE_DISAPPEAR;

	switch (m_tInfo.eState[0])
	{
	default:
		break;
	//case MONSTER_STATE_SUMMON:
	//	ScorpoinEvilSoul::State_Summon(_DT);
	//	break;
	case MONSTER_STATE_APPEAR:
		ScorpoinEvilSoul::State_Appear(_DT);
		break;
	case MONSTER_STATE_DISAPPEAR:

		break;
	case MONSTER_STATE_IDLE:
		ScorpoinEvilSoul::State_Idle(_DT);
		break;
	case MONSTER_STATE_TRACKING:
		ScorpoinEvilSoul::State_Tracking(_DT);
		break;
	case MONSTER_STATE_CASTING:
		ScorpoinEvilSoul::State_Casting(_DT);
		break;
	case MONSTER_STATE_CHANNELING:
		ScorpoinEvilSoul::State_Channeling(_DT);
		break;
	case MONSTER_STATE_DEAD:
		ScorpoinEvilSoul::State_Dead();
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
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);


	if (KEY_DOWN(DIK_P))
	{
		m_tInfo.Change_State(MONSTER_STATE_DEAD);
	}
	return 0;
}
VOID ScorpoinEvilSoul::LateUpdate_GameObject(const _float& _DT) {
	
	GameObject::LateUpdate_GameObject(_DT);
	
	m_tInfo.vDirection.y = 0.f;
	Component_Transform->Move_Pos(D3DXVec3Normalize(&m_tInfo.vDirection, &m_tInfo.vDirection), m_tInfo.fSpeed, _DT);

	m_tInfo.Textureinfo._frameTick += _DT;

	switch (m_tInfo.eState[0])
	{
	default:
		if (FAILED(Monster::Set_TextureList(L"Spr_Monster_BlueScorpionEvilSoul_Stand", &m_tInfo)))
		{
			m_tInfo.Change_State(MONSTER_STATE_DEAD);
			return;
		}

		if (m_tInfo.Textureinfo._frameTick >= FRAMETICK)
		{
			++m_tInfo.Textureinfo._frame %= m_tInfo.Textureinfo._Endframe / 2;
			m_tInfo.Textureinfo._frameTick = 0.f;

			if (fabsf(m_tInfo.vDirection.z) > 0.1f)
				if (m_tInfo.vDirection.z > 0.f)
					m_tInfo.Textureinfo._frame += (m_tInfo.Textureinfo._frame < m_tInfo.Textureinfo._Endframe * 0.5f) * m_tInfo.Textureinfo._Endframe / 2;
		}

		if (FAILED(Monster::Flip_Horizontal(Component_Transform, &m_tInfo.vDirection, SCORPIONEVILSOUL_HORIZONTALFLIP_BUFFER)))
		{
			m_tInfo.Change_State(MONSTER_STATE_DEAD);
			return;
		}

		//if (m_tInfo.Textureinfo._frame < m_tInfo.Textureinfo._Endframe / 2)
		break;

	case MONSTER_STATE_APPEAR:
		if (FAILED(Monster::Set_TextureList(L"Spr_Monster_BlueScorpionEvilSoul_appear", &m_tInfo)))
		{
			m_tInfo.Change_State(MONSTER_STATE_DEAD);
			return;
		}


		break;
	case MONSTER_STATE_DISAPPEAR:
		if (FAILED(Monster::Set_TextureList(L"Spr_Monster_BlueScorpionEvilSoul_disappear", &m_tInfo)))
		{
			m_tInfo.Change_State(MONSTER_STATE_DEAD);
			return;
		}

		m_tInfo.Textureinfo._frameTick += _DT;
		if (m_tInfo.Textureinfo._frameTick > FRAMETICK)
		{
			m_tInfo.Textureinfo._frameTick = 0.f;
			++m_tInfo.Textureinfo._frame;

			if (m_tInfo.Textureinfo._frame >= m_tInfo.Textureinfo._Endframe)
			{
				ObjectDead = true;
			}
		}
		break;
	case MONSTER_STATE_SUMMON:
	case MONSTER_STATE_DEAD:
		break;
	}
	Monster::BillBoard(Component_Transform, GRPDEV);
}
VOID ScorpoinEvilSoul::Render_GameObject() {
	if (!ObjectDead)
	{

		GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

		switch (m_tInfo.eState[0])
		{
		default:
			GRPDEV->SetTexture(0, m_tInfo.Textureinfo._vecTexture[m_tInfo.Textureinfo._frame]);
			Component_Buffer->Render_Buffer();
			break;
		case MONSTER_STATE_SUMMON:
		case MONSTER_STATE_DEAD:
			break;
		}

		GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	}
}
HRESULT ScorpoinEvilSoul::Component_Initialize() {

	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;

	Component_Transform->Set_Pos(0.f, 0.5f, 0.f);
	Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
	Component_Transform->Set_Scale(SCORPIONEVILSOUL_WIDTH, SCORPIONEVILSOUL_HEIGHT, 1.f);
	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);


	return S_OK;
}
ScorpoinEvilSoul* ScorpoinEvilSoul::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	ScorpoinEvilSoul* MST = new ScorpoinEvilSoul(_GRPDEV);
	if (FAILED(MST->Ready_GameObject())) {
		MSG_BOX("Cannot Create ScorpoinEvilSoul.");
		Safe_Release(MST);
		return nullptr;
	}
	return MST;
}
BOOL ScorpoinEvilSoul::OnCollisionEnter(GameObject* _Other)
{
	wstring Tag = _Other->Get_ObjectTag();

	if (Tag == L"PlayerArrow")		Component_Collider->Set_Hp(Component_Collider->Get_Hp() - COLLIDER(_Other)->Get_Att()); return TRUE;

	return FALSE;
}
BOOL ScorpoinEvilSoul::OnCollisionStay(GameObject* _Other)
{
	return FALSE;
}
BOOL ScorpoinEvilSoul::OnCollisionExit(GameObject* _Other)
{
	return FALSE;
}
VOID ScorpoinEvilSoul::Free() {

	GameObject::Free();
}

VOID ScorpoinEvilSoul::State_Summon(const _float& _DT)
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
	}

	if (m_tInfo.bTrigger[0])
		if (m_tInfo.fTimer[0] >= (MONSTER_SUMMON01_PLAYTIME * 0.5f))
		{
			m_tInfo.Change_State(MONSTER_STATE_IDLE);
			m_tInfo.pGameObj[0] = nullptr;
		}
}

VOID ScorpoinEvilSoul::State_Appear(const _float& _DT)
{
	m_tInfo.Textureinfo._frameTick += _DT;
	if (m_tInfo.Textureinfo._frameTick > FRAMETICK)
	{
		m_tInfo.Textureinfo._frameTick = 0.f;

		if (++m_tInfo.Textureinfo._frame >= m_tInfo.Textureinfo._Endframe)
		{
			m_tInfo.Change_State(MONSTER_STATE_IDLE);
		}
	}
}

VOID ScorpoinEvilSoul::State_Idle(const _float& _DT)
{
	if (m_tInfo.pGameObj[0] == nullptr)
		m_tInfo.pGameObj[0] = (Monster::Set_Target(L"Player"));

	_vec3 vDir = *POS(m_tInfo.pGameObj[0]) - *MYPOS;
	vDir.y = 0.f;

	if (D3DXVec3Length(&vDir) < BAT_TRACKINGDIS)
		m_tInfo.Change_State(MONSTER_STATE_TRACKING);
}

VOID ScorpoinEvilSoul::State_Tracking(const _float& _DT)
{
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

VOID ScorpoinEvilSoul::State_Casting(const _float& _DT)
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
		*static_cast<Transform*>(pEffect->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Scale() = vEffectScale;
		EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, pEffect);
	}

	if (m_tInfo.fTimer[0] >= SCORPIONEVILSOUL_CASTING_TIME)
	{
		m_tInfo.bTrigger[1] = false;
		m_tInfo.Change_State(MONSTER_STATE_CHANNELING);
	}
}

VOID ScorpoinEvilSoul::State_Channeling(const _float& _DT)
{
	if (nullptr == m_tInfo.pGameObj[0] || m_tInfo.pGameObj[0]->Get_ObjectDead())
		m_tInfo.Change_State(MONSTER_STATE_IDLE);

	m_tInfo.fTimer[0] += _DT;
	m_tInfo.fTimer[1] += _DT;

	if (m_tInfo.pGameObj[1] == nullptr)
	{
		m_tInfo.pGameObj[1] = Monster::Create<SCORPIONEVILSOUL_BULLET_TYPE>(GRPDEV, *MYPOS, SCORPIONEVILSOUL_BULLET_SCALEMULT);
		
		SCORPIONEVILSOUL_BULLET_TYPE* pBullet = static_cast<SCORPIONEVILSOUL_BULLET_TYPE*>(m_tInfo.pGameObj[1]);
		pBullet->Set_Master(this);
		pBullet->Get_Info()->fSpeed *= SCORPIONEVILSOUL_BULLET_SPEEDMULT;

		m_tInfo.pGameObj[1]->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
		m_tInfo.pGameObj[1]->Set_ObjectTag(L"ScorpionBullet");

		PLAY_MONSTER_EFFECT_ONCE(MONSTER_EFFECT::BULLET_STANDARD_CHARGE, *MYPOS, SCORPIONEVILSOUL_CHANNELING_TIME);
		//SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(m_tInfo.pGameObj[1]);
	}

	if (m_tInfo.fTimer[0] >= SCORPIONEVILSOUL_CHANNELING_TIME)
	{
		_vec3 vDir = *POS(m_tInfo.pGameObj[0]) - *MYPOS;
		D3DXVec3Normalize(&vDir, &vDir);
		static_cast<ScorpionBullet*>(m_tInfo.pGameObj[1])->Set_Dir(vDir);
		
		//CollisionManager::GetInstance()->Add_ColliderObject(m_tInfo.pGameObj[1]);

		Monster::Add_Monster_to_Scene(m_tInfo.pGameObj[1],L"MonsterBullet", GAMEOBJECT_TYPE::OBJECT_MONSTER);

		static_cast<ScorpionBullet*>(m_tInfo.pGameObj[1])->Get_Info()->fSpeed = SCORPIONBULLET_SPEED;

		m_tInfo.pGameObj[1] = nullptr;
		m_tInfo.Change_State(MONSTER_STATE_IDLE);
	}
}
VOID ScorpoinEvilSoul::State_Dead()
{
	PLAY_MONSTER_EFFECT_ONCE(MONSTER_EFFECT::MONSTER_DEATH, *MYPOS, 1.f);
	
	ObjectDead = true;
}