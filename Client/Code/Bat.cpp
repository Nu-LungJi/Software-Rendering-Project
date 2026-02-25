#include "../Include/PCH.h"

Bat::Bat(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
Bat::Bat(const GameObject& _RHS) : GameObject(_RHS) {}
Bat::~Bat() {}

HRESULT Bat::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;

	m_tInfo.eState[0] = MONSTER_STATE_SUMMON;

	Component_Collider->Set_Hp(BAT_HP);
	Component_Collider->Set_Att(1.f);

	return S_OK;
}
INT	Bat::Update_GameObject(const _float& _DT)
{
	if (m_tInfo.eState[0] == MONSTER_STATE_MINIGAME_IDLE)	{
		ObjectDead = false;	
		return 0;
	}
	else if (m_tInfo.eState[0] == MONSTER_STATE_MINIGAME_MOVE)	{
		ObjectDead = false;
		return 0;
	}
	else
	{
		MYPOS->y = MYSCALE->y * 0.5f;
	}

	Component_Collider->Set_Scale(MYSCALE->x * 0.5f, MYSCALE->y, MYSCALE->x * 0.5f);

	if (Component_Collider->Get_Hp() <= 0.f)
		m_tInfo.Change_State(MONSTER_STATE_DEAD);

	GameObject::Update_GameObject(_DT);

	if (Component_Collider->Get_Hp() <= 0.f)
		m_tInfo.eState[0] = MONSTER_STATE_DEAD;

	switch (m_tInfo.eState[0])
	{
	//case MONSTER_STATE_APPEAR:
	//	Bat::State_Appear(_DT);
	//	break;
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
	default:
		break;
	}

	if (ObjectDead)
		return -1;

	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	return 0;
}
VOID Bat::LateUpdate_GameObject(const _float& _DT) {
	GameObject::LateUpdate_GameObject(_DT);

	Monster::Set_TextureList(L"Spr_Monster_BlueEvilBat", &m_tInfo.Textureinfo);
	m_tInfo.Textureinfo._frameTick += _DT;
	if (m_tInfo.Textureinfo._frameTick > FRAMETICK)
	{
		m_tInfo.Textureinfo._frameTick = 0.f;
		++m_tInfo.Textureinfo._frame %= m_tInfo.Textureinfo._Endframe / 2;
	}

	switch (m_tInfo.eState[0])
	{
	default:
		m_tInfo.vDirection.y = 0.f;
		break;
	case MONSTER_STATE_MINIGAME_IDLE:
	case MONSTER_STATE_MINIGAME_MOVE:
		break;
	}
	Component_Transform->Move_Pos(D3DXVec3Normalize(&m_tInfo.vDirection, &m_tInfo.vDirection), m_tInfo.fSpeed, _DT);

	Monster::Flip_Horizontal(Component_Transform, &m_tInfo.vDirection, BAT_HORIZONTALFLIP_BUFFER);

	AlphaZValue = Monster::BillBoard(Component_Transform, GRPDEV);
}
VOID Bat::Render_GameObject() {
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());
	
	switch (m_tInfo.eState[0])
	{
	default:
		GRPDEV->SetTexture(0, m_tInfo.Textureinfo._vecTexture[m_tInfo.Textureinfo._frame]);
		Component_Buffer->Render_Buffer();
		break;
	case MONSTER_STATE_SUMMON:
	case MONSTER_STATE_APPEAR:
	case MONSTER_STATE_DEAD:
		break;
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

	return S_OK;
}
Bat* Bat::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	Bat* MST = new Bat(_GRPDEV);
	if (FAILED(MST->Ready_GameObject())) {
		MSG_BOX("Cannot Create Bat.");
		Safe_Release(MST);
		return nullptr;
	}
	return MST;
}
BOOL Bat::OnCollisionEnter(GameObject* _Other)
{
	wstring Tag = _Other->Get_ObjectTag();

	if (Tag == L"PlayerArrow") {
		Component_Collider->Set_Hp(Component_Collider->Get_Hp() - COLLIDER(_Other)->Get_Att());
		return TRUE;
	}

	return FALSE;
}
BOOL Bat::OnCollisionStay(GameObject* _Other)
{
	return FALSE;
}
BOOL Bat::OnCollisionExit(GameObject* _Other)
{
	return FALSE;
}
VOID Bat::Free() {

	GameObject::Free();
}

