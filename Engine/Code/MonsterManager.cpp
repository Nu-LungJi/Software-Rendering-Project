#include "MonsterManager.h"

IMPLEMENT_SINGLETON(MonsterManager)

MonsterManager::MonsterManager(){}
MonsterManager::~MonsterManager(){}

void MonsterManager::Load_Textures_from_Folder(IDirect3DDevice9* _GRPDEV, const wstring& _Filepath)
{
	_wfinddata64_t FileData;
	wstring strSearchPath = _Filepath + L"\\*.*";

	intptr_t handle = _wfindfirst64(strSearchPath.c_str(), &FileData);
	if (handle == -1) return;

	do {
		if (FileData.name[0] == L'.')	continue;
		if (FileData.attrib & _A_SUBDIR) Load_Textures_from_Folder(_GRPDEV, _Filepath + L"\\" + FileData.name);
		else {
			FILENAMEINFO tInfo = Make_ID_from_Filename(FileData.name);
			if (tInfo.usResult == 0xffff)	continue;

			wstring			strFullPath	= _Filepath + L"\\" + FileData.name;
			LPDIRECT3DTEXTURE9 pTexture = nullptr;

			if (SUCCEEDED(D3DXCreateTextureFromFileEx(_GRPDEV, strFullPath.c_str(),
				D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
				D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pTexture)))
			{
				vector<LPDIRECT3DTEXTURE9>& vecTex = mapProtoType[tInfo.usResult];
				
				uint8_t Index = tInfo.FrameNum - 1;
				if (vecTex.size() <= Index )
					vecTex.resize(Index + 1, nullptr);

				vecTex[Index] = pTexture;
			}
		}
	} while (_wfindnext64(handle, &FileData) == 0);
	
	_findclose(handle);
}

