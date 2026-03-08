#include "../Include/PCH.h"
#include "StageBlackOut.h"

StageBlackOut::StageBlackOut(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV){}
StageBlackOut::StageBlackOut(CONST GameObject& _RHS) : GameObject(_RHS){}
StageBlackOut::~StageBlackOut() {}

HRESULT StageBlackOut::Ready_Effect(_vec3* vPos, _bool bDocheol) {
	if (FAILED(Component_Initialize())) return E_FAIL;
	D3DXCreateSprite(GRPDEV, &m_pDrawSprite);

	
		Make_TextureList(L"../../Tile/Frame/frame_black_L_to_R", WINCX, WINCY, 255, 46,SCENE_EFFECT::SCENE_STAGE);
		Make_TextureList(L"../../Tile/Frame/Spr_Effect_ChaosSwordBlackHole_Start_",WINCX,WINCY,255,46,SCENE_EFFECT::SCENE_BOSS);
		m_bLastBgm = false;
	m_fFrame = 0;
	m_fBossDelay = 0;
	m_iFrameCnt = 0;
	m_iPlayerSound = 1;
	m_bDocheol = bDocheol;
	m_bStop = false;
	Set_ObjectTag(L"BlackOut");
	m_bRestart = true;
	m_fVolume01 = 0.5f;
	m_fVolume02 = 0.f;
	m_fVolume03 = 0.5f;
	m_bCheonLog = true;
	return S_OK;
}
HRESULT StageBlackOut::Make_TextureList(wstring _FileName, UINT _WIDTH, UINT _HEIGHT, INT _OPACITY, INT iCnt, SCENE_EFFECT eid)
{
	INT FRAME = 0;

	while (++FRAME) {
		if (FRAME == iCnt)
			return E_FAIL;
		LPDIRECT3DTEXTURE9 TEX;
		wstring FileName = _FileName + to_wstring(FRAME) + L".png";

		D3DXCreateTextureFromFileExW(GRPDEV, FileName.c_str(), _WIDTH, _HEIGHT,
			1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &TEX);

		if (TEX == nullptr) break;
		 m_pSprtieList[(int)eid].push_back(TEX);
	}

	return S_OK;
}


INT  StageBlackOut::Update_GameObject(CONST FLOAT& _DT) {
	
	BackGround_Volume(_DT);

	if (!m_bRestart)
	{

		GameObject::Update_GameObject(_DT);

		switch (m_eSceneID)
		{
		case SCENE_EFFECT::SCENE_BOSS:
			m_fFrame += _DT;
			if (m_bStop && TileManager::GetInstance()->Get_Loading())
			{
				++m_iFrameCnt;

				dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Player")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(m_vPos);
				SoundManager::GetInstance()->Stop_AllSound();
				SoundManager::GetInstance()->Play_Sound(L"DoCheol/Docheol's area_Start.wav", CHANNELID::SOUND_BGM01, 0.4f,FALSE);
				m_bStop = false;
			}

			if (m_iFrameCnt == 6)
				TileManager::GetInstance()->Set_EndLoading(false);
			if (m_iFrameCnt == 6 && !TileManager::GetInstance()->Get_Loading())
			{
				m_bStop = true;
				return 1;
			}

			if (m_fFrame > 0.008f)
			{
				m_fFrame = 0;
				++m_iFrameCnt;
			}
			if (m_iFrameCnt > m_pSprtieList[(int)m_eSceneID].size() - 1)
			{
				m_bRestart = true;
				m_bStop = false;
				m_iFrameCnt = 0;
			}
			break;

		case SCENE_EFFECT::SCENE_STAGE:
			m_fFrame += _DT;
			if (m_bStop && TileManager::GetInstance()->Get_Loading())
			{
				if (TileManager::GetInstance()->Get_Stage() == TILE_STAGE::TILE_FIRSTBOSS || TileManager::GetInstance()->Get_Stage() == TILE_STAGE::TILE_DOCHERBOSS || TileManager::GetInstance()->Get_Stage() == TILE_STAGE::TILE_DEFENSE || TileManager::GetInstance()->Get_Stage() == TILE_STAGE::STAGE_ROLARUN || TileManager::GetInstance()->Get_BeforeStage() == TILE_STAGE::STAGE_ROLARUN || TileManager::GetInstance()->Get_BeforeStage() == TILE_STAGE::TILE_DEFENSE)
				{
					m_fBossDelay += _DT;
					if (m_fBossDelay > 1.f)
					{
						++m_iFrameCnt;
						m_fBossDelay = 0;
						m_bStop = false;
					}
					else
						return 1;
				}
				else
				{
					++m_iFrameCnt;
					m_bStop = false;
				}
			}

			if (m_iFrameCnt == 21)
				TileManager::GetInstance()->Set_EndLoading(false);
			if (m_iFrameCnt == 21 && !TileManager::GetInstance()->Get_Loading())
			{
				dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Player")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(m_vPos);
				m_bStop = true;
				return 1;
			}

			if (m_fFrame > 0.008)
			{
				m_fFrame = 0;
				++m_iFrameCnt;
			}
			if (m_iFrameCnt > m_pSprtieList[(int)m_eSceneID].size() - 1)
			{
				m_bRestart = true;
				m_bStop = false;
				m_iFrameCnt = 0;
			}
			break;

		}

	}

	return 0;
}
VOID StageBlackOut::LateUpdate_GameObject(CONST FLOAT& _DT) {
	GameObject::LateUpdate_GameObject(_DT);
}
VOID StageBlackOut::Render_GameObject() {
	
	if (!m_bRestart)
	{
		GRPDEV->SetRenderState(D3DRS_ZENABLE, FALSE);
		GRPDEV->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		m_pDrawSprite->Begin(D3DXSPRITE_ALPHABLEND);

		m_pDrawSprite->Draw(m_pSprtieList[(int)m_eSceneID][m_iFrameCnt], NULL, NULL, 0, D3DCOLOR_ARGB(255, 255, 255, 255));

		m_pDrawSprite->End();
		GRPDEV->SetRenderState(D3DRS_ZENABLE, TRUE);

		GRPDEV->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	}
	
}

