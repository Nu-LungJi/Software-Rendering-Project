#include "../Include/PCH.h"

ScorpionBullet::ScorpionBullet(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
ScorpionBullet::ScorpionBullet(const GameObject& _RHS) :	GameObject(_RHS)	{}
ScorpionBullet::~ScorpionBullet() {}

HRESULT ScorpionBullet::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;

	m_tInfo.fSpeed = SCORPIONBULLET_SPEED;

	Component_Collider->Set_Hp(1.f);
	Component_Collider->Set_Att(1.f);

	return S_OK;
}
INT	ScorpionBullet::Update_GameObject(const _float& _DT)
{
	if (m_tInfo.eState[0] == MONSTER_STATE_MINIGAME_IDLE) {
		ObjectDead = false;
		return 0;
	}
	else if (m_tInfo.eState[0] == MONSTER_STATE_MINIGAME_MOVE) {
		ObjectDead = false;
		return 0;
	}

	Component_Collider->Set_Scale(MYSCALE->x * 0.5f, 1.f, MYSCALE->z * 0.5f);

	Monster::Destory_Tile(this);

	m_tInfo.fTimer[0] += _DT;

	if (m_tInfo.fTimer[0] > 10.f)
	{
		Component_Collider->Set_Hp(-1.f);
	}

	if (Component_Collider->Get_Hp() <= 0.f)
	{
		Monster::Set_TextureList(L"Spr_Bullet_ScorpionBullet_Death", &m_tInfo);

		m_tInfo.fTimer[1] += _DT;
		ObjectDead = m_tInfo.fTimer[1] >= 2.f;

		if (!m_tInfo.bTrigger[1])
		{
			m_tInfo.fSpeed *= 0.3f;
			for (int i = 0; i < SCORPIONBULLET_CHAINBULLET_NUM; ++i)
			{
				m_tInfo.pGameObj[1] = Monster::Create<Bullet_Chain_Head>(GRPDEV, *MYPOS, MYSCALE->x * 0.5f);

				_float fRadian = 2.f * D3DX_PI * ((_float)i / SCORPIONBULLET_CHAINBULLET_NUM);
				fRadian = (fRadian > D3DX_PI) ? fRadian - (2.f * D3DX_PI) : fRadian;

				static_cast<Bullet_Chain_Head*>(m_tInfo.pGameObj[1])->Set_Dir(cosf(fRadian), 0.f, sinf(fRadian));

				Monster::Add_Monster_to_Scene(m_tInfo.pGameObj[1], L"MonsterBullet", GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
				m_tInfo.pGameObj[1] = nullptr;
			}
			m_tInfo.bTrigger[1] = true;
		}
	}
	else
	{
		Monster::Set_TextureList(L"Spr_Bullet_ScorpionBullet", &m_tInfo);
	}

	m_tInfo.Textureinfo._frameTick += _DT;
	if (m_tInfo.Textureinfo._frameTick > FRAMETICK)
	{
		m_tInfo.Textureinfo._frameTick = 0.f;
		++m_tInfo.Textureinfo._frame;
		if (m_tInfo.bTrigger[0])
		{
			ObjectDead = m_tInfo.Textureinfo._frame >= m_tInfo.Textureinfo._Endframe;
		}
		else
		{
			m_tInfo.Textureinfo._frame %= m_tInfo.Textureinfo._Endframe;
		}
	}

	//GameObject::Update_GameObject(_DT);
	Component_Buffer->Update_Component(_DT);
	Component_Collider->Update_Component(_DT);

	if (ObjectDead)
		return -1;
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	return 0;
}
VOID ScorpionBullet::LateUpdate_GameObject(const _float& _DT) {
	GameObject::LateUpdate_GameObject(_DT);

	m_tInfo.vDirection.y = 0.f;
	Component_Transform->Move_Pos(&m_tInfo.vDirection, m_tInfo.fSpeed, _DT);

	_float fRadian = m_tInfo.fTimer[0] * D3DX_PI * 6.f;
	AlphaZValue = Monster::BillBoard(Component_Transform, GRPDEV, { cosf(fRadian),0.f,sinf(fRadian) }, false);

	//_matrix* pmatWorld = Component_Transform->Get_World();
	//_matrix matView, matRot;
	//_vec3* vAxis = (_vec3*)&matView._31;

	//_float Radian = D3DX_PI / 2 * m_tInfo.Textureinfo._frame / m_tInfo.Textureinfo._Endframe;

	//GRPDEV->GetTransform(D3DTS_VIEW, &matView);
	//D3DXMatrixInverse(&matView, NULL, &matView);
	//D3DXMatrixRotationAxis(&matRot, vAxis, m_tInfo.fTimer[0] * 12.f);

	//_vec3 vPos = { pmatWorld->_41,pmatWorld->_42, pmatWorld->_43 };
	//pmatWorld->_41 = pmatWorld->_42 = pmatWorld->_43 = 0.f;

	//*Component_Transform->Get_World() *= matRot;
	//pmatWorld->_41 = vPos.x;
	//pmatWorld->_42 = vPos.y;
	//pmatWorld->_43 = vPos.z;

	//Component_Collider->Set_Scale(MYSCALE->x, 1.f, MYSCALE->z);
	//AlphaSorting(Component_Transform->Get_Position());
	//AlphaSorting((_vec3*)&Component_Transform->Get_World()->_41);

}
VOID ScorpionBullet::Render_GameObject() {
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	GRPDEV->SetTexture(0, m_tInfo.Textureinfo._vecTexture[m_tInfo.Textureinfo._frame]);
	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
HRESULT ScorpionBullet::Component_Initialize() {

	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;

	Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
	Component_Transform->Set_Scale(SCORPIONBULLET_WIDTH, SCORPIONBULLET_HEIGHT, 1.f);
	Component_Transform->Set_Pos(0.f, 0.5f, 0.f);

	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);

	return S_OK;
}

BOOL ScorpionBullet::OnCollisionEnter(GameObject* _Other)
{
	wstring Tag = _Other->Get_ObjectTag();
	MainUI* mainUI;
	if (Tag == L"PlayerArrow") {

		Component_Collider->Set_Hp(Component_Collider->Get_Hp() - COLLIDER(_Other)->Get_Att());
		return TRUE;
	}
	else if (Tag == L"Player")
	{
		mainUI = dynamic_cast<MainUI*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"MainUI"));
		mainUI->Player_LostHP();
		Component_Collider->Set_Hp(Component_Collider->Get_Hp() - COLLIDER(_Other)->Get_Att());
		return TRUE;
	}
	return FALSE;

	//MainUI* mainUI;
	//if (!m_tInfo.bTrigger[0])
	//{
	//	if (_Other->Get_ObjectTag() == L"Player") {
	//		mainUI = dynamic_cast<MainUI*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"MainUI"));
	//		mainUI->Player_LostHP();
	//		Component_Collider->Set_Hp(Component_Collider->Get_Hp() - COLLIDER(_Other)->Get_Att());
	//		m_tInfo.fSpeed = 0.f;
	//		m_tInfo.bTrigger[0] = true;
	//		m_tInfo.fTimer[0] = 8.f;
	//	}
	//	switch (_Other->Get_ObjectType())
	//	{
	//	default:
	//		//break;
	//	case GAMEOBJECT_TYPE::OBJECT_PLAYER:
	//	case GAMEOBJECT_TYPE::OBJECT_TERRAIN:
	//		m_tInfo.fSpeed = 0.f;
	//		m_tInfo.bTrigger[0] = true;
	//		m_tInfo.fTimer[0] = 8.f;
	//		break;
	//	}
	//	if (!m_tInfo.bTrigger[1])
	//	{
	//		m_tInfo.bTrigger[1] = true;
	//		for (int i = 0; i < SCORPIONBULLET_CHAINBULLET_NUM; ++i)
	//		{
	//			m_tInfo.pGameObj[1] = Monster::Create<Bullet_Chain_Head>(GRPDEV, *MYPOS);
	//			m_tInfo.pGameObj[1]->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
	//		}
	//	}
	//	wstring Tag = _Other->Get_ObjectTag();


	//	if (Tag == L"PlayerArrow") {
	//		Component_Collider->Set_Hp(Component_Collider->Get_Hp() - COLLIDER(_Other)->Get_Att());
	//		return TRUE;
	//		// 플레이어는 겹쳐서 합쳤는데 화살은 어디둘지 잘 모르겠어서 일단 놔두었습니다
	//	}
	//	return FALSE;
	//}
}

BOOL ScorpionBullet::OnCollisionStay(GameObject* _Other)
{
	return 0;
}

BOOL ScorpionBullet::OnCollisionExit(GameObject* _Other)
{
	return 0;
}

ScorpionBullet* ScorpionBullet::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	ScorpionBullet* MST = new ScorpionBullet(_GRPDEV);
	if (FAILED(MST->Ready_GameObject())) {
		MSG_BOX("Cannot Create ScorpionBullet.");
		Safe_Release(MST);
		return nullptr;
	}
	return MST;
}
VOID ScorpionBullet::Free()
{
	GameObject::Free();
}