FILENAMEINFO MonsterManager::Make_ID_from_Filename(const wstring& _Filename)
{
	FILENAMEINFO tInfo(_Filename);

	if (tInfo.szState[0] == L'\0' || !wcscmp(tInfo.szState, L" ")) {
		tInfo.State = 0x00;
	}

	if		(!wcscmp(tInfo.szType, L"Monster"))						{ tInfo.Type = (uint8_t)MONSTER_SEP::Monster; 
		if      (!wcscmp(tInfo.szName, L"BlueEvilBat"))					{ tInfo.name = (uint8_t)MONSTER_TYPE::Bat;
				if		(!wcscmp(tInfo.szState, L"Stand"))						tInfo.State = (uint8_t)MONSTER_ANIM::Stand;
		}
		else if (!wcscmp(tInfo.szName, L"BlueScorpionEvilSoul"))	{ tInfo.name = (uint8_t)MONSTER_TYPE::ScorpionEvilSoul;
				if		(!wcscmp(tInfo.szState, L"Stand"))					tInfo.State = (uint8_t)MONSTER_ANIM::Stand;
				else if (!wcscmp(tInfo.szState, L"appear"))					tInfo.State = (uint8_t)MONSTER_ANIM::Appear;
				else if (!wcscmp(tInfo.szState, L"disappear"))				tInfo.State = (uint8_t)MONSTER_ANIM::Disappear;
		}
		else if (!wcscmp(tInfo.szName, L"BlueEvilSlime"))			{ tInfo.name = (uint8_t)MONSTER_TYPE::EvilSlime;
				if		(!wcscmp(tInfo.szState, L"Stand"))					tInfo.State = (uint8_t)MONSTER_ANIM::Stand;
		}
		else if (!wcscmp(tInfo.szName, L"BlueEliteShotGunEvilSoul")){ tInfo.name = (uint8_t)MONSTER_TYPE::ShotGunEvilSoul;
				if (!wcscmp(tInfo.szState, L"Stand"))						tInfo.State = (uint8_t)MONSTER_ANIM::Stand;
		}
	}
	else if (!wcscmp(tInfo.szType, L"Bullet"))				{ tInfo.Type = (uint8_t)MONSTER_SEP::Bullet;
		if		(!wcscmp(tInfo.szName, L"Standard"))				{ tInfo.name = (uint8_t)BULLET_TYPE::Standard;
				if		(!wcscmp(tInfo.szState, L"Birth"))					tInfo.State = (uint8_t)MONSTER_EFFECT::BULLET_STANDARD_BIRTH;
				else if (!wcscmp(tInfo.szState, L"BirthRayUp"))				tInfo.State = (uint8_t)MONSTER_EFFECT::BULLET_STANDARD_BIRTHRAY;
				else if (!wcscmp(tInfo.szState, L"Death"))					tInfo.State = (uint8_t)MONSTER_EFFECT::BULLET_STANDARD_DEATH;
				else if (!wcscmp(tInfo.szState, L"Charge"))					tInfo.State = (uint8_t)MONSTER_EFFECT::BULLET_STANDARD_CHARGE	;
		}
		else if (!wcscmp(tInfo.szName, L"ScorpionBullet"))			{ tInfo.name = (uint8_t)BULLET_TYPE::ScorpionBullet;
				if (!wcscmp(tInfo.szState, L"Death"))					tInfo.State = (uint8_t)MONSTER_ANIM::Death;
		}
		else if (!wcscmp(tInfo.szName, L"Chain"))					{ tInfo.name = (uint8_t)BULLET_TYPE::Chain;
				if		(!wcscmp(tInfo.szState, L"Head"))				tInfo.State = (uint8_t)BULLET_CHAIN_TYPE::Head;
				else if (!wcscmp(tInfo.szState, L"Rect"))				tInfo.State = (uint8_t)BULLET_CHAIN_TYPE::Rect;
				else if (!wcscmp(tInfo.szState, L"Line"))				tInfo.State = (uint8_t)BULLET_CHAIN_TYPE::Line;
		}
		else if (!wcscmp(tInfo.szName, L"Fireball"))				tInfo.name = (uint8_t)BULLET_TYPE::Fireball;
	}
	else if (!wcscmp(tInfo.szType, L"Effect"))				{ tInfo.Type = (uint8_t)MONSTER_SEP::Effect;
		if		(!wcscmp(tInfo.szName, L"BlueEvilSlimeGroudIceEffect")) tInfo.name = (uint8_t)BULLET_TYPE::GroundIce;
		else if (!wcscmp(tInfo.szName, L"Alert"))					  { tInfo.name = (uint8_t)MONSTER_EFFECT::ALERT;
				if		(!wcscmp(tInfo.szState, L"Circle"))					tInfo.State = (uint8_t)ALERT_TYPE::Circle;
				else if (!wcscmp(tInfo.szState, L"Rect"))					tInfo.State = (uint8_t)ALERT_TYPE::Rect;
				else if (!wcscmp(tInfo.szState, L"Line"))					tInfo.State = (uint8_t)ALERT_TYPE::Line;
		}
		else if (!wcscmp(tInfo.szName, L"MonsterSummons01"))			tInfo.name = (uint8_t)MONSTER_EFFECT::MONSTER_SUMMONS01;
		else if (!wcscmp(tInfo.szName, L"MonsterSummons02"))			tInfo.name = (uint8_t)MONSTER_EFFECT::MONSTER_SUMMONS02;
		else if (!wcscmp(tInfo.szName, L"MonsterSummons03"))			tInfo.name = (uint8_t)MONSTER_EFFECT::MONSTER_SUMMONS03;
		else if (!wcscmp(tInfo.szName, L"baseDeathEffect"))				tInfo.name = (uint8_t)MONSTER_EFFECT::MONSTER_DEATH;
	}
	else if (!wcscmp(tInfo.szType, L"Tile"))			{tInfo.Type = (uint8_t)MONSTER_SEP::Tile;
	}
	else return tInfo;
	
	//if      (!wcscmp(tInfo.szName, L"BlueEvilBat"))					tInfo.name = (uint8_t)MONSTER_TYPE::Bat;
	//else if (!wcscmp(tInfo.szName, L"BlueScorpionEvilSoul"))		tInfo.name = (uint8_t)MONSTER_TYPE::ScorpionEvilSoul;
	//else if (!wcscmp(tInfo.szName, L"BlueEvilSlime"))				tInfo.name = (uint8_t)MONSTER_TYPE::EvilSlime;
	//else if (!wcscmp(tInfo.szName, L"BlueEliteShotGunEvilSoul"))	tInfo.name = (uint8_t)MONSTER_TYPE::ShotGunEvilSoul;
	//else if (!wcscmp(tInfo.szName, L"Standard"))					tInfo.name = (uint8_t)BULLET_TYPE::Standard;
	//else if (!wcscmp(tInfo.szName, L"ScorpionBullet"))				tInfo.name = (uint8_t)BULLET_TYPE::ScorpionBullet;
	//else if (!wcscmp(tInfo.szName, L"Chain01"))						tInfo.name = (uint8_t)BULLET_TYPE::Chain01;
	//else if (!wcscmp(tInfo.szName, L"Chain02"))						tInfo.name = (uint8_t)BULLET_TYPE::Chain02;
	//else if (!wcscmp(tInfo.szName, L"ChainHead"))					tInfo.name = (uint8_t)BULLET_TYPE::ChainHead;
	//else if (!wcscmp(tInfo.szName, L"Fireball"))					tInfo.name = (uint8_t)BULLET_TYPE::Fireball;
	//else if (!wcscmp(tInfo.szName, L"BlueEvilSlimeGroudIceEffect")) tInfo.name = (uint8_t)BULLET_TYPE::GroundIce;
	//else if (!wcscmp(tInfo.szName, L"Alert"))						tInfo.name = (uint8_t)MONSTER_EFFECT::ALERT;

	//else if (!wcscmp(tInfo.szState, L"Stand"))                     tInfo.State = (uint8_t)MONSTER_ANIM::Stand;
	//else if (!wcscmp(tInfo.szState, L"appear"))                    tInfo.State = (uint8_t)MONSTER_ANIM::Appear;
	//else if (!wcscmp(tInfo.szState, L"disappear"))                 tInfo.State = (uint8_t)MONSTER_ANIM::Disappear;
	//else if (!wcscmp(tInfo.szState, L"Attack"))                    tInfo.State = (uint8_t)MONSTER_ANIM::Attack;
	//else if (!wcscmp(tInfo.szState, L"Death"))                     tInfo.State = (uint8_t)MONSTER_ANIM::Death;


	tInfo.usResult =  ((uint16_t)tInfo.Type     << 14) |
					 ((uint16_t)tInfo.name      << 10) |
					 ((uint16_t)tInfo.State     <<  6);

	return tInfo;
}

