#include "../Include/PCH.h"

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
	
	//_TexInfo->pTexture = MonsterManager::GetInstance()->Find_Textures(SearchKey);
	//if (nullptr == _TexInfo->pTexture)				return E_POINTER;

	_TexInfo->pTexture = pNewTex;
	_TexInfo->_vecTexture.clear();

	_TexInfo->_frame = 0;
	_TexInfo->_frameTick = 0.f;
	_TexInfo->_Endframe = _TexInfo->pTexture->size() -1 ;

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

	// 여기서 누수남..? 왜?
	_vec3 vCampos = *dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Camera"))->
					Get_EyeVec();

	//_matrix matView;
	//_GRPDEV->GetTransform(D3DTS_VIEW, &matView);
	//_vec3 vCampos	 = ?

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
	//TCHAR Classname[256];
	//swprintf_s(Classname, 256, L"%S", typeid(*pMonster).name());
	//CONST TCHAR* pName = wcschr(Classname, L' ');
	//pName = (pName != nullptr) ? pName + 1 : Classname;

	TCHAR Classname[256];
	swprintf_s(Classname, 256, L"%S", typeid(*SceneManager::GetInstance()->Get_CurrentScene()).name());
	CONST TCHAR* pName = wcschr(Classname, L' ');
	pName = (pName != nullptr) ? pName + 1 : Classname;

	pMonster->Set_ObjectTag(_TAG.c_str());
	pMonster->Set_ObjectType(eType);

	SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(pMonster);
	if (pMonster->Get_Component(COMPONENT_TYPE::COMPONENT_COLLIDER) != nullptr)
		CollisionManager::GetInstance()->Add_ColliderObject(pMonster);
}
//
//VOID Monster::BillBoard_Standard(LPDIRECT3DDEVICE9 GRPDEV, Transform* Component_Transform)
//{
//	_matrix		matBill, matWorld, matView;
//
//	matWorld = *Component_Transform->Get_World();
//	GRPDEV->GetTransform(D3DTS_VIEW, &matView);
//
//	D3DXMatrixIdentity(&matBill);
//
//	//XÃà
//	matBill._11 = matView._11;
//	matBill._12 = matView._12;
//	matBill._13 = matView._13;
//	//YÃà
//	matBill._21 = matView._21;
//	matBill._22 = matView._22;
//	matBill._23 = matView._23;
//	//ZÃà
//	matBill._31 = matView._31;
//	matBill._32 = matView._32;
//	matBill._33 = matView._33;
//
//	D3DXMatrixInverse(&matBill, 0, &matBill);
//
//	// ÁÖÀÇ ÇÒ °Í
//	matWorld = matBill * matWorld;
//
//	Component_Transform->Set_World(&matWorld);
//}

//uint64_t Monster::XorShift128plus(uint64_t& _Seed1, uint64_t& _Seed2)
//{
//	// 창준이형 로컬에서 추가된 함수인것 같습니다. 같이 상의해서 사용 부탁드려요~
//	if (0 == _Seed1 || 0 == _Seed2)
//	{
//		_Seed1 = 0x123456789ABCDEF0;
//		_Seed2 = 0xFEDCBA9876543210;
//	}
//
//	uint64_t x = _Seed1;
//	uint64_t const y = _Seed2;
//	_Seed1 = y;
//	x ^= x << 23;
//	_Seed2 = x ^ y ^ (x >> 17) ^ (y >> 26);
//
//	return _Seed2 + y;
//}


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
