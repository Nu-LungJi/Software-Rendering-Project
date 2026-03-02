#include "../Include/PCH.h"
#include"EvilSlimeGroundIce.h"

EvilSlimeGroundIce::EvilSlimeGroundIce(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
EvilSlimeGroundIce::EvilSlimeGroundIce(const GameObject& _RHS) : GameObject(_RHS) {}
EvilSlimeGroundIce::~EvilSlimeGroundIce() {}

HRESULT EvilSlimeGroundIce::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;

	m_tInfo.Change_State(MONSTER_STATE::MONSTER_STATE_SUMMON);

	//MYPOS->z + 0.001f;
	//m_tInfo.pGameObj[1] = Monster::Create<Alert>(GRPDEV, { MYPOS->x, 0.002f, MYPOS->z });
	//Alert* pAlert = static_cast<Alert*>(m_tInfo.pGameObj[1]);
	//pAlert->Get_Info()->pGameObj[0] = m_tInfo.pGameObj[0];
	//pAlert->Get_Info()->pGameObj[1] = this;
	//pAlert->Get_Info()->fTimer[1] = m_tInfo.fTimer[1];
	//Monster::Add_Monster_to_Scene(m_tInfo.pGameObj[1], L"MonsterEffect", GAMEOBJECT_TYPE::MONSTER_EFFECT);

	return	S_OK;
}
INT	EvilSlimeGroundIce::Update_GameObject(const _float& _DT) {

	Component_Collider->Update_Component(_DT);

	m_tInfo.fTimer[0] += _DT;

	if (m_tInfo.bTrigger[0])
	{
		m_tInfo.Change_State(MONSTER_STATE_APPEAR);
		Component_Collider->Set_Hp(1.f);
		Component_Collider->Set_Att(1.f);
		Component_Collider->Set_Scale(MYSCALE->x * 0.5f, MYSCALE->y, MYSCALE->z * 0.5f);
	}

	switch (m_tInfo.eState[0])
	{
	case MONSTER_STATE::MONSTER_STATE_SUMMON:
		if (m_tInfo.pGameObj[1] == nullptr)
		{
			MYPOS->z + 0.001f;
			m_tInfo.pGameObj[1] = Monster::Create<Alert>(GRPDEV, {MYPOS->x, 0.002f, MYPOS->z});
			m_tInfo.pGameObj[1]->AddRef();
			Alert* pAlert = static_cast<Alert*>(m_tInfo.pGameObj[1]);
			pAlert->Get_Info()->pGameObj[0] = m_tInfo.pGameObj[0];
			pAlert->Get_Info()->pGameObj[1] = this;
			pAlert->Get_Info()->fTimer[1] = m_tInfo.fTimer[1];
			Monster::Add_Monster_to_Scene(m_tInfo.pGameObj[1], L"MonsterEffect", GAMEOBJECT_TYPE::MONSTER_EFFECT);
		}
		else
		{
			_float	fRatio = m_tInfo.fTimer[0] / m_tInfo.fTimer[1];
			_vec3	vScale = *MYSCALE;
			vScale.y = vScale.x;
			*SCALE(m_tInfo.pGameObj[1]) = vScale * fRatio;
		}
		break;
	case MONSTER_STATE::MONSTER_STATE_APPEAR:
		if (m_tInfo.pGameObj[1] != nullptr) {
			m_tInfo.pGameObj[1]->Set_ObjectDead(true);
			m_tInfo.pGameObj[1] = nullptr;
		}
		
		m_tInfo.Textureinfo._frameTick += _DT;
		if (m_tInfo.Textureinfo._frameTick > FRAMETICK)
		{
			m_tInfo.Textureinfo._frameTick = 0.f;
			ObjectDead = ++m_tInfo.Textureinfo._frame >= m_tInfo.Textureinfo._Endframe;
		}
		break;
	default:
		break;
	}

	if (ObjectDead)
	{
		if(m_tInfo.pGameObj[1] != nullptr)
			m_tInfo.pGameObj[1]->Set_ObjectDead(true);
		m_tInfo.pGameObj[1] = nullptr;
		return -1;
	}
	

	return 0;
}
VOID EvilSlimeGroundIce::LateUpdate_GameObject(const _float& _DT) {

	GameObject::LateUpdate_GameObject(_DT);

	if (static_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Camera"))->IsIn_Frustum(*MYPOS, 3.f)) {
		AlphaZValue = Monster::BillBoard(Component_Transform, GRPDEV);
		RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	}

}
VOID EvilSlimeGroundIce::Render_GameObject() {

	GRPDEV->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	switch (m_tInfo.eState[0])
	{
	default:
		break;
	case MONSTER_STATE_APPEAR:
		GRPDEV->SetTexture(0, (*m_tInfo.Textureinfo.pTexture)[m_tInfo.Textureinfo._frame]);
		Component_Buffer->Render_Buffer();
		break;
	}


	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
HRESULT EvilSlimeGroundIce::Component_Initialize() {

	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;

	Component_Transform->Set_Pos(1.f, 0.5f, 1.f);
	Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
	Component_Transform->Set_Scale(EVILSLIMEGROUNDICE_WIDTH, EVILSLIMEGROUNDICE_HEIGHT, 1.f);

	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);

	Component_Collider->Set_Hp(0.f);
	Component_Collider->Set_Att(0.f);

	m_tInfo.ID = MonsterManager::Make_Key((uint8_t)MONSTER_SEP::Effect, (uint8_t)BULLET_TYPE::GroundIce, 0);
	return Monster::Set_TextureList(m_tInfo.ID, &m_tInfo.Textureinfo);
}
BOOL EvilSlimeGroundIce::OnCollisionEnter(GameObject* _Other)
{
	return 0;
}
BOOL EvilSlimeGroundIce::OnCollisionStay(GameObject* _Other)
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
BOOL EvilSlimeGroundIce::OnCollisionExit(GameObject* _Other)
{
	return 0;
}

VOID EvilSlimeGroundIce::Free() {
	GameObject::Free();
}