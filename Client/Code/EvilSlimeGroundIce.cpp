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
	//GameObject::Update_GameObject(_DT);
	Component_Buffer->Update_Component(_DT);
	Component_Collider->Update_Component(_DT);


	m_tInfo.fTimer[0] += _DT;

	//Component_Collider->Set_Scale(MYSCALE->x * 0.5f, MYSCALE->y, MYSCALE->z * 0.5f);

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
	
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}
VOID EvilSlimeGroundIce::LateUpdate_GameObject(const _float& _DT) {

	GameObject::LateUpdate_GameObject(_DT);

	//Monster::BillBoard_Standard(GRPDEV, Component_Transform);
	AlphaZValue = Monster::BillBoard(Component_Transform, GRPDEV,{1.f,0.f,0.f}, 0);
}
VOID EvilSlimeGroundIce::Render_GameObject() {

	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	switch (m_tInfo.eState[0])
	{
	default:
		break;
	case MONSTER_STATE_APPEAR:
		GRPDEV->SetTexture(0, m_tInfo.Textureinfo._vecTexture[m_tInfo.Textureinfo._frame]);
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

	return FAILED(Monster::Set_TextureList(L"Spr_Effect_BlueEvilSlimeGroudIceEffect", &m_tInfo));
	
}
BOOL EvilSlimeGroundIce::OnCollisionEnter(GameObject* _Other)
{
	return 0;
}
BOOL EvilSlimeGroundIce::OnCollisionStay(GameObject* _Other)
{
	return 0;
}
BOOL EvilSlimeGroundIce::OnCollisionExit(GameObject* _Other)
{
	return 0;
}
//EvilSlimeGroundIce* EvilSlimeGroundIce::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
//	EvilSlimeGroundIce* NPN = new EvilSlimeGroundIce(_GRPDEV);
//	if (FAILED(NPN->Ready_GameObject())) {
//		MSG_BOX("Cannot Create EvilSlimeGroundIce.");
//		Safe_Release(NPN);
//		return nullptr;
//	}
//	return NPN;
//}
VOID EvilSlimeGroundIce::Free() {
	GameObject::Free();
}