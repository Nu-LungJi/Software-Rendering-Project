#include "CubeFloorTile.h"
#include "../Include/PCH.h"

CubeFloorTile::CubeFloorTile(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV), m_pBuffer(nullptr), m_pTransform(nullptr), m_eTileState(TILE_STATE::STATE_END), m_iTileNumber(0), m_eTileSide(TILE_SIDE::TILE_END) {}
CubeFloorTile::CubeFloorTile(const GameObject& _RHS) : GameObject(_RHS) {}
CubeFloorTile::~CubeFloorTile() {}

HRESULT CubeFloorTile::Ready_GameObject() {

	if (FAILED(Component_Initialize())) return E_FAIL;

	return S_OK;
}
INT	CubeFloorTile::Update_GameObject(const _float& _DT) {

	CubeFunction::Grid(m_pTransform, m_bGrid);

	m_pCollider->Set_Scale(m_pTransform->Get_Scale()->x, m_pTransform->Get_Scale()->y, m_pTransform->Get_Scale()->z);

	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return GameObject::Update_GameObject(_DT);
}
VOID CubeFloorTile::LateUpdate_GameObject(const _float& _DT) {

	if (m_bTrigger) return;
	GameObject::LateUpdate_GameObject(_DT);
}

VOID CubeFloorTile::Render_GameObject()
{

	if (m_bTrigger) return;

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GRPDEV->SetTransform(D3DTS_WORLD, m_pTransform->Get_World());

	GRPDEV->SetTexture(0, ResourceManager::GetInstance()->Find_Texture(L"CubeFloor.dds"));

	m_pBuffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CubeFloorTile::Component_Initialize() {

	m_pBuffer		= ADD_COMPONENT_CUBE;
	m_pTransform	= ADD_COMPONENT_TRANSFORM;
	m_pTransform->Set_Pos(0.f, 0.f, 0.f);

	// ONLY MINIGAME
	//m_pTexture		= MINIGAMESCENE->Set_Texture(L"CubeFloorTile", m_bTrigger);

	m_pCollider		= ADD_COMPONENT_COLLIDER;
	m_pCollider->Set_CenterPos(m_pTransform);
	//CollisionManager::GetInstance()->Add_ColliderObject(this);

	//if (m_bTrigger)
	//	m_pTexture->Import_TextureFromFolder(L"../../Tile/CubeFloorTile");

	m_bTrigger = false;

	return S_OK;
}

CubeFloorTile* CubeFloorTile::Create(LPDIRECT3DDEVICE9 pGraphicDev, bool _Grid)
{
	CubeFloorTile* pCubeFloorTile = new CubeFloorTile(pGraphicDev);

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