#include "../Include/PCH.h"

CameraObject*	Monster::m_pCam		= nullptr;
Player*			Monster::m_pPlayer	= nullptr;

GameObject* Monster::Set_Target(const TCHAR* _TAG, GameObject*& GameObj)
{
	GameObj = SceneManager::GetInstance()->Get_GameObject(_TAG);
	return GameObj;
}
GameObject* Monster::Set_Target(CONST TCHAR* _TAG)
{
	GameObject* GameObj = SceneManager::GetInstance()->Get_GameObject(_TAG);
	return GameObj;
}
HRESULT Monster::Set_TextureList(uint16_t _Key, TEXINFO* _TexInfo)
{
	if (nullptr == _TexInfo)
		return E_POINTER;

	uint16_t SearchKey = _Key & 0xffc0;

	const vector<IDirect3DTexture9*>* pNewTex = MonsterManager::GetInstance()->Find_Textures(SearchKey);
	if (nullptr == pNewTex)				return E_POINTER;
	if (pNewTex == _TexInfo->pTexture)	return S_OK;

	_TexInfo->pTexture = pNewTex;
	_TexInfo->_vecTexture.clear();

	_TexInfo->_frame = 0;
	_TexInfo->_frameTick = 0.f;
	_TexInfo->_Endframe = _TexInfo->pTexture->size() - (_TexInfo->_Endframe > 0);

	return S_OK;
}

HRESULT Monster::Set_TextureList(const TCHAR* __FileName, TEXINFO* __Textures)
{
	if (nullptr == __Textures)
		return E_POINTER;
		
	if (!wcscmp(__FileName, __Textures->_Filename))
		return S_OK;

	__Textures->_frame = 0;
	__Textures->_frameTick = 0.f;
	__Textures->_Endframe = 0;
	__Textures->_vecTexture.clear();

	__Textures->pTexture = &__Textures->_vecTexture;

	wcscpy_s(__Textures->_Filename, 256, __FileName);

	while (true)
	{
		IDirect3DTexture9* pTexture = nullptr;
		TCHAR Filename[256];
		swprintf_s(Filename, 256, L"%s_%02d.png", __FileName, ++__Textures->_Endframe);
		pTexture = ResourceManager::GetInstance()->Find_Texture(Filename);
		if (nullptr == pTexture) break;
		else __Textures->_vecTexture.push_back(pTexture);
	}

	if (--__Textures->_Endframe) return S_OK;
	else						 return E_FAIL;
}

HRESULT Monster::Set_TextureList(const TCHAR* __FileName, MONINFO* _MonsterInfo)
{

	return Monster::Set_TextureList(__FileName, &_MonsterInfo->Textureinfo);
}

FLOAT Monster::BillBoard(Transform* TransCom, LPDIRECT3DDEVICE9 _GRPDEV, _vec3 vDir, BOOL OffSet)
{
	_vec3 vPos = *TransCom->Get_Position();
	_vec3 vScale = *TransCom->Get_Scale();

	_vec3 vCampos = *dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Camera"))->
					Get_EyeVec();

	_vec3 vLook = vCampos - vPos;
	D3DXVec3Normalize(&vLook, &vLook);

	_vec3 vRight = vDir;
	D3DXVec3Normalize(&vRight, &vRight);

	_vec3 vUp;
	D3DXVec3Cross(&vUp, &vRight, &vLook);
	D3DXVec3Normalize(&vUp, &vUp);
	
	D3DXVec3Cross(&vLook, &vRight, &vUp);

	_matrix matWorld;
	D3DXMatrixIdentity(&matWorld);

	*(_vec3*)&matWorld._11 = vRight * vScale.x;
	*(_vec3*)&matWorld._21 = vUp * vScale.y;
	*(_vec3*)&matWorld._31 = vLook * vScale.z;

	_vec3 vFinalPos = vPos + OffSet * (vUp * (vScale.y * 0.45f));

	matWorld._41 = vFinalPos.x;
	matWorld._42 = vFinalPos.y;
	matWorld._43 = vFinalPos.z;

	TransCom->Set_World(&matWorld);

	_vec3 vDist = vCampos - vPos;
	return D3DXVec3Length(&vDist);
}

HRESULT Monster::Flip_Horizontal(Transform* TransCom, _vec3* pDir, _float Buffer)
{
	if (pDir->x	 < -Buffer)
	{
		if (TransCom->Get_Scale()->x < 0)
			TransCom->Get_Scale()->x *= -1.f;
	}
	else if (pDir->x > Buffer)
	{
		if (TransCom->Get_Scale()->x > 0)
			TransCom->Get_Scale()->x *= -1.f;
	}
	return S_OK;
}

