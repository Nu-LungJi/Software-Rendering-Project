#include "CubeFloorTile.h"
#include "../Include/PCH.h"

CubeFloorTile::CubeFloorTile(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV), m_pBuffer(nullptr), m_pTransform(nullptr), m_pTexture(nullptr) {}
CubeFloorTile::CubeFloorTile(const GameObject& _RHS) : GameObject(_RHS) {}
CubeFloorTile::~CubeFloorTile() {}

HRESULT CubeFloorTile::Ready_GameObject() {

	if (FAILED(Component_Initialize())) return E_FAIL;

	return S_OK;
}
INT	CubeFloorTile::Update_GameObject(const _float& _DT) {

	if (!m_bTrigger) {

		CubeFunction::Grid(m_pTransform, m_bGrid);
		m_pCollider->Set_Scale(m_pTransform->Get_Scale()->x, m_pTransform->Get_Scale()->y, m_pTransform->Get_Scale()->z);
		m_bTrigger = true;
	}

	m_pBuffer->Update_Component(_DT);
	m_pCollider->Update_Component(_DT);

	return 0;
}
VOID CubeFloorTile::LateUpdate_GameObject(const _float& _DT) {

	if (m_pCam == nullptr)	
		m_pCam = static_cast<CameraObject*>(SceneManager::GetInstance()->Get_GameObject(L"Camera"));

	Pooling();

	if (m_iTileNumber == MINIGAMETILEX * MINIGAMETILEZ -1)	RenderManager::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

	if (!IsIn_Cam) return;

	if (!m_bTrigger) {

		CubeFunction::Grid(m_pTransform, m_bGrid);
		m_pCollider->Set_Scale(m_pTransform->Get_Scale()->x, m_pTransform->Get_Scale()->y, m_pTransform->Get_Scale()->z);
		m_bTrigger = true;
	}


	GameObject::LateUpdate_GameObject(_DT);
}

VOID CubeFloorTile::Render_GameObject()
{
	MonsterManager::GetInstance()->Render_Static_Batch(GRPDEV, m_pTexture);

	//GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//GRPDEV->SetTransform(D3DTS_WORLD, m_pTransform->Get_World());
	//GRPDEV->SetTexture(0, m_pTexture);
	//m_pBuffer->Render_Buffer();
	//GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CubeFloorTile::Component_Initialize() {

	m_pBuffer		= ADD_COMPONENT_CUBE;
	m_pTransform	= ADD_COMPONENT_TRANSFORM;
	m_pTransform->Set_Scale(2.f, 1.f, 2.f);
	m_pTransform->Set_Pos(0.f, -0.5f, 0.f);

	m_pCollider		= ADD_COMPONENT_COLLIDER;
	m_pCollider->Set_CenterPos(m_pTransform);

	m_bTrigger = false;

	m_pTexture = ResourceManager::GetInstance()->Find_Texture(L"CubeFloor.dds");
	if (m_pTexture == nullptr) return E_FAIL;

	Monster::Staic_Obj(GRPDEV, m_pTransform);
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
BOOL CubeFloorTile::OnCollisionEnter(GameObject* _Other)
{
	return false;
}
VOID CubeFloorTile::Free() {

	
	GameObject::Free();
}

bool CubeFloorTile::Pooling()
{
	if (m_pCam == nullptr)return false;
	bool bVisible = m_pCam->IsIn_Frustum(*m_pTransform->Get_Position(), fabsf(m_pTransform->Get_Scale()->x + 0.5f));

	if (bVisible && !IsIn_Cam)		CollisionManager::GetInstance()->Add_ColliderObject(this);
	else if (!bVisible && IsIn_Cam)	CollisionManager::GetInstance()->Delete_ColliderObject(this);


	IsIn_Cam = bVisible;

	if (!bVisible)
	{
		_vec3* vPos = m_pTransform->Get_Position();
		_vec3 vDir = *vPos - *m_pCam->Get_AtVec();

		_float fTileSize = m_pTransform->Get_Scale()->x * 2.f;
		_float fDiffX = fTileSize * MINIGAMETILEX; // 3.0 * 32 = 96.0
		_float fDiffZ = fTileSize * MINIGAMETILEZ;

		bool bMoved = false;

		if (vDir.x < -fDiffX * 0.5f)		{
			vPos->x += fDiffX;
			bMoved = true;
		}
		else if (vDir.x > fDiffX * 0.5f)	{
			vPos->x -= fDiffX;
			bMoved = true;
		}
		if (vDir.z < -fDiffZ * 0.5f)		{
			vPos->z += fDiffZ;
			bMoved = true;
		}
		else if (vDir.z > fDiffZ * 0.5f)	{
			vPos->z -= fDiffZ;
			bMoved = true;
		}
		if (bMoved) {
			MonsterManager::GetInstance()->Update_Tile((_uint)m_iTileNumber, m_pTransform);
			Monster::Staic_Obj(GRPDEV, m_pTransform);
		}
	}
	return IsIn_Cam;
}