uint16_t MonsterManager::Make_Key(uint8_t eType, uint8_t eName, uint8_t eState)
{
	uint16_t Key	= 0x00;
	
	uint8_t Type	= eType;
	if (Type > 4) return 0xff;
	
	uint8_t Name	= eName;
	if (Name > 16) return 0xff;
	
	uint8_t State	= eState;
	if (State > 16) return 0xff;

	Key =	((uint16_t)Type << 14) |
			((uint16_t)Name << 10) |
			((uint16_t)State << 6);

	return Key +1;
}

uint16_t MonsterManager::Update_Key(uint16_t Key, uint8_t eState)
{
	return ((Key & 0xfc00) | ((uint16_t)eState << 6));
}


const vector<IDirect3DTexture9*>* MonsterManager::Find_Textures(uint16_t _Key)
{
	uint16_t Key = _Key & 0xFFC0;
	auto it = mapProtoType.find(Key);

	if(it == mapProtoType.end())
		return nullptr;

	return &(it->second);
}

void MonsterManager::Free()
{
	for (auto& pair : mapProtoType)	{
		for (auto& tex : pair.second) {
			tex->Release();
			tex = nullptr;
		}
		pair.second.clear();
	}
	mapProtoType.clear();
}


HRESULT MonsterManager::Ready_Origin_Buffer(Buffer* _pBuffer)
{
	if (_pBuffer == nullptr) return E_POINTER;

	LPDIRECT3DVERTEXBUFFER9 pVB			= _pBuffer->Get_VB();
	VTXTRUECUBE*			pSourceVtx	= nullptr;

	if (FAILED(pVB->Lock(0, 0, (void**)&pSourceVtx, D3DLOCK_READONLY)))
		return E_FAIL;

	memcpy(m_vOriginVtx, pSourceVtx, sizeof(VTXTRUECUBE) * 24);
	pVB->Unlock();


	LPDIRECT3DINDEXBUFFER9 pIB = _pBuffer->Get_IB();
	INDEX32* pSourceIdx = nullptr;

	if (FAILED(pIB->Lock(0, 0, (void**)&pSourceIdx, D3DLOCK_READONLY)))
		return E_FAIL;

	memcpy(m_vOriginIdx, pSourceIdx, sizeof(INDEX32) * 12);
	pIB->Unlock();

	return S_OK;
}