VOID Monster::Add_Monster_to_Scene(GameObject* pMonster, wstring _TAG, GAMEOBJECT_TYPE eType)
{
	pMonster->Set_ObjectTag(_TAG.c_str());
	pMonster->Set_ObjectType(eType);

	SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(pMonster);
	if (pMonster->Get_Component(COMPONENT_TYPE::COMPONENT_COLLIDER) != nullptr)
		CollisionManager::GetInstance()->Add_ColliderObject(pMonster);
}
void Monster::Release_Hurdle(MONSTERINFO* _Info) {
	if (_Info->_pHurdle != nullptr) {
		_Info->_pHurdle->RefCount--;

		if (_Info->_pHurdle->RefCount == 0) {
			vector<tagHurdleInfo*>* pContainer = MonsterManager::GetInstance()->Get_Hurdles();
			for (auto it = pContainer->begin(); it != pContainer->end(); ++it) {
				if (*it == _Info->_pHurdle) {
					delete* it;
					pContainer->erase(it);
					break;
				}
			}
		}
	}
}


VOID Monster::BillBoard_Standard(LPDIRECT3DDEVICE9 GRPDEV, Transform* Component_Transform)
{
	_matrix		matBill, matWorld, matView;

	matWorld = *Component_Transform->Get_World();
	GRPDEV->GetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixIdentity(&matBill);

	//XÃà
	matBill._11 = matView._11;
	matBill._12 = matView._12;
	matBill._13 = matView._13;
	//YÃà
	matBill._21 = matView._21;
	matBill._22 = matView._22;
	matBill._23 = matView._23;
	//ZÃà
	matBill._31 = matView._31;
	matBill._32 = matView._32;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, 0, &matBill);

	// ÁÖÀÇ ÇÒ °Í
	matWorld = matBill * matWorld;

	Component_Transform->Set_World(&matWorld);
}

