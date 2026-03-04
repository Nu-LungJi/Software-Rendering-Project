#include "CubeFloorTile.h"
#include "../Include/PCH.h"

CubeFloorTile::CubeFloorTile(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV), m_pBuffer(nullptr), m_pTransform(nullptr), m_pTexture(nullptr), m_pTimer(nullptr) {}
CubeFloorTile::CubeFloorTile(const GameObject& _RHS) : GameObject(_RHS) {}
CubeFloorTile::~CubeFloorTile() {}

HRESULT CubeFloorTile::Ready_GameObject() {

	if (FAILED(Component_Initialize())) return E_FAIL;
	
	m_pTimer = static_cast<MiniGameScene*>(SceneManager::GetInstance()->Get_CurrentScene())->Get_Timer();
	return S_OK;
}
INT	CubeFloorTile::Update_GameObject(const _float& _DT) {

	if (!m_bTrigger) {

		CubeFunction::Grid(m_pTransform, m_bGrid);
		m_pCollider->Set_Scale(m_pTransform->Get_Scale()->x, m_pTransform->Get_Scale()->y, m_pTransform->Get_Scale()->z);
		m_bTrigger = true;
	}

	m_pBuffer->Update_Component(_DT);
	//m_pCollider->Update_Component(_DT);

	return 0;
}
VOID CubeFloorTile::LateUpdate_GameObject(const _float& _DT) {

	if (m_pCam == nullptr)
		m_pCam = static_cast<CameraObject*>(SceneManager::GetInstance()->Get_GameObject(L"Camera"));

	if (m_pTarget == nullptr)
		m_pTarget = static_cast<CameraObject*>(SceneManager::GetInstance()->Get_GameObject(L"Player"));

	Pooling();

	//if (m_pTransform->Get_Position()->x < (POS(m_pTarget)->x - 30.f))
	//	m_iFalling = 4;
	//else if (m_pTransform->Get_Position()->x < (POS(m_pTarget)->x - 10.f))
	//	m_iFalling = 1;

	if (m_iTileNumber == MINIGAMETILEX * MINIGAMETILEZ - 1)	
		RenderManager::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

	if (!m_bTrigger) {
		CubeFunction::Grid(m_pTransform, m_bGrid);
		m_pCollider->Set_Scale(m_pTransform->Get_Scale()->x, m_pTransform->Get_Scale()->y, m_pTransform->Get_Scale()->z);
		m_bTrigger = true;
	}

	if (m_iFalling == 1) {
		m_fTimer += _DT;
		_vec3 vDir = { 0.f, -1.f, 0.f };
		m_pTransform->Move_Pos(&vDir, 10, _DT);

		Monster::Staic_Obj(GRPDEV, m_pTransform);
		MonsterManager::GetInstance()->Update_Tile(m_iTileNumber, m_pTransform);

		if (m_fTimer > 2.f) m_iFalling = 3;
	}

	if (m_iFalling == 4) {
		m_fTimer = 0.f;
		m_iFalling = 0;

		_vec3* vPos = m_pTransform->Get_Position();
		_vec3* vScale = m_pTransform->Get_Scale();

		if (m_ePoolingMode == POOLINGMODE::X) {
			vPos->y = -1.f;
		}
		else if (m_ePoolingMode == POOLINGMODE::Y) {
			vPos->z = 10.f;
		}
		else if (m_ePoolingMode == POOLINGMODE::Z) {
			vPos->y = 51.f;
		}

		Monster::Staic_Obj(GRPDEV, m_pTransform);
		MonsterManager::GetInstance()->Update_Tile(m_iTileNumber, m_pTransform);
	}
	if (!IsIn_Cam) return;

	GameObject::LateUpdate_GameObject(_DT);
}

VOID CubeFloorTile::Render_GameObject()
{
	GRPDEV->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	GRPDEV->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	GRPDEV->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	MonsterManager::GetInstance()->Render_Static_Batch(GRPDEV, m_pTexture);

}

