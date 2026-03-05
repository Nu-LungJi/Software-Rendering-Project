#include "../Include/PCH.h"

MiniGameCounter::MiniGameCounter(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV), m_iWave(1),m_bEnd(false), m_fDefense(0.f), m_fTime(0), m_fFrame(0), m_bStopFrame(false), m_pBuffer(nullptr), m_pTransform(nullptr), m_pTileInfo(nullptr) {}
MiniGameCounter::MiniGameCounter(const GameObject& _RHS) : GameObject(_RHS) {}
MiniGameCounter::~MiniGameCounter() {}

HRESULT MiniGameCounter::Ready_GameObject() {

	SoundManager::GetInstance()->Stop_AllSound();
    SoundManager::GetInstance()->Play_Sound(L"Stage/MiniGame_VamPire.mp3", CHANNELID::SOUND_BGM03, 0.3f);

	if (FAILED(Component_Initialize())) return E_FAIL;
	m_iLast = 0;
	m_iCnt = 0;
	m_bEndWave = false;
	m_bAugment = false;
	m_iKeyCnt = 0;
	m_StageCnt[0] = 2;
	m_StageCnt[1] = 4;
	m_StageCnt[2] = 6;
	m_iMaxWave = 3;
	UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"WAVE :", { 423.14f, 10.f }, 30, L"STAGE_NAME", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255));
	UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"1", { 495.52f, 10.f }, 30, L"STAGE_COUNT", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255));

	UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"남은 마리 수 : ", { 766.537f, 10.f }, 30, L"MONSTER_NUMBER", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255));
	UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"0", { 878.10f, 10.f }, 30, L"MONSTER_NAME", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255));
	Make_TextureList(L"../../Resource/Clear/CLEAR_");
	return S_OK;
}
HRESULT MiniGameCounter::Make_TextureList(wstring _FileName)
{
	INT FRAME = 0;

	while (++FRAME) {
		wstring FileName = _FileName + to_wstring(FRAME) + L".png";
		wstring KeyName = _FileName + to_wstring(FRAME);
		
		m_pSprite->Import_Sprite(FileName.c_str(), KeyName.c_str(), WINCX / 6, WINCY /6, 800, 150, TRUE, 255);
		m_vecKeyList.push_back(KeyName);
		if (FRAME == 20)
			return S_OK;
	}


	return S_OK;
}
INT	MiniGameCounter::Update_GameObject(const _float& _DT) {

	GameObject::Update_GameObject(_DT);
	//Imgui();
	if (Get_ObjectDead() == TRUE)
	{
		SoundManager::GetInstance()->Stop_AllSound();
		SoundManager::GetInstance()->Play_Sound(L"Stage/Bgm_Stage1-2_Loop.wav", CHANNELID::SOUND_BGM01, 0.3f);
		SoundManager::GetInstance()->Play_Sound(L"Stage/Ambience_Rain.wav", CHANNELID::SOUND_BGM02, 0.25f);

		return -1;
	}
	
	if (m_StageCnt[m_iCnt] <= 0)
	{
		++m_iWave;
		++m_iCnt;
	}

	if (m_iCnt > 2)
		m_iCnt = 2;

	if (m_StageCnt[m_iCnt] <= 0)
		m_StageCnt[m_iCnt] = 0;

	if (m_iWave >= m_iMaxWave)
	{
		m_iWave = m_iMaxWave;

	}
		
	if (!m_bAugment)
	{
		UIManager::GetInstance()->Find_FontObject(L"STAGE_COUNT")->Set_Text(to_wstring(m_iWave));
		UIManager::GetInstance()->Find_FontObject(L"MONSTER_NAME")->Set_Text(to_wstring(m_StageCnt[m_iCnt]));

	}
	RenderManager::GetInstance()->Add_RenderGroup(RENDERID::RENDER_UI, this);

	return 0;

}

VOID MiniGameCounter::LateUpdate_GameObject(const _float& _DT) {
	GameObject::LateUpdate_GameObject(_DT);
	
	if (m_StageCnt[2] <= 0)
	{
		m_bEnd = true;
		m_fTime += _DT;
		m_fFrame += _DT;
		if (m_fFrame > 0.1f)
		{
			++m_iLast;
			++m_iKeyCnt;
		}
		if (m_iKeyCnt > m_vecKeyList.size() - 1)
			m_iKeyCnt = m_vecKeyList.size() - 1;
		if (m_fTime > 3.f && !m_bAugment)
		{
			UIManager::GetInstance()->Delete_FontObject(UIManager::GetInstance()->Find_FontObject(L"STAGE_NAME"));
			UIManager::GetInstance()->Delete_FontObject(UIManager::GetInstance()->Find_FontObject(L"STAGE_COUNT"));
			UIManager::GetInstance()->Delete_FontObject(UIManager::GetInstance()->Find_FontObject(L"MONSTER_NUMBER"));
			UIManager::GetInstance()->Delete_FontObject(UIManager::GetInstance()->Find_FontObject(L"MONSTER_NAME"));
			Augment* pObj = Augment::Create(GRPDEV);
			pObj->Set_ObjectTag(L"Agument");
			SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(pObj);
			m_bAugment = true;
		}
			

		if (m_fTime > 6.f && m_bEndWave)
		{
			
			_vec3 vPos = { 17.862f, 0.5f, 121.045f };
			dynamic_cast<StageBlackOut*>(EffectManager::GetInstance()->Get_Scene())->Set_Pos(vPos, false, 0,false);
			TileManager::GetInstance()->Set_CurStage(TILE_STAGE::TILE_STAGE4);

			Set_ObjectDead(TRUE);
		}
	}
	

}
VOID MiniGameCounter::Render_GameObject()
{

	if (!m_bAugment)
	{

		GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		GRPDEV->SetRenderState(D3DRS_ZENABLE, FALSE);
		Sprite->Begin(D3DXSPRITE_ALPHABLEND);
		Sprite->Draw(m_pSprite->Get_Texture(m_vecKeyList[m_iKeyCnt])->TEXTURE, NULL, NULL, &m_pSprite->Get_Texture(m_vecKeyList[m_iKeyCnt])->POS, D3DCOLOR_ARGB(255, 255, 255, 255));
		Sprite->End();
		GRPDEV->SetRenderState(D3DRS_ZENABLE, TRUE);


		GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	}

}
HRESULT MiniGameCounter::Component_Initialize() {

	m_pSprite = ADD_COMPONENT_SPRITE;
	D3DXCreateSprite(GRPDEV, &Sprite);
	return S_OK;
}

MiniGameCounter* MiniGameCounter::Create(LPDIRECT3DDEVICE9 _GRPDEV) {

	MiniGameCounter* pMiniGameCounter = new MiniGameCounter(_GRPDEV);

	if (FAILED(pMiniGameCounter->Ready_GameObject())) {
		MSG_BOX("Cannot Create MiniGameCounter.");
		Safe_Release(pMiniGameCounter);
		return nullptr;
	}

	return pMiniGameCounter;
}
VOID MiniGameCounter::Free() {

	Safe_Release(Sprite);
	GameObject::Free();
}