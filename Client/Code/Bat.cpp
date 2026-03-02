#include "../Include/PCH.h"

Bat::Bat(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
Bat::Bat(const GameObject& _RHS) : GameObject(_RHS) {}
Bat::~Bat() {}
HRESULT Bat::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;

	Component_Collider->Set_Hp(BAT_HP);
	Component_Collider->Set_Att(1.f);

	ObjectTAG = L"Monster";
	return S_OK;
}
HRESULT Bat::Ready_GameObject(_vec3 vPos, BOOL bMini) {
	if (FAILED(Component_Initialize())) return E_FAIL;

	m_tInfo.eState[0] = MONSTER_STATE_SUMMON;
	m_tInfo.bMiniGame = bMini;
	Component_Collider->Set_Hp(BAT_HP);
	Component_Collider->Set_Att(1.f);

	if (m_tInfo.bMiniGame)
	{// ??? ???
		ObjectTAG = L"Monster";
		Component_Transform->Set_Pos(vPos);
	}

	return S_OK;
}
INT	Bat::Update_GameObject(const _float& _DT)
{
	if (m_tInfo.bMiniGame)
	{
		GameObject::Update_GameObject(_DT);
		Bat::State_Tracking(_DT);
		MYPOS->y = 1.f;
		Component_Collider->Set_Scale(MYSCALE->x * 0.5f, 1.f, MYSCALE->x * 0.5f);
		RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
		return 1;
	}

	Monster::Minigame_Update(_DT, &m_tInfo, MYPOS);


	MYPOS->y = MYSCALE->y * 0.5f;
	Component_Collider->Set_Scale(MYSCALE->x * 0.5f, 1.f, MYSCALE->x * 0.5f);


	if (Component_Collider->Get_Hp() <= 0.f)
		m_tInfo.Change_State(MONSTER_STATE_DEAD);

	Component_Buffer->Update_Component(_DT);
	Component_Collider->Update_Component(_DT);

	if (Component_Collider->Get_Hp() <= 0.f)
		m_tInfo.eState[0] = MONSTER_STATE_DEAD;

	switch (m_tInfo.eState[0])
	{
	case MONSTER_STATE_SUMMON:
		Bat::State_Summon(_DT);
		break;
	case MONSTER_STATE_IDLE:
		Bat::State_Idle();
		break;
	case MONSTER_STATE_TRACKING:
		Bat::State_Tracking(_DT);
		break;
	case MONSTER_STATE_CASTING:
		Bat::State_Casting(_DT);
		break;
	case MONSTER_STATE_CHANNELING:
		Bat::State_Channeling(_DT);
		break;
	case MONSTER_STATE_DEAD:
		Bat::State_Dead();
		break;
	case MONSTER_STATE_MINIGAME_MOVE:

		break;
	default:
		break;
	}

	if (ObjectDead)
		return -1;

	return 0;
}
VOID Bat::LateUpdate_GameObject(const _float& _DT) {
	GameObject::LateUpdate_GameObject(_DT);

	Component_Transform->Move_Pos(&m_tInfo.vDirection, m_tInfo.fSpeed, _DT);
	m_tInfo.Textureinfo._frameTick += _DT;

	switch (m_tInfo.eState[0])
	{
	default:
		m_tInfo.vDirection.y = 0.f;
		if (m_tInfo.Textureinfo._frameTick >= FRAMETICK)
		{
			m_tInfo.Textureinfo._frameTick = 0.f;

			_uint HalfFrame = (m_tInfo.Textureinfo._Endframe + 1) * 0.5f;
			_uint BaseFrame = (m_tInfo.Textureinfo._frame + 1) % HalfFrame;
			_uint Offset = (fabsf(m_tInfo.vDirection.z) > 0.1f) * (m_tInfo.vDirection.z > 0.f) * HalfFrame;

			m_tInfo.Textureinfo._frame = BaseFrame + Offset;
		}

		if (FAILED(Monster::Flip_Horizontal(Component_Transform, &m_tInfo.vDirection, SCORPIONEVILSOUL_HORIZONTALFLIP_BUFFER)))
		{
			m_tInfo.Change_State(MONSTER_STATE_DEAD);
			return;
		}

		break;
	}

	if (Monster::Minigame_LateUpdate(_DT, &m_tInfo) ||
		(static_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Camera"))
			->IsIn_Frustum(*MYPOS, 10.f))) {
		Monster::Flip_Horizontal(Component_Transform, &m_tInfo.vDirection, BAT_HORIZONTALFLIP_BUFFER);
		AlphaZValue = Monster::BillBoard(Component_Transform, GRPDEV);
		RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	}
}
VOID Bat::Render_GameObject() {
	if (m_tInfo.Textureinfo._frame > m_tInfo.Textureinfo._Endframe) return;

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
HRESULT Bat::Component_Initialize() {

	Component_Transform = ADD_COMPONENT_TRANSFORM;
	Component_Buffer = ADD_COMPONENT_RECTTEX;

	Component_Transform->Set_Pos(10.f, 0.112f, 10.f);
	Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
	Component_Transform->Set_Scale(BAT_WIDTH, BAT_HEIGHT, 1.f);

	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);

	Component_Collider->Set_Scale(BAT_WIDTH, 1.f, BAT_HEIGHT);

	m_tInfo.ID = MonsterManager::Make_Key((uint8_t)MONSTER_SEP::Monster,
		(uint8_t)MONSTER_TYPE::Bat,
		(uint8_t)MONSTER_ANIM::Stand);
	return Monster::Set_TextureList(m_tInfo.ID, &m_tInfo.Textureinfo);
}
Bat* Bat::Create(LPDIRECT3DDEVICE9 _GRPDEV, _vec3 vPos, BOOL bMini) {
	Bat* MST = new Bat(_GRPDEV);
	if (FAILED(MST->Ready_GameObject(vPos, bMini))) {
		MSG_BOX("Cannot Create Bat.");
		Safe_Release(MST);
		return nullptr;
	}
	return MST;
}
BOOL Bat::OnCollisionEnter(GameObject* _Other)
{
	wstring Tag = _Other->Get_ObjectTag();

	switch (m_tInfo.eState[0])
	{
	default:
		if (Tag == L"PlayerArrow") {
			Component_Collider->Set_Hp(Component_Collider->Get_Hp() - COLLIDER(_Other)->Get_Att());
			return TRUE;
			break;
		}
	}

	return FALSE;
}
BOOL Bat::OnCollisionStay(GameObject* _Other)
{
	wstring Tag = _Other->Get_ObjectTag();
	switch (m_tInfo.eState[0])
	{
	default:
	case MONSTER_STATE_MINIGAME_IDLE:
	case MONSTER_STATE_MINIGAME_MOVE:
		if (Tag == L"Player") {
			Player* pPlayer = static_cast<Player*>(_Other);
			_vec3* vPlayer = POS(pPlayer);
			_vec3 vDir = *vPlayer - *MYPOS;
			//_float fDis = D3DXVec3Length(&vDir);
			//D3DXVec3Normalize(&vDir, &vDir);
			vPlayer->x += vDir.x;
			vPlayer->z += vDir.y;
			return true;
		}
		break;
	}
	return FALSE;
}
BOOL Bat::OnCollisionExit(GameObject* _Other)
{
	return FALSE;
}
VOID Bat::Free() {

	Monster::Release_Hurdle(&m_tInfo);
	GameObject::Free();
}

