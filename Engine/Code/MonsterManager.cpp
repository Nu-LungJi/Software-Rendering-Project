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

	if (tInfo.iCount != 5) 
		return tInfo;

	if      (!wcscmp(tInfo.szType, L"Monster"))  tInfo.Type = (uint8_t)MONSTER_SEP::Monster;
	else if (!wcscmp(tInfo.szType, L"Bullet"))   tInfo.Type = (uint8_t)MONSTER_SEP::Bullet;
	else if (!wcscmp(tInfo.szType, L"Effect"))   tInfo.Type = (uint8_t)MONSTER_SEP::Effect;

	if      (!wcscmp(tInfo.szName, L"BlueEvilBat"))                tInfo.name = (uint8_t)MONSTER_TYPE::Bat;
	else if (!wcscmp(tInfo.szName, L"BlueScorpionEvilSoul"))       tInfo.name = (uint8_t)MONSTER_TYPE::ScorpionEvilSoul;
	else if (!wcscmp(tInfo.szName, L"BlueEvilSlime"))              tInfo.name = (uint8_t)MONSTER_TYPE::EvilSlime;
	else if (!wcscmp(tInfo.szName, L"BlueEliteShotGunEvilSoul"))   tInfo.name = (uint8_t)MONSTER_TYPE::ShotGunEvilSoul;
	else if (!wcscmp(tInfo.szName, L"Standard"))                   tInfo.name = (uint8_t)BULLET_TYPE::Standard;
	else if (!wcscmp(tInfo.szName, L"ScorpionBullet"))             tInfo.name = (uint8_t)BULLET_TYPE::ScorpionBullet;
	else if (!wcscmp(tInfo.szName, L"Chain01"))                    tInfo.name = (uint8_t)BULLET_TYPE::Chain01;
	else if (!wcscmp(tInfo.szName, L"Chain02"))                    tInfo.name = (uint8_t)BULLET_TYPE::Chain02;
	else if (!wcscmp(tInfo.szName, L"ChainHead"))                  tInfo.name = (uint8_t)BULLET_TYPE::ChainHead;
	else if (!wcscmp(tInfo.szName, L"Fireball"))					tInfo.name = (uint8_t)BULLET_TYPE::Fireball;
	else if (!wcscmp(tInfo.szName, L"BlueEvilSlimeGroudIceEffect")) tInfo.name = (uint8_t)BULLET_TYPE::GroundIce;

	if      (!wcscmp(tInfo.szState, L" ") || !wcscmp(tInfo.szState, L"")) tInfo.State = 0x00;
	else if (!wcscmp(tInfo.szState, L"Stand"))                     tInfo.State = (uint8_t)MONSTER_ANIM::Stand;
	else if (!wcscmp(tInfo.szState, L"appear"))                    tInfo.State = (uint8_t)MONSTER_ANIM::Appear;
	else if (!wcscmp(tInfo.szState, L"disappear"))                 tInfo.State = (uint8_t)MONSTER_ANIM::Disappear;
	else if (!wcscmp(tInfo.szState, L"Attack"))                    tInfo.State = (uint8_t)MONSTER_ANIM::Attack;
	else if (!wcscmp(tInfo.szState, L"Death"))                     tInfo.State = (uint8_t)MONSTER_ANIM::Death;

	//if      (!_wcsicmp(tInfo.szExtension, L".png")) tInfo.extension = 0x00;
	//else if (!_wcsicmp(tInfo.szExtension, L".dds")) tInfo.extension = 0x01;

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