HRESULT	StageBlackOut::Component_Initialize() {

	return S_OK;
}
void StageBlackOut::BackGround_Volume(const _float _DT)
{
	
	if (TileManager::GetInstance()->Get_CurrentStage() == TILE_STAGE::TILE_FIRSTBOSS && m_bCheonLog)
	{
		if (m_fVolume02 >= 0)
		{
			
			m_fVolume02 -= (_DT / 8);
		}

		SoundManager::GetInstance()->Set_ChannelGroupVolume(CHANNELID::SOUND_BGM01, m_fVolume02);
		SoundManager::GetInstance()->Set_ChannelGroupVolume(CHANNELID::SOUND_BGM02, m_fVolume02);
		
		if (m_fVolume02 <= 0)
		{
			m_fVolume02 = 0;
			m_bCheonLog = false;
			return;
		}
	}

	if (TileManager::GetInstance()->Get_BeforeStage() == TILE_STAGE::TILE_DEFENSE)
	{
	
		if (m_fVolume02 <= 0.3f)
		{
			m_fVolume02 += _DT / 6;
		}
		else
		{
			return;
			
		}
			
		SoundManager::GetInstance()->Set_ChannelGroupVolume(CHANNELID::SOUND_BGM01, m_fVolume02);
		SoundManager::GetInstance()->Set_ChannelGroupVolume(CHANNELID::SOUND_BGM02, m_fVolume02);
	}

	if (TileManager::GetInstance()->Get_BeforeStage() == TILE_STAGE::STAGE_ROLARUN)
	{
		if (m_fVolume03 <= 0.5f)
		{
			m_fVolume03 += _DT / 6;
		}
		else
			return;

			SoundManager::GetInstance()->Set_ChannelGroupVolume(CHANNELID::SOUND_BGM01, m_fVolume03);
	}
	if (!m_bLastBgm && TileManager::GetInstance()->Get_Stage() == TILE_STAGE::TILE_DOCHERBOSS && TileManager::GetInstance()->Get_BeforeStage() == TILE_STAGE::TILE_DOCHER1)
	{
		if (m_fVolume03 >= 0.f)
		{
			m_fVolume03 -= _DT / 6;
		}
		else

			if (m_fVolume03 <= 0)
				m_fVolume03 = 0.f;
		SoundManager::GetInstance()->Set_ChannelGroupVolume(CHANNELID::SOUND_BGM01, m_fVolume03);

		if (m_fVolume03 <= 0)
		{
			//STOP_ALLSOUND;
			m_bLastBgm = true;
		}
			
	}

}
StageBlackOut* StageBlackOut::Create(LPDIRECT3DDEVICE9 _GRPDEV,_vec3* vPos, _bool bDocheol) {
	StageBlackOut* Blackout = new StageBlackOut(_GRPDEV);
	if (FAILED(Blackout->Ready_Effect(vPos, bDocheol))) {
		MSG_BOX("Cannot Create Effect.");
		Safe_Release(Blackout);
		return nullptr;
	}
	return Blackout;
}
VOID StageBlackOut::Free() {

	Safe_Release(m_pDrawSprite);

	for (_int i = 0; i < (int)SCENE_EFFECT::SCENE_END; ++i)
	{
		for (auto& iter : m_pSprtieList[i])
			Safe_Release(iter);
	
		m_pSprtieList[i].clear();
	}
	
	
	GameObject::Free();
}