HRESULT CubeFloorTile::Component_Initialize() {

	m_pBuffer = ADD_COMPONENT_CUBE;
	m_pTransform = ADD_COMPONENT_TRANSFORM;
	m_pTransform->Set_Scale(2.f, 1.f, 2.f);
	m_pTransform->Set_Pos(0.f, -0.5f, 0.f);

	m_pCollider = ADD_COMPONENT_COLLIDER;
	m_pCollider->Set_CenterPos(m_pTransform);

	m_bTrigger = false;

	m_pTexture = ResourceManager::GetInstance()->Find_Texture(L"CubeFloorTileDark.dds");
	if (m_pTexture == nullptr) return E_FAIL;

	Monster::Staic_Obj(GRPDEV, m_pTransform);
	return S_OK;
}

CubeFloorTile* CubeFloorTile::Create(LPDIRECT3DDEVICE9 pGraphicDev, bool _Grid)
{
	CubeFloorTile* pCubeFloorTile = DBG_NEWW CubeFloorTile(pGraphicDev);

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
BOOL CubeFloorTile::OnCollisionStay(GameObject* _Other)
{
	wstring Tag = _Other->Get_ObjectTag();
	if (Tag == L"Player")
		switch (m_ePoolingMode)
		{
		case POOLINGMODE::X:
			Monster::Hurdle_CollisionStay(this, _Other, 0, 1, 0);
			break;
		case POOLINGMODE::Y:
			Monster::Hurdle_CollisionStay(this, _Other, 0, 0, 1);
			break;
		case POOLINGMODE::Z:
			Monster::Hurdle_CollisionStay(this, _Other, 0, 1, 0);
			break;
		}
	return 0;
}
VOID CubeFloorTile::Free() {


	GameObject::Free();
}
bool CubeFloorTile::Pooling()
{
	_vec3* vPos		= m_pTransform->Get_Position();
	_vec3* vScale	= m_pTransform->Get_Scale();

	if (m_pTarget == nullptr) 
		return false;

	_vec3 vTargetPos = *POS(m_pTarget);
	_vec3 vDir = vTargetPos - *vPos;
	_float fDisSq = D3DXVec3LengthSq(&vDir);

	_int iCol = m_iTileNumber % MINIGAMETILEX;
	_int iRow = m_iTileNumber / MINIGAMETILEX;
	_float fSize = vScale->x * 2.f;
	_float fFullLoop = fSize * MINIGAMETILEX;
	_float fHalf = fFullLoop * 0.5f;

	_float fLimitX = 50.f;
	_float fLimitY = 50.f;
	_float fThresholdX = fLimitX + (fSize * 2.5f);
	_float fThresholdY = fLimitY + (fSize * 4.5f);
	_float fEdgeZ = MINIGAMETILEZ * vScale->z * 2.f;

	bool bMoved = false;

	if (m_ePoolingMode == POOLINGMODE::X) {
		if ((vTargetPos.x - vPos->x > fHalf)||
		((Monster::Get_Gravity().z == 1.f)&&(fDisSq > 100.f)) ){
			_float fNextPosX = vPos->x + fFullLoop;

			if (fNextPosX >= fThresholdX) {
				m_ePoolingMode = POOLINGMODE::Y;
				vPos->x = fLimitX + (iRow - 2) * fSize + vScale->x;
				vPos->y = (iCol-2) * fSize;
				vPos->z = fEdgeZ;
			}
			else {
				vPos->x = fNextPosX;
			}
			bMoved = true;
		}
	}
	else if (m_ePoolingMode == POOLINGMODE::Y) {
		if ((vTargetPos.y > vPos->y + fHalf)|| 
			((Monster::Get_Gravity().y== 1.f))) {
			_float fNextPosY = vPos->y + fFullLoop;

			if (fNextPosY >= fThresholdY) {
				m_ePoolingMode = POOLINGMODE::Z;
				vPos->x = fLimitX + (iRow - 2) * fSize;
				vPos->y = 51.f;
				vPos->z = (iCol- 5) * fSize;
			}
			else {
				vPos->y = fNextPosY;
			}
			bMoved = true;
		}
	}
	else if (m_ePoolingMode == POOLINGMODE::Z) {
		if (fabsf(vTargetPos.z - vPos->z) > fHalf) {
			if (vDir.z < 0.f) {
				vPos->z -= fFullLoop;
			}
			else {
				vPos->z += fFullLoop;
			}
			bMoved = true;
		}
	}

	if (bMoved) {
		MonsterManager::GetInstance()->Update_Tile((_uint)m_iTileNumber, m_pTransform);
		Monster::Staic_Obj(GRPDEV, m_pTransform);
	}

	m_iFalling = 4;
	return IsIn_Cam;
}