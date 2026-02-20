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

_vec3 Monster::Normalize(_vec3 vec)
{
	_vec3 vTemp = vec;
	D3DXVec3Normalize(&vTemp, &vec);
	return vTemp;
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
	__Textures->_vecTexture.reserve(32);
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
	else return E_FAIL;
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

VOID Monster::Add_Monster_to_Scene(GameObject* pMonster, GAMEOBJECT_TYPE eType)
{
	TCHAR Classname[256];
	swprintf_s(Classname, 256, L"%S", typeid(*pMonster).name());

	CONST TCHAR* pName = wcschr(Classname, L' ');

	pName = (pName != nullptr) ? pName + 1 : Classname;
	pMonster->Set_ObjectTag(pName);

	pMonster->Set_ObjectType(eType);

	SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(pMonster);
	CollisionManager::GetInstance()->Add_ColliderObject(pMonster);
}

	uint64_t Monster::XorShift128plus(uint64_t& _Seed1, uint64_t& _Seed2)
	{
		if (0 == _Seed1 || 0 == _Seed2)
		{
			_Seed1 = 0x123456789ABCDEF0;
			_Seed2 = 0xFEDCBA9876543210;
		}

		uint64_t x = _Seed1;
		uint64_t const y = _Seed2;
		_Seed1 = y;
		x ^= x << 23;
		_Seed2 = x ^ y ^ (x >> 17) ^ (y >> 26);

		return _Seed2 + y;
	}


VOID Monster::BillBoard_Standard(LPDIRECT3DDEVICE9 GRPDEV, Transform* Component_Transform)
{
	_matrix		matBill, matWorld, matView;

	matWorld = *Component_Transform->Get_World();
	GRPDEV->GetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixIdentity(&matBill);

	//X축
	matBill._11 = matView._11;
	matBill._12 = matView._12;
	matBill._13 = matView._13;
	//Y축
	matBill._21 = matView._21;
	matBill._22 = matView._22;
	matBill._23 = matView._23;
	//Z축
	matBill._31 = matView._31;
	matBill._32 = matView._32;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, 0, &matBill);

	// 주의 할 것
	matWorld = matBill * matWorld;

	Component_Transform->Set_World(&matWorld);
}