VOID Bat::State_Summon(const _float& _DT)
{
	m_tInfo.fTimer[0] += _DT;
	if (FAILED(MonsterEffect::Monster_SummonEffect_Set(GRPDEV, Component_Transform, &m_tInfo.bTrigger[0], &m_tInfo.fTimer[0]))) { ObjectDead = true; return; }
	if (m_tInfo.bTrigger[0] > 3)	m_tInfo.Change_State(MONSTER_STATE_IDLE);
}

VOID Bat::State_Idle()
{
	if (m_tInfo.pGameObj[0] == nullptr)
		m_tInfo.pGameObj[0] = (Monster::Set_Target(L"Player"));

	_vec3 vDir = *POS(m_tInfo.pGameObj[0]) - *MYPOS;
	vDir.y = 0.f;

	if (D3DXVec3Length(&vDir) < BAT_TRACKINGDIS)
		m_tInfo.Change_State(MONSTER_STATE_TRACKING);
}

VOID Bat::State_Tracking(const _float& _DT)
{
	m_tInfo.fSpeed = BAT_SPEED;
	_vec3 vDir = *POS(m_tInfo.pGameObj[0]) - *MYPOS;
	D3DXVec3Normalize(&m_tInfo.vDirection, &vDir);

	if (m_tInfo.bMiniGame)
	{
		_vec3 vPos = *dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Player")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position();
		m_tInfo.vDirection = vPos - *MYPOS;
	  D3DXVec3Normalize(&m_tInfo.vDirection, &vDir);
		m_tInfo.fSpeed = 3.f;
		return;
	}
	if (nullptr == m_tInfo.pGameObj[0] || m_tInfo.pGameObj[0]->Get_ObjectDead())
		m_tInfo.Change_State(MONSTER_STATE_IDLE);


	if (D3DXVec3Length(&m_tInfo.vDirection) < BAT_TRACKINGDIS)
	{
		m_tInfo.fTimer[0] += _DT;
		m_tInfo.fTimer[1] = 0.f;
	}
	else
	{
		m_tInfo.fTimer[1] += _DT;
	}

	if (m_tInfo.fTimer[0] >= BAT_TRACKING_TIME)
	{
		m_tInfo.Change_State(MONSTER_STATE_CASTING);
	}
	else if (m_tInfo.fTimer[1] >= BAT_LOST_TIME)
	{
		m_tInfo.Change_State(MONSTER_STATE_IDLE);
	}

}
VOID Bat::State_Casting(const _float& _DT)
{
	if (nullptr == m_tInfo.pGameObj[0] || m_tInfo.pGameObj[0]->Get_ObjectDead())
		m_tInfo.Change_State(MONSTER_STATE_IDLE);

	m_tInfo.fTimer[0] += _DT;
	m_tInfo.fSpeed = 0.f;

	if (m_tInfo.fTimer[0] >= BAT_CASTING_TIME)
	{
		m_tInfo.Change_State(MONSTER_STATE_CHANNELING);
	}
}