VOID Bat::State_Summon(const _float& _DT)
{
	m_tInfo.fTimer[0] += _DT;
	if (FAILED(MonsterEffect::Monster_SummonEffect_Set(GRPDEV, Component_Transform, &m_tInfo.bTrigger[0], &m_tInfo.fTimer[0]))) { ObjectDead = true; return; }
	if (m_tInfo.bTrigger[0] > 3)	m_tInfo.Change_State(MONSTER_STATE_IDLE);
}

//VOID Bat::State_Appear(const _float& _DT)
//{
//	m_tInfo.fTimer[0] += _DT;
//	if (nullptr == m_tInfo.pGameObj[0])
//	{
//		m_tInfo.bTrigger[0] = false;
//		_vec3 vPos = *MYPOS;
//		vPos.z += 0.001f;
//
//		m_tInfo.pGameObj[0] = MonsterEffect::Create(GRPDEV, MONSTER_EFFECT::MONSTER_SUMMONS03, 
//			vPos, MYSCALE->x, MONSTER_SUMMON03_PLAYTIME, false);
//
//		EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, m_tInfo.pGameObj[0]);
//	}
//
//	if (m_tInfo.fTimer[0] >= MONSTER_SUMMON03_PLAYTIME)
//	{
//		_vec3 vPos = *MYPOS;
//		vPos.z += 0.001f;
//		m_tInfo.fTimer[0] = 0.f;
//		m_tInfo.bTrigger[0] = true;
//		m_tInfo.pGameObj[0] = MonsterEffect::Create(GRPDEV, MONSTER_EFFECT::MONSTER_SUMMONS01, 
//			vPos, MYSCALE->x, MONSTER_SUMMON01_PLAYTIME, false);
//		EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, m_tInfo.pGameObj[0]);
//		
//		
//		EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, 
//			MonsterEffect::Create(GRPDEV, MONSTER_EFFECT::MONSTER_SUMMONS02, 
//				*MYPOS, MYSCALE->x, MONSTER_SUMMON02_PLAYTIME, false));
//	}
//
//	if (m_tInfo.bTrigger[0])
//		if (m_tInfo.fTimer[0] >= (MONSTER_SUMMON01_PLAYTIME * 0.5f))
//		{
//			m_tInfo.Change_State(MONSTER_STATE_IDLE);
//			m_tInfo.pGameObj[0] = nullptr;
//		}
//}

VOID Bat::State_Idle()
{
	if (m_tInfo.pGameObj[0] == nullptr)
		m_tInfo.pGameObj[0]=(Monster::Set_Target(L"Player"));

	_vec3 vDir = *POS(m_tInfo.pGameObj[0]) - *MYPOS;
	vDir.y = 0.f;

	if (D3DXVec3Length(&vDir) < BAT_TRACKINGDIS)
		m_tInfo.Change_State(MONSTER_STATE_TRACKING);
}

VOID Bat::State_Tracking(const _float& _DT)
{
	if (nullptr == m_tInfo.pGameObj[0] || m_tInfo.pGameObj[0]->Get_ObjectDead())
		m_tInfo.Change_State(MONSTER_STATE_IDLE);

	m_tInfo.fSpeed = BAT_SPEED;
	m_tInfo.vDirection = *POS(m_tInfo.pGameObj[0]) - *MYPOS;

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

	m_tInfo.fTimer[0]	+= _DT;
	m_tInfo.fSpeed		 = 0.f;
	
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

		m_tInfo.pGameObj[1] = Monster::Create<BAT_BULLET_TYPE>(GRPDEV, { MYPOS->x, 0.5f, MYPOS->z });

		BAT_BULLET_TYPE* pBullet = static_cast<BAT_BULLET_TYPE*>(m_tInfo.pGameObj[1]);
		pBullet->Set_Master(this);
		_vec3 vDir = *POS(m_tInfo.pGameObj[0]) - *MYPOS;
		D3DXVec3Normalize(&vDir, &vDir);
		pBullet->Set_Dir(vDir);
		pBullet->Get_Info()->fSpeed *= BAT_BULLET_SPEEDMULT;

		Monster::Add_Monster_to_Scene(m_tInfo.pGameObj[1], L"MonsterBullet", GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);


		MonsterEffect* pEffect = MonsterEffect::Create(GRPDEV, MONSTER_EFFECT::BULLET_STANDARD_CHARGE, *MYPOS, FALSE, BAT_CHANNELING_TIME-m_tInfo.fTimer[0]);
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
