#include "CubeFloorTile.h"
#include "../Include/PCH.h"

CubeFloorTile::CubeFloorTile(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV), m_pBuffer(nullptr), m_pTransform(nullptr){}
CubeFloorTile::CubeFloorTile(const GameObject& _RHS) : GameObject(_RHS) {}
CubeFloorTile::~CubeFloorTile() {}

HRESULT CubeFloorTile::Ready_GameObject() {

	if (FAILED(Component_Initialize())) return E_FAIL;

	return S_OK;
}
INT	CubeFloorTile::Update_GameObject(const _float& _DT) {

	if (!m_bTrigger) {

		//_vec3 vPos = *m_pTransform->Get_Position();
		//Monster::Add_Monster_to_Scene(Monster::Create<EvilSlime>(GRPDEV, vPos), L"Monster", GAMEOBJECT_TYPE::OBJECT_MONSTER);

		CubeFunction::Grid(m_pTransform, m_bGrid);
		m_pCollider->Set_Scale(m_pTransform->Get_Scale()->x, m_pTransform->Get_Scale()->y, m_pTransform->Get_Scale()->z);
		m_bTrigger = true;
	}

	if (!IsIn_Cam) return 0;

	GameObject::Update_GameObject(_DT);
	//m_pBuffer->Update_Component(_DT);
	//m_pCollider->Update_Component(_DT);

	return 0;
}
VOID CubeFloorTile::LateUpdate_GameObject(const _float& _DT) {

	if (m_pCam == nullptr)	
		m_pCam = static_cast<CameraObject*>(SceneManager::GetInstance()->Get_GameObject(L"Camera"));
	//IsIn_Cam = m_pCam->IsIn_Frustum(*m_pTransform->Get_Position(), m_pTransform->Get_Scale()->x);
	IsIn_Cam = m_pCam->IsIn_Frustum(*m_pTransform->Get_Position(), 0.f);

	if (!IsIn_Cam) return;

	if (!m_bTrigger) {

		//_vec3 vPos = *m_pTransform->Get_Position();
		//Monster::Add_Monster_to_Scene(Monster::Create<EvilSlime>(GRPDEV, vPos), L"Monster", GAMEOBJECT_TYPE::OBJECT_MONSTER);

		CubeFunction::Grid(m_pTransform, m_bGrid);
		m_pCollider->Set_Scale(m_pTransform->Get_Scale()->x, m_pTransform->Get_Scale()->y, m_pTransform->Get_Scale()->z);
		m_bTrigger = true;
	}
	//m_fTimer += _DT;

	//_float fRadian = m_fTimer * D3DX_PI * 6.f;
	//AlphaZValue = Monster::BillBoard(m_pTransform, GRPDEV, { cosf(fRadian),0.f,sinf(fRadian) }, false);
	//Monster::BillBoard(m_pTransform, GRPDEV, { 1.f,0.f,0.f }, 0);


	RenderManager::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

	GameObject::LateUpdate_GameObject(_DT);
}

VOID CubeFloorTile::Render_GameObject()
{

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GRPDEV->SetTransform(D3DTS_WORLD, m_pTransform->Get_World());

	GRPDEV->SetTexture(0, ResourceManager::GetInstance()->Find_Texture(L"CubeFloor.dds"));

	m_pBuffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CubeFloorTile::Component_Initialize() {

	m_pBuffer		= ADD_COMPONENT_CUBE;
	m_pTransform	= ADD_COMPONENT_TRANSFORM;
	m_pTransform->Set_Scale(4.f, 1.f, 4.f);
	m_pTransform->Set_Pos(0.f, -0.5f, 0.f);

	m_pCollider		= ADD_COMPONENT_COLLIDER;
	m_pCollider->Set_CenterPos(m_pTransform);

	m_bTrigger = false;

	return S_OK;
}

CubeFloorTile* CubeFloorTile::Create(LPDIRECT3DDEVICE9 pGraphicDev, bool _Grid)
{
	CubeFloorTile* pCubeFloorTile =DBG_NEWW CubeFloorTile(pGraphicDev);

	pCubeFloorTile->m_bGrid = _Grid;

	if (FAILED(pCubeFloorTile->Ready_GameObject())) {
		MSG_BOX("Cannot Create CubeFloorTile.");
		Safe_Release(pCubeFloorTile);
		return nullptr;
	}

	return pCubeFloorTile;
}
CubeFloorTile* CubeFloorTile::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, bool _Grid)
{
	CubeFloorTile* pCubeFloorTile = CubeFloorTile::Create(pGraphicDev, _Grid);

	*static_cast<Transform*>(pCubeFloorTile->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position() = vPos;

	return pCubeFloorTile;
}
CubeFloorTile* CubeFloorTile::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale, bool _Grid)
{
	CubeFloorTile* pCubeFloorTile = CubeFloorTile::Create(pGraphicDev, vPos, _Grid);

	*static_cast<Transform*>(pCubeFloorTile->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Scale() = vScale;

	return pCubeFloorTile;
}
VOID CubeFloorTile::Free() {

	
	GameObject::Free();
}