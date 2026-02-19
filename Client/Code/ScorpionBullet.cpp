#include "../Include/PCH.h"

ScorpionBullet::ScorpionBullet(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
ScorpionBullet::ScorpionBullet(const GameObject& _RHS) :	GameObject(_RHS)	{}
ScorpionBullet::~ScorpionBullet() {}

HRESULT ScorpionBullet::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;

	m_tInfo.fSpeed = 0.f;

	return S_OK;
}
INT	ScorpionBullet::Update_GameObject(const _float& _DT)
{
	if (!m_tInfo.bTrigger[0])
	{
		Monster::Set_TextureList(L"Spr_Bullet_ScorpionBullet", &m_tInfo);
	}
	else
	{
		Monster::Set_TextureList(L"Spr_Bullet_ScorpionBullet_Death", &m_tInfo);
	}


	MYPOS->y = 0.5f;
	Component_Collider->Set_Scale(MYSCALE->x * 0.5f, 1.f, MYSCALE->y * 0.5f);


	m_tInfo.Textureinfo._frameTick += _DT;
	if (m_tInfo.Textureinfo._frameTick > FRAMETICK)
	{
		++m_tInfo.Textureinfo._frame %= m_tInfo.Textureinfo._Endframe;
		m_tInfo.Textureinfo._frameTick = 0.f;
	}

	GameObject::Update_GameObject(_DT);

	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	return 0;
}
VOID ScorpionBullet::LateUpdate_GameObject(const _float& _DT) {
	GameObject::LateUpdate_GameObject(_DT);

	m_tInfo.vDirection.y = 0.f;
	Component_Transform->Move_Pos(&m_tInfo.vDirection, m_tInfo.fSpeed, _DT);

	m_tInfo.fTimer[0] += _DT;
	if (m_tInfo.fTimer[0] > 10.f)
	{
		ObjectDead = true;
		return;
	}

	Monster::BillBoard(Component_Transform, GRPDEV);

	_matrix* pmatWorld = Component_Transform->Get_World();
	_matrix matView, matRot;
	_vec3* vAxis = (_vec3*)&matView._31;

	//_float Radian = D3DX_PI / 2 * m_tInfo.Textureinfo._frame / m_tInfo.Textureinfo._Endframe;

	GRPDEV->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);
	D3DXMatrixRotationAxis(&matRot, vAxis, m_tInfo.fTimer[0] * 12.f);

	_vec3 vPos = { pmatWorld->_41,pmatWorld->_42, pmatWorld->_43 };
	pmatWorld->_41 = pmatWorld->_42 = pmatWorld->_43 = 0.f;

	*Component_Transform->Get_World() *= matRot;
	pmatWorld->_41 = vPos.x;
	pmatWorld->_42 = vPos.y;
	pmatWorld->_43 = vPos.z;

	Component_Collider->Set_Scale(MYSCALE->x, 1.f, MYSCALE->z);
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
	Component_Transform->Set_Scale(0.2f, 0.2f, 0.2f);
	Component_Transform->Set_Pos(0.f, 0.5f, 0.f);

	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);
	Component_Collider->Set_Scale(1.f, 1.f, 1.f);

	return S_OK;
}

BOOL ScorpionBullet::OnCollisionEnter(GameObject* _Other)
{
	if (!m_tInfo.bTrigger[0])
	{
		switch (_Other->Get_ObjectType())
		{
		default:
			//break;
		case GAMEOBJECT_TYPE::OBJECT_PLAYER:
		case GAMEOBJECT_TYPE::OBJECT_TERRAIN:
			m_tInfo.fSpeed = 0.f;
			m_tInfo.bTrigger[0] = true;
			m_tInfo.fTimer[0] = 8.f;
			break;
		}
		if (!m_tInfo.bTrigger[1])
		{
			m_tInfo.bTrigger[1] = true;
			for (int i = 0; i < SCORPIONBULLET_CHAINBULLET_NUM; ++i)
			{
				m_tInfo.pGameObj[1] = Monster::Create<Bullet_Chain_Head>(GRPDEV, *MYPOS);
				m_tInfo.pGameObj[1]->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);

				_float fRadian = 2.f * D3DX_PI * ((_float)i / SCORPIONBULLET_CHAINBULLET_NUM);
				fRadian = (fRadian > D3DX_PI) ? fRadian - (2.f * D3DX_PI) : fRadian;

				static_cast<Bullet_Chain_Head*>(m_tInfo.pGameObj[1])->Set_Dir(cosf(fRadian), 0.f, sinf(fRadian));
				static_cast<Bullet_Chain_Head*>(m_tInfo.pGameObj[1])->Set_Master(this);

				Monster::Add_Monster_to_Scene(m_tInfo.pGameObj[1], GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
			}
			m_tInfo.pGameObj[1] = nullptr;
		}
	}
	return TRUE;
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