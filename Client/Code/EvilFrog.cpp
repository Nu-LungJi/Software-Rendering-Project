#include "../Include/PCH.h"

EvilFrog::EvilFrog(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
EvilFrog::EvilFrog(const GameObject& _RHS) : GameObject(_RHS) {}
EvilFrog::~EvilFrog() {}

HRESULT EvilFrog::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;

	m_tInfo.eState[0] = MONSTER_STATE_IDLE;

	Component_Collider->Set_Hp(EVILFROG_HP);

	m_tInfo.vDirection = { -1.f,0.f,0.f };

	return S_OK;
}
INT EvilFrog::Update_GameObject(const FLOAT& _DT) {

	MYPOS->y = MYSCALE->y * 0.5f;
	Component_Collider->Set_Scale(MYSCALE->x * 0.5f, MYSCALE->y, MYSCALE->x * 0.5f);

	if (Component_Collider->Get_Hp() <= 0.f)
		m_tInfo.Change_State(MONSTER_STATE_DEAD);

	GameObject::Update_GameObject(_DT);

	if (Component_Collider->Get_Hp() <= 0.f)
		m_tInfo.eState[0] = MONSTER_STATE_DEAD;

	switch (m_tInfo.eState[0])
	{
	default:
		break;
	case MONSTER_STATE_SUMMON:
		EvilFrog::State_Summon(_DT);
		break;
	case MONSTER_STATE_IDLE:
		EvilFrog::State_Idle(_DT);
		break;
	case MONSTER_STATE_TRACKING:
		EvilFrog::State_Tracking(_DT);
		break;
	case MONSTER_STATE_CASTING:
		EvilFrog::State_Casting(_DT);
		break;
	case MONSTER_STATE_CHANNELING:
		EvilFrog::State_Channeling(_DT);
		break;
	case MONSTER_STATE_DEAD:
		EvilFrog::State_Dead();
		break;
	}
	
	if (KEY_DOWN(DIK_1) && KEY_DOWN(DIK_2))
		m_tInfo.Change_State(MONSTER_STATE_DISAPPEAR);

	if (ObjectDead)
	{
		TileManager::GetInstance()->Set_StageArray();
		return -1;
	}
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	if (KEY_DOWN(DIK_LCONTROL) && KEY_DOWN(DIK_T))
		m_tInfo.Change_State(MONSTER_STATE_DEAD);

	return 0;
}
VOID EvilFrog::LateUpdate_GameObject(const FLOAT& _DT)
{
	GameObject::LateUpdate_GameObject(_DT);

	m_tInfo.vDirection.y = 0.f;
	Component_Transform->Move_Pos(D3DXVec3Normalize(&m_tInfo.vDirection, &m_tInfo.vDirection), m_tInfo.fSpeed, _DT);

	m_tInfo.Textureinfo._frameTick += _DT;

	switch (m_tInfo.eState[0])
	{
	case MONSTER_STATE_IDLE:
		if (FAILED(Monster::Set_TextureList(L"Spr_Monster_EvilFrog_Stand", &m_tInfo)))
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
		break;
	case MONSTER_STATE_TRACKING:	
		if (m_tInfo.Textureinfo._frameTick >= FRAMETICK)
		{
			++m_tInfo.Textureinfo._frame %= m_tInfo.Textureinfo._Endframe / 2;
			m_tInfo.Textureinfo._frameTick = 0.f;

			if (fabsf(m_tInfo.vDirection.z) > 0.1f)
				if (m_tInfo.vDirection.z > 0.f)
					m_tInfo.Textureinfo._frame += (m_tInfo.Textureinfo._frame < m_tInfo.Textureinfo._Endframe * 0.5f) * m_tInfo.Textureinfo._Endframe / 2;
		}
		break;
	}
	Monster::Flip_Horizontal(Component_Transform, &m_tInfo.vDirection, BAT_HORIZONTALFLIP_BUFFER);
	Monster::BillBoard(Component_Transform, GRPDEV);
}
VOID EvilFrog::Render_GameObject() {
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
HRESULT EvilFrog::Component_Initialize() {
	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;
	Component_Transform->Set_Pos(0.f, 0.5f, 0.f);
	Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
	Component_Transform->Set_Scale(EVILFROG_WIDTH, EVILFROG_HEIGHT, 1.f);
	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);

	return S_OK;
}
VOID EvilFrog::State_Summon(const _float& _DT) {

}

