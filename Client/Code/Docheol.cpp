#include "../Include/PCH.h"

Docheol::Docheol(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
Docheol::Docheol(const GameObject& _RHS)	: GameObject(_RHS)	{}
Docheol::~Docheol() {}

HRESULT Docheol::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;

	m_tInfo.Change_State(MONSTER_STATE_SUMMON);

	Component_Collider->Set_Scale(MYSCALE->x * 0.5f, 1.f, MYSCALE->y * 0.5f);
	return S_OK;
}
INT	Docheol::Update_GameObject(const _float& _DT)
{
	// <플레이어 업데이트 시점>
	m_tInfo.fTimer[0] += _DT;
	m_tInfo.fTimer[1] += _DT;
	m_tInfo.fTimer[2] += _DT;
	m_tInfo.fTimer[3] += _DT;

	switch (m_tInfo.eState[0])
	{
	case BOSS_DOCHEOL_SUMMON:
		Docheol::State_Summon(_DT);
		break;
	case BOSS_DOCHEOL_HANDUPAPPEAR:
		Docheol::State_HandUpAppear(_DT);
	case BOSS_DOCHEOL_APPEAR:
		Docheol::State_Appear(_DT);
		break;
	default:
		break;
	}

	GameObject::Update_GameObject(_DT);

	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	return 0;
}
VOID Docheol::LateUpdate_GameObject(const _float& _DT) {
	GameObject::LateUpdate_GameObject(_DT);
	Monster::BillBoard(Component_Transform, GRPDEV);
}
VOID Docheol::Render_GameObject() {
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	GRPDEV->SetTexture(0, m_tInfo.Textureinfo._vecTexture[m_tInfo.Textureinfo._frame]);

	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
HRESULT Docheol::Component_Initialize() {

	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;

	Component_Transform->Set_Scale(5.51f, 5.07f, 1.f);

	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);

	return S_OK;
}

Docheol* Docheol::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	Docheol* MST = new Docheol(_GRPDEV);
	if (FAILED(MST->Ready_GameObject())) {
		MSG_BOX("Cannot Create Docheol.");
		Safe_Release(MST);
		return nullptr;
	}
	return MST;
}
VOID Docheol::Free() 
{
	GameObject::Free();
}

VOID Docheol::State_Summon(const _float& _DT)
{
	if (!m_tInfo.bTrigger[0])
	{
		m_tInfo.bTrigger[0] = true;

		m_tInfo.pGameObj[0] = Monster::Create<DocheolAppearMark>(GRPDEV, *MYPOS);
		m_tInfo.pGameObj[0]->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
		Monster::Add_Monster_to_Scene(m_tInfo.pGameObj[0]);
	}
	
	if (m_tInfo.bTrigger[0])
	{
		if (m_tInfo.pGameObj[0] != nullptr)
		{
			if (m_tInfo.pGameObj[0]->Get_ObjectDead())
			{
				m_tInfo.Change_State(BOSS_DOCHEOL_HANDUPAPPEAR);
				m_tInfo.pGameObj[0] = nullptr;
			}
		}
	}

}

VOID Docheol::State_HandUpAppear(const _float& _DT)
{
	if (FAILED(Monster::Set_TextureList(L"Spr_Boss_Docheol_HandUpAppear", &m_tInfo)))
	{
		ObjectDead = true;
		return;
	}

	m_tInfo.Textureinfo._frameTick += _DT;
	if (m_tInfo.Textureinfo._frameTick >= FRAMETICK)
	{
		m_tInfo.Textureinfo._frameTick = 0.f;
		if (++m_tInfo.Textureinfo._frame >= m_tInfo.Textureinfo._Endframe)
		{
			m_tInfo.Change_State(BOSS_DOCHEOL_APPEAR);
		}
	}
}

VOID Docheol::State_Appear(const _float& _DT)
{
	if (FAILED(Monster::Set_TextureList(L"Spr_Boss_Docheol_Appear", &m_tInfo)))
	{
		ObjectDead = true;
		return;
	}

	if (m_tInfo.Textureinfo._frameTick > FRAMETICK)
	{
		m_tInfo.Textureinfo._frameTick = 0.f;
		if (++m_tInfo.Textureinfo._frame >= m_tInfo.Textureinfo._Endframe)
		{
			m_tInfo.Change_State(BOSS_DOCHEOL_IDLE);
		}
	}

}
VOID Docheol::State_Idle(const _float& _DT)
{
	if (FAILED(Monster::Set_TextureList(L"Spr_Boss_Docheol_Appear", &m_tInfo)))
	{
		ObjectDead = true;
		return;
	}

	if (!Monster::Set_Target(L"Player", m_tInfo.pGameObj[0]))
	{
		m_tInfo.Change_State(BOSS_DOCHEOL_TRACKING);
	}
}
VOID Docheol::State_Tracking(const _float& _DT)
{
	if (FAILED(Monster::Set_TextureList(L"Spr_Boss_Docheol_Appear", &m_tInfo)))
	{
		ObjectDead = true;
		return;
	}

	if (nullptr == m_tInfo.pGameObj[0])
	{
		m_tInfo.Change_State(BOSS_DOCHEOL_IDLE);
	}
}

VOID Docheol::State_Punch(const _float& _DT)
{
	if (FAILED(Monster::Set_TextureList(L"Spr_Boss_Docheol_RightSwing", &m_tInfo)))
	{
		ObjectDead = true;
		return;
	}
}