VOID Monster::Destory_Tile(GameObject* pObj)
{
	_vec3 vPos, vScale, vTileScale, vTilePos{ 0,0,0 };

	Transform* pTile = nullptr;
	vPos = *dynamic_cast<Transform*>(pObj->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position();
	vScale = *dynamic_cast<Transform*>(pObj->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Scale();
	for (auto& iter : TileManager::GetInstance()->Get_DestoryTile_List())
	{
		if (dynamic_cast<TileInfo*>(iter->Get_Component(COMPONENT_TYPE::COMPONENT_TILEINFO))->Get_TileStateName() == TILE_STATE::STATE_DESTORY ||
			dynamic_cast<TileInfo*>(iter->Get_Component(COMPONENT_TYPE::COMPONENT_TILEINFO))->Get_TileStateName() == TILE_STATE::STATE_BOOM)
		{
			pTile = dynamic_cast<Transform*>(iter->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
			vTilePos = *pTile->Get_Position();

			if (vPos.x > vTilePos.x - 1 && vPos.x < vTilePos.x + 1 && vPos.z > vTilePos.z - 1 && vPos.z < vTilePos.z + 1)
			{
				pObj->Set_ObjectDead(TRUE);
				dynamic_cast<CXZTile*>(iter)->Set_Destory();
			}
		}
	}
}

HRESULT Monster::Minigame_Update(const _float& _DT, MONINFO* _pInfo, _vec3* vPos)
{

	if (_pInfo->eState[0] == MONSTER_STATE_MINIGAME_IDLE) {
		CameraObject* pCamera = Monster::Get_Camera();
		tagHurdleInfo* pHurdle = _pInfo->_pHurdle;
		if ((nullptr == pCamera) || (nullptr == pHurdle)) return 0;

		pHurdle->VisibleCount += pCamera->IsIn_Frustum(pHurdle->vPos, pHurdle->fDis * 0.6f);

		return 0;
	}
	else if (_pInfo->eState[0] == MONSTER_STATE_MINIGAME_MOVE) {
		CameraObject* pCamera = Monster::Get_Camera();
		tagHurdleInfo* pHurdle = _pInfo->_pHurdle;
		if ((nullptr == pCamera) || (nullptr == pHurdle)) return 0;

		pHurdle->VisibleCount += pCamera->IsIn_Frustum(pHurdle->vPos, pHurdle->fDis * 0.6f);
		_vec3 vCalc = pHurdle->vDst - *vPos;
		_float fCalc = D3DXVec3Dot(&vCalc, &pHurdle->vDir);
		if (fCalc < 0.f)
			*vPos = pHurdle->vSrc;

		return 0;
	}
	return E_FAIL;
}

BOOL Monster::Minigame_LateUpdate(const _float& _DT, MONINFO* _pInfo)
{
	if (_pInfo->eState[0] == MONSTER_STATE_MINIGAME_IDLE) {
		tagHurdleInfo* pHurdle = _pInfo->_pHurdle;
		if (pHurdle == nullptr)	
			return FALSE;
		if (_pInfo->_pHurdle->VisibleCount > 0) {
			_pInfo->Change_State(MONSTER_STATE_MINIGAME_MOVE);
			_pInfo->vDirection = pHurdle->vDir;
			_pInfo->fSpeed = pHurdle->fSpeed;
		}
		return FALSE;
	}
	else if (_pInfo->eState[0] == MONSTER_STATE_MINIGAME_MOVE) {
		tagHurdleInfo* pHurdle = _pInfo->_pHurdle;
		if (pHurdle == nullptr)
			return FALSE;
		if (pHurdle->VisibleCount == 0) {
			_pInfo->Change_State(MONSTER_STATE_MINIGAME_IDLE);
		}
		return TRUE;
	}
}

HRESULT Monster::Staic_Obj(LPDIRECT3DDEVICE9 _GRPDEV, Transform* Transcom)
{
	_vec3 vPos			= *Transcom->Get_Position();
	_vec3 vScale		= *Transcom->Get_Scale();
	_matrix* pWorldMat	= Transcom->Get_World();

	pWorldMat->_11 = vScale.x;  pWorldMat->_12 = 0.f;       pWorldMat->_13 = 0.f;       pWorldMat->_14 = 0.f;
	pWorldMat->_21 = 0.f;       pWorldMat->_22 = vScale.y;  pWorldMat->_23 = 0.f;       pWorldMat->_24 = 0.f;
	pWorldMat->_31 = 0.f;       pWorldMat->_32 = 0.f;       pWorldMat->_33 = vScale.z;  pWorldMat->_34 = 0.f;
	pWorldMat->_41 = vPos.x;    pWorldMat->_42 = vPos.y;    pWorldMat->_43 = vPos.z;    pWorldMat->_44 = 1.f;

	return S_OK;
}

BOOL Monster::Hurdle_CollisionStay(GameObject* _pNoMove, GameObject* _pMove, BOOL x, BOOL y, BOOL z)
{
	Collider* pColliderNoMove = static_cast<Collider*>(_pNoMove->Get_Component(COMPONENT_TYPE::COMPONENT_COLLIDER));
	Collider* pColliderMove = static_cast<Collider*>(_pMove->Get_Component(COMPONENT_TYPE::COMPONENT_COLLIDER));
	_vec3* pPosNoMove = pColliderNoMove->Get_CenterPos()->Get_Position();
	_vec3* pPosMove = pColliderMove->Get_CenterPos()->Get_Position();

	_vec3		vDir = *pPosMove - *pPosNoMove;

	_float	dx(-1.f), dy(-1.f), dz(-1.f);
	if (x) dx = fabsf(vDir.x);
	if (y) dy = fabsf(vDir.y);
	if (z) dz = fabsf(vDir.z);

	if ((dx >= dz) && (dx > dy) && (dx >= 0.f)) {
		_float fDis = pColliderMove->Get_Scale().x * 0.501f;
		pPosMove->x = (vDir.x < 0.f) ? pColliderNoMove->Get_MinPoint().x - fDis : pColliderNoMove->Get_MaxPoint().x + fDis;
		return true;
	}
	else if ((dz >= dy) && (dz >= 0.f)) {
		_float fDis = pColliderMove->Get_Scale().z * 0.501f;
		pPosMove->z = (vDir.z < 0.f) ? pColliderNoMove->Get_MinPoint().z - fDis : pColliderNoMove->Get_MaxPoint().z + fDis;
		return true;
	}
	else if (dy >= 0.f) {
		_float fDis = pColliderMove->Get_Scale().y * 0.501f;
		pPosMove->y = (vDir.y < 0.f) ? pColliderNoMove->Get_MinPoint().y - fDis : pColliderNoMove->Get_MaxPoint().y + fDis;
		return true;
	}
	else {
		return false;
	}
}