VOID EvilFrog::State_Idle(const _float& _DT) {
	if (m_tInfo.pGameObj[0] == nullptr)
		m_tInfo.pGameObj[0] = (Monster::Set_Target(L"Player"));

	_vec3 vDir = *POS(m_tInfo.pGameObj[0]) - *MYPOS;
	vDir.y = 0.f;

	if (D3DXVec3Length(&vDir) < EVILFROG_TRACKINGDIS)
		m_tInfo.Change_State(MONSTER_STATE_TRACKING);
}

VOID EvilFrog::State_Tracking(const _float& _DT) {

	if(nullptr== m_tInfo.pGameObj[0] || m_tInfo.pGameObj[0]->Get_ObjectDead())
    m_tInfo.Change_State(MONSTER_STATE_IDLE);

  m_tInfo.vDirection = *POS(m_tInfo.pGameObj[0]) - *MYPOS;

	_float fDis = D3DXVec3Length(&m_tInfo.vDirection);

	m_tInfo.fSpeed = EVILFROG_SPEED;

	if (fDis < EVILFROG_TRACKINGDIS)
	{
		m_tInfo.fTimer[0] += _DT;
		m_tInfo.fTimer[1] = 0.f;
	}
	else
		m_tInfo.fTimer[1] += _DT;

	if (m_tInfo.fTimer[0] > EVILFROG_TRACKING_TIME)
		m_tInfo.Change_State(MONSTER_STATE_IDLE);
	else if (m_tInfo.fTimer[1] >= EVILFROG_LOST_TIME)
		m_tInfo.Change_State(MONSTER_STATE_IDLE);

}
VOID EvilFrog::State_Casting(const _float& _DT) {

	if(nullptr == m_tInfo.pGameObj[0] || m_tInfo.pGameObj[0]->Get_ObjectDead())
		m_tInfo.Change_State(MONSTER_STATE_IDLE);

	m_tInfo.fTimer[0] += _DT;
}
VOID EvilFrog::State_Channeling(const _float& _DT) {

}

VOID EvilFrog::State_Dead() {
	PLAY_MONSTER_EFFECT_ONCE(MONSTER_EFFECT::MONSTER_DEATH, *MYPOS, 1.f);
	ObjectDead = TRUE;
}
BOOL EvilFrog::OnCollisionEnter(GameObject* _Other) {

	wstring Tag = _Other->Get_ObjectTag();
	if (Tag == L"Player" )
	{
		MainUI* mainUI = dynamic_cast<MainUI*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"MainUI"));
		mainUI->Player_LostHP();
	}
	return FALSE;
}
BOOL EvilFrog::OnCollisionStay(GameObject* _Other) {
  wstring Tag = _Other->Get_ObjectTag();

	if(Tag== L"PlayerArrow") {
		Component_Collider->Set_Hp(Component_Collider->Get_Hp() - COLLIDER(_Other)->Get_Att());
  }
	return FALSE;
}
BOOL EvilFrog::OnCollisionExit(GameObject* _Other) {
	return FALSE;
}
EvilFrog* EvilFrog::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	EvilFrog* EF = new EvilFrog(_GRPDEV);
	if (FAILED(EF->Ready_GameObject())) {
    MSG_BOX("Cannot Create EvilFrog.");
		Safe_Release(EF);
		return nullptr;
	}
	return EF;
}
VOID EvilFrog::Free() {
	GameObject::Free();
}