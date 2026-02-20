//#include "../Include/PCH.h"
//
//Docheol::Docheol(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
//Docheol::Docheol(const GameObject& _RHS)	: GameObject(_RHS)	{}
//Docheol::~Docheol() {}
//
//HRESULT Docheol::Ready_GameObject() {
//	if (FAILED(Component_Initialize())) return E_FAIL;
//
//	m_tInfo.Change_State(BOSS_DOCHEOL_SUMMON);
//
//	Component_Collider->Set_Scale(MYSCALE->x * 0.5f, 1.f, MYSCALE->y * 0.5f);
//	return S_OK;
//}
//INT	Docheol::Update_GameObject(const _float& _DT)
//{
//	// <플레이어 업데이트 시점>
//	m_tInfo.fTimer[0] += _DT;
//	m_tInfo.fTimer[1] += _DT;
//	m_tInfo.fTimer[2] += _DT;
//	m_tInfo.fTimer[3] += _DT;
//
//	switch (m_tInfo.eState[0])
//	{
//	case BOSS_DOCHEOL_SUMMON:
//		Docheol::State_Summon(_DT);
//		break;
//	case BOSS_DOCHEOL_HANDUPAPPEAR:
//		Docheol::State_HandUpAppear(_DT);
//		break;
//	case BOSS_DOCHEOL_APPEAR:
//		Docheol::State_Appear(_DT);
//		break;
//	case BOSS_DOCHEOL_IDLE:
//		Docheol::State_Idle(_DT);
//		break;
//	case BOSS_DOCHEOL_TRACKING:
//		Docheol::State_Tracking(_DT);
//		break;
//	//case BOSS_DOCHEOL_SLAM:
//	//	Docheol::State_Slam(_DT);
//	//	break;
//	case BOSS_DOCHEOL_PUNCH:
//		Docheol::State_Punch(_DT);
//		break;
//	case BOSS_DOCHEOL_METEOR:
//	case BOSS_DOCHEOL_SLAM:
//		Docheol::State_Meteor(_DT);
//		break;
//
//	default:
//		break;
//	}
//
//	GameObject::Update_GameObject(_DT);
//
//	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
//	return 0;
//}
//VOID Docheol::LateUpdate_GameObject(const _float& _DT) {
//	
//	GameObject::LateUpdate_GameObject(_DT);
//
//	switch (m_tInfo.eState[0])
//	{
//	default:
//	case BOSS_DOCHEOL_SUMMON:
//		break;
//	case BOSS_DOCHEOL_HANDUPAPPEAR:	
//		if (FAILED(Monster::Set_TextureList(L"Spr_Boss_Docheol_HandUpAppear", &m_tInfo)))
//		{
//			ObjectDead = true;
//			return;
//		}
//		break;
//	case BOSS_DOCHEOL_APPEAR:
//		if (FAILED(Monster::Set_TextureList(L"Spr_Boss_Docheol_Appear", &m_tInfo)))
//		{
//			ObjectDead = true;
//			return;
//		}
//		break;
//	case BOSS_DOCHEOL_IDLE:
//	case BOSS_DOCHEOL_TRACKING:
//		if (FAILED(Monster::Set_TextureList(L"Spr_Boss_Docheol_Stand", &m_tInfo)))
//		{
//			ObjectDead = true;
//			return;
//		}
//		break;
//	case BOSS_DOCHEOL_SLAM:
//	case BOSS_DOCHEOL_METEOR:
//		if (FAILED(Monster::Set_TextureList(L"Spr_Boss_Docheol_TwoHandSlam", &m_tInfo)))
//		{
//			ObjectDead = true;
//			return;
//		}
//		break;
//	case BOSS_DOCHEOL_PUNCH:
//		if (FAILED(Monster::Set_TextureList(L"Spr_Boss_Docheol_RightSwing", &m_tInfo)))
//		{
//			ObjectDead = true;
//			return;
//		}
//		break;
//	}
//
//	Monster::Flip_Horizontal(Component_Transform, &m_tInfo.vDirection, 0.1f);
//
//	Monster::BillBoard(Component_Transform, GRPDEV);
//}
//VOID Docheol::Render_GameObject() {
//	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
//	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());
//
//
//	switch (m_tInfo.eState[0])
//	{
//	default:
//		GRPDEV->SetTexture(0, m_tInfo.Textureinfo._vecTexture[m_tInfo.Textureinfo._frame]);
//		Component_Buffer->Render_Buffer();
//		break;
//	case BOSS_DOCHEOL_SUMMON:
//		break;
//	}
//
//
//	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
//}
//HRESULT Docheol::Component_Initialize() {
//
//	Component_Buffer = ADD_COMPONENT_RECTTEX;
//	Component_Transform = ADD_COMPONENT_TRANSFORM;
//
//	Component_Transform->Set_Scale(5.51f, 5.07f, 1.f);
//
//	Component_Collider = ADD_COMPONENT_COLLIDER;
//	Component_Collider->Set_CenterPos(Component_Transform);
//
//	return S_OK;
//}
//
//Docheol* Docheol::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
//	Docheol* MST = new Docheol(_GRPDEV);
//	if (FAILED(MST->Ready_GameObject())) {
//		MSG_BOX("Cannot Create Docheol.");
//		Safe_Release(MST);
//		return nullptr;
//	}
//	return MST;
//}
//VOID Docheol::Free() 
//{
//	GameObject::Free();
//}
//
//VOID Docheol::State_Summon(const _float& _DT)
//{
//	if (m_tInfo.pGameObj[0] == nullptr)
//	{
//		m_tInfo.pGameObj[0] = Monster::Create<DocheolAppearMark>(GRPDEV, *MYPOS);
//		m_tInfo.pGameObj[0]->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
//		static_cast<DocheolAppearMark*>(m_tInfo.pGameObj[0])->Get_Info()->pGameObj[0] = this;
//		Monster::Add_Monster_to_Scene(m_tInfo.pGameObj[0], GAMEOBJECT_TYPE::OBJECT_MONSTER_EFFECT);
//	}
//
//	if (m_tInfo.pGameObj[0] != nullptr)
//	{
//		if (static_cast<DocheolAppearMark*>(m_tInfo.pGameObj[0])->Get_Info()->bTrigger[0])
//		{
//			m_tInfo.pGameObj[0] = nullptr;
//			m_tInfo.Change_State(BOSS_DOCHEOL_HANDUPAPPEAR);
//		}
//	}
//}
//
//VOID Docheol::State_HandUpAppear(const _float& _DT)
//{
//	m_tInfo.Textureinfo._frameTick += _DT;
//
//	if (m_tInfo.Textureinfo._frameTick >= FRAMETICK)
//	{
//		m_tInfo.Textureinfo._frameTick = 0.f;
//		if (++m_tInfo.Textureinfo._frame >= m_tInfo.Textureinfo._Endframe)
//		{
//			m_tInfo.Change_State(BOSS_DOCHEOL_APPEAR);
//		}
//	}
//}
//
//VOID Docheol::State_Appear(const _float& _DT)
//{
//	m_tInfo.Textureinfo._frameTick += _DT;
//
//	if (m_tInfo.Textureinfo._frameTick > FRAMETICK)
//	{
//		m_tInfo.Textureinfo._frameTick = 0.f;
//		if (++m_tInfo.Textureinfo._frame >= m_tInfo.Textureinfo._Endframe)
//		{
//			m_tInfo.Change_State(BOSS_DOCHEOL_IDLE);
//		}
//	}
//
//}
//VOID Docheol::State_Idle(const _float& _DT)
//{
//	m_tInfo.Textureinfo._frameTick += _DT;
//
//	if (m_tInfo.Textureinfo._frameTick >= FRAMETICK)
//	{
//		m_tInfo.Textureinfo._frameTick = 0.f;
//		++m_tInfo.Textureinfo._frame %= m_tInfo.Textureinfo._Endframe;
//	}
//
//	if (Monster::Set_Target(L"Player", m_tInfo.pGameObj[0]) != nullptr)
//	{
//		m_tInfo.Change_State(BOSS_DOCHEOL_TRACKING);
//	}
//}
//VOID Docheol::State_Tracking(const _float& _DT)
//{
//	if (nullptr == m_tInfo.pGameObj[0])
//	{
//		m_tInfo.Change_State(BOSS_DOCHEOL_IDLE);
//	}
//
//	_vec3 vDir = *POS(m_tInfo.pGameObj[0]) - *MYPOS;
//	D3DXVec3Normalize(&m_tInfo.vDirection, &vDir);
//
//	m_tInfo.Textureinfo._frameTick += _DT;
//	if (m_tInfo.Textureinfo._frameTick >= FRAMETICK)
//	{
//		m_tInfo.Textureinfo._frameTick = 0.f;
//		++m_tInfo.Textureinfo._frame %= m_tInfo.Textureinfo._Endframe;
//	}
//
//	if (m_tInfo.fTimer[0] >= DOCHEOL_COOLDOWN)
//	{
//		if (m_tInfo.bTrigger[0])
//		{
//
//		}
//		else
//		{
//			if (m_tInfo.bTrigger[2])
//				m_tInfo.Change_State(BOSS_DOCHEOL_PUNCH);
//			else if (m_tInfo.bTrigger[3])
//				m_tInfo.Change_State(BOSS_DOCHEOL_METEOR);
//			else
//				m_tInfo.Change_State(BOSS_DOCHEOL_SLAM);
//		}
//	}
//}
//
//VOID Docheol::State_Slam(const _float& _DT)
//{
//	m_tInfo.Textureinfo._frameTick += _DT;
//	if (m_tInfo.Textureinfo._frameTick > FRAMETICK)
//	{
//		m_tInfo.Textureinfo._frameTick = 0.f;
//		if (++m_tInfo.Textureinfo._frame >= m_tInfo.Textureinfo._Endframe)
//		{
//			// EFFECT & BULLET
//			m_tInfo.bTrigger[1] = !m_tInfo.bTrigger[1];
//			if (!m_tInfo.bTrigger)
//				m_tInfo.bTrigger[2] = true;
//
//			m_tInfo.Change_State(BOSS_DOCHEOL_TRACKING);
//		}
//	}
//}
//
//
//
//VOID Docheol::State_Punch(const _float& _DT)
//{
//	m_tInfo.Textureinfo._frameTick += _DT;
//	if (m_tInfo.Textureinfo._frameTick >= FRAMETICK)
//	{
//		m_tInfo.Textureinfo._frameTick = 0.f;
//
//		if (!m_tInfo.bTrigger[0])
//		{
//			if (++m_tInfo.Textureinfo._frame >= m_tInfo.Textureinfo._Endframe / 2 - 1
//				&& m_tInfo.Textureinfo._frame <= m_tInfo.Textureinfo._Endframe / 2 + 1)
//			{
//				_float fSpread = D3DXToRadian(DOCHEOL_PUNCH_FIREBALL_ANGLE);
//				_float fBaseRadian = atan2f(m_tInfo.vDirection.y, m_tInfo.vDirection.x);
//
//				for (int i = 0; i < DOCHEOL_PUNCH_FIREBALL_NUM; ++i)
//				{
//					_float fRadian = fBaseRadian - (fSpread * 0.5f) + (fSpread * i / DOCHEOL_PUNCH_FIREBALL_NUM);
//
//					m_tInfo.pGameObj[1] = Monster::Create<Fireball>(GRPDEV, { MYPOS->x, 0.5f, MYPOS->y }, 2.f);
//
//					Fireball* pBullet = static_cast<Fireball*>(m_tInfo.pGameObj[1]);
//
//					pBullet->Set_Master(this);
//					pBullet->Set_Dir({ cosf(fRadian), 0.f, sinf(fRadian) });
//
//					Monster::Add_Monster_to_Scene(m_tInfo.pGameObj[1], GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
//
//					m_tInfo.pGameObj[1] = nullptr;
//				}
//			}
//		}
//		else
//		{
//			if ((++m_tInfo.Textureinfo._frame >= 3.f && m_tInfo.Textureinfo._frame <= 5.f)
//				|| (m_tInfo.Textureinfo._frame >= 8.f  && m_tInfo.Textureinfo._frame <= 10.f))
//			{
//				_float fSpread = D3DXToRadian(DOCHEOL_PUNCH_FIREBALL_ANGLE);
//				_float fBaseRadian = atan2f(m_tInfo.vDirection.y, m_tInfo.vDirection.x);
//
//				for (int i = 0; i < DOCHEOL_PUNCH_FIREBALL_NUM; ++i)
//				{
//					_float fRadian = fBaseRadian - (fSpread * 0.5f) + (fSpread * i / DOCHEOL_PUNCH_FIREBALL_NUM);
//
//					m_tInfo.pGameObj[1] = Monster::Create<Fireball>(GRPDEV, { MYPOS->x, 0.5f, MYPOS->y }, 2.f);
//
//					Fireball* pBullet = static_cast<Fireball*>(m_tInfo.pGameObj[1]);
//
//					pBullet->Set_Master(this);
//					pBullet->Set_Dir({ cosf(fRadian), 0.f, sinf(fRadian) });
//
//					Monster::Add_Monster_to_Scene(m_tInfo.pGameObj[1], GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
//
//					m_tInfo.pGameObj[1] = nullptr;
//				}
//			}
//		}
//	}
//}
//
//VOID Docheol::State_Meteor(const _float& _DT)
//{
//	m_tInfo.Textureinfo._frameTick += _DT;
//	if (m_tInfo.Textureinfo._frameTick > FRAMETICK)
//	{
//		m_tInfo.Textureinfo._frameTick = 0.f;
//		if (++m_tInfo.Textureinfo._frame >= m_tInfo.Textureinfo._Endframe)
//		{
//			uint64_t Seed[2] = { (uint64_t)time(NULL), GetTickCount64() };
//			for (int i = 0; i < DOCHEOL_METEOR_NUM; ++i)
//			{
//				_vec3 vPos = { (Monster::XorShift128plus(Seed[0], Seed[1]) % 1000) / 1000.f, 0.f, MYPOS->z + 50.f };
//				m_tInfo.pGameObj[1] = Monster::Create<Fireball>(GRPDEV, vPos, 3.f);
//
//				_float fx = (Monster::XorShift128plus(Seed[0], Seed[1]) % 1000) / 100;
//				_float fz = (Monster::XorShift128plus(Seed[0], Seed[1]) % 1000) / 100;
//
//				vPos = *POS(m_tInfo.pGameObj[0]);
//				vPos.x += fx - 5.f;
//				vPos.y = 0.01f;
//				vPos.z += fz - 5.f;
//
//				m_tInfo.pGameObj[2] = Monster::Create<Alert>(GRPDEV, vPos);
//				
//				*SCALE(m_tInfo.pGameObj[2]) = *SCALE(m_tInfo.pGameObj[1]);
//
//				Fireball* pFireball = static_cast<Fireball*>(m_tInfo.pGameObj[1]);
//				Alert* pAlert = static_cast<Alert*>(m_tInfo.pGameObj[2]);
//				
//				pAlert->Get_Info()->fTimer[0] = 1.f;
//				pAlert->Get_Info()->pGameObj[0] = m_tInfo.pGameObj[1];
//
//				_vec3 vDir = *POS(m_tInfo.pGameObj[2]) - *POS(m_tInfo.pGameObj[1]);
//
//				pFireball->Get_Info()->fSpeed = D3DXVec3Length(&vDir);
//				D3DXVec3Normalize(&pFireball->Get_Info()->vDirection, &vDir);
//
//				Monster::Add_Monster_to_Scene(m_tInfo.pGameObj[1], GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
//				Monster::Add_Monster_to_Scene(m_tInfo.pGameObj[2], GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
//
//				m_tInfo.pGameObj[1] = m_tInfo.pGameObj[2] = nullptr;
//			}
//
//			m_tInfo.Change_State(BOSS_DOCHEOL_TRACKING);
//			m_tInfo.bTrigger[1] = m_tInfo.bTrigger[2] = m_tInfo.bTrigger[3] = false;
//		}
//	}
//}