VOID Bat::State_Channeling(const _float& _DT)
{
	Component_Collider->Set_Att(1.f);

	if (nullptr == m_tInfo.pGameObj[0] || m_tInfo.pGameObj[0]->Get_ObjectDead())
		m_tInfo.Change_State(MONSTER_STATE_IDLE);

	m_tInfo.fSpeed = BAT_RUSHSPEED;
	m_tInfo.fTimer[0] += _DT;
	m_tInfo.fTimer[1] += _DT;

	if (m_tInfo.fTimer[1] >= BAT_CHANNELING_TIME / (BAT_BULLET_NUM + 1))
	{
		m_tInfo.fTimer[1] = 0.f;

		m_tInfo.pGameObj[1] = Monster::Create<BAT_BULLET_TYPE>(GRPDEV, { MYPOS->x, 0.5f, MYPOS->z }, MYSCALE->x * 0.5f);

		BAT_BULLET_TYPE* pBullet = static_cast<BAT_BULLET_TYPE*>(m_tInfo.pGameObj[1]);
		pBullet->Set_Master(this);
		_vec3 vDir = *POS(m_tInfo.pGameObj[0]) - *MYPOS;
		D3DXVec3Normalize(&vDir, &vDir);
		pBullet->Set_Dir(vDir);
		pBullet->Get_Info()->fSpeed *= BAT_BULLET_SPEEDMULT;

		Monster::Add_Monster_to_Scene(m_tInfo.pGameObj[1], L"MonsterBullet", GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);


		MonsterEffect* pEffect = MonsterEffect::Create(GRPDEV, MONSTER_EFFECT::BULLET_STANDARD_CHARGE, *MYPOS, FALSE, BAT_CHANNELING_TIME - m_tInfo.fTimer[0]);
		_float fScale = 2.f * SCALE(m_tInfo.pGameObj[1])->x;
		_vec3 vEffectScale = { fScale, fScale, fScale };
		*static_cast<Transform*>(pEffect->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Scale() = vEffectScale;
		EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, pEffect);

		m_tInfo.pGameObj[1] == nullptr;
	}

	if (m_tInfo.fTimer[0] >= BAT_CHANNELING_TIME)
	{
		Component_Collider->Set_Att(0.f);
		m_tInfo.Change_State(MONSTER_STATE_IDLE);
	}
}

VOID Bat::State_Dead()
{
	PLAY_MONSTER_EFFECT_ONCE(MONSTER_EFFECT::MONSTER_DEATH, *MYPOS, 1.f);
	TileManager::GetInstance()->Set_StageArray();
	ObjectDead = true;
}
