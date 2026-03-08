#include "../Include/PCH.h"
#include "WhiteOut.h"

StageWhiteOut::StageWhiteOut(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
StageWhiteOut::StageWhiteOut(CONST GameObject& _RHS) : GameObject(_RHS) {}
StageWhiteOut::~StageWhiteOut() {}

HRESULT StageWhiteOut::Ready_Effect() {
	if (FAILED(Component_Initialize())) return E_FAIL;
	D3DXCreateSprite(GRPDEV, &m_pDrawSprite);
	m_pTexture = ResourceManager::GetInstance()->Find_Texture(L"ffffff.png");

	SoundManager::GetInstance()->Stop_AllSound();
	SoundManager::GetInstance()->Play_Sound_Once(L"Stage/WhiteOut.wav", CHANNELID::SOUND_BGM02);

	return S_OK;
}

INT  StageWhiteOut::Update_GameObject(CONST FLOAT& _DT) {


	m_fAlpha += _DT * 60.f;

	if (m_fAlpha >= 255.f - 60.f) {
		if (m_fTimer == 0.f) {
			MiniGameScene* pScene = dynamic_cast<MiniGameScene*>(SceneManager::GetInstance()->Get_CurrentScene());
			SoundManager::GetInstance()->Stop_AllSound();
			SoundManager::GetInstance()->Play_Sound_Once(L"Object/MiniGameItemDrop.wav", CHANNELID::SOUND_BGM03);
			if (pScene != nullptr)
				*pScene->Get_EventTrigger() = -1;
		}
		m_fTimer += _DT;
	}
	if (m_fTimer >= 2.6f) {
		SoundManager::GetInstance()->Play_Sound(L"DoCheol/Docheol's area_Start.wav", CHANNELID::SOUND_BGM01, 0.f, FALSE);
		return -1;
	}


	if (m_fAlpha >= 255.f) {
		m_fAlpha = 255.f;
	}

	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return GameObject::Update_GameObject(_DT);
}
VOID StageWhiteOut::LateUpdate_GameObject(CONST FLOAT& _DT) {
	GameObject::LateUpdate_GameObject(_DT);
}
VOID StageWhiteOut::Render_GameObject() {
	
	D3DXMATRIX matScale;
	D3DXMatrixScaling(&matScale, (float)(WINCX / 64), (float)(WINCY / 64), 1.0f);

	m_pDrawSprite->Begin(D3DXSPRITE_ALPHABLEND);

	m_pDrawSprite->SetTransform(&matScale);

	m_pDrawSprite->Draw(m_pTexture, NULL, NULL, 0, D3DCOLOR_ARGB((int)m_fAlpha, 255, 255, 255));

	m_pDrawSprite->End();
}

HRESULT	StageWhiteOut::Component_Initialize() {

	return S_OK;
}
StageWhiteOut* StageWhiteOut::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	StageWhiteOut* WhiteOut = new StageWhiteOut(_GRPDEV);
	if (FAILED(WhiteOut->Ready_Effect())) {
		MSG_BOX("Cannot Create Effect.");
		Safe_Release(WhiteOut);
		return nullptr;
	}
	return WhiteOut;
}
VOID StageWhiteOut::Free() {

	Safe_Release(m_pDrawSprite);
	GameObject::Free();
}