HRESULT MonsterManager::Ready_Origin_Buffer()
{   
	// --- 1. 정점 데이터(24개) 정의 ---
    // 구조체 순서: vPosition, vNormal, dwColor, vTexUV(_vec3)

    // 앞면 (Z-)
	m_vOriginVtx[0] = { { -1.f,  1.f, -1.f }, { 0.f,  0.f, -1.f }, 0xffffffff, { 0.f, 0.f, 0.f } };
	m_vOriginVtx[1] = { {  1.f,  1.f, -1.f }, { 0.f,  0.f, -1.f }, 0xffffffff, { 1.f, 0.f, 0.f } };
	m_vOriginVtx[2] = { {  1.f, -1.f, -1.f }, { 0.f,  0.f, -1.f }, 0xffffffff, { 1.f, 1.f, 0.f } };
	m_vOriginVtx[3] = { { -1.f, -1.f, -1.f }, { 0.f,  0.f, -1.f }, 0xffffffff, { 0.f, 1.f, 0.f } };

	// 뒷면 (Z+)
	m_vOriginVtx[4] = { {  1.f,  1.f,  1.f }, { 0.f,  0.f,  1.f }, 0xffffffff, { 0.f, 0.f, 0.f } };
	m_vOriginVtx[5] = { { -1.f,  1.f,  1.f }, { 0.f,  0.f,  1.f }, 0xffffffff, { 1.f, 0.f, 0.f } };
	m_vOriginVtx[6] = { { -1.f, -1.f,  1.f }, { 0.f,  0.f,  1.f }, 0xffffffff, { 1.f, 1.f, 0.f } };
	m_vOriginVtx[7] = { {  1.f, -1.f,  1.f }, { 0.f,  0.f,  1.f }, 0xffffffff, { 0.f, 1.f, 0.f } };

	// 윗면 (Y+)
	m_vOriginVtx[8] = { { -1.f,  1.f,  1.f }, { 0.f,  1.f,  0.f }, 0xffffffff, { 0.f, 0.f, 0.f } };
	m_vOriginVtx[9] = { {  1.f,  1.f,  1.f }, { 0.f,  1.f,  0.f }, 0xffffffff, { 1.f, 0.f, 0.f } };
	m_vOriginVtx[10] = { {  1.f,  1.f, -1.f }, { 0.f,  1.f,  0.f }, 0xffffffff, { 1.f, 1.f, 0.f } };
	m_vOriginVtx[11] = { { -1.f,  1.f, -1.f }, { 0.f,  1.f,  0.f }, 0xffffffff, { 0.f, 1.f, 0.f } };

	// 아랫면 (Y-)
	m_vOriginVtx[12] = { { -1.f, -1.f, -1.f }, { 0.f, -1.f,  0.f }, 0xffffffff, { 0.f, 0.f, 0.f } };
	m_vOriginVtx[13] = { {  1.f, -1.f, -1.f }, { 0.f, -1.f,  0.f }, 0xffffffff, { 1.f, 0.f, 0.f } };
	m_vOriginVtx[14] = { {  1.f, -1.f,  1.f }, { 0.f, -1.f,  0.f }, 0xffffffff, { 1.f, 1.f, 0.f } };
	m_vOriginVtx[15] = { { -1.f, -1.f,  1.f }, { 0.f, -1.f,  0.f }, 0xffffffff, { 0.f, 1.f, 0.f } };

	// 왼쪽면 (X-)
	m_vOriginVtx[16] = { { -1.f,  1.f,  1.f }, { -1.f, 0.f,  0.f }, 0xffffffff, { 0.f, 0.f, 0.f } };
	m_vOriginVtx[17] = { { -1.f,  1.f, -1.f }, { -1.f, 0.f,  0.f }, 0xffffffff, { 1.f, 0.f, 0.f } };
	m_vOriginVtx[18] = { { -1.f, -1.f, -1.f }, { -1.f, 0.f,  0.f }, 0xffffffff, { 1.f, 1.f, 0.f } };
	m_vOriginVtx[19] = { { -1.f, -1.f,  1.f }, { -1.f, 0.f,  0.f }, 0xffffffff, { 0.f, 1.f, 0.f } };

	// 오른쪽면 (X+)
	m_vOriginVtx[20] = { {  1.f,  1.f, -1.f }, {  1.f, 0.f,  0.f }, 0xffffffff, { 0.f, 0.f, 0.f } };
	m_vOriginVtx[21] = { {  1.f,  1.f,  1.f }, {  1.f, 0.f,  0.f }, 0xffffffff, { 1.f, 0.f, 0.f } };
	m_vOriginVtx[22] = { {  1.f, -1.f,  1.f }, {  1.f, 0.f,  0.f }, 0xffffffff, { 1.f, 1.f, 0.f } };
	m_vOriginVtx[23] = { {  1.f, -1.f, -1.f }, {  1.f, 0.f,  0.f }, 0xffffffff, { 0.f, 1.f, 0.f } };

	// --- 2. 인덱스 데이터(12개) 생성 ---
	for (_uint i = 0; i < 6; ++i)
	{
		_uint uiVtxIdx = i * 4;
		_uint uiTriIdx = i * 2;

		m_vOriginIdx[uiTriIdx]._0 = uiVtxIdx;
		m_vOriginIdx[uiTriIdx]._1 = uiVtxIdx + 1;
		m_vOriginIdx[uiTriIdx]._2 = uiVtxIdx + 2;

		m_vOriginIdx[uiTriIdx + 1]._0 = uiVtxIdx;
		m_vOriginIdx[uiTriIdx + 1]._1 = uiVtxIdx + 2;
		m_vOriginIdx[uiTriIdx + 1]._2 = uiVtxIdx + 3;
	}

	return S_OK;
}

HRESULT MonsterManager::Ready_Static_Batch(LPDIRECT3DDEVICE9 _GRPDEV)
{
	m_uiMaxTile = m_vecTiles.size();
	if (m_uiMaxTile == 0) return E_FAIL;

	if (m_pTileVB != nullptr) {
		m_pTileVB->Release();	m_pTileVB = nullptr;
	}
	if (m_pTileIB != nullptr) {
		m_pTileIB->Release(); m_pTileIB = nullptr;
	}

	_uint iMaxVtxCnt = m_uiMaxTile * 24;
	_uint iMaxTriCnt = m_uiMaxTile * 12;

	if (FAILED(_GRPDEV->CreateVertexBuffer(sizeof(VTXTRUECUBE) * iMaxVtxCnt,
		D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, FVF_TRUECUBE, D3DPOOL_DEFAULT, &m_pTileVB, NULL)))
		return E_FAIL;

	if (FAILED(_GRPDEV->CreateIndexBuffer(sizeof(INDEX32) * iMaxTriCnt,
		0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_pTileIB, NULL)))
		return E_FAIL;

    VTXTRUECUBE*	pStaticVtx = nullptr;
    INDEX32*		pStaticIdx = nullptr;

	m_pTileVB->Lock(0, 0, (void**)&pStaticVtx, 0);
	m_pTileIB->Lock(0, 0, (void**)&pStaticIdx, 0);

	for (_uint i = 0; i < m_uiMaxTile; ++i)	{
		Transform* pTransCom = static_cast<Transform*>(m_vecTiles[i]->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
		_vec3 vPos = *pTransCom->Get_Position();
		_vec3 vScale = *pTransCom->Get_Scale();
		_uint uiVtxOff = i * 24;
		_uint uiIdxOff = i * 12;

		for (_uint j = 0; j < 24; ++j)		{
			pStaticVtx[uiVtxOff + j] = m_vOriginVtx[j];

			pStaticVtx[uiVtxOff + j].vPosition.x *= vScale.x;
			pStaticVtx[uiVtxOff + j].vPosition.y *= vScale.y;
			pStaticVtx[uiVtxOff + j].vPosition.z *= vScale.z;

			pStaticVtx[uiVtxOff + j].vPosition += vPos;
		}

		for (_uint j = 0; j < 12; ++j)		{
			pStaticIdx[uiIdxOff + j]._0 = m_vOriginIdx[j]._0 + uiVtxOff;
			pStaticIdx[uiIdxOff + j]._1 = m_vOriginIdx[j]._1 + uiVtxOff;
			pStaticIdx[uiIdxOff + j]._2 = m_vOriginIdx[j]._2 + uiVtxOff;
		}
	}
	m_pTileVB->Unlock();
	m_pTileIB->Unlock();

	return S_OK;
}	

void MonsterManager::Render_Static_Batch(LPDIRECT3DDEVICE9 GRPDEV, LPDIRECT3DTEXTURE9 Texture)
{
	if (m_pTileVB == nullptr)return;

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	_matrix	matWorld;
	D3DXMatrixIdentity(&matWorld);
	GRPDEV->SetTransform(D3DTS_WORLD, &matWorld);

	GRPDEV->SetTexture(0, Texture);

	GRPDEV->SetStreamSource(0, m_pTileVB, 0, sizeof(VTXTRUECUBE));
	GRPDEV->SetIndices(m_pTileIB);
	GRPDEV->SetFVF(FVF_TRUECUBE);
	GRPDEV->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24 * m_uiMaxTile, 0, 12 * m_uiMaxTile);

	GRPDEV->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
}

void MonsterManager::Update_Tile(_uint _Num, Transform* TransCom)
{
	if (m_pTileVB == nullptr)	return;

	_uint iVtxSize		= sizeof(VTXTRUECUBE);
	_uint iStartByte	= _Num * 24 * iVtxSize;
	_uint iLockSize		= 24 * iVtxSize;

	VTXTRUECUBE* pDestVtx = nullptr;

	if (FAILED(m_pTileVB->Lock(iStartByte, iLockSize, (void**)&pDestVtx, D3DLOCK_NOOVERWRITE)))
		return;

	for (int i = 0; i < 24; ++i)
	{
		_vec3 vScale = *TransCom->Get_Scale();
		pDestVtx[i] = m_vOriginVtx[i];

		pDestVtx[i].vPosition.x *= vScale.x;
		pDestVtx[i].vPosition.y *= vScale.y;
		pDestVtx[i].vPosition.z *= vScale.z;

		pDestVtx[i].vPosition += *TransCom->Get_Position();
	}

	m_pTileVB->Unlock();
}

